/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung
  Created on: 2009-12-07

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsManagerLocal.h>

#include <cisstCommon/cmnThrow.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstOSAbstraction/osaGetTime.h>
#include <cisstOSAbstraction/osaSocket.h>

#include <cisstMultiTask/mtsConfig.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsManagerGlobal.h>
#include <cisstMultiTask/mtsTaskContinuous.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsTaskFromCallback.h>
#include <cisstMultiTask/mtsTaskFromSignal.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>

#if CISST_MTS_HAS_ICE
#include <cisstMultiTask/mtsComponentProxy.h>
#include <cisstMultiTask/mtsManagerProxyClient.h>
#include <cisstMultiTask/mtsManagerProxyServer.h>
#endif

// Static variable definition
mtsManagerLocal * mtsManagerLocal::Instance;
osaMutex mtsManagerLocal::ConfigurationChange;

bool mtsManagerLocal::UnitTestEnabled = false;
bool mtsManagerLocal::UnitTestNetworkProxyEnabled = false;

#define DEFAULT_PROCESS_NAME "LCM"

mtsManagerLocal::mtsManagerLocal(void)
#if 0
    , JGraphSocket(osaSocket::TCP)
#endif
{
    Initialize();

    if (!UnitTestEnabled) {
        TimeServer.SetTimeOrigin();
    }

#if 0
    JGraphSocketConnected = false;

    // Try to connect to the JGraph application software (Java program).
    // Note that the JGraph application also sends event messages back via the socket,
    // though we don't currently read them. To do this, it would be best to implement
    // the TaskManager as a periodic task.
    JGraphSocketConnected = JGraphSocket.Connect("127.0.0.1", 4444);
    if (JGraphSocketConnected) {
        osaSleep(1.0 * cmn_s);  // need to wait or JGraph server will not start properly
    } else {
        CMN_LOG_CLASS_INIT_WARNING << "Failed to connect to JGraph server" << std::endl;
    }
#endif

    // In standalone mode, process name is set as DEFAULT_PROCESS_NAME by
    // default since there is only one instance of local task manager.
    ProcessName = DEFAULT_PROCESS_NAME;

    CMN_LOG_CLASS_INIT_VERBOSE << "Local component manager: STANDALONE mode" << std::endl;

    // In standalone mode, the global component manager is an instance of
    // mtsManagerGlobal that runs in the same process in which this local
    // component manager runs.
    mtsManagerGlobal * globalManager = new mtsManagerGlobal;

    // Register process name to the global component manager
    if (!globalManager->AddProcess(ProcessName)) {
        cmnThrow(std::runtime_error("Failed to register process name to the global component manager"));
    }

    // Register process object to the global component manager
    if (!globalManager->AddProcessObject(this)) {
        cmnThrow(std::runtime_error("Failed to register process object to the global component manager"));
    }

    ManagerGlobal = globalManager;
}

#if CISST_MTS_HAS_ICE
mtsManagerLocal::mtsManagerLocal(const std::string & globalComponentManagerIP,
                                 const std::string & thisProcessName,
                                 const std::string & thisProcessIP)
                                 : ProcessName(thisProcessName),
                                   GlobalComponentManagerIP(globalComponentManagerIP),
                                   ProcessIP(thisProcessIP)
{
    Initialize();

    if (!UnitTestEnabled) {
        TimeServer.SetTimeOrigin();
    }

#if 0
    JGraphSocketConnected = false;

    // Try to connect to the JGraph application software (Java program).
    // Note that the JGraph application also sends event messages back via the socket,
    // though we don't currently read them. To do this, it would be best to implement
    // the TaskManager as a periodic task.
    JGraphSocketConnected = JGraphSocket.Connect("127.0.0.1", 4444);
    if (JGraphSocketConnected) {
        osaSleep(1.0 * cmn_s);  // need to wait or JGraph server will not start properly
    } else {
        CMN_LOG_CLASS_INIT_WARNING << "Failed to connect to JGraph server" << std::endl;
    }
#endif

    // Create proxy
    if (!CreateProxy()) {
        cmnThrow(std::runtime_error("Failed to initialize global component manager proxy"));
    }

    // Set this machine's IP
    SetIPAddress();
}

bool mtsManagerLocal::CreateProxy(void)
{
    // If process ip is not specified (""), the first ip address detected is used as this process ip
    if (ProcessIP == "") {
        std::vector<std::string> ipAddresses;
        osaSocket::GetLocalhostIP(ipAddresses);
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

    // Run the proxy and connect it to the global component manager
    if (!globalComponentManagerProxy->Start(this)) {
        CMN_LOG_CLASS_INIT_ERROR << "Failed to initialize global component manager proxy" << std::endl;
        delete globalComponentManagerProxy;
        return false;
    }

    // Register process name to the global component manager.
    if (!globalComponentManagerProxy->AddProcess(ProcessName)) {
        CMN_LOG_CLASS_INIT_ERROR << "Failed to register process name to the global component manager" << std::endl;
        delete globalComponentManagerProxy;
        return false;
    }

    // In network mode, a process object doesn't need to be registered
    // to the global component manager.
    ManagerGlobal = globalComponentManagerProxy;

    CMN_LOG_CLASS_INIT_VERBOSE << "Local component manager     : NETWORK mode" << std::endl;
    CMN_LOG_CLASS_INIT_VERBOSE << "Global component manager IP : " << GlobalComponentManagerIP << std::endl;
    CMN_LOG_CLASS_INIT_VERBOSE << "This process name           : " << ProcessName << std::endl;
    CMN_LOG_CLASS_INIT_VERBOSE << "This process IP             : " << ProcessIP << std::endl;

    return true;
}
#endif

mtsManagerLocal::~mtsManagerLocal()
{
    // If ManagerGlobal is not NULL, it means Cleanup() has not been called
    // before. Thus, it needs to be called here for safe and clean termination.
    if (ManagerGlobal) {
        Cleanup();
    }
}

void mtsManagerLocal::Initialize(void)
{
    __os_init();
    ComponentMap.SetOwner(*this);
}

void mtsManagerLocal::Cleanup(void)
{
    //JGraphSocket.Close();
    //JGraphSocketConnected = false;

    if (ManagerGlobal) {
        delete ManagerGlobal;
        ManagerGlobal = 0;
    }

    ComponentMap.DeleteAll();

    __os_exit();
}

#if !CISST_MTS_HAS_ICE
mtsManagerLocal * mtsManagerLocal::GetInstance(void)
{
    if (!Instance) {
        Instance = new mtsManagerLocal;
    }

    return Instance;
}
#else
mtsManagerLocal * mtsManagerLocal::GetInstance(const std::string & globalComponentManagerIP,
                                               const std::string & thisProcessName,
                                               const std::string & thisProcessIP)
{
    if (!Instance) {
        // If no argument is specified, standalone configuration is set by default.
        if (globalComponentManagerIP == "" && thisProcessName == "" && thisProcessIP == "") {
            Instance = new mtsManagerLocal;
            CMN_LOG_INIT_WARNING << "Class mtsManagerLocal: Reconfiguration: Inter-process communication support is disabled" << std::endl;
        } else {
            Instance = new mtsManagerLocal(globalComponentManagerIP, thisProcessName, thisProcessIP);
        }

        return Instance;
    }

    if (globalComponentManagerIP == "" && thisProcessName == "" && thisProcessIP == "") {
        return Instance;
    }

    // If this local component manager has been previously configured as standalone
    // mode and GetInstance() is called again with proper arguments, the manager is
    // reconfigured as networked mode. For thread-safe transition of configuration
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
        mtsManagerLocal::ConfigurationChange.Unlock();
        return Instance;
    }

    //
    // Transfer all internal data--i.e., components and connections--from current
    // LCM to new LCM.
    //
    {
        // Transfer component information
        ComponentMapType::const_iterator it = Instance->ComponentMap.begin();
        const ComponentMapType::const_iterator itEnd = Instance->ComponentMap.end();
        for (; it != itEnd; ++it) {
            if (!newInstance->AddComponent(it->second)) {
                CMN_LOG_INIT_ERROR << "Class mtsManagerLocal: Reconfiguration: failed to trasnfer component while reconfiguring LCM: "
                                   << it->second->GetName() << std::endl;

                // Clean up new LCM object
                delete newInstance;
                mtsManagerLocal::ConfigurationChange.Unlock();

                // Keep using current LCM
                return Instance;
            } else {
                CMN_LOG_INIT_VERBOSE << "Class mtsManagerLocal: Reconfiguration: Successfully transferred component: "
                                     << it->second->GetName() << std::endl;
            }
        }
    }

    //
    // Transfer connection information
    //
    {
        // Get current connection information
        std::vector<mtsManagerGlobal::ConnectionStrings> list;
        currentGCM->GetListOfConnections(list);

        // Register all the connections established to the new GCM
        mtsManagerGlobalInterface * newGCM = newInstance->ManagerGlobal;

        int connectionId;
        std::vector<mtsManagerGlobal::ConnectionStrings>::const_iterator it = list.begin();
        const std::vector<mtsManagerGlobal::ConnectionStrings>::const_iterator itEnd = list.end();
        for (; it != itEnd; ++it) {
            connectionId = newGCM->Connect(thisProcessName,
                thisProcessName, it->ClientComponentName, it->ClientInterfaceRequiredName,
                thisProcessName, it->ServerComponentName, it->ServerInterfaceProvidedName);
            if (connectionId == -1) {
                CMN_LOG_INIT_ERROR << "Class mtsManagerLocal: Reconfiguration: failed to transfer previous connection: "
                                   << thisProcessName << ":" << it->ClientComponentName << ":" << it->ClientInterfaceRequiredName << "-"
                                   << thisProcessName << ":" << it->ServerComponentName << ":" << it->ServerInterfaceProvidedName << std::endl;
            } else {
                // Notify the GCM of successful local connection (although nothing actually needs to happen).
                if (!newGCM->ConnectConfirm(connectionId)) {
                    CMN_LOG_INIT_ERROR << "Class mtsManagerLocal: Reconfiguration: failed to notify GCM of connection: "
                                       << it->ClientComponentName << ":" << it->ClientInterfaceRequiredName << "-"
                                       << it->ServerComponentName << ":" << it->ServerInterfaceProvidedName << std::endl;

                    if (!newInstance->Disconnect(it->ClientComponentName, it->ClientInterfaceRequiredName,
                                                 it->ServerComponentName, it->ServerInterfaceProvidedName))
                    {
                        CMN_LOG_INIT_ERROR << "Class mtsManagerLocal: Reconfiguration: clean up error: disconnection failed: "
                                           << it->ClientComponentName << ":" << it->ClientInterfaceRequiredName << "-"
                                           << it->ServerComponentName << ":" << it->ServerInterfaceProvidedName << std::endl;
                    }
                }
                CMN_LOG_INIT_VERBOSE << "Class mtsManagerLocal: Reconfiguration: Successfully transferred previous connection: "
                                     << thisProcessName << ":" << it->ClientComponentName << ":" << it->ClientInterfaceRequiredName << "-"
                                     << thisProcessName << ":" << it->ServerComponentName << ":" << it->ServerInterfaceProvidedName << std::endl;
            }
        }
    }

    // Remove previous LCM instance
    delete Instance;

    // Replace singleton object
    Instance = newInstance;

    mtsManagerLocal::ConfigurationChange.Unlock();

    return Instance;
}
#endif

bool mtsManagerLocal::AddComponent(mtsComponent * component)
{
    if (!component) {
        CMN_LOG_CLASS_INIT_ERROR << "AddComponent: invalid component" << std::endl;
        return false;
    }

    const std::string componentName = component->GetName();

    // Try to register new component to the global component manager first.
    if (!ManagerGlobal->AddComponent(ProcessName, componentName)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddComponent: failed to add component: " << componentName << std::endl;
        return false;
    }

    // Register all the existing required interfaces and provided interfaces to
    // the global component manager and mark them as registered.
    if (!RegisterInterfaces(component)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddComponent: failed to register interfaces" << std::endl;
        return false;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "AddComponent: "
                               << "successfully added component to the global component manager: " << componentName << std::endl;

    bool success;
    ComponentMapChange.Lock();
    success = ComponentMap.AddItem(componentName, component);
    ComponentMapChange.Unlock();

    if (!success) {
        CMN_LOG_CLASS_INIT_ERROR << "AddComponent: "
                                 << "failed to add component to local component manager: " << componentName << std::endl;
        return false;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "AddComponent: "
                               << "successfully added component to local component manager: " << componentName << std::endl;

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
    if (!component) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveComponent: invalid argument" << std::endl;
        return false;
    }

    return RemoveComponent(component->GetName());
}

bool mtsManagerLocal::RemoveComponent(const std::string & componentName)
{
    // Notify the global component manager of the removal of this component

    if (!ManagerGlobal->RemoveComponent(ProcessName, componentName)) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveComponent: failed to remove component at global component manager: " << componentName << std::endl;
        return false;
    }

    // Get a component to be removed
    mtsComponent * component = ComponentMap.GetItem(componentName);
    if (!component) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveComponent: failed to get component to be removed: " << componentName << std::endl;
        return false;
    }

    bool success;
    ComponentMapChange.Lock();
    success = ComponentMap.RemoveItem(componentName);
    ComponentMapChange.Unlock();

    if (!success) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveComponent: failed to removed component: " << componentName << std::endl;
        return false;
    }
    else {
        delete component;

        CMN_LOG_CLASS_INIT_VERBOSE << "RemoveComponent: removed component: " << componentName << std::endl;
    }

    return true;
}

std::vector<std::string> mtsManagerLocal::GetNamesOfComponents(void) const
{
    return ComponentMap.GetNames();
}

std::vector<std::string> CISST_DEPRECATED mtsManagerLocal::GetNamesOfTasks(void) const
{
    mtsComponent * component;
    std::vector<std::string> namesOfTasks;

    ComponentMapType::const_iterator it = ComponentMap.begin();
    const ComponentMapType::const_iterator itEnd = ComponentMap.end();
    for (; it != itEnd; ++it) {
        component = dynamic_cast<mtsTask*>(it->second);
        if (component) {
            namesOfTasks.push_back(it->first);
        }
    }

    return namesOfTasks;
}

std::vector<std::string> CISST_DEPRECATED mtsManagerLocal::GetNamesOfDevices(void) const
{
    mtsComponent * component;
    std::vector<std::string> namesOfDevices;

    ComponentMapType::const_iterator it = ComponentMap.begin();
    const ComponentMapType::const_iterator itEnd = ComponentMap.end();
    for (; it != itEnd; ++it) {
        component = dynamic_cast<mtsTask*>(it->second);
        if (!component) {
            namesOfDevices.push_back(it->first);
        }
    }

    return namesOfDevices;
}

void mtsManagerLocal::GetNamesOfComponents(std::vector<std::string> & namesOfComponents) const
{
    ComponentMap.GetNames(namesOfComponents);
}

const std::string mtsManagerLocal::GetEndUserName(void) const
{
    std::string endUserName(this->ProcessName);
    endUserName += "-LocalManager";

    return endUserName;
}

void CISST_DEPRECATED mtsManagerLocal::GetNamesOfDevices(std::vector<std::string>& namesOfDevices) const
{
    mtsComponent * component;

    ComponentMapType::const_iterator it = ComponentMap.begin();
    const ComponentMapType::const_iterator itEnd = ComponentMap.end();
    for (; it != itEnd; ++it) {
        component = dynamic_cast<mtsTask*>(it->second);
        if (!component) {
            namesOfDevices.push_back(it->first);
        }
    }
}

void CISST_DEPRECATED mtsManagerLocal::GetNamesOfTasks(std::vector<std::string>& namesOfTasks) const
{
    mtsComponent * component;

    ComponentMapType::const_iterator it = ComponentMap.begin();
    const ComponentMapType::const_iterator itEnd = ComponentMap.end();
    for (; it != itEnd; ++it) {
        component = dynamic_cast<mtsTask*>(it->second);
        if (component) {
            namesOfTasks.push_back(it->first);
        }
    }
}

#if CISST_MTS_HAS_ICE
void mtsManagerLocal::GetNamesOfCommands(std::vector<std::string>& namesOfCommands,
                                         const std::string & componentName,
                                         const std::string & interfaceProvidedName,
                                         const std::string & CMN_UNUSED(listenerID))
{
    InterfaceProvidedDescription desc;
    if (!GetInterfaceProvidedDescription(componentName, interfaceProvidedName, desc)) {
        CMN_LOG_CLASS_INIT_ERROR << "GetNamesOfCommands: failed to get provided interface information: "
                                 << this->ProcessName << ":" << componentName << ":" << interfaceProvidedName << std::endl;
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
}

void mtsManagerLocal::GetNamesOfEventGenerators(std::vector<std::string>& namesOfEventGenerators,
                                                const std::string & componentName,
                                                const std::string & interfaceProvidedName,
                                                const std::string & CMN_UNUSED(listenerID))
{
    InterfaceProvidedDescription desc;
    if (!GetInterfaceProvidedDescription(componentName, interfaceProvidedName, desc)) {
        CMN_LOG_CLASS_INIT_ERROR << "GetNamesOfEventGenerators: failed to get provided interface information: "
                                 << this->ProcessName << ":" << componentName << ":" << interfaceProvidedName << std::endl;
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

void mtsManagerLocal::GetNamesOfFunctions(std::vector<std::string>& namesOfFunctions,
                                          const std::string & componentName,
                                          const std::string & requiredInterfaceName,
                                          const std::string & CMN_UNUSED(listenerID))
{
    InterfaceRequiredDescription desc;
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
}

void mtsManagerLocal::GetNamesOfEventHandlers(std::vector<std::string>& namesOfEventHandlers,
                                              const std::string & componentName,
                                              const std::string & requiredInterfaceName,
                                              const std::string & CMN_UNUSED(listenerID))
{
    InterfaceRequiredDescription desc;
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
                                              const std::string & interfaceProvidedName,
                                              const std::string & commandName,
                                              const std::string & CMN_UNUSED(listenerID))
{
    mtsComponent * component = GetComponent(componentName);
    if (!component) return;

    mtsInterfaceProvided * interfaceProvided = component->GetInterfaceProvided(interfaceProvidedName);
    if (!interfaceProvided) return;

    // Get command type
    char commandType = *commandName.c_str();
    std::string actualCommandName = commandName.substr(3, commandName.size() - 2);

    description = "Argument type: ";
    switch (commandType) {
        case 'V':
            {
                mtsCommandVoidBase * command = interfaceProvided->GetCommandVoid(actualCommandName);
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
                mtsCommandReadBase * command = interfaceProvided->GetCommandRead(actualCommandName);
                if (!command) {
                    description = "No read command found for ";
                    description += actualCommandName;
                    return;
                }
                description += command->GetArgumentClassServices()->GetName();
            }
            break;
        case 'Q':
            {
                mtsCommandQualifiedReadBase * command = interfaceProvided->GetCommandQualifiedRead(actualCommandName);
                if (!command) {
                    description = "No qualified read command found for ";
                    description += actualCommandName;
                    return;
                }
                description = "Argument1 type: ";
                description += command->GetArgument1ClassServices()->GetName();
                description += "\nArgument2 type: ";
                description += command->GetArgument2ClassServices()->GetName();
            }
            break;
        default:
            description = "Failed to get command description";
            return;
    }
}

void mtsManagerLocal::GetDescriptionOfEventGenerator(std::string & description,
                                                     const std::string & componentName,
                                                     const std::string & interfaceProvidedName,
                                                     const std::string & eventGeneratorName,
                                                     const std::string & CMN_UNUSED(listenerID))
{
    mtsComponent * component = GetComponent(componentName);
    if (!component) return;

    mtsInterfaceProvided * interfaceProvided = component->GetInterfaceProvided(interfaceProvidedName);
    if (!interfaceProvided) return;

    // Get event generator type
    char eventGeneratorType = *eventGeneratorName.c_str();
    std::string actualEventGeneratorName = eventGeneratorName.substr(3, eventGeneratorName.size() - 2);

    description = "Argument type: ";
    switch (eventGeneratorType) {
        case 'V':
            {
                mtsCommandVoidBase * eventGenerator = interfaceProvided->EventVoidGenerators.GetItem(actualEventGeneratorName);
                if (!eventGenerator) {
                    description = "No void event generator found";
                    return;
                }
                description += "(none)";
            }
            break;
        case 'W':
            {
                mtsCommandWriteBase * eventGenerator = interfaceProvided->EventWriteGenerators.GetItem(actualEventGeneratorName);
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
    char functionType = *functionName.c_str();
    std::string actualFunctionName = functionName.substr(3, functionName.size() - 2);

    description = "Resource argument type: ";
#if 0 // adeguet1 todo fix --- this is using internal values of the interface, this should be done otherwise
    switch (functionType) {
        case 'V':
            {
                mtsInterfaceRequired::CommandInfo<mtsCommandVoidBase> * function = requiredInterface->CommandPointersVoid.GetItem(actualFunctionName);
                if (!function) {
                    description = "No void function found";
                    return;
                }
                description += "(none)";
            }
            break;
        case 'W':
            {
                mtsInterfaceRequired::CommandInfo<mtsCommandWriteBase> * function = requiredInterface->CommandPointersWrite.GetItem(actualFunctionName);
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
                mtsInterfaceRequired::CommandInfo<mtsCommandReadBase> * function = requiredInterface->CommandPointersRead.GetItem(actualFunctionName);
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
                mtsInterfaceRequired::CommandInfo<mtsCommandQualifiedReadBase> * function = requiredInterface->CommandPointersQualifiedRead.GetItem(actualFunctionName);
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
                mtsCommandVoidBase * command = requiredInterface->EventHandlersVoid.GetItem(actualEventHandlerName);
                if (!command) {
                    description = "No void event handler found";
                    return;
                }
                description += "(none)";
            }
            break;
        case 'W':
            {
                mtsCommandWriteBase * command = requiredInterface->EventHandlersWrite.GetItem(actualEventHandlerName);
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

void mtsManagerLocal::GetArgumentInformation(std::string & argumentName,
                                             std::vector<std::string> & signalNames,
                                             const std::string & componentName,
                                             const std::string & interfaceProvidedName,
                                             const std::string & commandName,
                                             const std::string & CMN_UNUSED(listenerID))
{
    mtsComponent * component = GetComponent(componentName);
    if (!component) return;

    mtsInterfaceProvided * interfaceProvided = component->GetInterfaceProvided(interfaceProvidedName);
    if (!interfaceProvided) return;

    // Get argument name
    mtsCommandReadBase * command;
    char commandType = *commandName.c_str();
    std::string actualCommandName = commandName.substr(3, commandName.size() - 2);

    switch (commandType) {
        case 'V':
            argumentName = "Cannot visualize void command";
            return;
        case 'W':
            argumentName = "Cannot visualize write command";
            return;
        case 'Q':
            argumentName = "Cannot visualize q.read command";
            return;
        case 'R':
            command = interfaceProvided->GetCommandRead(actualCommandName);
            if (!command) {
                argumentName = "No read command found";
                return;
            }
            argumentName = command->GetArgumentClassServices()->GetName();
            break;
        default:
            argumentName = "Failed to get argument information";
            return;
    }

    // Get argument prototype
    const mtsGenericObject * argument = command->GetArgumentPrototype();
    if (!argument) {
        argumentName = "Failed to get argument";
        return;
    }

    // Get signal information
    const size_t signalCount = argument->GetNumberOfScalar();
    for (size_t i = 0; i < signalCount; ++i) {
        signalNames.push_back(argument->GetScalarName(i));
    }
}

void mtsManagerLocal::GetValuesOfCommand(SetOfValues & values,
                                         const std::string & componentName,
                                         const std::string & interfaceProvidedName,
                                         const std::string & commandName,
                                         const int scalarIndex,
                                         const std::string & CMN_UNUSED(listenerID))
{
    mtsComponent * component = GetComponent(componentName);
    if (!component) return;

    mtsInterfaceProvided * interfaceProvided = component->GetInterfaceProvided(interfaceProvidedName);
    if (!interfaceProvided) return;

    // Get argument name
    std::string actualCommandName = commandName.substr(3, commandName.size() - 2);
    mtsCommandReadBase * command = interfaceProvided->GetCommandRead(actualCommandName);
    if (!command) {
        CMN_LOG_CLASS_INIT_ERROR << "GetValuesOfCommand: no command found: " << actualCommandName << std::endl;
        return;
    };

    // Get argument prototype
    mtsGenericObject * argument = dynamic_cast<mtsGenericObject*>(command->GetArgumentClassServices()->Create());
    if (!argument) {
        CMN_LOG_CLASS_INIT_ERROR << "GetValuesOfCommand: failed to create temporary argument" << std::endl;
        return;
    }

    // Execute read command
    command->Execute(*argument);

    // Get current values with timestamps
    ValuePair value;
    Values valueSet;
    double relativeTime;
    values.clear();
    /*
    for (unsigned int i = 0; i < argument->GetNumberOfScalar(); ++i) {
        value.Value = argument->GetScalarAsDouble(i);
        argument->GetTimestamp(relativeTime);
        TimeServer.RelativeToAbsolute(relativeTime, value.Timestamp);

        valueSet.push_back(value);
    }
    */
    value.Value = argument->GetScalarAsDouble(scalarIndex);
    argument->GetTimestamp(relativeTime);
    TimeServer.RelativeToAbsolute(relativeTime, value.Timestamp);
    valueSet.push_back(value);
    values.push_back(valueSet);

    delete argument;
}

#endif

mtsComponent * mtsManagerLocal::GetComponent(const std::string & componentName) const
{
    return ComponentMap.GetItem(componentName);
}

mtsTask * mtsManagerLocal::GetComponentAsTask(const std::string & componentName) const
{
    mtsTask * componentTask = 0;

    mtsComponent * component = ComponentMap.GetItem(componentName);
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
    return ComponentMap.GetItem(deviceName);
}

bool mtsManagerLocal::FindComponent(const std::string & componentName) const
{
    return (GetComponent(componentName) != 0);
}

void mtsManagerLocal::CreateAll(void)
{
    mtsTask * componentTask;

    ComponentMapChange.Lock();

    ComponentMapType::const_iterator it = ComponentMap.begin();
    const ComponentMapType::const_iterator itEnd = ComponentMap.end();

    for (; it != itEnd; ++it) {
        // Skip components of mtsComponent type
        componentTask = dynamic_cast<mtsTask*>(it->second);
        if (!componentTask) continue;

        // Note that the order of dynamic casts matters to figure out
        // a type of an original task since tasks have multiple inheritance.

        // mtsTaskPeriodic type component
        componentTask = dynamic_cast<mtsTaskPeriodic*>(it->second);
        if (componentTask) {
            componentTask->Create();
            continue;
        }

        // mtsTaskFromSignal type component
        componentTask = dynamic_cast<mtsTaskFromSignal*>(it->second);
        if (componentTask) {
            componentTask->Create();
            continue;
        }

        // mtsTaskContinuous type component
        componentTask = dynamic_cast<mtsTaskContinuous*>(it->second);
        if (componentTask) {
            componentTask->Create();
            continue;
        }

        // mtsTaskFromCallback type component
        componentTask = dynamic_cast<mtsTaskFromCallback*>(it->second);
        if (componentTask) {
            componentTask->Create();
            continue;
        }
    }

    ComponentMapChange.Unlock();
}

void mtsManagerLocal::StartAll(void)
{
    // Get the current thread id in order to check if any task will use the current thread.
    // If so, start that task for last.
    const osaThreadId threadId = osaGetCurrentThreadId();

    mtsTask * componentTask;

    ComponentMapChange.Lock();

    ComponentMapType::const_iterator it = ComponentMap.begin();
    const ComponentMapType::const_iterator itEnd = ComponentMap.end();
    ComponentMapType::const_iterator itLastTask = ComponentMap.end();

    for (; it != itEnd; ++it) {
        // look for component
        componentTask = dynamic_cast<mtsTask*>(it->second);
        if (componentTask) {
            // Check if the task will use the current thread.
            if (componentTask->Thread.GetId() == threadId) {
                CMN_LOG_CLASS_INIT_WARNING << "StartAll: component \"" << it->first
                                           << "\" uses current thread, will be started last." << std::endl;
                if (itLastTask != ComponentMap.end()) {
                    CMN_LOG_CLASS_INIT_ERROR << "StartAll: found another task using current thread (\""
                                             << it->first << "\"), only first will be started (\""
                                             << itLastTask->first << "\")." << std::endl;
                } else {
                    // set iterator to last task to be started
                    itLastTask = it;
                }
            } else {
                CMN_LOG_CLASS_INIT_DEBUG << "StartAll: starting task \"" << it->first << "\"" << std::endl;
                it->second->Start();  // If task will not use current thread, start it immediately.
            }
        } else {
            CMN_LOG_CLASS_INIT_DEBUG << "StartAll: starting component \"" << it->first << "\"" << std::endl;
            it->second->Start();  // this is a component, it doesn't have a thread
        }
    }

    if (itLastTask != ComponentMap.end()) {
        itLastTask->second->Start();
    }

    ComponentMapChange.Unlock();
}


void mtsManagerLocal::KillAll(void)
{
    mtsTask *componentTask, *componentTaskTemp;

    ComponentMapChange.Lock();

    ComponentMapType::const_iterator it = ComponentMap.begin();
    const ComponentMapType::const_iterator itEnd = ComponentMap.end();
    for (; it != itEnd; ++it) {
        // mtsTaskPeriodic type component
        componentTaskTemp = dynamic_cast<mtsTaskPeriodic*>(it->second);
        if (componentTaskTemp) {
            componentTask = componentTaskTemp;
        } else {
            // mtsTaskFromSignal type component
            componentTaskTemp = dynamic_cast<mtsTaskFromSignal*>(it->second);
            if (componentTaskTemp) {
                componentTask = componentTaskTemp;
            } else {
                // mtsTaskContinuous type component
                componentTaskTemp = dynamic_cast<mtsTaskContinuous*>(it->second);
                if (componentTaskTemp) {
                    componentTask = componentTaskTemp;
                } else {
                    // mtsTaskFromCallback type component
                    componentTaskTemp = dynamic_cast<mtsTaskFromCallback*>(it->second);
                    if (componentTaskTemp) {
                        componentTask = componentTaskTemp;
                    } else {
                        componentTask = 0;
                        CMN_LOG_CLASS_INIT_ERROR << "KillAll: invalid component: unknown mtsTask type" << std::endl;
                        continue;
                    }
                }
            }
        }
        componentTask->Kill();
    }

    ComponentMapChange.Unlock();
}

bool mtsManagerLocal::Connect(const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
                              const std::string & serverComponentName, const std::string & serverInterfaceProvidedName)
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
        return false;
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
        return false;
    }

    const int connectionId = ManagerGlobal->Connect(ProcessName,
        ProcessName, clientComponentName, clientInterfaceRequiredName,
        ProcessName, serverComponentName, serverInterfaceProvidedName);
    if (connectionId == -1) {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to get connection id from the Global Component Manager: "
                                 << clientComponentName << ":" << clientInterfaceRequiredName << " - "
                                 << serverComponentName << ":" << serverInterfaceProvidedName << std::endl;
        return false;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "Connect: connection id was issued: " << connectionId << std::endl;

    const bool ret = ConnectLocally(clientComponentName, clientInterfaceRequiredName,
                                    serverComponentName, serverInterfaceProvidedName);
    if (!ret) {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to establish local connection: "
            << clientComponentName << ":" << clientInterfaceRequiredName << " - "
            << serverComponentName << ":" << serverInterfaceProvidedName << std::endl;
        return false;
    }

    // Notify the GCM of successful local connection
    if (!ManagerGlobal->ConnectConfirm(connectionId)) {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to notify GCM of this connection: "
                                 << clientComponentName << ":" << clientInterfaceRequiredName << " - "
                                 << serverComponentName << ":" << serverInterfaceProvidedName << std::endl;

        if (!Disconnect(clientComponentName, clientInterfaceRequiredName, serverComponentName, serverInterfaceProvidedName)) {
            CMN_LOG_CLASS_INIT_ERROR << "Connect: clean up error: disconnection failed: "
                                     << clientComponentName << ":" << clientInterfaceRequiredName << " - "
                                     << serverComponentName << ":" << serverInterfaceProvidedName << std::endl;
        }

        return false;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "Connect: successfully established local connection: "
                               << clientComponentName << ":" << clientInterfaceRequiredName << " - "
                               << serverComponentName << ":" << serverInterfaceProvidedName << std::endl;

    return true;
}

#if CISST_MTS_HAS_ICE
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

bool mtsManagerLocal::Connect(
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName)
{
    // Prevent this method from being used to connect two local interfaces
    if (clientProcessName == serverProcessName) {
        return Connect(clientComponentName, clientInterfaceRequiredName, serverComponentName, serverInterfaceProvidedName);
    }

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
        CMN_LOG_CLASS_INIT_ERROR << "Connect: cannot connect two external components." << std::endl;
        return false;
    }

    // To support bi-directional connection, retry connection untill success up
    // to 10 seconds.
    //
    // An example of the bi-directional connection:
    // component A has a required interface that connects to component B's
    // provided interface and the component B also has a required interface that
    // needs to connect to component A's provided interface.
    const unsigned int maxRetryCount = 10;
    unsigned int retryCount = 1;
    int connectionID;

    while (retryCount <= maxRetryCount) {
        // Inform the global component manager of a new connection being established.
        connectionID = ManagerGlobal->Connect(this->ProcessName,
            clientProcessName, clientComponentName, clientInterfaceRequiredName,
            serverProcessName, serverComponentName, serverInterfaceProvidedName);
        if (connectionID == -1) {
            CMN_LOG_CLASS_INIT_ERROR << "Connect: Waiting for connection to be established.... Retrying "
                << retryCount++ << "/" << maxRetryCount << std::endl;
            osaSleep(1 * cmn_s);
        } else {
            break;
        }
    }

    if (connectionID == -1) {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to issue new connection id: "
                                 << mtsManagerGlobal::GetInterfaceUID(clientProcessName, clientComponentName, clientInterfaceRequiredName)
                                 << " - "
                                 << mtsManagerGlobal::GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceProvidedName)
                                 << std::endl;
        return false;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "Connect: new connection id is issued: " << connectionID << " for "
                               << mtsManagerGlobal::GetInterfaceUID(clientProcessName, clientComponentName, clientInterfaceRequiredName)
                               << " - "
                               << mtsManagerGlobal::GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceProvidedName)
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
        if (!ConnectClientSideInterface(connectionID,
                clientProcessName, clientComponentName, clientInterfaceRequiredName,
                serverProcessName, serverComponentName, serverInterfaceProvidedName))
        {
            CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to connect at client process \"" << clientProcessName << "\"" << std::endl;

            if (!Disconnect(clientProcessName, clientComponentName, clientInterfaceRequiredName,
                            serverProcessName, serverComponentName, serverInterfaceProvidedName))
            {
                CMN_LOG_CLASS_INIT_ERROR << "Connect: clean up error: disconnection failed";
            }
            return false;
        }
    }
    // If server process calls Connect(), let client process initiate connection
    // process via the GCM.
    else {
        if (!ManagerGlobal->InitiateConnect(connectionID,
                clientProcessName, clientComponentName, clientInterfaceRequiredName,
                serverProcessName, serverComponentName, serverInterfaceProvidedName))
        {
            CMN_LOG_CLASS_INIT_ERROR << "Connect: Failed to initiate connection" << std::endl;

            if (!Disconnect(clientProcessName, clientComponentName, clientInterfaceRequiredName,
                            serverProcessName, serverComponentName, serverInterfaceProvidedName))
            {
                CMN_LOG_CLASS_INIT_ERROR << "Connect: clean up (disconnect failed) error";
            }
            return false;
        }
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "Connect: successfully established remote connection: "
                               << mtsManagerGlobal::GetInterfaceUID(clientProcessName, clientComponentName, clientInterfaceRequiredName)
                               << " - "
                               << mtsManagerGlobal::GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceProvidedName)
                               << std::endl;

    return true;
}
#endif

bool mtsManagerLocal::ConnectLocally(const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
                                     const std::string & serverComponentName, const std::string & serverInterfaceProvidedName)
{
    // At this point, it is guaranteed that all components and interfaces exist
    // in the same process because the global component manager has already
    // created all proxy objects needed.
    mtsComponent * clientComponent = GetComponent(clientComponentName);
    if (!clientComponent) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectLocally: failed to get client component: \"" << clientComponentName << "\"" << std::endl;
        return false;
    }

    mtsComponent * serverComponent = GetComponent(serverComponentName);
    if (!serverComponent) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectLocally: failed to get server component: \"" << serverComponentName << "\"" << std::endl;
        return false;
    }

    mtsInterfaceProvidedOrOutput * serverInterfaceProvidedOrOutput = serverComponent->GetInterfaceProvidedOrOutput(serverInterfaceProvidedName);
    if (!serverInterfaceProvidedOrOutput) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectLocally: failed to get provided/output interface \"" << serverInterfaceProvidedName << "\""
                                 << " in component \"" << serverComponentName << "\"" << std::endl;
        return false;
    }

#if 0
    // If a server component is a proxy, it should create a new instance of
    // provided interface proxy and clone all the proxy objects in it (e.g.
    // command and event object proxies). This is a crucial step for thread-
    // safe data exchange over networks.
    // See mtsComponentProxy::CreateInterfaceProvidedProxy() for details.
    mtsInterfaceProvided * interfaceProvidedInstance = 0;

#if CISST_MTS_HAS_ICE
    const bool isServerComponentProxy = IsProxyComponent(serverComponentName);
    if (isServerComponentProxy) {
        mtsComponentProxy * serverComponentProxy = dynamic_cast<mtsComponentProxy *>(serverComponent);
        if (!serverComponentProxy) {
            CMN_LOG_CLASS_INIT_ERROR << "ConnectLocally: invalid type of server component: " << serverComponentName << std::endl;
            return false;
        }

        // Issue a new resource user id and create provided interface instance
        interfaceProvidedInstance = serverComponentProxy->CreateInterfaceProvidedInstance(serverInterfaceProvided, interfaceProvidedInstanceID);
        if (!interfaceProvidedInstance) {
            CMN_LOG_CLASS_INIT_ERROR << "ConnectLocally: failed to create provided interface proxy instance: "
                                     << clientComponentName << ":" << clientInterfaceRequiredName << std::endl;
            return -1;
        }

        /* TODO: How to resolve a problem of receiving duplicate events?
        // Disable event void (see mtsCommandBase.h for detailed comments)
        mtsComponentInterface::EventVoidMapType::const_iterator itVoid =
            interfaceProvidedInstance->EventVoidGenerators.begin();
        const mtsComponentInterface::EventVoidMapType::const_iterator itVoidEnd =
            interfaceProvidedInstance->EventVoidGenerators.end();
        for (; itVoid != itVoidEnd; ++itVoid) {
            itVoid->second->DisableEvent();
        }

        // Disable event write
        mtsComponentInterface::EventWriteMapType::const_iterator itWrite =
            interfaceProvidedInstance->EventWriteGenerators.begin();
        const mtsComponentInterface::EventWriteMapType::const_iterator itWriteEnd =
            interfaceProvidedInstance->EventWriteGenerators.end();
        for (; itWrite != itWriteEnd; ++itWrite) {
            itWrite->second->DisableEvent();
        }
        */

        CMN_LOG_CLASS_INIT_VERBOSE << "ConnectLocally: "
                                   << "created provided interface proxy instance: id = "
                                   << interfaceProvidedInstanceID << std::endl;
    }
#endif

    // If interfaceProvidedInstance is 0, it is assumed that this connection
    // is established between two original interfaces.
    if (!interfaceProvidedInstance) {
        interfaceProvidedInstance = serverInterfaceProvided;
    }
#endif

    mtsInterfaceProvidedOrOutput * interfaceProvidedOrOutputInstance = serverInterfaceProvidedOrOutput;

    // Connect two interfaces. mtsInterfaceProvided::GetEndUserInterface() is
    // internally called to create a new provided interface instance for the
    // client component.  This solves thread-safety issues in data exchange
    // across a network.
    if (!clientComponent->ConnectInterfaceRequiredOrInput(clientInterfaceRequiredName, interfaceProvidedOrOutputInstance)) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectLocally: failed to connect interfaces: "
                                 << clientComponentName << ":" << clientInterfaceRequiredName << " - "
                                 << serverComponentName << ":" << serverInterfaceProvidedName << std::endl;
        return false;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "ConnectLocally: successfully connected: "
                               << clientComponentName << ":" << clientInterfaceRequiredName << " - "
                               << serverComponentName << ":" << serverInterfaceProvidedName << std::endl;

    return true;
}


bool mtsManagerLocal::Disconnect(const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
                                 const std::string & serverComponentName, const std::string & serverInterfaceProvidedName)
{
    bool success = ManagerGlobal->Disconnect(
        ProcessName, clientComponentName, clientInterfaceRequiredName,
        ProcessName, serverComponentName, serverInterfaceProvidedName);

    if (!success) {
        CMN_LOG_CLASS_INIT_ERROR << "Disconnect: disconnection failed." << std::endl;
        return false;
    }

    //
    // TODO: LOCAL DISCONNECT!!! (e.g. disable all commands and events, and all the other
    // resource clean-up and disconnection chores)
    //

    CMN_LOG_CLASS_INIT_VERBOSE << "Disconnect: successfully disconnected." << std::endl;

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

    mtsInterfaceProvidedOrOutput * interfaceProvidedOrOutput;
    interfaceNames = component->GetNamesOfInterfacesProvidedOrOutput();
    for (size_t i = 0; i < interfaceNames.size(); ++i) {
        interfaceProvidedOrOutput = component->GetInterfaceProvidedOrOutput(interfaceNames[i]);
        if (!interfaceProvidedOrOutput) {
            CMN_LOG_CLASS_INIT_ERROR << "RegisterInterfaces: NULL provided/output interface detected: " << interfaceNames[i] << std::endl;
            return false;
        } else {
            if (ManagerGlobal->FindInterfaceProvidedOrOutput(ProcessName, componentName, interfaceNames[i])) {
                continue;
            }
        }
        if (!ManagerGlobal->AddInterfaceProvidedOrOutput(ProcessName, componentName, interfaceNames[i], false)) {
            CMN_LOG_CLASS_INIT_ERROR << "RegisterInterfaces: failed to add provided/output interface: "
                                     << componentName << ":" << interfaceNames[i] << std::endl;
            return false;
        }
    }

    mtsInterfaceRequiredOrInput * interfaceRequiredOrInput;
    interfaceNames = component->GetNamesOfInterfacesRequiredOrInput();
    for (size_t i = 0; i < interfaceNames.size(); ++i) {
        interfaceRequiredOrInput = component->GetInterfaceRequiredOrInput(interfaceNames[i]);
        if (!interfaceRequiredOrInput) {
            CMN_LOG_CLASS_INIT_ERROR << "RegisterInterfaces: NULL required/input interface detected: " << interfaceNames[i] << std::endl;
            return false;
        } else {
            if (ManagerGlobal->FindInterfaceRequiredOrInput(ProcessName, componentName, interfaceNames[i])) {
                continue;
            }
        }
        if (!ManagerGlobal->AddInterfaceRequiredOrInput(ProcessName, componentName, interfaceNames[i], false)) {
            CMN_LOG_CLASS_INIT_ERROR << "RegisterInterfaces: failed to add required/input interface: "
                                     << componentName << ":" << interfaceNames[i] << std::endl;
            return false;
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



#if CISST_MTS_HAS_ICE
bool mtsManagerLocal::Disconnect(
    const std::string & clientProcessName,
    const std::string & clientComponentName,
    const std::string & clientInterfaceRequiredName,
    const std::string & serverProcessName,
    const std::string & serverComponentName,
    const std::string & serverInterfaceProvidedName)
{
    bool success = ManagerGlobal->Disconnect(
        clientProcessName, clientComponentName, clientInterfaceRequiredName,
        serverProcessName, serverComponentName, serverInterfaceProvidedName);

    if (!success) {
        CMN_LOG_CLASS_INIT_ERROR << "Disconnect: disconnection failed." << std::endl;
        return false;
    }

    //
    // TODO: LOCAL DISCONNECT!!!
    //

    CMN_LOG_CLASS_INIT_VERBOSE << "Disconnect: successfully disconnected." << std::endl;

    return true;
}

bool mtsManagerLocal::GetInterfaceProvidedDescription(
    const std::string & serverComponentName, const std::string & interfaceProvidedName,
    InterfaceProvidedDescription & interfaceProvidedDescription, const std::string & CMN_UNUSED(listenerID))
{
    // Get component specified
    mtsComponent * component = GetComponent(serverComponentName);
    if (!component) {
        CMN_LOG_CLASS_INIT_ERROR << "GetInterfaceProvidedDescription: no component \""
                                 << serverComponentName << "\" found in process: \"" << ProcessName << "\"" << std::endl;
        return false;
    }

    // Get provided interface specified
    mtsInterfaceProvided * interfaceProvided = component->GetInterfaceProvided(interfaceProvidedName);
    if (!interfaceProvided) {
        CMN_LOG_CLASS_INIT_ERROR << "GetInterfaceProvidedDescription: no provided interface \""
                                 << interfaceProvidedName << "\" found in component \"" << serverComponentName << "\"" << std::endl;
        return false;
    }

    // Extract complete information about all commands and event generators in
    // the provided interface specified. Argument prototypes are serialized.
    interfaceProvidedDescription.InterfaceProvidedName = interfaceProvidedName;
    mtsComponentProxy::ExtractInterfaceProvidedDescription(interfaceProvided, interfaceProvidedDescription);

    return true;
}

bool mtsManagerLocal::GetInterfaceRequiredDescription(
    const std::string & componentName, const std::string & requiredInterfaceName,
    InterfaceRequiredDescription & requiredInterfaceDescription, const std::string & CMN_UNUSED(listenerID))
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
    requiredInterfaceDescription.InterfaceRequiredName = requiredInterfaceName;

    mtsComponentProxy::ExtractInterfaceRequiredDescription(requiredInterface, requiredInterfaceDescription);

    return true;
}

bool mtsManagerLocal::CreateComponentProxy(const std::string & componentProxyName, const std::string & CMN_UNUSED(listenerID))
{
    // Create a component proxy
    mtsComponent * newComponent = new mtsComponentProxy(componentProxyName);

    bool success = AddComponent(newComponent);
    if (!success) {
        delete newComponent;
        return false;
    }

    return true;
}

bool mtsManagerLocal::RemoveComponentProxy(const std::string & componentProxyName, const std::string & CMN_UNUSED(listenerID))
{
    return RemoveComponent(componentProxyName);
}

bool mtsManagerLocal::CreateInterfaceProvidedProxy(
    const std::string & serverComponentProxyName,
    const InterfaceProvidedDescription & interfaceProvidedDescription, const std::string & CMN_UNUSED(listenerID))
{
    const std::string interfaceProvidedName = interfaceProvidedDescription.InterfaceProvidedName;

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
    if (!ManagerGlobal->AddInterfaceProvidedOrOutput(ProcessName, serverComponentProxyName, interfaceProvidedName, true)) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: "
                                 << "failed to add provided interface proxy to global component manager: "
                                 << ProcessName << ":" << serverComponentProxyName << ":" << interfaceProvidedName << std::endl;
        return false;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "CreateInterfaceProvidedProxy: "
                               << "successfully created Provided interface proxy: " << serverComponentProxyName << ":"
                               << interfaceProvidedName << std::endl;
    return true;
}


bool mtsManagerLocal::CreateInterfaceRequiredProxy(
    const std::string & clientComponentProxyName, const InterfaceRequiredDescription & requiredInterfaceDescription, const std::string & CMN_UNUSED(listenerID))
{
    const std::string requiredInterfaceName = requiredInterfaceDescription.InterfaceRequiredName;

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
    if (!ManagerGlobal->AddInterfaceRequiredOrInput(ProcessName, clientComponentProxyName, requiredInterfaceName, true)) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceRequiredProxy: "
                                 << "failed to add required interface proxy to global component manager: "
                                 << ProcessName << ":" << clientComponentProxyName << ":" << requiredInterfaceName << std::endl;
        return false;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "CreateInterfaceRequiredProxy: "
                               << "successfully created required interface proxy: " << clientComponentProxyName << ":"
                               << requiredInterfaceName << std::endl;
    return true;
}

bool mtsManagerLocal::RemoveInterfaceProvidedProxy(
    const std::string & clientComponentProxyName, const std::string & interfaceProvidedProxyName, const std::string & CMN_UNUSED(listenerID))
{
    mtsComponent * clientComponent = GetComponent(clientComponentProxyName);
    if (!clientComponent) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveInterfaceProvidedProxy: can't find client component: " << clientComponentProxyName << std::endl;
        return false;
    }

    mtsComponentProxy * clientComponentProxy = dynamic_cast<mtsComponentProxy*>(clientComponent);
    if (!clientComponentProxy) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveInterfaceProvidedProxy: client component is not a proxy: " << clientComponentProxyName << std::endl;
        return false;
    }

    // Check a number of required interfaces using (connecting to) this provided interface.
    mtsInterfaceProvided * interfaceProvidedProxy = clientComponentProxy->GetInterfaceProvided(interfaceProvidedProxyName);
    if (!interfaceProvidedProxy) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveInterfaceProvidedProxy: can't get provided interface proxy.: " << interfaceProvidedProxyName << std::endl;
        return false;
    }

    // Remove provided interface proxy only when user counter is zero.
    if (--interfaceProvidedProxy->UserCounter == 0) {
        // Remove provided interface from component proxy.
        if (!clientComponentProxy->RemoveInterfaceProvidedProxy(interfaceProvidedProxyName)) {
            CMN_LOG_CLASS_INIT_ERROR << "RemoveInterfaceProvidedProxy: failed to remove provided interface proxy: " << interfaceProvidedProxyName << std::endl;
            return false;
        }

        CMN_LOG_CLASS_INIT_VERBOSE << "RemoveInterfaceProvidedProxy: removed provided interface: "
                                   << clientComponentProxyName << ":" << interfaceProvidedProxyName << std::endl;
    } else {
        CMN_LOG_CLASS_INIT_VERBOSE << "RemoveInterfaceProvidedProxy: decreased active user counter. current counter: "
                                   << interfaceProvidedProxy->UserCounter << std::endl;
    }

    return true;
}

bool mtsManagerLocal::RemoveInterfaceRequiredProxy(
    const std::string & serverComponentProxyName, const std::string & requiredInterfaceProxyName, const std::string & CMN_UNUSED(listenerID))
{
    mtsComponent * serverComponent = GetComponent(serverComponentProxyName);
    if (!serverComponent) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveInterfaceRequiredProxy: can't find server component: " << serverComponentProxyName << std::endl;
        return false;
    }

    mtsComponentProxy * serverComponentProxy = dynamic_cast<mtsComponentProxy*>(serverComponent);
    if (!serverComponentProxy) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveInterfaceRequiredProxy: server component is not a proxy: " << serverComponentProxyName << std::endl;
        return false;
    }

    // Remove required interface from component proxy.
    if (!serverComponentProxy->RemoveInterfaceRequiredProxy(requiredInterfaceProxyName)) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveInterfaceRequiredProxy: failed to remove required interface proxy: " << requiredInterfaceProxyName << std::endl;
        return false;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "RemoveInterfaceRequiredProxy: removed required interface: "
                               << serverComponentProxyName << ":" << requiredInterfaceProxyName << std::endl;

    return true;
}

int mtsManagerLocal::GetTotalNumberOfInterfaces(const std::string & componentName, const std::string & CMN_UNUSED(listenerID))
{
    // Check if the component specified exists
    mtsComponent * component = GetComponent(componentName);
    if (!component) {
        CMN_LOG_CLASS_INIT_ERROR << "GetTotalNumberOfInterfaces: no component found: " << componentName << " on " << ProcessName << std::endl;
        return -1;
    }

    return (int)(component->GetNumberOfInterfacesProvided() + component->GetNumberOfInterfacesRequired());
}

void mtsManagerLocal::SetIPAddress(void)
{
    // Fetch all ip addresses available on this machine.
    std::vector<std::string> ipAddresses;
    osaSocket::GetLocalhostIP(ipAddresses);

    for (size_t i = 0; i < ipAddresses.size(); ++i) {
        CMN_LOG_CLASS_INIT_VERBOSE << "This machine's IP address : " << ipAddresses[i] << std::endl;
    }

    ProcessIPList.insert(ProcessIPList.begin(), ipAddresses.begin(), ipAddresses.end());
}

bool mtsManagerLocal::SetInterfaceProvidedProxyAccessInfo(
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName,
    const std::string & endpointInfo)
{
    return ManagerGlobal->SetInterfaceProvidedProxyAccessInfo(
        clientProcessName, clientComponentName, clientInterfaceRequiredName,
        serverProcessName, serverComponentName, serverInterfaceProvidedName,
        endpointInfo);
}

bool mtsManagerLocal::ConnectServerSideInterface(const unsigned int connectionID,
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName,
    const std::string & CMN_UNUSED(listenerID))
{
    // This method is called only by the GCM to connect two local interfaces
    // -- one inteface is an original interface and the other one is a proxy
    // interface -- at server side.

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
    bool ret = ConnectLocally(actualClientComponentName, clientInterfaceRequiredName,
                              actualServerComponentName, serverInterfaceProvidedName);
    if (!ret) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectServerSideInterface: ConnectLocally() failed" << std::endl;
        return false;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "ConnectServerSideInterface: successfully established local connection at server process." << std::endl;

    // Information to access (connect to) network interface proxy server.
    std::string serverEndpointInfo;
    mtsComponentProxy * clientComponentProxy;

    int numTrial = 0;
    const int maxTrial = 10;
    const double sleepTime = 200 * cmn_ms;

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
    // interface proxy server has not started yet, has not yet registered its
    // access information to the global component manager, and thus access
    // information is not readily available.  To handle such a case, a required
    // interface proxy client tries fetching the information from the global
    // component manager for five times.  After these trials without success,
    // this method returns false, resulting in disconnecting and cleaning up the
    // current pending connection.

    // Fecth proxy server's access information from the GCM
    while (++numTrial <= maxTrial) {
        // Try to get server proxy access information
        if (ManagerGlobal->GetInterfaceProvidedProxyAccessInfo(
                clientProcessName, clientComponentName, clientInterfaceRequiredName,
                serverProcessName, serverComponentName, serverInterfaceProvidedName,
                serverEndpointInfo))
        {
            CMN_LOG_CLASS_INIT_VERBOSE << "ConnectServerSideInterface: fetched server proxy access information: "
                                       << serverEndpointInfo << std::endl;
            break;
        }

        // Wait for 1 second
        CMN_LOG_CLASS_INIT_VERBOSE << "ConnectServerSideInterface: waiting for server proxy access information ... "
                                   << numTrial << " / " << maxTrial << std::endl;
        osaSleep(sleepTime);
    }

    // If this client proxy finally didn't get the access information.
    if (numTrial > maxTrial) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectServerSideInterface: failed to fetch information to access network proxy server" << std::endl;
        goto ConnectServerSideInterfaceError;
    }

    // Create and run required interface proxy client
    if (!UnitTestEnabled || (UnitTestEnabled && UnitTestNetworkProxyEnabled)) {
        if (!clientComponentProxy->CreateInterfaceProxyClient(
                clientInterfaceRequiredName, serverEndpointInfo, connectionID)) {
            CMN_LOG_CLASS_INIT_ERROR << "ConnectServerSideInterface: failed to create network interface proxy client"
                                     << ": " << clientComponentProxy->GetName() << std::endl;
            goto ConnectServerSideInterfaceError;
        }

        // Wait for the required interface proxy client to successfully connect to
        // provided interface proxy server.
        numTrial = 0;
        while (++numTrial <= maxTrial) {
            if (clientComponentProxy->IsActiveProxy(clientInterfaceRequiredName, false)) {
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
        if (!clientComponentProxy->UpdateEventHandlerProxyID(clientComponentName, clientInterfaceRequiredName)) {
            CMN_LOG_CLASS_INIT_ERROR << "ConnectServerSideInterface: failed to update event handler proxies" << std::endl;
            goto ConnectServerSideInterfaceError;
        }
    }

    return true;

ConnectServerSideInterfaceError:
    if (!Disconnect(clientProcessName, clientComponentName, clientInterfaceRequiredName,
                    serverProcessName, serverComponentName, serverInterfaceProvidedName))
    {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectServerSideInterface: clean up (disconnect failed) error";
    }

    return false;
}

bool mtsManagerLocal::ConnectClientSideInterface(const unsigned int connectionID,
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName,
    const std::string & CMN_UNUSED(listenerID))
{
    std::string endpointAccessInfo, communicatorId;
    mtsComponent *clientComponent, *serverComponent;
    mtsComponentProxy *serverComponentProxy;

    // Get actual names of components (either a client component or a server
    // component should be a proxy object).
    const std::string actualClientComponentName = clientComponentName;
    const std::string actualServerComponentName = mtsManagerGlobal::GetComponentProxyName(serverProcessName, serverComponentName);

    // Connect two local interfaces
    bool ret = ConnectLocally(actualClientComponentName, clientInterfaceRequiredName,
                              actualServerComponentName, serverInterfaceProvidedName);
    if (!ret) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectClientSideInterface: failed to connect two local interfaces: "
                                 << actualClientComponentName << ":" << clientInterfaceRequiredName << " - "
                                 << actualServerComponentName << ":" << serverInterfaceProvidedName << std::endl;
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
    // interface of which name is 'serverInterfaceProvidedName.'
    if (!serverComponentProxy->FindInterfaceProxyServer(serverInterfaceProvidedName)) {
        if (!UnitTestEnabled || (UnitTestEnabled && UnitTestNetworkProxyEnabled)) {
            if (!serverComponentProxy->CreateInterfaceProxyServer(
                    serverInterfaceProvidedName, endpointAccessInfo, communicatorId))
            {
                CMN_LOG_CLASS_INIT_ERROR << "ConnectClientSideInterface: failed to create network interface proxy server: "
                                         << serverComponentProxy->GetName() << std::endl;
                goto ConnectClientSideInterfaceError;
            }
            CMN_LOG_CLASS_INIT_VERBOSE << "ConnectClientSideInterface: successfully created network interface proxy server: "
                                       << serverComponentProxy->GetName() << std::endl;
        }
    }
    // If there is a server proxy already running, fetch and use the access
    // information of it without specifying client interface.
    else {
        if (!ManagerGlobal->GetInterfaceProvidedProxyAccessInfo("", "", "",
                serverProcessName, serverComponentName, serverInterfaceProvidedName,
                endpointAccessInfo))
        {
            CMN_LOG_CLASS_INIT_ERROR << "ConnectClientSideInterface: failed to fecth server proxy access information: "
                                     << mtsManagerGlobal::GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceProvidedName)
                                     << std::endl;
            goto ConnectClientSideInterfaceError;
        }
    }

    // Inform the global component manager of the access information of this
    // server proxy so that a network proxy client (of type mtsComponentInterfaceProxyClient)
    // can connect to this server proxy later.
    if (!SetInterfaceProvidedProxyAccessInfo(
            clientProcessName, clientComponentName, clientInterfaceRequiredName,
            serverProcessName, serverComponentName, serverInterfaceProvidedName,
            endpointAccessInfo))
    {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectClientSideInterface: failed to set server proxy access information: "
                                 << serverInterfaceProvidedName << ", " << endpointAccessInfo << std::endl;
        goto ConnectClientSideInterfaceError;
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "ConnectClientSideInterface: successfully set server proxy access information: "
                               << serverInterfaceProvidedName << ", " << endpointAccessInfo << std::endl;

    // Make the server process begin connection process via the GCM. Note that
    // this call is blocking and returns only after the server process finishes
    // establishing server-side connection (regardless of success or failure)
    // and the global component manager gets informed of the connection.
    if (!ManagerGlobal->ConnectServerSideInterfaceRequest(connectionID,
            clientProcessName, clientComponentName, clientInterfaceRequiredName,
            serverProcessName, serverComponentName, serverInterfaceProvidedName))
    {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectClientSideInterface: failed to connect interfaces at server process" << std::endl;
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
                                                     serverInterfaceProvidedName,
                                                     clientInterfaceRequiredName);
    if (!ret) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectClientSideInterface: failed to update command proxy id" << std::endl;
        goto ConnectClientSideInterfaceError;
    }

    // Sleep for unit tests which include networking
    if (UnitTestEnabled && UnitTestNetworkProxyEnabled) {
        osaSleep(3);
    }

    // Inform the GCM that the connection is successfully established and
    // becomes active (network proxies are running now and an ICE client
    // proxy is connected to an ICE server proxy).
    if (!ManagerGlobal->ConnectConfirm(connectionID)) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectClientSideInterface: failed to notify GCM of this connection" << std::endl;
        goto ConnectClientSideInterfaceError;
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "ConnectClientSideInterface: successfully informed global component manager of this connection: connection id = "
                               << connectionID << std::endl;

    // Register this connection information to a provided interface proxy
    // server so that the proxy server can clean up this connection when a
    // required interface proxy client is detected as disconnected.
    if (!serverComponentProxy->AddConnectionInformation(connectionID,
                                                        clientProcessName, clientComponentName, clientInterfaceRequiredName,
                                                        serverProcessName, serverComponentName, serverInterfaceProvidedName))
    {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectClientSideInterface: failed to add connection information: connection id = " << connectionID << std::endl;
        goto ConnectClientSideInterfaceError;
    }

    return true;

ConnectClientSideInterfaceError:
    if (!Disconnect(clientProcessName, clientComponentName, clientInterfaceRequiredName,
                    serverProcessName, serverComponentName, serverInterfaceProvidedName))
    {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectClientSideInterface: disconnect error while cleaning up connection: connection id = " << connectionID << std::endl;
    }

    return false;
}

void mtsManagerLocal::DisconnectGCM()
{
    mtsManagerProxyClient * globalComponentManagerProxy = dynamic_cast<mtsManagerProxyClient*>(ManagerGlobal);
    CMN_ASSERT(globalComponentManagerProxy);

    globalComponentManagerProxy->Stop();
}

void mtsManagerLocal::ReconnectGCM()
{
    mtsManagerProxyClient * globalComponentManagerProxy = dynamic_cast<mtsManagerProxyClient*>(ManagerGlobal);
    CMN_ASSERT(globalComponentManagerProxy);

    if (!globalComponentManagerProxy->Start(this)) {
        CMN_LOG_CLASS_INIT_ERROR << "ReconnectGCM: Start failed" << std::endl;
        return;
    }

    if (!globalComponentManagerProxy->AddProcess(ProcessName)) {
        CMN_LOG_CLASS_INIT_ERROR << "ReconnectGCM: AddProcess failed" << std::endl;
        return;
    }
}
#endif
