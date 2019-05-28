/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Min Yang Jung
  Created on: 2009-12-07

  (C) Copyright 2009-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsManagerLocal.h>

#include <cisstCommon/cmnThrow.h>
#include <cisstCommon/cmnPath.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstOSAbstraction/osaGetTime.h>
#include <cisstOSAbstraction/osaSocket.h>
#include <cisstOSAbstraction/osaTimeServer.h>
#include <cisstOSAbstraction/osaDynamicLoader.h>

#include <cisstMultiTask/mtsConfig.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsManagerGlobal.h>
#include <cisstMultiTask/mtsTaskContinuous.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsTaskFromCallback.h>
#include <cisstMultiTask/mtsTaskFromSignal.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsManagerComponentClient.h>
#include <cisstMultiTask/mtsManagerComponentServer.h>
#include <cisstMultiTask/mtsLODMultiplexerStreambuf.h>

#if CISST_MTS_HAS_ICE
#include "mtsComponentProxy.h"
#include "mtsManagerProxyClient.h"
#include "mtsManagerProxyServer.h"
#endif

// Time server used by all tasks
osaTimeServer TimeServer;
bool TimeServerOriginSet = false;

mtsManagerLocal * mtsManagerLocal::Instance = 0;
mtsManagerLocal * mtsManagerLocal::InstanceReconfiguration = 0;
osaMutex mtsManagerLocal::ConfigurationChange;

bool mtsManagerLocal::UnitTestEnabled = false;
bool mtsManagerLocal::UnitTestNetworkProxyEnabled = false;

const std::string mtsManagerLocal::ProcessNameOfLCMDefault = "LCM";
const std::string mtsManagerLocal::ProcessNameOfLCMWithGCM = "GCM";

// System-wide logging: Define logger-related variables here so that
// the logger doesn't have to call GetInstance() everytime it receives
// log messages. {{
mtsLODMultiplexerStreambuf * SystemLogMultiplexer = 0;
bool           LogForwardEnabled = false;
bool           LogDisabled = false;
osaMutex       LogMutex;

typedef std::list<mtsLogMessage> LogQueueType;
LogQueueType   LogQueue;

std::string    ThisProcessName;
// }}

mtsManagerLocal::mtsManagerLocal(void) : ComponentMap("ComponentMap")
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Local component manager: STANDALONE mode" << std::endl;
    InitializeLocal();
}

#if CISST_MTS_HAS_ICE
mtsManagerLocal::mtsManagerLocal(mtsManagerGlobal & globalComponentManager) : ComponentMap("ComponentMap")
#else
mtsManagerLocal::mtsManagerLocal(mtsManagerGlobal & CMN_UNUSED(globalComponentManager)) : ComponentMap("ComponentMap")
#endif
{
#if CISST_MTS_HAS_ICE
    Initialize();

    ProcessName = ProcessNameOfLCMWithGCM;
    ThisProcessName = ProcessName;

    CMN_LOG_CLASS_INIT_VERBOSE << "Local component manager: NETWORK mode with GCM" << std::endl;

    // Register process object to the global component manager
    if (!globalComponentManager.AddProcessObject(this)) {
        cmnThrow(std::runtime_error("Failed to register process object to Global Component Manager"));
    }

    ManagerGlobal = &globalComponentManager;

    Configuration = LCM_CONFIG_NETWORKED_WITH_GCM;

    SetGCMConnected(true); // Always true because LCM and GCM run in the same process in this configuration
#else
    CMN_LOG_CLASS_INIT_ERROR << "Requested NETWORK mode with GCM, but CISST_MTS_HAS_ICE is false" << std::endl;
    InitializeLocal();
#endif
}

mtsManagerLocal::mtsManagerLocal(const std::string & globalComponentManagerIP,
                                 const std::string & thisProcessName,
                                 const std::string & thisProcessIP)
                                 : ComponentMap("ComponentMap"),
                                   ProcessName(thisProcessName),
                                   GlobalComponentManagerIP(globalComponentManagerIP),
                                   ProcessIP(thisProcessIP)
{
#if CISST_MTS_HAS_ICE
    Initialize();
    ThisProcessName = ProcessName;

    // Create network proxies
    if (!ConnectToGlobalComponentManager()) {
        std::string s("Failed to connect to Global Component Manager: ");
        s += globalComponentManagerIP;
        cmnThrow(std::runtime_error(s));
    }

    // Give proxies some time to start up
#if !IMPROVE_ICE_THREADING
    if (this->GetConfiguration() != LCM_CONFIG_STANDALONE) {
        osaSleep(2.0 * cmn_s); // MJ TEMP: Better way to handle this without sleep??
    }
#endif

    SetGCMConnected(true);

    // Set this machine's IP
    SetIPAddress();

    // Set running configuration of this LCM
    Configuration = LCM_CONFIG_NETWORKED;
#else
    CMN_LOG_CLASS_INIT_ERROR << "mtsManagerLocal(" << globalComponentManagerIP << ", "
                               << thisProcessName << ", " << thisProcessIP
                               << ") called, but CISST_MTS_HAS_ICE is false" << std::endl;
    InitializeLocal();
#endif
}

bool mtsManagerLocal::ConnectToGlobalComponentManager(void)
{
#if CISST_MTS_HAS_ICE
    // If process ip is not specified (""), the first ip address detected is used as primary ip
    if (ProcessIP == "") {
        std::vector<std::string> ipAddresses;
        int ret = osaSocket::GetLocalhostIP(ipAddresses);
        if (ret == 0) {
             CMN_LOG_CLASS_INIT_ERROR << "Failed to get local host ip address.  Check network interface status." << std::endl;
             return false;
        }
        ProcessIP = ipAddresses[0];

        CMN_LOG_CLASS_INIT_VERBOSE << "Ip of this process was not specified. First ip detected ("
            << ProcessIP << ") will be used instead" << std::endl;
    }

    // If process name is not specified (""), the ip address is used as this process name.
    if (ProcessName == "") {
        ProcessName = ProcessIP;

        CMN_LOG_CLASS_INIT_VERBOSE << "Name of this process was not specified. Ip address ("
            << ProcessName << ") will be used instead" << std::endl;
    }

    // Generate an endpoint string to connect to the global component manager
    std::stringstream ss;
    ss << mtsManagerProxyServer::GetManagerCommunicatorID()
       << ":default -h " << GlobalComponentManagerIP
       << " -p " << mtsManagerProxyServer::GetGCMPortNumberAsString();

    // Create a proxy for the GCM
    mtsManagerProxyClient * globalComponentManagerProxy = new mtsManagerProxyClient(ss.str());

    // Run Ice proxy and connect to the global component manager
    if (!globalComponentManagerProxy->StartProxy(this)) {
        std::string s("Failed to connect to Global Component Manager: ");
        s += GlobalComponentManagerIP;
        CMN_LOG_CLASS_INIT_ERROR << s << std::endl;
        delete globalComponentManagerProxy;
        return false;
    }

    // Wait for proxies to be in active state (PROXY_STATE_ACTIVE)
#if !IMPROVE_ICE_THREADING
    if (this->GetConfiguration() != LCM_CONFIG_STANDALONE) {
        osaSleep(1.0 * cmn_s); // MJ TEMP: Better way to handle this without sleep??
    }
#endif

    // Register process name to the global component manager.
    if (!globalComponentManagerProxy->AddProcess(ProcessName)) {
        CMN_LOG_CLASS_INIT_ERROR << "Failed to register process name to Global Component Manager" << std::endl;
        delete globalComponentManagerProxy;
        return false;
    }

    // In network mode, a process object doesn't need to be registered
    // to the global component manager.
    ManagerGlobal = globalComponentManagerProxy;

    CMN_LOG_CLASS_INIT_VERBOSE << "Local component manager     : NETWORK mode" << std::endl
                               << "Global component manager IP : " << GlobalComponentManagerIP << std::endl
                               << "This process name           : " << ProcessName << std::endl
                               << "This process IP             : " << ProcessIP << std::endl;

#else
    CMN_LOG_CLASS_INIT_WARNING << "ConnectToGlobalComponentManager called when CISST_MTS_HAS_ICE is false"
                               << std::endl;
#endif
    return true;
}

mtsManagerLocal::~mtsManagerLocal()
{
    /*
    // If ManagerGlobal is not NULL, it means Cleanup() has not been called
    // before. Thus, it needs to be called here for safe and clean termination.
    if (ManagerGlobal) {
        //if (Configuration == LCM_CONFIG_NETWORKED) {
            Cleanup();
        //}
    }
    */
    Cleanup();
}

void mtsManagerLocal::Initialize(void)
{
    __os_init();
    ComponentMap.SetOwner(*this);

    InstanceReconfiguration = 0;
    ManagerComponent.Client = 0;
    ManagerComponent.Server = 0;

    CurrentMainTask = 0;

    SetGCMConnected(false);

    TimeServer.SetTimeOrigin();
    TimeServerOriginSet = true;

    SetupSystemLogger();
}

void mtsManagerLocal::InitializeLocal(void)
{
    Initialize();

    // In standalone mode, process name is set as ProcessNameOfLCMDefault by
    // default since there is only one instance of local task manager.
    ProcessName = ProcessNameOfLCMDefault;
    ThisProcessName = ProcessName;

    // In standalone mode, the global component manager is an instance of
    // mtsManagerGlobal that runs in the same process in which this local
    // component manager runs.
    mtsManagerGlobal * globalManager = new mtsManagerGlobal;

    // Register process name to the global component manager
    if (!globalManager->AddProcess(ProcessName)) {
        cmnThrow(std::runtime_error("Failed to register process name to Global Component Manager"));
    }

    // Register process object to the global component manager
    if (!globalManager->AddProcessObject(this)) {
        cmnThrow(std::runtime_error("Failed to register process object to Global Component Manager"));
    }

    ManagerGlobal = globalManager;

    Configuration = LCM_CONFIG_STANDALONE;

    SetGCMConnected(true); // Always true in case of standalone configuration
}

void mtsManagerLocal::Cleanup(void)
{
    if (LogThreadFinishWaiting) return;

    LogThreadFinishWaiting = true;
    LogTheadFinished.Wait();

    if (ManagerGlobal) {
        delete ManagerGlobal;
        ManagerGlobal = 0;
    }

    if (ManagerComponent.Client) {
        ManagerComponent.Client->Kill();
        ManagerComponent.Client->WaitForState(mtsComponentState::FINISHED, 30.0 * cmn_s);
        delete ManagerComponent.Client;
        ManagerComponent.Client = 0;
    }

    if (ManagerComponent.Server) {
        ManagerComponent.Server->Kill();
        ManagerComponent.Server->WaitForState(mtsComponentState::FINISHED, 30.0 * cmn_s);
        delete ManagerComponent.Server;
        ManagerComponent.Server = 0;
    }

    if (SystemLogMultiplexer) {
        cmnLogger::GetMultiplexer()->RemoveMultiplexer(SystemLogMultiplexer);
        SystemLogMultiplexer->RemoveAllChannels();
        delete SystemLogMultiplexer;
        SystemLogMultiplexer = 0;
    }

    __os_exit();
}

const osaTimeServer & mtsManagerLocal::GetTimeServer(void) const
{
    return TimeServer;
}

void mtsManagerLocal::SetupSystemLogger(void)
{
    LogThreadFinishWaiting = false;
    LogThead.Create<mtsManagerLocal, void *>(this, &mtsManagerLocal::LogDispatchThread);

    SystemLogMultiplexer = new mtsLODMultiplexerStreambuf();
    if (!cmnLogger::GetMultiplexer()->AddMultiplexer(SystemLogMultiplexer)) {
        CMN_LOG_INIT_ERROR << "Failed to add mts system logger" << std::endl;
    }
}

bool mtsManagerLocal::IsLogAllowed(void) {
    return !LogDisabled;
}

bool mtsManagerLocal::IsLogForwardingEnabled(void) {
    return LogForwardEnabled;
}

void mtsManagerLocal::SetLogForwarding(bool activate) {
    LogForwardEnabled = activate;
}

void mtsManagerLocal::GetLogForwardingState(bool & state) {
    state = IsLogForwardingEnabled();
}

bool mtsManagerLocal::GetLogForwardingState(void) {
    return IsLogForwardingEnabled();
}

bool mtsManagerLocal::MCCReadyForLogForwarding(void) const
{
    if (!Instance) return false;

    if (!Instance->ManagerComponent.Client ||
        !Instance->ManagerComponent.Client->CanForwardLog())
    {
        return false;
    }

    return true;
}

void mtsManagerLocal::LogDispatcher(const char * str, int len)
{
    if (!LogForwardEnabled) return;

    bool deadlockAvoidance = false;
    if (Instance) {
        if (LogMutex.IsLocker() && Instance->MCCReadyForLogForwarding()) {
            deadlockAvoidance = true;
        }
    }

    if (!deadlockAvoidance) {
        LogMutex.Lock();
    }

    if (len == 1 && str[0] == '\n') {
        if (!deadlockAvoidance) {
            LogMutex.Unlock();
        }
        return;
    }

    // MJ TODO: Deal with cases that len > 1000
    mtsLogMessage log(str, len);
    // Timestamping (as early as possible)
    if (TimeServerOriginSet) {
        log.SetTimestamp(TimeServer.GetAbsoluteTimeInSeconds());
        log.SetValid(true);
    } else {
        log.SetTimestamp(0);
        log.SetValid(false);
    }
    log.ProcessName = ThisProcessName;

    if (!deadlockAvoidance) {
        // Queue log message and return immediately
        LogQueue.push_back(log);
    } else {
        // If current thread locked this mutex earlier, forward the log immediately
        // to avoid deadlock.  Note that all validity checks are already done
        // in the log dispatch thread in this case.
        if (Instance->MCCReadyForLogForwarding()) {
            Instance->ManagerComponent.Client->ForwardLog(log);
        }
    }

    if (!deadlockAvoidance) {
        LogMutex.Unlock();
    }
}

void * mtsManagerLocal::LogDispatchThread(void * CMN_UNUSED(arg))
{
    int count = 0;

    while (!LogThreadFinishWaiting) {
        if (LogQueue.size() == 0) {
            osaSleep(1.0 * cmn_ms);
            continue;
        }

        // Wait for MCC to be ready (activated and connected) before starting log fowarding
        if (!MCCReadyForLogForwarding()) {
            osaSleep(100.0 * cmn_ms);
            continue;
        }

        LogMutex.Lock();
        count = 0;
        for (LogQueueType::iterator it = LogQueue.begin();
             it != LogQueue.end();
             ++count)
        {
            if (Instance->ManagerComponent.Client->ForwardLog(*it)) {
                ++it;
                LogQueue.pop_front(); // FIFO
            }
            // MJ: after 30 log messages forwarded, give other threads a chance to queue
            // logs by releasing the lock (30 is arbitrary)
            if (count == 30)
                break;
        }
        LogMutex.Unlock();
    }

    LogTheadFinished.Raise();

    return 0;
}

mtsManagerLocal * mtsManagerLocal::GetSafeInstance(void)
{
    mtsManagerLocal * instance = mtsManagerLocal::InstanceReconfiguration;
    if (!instance) {
        instance = mtsManagerLocal::GetInstance();
    }

    return instance;
}

mtsManagerLocal * mtsManagerLocal::GetInstance(void)
{
    if (!Instance) {
        Instance = new mtsManagerLocal;
        Instance->MainThreadId = osaGetCurrentThreadId();

        // Create manager components
        if (!Instance->CreateManagerComponents()) {
            CMN_LOG_INIT_ERROR << "class mtsManagerLocal: GetInstance: Failed to add internal manager components" << std::endl;
        }
    }

    return Instance;
}

mtsManagerLocal * mtsManagerLocal::GetInstance(const std::string & globalComponentManagerIP,
                                               const std::string & thisProcessName,
                                               const std::string & thisProcessIP)
{
    if (!Instance) {
        // If no argument is specified, standalone configuration is set by default.
        if (globalComponentManagerIP == "" && thisProcessName == "" && thisProcessIP == "") {
            Instance = new mtsManagerLocal;
            CMN_LOG_INIT_WARNING << "WARNING: Inter-process communication support is disabled" << std::endl;
        } else {
            Instance = new mtsManagerLocal(globalComponentManagerIP, thisProcessName, thisProcessIP);
        }
        Instance->MainThreadId = osaGetCurrentThreadId();

        // Create manager components
        if (!Instance->CreateManagerComponents()) {
            CMN_LOG_INIT_ERROR << "GetInstance: Failed to add internal manager components" << std::endl;
        }

        return Instance;
    }

    if (globalComponentManagerIP == "" && thisProcessName == "" && thisProcessIP == "") {
        return Instance;
    }

#if CISST_MTS_HAS_ICE
    // If this local component manager has been previously configured as standalone
    // mode and GetInstance() is called again with proper arguments, the manager is
    // reconfigured as networked mode.  For thread-safe transition of configuration
    // from standalone mode to networked mode, a caller thread is blocked until
    // the transition process finishes.

    // Allow configuration change from standalone mode to networked mode only
    if (dynamic_cast<mtsManagerGlobal*>(Instance->ManagerGlobal) == 0) {
        CMN_LOG_INIT_WARNING << "Class mtsManagerLocal: Reconfiguration: Inter-process communication has already been set: skip reconfiguration" << std::endl;
        return Instance;
    }

    CMN_LOG_INIT_VERBOSE << "Class mtsManagerLocal: Enable network support for local component manager (\"" << Instance->ProcessName << "\")" << std::endl
                         << ": with global component manager IP : " << globalComponentManagerIP << std::endl
                         << ": with this process name           : " << thisProcessName << std::endl
                         << ": with this process IP             : " << (thisProcessIP == "" ? "\"\"" : thisProcessIP) << std::endl;

    mtsManagerLocal::ConfigurationChange.Lock();

    // Remember the current global component manager which was created locally
    // (because local component manager was configured as standalone mode) and
    // contains all information about currently active components.
    // Note that this->ManagerGlobal is replaced with a new instance of GCM
    // proxy when the constructor of mtsManagerLocal() is called with proper
    // arguments.
    mtsManagerGlobalInterface * currentGCM = Instance->ManagerGlobal;

    // Create a new singleton object of LCM with network support
    mtsManagerLocal * newInstance = 0;
    try {
        newInstance = new mtsManagerLocal(globalComponentManagerIP, thisProcessName, thisProcessIP);
    } catch (const std::exception& ex) {
        CMN_LOG_INIT_ERROR << "Class mtsManagerLocal: Reconfiguration: failed to enable network support: " << ex.what() << std::endl;
        if (newInstance) {
            delete newInstance;
        }
        mtsManagerLocal::ConfigurationChange.Unlock();
        return Instance;
    }

    // Create internal manager components
    InstanceReconfiguration = newInstance;
    if (!newInstance->CreateManagerComponents()) {
        CMN_LOG_INIT_ERROR << "class mtsManagerLocal: GetInstance: failed to create MCC/MCS" << std::endl;

        // Clean up new LCM object
        delete newInstance;
        mtsManagerLocal::ConfigurationChange.Unlock();

        // Keep using current LCM
        return Instance;
    }
    InstanceReconfiguration = 0;

    //
    // Transfer all internal data--i.e., components and connections--from current
    // LCM to new LCM.
    //
    {   // Transfer component information
        std::string componentName;
        ComponentMapType::const_iterator it = Instance->ComponentMap.begin();
        const ComponentMapType::const_iterator itEnd = Instance->ComponentMap.end();
        for (; it != itEnd; ++it) {
            componentName = it->second->GetName();
            // Do not transfer manager components (both client and server).  They will be created
            // again by CreateManagerComponents() later.
            if (mtsManagerComponentBase::IsManagerComponentServer(componentName) ||
                mtsManagerComponentBase::IsManagerComponentClient(componentName))
            {
                continue;
            }
            if (!newInstance->AddComponent(it->second)) {
                CMN_LOG_INIT_ERROR << "Class mtsManagerLocal: Reconfiguration: failed to trasnfer component: "
                                   << componentName << std::endl;

                // Clean up new LCM object
                delete newInstance;
                mtsManagerLocal::ConfigurationChange.Unlock();

                // Keep using current LCM
                return Instance;
            } else {
                CMN_LOG_INIT_VERBOSE << "Class mtsManagerLocal: Reconfiguration: successfully transferred component: "
                                     << componentName << std::endl;
            }
        }
    }

    {   // Transfer connection information

        // Get current connection information
        std::vector<mtsDescriptionConnection> list;
        currentGCM->GetListOfConnections(list);

        // Register all the connections established to the new GCM
        mtsManagerGlobalInterface * newGCM = newInstance->ManagerGlobal;

        bool managerComponentInvolvedConnection;
        ConnectionIDType connectionId;
        std::vector<mtsDescriptionConnection>::const_iterator it = list.begin();
        const std::vector<mtsDescriptionConnection>::const_iterator itEnd = list.end();
        for (; it != itEnd; ++it) {
            // Do not trasnfer connections between manager components. They will be re-established
            // again by CreateManagerComponents() later.
            managerComponentInvolvedConnection = false;

#define CHECK_INTERNAL_CONNECTION(_classType, _interfaceType)\
            if (!managerComponentInvolvedConnection) \
                managerComponentInvolvedConnection |= (it->Client.InterfaceName == _classType::InterfaceNames::_interfaceType##Required);\
            if (!managerComponentInvolvedConnection)\
                managerComponentInvolvedConnection |= (it->Server.InterfaceName == _classType::InterfaceNames::_interfaceType##Provided);
            // Check InterfaceGCM
            CHECK_INTERNAL_CONNECTION(mtsManagerComponentBase, InterfaceGCM);
            // Check InterfaceLCM
            CHECK_INTERNAL_CONNECTION(mtsManagerComponentBase, InterfaceLCM);
            // Check InterfaceComponent
            CHECK_INTERNAL_CONNECTION(mtsManagerComponentBase, InterfaceComponent);
            // Check InterfaceInternal
            CHECK_INTERNAL_CONNECTION(mtsManagerComponentBase, InterfaceInternal);
#undef CHECK_INTERNAL_CONNECTION

            // If any internal interface is involved in this connection, don't transfer it to
            // the new LCM.
            if (managerComponentInvolvedConnection) {
                continue;
            }

            connectionId = newGCM->Connect(thisProcessName,
                thisProcessName, it->Client.ComponentName, it->Client.InterfaceName,
                thisProcessName, it->Server.ComponentName, it->Server.InterfaceName);
            if (connectionId == InvalidConnectionID) {
                CMN_LOG_INIT_ERROR << "Class mtsManagerLocal: Reconfiguration: failed to transfer previous connection: "
                    << mtsManagerGlobal::GetInterfaceUID(thisProcessName, it->Client.ComponentName, it->Client.InterfaceName)
                    << "-"
                    << mtsManagerGlobal::GetInterfaceUID(thisProcessName, it->Server.ComponentName, it->Server.InterfaceName)
                    << std::endl;
            } else {
                // Notify the GCM of successful local connection (although nothing actually needs to happen).
                if (!newGCM->ConnectConfirm(connectionId)) {
                    CMN_LOG_INIT_ERROR << "Class mtsManagerLocal: Reconfiguration: failed to notify GCM of connection: "
                                       << it->Client.ComponentName << ":" << it->Client.InterfaceName << "-"
                                       << it->Server.ComponentName << ":" << it->Server.InterfaceName << std::endl;

                    if (!newInstance->Disconnect(it->Client.ComponentName, it->Client.InterfaceName,
                                                 it->Server.ComponentName, it->Server.InterfaceName))
                    {
                        CMN_LOG_INIT_ERROR << "Class mtsManagerLocal: Reconfiguration: clean up error: disconnection failed: "
                                           << it->Client.ComponentName << ":" << it->Client.InterfaceName << "-"
                                           << it->Server.ComponentName << ":" << it->Server.InterfaceName << std::endl;
                    }
                }
                CMN_LOG_INIT_VERBOSE << "Class mtsManagerLocal: Reconfiguration: Successfully transferred previous connection: "
                    << mtsManagerGlobal::GetInterfaceUID(thisProcessName, it->Client.ComponentName, it->Client.InterfaceName)
                    << "-"
                    << mtsManagerGlobal::GetInterfaceUID(thisProcessName, it->Server.ComponentName, it->Server.InterfaceName)
                    << std::endl;
            }
        }
    }

    // Remove previous LCM instance
    delete Instance;

    // Replace singleton object
    Instance = newInstance;

    mtsManagerLocal::ConfigurationChange.Unlock();
#else
    CMN_LOG_INIT_ERROR << "GetInstance(" << globalComponentManagerIP << ", "
                       << thisProcessName << ", " << thisProcessIP
                       << ") called, but network support is disabled" << std::endl;
#endif

    return Instance;
}

#if CISST_MTS_HAS_ICE
mtsManagerLocal * mtsManagerLocal::GetInstance(mtsManagerGlobal & globalComponentManager)
#else
mtsManagerLocal * mtsManagerLocal::GetInstance(mtsManagerGlobal & CMN_UNUSED(globalComponentManager))
#endif
{
#if CISST_MTS_HAS_ICE
    if (!Instance) {
        Instance = new mtsManagerLocal(globalComponentManager);
        Instance->MainThreadId = osaGetCurrentThreadId();

        // Create manager components
        if (!Instance->CreateManagerComponents()) {
            CMN_LOG_INIT_ERROR << "class mtsManagerLocal: GetInstance: Failed to add internal manager components" << std::endl;
        }
    }
    return Instance;
#else
    CMN_LOG_INIT_ERROR << "GetInstance(GCM) called when CISST_MTS_HAS_ICE is false" << std::endl;
    return GetInstance();
#endif
}

bool mtsManagerLocal::AddManagerComponent(const std::string & processName, const bool isServer)
{
    // Create manager component client
    if (!isServer) {
        const std::string managerComponentName = mtsManagerComponentBase::GetNameOfManagerComponentClientFor(processName);

        mtsManagerComponentClient * managerComponentClient = new mtsManagerComponentClient(managerComponentName);
        CMN_LOG_CLASS_INIT_VERBOSE << "AddManagerComponent: MCC is created: " << managerComponentClient->GetName() << std::endl;

        if (AddComponent(managerComponentClient)) {
            ManagerComponent.Client = managerComponentClient;
            CMN_LOG_CLASS_INIT_VERBOSE << "AddManagerComponent: MCC is added: " << managerComponentClient->GetName() << std::endl;
        } else {
            CMN_LOG_CLASS_INIT_ERROR << "AddManagerComponent: Failed to add MCC" << std::endl;
            return false;
        }
    }
    // Create manager component server
    else {
        mtsManagerGlobal * gcm = dynamic_cast<mtsManagerGlobal *>(ManagerGlobal);
        if (!gcm) {
            CMN_LOG_CLASS_INIT_ERROR << "AddManagerComponent: Cannot create manager component server: invalid type of Global Component Manager" << std::endl;
            return false;
        }
        mtsManagerComponentServer * managerComponentServer = new mtsManagerComponentServer(gcm);
        gcm->SetMCS(managerComponentServer);

        CMN_LOG_CLASS_INIT_VERBOSE << "AddManagerComponent: MCS is created: " << managerComponentServer->GetName() << std::endl;

        if (AddComponent(managerComponentServer)) {
            ManagerComponent.Server = managerComponentServer;
            CMN_LOG_CLASS_INIT_VERBOSE << "AddManagerComponent: MCS is added: " << managerComponentServer->GetName() << std::endl;
        } else {
            CMN_LOG_CLASS_INIT_ERROR << "AddManagerComponent: Failed to add MCS" << std::endl;
            return false;
        }
    }

    return true;
}


bool mtsManagerLocal::ConnectManagerComponentClientToServer(void)
{
    switch (Configuration) {
        case LCM_CONFIG_STANDALONE:
            // Check if both manager component client and server have been created
            if (!ManagerComponent.Client) {
                CMN_LOG_CLASS_INIT_ERROR << "ConnectManagerComponentClientToServer: MCC (standalone) is not initialized" << std::endl;
                return false;
            }
            if (!ManagerComponent.Server) {
                CMN_LOG_CLASS_INIT_ERROR << "ConnectManagerComponentClientToServer: MCS (standalone) is not initialized" << std::endl;
                return false;
            }
            if (!Connect(ManagerComponent.Client->GetName(),
                         mtsManagerComponentBase::GetNameOfInterfaceLCMRequired(),
                         mtsManagerComponentBase::GetNameOfManagerComponentServer(),
                         mtsManagerComponentBase::GetNameOfInterfaceGCMProvided())
                )
            {
                CMN_LOG_CLASS_INIT_ERROR << "ConnectManagerComponentClientToServer: failed to connect: "
                                         << ManagerComponent.Client->GetName() << ":" << mtsManagerComponentBase::GetNameOfInterfaceLCMRequired()
                                         << " - "
                                         << mtsManagerComponentBase::GetNameOfManagerComponentServer() << ":" << mtsManagerComponentBase::GetNameOfInterfaceGCMProvided()
                                         << std::endl;
                return false;
            }
            break;
        case LCM_CONFIG_NETWORKED_WITH_GCM:
            // Check if manager component server has been created
            if (!ManagerComponent.Server) {
                CMN_LOG_CLASS_INIT_ERROR << "Manager component server (networked) is not initialized" << std::endl;
                return false;
            }

            // Connection between InterfaceGCM's required interface and
            // InterfaceLCM's provided interface is not established here
            // because InterfaceGCM's required interface needs to be dynamically
            // created when a manager component client connects to the manager
            // component server.

            // NOTE: no break statement here so that we fall through to the next block of code
            // to connect the MCC to the MCS in the GCM process.

        case LCM_CONFIG_NETWORKED:
            // Check if manager component client has been created
            if (!ManagerComponent.Client) {
                CMN_LOG_CLASS_INIT_ERROR << "ConnectManagerComponentClientToServer: manager component client (networked) is not initialized" << std::endl;
                return false;
            }
            if (!Connect(this->ProcessName,
                         ManagerComponent.Client->GetName(),
                         mtsManagerComponentBase::GetNameOfInterfaceLCMRequired(),
                         ProcessNameOfLCMWithGCM,
                         mtsManagerComponentBase::GetNameOfManagerComponentServer(),
                         mtsManagerComponentBase::GetNameOfInterfaceGCMProvided()))
            {
                CMN_LOG_CLASS_INIT_ERROR << "ConnectManagerComponentClientToServer: failed to connect: "
                                         << mtsManagerGlobal::GetInterfaceUID(this->ProcessName,
                                                                              ManagerComponent.Client->GetName(),
                                                                              mtsManagerComponentBase::GetNameOfInterfaceLCMRequired())
                                         << " - "
                                         << mtsManagerGlobal::GetInterfaceUID(ProcessNameOfLCMWithGCM,
                                                                              mtsManagerComponentBase::GetNameOfManagerComponentServer(),
                                                                              mtsManagerComponentBase::GetNameOfInterfaceGCMProvided())
                                         << std::endl;
                return false;
            }
            break;

    }

    return true;
}


bool mtsManagerLocal::ConnectToManagerComponentClient(const std::string & componentName)
{
    mtsManagerComponentClient * managerComponent = ManagerComponent.Client;
    if (!ManagerComponent.Client) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectToManagerComponentClient: MCC is not created" << std::endl;
        return false;
    }

    mtsComponent * component = GetComponent(componentName);
    if (!component) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectToManagerComponentClient: no component found with name of "
            << "\"" << componentName << "\"" << std::endl;
        return false;
    }

    // Connect InterfaceComponent's required interface to InterfaceInternal's
    // provided interface of the connecting component.
    const std::string nameOfinterfaceComponentRequired
        = mtsManagerComponentBase::GetNameOfInterfaceComponentRequiredFor(componentName);
    if (!Connect(managerComponent->GetName(), nameOfinterfaceComponentRequired,
                 componentName, mtsManagerComponentBase::GetNameOfInterfaceInternalProvided())) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectToManagerComponentClient: failed to connect: "
                                 << managerComponent->GetName() << ":" << nameOfinterfaceComponentRequired
                                 << " - "
                                 << componentName << ":" << mtsManagerComponentBase::GetNameOfInterfaceInternalProvided()
                                 << std::endl;
        return false;
    }

    // If a component has support for the dynamic component control services,
    // connect InterfaceInternal's required interface to InterfaceComponent's
    // provided interface.
    if (component->GetInterfaceRequired(mtsManagerComponentBase::GetNameOfInterfaceInternalRequired())) {
        if (!Connect(component->GetName(), mtsManagerComponentBase::GetNameOfInterfaceInternalRequired(),
                     managerComponent->GetName(), mtsManagerComponentBase::GetNameOfInterfaceComponentProvided())) {
            CMN_LOG_CLASS_INIT_ERROR << "ConnectToManagerComponentClient: failed to connect: "
                                     << component->GetName() << ":" << mtsManagerComponentBase::GetNameOfInterfaceInternalRequired()
                                     << " - "
                                     << managerComponent->GetName() << ":" << mtsManagerComponentBase::GetNameOfInterfaceComponentProvided()
                                     << std::endl;
            return false;
        }
    }

    return true;
}


mtsComponent * mtsManagerLocal::CreateComponentDynamically(const std::string & className, const std::string & componentName,
                                                           const std::string & constructorArgSerialized)
{
    cmnGenericObject * baseObject = 0;
    mtsComponent * newComponent = 0;
    const cmnClassServicesBase * services = cmnClassRegister::FindClassServices(className);
    if (!services) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateComponentDynamically: unable to create component of type \""
                                 << className << "\" -- no services (make sure the macros CMN_DECLARE_SERVICES and CMN_IMPLEMENT_SERVICES have been used)" << std::endl;
        return 0;
    }
    bool isComponent = services->IsDerivedFrom<mtsComponent>();
    const cmnClassServicesBase *argServices = services->GetConstructorArgServices();
    if (services->OneArgConstructorAvailable() && argServices) {
        if (!isComponent) {
            CMN_LOG_CLASS_INIT_WARNING << "Class " << className << " has one arg constructor, "
                                       << "but class services does not show inheritance from mtsComponent " << std::endl;
        }
        // We can create the object using the "one argument" constructor.  This includes the case where
        // the "one argument" constructor is just an std::string (including the combination of default
        // constructor and SetName method).
        cmnGenericObject *tempArg = 0;
        if (!constructorArgSerialized.empty()) {
            // Case 1: If the serialized constructor arg is not empty, then we just deserialize it and call
            //         CreateWithArg.  We could check if the arg is the correct type, but CreateWithArg will
            //         do it anyway.
            std::stringstream buffer(constructorArgSerialized);
            cmnDeSerializer deserializer(buffer);
            try {
                tempArg = dynamic_cast<cmnGenericObject *>(deserializer.DeSerialize());
            } catch (std::exception &e) {
                CMN_LOG_CLASS_INIT_ERROR << "CreateComponentDynamically: failed to deserialize constructor arg for class \""
                                         << className << "\", error = " << e.what() << std::endl;
                return 0;
            }

            baseObject = services->CreateWithArg(*tempArg);
            delete tempArg;
        }
        else {
            // Case 2: If the serialized constructor arg is empty, then we just have the componentName.
            //         There are actually 2 sub-cases (see below)
            mtsGenericObjectProxyRef<std::string> tempRef(componentName);
            if (argServices == mtsStdString::ClassServices())
                // Case 2a: We just have a string (component name)
                baseObject = services->CreateWithArg(tempRef);
            else {
                // Case 2b: The componentName actually contains the streamed constructor arg (i.e., created
                //          with ToStreamRaw, rather than with SerializeRaw).
                tempArg = argServices->Create();
                if (tempArg) {
                    std::stringstream ss;
                    tempRef.ToStreamRaw(ss);
                    if (!tempArg->FromStreamRaw(ss)) {
                        CMN_LOG_CLASS_INIT_ERROR << "CreateComponentDynamically: Could not parse \""
                                                 << componentName << "\" for constructor of "
                                                 << className << std::endl;
                    }
                    else {
                        baseObject = services->CreateWithArg(*tempArg);
                    }
                    delete tempArg;
                }
                else
                    CMN_LOG_CLASS_INIT_ERROR << "Could not create constructor argument for " << className << std::endl;
            }
        }
        if (baseObject) {
            // If we were able to create an object, dynamic cast it to an mtsComponent so that we can return it.
            newComponent = dynamic_cast<mtsComponent *>(baseObject);
            if (newComponent) {
                CMN_LOG_CLASS_INIT_VERBOSE << "CreateComponentDynamically: successfully created new component: "
                               << "\"" << newComponent->GetName() << "\" of type \""
                                           << className << "\" with arg " << argServices->GetName() << std::endl;

                return newComponent;
            }
            else
                CMN_LOG_CLASS_INIT_ERROR << "CreateComponentDynamically: class \"" << className
                                         << "\" is not derived from mtsComponent" << std::endl;
        }
    }
    else if (!constructorArgSerialized.empty()) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateComponentDynamically: class \"" << className
                                 << "\" cannot handle serialized constructor arg" << std::endl;
        return 0;
    }

    // Above should have worked, following is for backward compatibility
    // looking in class register to create this component
    baseObject = cmnClassRegister::Create(className);
    if (!baseObject) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateComponentDynamically: unable to create component of type \""
                                 << className << "\"" << std::endl;
        return 0;
    }

    // make sure this is an mtsComponent
    newComponent = dynamic_cast<mtsComponent *>(baseObject);
    if (!newComponent) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateComponentDynamically: class \"" << className
                                 << "\" is not derived from mtsComponent" << std::endl;
        delete baseObject;
        return 0;
    }

    if (!isComponent) {
        CMN_LOG_CLASS_INIT_WARNING << "Class " << className << " is derived from mtsComponent, "
                                   << "but class services does not show inheritance from mtsComponent." << std::endl;
    }

    // rename the component
    newComponent->SetName(componentName);

    CMN_LOG_CLASS_INIT_VERBOSE << "CreateComponentDynamically: successfully created new component: "
                               << "\"" << newComponent->GetName() << "\" of type \""
                               << className << "\"" << std::endl;

    return newComponent;
}

#if CISST_HAS_JSON
bool mtsManagerLocal::ConfigureJSON(const std::string & filename)
{
    // extract path of main json config file to search other files relative to it
    cmnPath configPath(cmnPath::GetWorkingDirectory());
    std::string fullname = configPath.Find(filename);
    std::string configDir = fullname.substr(0, fullname.find_last_of('/'));
    configPath.Add(configDir, cmnPath::TAIL);
    // open json file
    std::ifstream jsonStream;
    jsonStream.open(filename.c_str());
    Json::Value jsonConfig;
    Json::Reader jsonReader;
    if (!jsonReader.parse(jsonStream, jsonConfig)) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureJSON: failed to parse configuration" << std::endl
                                 << "File: " << filename << std::endl
                                 << "Error(s):" << std::endl
                                 << jsonReader.getFormattedErrorMessages();
        return false;
    }

    if (jsonConfig.empty()) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureJSON: failed to configure component-manager, the file "
                                 << filename << " seems to be empty" << std::endl;
        return false;
    }

    return this->ConfigureJSON(jsonConfig, configPath);
}

bool mtsManagerLocal::ConfigureJSON(const Json::Value & configuration, const cmnPath & configPath)
{
    const Json::Value components = configuration["components"];
    for (unsigned int index = 0;
         index < components.size();
         ++index) {
        if (!ConfigureComponentJSON(components[index], configPath)) {
            CMN_LOG_CLASS_INIT_ERROR << "ConfigureJSON: failed to configure component ["
                                     << index << "]" << std::endl;
            return false;
        }
    }
    const Json::Value connections = configuration["connections"];
    for (unsigned int index = 0;
         index < connections.size();
         ++index) {
        if (!ConfigureConnectionJSON(connections[index])) {
            CMN_LOG_CLASS_INIT_ERROR << "ConfigureJSON: failed to configure connection ["
                                     << index << "]" << std::endl;
            return false;
        }
    }
    return true;
}

bool mtsManagerLocal::ConfigureComponentJSON(const Json::Value & componentConfiguration, const cmnPath & configPath)
{
    std::string sharedLibrary, className, constructorArgJSON;
    Json::Value jsonValue;

    // shared library is optional
    jsonValue = componentConfiguration["shared-library"];
    if (!jsonValue.empty()){
        sharedLibrary = jsonValue.asString();
    } else {
        sharedLibrary = "";
    }
    // class name is required
    jsonValue = componentConfiguration["class-name"];
    if (!jsonValue.empty()) {
        className = jsonValue.asString();
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureComponentJSON: can't find \"class-name\"" << std::endl;
        return false;
    }
    // constructor argument is required
    jsonValue = componentConfiguration["constructor-arg"];
    if (!jsonValue.empty()) {
        Json::FastWriter fastWriter;
        constructorArgJSON = fastWriter.write(jsonValue);
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureComponentJSON: can't find \"constructor-arg\"" << std::endl;
        return false;
    }
    // create (the method CreateComponentDynamicallyJSON should handle case w/o shared library
    mtsComponent * component
        = this->CreateComponentDynamicallyJSON(sharedLibrary,
                                               className,
                                               constructorArgJSON);
    if (!component) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureComponentJSON: failed to dynamically create component" << std::endl;
        return false;
    }
    // configure as needed
    Json::Value configureParameter = componentConfiguration["configure-parameter"];
    if (configureParameter.empty()) {
        component->Configure();
    } else {
        std::string configParam = configureParameter.asString();
        // see if we can find a file corresponding to string
        std::string configFile = configPath.Find(configParam);
        if (configFile == "") {
            // else pass the string as-is
            component->Configure(configParam);
        } else {
            component->Configure(configFile);
        }
    }
    // add
    if (!this->AddComponent(component)) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureComponentJSON: failed to add component to component manager" << std::endl;
        return false;
    }
    return true;
}


bool mtsManagerLocal::ConfigureConnectionJSON(const Json::Value & connectionConfiguration)
{
    Json::Value provided, required, jsonValue;
    // required
    required = connectionConfiguration["required"];
    if (required.empty()) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureConnectionJSON: failed to find \"required\"" << std::endl;
        return false;
    }
    jsonValue = required["component"];
    if (jsonValue.empty()) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureConnectionJSON: failed to find \"required\", \"component\""
                                 << std::endl;
        return false;
    }
    const std::string requiredComponent = jsonValue.asString();
    if (requiredComponent.empty()) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureConnectionJSON: \"required\", \"component\" is not a valid string"
                                 << std::endl;
        return false;
    }
    jsonValue = required["interface"];
    if (jsonValue.empty()) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureConnectionJSON: failed to find \"required\", \"interface\""
                                 << std::endl;
        return false;
    }
    const std::string requiredInterface = jsonValue.asString();
    if (requiredInterface.empty()) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureConnectionJSON: \"required\", \"interface\" is not a valid string"
                                 << std::endl;
        return false;
    }
    // provided
    provided = connectionConfiguration["provided"];
    if (provided.empty()) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureConnectionJSON: failed to find \"provided\"" << std::endl;
        return false;
    }
    jsonValue = provided["component"];
    if (jsonValue.empty()) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureConnectionJSON: failed to find \"provided\", \"component\""
                                 << std::endl;
        return false;
    }
    const std::string providedComponent = jsonValue.asString();
    if (providedComponent.empty()) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureConnectionJSON: \"provided\", \"component\" is not a valid string"
                                 << std::endl;
        return false;
    }
    jsonValue = provided["interface"];
    if (jsonValue.empty()) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureConnectionJSON: failed to find \"provided\", \"interface\""
                                 << std::endl;
        return false;
    }
    const std::string providedInterface = jsonValue.asString();
    if (providedInterface.empty()) {
        CMN_LOG_CLASS_INIT_ERROR << "ConfigureConnectionJSON: \"provided\", \"interface\" is not a valid string"
                                 << std::endl;
        return false;
    }
    // finally, request connection
    return this->Connect(requiredComponent, requiredInterface,
                         providedComponent, providedInterface);
}

mtsComponent * mtsManagerLocal::CreateComponentDynamicallyJSON(const std::string & sharedLibrary,
                                                               const std::string & className,
                                                               const std::string & constructorArgSerialized)
{
    // -1- try to dynamically load the library if specified
    if (!sharedLibrary.empty()) {
        // create load and path based on LD_LIBRARY_PATH
        osaDynamicLoader loader;
        std::string fullPath;
        // check if the file already exists, i.e. use provided a full path
        if (cmnPath::Exists(sharedLibrary)) {
            fullPath = sharedLibrary;
        } else {
            cmnPath path;
            path.AddFromEnvironment("LD_LIBRARY_PATH");
            fullPath = path.Find(cmnPath::SharedLibrary(sharedLibrary));
            if (fullPath.empty())  {
                fullPath = sharedLibrary;
                CMN_LOG_CLASS_INIT_WARNING << "CreateComponentDynamicallyJSON: using path: "
                                           << path << ", couldn't find \""
                                           << cmnPath::SharedLibrary(sharedLibrary)
                                           << "\"" << std::endl;;
            } else {
                CMN_LOG_CLASS_INIT_VERBOSE << "CreateComponentDynamicallyJSON: using path: "
                                           << path << ", found full path name \""
                                           << fullPath << "\"" << std::endl;;
            }
        }
        if (!loader.Load(fullPath)) {
            CMN_LOG_CLASS_INIT_ERROR << "CreateComponentDynamicallyJSON: failed to load shared library "
                                     << sharedLibrary << std::endl;
            return 0;
        }
    }

    // -2- try to dynamically create an instance of that class
    cmnClassServicesBase * componentClassServices = cmnClassRegister::FindClassServices(className);
    if (!componentClassServices) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateComponentDynamicallyJSON: unable to find class services for type "
                                 << className << std::endl;
        return 0;
    }

    // -3- check if we need to also create an argument for the constructor
    if (!componentClassServices->OneArgConstructorAvailable()) {
        return 0;
    }

    const cmnClassServicesBase * argumentClassServices = componentClassServices->GetConstructorArgServices();
    CMN_ASSERT(argumentClassServices); // this should not fail
    cmnGenericObject * argument = argumentClassServices->Create();
    if (!argument) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateComponentDynamicallyJSON: unable to create a constructor argument for "
                                 << className
                                 << ".  Make sure the macro CMN_IMPLEMENT_SERVICE_xx is correct for this class."
                                 << std::endl;
        return 0;
    }
    // then deserialize from JSON value...
    Json::Value jsonValue;
    Json::Reader reader;
    // parsing should work since the string has been generated after a previous parse
    bool parsedOk = reader.parse(constructorArgSerialized, jsonValue);
    CMN_ASSERT(parsedOk);
    try {
        argument->DeSerializeTextJSON(jsonValue);
    } catch (std::runtime_error e) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateComponentDynamicallyJSON: unable to deserialize constructor for "
                                 << className << " from JSON file, got exception: "
                                 << e.what() << std::endl;
        delete argument;
        return 0;
    }
    // now, finally, construct the component!
    cmnGenericObject * componentBase
        = componentClassServices->CreateWithArg(*argument);
    if (!componentBase) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateComponentDynamicallyJSON: failed to create component of type "
                                 << className << std::endl;
        delete argument;
        return 0;
    }
    // cleanup argument
    delete argument;

    // make sure this is a component
    mtsComponent * component = dynamic_cast<mtsComponent *>(componentBase);
    if (!component) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateComponentDynamicallyJSON: failed to cast newly created object of type "
                                 << className << " to mtsComponent" << std::endl;
        delete componentBase;
        return 0;
    }

    // looks like it worked!
    return component;
}
#endif

bool mtsManagerLocal::AddComponent(mtsComponent * component)
{
    if (!component) {
        CMN_LOG_CLASS_INIT_ERROR << "AddComponent: invalid component" << std::endl;
        return false;
    }

    std::string componentName = component->GetName();

    // If component does not yet have a valid name, assign one now, based on the class
    // name and the pointer value (to ensure that name is unique).
    if (componentName == "") {
        componentName.assign(component->Services()->GetName());
        char buf[20];
        sprintf(buf, "_%p", component);
        componentName.append(buf);
        CMN_LOG_CLASS_INIT_DEBUG << "AddComponent: assigning name \"" << componentName << "\"" << std::endl;
        component->SetName(componentName);
    }

    // Try to register new component to the global component manager first.
    if (!ManagerGlobal->AddComponent(ProcessName, componentName)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddComponent: failed to add component: " << componentName << std::endl;
        return false;
    }

    // If dynamic component management is enabled
    if (component->GetInterfaceRequired(mtsManagerComponentBase::GetNameOfInterfaceInternalRequired())) {
        // Add internal provided and required interface for dynamic component management service
        if (!component->AddInterfaceInternal(true)) {
            CMN_LOG_CLASS_INIT_ERROR << "AddComponent: failed to add \"Internal\" provided and required interfaces: " << componentName << std::endl;
            return false;
        }
    }
    // If dynamic component management is not enabled
    else {
        // Add internal interfaces depending on a type of the component.
        // Manager component client
        mtsManagerComponentClient * managerComponentClient = dynamic_cast<mtsManagerComponentClient*>(component);
        mtsManagerComponentServer * managerComponentServer = dynamic_cast<mtsManagerComponentServer*>(component);
        if (managerComponentClient) {
            if (!managerComponentClient->AddInterfaceComponent()) {
                CMN_LOG_CLASS_INIT_ERROR << "AddComponent: failed to add \"Component\" interfaces: " << componentName << std::endl;
                return false;
            }
            if (!managerComponentClient->AddInterfaceLCM()) {
                CMN_LOG_CLASS_INIT_ERROR << "AddComponent: failed to add \"LCM\" interfaces: " << componentName << std::endl;
                return false;
            }
        }
        // Manager component server
        else if (managerComponentServer) {
            if (!managerComponentServer->AddInterfaceGCM()) {
                CMN_LOG_CLASS_INIT_ERROR << "AddComponent: failed to add \"GCM\" interfaces: " << componentName << std::endl;
                return false;
            }
        }
        // User(generic) component
        else {
            // Add a internal provided interface.  This interface is connected to the
            // manager component client and is used to inform it of the change of
            // the running state of this component (more features can be added later).
            if (!component->AddInterfaceInternal()) {
                CMN_LOG_CLASS_INIT_ERROR << "AddComponent: failed to add \"Internal\" provided interfaces: " << componentName << std::endl;
                return false;
            }
        }
    }

    // Register all the existing required interfaces and provided interfaces to
    // the global component manager and mark them as registered.
    if (!RegisterInterfaces(component)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddComponent: failed to register interfaces" << std::endl;
        return false;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "AddComponent: successfully added component to GCM: " << componentName << std::endl;
    // PK TEMP
    ManagerGlobal->AddComponent(ProcessName, componentName+"-END");

    bool success;
    ComponentMapChange.Lock();
    success = ComponentMap.AddItem(componentName, component);
    ComponentMapChange.Unlock();

    if (!success) {
        CMN_LOG_CLASS_INIT_ERROR << "AddComponent: "
                                 << "failed to add component to local component manager: " << componentName << std::endl;
        return false;
    }

    // MJ: The current design of dynamic component composition services
    // assumes that no user component runs in the GCM's process.  That is,
    // the manager component server (MCS) running in the GCM doesn't need
    // to be connected to any component.
    const bool isManagerComponent = (mtsManagerComponentBase::IsManagerComponentServer(componentName) ||
                                     mtsManagerComponentBase::IsManagerComponentClient(componentName));
    if (!isManagerComponent) {
        // Connect user component's internal interface to the manager component.
        // That is, connect InterfaceInternal.Required to InterfaceComponent.Provided.
        // This enables user components to use dynamic component composition services
        // through cisstMultiTask's thread-safe command pattern.
        // PK: Always do this
        //if ((Configuration == LCM_CONFIG_STANDALONE) || (Configuration == LCM_CONFIG_NETWORKED)) {
        if (1) {
            mtsManagerComponentClient * managerComponent = ManagerComponent.Client;
            if (!managerComponent) {
                CMN_LOG_CLASS_INIT_ERROR << "AddComponent: failed to get MCC" << std::endl;
                return false;
            }

            if (componentName != managerComponent->GetName() &&
                componentName != mtsManagerComponentBase::GetNameOfManagerComponentServer())
            {
                // Create InterfaceComponent's required interface which will be connected
                // to connect user component's InterfaceInternal's provided interface.
                if (!managerComponent->AddNewClientComponent(componentName)) {
                    CMN_LOG_CLASS_INIT_ERROR << "AddComponent: "
                        << "failed to add InterfaceComponent's required interface to MCC: "
                        << "\"" << componentName << "\"" << std::endl;
                    return false;
                }

                // Connect user component to the manager component client.  If a component
                // has InterfaceInternal's required interface which provides dynamic
                // component control services, the required interface gets connected to
                // InterfaceComponent's provided interface.
                if (!ConnectToManagerComponentClient(componentName)) {
                    CMN_LOG_CLASS_INIT_ERROR << "AddComponent: failed to connect component \"" << componentName << "\" "
                        << "to MCC" << std::endl;
                    return false;
                }

                CMN_LOG_CLASS_INIT_VERBOSE << "AddComponent: connected user components "
                    << "\"" << componentName << "\" to manager component client "
                    << "\"" << managerComponent->GetName() << "\""
                    << std::endl;
            }
        }
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "AddComponent: successfully added component to LCM: " << componentName << std::endl;

    return true;
}

bool CISST_DEPRECATED mtsManagerLocal::AddTask(mtsTask * component)
{
    return AddComponent(component);
}

bool CISST_DEPRECATED mtsManagerLocal::AddDevice(mtsComponent * component)
{
    return AddComponent(component);
}

bool mtsManagerLocal::RemoveComponent(mtsComponent * component)
{
    if (component == 0) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveComponent: null component pointer passed to this method" << std::endl;
        return false;
    }
    return RemoveComponent(component->GetName(), true);
}

bool mtsManagerLocal::RemoveComponent(const std::string & componentName)
{
    return RemoveComponent(componentName, true);
}

bool mtsManagerLocal::RemoveComponent(mtsComponent * component, const bool notifyGCM)
{
    if (!component) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveComponent: invalid argument" << std::endl;
        return false;
    }

    return RemoveComponent(component->GetName(), notifyGCM);
}

bool mtsManagerLocal::RemoveComponent(const std::string & componentName, const bool notifyGCM)
{
    // Notify the global component manager of the removal of this component
    if (notifyGCM) {
        if (!ManagerGlobal->RemoveComponent(ProcessName, componentName)) {
            CMN_LOG_CLASS_INIT_ERROR << "RemoveComponent: failed to remove \"" << componentName << "\""
                << "from component Global Component Manager" << std::endl;
            return false;
        }
    }

    // Get a component to be removed
    mtsComponent * component = ComponentMap.GetItem(componentName, CMN_LOG_LEVEL_NONE);
    if (!component) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveComponent: failed to get component to be removed: " << componentName << std::endl;
        return false;
    }

    bool success;
    ComponentMapChange.Lock();
    success = ComponentMap.RemoveItem(componentName);
    ComponentMapChange.Unlock();

    if (!success) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveComponent: failed to remove component: " << componentName << std::endl;
        return false;
    }
    else {
        CMN_LOG_CLASS_INIT_VERBOSE << "RemoveComponent: removed component: " << componentName << std::endl;
    }

    return true;
}

size_t mtsManagerLocal::RemoveAllUserComponents(void)
{
    std::vector<std::string> componentNames = ComponentMap.GetNames();
    size_t numRemoved = 0;
    for (size_t i = 0; i < componentNames.size(); i++) {
        const bool isManagerComponent = (mtsManagerComponentBase::IsManagerComponentServer(componentNames[i]) ||
                                         mtsManagerComponentBase::IsManagerComponentClient(componentNames[i]));
        if (!isManagerComponent) {
            if (!RemoveComponent(componentNames[i])) {
                CMN_LOG_CLASS_RUN_WARNING << "RemoveAllUserComponents: failed to remove "
                                          << componentNames[i] << std::endl;
            }
            else {
                numRemoved++;
            }
        }
    }
    if (numRemoved > 0) {
        CMN_LOG_CLASS_RUN_VERBOSE << "RemoveAllUserComponents: removed " << numRemoved << " user components, leaving "
                                  << componentNames.size() << " system components" << std::endl;
    }
    return numRemoved;
}

std::vector<std::string> mtsManagerLocal::GetNamesOfComponents(void) const
{
    return ComponentMap.GetNames();
}

void mtsManagerLocal::GetNamesOfComponents(std::vector<std::string> & namesOfComponents) const
{
    ComponentMap.GetNames(namesOfComponents);
}

void mtsManagerLocal::PushCurrentMainTask(mtsTaskContinuous *cur)
{
    if (!cur) {
        CMN_LOG_CLASS_RUN_ERROR << "PushCurrentMainTask: null parameter" << std::endl;
        return;
    }
    if (cur == CurrentMainTask) {
        CMN_LOG_CLASS_RUN_WARNING << "PushCurrentMainTask: duplicate call to push " << cur->GetName() << std::endl;
        return;
    }
    if (CurrentMainTask)
         CMN_LOG_CLASS_RUN_WARNING << "CurrentMainTask changing from " << CurrentMainTask->GetName()
                                      << " to " << cur->GetName() << std::endl;
    else
         CMN_LOG_CLASS_RUN_VERBOSE << "Setting CurrentMainTask to " << cur->GetName() << std::endl;
    CurrentMainTask = cur;
    MainTaskNames.push(CurrentMainTask->GetName());
}

mtsTaskContinuous *mtsManagerLocal::PopCurrentMainTask(void)
{
    mtsTaskContinuous *previousMainTask = 0;
    while (!previousMainTask && !MainTaskNames.empty()) {
        previousMainTask = dynamic_cast<mtsTaskContinuous *>(GetComponent(MainTaskNames.top()));
        if (!previousMainTask) {
            CMN_LOG_CLASS_RUN_WARNING << "PopCurrentMainTask: could not find " << MainTaskNames.top() << std::endl;
        }
        MainTaskNames.pop();
    }
    if (previousMainTask) {
        CMN_LOG_CLASS_RUN_VERBOSE << CurrentMainTask->GetName() << " is exiting, so main task reverts to "
                                  << previousMainTask->GetName() << std::endl;
    }
    else {
        CMN_LOG_CLASS_RUN_VERBOSE << CurrentMainTask->GetName() << " is exiting, no main task remaining" << std::endl;
    }
    CurrentMainTask = previousMainTask;
    return CurrentMainTask;
}

void mtsManagerLocal::GetNamesOfCommands(std::vector<std::string>& namesOfCommands,
                                         const std::string & componentName,
                                         const std::string & interfaceName,
                                         const std::string & CMN_UNUSED(listenerID))
{
    mtsInterfaceProvidedDescription desc;
    if (!GetInterfaceProvidedDescription(componentName, interfaceName, desc)) {
        CMN_LOG_CLASS_INIT_ERROR << "GetNamesOfCommands: failed to get provided interface information: "
                                 << this->ProcessName << ":" << componentName << ":" << interfaceName << std::endl;
        return;
    }

    std::string name;
    for (size_t i = 0; i < desc.CommandsVoid.size(); ++i) {
        name = "V) ";
        name += desc.CommandsVoid[i].Name;
        namesOfCommands.push_back(name);
    }
    for (size_t i = 0; i < desc.CommandsWrite.size(); ++i) {
        name = "W) ";
        name += desc.CommandsWrite[i].Name;
        namesOfCommands.push_back(name);
    }
    for (size_t i = 0; i < desc.CommandsRead.size(); ++i) {
        name = "R) ";
        name += desc.CommandsRead[i].Name;
        namesOfCommands.push_back(name);
    }
    for (size_t i = 0; i < desc.CommandsQualifiedRead.size(); ++i) {
        name = "Q) ";
        name += desc.CommandsQualifiedRead[i].Name;
        namesOfCommands.push_back(name);
    }
    for (size_t i = 0; i < desc.CommandsVoidReturn.size(); ++i) {
        name = "v) ";
        name += desc.CommandsVoidReturn[i].Name;
        namesOfCommands.push_back(name);
    }
    for (size_t i = 0; i < desc.CommandsWriteReturn.size(); ++i) {
        name = "w) ";
        name += desc.CommandsWriteReturn[i].Name;
        namesOfCommands.push_back(name);
    }
}

void mtsManagerLocal::GetNamesOfEventGenerators(std::vector<std::string> & namesOfEventGenerators,
                                                const std::string & componentName,
                                                const std::string & interfaceName,
                                                const std::string & CMN_UNUSED(listenerID))
{
    mtsInterfaceProvidedDescription desc;
    if (!GetInterfaceProvidedDescription(componentName, interfaceName, desc)) {
        CMN_LOG_CLASS_INIT_ERROR << "GetNamesOfEventGenerators: failed to get provided interface information: "
                                 << this->ProcessName << ":" << componentName << ":" << interfaceName << std::endl;
        return;
    }

    std::string name;
    for (size_t i = 0; i < desc.EventsVoid.size(); ++i) {
        name = "V) ";
        name += desc.EventsVoid[i].Name;
        namesOfEventGenerators.push_back(name);
    }
    for (size_t i = 0; i < desc.EventsWrite.size(); ++i) {
        name = "W) ";
        name += desc.EventsWrite[i].Name;
        namesOfEventGenerators.push_back(name);
    }
}

void mtsManagerLocal::GetNamesOfFunctions(std::vector<std::string> & namesOfFunctions,
                                          const std::string & componentName,
                                          const std::string & requiredInterfaceName,
                                          const std::string & CMN_UNUSED(listenerID))
{
    mtsInterfaceRequiredDescription desc;
    if (!GetInterfaceRequiredDescription(componentName, requiredInterfaceName, desc)) {
        return;
    }

    std::string name;
    for (size_t i = 0; i < desc.FunctionVoidNames.size(); ++i) {
        name = "V) ";
        name += desc.FunctionVoidNames[i];
        namesOfFunctions.push_back(name);
    }
    for (size_t i = 0; i < desc.FunctionWriteNames.size(); ++i) {
        name = "W) ";
        name += desc.FunctionWriteNames[i];
        namesOfFunctions.push_back(name);
    }
    for (size_t i = 0; i < desc.FunctionReadNames.size(); ++i) {
        name = "R) ";
        name += desc.FunctionReadNames[i];
        namesOfFunctions.push_back(name);
    }
    for (size_t i = 0; i < desc.FunctionQualifiedReadNames.size(); ++i) {
        name = "Q) ";
        name += desc.FunctionQualifiedReadNames[i];
        namesOfFunctions.push_back(name);
    }
    for (size_t i = 0; i < desc.FunctionVoidReturnNames.size(); ++i) {
        name = "v) ";
        name += desc.FunctionVoidReturnNames[i];
        namesOfFunctions.push_back(name);
    }
    for (size_t i = 0; i < desc.FunctionWriteReturnNames.size(); ++i) {
        name = "w) ";
        name += desc.FunctionWriteReturnNames[i];
        namesOfFunctions.push_back(name);
    }
}

void mtsManagerLocal::GetNamesOfEventHandlers(std::vector<std::string> & namesOfEventHandlers,
                                              const std::string & componentName,
                                              const std::string & requiredInterfaceName,
                                              const std::string & CMN_UNUSED(listenerID))
{
    mtsInterfaceRequiredDescription desc;
    if (!GetInterfaceRequiredDescription(componentName, requiredInterfaceName, desc)) {
        return;
    }

    std::string name;
    for (size_t i = 0; i < desc.EventHandlersVoid.size(); ++i) {
        name = "V) ";
        name += desc.EventHandlersVoid[i].Name;
        namesOfEventHandlers.push_back(name);
    }
    for (size_t i = 0; i < desc.EventHandlersWrite.size(); ++i) {
        name = "W) ";
        name += desc.EventHandlersWrite[i].Name;
        namesOfEventHandlers.push_back(name);
    }
}

void mtsManagerLocal::GetDescriptionOfCommand(std::string & description,
                                              const std::string & componentName,
                                              const std::string & interfaceName,
                                              const std::string & commandName,
                                              const std::string & CMN_UNUSED(listenerID))
{
    mtsComponent * component = GetComponent(componentName);
    if (!component) return;

    mtsInterfaceProvided * interfaceProvided = component->GetInterfaceProvided(interfaceName);
    if (!interfaceProvided) return;

    // Get command type
    char commandType = *commandName.c_str();
    std::string actualCommandName = commandName.substr(3, commandName.size() - 2);

    description = "Argument type: ";
    switch (commandType) {
        case 'V':
            {
                mtsCommandVoid * command = interfaceProvided->GetCommandVoid(actualCommandName);
                if (!command) {
                    description = "No void command found for ";
                    description += actualCommandName;
                    return;
                }
                description += "(none)";
            }
            break;
        case 'W':
            {
                mtsCommandWriteBase * command = interfaceProvided->GetCommandWrite(actualCommandName);
                if (!command) {
                    description = "No write command found for ";
                    description += actualCommandName;
                    return;
                }
                description += command->GetArgumentClassServices()->GetName();
            }
            break;
        case 'R':
            {
                mtsCommandRead * command = interfaceProvided->GetCommandRead(actualCommandName);
                if (!command) {
                    description = "No read command found for ";
                    description += actualCommandName;
                    return;
                }
                description += command->GetArgumentPrototype()->Services()->GetName();
            }
            break;
        case 'Q':
            {
                mtsCommandQualifiedRead * command = interfaceProvided->GetCommandQualifiedRead(actualCommandName);
                if (!command) {
                    description = "No qualified read command found for ";
                    description += actualCommandName;
                    return;
                }
                description = "Argument1 type: ";
                description += command->GetArgument1Prototype()->Services()->GetName();
                description += "\nArgument2 type: ";
                description += command->GetArgument2Prototype()->Services()->GetName();
            }
            break;
        default:
            CMN_LOG_CLASS_INIT_ERROR << "GetDescriptionOfCommand: type of command not handled for command \"" << commandName << "\"" << std::endl;
            description = "Failed to get command description";
            return;
    }
}

void mtsManagerLocal::GetDescriptionOfEventGenerator(std::string & description,
                                                     const std::string & componentName,
                                                     const std::string & interfaceName,
                                                     const std::string & eventGeneratorName,
                                                     const std::string & CMN_UNUSED(listenerID))
{
    mtsComponent * component = GetComponent(componentName);
    if (!component) return;

    mtsInterfaceProvided * interfaceProvided = component->GetInterfaceProvided(interfaceName);
    if (!interfaceProvided) return;

    // Get event generator type
    char eventGeneratorType = *eventGeneratorName.c_str();
    std::string actualEventGeneratorName = eventGeneratorName.substr(3, eventGeneratorName.size() - 2);

    description = "Argument type: ";
    switch (eventGeneratorType) {
        case 'V':
            {
                mtsCommandVoid * eventGenerator = interfaceProvided->EventVoidGenerators.GetItem(actualEventGeneratorName, CMN_LOG_LEVEL_NONE);
                if (!eventGenerator) {
                    description = "No void event generator found";
                    return;
                }
                description += "(none)";
            }
            break;
        case 'W':
            {
                mtsCommandWriteBase * eventGenerator = interfaceProvided->EventWriteGenerators.GetItem(actualEventGeneratorName, CMN_LOG_LEVEL_NONE);
                if (!eventGenerator) {
                    description = "No write event generator found";
                    return;
                }
                description += eventGenerator->GetArgumentClassServices()->GetName();
            }
            break;
        default:
            description = "Failed to get event generator description";
            return;
    }
}

void mtsManagerLocal::GetDescriptionOfFunction(std::string & description,
                                               const std::string & componentName,
                                               const std::string & requiredInterfaceName,
                                               const std::string & functionName,
                                               const std::string & CMN_UNUSED(listenerID))
{
    mtsComponent * component = GetComponent(componentName);
    if (!component) return;

    mtsInterfaceRequired * requiredInterface = component->GetInterfaceRequired(requiredInterfaceName);
    if (!requiredInterface) return;

    // Get function type
    //char functionType = *functionName.c_str();
    std::string actualFunctionName = functionName.substr(3, functionName.size() - 2);

    description = "Resource argument type: ";
#if 0 // adeguet1 todo fix --- this is using internal values of the interface, this should be done otherwise
    switch (functionType) {
        case 'V':
            {
                mtsInterfaceRequired::CommandInfo<mtsCommandVoidBase> * function = requiredInterface->CommandPointersVoid.GetItem(actualFunctionName, CMN_LOG_LEVEL_NONE);
                if (!function) {
                    description = "No void function found";
                    return;
                }
                description += "(none)";
            }
            break;
        case 'W':
            {
                mtsInterfaceRequired::CommandInfo<mtsCommandWriteBase> * function = requiredInterface->CommandPointersWrite.GetItem(actualFunctionName, CMN_LOG_LEVEL_NONE);
                if (!function) {
                    description = "No write function found";
                    return;
                }
                if (*function->CommandPointer) {
                    description += (*function->CommandPointer)->GetArgumentClassServices()->GetName();
                } else {
                    description += "(unbounded write function)";
                }
            }
            break;
        case 'R':
            {
                mtsInterfaceRequired::CommandInfo<mtsCommandReadBase> * function = requiredInterface->CommandPointersRead.GetItem(actualFunctionName, CMN_LOG_LEVEL_NONE);
                if (!function) {
                    description = "No read function found";
                    return;
                }
                if (*function->CommandPointer) {
                    description += (*function->CommandPointer)->GetArgumentClassServices()->GetName();
                } else {
                    description += "(unbounded read function)";
                }
            }
            break;
        case 'Q':
            {
                mtsInterfaceRequired::CommandInfo<mtsCommandQualifiedReadBase> * function = requiredInterface->CommandPointersQualifiedRead.GetItem(actualFunctionName, CMN_LOG_LEVEL_NONE);
                if (!function) {
                    description = "No qualified read function found";
                    return;
                }
                if (*function->CommandPointer) {
                    description = "Resource argument1 type: ";
                    description += (*function->CommandPointer)->GetArgument1ClassServices()->GetName();
                    description += "\nResource argument2 type: ";
                    description += (*function->CommandPointer)->GetArgument2ClassServices()->GetName();
                } else {
                    description = "Resource argument1 type: ";
                    description += "(unbounded qualified read function)";
                    description += "\nResource argument2 type: ";
                    description += "(unbounded qualified read function)";
                }

            }
            break;
        default:
            CMN_LOG_CLASS_INIT_ERROR << "GetDescriptionOfFunction: type of function not handled for command \"" << functionName << "\"" << std::endl;
            description = "Failed to get function description";
            return;
    }
#endif
}

void mtsManagerLocal::GetDescriptionOfEventHandler(std::string & description,
                                                   const std::string & componentName,
                                                   const std::string & requiredInterfaceName,
                                                   const std::string & eventHandlerName,
                                                   const std::string & CMN_UNUSED(listenerID))
{
    mtsComponent * component = GetComponent(componentName);
    if (!component) return;

    mtsInterfaceRequired * requiredInterface = component->GetInterfaceRequired(requiredInterfaceName);
    if (!requiredInterface) return;

    // Get event handler type
    char eventHandlerType = *eventHandlerName.c_str();
    std::string actualEventHandlerName = eventHandlerName.substr(3, eventHandlerName.size() - 2);

    description = "Argument type: ";
    switch (eventHandlerType) {
        case 'V':
            {
                mtsCommandVoid * command = requiredInterface->EventHandlersVoid.GetItem(actualEventHandlerName, CMN_LOG_LEVEL_NONE);
                if (!command) {
                    description = "No void event handler found";
                    return;
                }
                description += "(none)";
            }
            break;
        case 'W':
            {
                mtsCommandWriteBase * command = requiredInterface->EventHandlersWrite.GetItem(actualEventHandlerName, CMN_LOG_LEVEL_NONE);
                if (!command) {
                    description = "No write event handler found";
                    return;
                }
                description += command->GetArgumentClassServices()->GetName();
            }
            break;
        default:
            description = "Failed to get event handler description";
            return;
    }
}


mtsComponent * mtsManagerLocal::GetComponent(const std::string & componentName) const
{
    return ComponentMap.GetItem(componentName, CMN_LOG_LEVEL_NONE);
}

mtsTask * mtsManagerLocal::GetComponentAsTask(const std::string & componentName) const
{
    mtsTask * componentTask = 0;

    mtsComponent * component = ComponentMap.GetItem(componentName, CMN_LOG_LEVEL_NONE);
    if (component) {
        componentTask = dynamic_cast<mtsTask*>(component);
    }

    return componentTask;
}

mtsTask CISST_DEPRECATED * mtsManagerLocal::GetTask(const std::string & taskName)
{
    return GetComponentAsTask(taskName);
}

mtsComponent CISST_DEPRECATED * mtsManagerLocal::GetDevice(const std::string & deviceName)
{
    return ComponentMap.GetItem(deviceName, CMN_LOG_LEVEL_NONE);
}

bool mtsManagerLocal::FindComponent(const std::string & componentName) const
{
    return (GetComponent(componentName) != 0);
}

bool mtsManagerLocal::CreateManagerComponents(void)
{
    // Automatically add internal manager component when the LCM is initialized.
#if CISST_MTS_HAS_ICE
    if ((Configuration == LCM_CONFIG_STANDALONE) || (Configuration == LCM_CONFIG_NETWORKED_WITH_GCM)) {
#else
    if (Configuration == LCM_CONFIG_STANDALONE) {
#endif
        if (!AddManagerComponent(GetProcessName(), true)) {
            CMN_LOG_CLASS_INIT_ERROR << "CreateManagerComponents: failed to add internal manager component server" << std::endl;
            return false;
        }
    }

    // Always add the MCC and connect it to the MCS
    //if ((Configuration == LCM_CONFIG_STANDALONE) || (Configuration == LCM_CONFIG_NETWORKED)) {
    if (1) {
        if (!AddManagerComponent(GetProcessName())) {
            CMN_LOG_CLASS_INIT_ERROR << "CreateManagerComponents: failed to add internal MCC" << std::endl;
            return false;
        }
        // Connect manager component client to manager component server, i.e.,
        // connect InterfaceLCM.Required - InterfaceGCM.Provided
        if (!ConnectManagerComponentClientToServer()) {
            CMN_LOG_CLASS_INIT_ERROR << "CreateManagerComponents: failed to connect MCC to server" << std::endl;
            return false;
        }
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "CreateManagerComponents: Successfully created manager components" << std::endl;

    ManagerComponent.Client->MCSReady = true;

    return true;
}


bool mtsManagerLocal::WaitForStateAll(mtsComponentState desiredState, double timeout) const
{
    // wait for all components to be started if timeout is positive
    bool allAtState = true;
    mtsManagerComponentBase * isManager;
    if (timeout > 0.0) {
        // will iterate on all components
        ComponentMapType::const_iterator iterator = ComponentMap.begin();
        const ComponentMapType::const_iterator end = ComponentMap.end();
        double timeStartedAll = TimeServer.GetRelativeTime();
        double timeEnd = timeStartedAll + timeout;
        bool timedOut = false;
        for (; (iterator != end) && allAtState && !timedOut; ++iterator) {
            // compute how much time do we have left based on when we started
            double timeLeft = timeEnd - TimeServer.GetRelativeTime();
            isManager = dynamic_cast<mtsManagerComponentBase *>(iterator->second);
            if (!isManager) {
                allAtState = iterator->second->WaitForState(desiredState, timeLeft);
                if (!allAtState) {
                    CMN_LOG_CLASS_INIT_ERROR << "WaitForStateAll: component \"" << iterator->first << "\" failed to reach state \""
                                             << desiredState << "\"" << std::endl;
                }
            }
            if (TimeServer.GetRelativeTime() > timeEnd) {
                // looks like we don't have any time left to start the remaining components.
                timedOut = true;
                allAtState = false;
                CMN_LOG_CLASS_INIT_ERROR << "WaitForStateAll: timed out while waiting for state \""
                                         << desiredState << "\"" << std::endl;
            }
        }
        // report results
        if (allAtState && !timedOut) {
            CMN_LOG_CLASS_INIT_VERBOSE << "WaitForStateAll: all components reached state \""
                                       << desiredState << "\" in " << (TimeServer.GetRelativeTime() - timeStartedAll) << " seconds" << std::endl;
        } else {
            CMN_LOG_CLASS_INIT_ERROR << "WaitForStateAll: failed to reached state \""
                                     << desiredState << "\" for all components" << std::endl;
        }
    } else {
        CMN_LOG_CLASS_INIT_VERBOSE << "WaitForStateAll: called with null timeout (not blocking)" << std::endl;
    }
    return allAtState;
}


void mtsManagerLocal::CreateAll(void)
{
    ComponentMapChange.Lock();

    ComponentMapType::const_iterator iterator = ComponentMap.begin();
    const ComponentMapType::const_iterator end = ComponentMap.end();
    for (; iterator != end; ++iterator) {
        iterator->second->Create();
    }

    ComponentMapChange.Unlock();
}


bool mtsManagerLocal::CreateAllAndWait(double timeoutInSeconds)
{
    this->CreateAll();
    return this->WaitForStateAll(mtsComponentState::READY, timeoutInSeconds);
}


void mtsManagerLocal::StartAll(void)
{
    // Get the current thread id in order to check if any task will use the current thread.
    // If so, start that task last.
    const osaThreadId threadId = osaGetCurrentThreadId();
    if (threadId != this->MainThreadId) {
        CMN_LOG_CLASS_RUN_WARNING << "StartAll: current thread is not main thread." << std::endl;
    }

    mtsTask * componentTask;

    ComponentMapChange.Lock();

    ComponentMapType::const_iterator iterator = ComponentMap.begin();
    const ComponentMapType::const_iterator end = ComponentMap.end();
    ComponentMapType::const_iterator lastTask = ComponentMap.end();

    for (; iterator != end; ++iterator) {
        // look for component
        componentTask = dynamic_cast<mtsTask*>(iterator->second);
        if (componentTask) {
            // Check if the task will use the current thread.
            if (componentTask->Thread.GetId() == threadId) {
                if (dynamic_cast<mtsTaskFromCallback*>(iterator->second)) {
                    CMN_LOG_CLASS_INIT_VERBOSE << "StartAll: component \"" << iterator->first
                                               << "\" uses current thread, but is a callback task;"
                                               << " expect that it will be called by dispatcher." << std::endl;
                    iterator->second->Start();
                }
                else {
                    CMN_LOG_CLASS_INIT_WARNING << "StartAll: component \"" << iterator->first
                                               << "\" uses current thread, will be started last." << std::endl;
                    if (lastTask != end) {
                        CMN_LOG_CLASS_INIT_ERROR << "StartAll: found another task using current thread (\""
                                                 << iterator->first << "\"), only first will be started (\""
                                                 << lastTask->first << "\")." << std::endl;
                        // PK: I don't think this task should be started if it uses the current thread
                        iterator->second->Start();
                    } else {
                        // set iterator to last task to be started
                        lastTask = iterator;
                    }
                }
            } else {
                CMN_LOG_CLASS_INIT_DEBUG << "StartAll: starting task \"" << iterator->first << "\"" << std::endl;
                if (componentTask->Thread.GetId() == MainThreadId) {
                    if (dynamic_cast<mtsTaskContinuous *>(componentTask)) {
                        CMN_LOG_CLASS_INIT_WARNING << "StartAll: is the main task really " << iterator->first << "???" << std::endl;
                    }
                }
                iterator->second->Start();  // If task will not use current thread, start it immediately.
            }
        } else {
            CMN_LOG_CLASS_INIT_DEBUG << "StartAll: starting component \"" << iterator->first << "\"" << std::endl;
            iterator->second->Start();  // this is a component, it doesn't have a thread
        }
    }

    ComponentMapChange.Unlock();

    if (lastTask != end) {
        lastTask->second->Start();
    }
}


bool mtsManagerLocal::StartAllAndWait(double timeoutInSeconds)
{
    this->StartAll();
    return this->WaitForStateAll(mtsComponentState::ACTIVE, timeoutInSeconds);
}


void mtsManagerLocal::KillAll(void)
{
    mtsManagerComponentBase * isManager;
    ComponentMapChange.Lock(); {
        ComponentMapType::const_iterator iterator = ComponentMap.begin();
        const ComponentMapType::const_iterator end = ComponentMap.end();
        for (; iterator != end; ++iterator) {
            if (!iterator->second) {
                CMN_LOG_CLASS_INIT_DEBUG << "KillAll: null component" << std::endl;
                continue;
            }
            isManager = dynamic_cast<mtsManagerComponentBase *>(iterator->second);
            if (!isManager) {
                iterator->second->Kill();
            } else {
                CMN_LOG_CLASS_INIT_DEBUG << "KillAll: skip manager component: " << iterator->second->GetName() << std::endl;
            }
        }
    }
    ComponentMapChange.Unlock();

    // Block further logs
    LogDisabled = true;
    SetLogForwarding(false);
}


bool mtsManagerLocal::KillAllAndWait(double timeoutInSeconds)
{
    this->KillAll();
    return this->WaitForStateAll(mtsComponentState::FINISHED, timeoutInSeconds);
}


bool mtsManagerLocal::Connect(const std::string & clientComponentName, const std::string & clientInterfaceName,
                              const std::string & serverComponentName, const std::string & serverInterfaceName)
{
    if (!ManagerComponent.Client) {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: MCC not yet created" << std::endl;
        return false;
    }

    return ManagerComponent.Client->Connect(clientComponentName, clientInterfaceName,
                                            serverComponentName, serverInterfaceName);
}

ConnectionIDType mtsManagerLocal::ConnectSetup(const std::string & clientComponentName, const std::string & clientInterfaceName,
                                               const std::string & serverComponentName, const std::string & serverInterfaceName)
{
    std::vector<std::string> options;
    std::stringstream allOptions;
    std::ostream_iterator< std::string > output(allOptions, " ");

    // Make sure all interfaces created so far are registered to the GCM.
    if (!RegisterInterfaces(clientComponentName)) {
        GetNamesOfComponents(options);
        if (options.size() == 0) {
            allOptions << "there is no component available";
        } else {
            allOptions << "the following component(s) are available: ";
            std::copy(options.begin(), options.end(), output);
        }
        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to register interfaces for component \""
                                 << clientComponentName << "\", " << allOptions.str() << std::endl;
        return InvalidConnectionID;
    }
    if (!RegisterInterfaces(serverComponentName)) {
        GetNamesOfComponents(options);
        if (options.size() == 0) {
            allOptions << "there is no component available";
        } else {
            allOptions << "the following component(s) are available: ";
            std::copy(options.begin(), options.end(), output);
        }
        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to register interfaces for component \""
                                 << serverComponentName << "\", " << allOptions.str() << std::endl;
        return InvalidConnectionID;
    }

    const ConnectionIDType connectionID =
        ManagerGlobal->Connect(ProcessName,
                               ProcessName, clientComponentName, clientInterfaceName,
                               ProcessName, serverComponentName, serverInterfaceName);
    if (connectionID == InvalidConnectionID) {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to get connection id from Global Component Manager: "
                                 << clientComponentName << ":" << clientInterfaceName << " - "
                                 << serverComponentName << ":" << serverInterfaceName << std::endl;
    } else {
        CMN_LOG_CLASS_INIT_VERBOSE << "Connect: new connection id: LOCAL (" << connectionID << ") for "
                                   << mtsManagerGlobal::GetInterfaceUID(ProcessName, clientComponentName, clientInterfaceName)
                                   << " - "
                                   << mtsManagerGlobal::GetInterfaceUID(ProcessName, serverComponentName, serverInterfaceName)
                                   << std::endl;
    }

    return connectionID;
}

bool mtsManagerLocal::ConnectNotify(ConnectionIDType connectionId,
                                    const std::string & clientComponentName, const std::string & clientInterfaceName,
                                    const std::string & serverComponentName, const std::string & serverInterfaceName)
{
    // Notify the GCM of successful local connection
    if (!ManagerGlobal->ConnectConfirm(connectionId)) {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to notify GCM of this connection (" << connectionId << "): "
                                 << clientComponentName << ":" << clientInterfaceName << " - "
                                 << serverComponentName << ":" << serverInterfaceName << std::endl;

        if (!Disconnect(clientComponentName, clientInterfaceName, serverComponentName, serverInterfaceName)) {
            CMN_LOG_CLASS_INIT_ERROR << "Connect: clean up error: disconnection failed: "
                                     << clientComponentName << ":" << clientInterfaceName << " - "
                                     << serverComponentName << ":" << serverInterfaceName << std::endl;
        }

        return false;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "Connect: successfully established local connection: "
                               << clientComponentName << ":" << clientInterfaceName << " - "
                               << serverComponentName << ":" << serverInterfaceName << std::endl;

    return true;
}

std::vector<std::string> mtsManagerLocal::GetIPAddressList(void)
{
    std::vector<std::string> ipAddresses;
    osaSocket::GetLocalhostIP(ipAddresses);

    return ipAddresses;
}

void mtsManagerLocal::GetIPAddressList(std::vector<std::string> & ipAddresses)
{
    osaSocket::GetLocalhostIP(ipAddresses);
}

bool mtsManagerLocal::Connect(const std::string & clientProcessName,
                              const std::string & clientComponentName, const std::string & clientInterfaceName,
                              const std::string & serverProcessName,
                              const std::string & serverComponentName, const std::string & serverInterfaceName,
#if CISST_MTS_HAS_ICE
                              const unsigned int retryCount)
#else
                              const unsigned int CMN_UNUSED(retryCount))
#endif
{
    // Prevent this method from being used to connect two local interfaces
    if (clientProcessName == serverProcessName) {
        return Connect(clientComponentName, clientInterfaceName, serverComponentName, serverInterfaceName);
    }

#if CISST_MTS_HAS_ICE
    // Make sure all interfaces created so far are registered to the GCM.
    if (this->ProcessName == clientProcessName) {
        if (!RegisterInterfaces(clientComponentName)) {
            CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to register client component's interfaces: " << clientComponentName << std::endl;
            return false;
        }
    }
    if (this->ProcessName == serverProcessName) {
        if (!RegisterInterfaces(serverComponentName)) {
            CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to register server component's interfaces: " << serverComponentName << std::endl;
            return false;
        }
    }

    // We only check the validity of arguments (e.g., if the client component
    // specified actually exists) since the global component manager will do.
    //
    // Connect() can be called by two different processes: either by a client
    // process or by a server process. Whichever calls Connect(), the connection
    // result is identical.
    bool isConnectRequestedByClientProcess;

    // If this local component manager has a client component
    if (this->ProcessName == clientProcessName) {
        isConnectRequestedByClientProcess = true;
    }
    // If this local component manager has a server component
    else if (this->ProcessName == serverProcessName) {
        isConnectRequestedByClientProcess = false;
    }
    // This should not be the case: two external component cannot be connected.
    else {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: cannot connect two external components: "
                                 << serverProcessName << ":" << serverComponentName << ":" << serverInterfaceName
                                 << " to "
                                 << clientProcessName << ":" << clientComponentName << ":" << clientInterfaceName
                                 << " on process " << this->ProcessName << std::endl;
        return false;
    }

    // To support bi-directional connection, retry connection untill success up
    // to 10 seconds.
    //
    // An example of the bi-directional connection:
    // The component A has a required interface that connects to a provided interface in
    // the component B and the component B also has a required interface that
    // needs to connect to a provided interface in the component A.
    unsigned int count = 1;
    ConnectionIDType connectionID = InvalidConnectionID;

    while (count <= retryCount) {
        // Inform the global component manager of a new connection being established.
        connectionID = ManagerGlobal->Connect(this->ProcessName,
            clientProcessName, clientComponentName, clientInterfaceName,
            serverProcessName, serverComponentName, serverInterfaceName);
        if (connectionID == InvalidConnectionID) {
            CMN_LOG_CLASS_INIT_ERROR << "Connect: Waiting for connection to be established.... Retrying "
                                     << count++ << "/" << retryCount << std::endl;
            osaSleep(1.0 * cmn_s);
        } else {
            break;
        }
    }

    if (connectionID == InvalidConnectionID) {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to get new connection id: "
                                 << mtsManagerGlobal::GetInterfaceUID(clientProcessName, clientComponentName, clientInterfaceName)
                                 << " - "
                                 << mtsManagerGlobal::GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceName)
                                 << std::endl;
        return false;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "Connect: new connection id: REMOTE (" << connectionID << ") for "
                               << mtsManagerGlobal::GetInterfaceUID(clientProcessName, clientComponentName, clientInterfaceName)
                               << " - "
                               << mtsManagerGlobal::GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceName)
                               << std::endl;

    // At this point, both server and client process have the identical set of
    // components because the GCM has created proxy components and interfaces
    // as needed.

    // If client process calls Connect(),
    // - Create a network proxy to provide services for server component proxy
    //   (of type mtsComponentInterfaceProxyServer).
    // - Register its access(endpoint) information to the GCM.
    // - Let server process begin connection process via the GCM.
    // - Inform the GCM that the connection is successfully established.
    if (isConnectRequestedByClientProcess) {
        if (!ConnectClientSideInterface(mtsDescriptionConnection(
                clientProcessName, clientComponentName, clientInterfaceName,
                serverProcessName, serverComponentName, serverInterfaceName, connectionID)))
        {
            CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to connect at client process \"" << clientProcessName << "\"" << std::endl;

            if (!Disconnect(connectionID)) {
                CMN_LOG_CLASS_INIT_ERROR << "Connect: clean up error - failed to disconnect connection \"" << connectionID << "\"" << std::endl;
            }
            return false;
        }
    }
    // If server process calls Connect(), let client process initiate connection
    // process via the GCM.
    else {
        if (!ManagerGlobal->InitiateConnect(connectionID)) {
            CMN_LOG_CLASS_INIT_ERROR << "Connect: Failed to initiate connection" << std::endl;

            if (!Disconnect(connectionID)) {
                CMN_LOG_CLASS_INIT_ERROR << "Connect: clean up error - failed to disconnect connection \"" << connectionID << "\"" << std::endl;
            }
            return false;
        }
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "Connect: successfully established remote connection: "
                               << mtsManagerGlobal::GetInterfaceUID(clientProcessName, clientComponentName, clientInterfaceName)
                               << " - "
                               << mtsManagerGlobal::GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceName)
                               << std::endl;
#else
    CMN_LOG_CLASS_INIT_ERROR << "Connect: CISST_MTS_HAS_ICE is false, so could not make network connection: "
                             << mtsManagerGlobal::GetInterfaceUID(clientProcessName, clientComponentName, clientInterfaceName)
                             << " - "
                             << mtsManagerGlobal::GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceName)
                             << std::endl;
#endif
    return true;
}

bool mtsManagerLocal::Disconnect(const ConnectionIDType connectionID)
{
    // If GCM is not connected, don't reqeust disconnection
    if (!IsGCMActive()) {
        CMN_LOG_CLASS_RUN_VERBOSE << "Disconnect: GCM disconnected -- Disconnection request ignored (connection id [ " << connectionID << " ])" << std::endl;
        return true;
    }

    bool success = ManagerGlobal->Disconnect(connectionID);
    if (IsGCMActive()) { // Connection to GCM can be disconnected while executing the line above
        if (!success) {
            CMN_LOG_CLASS_RUN_ERROR << "Disconnect: disconnection request failed: connection id [ " << connectionID << " ]" << std::endl;
            return false;
        }
    }

    return true;
}

// This should probably be split to functions such as DisconnectSetup and DisconnectNotify.
bool mtsManagerLocal::Disconnect(const std::string & clientComponentName, const std::string & clientInterfaceName,
                                 const std::string & serverComponentName, const std::string & serverInterfaceName)
{
    if (!IsGCMActive()) {
        CMN_LOG_CLASS_RUN_VERBOSE << "Disconnect: GCM disconnected -- disconnection request ignored: \""
            << mtsManagerGlobal::GetInterfaceUID(ProcessName, clientComponentName, clientInterfaceName)
            << " - "
            << mtsManagerGlobal::GetInterfaceUID(ProcessName, serverComponentName, serverInterfaceName)
            << std::endl;
        return true;
    }

    bool success = ManagerGlobal->Disconnect(
        ProcessName, clientComponentName, clientInterfaceName,
        ProcessName, serverComponentName, serverInterfaceName);

    if (IsGCMActive()) { // Connection to GCM can be disconnected while executing the line above
        if (!success) {
            CMN_LOG_CLASS_RUN_ERROR << "Disconnect: disconnection request failed: \""
                << mtsManagerGlobal::GetInterfaceUID(ProcessName, clientComponentName, clientInterfaceName)
                << " - "
                << mtsManagerGlobal::GetInterfaceUID(ProcessName, serverComponentName, serverInterfaceName)
                << std::endl;
            return false;
        }
    }

    return true;
}

void CISST_DEPRECATED mtsManagerLocal::ToStream(std::ostream & CMN_UNUSED(outputStream)) const
{
#if 0
    TaskMapType::const_iterator taskIterator = TaskMap.begin();
    const TaskMapType::const_iterator taskEndIterator = TaskMap.end();
    outputStream << "List of tasks: name and address" << std::endl;
    for (; taskIterator != taskEndIterator; ++taskIterator) {
        outputStream << "  Task: " << taskIterator->first << ", address: " << taskIterator->second << std::endl;
    }
    DeviceMapType::const_iterator deviceIterator = DeviceMap.begin();
    const DeviceMapType::const_iterator deviceEndIterator = DeviceMap.end();
    outputStream << "List of devices: name and address" << std::endl;
    for (; deviceIterator != deviceEndIterator; ++deviceIterator) {
        outputStream << "  Device: " << deviceIterator->first << ", adress: " << deviceIterator->second << std::endl;
    }
    AssociationSetType::const_iterator associationIterator = AssociationSet.begin();
    const AssociationSetType::const_iterator associationEndIterator = AssociationSet.end();
    outputStream << "Associations: task::requiredInterface associated to device/task::requiredInterface" << std::endl;
    for (; associationIterator != associationEndIterator; ++associationIterator) {
        outputStream << "  " << associationIterator->first.first << "::" << associationIterator->first.second << std::endl
                     << "  -> " << associationIterator->second.first << "::" << associationIterator->second.second << std::endl;
    }
#endif
}

void CISST_DEPRECATED mtsManagerLocal::ToStreamDot(std::ostream & CMN_UNUSED(outputStream)) const
{
#if 0
    std::vector<std::string> interfacesProvidedAvailable, requiredInterfacesAvailable;
    std::vector<std::string>::const_iterator stringIterator;
    unsigned int clusterNumber = 0;
    // dot header
    outputStream << "/* Automatically generated by cisstMultiTask, mtsTaskManager::ToStreamDot.\n"
                 << "   Use Graphviz utility \"dot\" to generate a graph of tasks/devices interactions. */"
                 << std::endl;
    outputStream << "digraph mtsTaskManager {" << std::endl;
    // create all nodes for tasks
    TaskMapType::const_iterator taskIterator = TaskMap.begin();
    const TaskMapType::const_iterator taskEndIterator = TaskMap.end();
    for (; taskIterator != taskEndIterator; ++taskIterator) {
        outputStream << "subgraph cluster" << clusterNumber << "{" << std::endl
                     << "node[style=filled,color=white,shape=box];" << std::endl
                     << "style=filled;" << std::endl
                     << "color=lightgrey;" << std::endl;
        clusterNumber++;
        outputStream << taskIterator->first
                     << " [label=\"Task:\\n" << taskIterator->first << "\"];" << std::endl;
        interfacesProvidedAvailable = taskIterator->second->GetNamesOfInterfaceProvideds();
        for (stringIterator = interfacesProvidedAvailable.begin();
             stringIterator != interfacesProvidedAvailable.end();
             stringIterator++) {
            outputStream << taskIterator->first << "interfaceProvided" << *stringIterator
                         << " [label=\"Provided interface:\\n" << *stringIterator << "\"];" << std::endl;
            outputStream << taskIterator->first << "interfaceProvided" << *stringIterator
                         << "->" << taskIterator->first << ";" << std::endl;
        }
        requiredInterfacesAvailable = taskIterator->second->GetNamesOfInterfacesRequired();
        for (stringIterator = requiredInterfacesAvailable.begin();
             stringIterator != requiredInterfacesAvailable.end();
             stringIterator++) {
            outputStream << taskIterator->first << "requiredInterface" << *stringIterator
                         << " [label=\"Required interface:\\n" << *stringIterator << "\"];" << std::endl;
            outputStream << taskIterator->first << "->"
                         << taskIterator->first << "requiredInterface" << *stringIterator << ";" << std::endl;
        }
        outputStream << "}" << std::endl;
    }
    // create all nodes for devices
    DeviceMapType::const_iterator deviceIterator = DeviceMap.begin();
    const DeviceMapType::const_iterator deviceEndIterator = DeviceMap.end();
    for (; deviceIterator != deviceEndIterator; ++deviceIterator) {
        outputStream << "subgraph cluster" << clusterNumber << "{" << std::endl
                     << "node[style=filled,color=white,shape=box];" << std::endl
                     << "style=filled;" << std::endl
                     << "color=lightgrey;" << std::endl;
        clusterNumber++;
        outputStream << deviceIterator->first
                     << " [label=\"Device:\\n" << deviceIterator->first << "\"];" << std::endl;
        interfacesProvidedAvailable = deviceIterator->second->GetNamesOfInterfaceProvideds();
        for (stringIterator = interfacesProvidedAvailable.begin();
             stringIterator != interfacesProvidedAvailable.end();
             stringIterator++) {
            outputStream << deviceIterator->first << "interfaceProvided" << *stringIterator
                         << " [label=\"Provided interface:\\n" << *stringIterator << "\"];" << std::endl;
            outputStream << deviceIterator->first << "interfaceProvided" << *stringIterator
                         << "->" << deviceIterator->first << ";" << std::endl;
        }
        outputStream << "}" << std::endl;
    }
    // create edges
    AssociationSetType::const_iterator associationIterator = AssociationSet.begin();
    const AssociationSetType::const_iterator associationEndIterator = AssociationSet.end();
    for (; associationIterator != associationEndIterator; ++associationIterator) {
        outputStream << associationIterator->first.first << "requiredInterface" << associationIterator->first.second
                     << "->"
                     << associationIterator->second.first << "interfaceProvided" << associationIterator->second.second
                     << ";" << std::endl;
    }
    // end of file
    outputStream << "}" << std::endl;
#endif
}




bool mtsManagerLocal::RegisterInterfaces(mtsComponent * component)
{
    if (!component) {
        return false;
    }

    const std::string componentName = component->GetName();
    std::vector<std::string> interfaceNames;

    mtsInterfaceProvided * interfaceProvided;
    interfaceNames = component->GetNamesOfInterfacesProvided();
    for (size_t i = 0; i < interfaceNames.size(); ++i) {
        interfaceProvided = component->GetInterfaceProvided(interfaceNames[i]);
        if (!interfaceProvided) {
            CMN_LOG_CLASS_INIT_ERROR << "RegisterInterfaces: NULL provided interface detected: " << interfaceNames[i] << std::endl;
            return false;
        } else {
            if (ManagerGlobal->FindInterfaceProvidedOrOutput(ProcessName, componentName, interfaceNames[i])) {
                continue;
            }
        }
        if (!ManagerGlobal->AddInterfaceProvidedOrOutput(ProcessName, componentName, interfaceNames[i])) {
            CMN_LOG_CLASS_INIT_ERROR << "RegisterInterfaces: failed to add provided interface: "
                                     << componentName << ":" << interfaceNames[i] << std::endl;
            return false;
        }
        if (this->GetConfiguration() != LCM_CONFIG_STANDALONE) {
            osaSleep(0.1 * cmn_s);  // PK TEMP until blocking commands supported
        }
    }
    mtsInterfaceOutput * interfaceOutput;
    interfaceNames = component->GetNamesOfInterfacesOutput();
    for (size_t i = 0; i < interfaceNames.size(); ++i) {
        interfaceOutput = component->GetInterfaceOutput(interfaceNames[i]);
        if (!interfaceOutput) {
            CMN_LOG_CLASS_INIT_ERROR << "RegisterInterfaces: NULL output interface detected: " << interfaceNames[i] << std::endl;
            return false;
        } else {
            if (ManagerGlobal->FindInterfaceProvidedOrOutput(ProcessName, componentName, interfaceNames[i])) {
                continue;
            }
        }
        if (!ManagerGlobal->AddInterfaceProvidedOrOutput(ProcessName, componentName, interfaceNames[i])) {
            CMN_LOG_CLASS_INIT_ERROR << "RegisterInterfaces: failed to add output interface: "
                                     << componentName << ":" << interfaceNames[i] << std::endl;
            return false;
        }
        if (this->GetConfiguration() != LCM_CONFIG_STANDALONE) {
            osaSleep(0.1 * cmn_s);  // PK TEMP until blocking commands supported
        }
    }

    mtsInterfaceRequired * interfaceRequired;
    interfaceNames = component->GetNamesOfInterfacesRequired();
    for (size_t i = 0; i < interfaceNames.size(); ++i) {
        interfaceRequired = component->GetInterfaceRequired(interfaceNames[i]);
        if (!interfaceRequired) {
            CMN_LOG_CLASS_INIT_ERROR << "RegisterInterfaces: NULL required interface detected: " << interfaceNames[i] << std::endl;
            return false;
        } else {
            if (ManagerGlobal->FindInterfaceRequiredOrInput(ProcessName, componentName, interfaceNames[i])) {
                continue;
            }
        }
        if (!ManagerGlobal->AddInterfaceRequiredOrInput(ProcessName, componentName, interfaceNames[i])) {
            CMN_LOG_CLASS_INIT_ERROR << "RegisterInterfaces: failed to add required interface: "
                                     << componentName << ":" << interfaceNames[i] << std::endl;
            return false;
        }
        if (this->GetConfiguration() != LCM_CONFIG_STANDALONE) {
            osaSleep(0.1 * cmn_s);  // PK TEMP until blocking commands supported
        }
    }

    mtsInterfaceInput * interfaceInput;
    interfaceNames = component->GetNamesOfInterfacesInput();
    for (size_t i = 0; i < interfaceNames.size(); ++i) {
        interfaceInput = component->GetInterfaceInput(interfaceNames[i]);
        if (!interfaceInput) {
            CMN_LOG_CLASS_INIT_ERROR << "RegisterInterfaces: NULL input interface detected: " << interfaceNames[i] << std::endl;
            return false;
        } else {
            if (ManagerGlobal->FindInterfaceRequiredOrInput(ProcessName, componentName, interfaceNames[i])) {
                continue;
            }
        }
        if (!ManagerGlobal->AddInterfaceRequiredOrInput(ProcessName, componentName, interfaceNames[i])) {
            CMN_LOG_CLASS_INIT_ERROR << "RegisterInterfaces: failed to add input interface: "
                                     << componentName << ":" << interfaceNames[i] << std::endl;
            return false;
        }
        if (this->GetConfiguration() != LCM_CONFIG_STANDALONE) {
            osaSleep(0.1 * cmn_s);  // PK TEMP until blocking commands supported
        }
    }

    return true;
}


bool mtsManagerLocal::RegisterInterfaces(const std::string & componentName)
{
    mtsComponent * component = GetComponent(componentName);
    if (!component) {
        CMN_LOG_CLASS_INIT_ERROR << "RegisterInterfaces: invalid component name: \"" << componentName << "\"" << std::endl;
        return false;
    }

    return RegisterInterfaces(component);
}



bool mtsManagerLocal::Disconnect(
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceName)
{
    if (clientProcessName == serverProcessName)
        return Disconnect(clientComponentName, clientInterfaceName,
                          serverComponentName, serverInterfaceName);
#if CISST_MTS_HAS_ICE
    bool success = ManagerGlobal->Disconnect(
        clientProcessName, clientComponentName, clientInterfaceName,
        serverProcessName, serverComponentName, serverInterfaceName);

    if (success)
        CMN_LOG_CLASS_INIT_VERBOSE << "Disconnect: successfully disconnected." << std::endl;
    else
        CMN_LOG_CLASS_INIT_ERROR << "Disconnect: disconnection failed." << std::endl;

    return success;
#else
    CMN_LOG_CLASS_INIT_ERROR << "Cannot disconnect components in different processes because CISST_MTS_HAS_ICE is false" << std::endl;
    return false;
#endif

}

bool mtsManagerLocal::GetInterfaceProvidedDescription(
    const std::string & serverComponentName, const std::string & interfaceName,
    mtsInterfaceProvidedDescription & interfaceProvidedDescription,
    const std::string & CMN_UNUSED(listenerID))
{
    // Get component specified
    mtsComponent * component = GetComponent(serverComponentName);
    if (!component) {
        CMN_LOG_CLASS_INIT_ERROR << "GetInterfaceProvidedDescription: no component \""
                                 << serverComponentName << "\" found in process: \"" << ProcessName << "\"" << std::endl;
        return false;
    }

    // Get provided interface specified
    mtsInterfaceProvided * interfaceProvided = component->GetInterfaceProvided(interfaceName);
    if (!interfaceProvided) {
        CMN_LOG_CLASS_INIT_ERROR << "GetInterfaceProvidedDescription: no provided interface \""
                                 << interfaceName << "\" found in component \"" << serverComponentName << "\"" << std::endl;
        return false;
    }

    // Extract complete information about all commands and event generators in
    // the provided interface specified. Argument prototypes are serialized.
    interfaceProvidedDescription.InterfaceName = interfaceName;
    if (!interfaceProvided->GetDescription(interfaceProvidedDescription)) {
        CMN_LOG_CLASS_INIT_ERROR << "GetInterfaceProvidedDescription: failed to get complete information of \""
                                 << interfaceName << "\" found in component \"" << serverComponentName << "\"" << std::endl;
        return false;
    }

    return true;
}

bool mtsManagerLocal::GetInterfaceRequiredDescription(
    const std::string & componentName, const std::string & requiredInterfaceName,
    mtsInterfaceRequiredDescription & requiredInterfaceDescription,
    const std::string & CMN_UNUSED(listenerID))
{
    // Get the component instance specified
    mtsComponent * component = GetComponent(componentName);
    if (!component) {
        CMN_LOG_CLASS_INIT_ERROR << "GetInterfaceRequiredDescription: no component \""
                                 << componentName << "\" found in local component manager \"" << ProcessName << "\"" << std::endl;
        return false;
    }

    // Get the provided interface specified
    mtsInterfaceRequired * requiredInterface = component->GetInterfaceRequired(requiredInterfaceName);
    if (!requiredInterface) {
        CMN_LOG_CLASS_INIT_ERROR << "GetInterfaceRequiredDescription: no provided interface \""
                                 << requiredInterfaceName << "\" found in component \"" << componentName << "\"" << std::endl;
        return false;
    }

    // Extract complete information about all functions and event handlers in
    // a required interface. Argument prototypes are fetched with serialization.
    requiredInterfaceDescription.InterfaceName = requiredInterfaceName;
    requiredInterface->GetDescription(requiredInterfaceDescription);

    return true;
}

#if CISST_MTS_HAS_ICE
bool mtsManagerLocal::CreateComponentProxy(const std::string & componentProxyName, const std::string & CMN_UNUSED(listenerID))
#else
bool mtsManagerLocal::CreateComponentProxy(const std::string & CMN_UNUSED(componentProxyName), const std::string & CMN_UNUSED(listenerID))
#endif
{
#if CISST_MTS_HAS_ICE
    // Create a component proxy
    mtsComponent * newComponent = new mtsComponentProxy(componentProxyName);

    bool success = AddComponent(newComponent);
    if (!success) {
        delete newComponent;
        return false;
    }

    return true;
#else
    CMN_LOG_CLASS_INIT_ERROR << "CreateComponentProxy not implemented for standalone configuration." << std::endl;
    return false;
#endif
}

bool mtsManagerLocal::RemoveComponentProxy(const std::string & componentProxyName, const std::string & CMN_UNUSED(listenerID))
{
    return RemoveComponent(componentProxyName, false);
}

bool mtsManagerLocal::CreateInterfaceProvidedProxy(
#if CISST_MTS_HAS_ICE
    const std::string & serverComponentProxyName,
    const mtsInterfaceProvidedDescription & interfaceProvidedDescription,
    const std::string & CMN_UNUSED(listenerID))
#else
    const std::string & CMN_UNUSED(serverComponentProxyName),
    const mtsInterfaceProvidedDescription & CMN_UNUSED(interfaceProvidedDescription),
    const std::string & CMN_UNUSED(listenerID))
#endif
{
#if CISST_MTS_HAS_ICE
    const std::string interfaceProvidedName = interfaceProvidedDescription.InterfaceName;

    // Get current component proxy. If none, returns false because a component
    // proxy should be created before an interface proxy is created.
    mtsComponent * serverComponent = GetComponent(serverComponentProxyName);
    if (!serverComponent) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: "
                                 << "no component proxy found: " << serverComponentProxyName << std::endl;
        return false;
    }

    // Downcasting to its original type
    mtsComponentProxy * serverComponentProxy = dynamic_cast<mtsComponentProxy*>(serverComponent);
    if (!serverComponentProxy) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: "
                                 << "invalid component proxy: " << serverComponentProxyName << std::endl;
        return false;
    }

    // Create provided interface proxy.
    if (!serverComponentProxy->CreateInterfaceProvidedProxy(interfaceProvidedDescription)) {
        CMN_LOG_CLASS_INIT_VERBOSE << "CreateInterfaceProvidedProxy: "
                                   << "failed to create Provided interface proxy: " << serverComponentProxyName << ":"
                                   << interfaceProvidedName << std::endl;
        return false;
    }

    // Inform the global component manager of the creation of provided interface proxy
    if (!ManagerGlobal->AddInterfaceProvidedOrOutput(ProcessName, serverComponentProxyName, interfaceProvidedName)) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: "
                                 << "failed to add provided interface proxy to Global Component Manager: "
                                 << ProcessName << ":" << serverComponentProxyName << ":" << interfaceProvidedName << std::endl;
        return false;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "CreateInterfaceProvidedProxy: "
                               << "successfully created Provided interface proxy: " << serverComponentProxyName << ":"
                               << interfaceProvidedName << std::endl;
    return true;
#else
    CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy not implemented for standalone configuration." << std::endl;
    return false;
#endif
}


bool mtsManagerLocal::CreateInterfaceRequiredProxy(
#if CISST_MTS_HAS_ICE
    const std::string & clientComponentProxyName,
    const mtsInterfaceRequiredDescription & requiredInterfaceDescription,
    const std::string & CMN_UNUSED(listenerID))
#else
    const std::string & CMN_UNUSED(clientComponentProxyName),
    const mtsInterfaceRequiredDescription & CMN_UNUSED(requiredInterfaceDescription),
    const std::string & CMN_UNUSED(listenerID))
#endif
{
#if CISST_MTS_HAS_ICE
    const std::string requiredInterfaceName = requiredInterfaceDescription.InterfaceName;

    // Get current component proxy. If none, returns false because a component
    // proxy should be created before an interface proxy is created.
    mtsComponent * clientComponent = GetComponent(clientComponentProxyName);
    if (!clientComponent) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceRequiredProxy: "
                                 << "no component proxy found: " << clientComponentProxyName << std::endl;
        return false;
    }

    // Downcasting to its orginal type
    mtsComponentProxy * clientComponentProxy = dynamic_cast<mtsComponentProxy*>(clientComponent);
    if (!clientComponentProxy) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceRequiredProxy: "
                                 << "invalid component proxy: " << clientComponentProxyName << std::endl;
        return false;
    }

    // Create required interface proxy
    if (!clientComponentProxy->CreateInterfaceRequiredProxy(requiredInterfaceDescription)) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceRequiredProxy: "
                                 << "failed to create required interface proxy: " << clientComponentProxyName << ":"
                                 << requiredInterfaceName << std::endl;
        return false;
    }

    // Inform the global component manager of the creation of provided interface proxy
    if (!ManagerGlobal->AddInterfaceRequiredOrInput(ProcessName, clientComponentProxyName, requiredInterfaceName)) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceRequiredProxy: "
                                 << "failed to add required interface proxy to Global Component Manager: "
                                 << ProcessName << ":" << clientComponentProxyName << ":" << requiredInterfaceName << std::endl;
        return false;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "CreateInterfaceRequiredProxy: "
                               << "successfully created required interface proxy: " << clientComponentProxyName << ":"
                               << requiredInterfaceName << std::endl;
    return true;
#else
    CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceRequiredProxy not implemented for standalone configuration." << std::endl;
    return false;
#endif
}

bool mtsManagerLocal::RemoveInterfaceRequired(const std::string & componentName, const std::string & interfaceName)
{
    mtsComponent * component = GetComponent(componentName);
    if (!component) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveInterfaceRequired: can't find client component: " << componentName << std::endl;
        return false;
    }

    // Check total number of required interfaces using (connecting to) this provided interface.
    mtsInterfaceRequired * interfaceRequired = component->GetInterfaceRequired(interfaceName);
    if (!interfaceRequired) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveInterfaceRequired: no required interface found: " << interfaceName << std::endl;
        return false;
    }

    // Remove required interface
    if (!component->RemoveInterfaceRequired(interfaceName)) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveInterfaceRequired: failed to remove provided interface proxy: " << interfaceName << std::endl;
        return false;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "RemoveInterfaceRequired: removed provided interface: "
                               << componentName << ":" << interfaceName << std::endl;

    return true;
}

bool mtsManagerLocal::RemoveInterfaceProvided(const std::string & componentName, const std::string & interfaceName)
{
    mtsComponent * component = GetComponent(componentName);
    if (!component) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveInterfaceProvided: can't find client component: " << componentName << std::endl;
        return false;
    }

    // Check total number of required interfaces using (connecting to) this provided interface.
    mtsInterfaceProvided * interfaceProvided = component->GetInterfaceProvided(interfaceName);
    if (!interfaceProvided) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveInterfaceProvided: no provided interface found: " << interfaceName << std::endl;
        return false;
    }

    // Remove provided interface only when user counter is zero.
    if (interfaceProvided->UserCounter > 0) {
        --interfaceProvided->UserCounter;
    }
    if (interfaceProvided->UserCounter == 0) {
        // Remove provided interface
        if (!component->RemoveInterfaceProvided(interfaceName)) {
            CMN_LOG_CLASS_INIT_ERROR << "RemoveInterfaceProvided: failed to remove provided interface proxy: " << interfaceName << std::endl;
            return false;
        }

        CMN_LOG_CLASS_INIT_VERBOSE << "RemoveInterfaceProvided: removed provided interface: "
                                   << componentName << ":" << interfaceName << std::endl;
    } else {
        CMN_LOG_CLASS_INIT_VERBOSE << "RemoveInterfaceProvided: decreased active user counter. current counter: "
                                   << interfaceProvided->UserCounter << std::endl;
    }

    return true;
}

bool mtsManagerLocal::RemoveInterfaceProvidedProxy(
#if CISST_MTS_HAS_ICE
    const std::string & componentProxyName, const std::string & interfaceProvidedProxyName, const std::string & CMN_UNUSED(listenerID))
#else
    const std::string & CMN_UNUSED(componentProxyName), const std::string & CMN_UNUSED(interfaceProvidedProxyName), const std::string & CMN_UNUSED(listenerID))
#endif
{
#if CISST_MTS_HAS_ICE
    mtsComponent * clientComponent = GetComponent(componentProxyName);
    if (!clientComponent) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveInterfaceProvidedProxy: can't find client component: " << componentProxyName << std::endl;
        return false;
    }

    mtsComponentProxy * clientComponentProxy = dynamic_cast<mtsComponentProxy*>(clientComponent);
    if (!clientComponentProxy) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveInterfaceProvidedProxy: client component is not a proxy: " << componentProxyName << std::endl;
        return false;
    }

    // Check a number of required interfaces using (connecting to) this provided interface.
    mtsInterfaceProvided * interfaceProvidedProxy = clientComponentProxy->GetInterfaceProvided(interfaceProvidedProxyName);
    if (!interfaceProvidedProxy) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveInterfaceProvidedProxy: can't get provided interface proxy.: " << interfaceProvidedProxyName << std::endl;
        return false;
    }

    // Remove provided interface proxy only when number of end users is zero.
    if (interfaceProvidedProxy->GetNumberOfEndUsers() == 0) {
        // Remove provided interface from component proxy.
        if (!clientComponentProxy->RemoveInterfaceProvidedProxy(interfaceProvidedProxyName)) {
            CMN_LOG_CLASS_INIT_ERROR << "RemoveInterfaceProvidedProxy: failed to remove provided interface proxy: " << interfaceProvidedProxyName << std::endl;
            return false;
        }

        CMN_LOG_CLASS_INIT_VERBOSE << "RemoveInterfaceProvidedProxy: removed provided interface: "
                                   << componentProxyName << ":" << interfaceProvidedProxyName << std::endl;
    } else {
        CMN_LOG_CLASS_INIT_VERBOSE << "RemoveInterfaceProvidedProxy: decreased active user counter. current counter: "
                                   << interfaceProvidedProxy->UserCounter << std::endl;
    }

    return true;
#else
    CMN_LOG_CLASS_INIT_ERROR << "RemovedInterfaceProvidedProxy not implemented for standalone configuration." << std::endl;
    return false;
#endif
}

bool mtsManagerLocal::RemoveInterfaceRequiredProxy(
#if CISST_MTS_HAS_ICE
    const std::string & componentProxyName, const std::string & requiredInterfaceProxyName, const std::string & CMN_UNUSED(listenerID))
#else
    const std::string & CMN_UNUSED(componentProxyName), const std::string & CMN_UNUSED(requiredInterfaceProxyName), const std::string & CMN_UNUSED(listenerID))
#endif
{
#if CISST_MTS_HAS_ICE
    mtsComponent * serverComponent = GetComponent(componentProxyName);
    if (!serverComponent) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveInterfaceRequiredProxy: can't find proxy component: " << componentProxyName << std::endl;
        return false;
    }

    mtsComponentProxy * serverComponentProxy = dynamic_cast<mtsComponentProxy*>(serverComponent);
    if (!serverComponentProxy) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveInterfaceRequiredProxy: non-proxy component: " << componentProxyName << std::endl;
        return false;
    }

    // Remove required interface from component proxy.
    if (!serverComponentProxy->RemoveInterfaceRequiredProxy(requiredInterfaceProxyName)) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveInterfaceRequiredProxy: failed to remove required interface proxy: " << requiredInterfaceProxyName << std::endl;
        return false;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "RemoveInterfaceRequiredProxy: removed required interface: "
                               << componentProxyName << ":" << requiredInterfaceProxyName << std::endl;

    return true;
#else
    CMN_LOG_CLASS_INIT_WARNING << "RemoveInterfaceRequiredProxy: not yet implement for !CISST_MTS_HAS_ICE" << std::endl;
    return false;
#endif
}

void mtsManagerLocal::SetIPAddress(void)
{
    // Fetch all ip addresses available on this machine.
    std::vector<std::string> ipAddresses;
    int ret = osaSocket::GetLocalhostIP(ipAddresses);
    if (ret == 0) {
        CMN_LOG_CLASS_INIT_WARNING << "Failed to get local host ip address" << std::endl;
        return;
    }

    for (size_t i = 0; i < ipAddresses.size(); ++i) {
        CMN_LOG_CLASS_INIT_VERBOSE << "IP detected: (" << i + 1 << ") " << ipAddresses[i] << std::endl;
    }

    ProcessIPList.insert(ProcessIPList.begin(), ipAddresses.begin(), ipAddresses.end());
}

bool mtsManagerLocal::SetInterfaceProvidedProxyAccessInfo(const ConnectionIDType connectionID, const std::string & endpointInfo)
{
    return ManagerGlobal->SetInterfaceProvidedProxyAccessInfo(connectionID, endpointInfo);
}

#if CISST_MTS_HAS_ICE
bool mtsManagerLocal::ConnectServerSideInterface(const mtsDescriptionConnection & description, const std::string & CMN_UNUSED(listenerID))
#else
bool mtsManagerLocal::ConnectServerSideInterface(const mtsDescriptionConnection & CMN_UNUSED(description), const std::string & CMN_UNUSED(listenerID))
#endif
{
#if CISST_MTS_HAS_ICE
    // This method is called only by the GCM to connect two local interfaces
    // -- one interface is an original interface and the other one is a proxy
    // interface -- at server side.

    int numTrial = 0;
    const int maxTrial = 10;
    const double sleepTime = 200.0 * cmn_ms;

    const ConnectionIDType connectionID           = description.ConnectionID;
    const std::string serverProcessName           = description.Server.ProcessName;
    const std::string serverComponentName         = description.Server.ComponentName;
    const std::string serverInterfaceName         = description.Server.InterfaceName;
    const std::string clientProcessName           = description.Client.ProcessName;
    const std::string clientComponentName         = description.Client.ComponentName;
    const std::string clientInterfaceName         = description.Client.InterfaceName;

    // Make sure that this is a server process.
    if (this->ProcessName != serverProcessName) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectServerSideInterface: this is not the server process: " << serverProcessName << std::endl;
        return false;
    }

    // Get actual names of components (either a client component or a server
    // component is a proxy).
    const std::string actualClientComponentName = mtsManagerGlobal::GetComponentProxyName(clientProcessName, clientComponentName);
    const std::string actualServerComponentName = serverComponentName;

    // Connect two local interfaces
    if (!ManagerComponent.Client) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectServerSideInterface: MCC not yet created" << std::endl;
        return false;
    }

    bool ret = ManagerComponent.Client->ConnectLocally(actualClientComponentName, clientInterfaceName,
                                                       actualServerComponentName, serverInterfaceName,
                                                       clientProcessName);
    if (!ret) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectServerSideInterface: ConnectLocally() failed" << std::endl;
        return false;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "ConnectServerSideInterface: successfully established local connection at server process." << std::endl;

    // Information to access (connect to) network interface proxy server.
    std::string serverEndpointInfo;
    mtsComponentProxy * clientComponentProxy;

    // Get component proxy object. Note that this process is a server process
    // and the client component is a proxy, not an original component.
    const std::string clientComponentProxyName = mtsManagerGlobal::GetComponentProxyName(clientProcessName, clientComponentName);
    mtsComponent * clientComponent = GetComponent(clientComponentProxyName);
    if (!clientComponent) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectServerSideInterface: failed to get client component: " << clientComponentProxyName << std::endl;
        goto ConnectServerSideInterfaceError;
    }
    clientComponentProxy = dynamic_cast<mtsComponentProxy *>(clientComponent);
    if (!clientComponentProxy) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectServerSideInterface: client component is not a proxy: " << clientComponentProxyName << std::endl;
        goto ConnectServerSideInterfaceError;
    }

    // Fetch access information from the global component manager to connect
    // to interface server proxy. Note that it might be possible that an provided
    // interface proxy server has not started yet or has not yet registered its
    // access information to the global component manager, and thus access
    // information is not readily available.  To handle such a case, a required
    // interface proxy client tries to fetch the information from the global
    // component manager for maxTrial times defined below.  After these trials
    // without success, this method returns false, resulting in disconnection and
    // cleaning up the current connection which has not yet been established.


    // Fecth proxy server's access information from the GCM
    while (++numTrial <= maxTrial) {
        // Try to get server proxy access information
        if (ManagerGlobal->GetInterfaceProvidedProxyAccessInfo(connectionID, serverEndpointInfo)) {
            CMN_LOG_CLASS_INIT_VERBOSE << "ConnectServerSideInterface: server proxy info (connection id: "
                << connectionID << "): " << serverEndpointInfo << std::endl;
            break;
        }

        // Wait for 1 second
        CMN_LOG_CLASS_INIT_VERBOSE << "ConnectServerSideInterface: waiting for server proxy access information ... "
                                   << numTrial << " / " << maxTrial << std::endl;
        osaSleep(sleepTime);
    }

    // If this client proxy fails to get the access information
    if (numTrial > maxTrial) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectServerSideInterface: failed to fetch information to access network proxy server" << std::endl;
        goto ConnectServerSideInterfaceError;
    }

    // Create and run required interface proxy client
    if (!UnitTestEnabled || (UnitTestEnabled && UnitTestNetworkProxyEnabled)) {
        if (!clientComponentProxy->CreateInterfaceProxyClient(
                clientInterfaceName, serverEndpointInfo, connectionID)) {
            CMN_LOG_CLASS_INIT_ERROR << "ConnectServerSideInterface: failed to create network interface proxy client"
                                     << ": " << clientComponentProxy->GetName() << std::endl;
            goto ConnectServerSideInterfaceError;
        }

        // Wait for the required interface proxy client to successfully connect to
        // provided interface proxy server.
        numTrial = 0;
        while (++numTrial <= maxTrial) {
            if (clientComponentProxy->IsActiveProxy(clientInterfaceName, false)) {
                CMN_LOG_CLASS_INIT_VERBOSE << "ConnectServerSideInterface: connected to network interface proxy server" << std::endl;
                break;
            }

            // Wait for some time
            CMN_LOG_CLASS_INIT_VERBOSE << "ConnectServerSideInterface: connecting to network interface proxy server ... "
                                       << numTrial << " / " << maxTrial << std::endl;
            osaSleep(sleepTime);
        }

        // If this client proxy didn't connect to server proxy
        if (numTrial > maxTrial) {
            CMN_LOG_CLASS_INIT_ERROR << "ConnectServerSideInterface: failed to connect to network interface proxy server" << std::endl;
            goto ConnectServerSideInterfaceError;
        }

        // Now it is guaranteed that two local connections--one at server side
        // and the other one at client side--are successfully established.
        // Event handler IDs can be updated at this moment.

        // Update event handler ID: Set event handlers' IDs in a required interface
        // proxy at server side as event generators' IDs fetched from a provided
        // interface proxy at client side.
        if (!clientComponentProxy->UpdateEventHandlerProxyID(clientComponentName, clientInterfaceName)) {
            CMN_LOG_CLASS_INIT_ERROR << "ConnectServerSideInterface: failed to update event handler proxies" << std::endl;
            goto ConnectServerSideInterfaceError;
        }
    }

    return true;

ConnectServerSideInterfaceError:
    if (!Disconnect(clientProcessName, clientComponentName, clientInterfaceName,
                    serverProcessName, serverComponentName, serverInterfaceName))
    {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectServerSideInterface: clean up (disconnect failed) error";
    }

#else
    CMN_LOG_CLASS_INIT_WARNING << "ConnectServerSideInterface called when CISST_MTS_HAS_ICE is false"
                               << std::endl;
#endif
    return false;
}

#if CISST_MTS_HAS_ICE
bool mtsManagerLocal::ConnectClientSideInterface(const mtsDescriptionConnection & description, const std::string & CMN_UNUSED(listenerID))
#else
bool mtsManagerLocal::ConnectClientSideInterface(const mtsDescriptionConnection & CMN_UNUSED(description), const std::string & CMN_UNUSED(listenerID))
#endif
{
#if CISST_MTS_HAS_ICE
    std::string endpointAccessInfo, communicatorId;
    mtsComponent *clientComponent, *serverComponent;
    mtsComponentProxy *serverComponentProxy;

    const ConnectionIDType connectionID           = description.ConnectionID;
    const std::string serverProcessName           = description.Server.ProcessName;
    const std::string serverComponentName         = description.Server.ComponentName;
    const std::string serverInterfaceName         = //description.Server.InterfaceName;
        mtsComponentProxy::GetNameOfProvidedInterfaceInstance(
            description.Server.InterfaceName, connectionID);
    const std::string clientProcessName           = description.Client.ProcessName;
    const std::string clientComponentName         = description.Client.ComponentName;
    const std::string clientInterfaceName         = description.Client.InterfaceName;

    // Get actual names of components (either client or server component should be proxy)
    const std::string actualClientComponentName = clientComponentName;
    const std::string actualServerComponentName = mtsManagerGlobal::GetComponentProxyName(serverProcessName, serverComponentName);

    // Connect two local interfaces
    if (!ManagerComponent.Client) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectClientSideInterface: MCC not yet created" << std::endl;
        return false;
    }
    bool ret = ManagerComponent.Client->ConnectLocally(actualClientComponentName, clientInterfaceName,
                                                       actualServerComponentName, serverInterfaceName);
    if (!ret) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectClientSideInterface: failed to connect two local interfaces: "
                                 << actualClientComponentName << ":" << clientInterfaceName << " - "
                                 << actualServerComponentName << ":" << description.Server.InterfaceName << std::endl;
        return false;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "ConnectClientSideInterface: successfully established local connection at client process." << std::endl;

    // Get components
    serverComponent = GetComponent(actualServerComponentName);
    if (!serverComponent) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectClientSideInterface: failed to get server component: " << actualServerComponentName << std::endl;
        goto ConnectClientSideInterfaceError;
    }
    clientComponent = GetComponent(actualClientComponentName);
    if (!clientComponent) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectClientSideInterface: failed to get client component: " << actualClientComponentName << std::endl;
        goto ConnectClientSideInterfaceError;
    }

    // Downcast to server component proxy
    serverComponentProxy = dynamic_cast<mtsComponentProxy *>(serverComponent);
    if (!serverComponentProxy) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectClientSideInterface: server component is not a proxy object: " << serverComponent->GetName() << std::endl;
        goto ConnectClientSideInterfaceError;
    }

    // Create and run network proxy server to provide services for the provided
    // interface of which name is 'serverInterfaceName.'
    if (!serverComponentProxy->FindInterfaceProxyServer(serverInterfaceName)) {
        if (!UnitTestEnabled || (UnitTestEnabled && UnitTestNetworkProxyEnabled)) {
            if (!serverComponentProxy->CreateInterfaceProxyServer(
                    serverInterfaceName, endpointAccessInfo, communicatorId))
            {
                CMN_LOG_CLASS_INIT_ERROR << "ConnectClientSideInterface: failed to create network interface proxy server: "
                                         << serverComponentProxy->GetName() << std::endl;
                goto ConnectClientSideInterfaceError;
            }
            CMN_LOG_CLASS_INIT_VERBOSE << "ConnectClientSideInterface: successfully created network interface proxy server: "
                                       << serverComponentProxy->GetName() << std::endl;
        }
    }
    // If server proxy is already running, fetch the access information
    else {
        // MJ: this should not be reached because each connection has its own provided interface instance
        CMN_ASSERT(false);

        if (!ManagerGlobal->GetInterfaceProvidedProxyAccessInfo(clientProcessName,
                serverProcessName, serverComponentName, serverInterfaceName, endpointAccessInfo))
        {
            CMN_LOG_CLASS_INIT_ERROR << "ConnectClientSideInterface: failed to fetch server proxy access information: "
                                     << mtsManagerGlobal::GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceName)
                                     << std::endl;
            goto ConnectClientSideInterfaceError;
        }
    }

    // Inform the global component manager of the access information of this
    // server proxy so that a network proxy client (of type mtsComponentInterfaceProxyClient)
    // can connect to it later.
    if (!SetInterfaceProvidedProxyAccessInfo(connectionID, endpointAccessInfo)) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectClientSideInterface: failed to set server proxy access information: "
                                 << serverInterfaceName << ", " << endpointAccessInfo << std::endl;
        goto ConnectClientSideInterfaceError;
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "ConnectClientSideInterface: successfully set server proxy access information: "
                               << serverInterfaceName << ", " << endpointAccessInfo << std::endl;

    // Make the server process begin connection process via the GCM
    if (!ManagerGlobal->ConnectServerSideInterfaceRequest(connectionID)) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectClientSideInterface: failed to connect interfaces at server process for ("
                                 << clientProcessName << ", " << clientComponentName << ", " << clientInterfaceName << ") - ("
                                 << serverProcessName << ", " << serverComponentName << ", " << serverInterfaceName << ")" << std::endl;
        goto ConnectClientSideInterfaceError;
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "ConnectClientSideInterface: successfully connected server-side interfaces" << std::endl;

    // Now it is guaranteed that two local connections--one at server side
    // and the other one at client side--are successfully established.
    // The next thing to do is to update ids of command proxies' and event
    // handlers' in a provided interface proxy.

    // At a client side, set command proxy ids in a provided interface proxy as
    // function ids fetched from a required interface proxy at a server side so
    // that an original function object at a client process can execute original
    // commands at a server process in a thread-safe way across networks.
    ret = serverComponentProxy->UpdateCommandProxyID(connectionID,
                                                     serverInterfaceName,
                                                     clientInterfaceName);
    if (!ret) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectClientSideInterface: failed to update command proxy id" << std::endl;
        goto ConnectClientSideInterfaceError;
    }

    // Sleep for unit tests which include networking
    if (UnitTestEnabled && UnitTestNetworkProxyEnabled) {
        osaSleep(3.0 * cmn_s);
    }

    // Inform the GCM that the connection is successfully established and
    // is active (network proxies are running and Ice proxy client has
    // connected to Ice proxy server).
    if (!ManagerGlobal->ConnectConfirm(connectionID)) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectClientSideInterface: failed to notify GCM of this connection" << std::endl;
        goto ConnectClientSideInterfaceError;
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "ConnectClientSideInterface: successfully informed Global Component Manager of this connection: connection id = "
                               << connectionID << std::endl;

    // Register this connection information to a provided interface proxy
    // server so that the proxy server can clean up this connection when a
    // required interface proxy client is detected as disconnected.
    if (!serverComponentProxy->AddConnectionInformation(serverInterfaceName, connectionID)) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectClientSideInterface: failed to add connection [ " << connectionID
            << " ] to \"" << serverComponentProxy->GetName() << ":" << serverInterfaceName << "\"" << std::endl;
        goto ConnectClientSideInterfaceError;
    }

    return true;

ConnectClientSideInterfaceError:
    if (!Disconnect(clientProcessName, clientComponentName, clientInterfaceName,
                    serverProcessName, serverComponentName, serverInterfaceName))
    {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectClientSideInterface: disconnect error while cleaning up connection: connection id = " << connectionID << std::endl;
    }
#else
    CMN_LOG_CLASS_INIT_WARNING << "ConnectClientSideInterface called when CISST_MTS_HAS_ICE is false"
                               << std::endl;
#endif
    return false;
}

bool mtsManagerLocal::GetGCMProcTimeSyncInfo(std::vector<std::string> &processNames, std::vector<double> &timeOffsets) {

    if (!IsGCMActive())
        return false;

    if (ManagerComponent.Server){
        ManagerComponent.Server->GetNamesOfProcesses(processNames);
        ManagerComponent.Server->InterfaceGCMCommands_GetAbsoluteTimeDiffs(processNames,timeOffsets);
        return true;
    }
    else if (ManagerComponent.Client) {
        ManagerComponent.Client->GetNamesOfProcesses(processNames);
        ManagerComponent.Client->InterfaceComponentCommands_GetAbsoluteTimeDiffs(processNames,timeOffsets);
        return true;
    }

    else
        return false;
}
