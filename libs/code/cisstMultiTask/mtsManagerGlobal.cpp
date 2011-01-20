/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung
  Created on: 2009-11-12

  (C) Copyright 2009-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaSocket.h>
#include <cisstOSAbstraction/osaSleep.h>

#include <cisstMultiTask/mtsConfig.h>
#include <cisstMultiTask/mtsManagerGlobal.h>
#include <cisstMultiTask/mtsManagerComponentClient.h>
#include <cisstMultiTask/mtsManagerComponentServer.h>
#include <cisstMultiTask/mtsConnection.h>

#if CISST_MTS_HAS_ICE
#include <cisstMultiTask/mtsManagerProxyServer.h>
#include <cisstMultiTask/mtsComponentProxy.h>
#endif // CISST_MTS_HAS_ICE

#include <iostream>
#include <iterator>

mtsManagerGlobal::mtsManagerGlobal() :
    ProcessMap("ProcessMap"), LocalManager(0), LocalManagerConnected(0), ConnectionID(0), 
#if CISST_MTS_HAS_ICE
    ProxyServer(0),
#endif
    ManagerComponentServer(0),
    ThreadDisconnectRunning(true)
{
    ProcessMap.SetOwner(*this);

    // Create internal thread for thread-safe disconnection
    ThreadDisconnect.Create<mtsManagerGlobal, void *>(this, &mtsManagerGlobal::ThreadDisconnectProcess);
}

mtsManagerGlobal::~mtsManagerGlobal()
{
    Cleanup();
}

//-------------------------------------------------------------------------
//  Processing Methods
//-------------------------------------------------------------------------
void mtsManagerGlobal::Cleanup(void)
{
    // Disconnect all connections
    ConnectionMapType::const_iterator it1 = ConnectionMap.begin();
    const ConnectionMapType::const_iterator itEnd1 = ConnectionMap.end();
    for (; it1 != itEnd1; ++it1) {
        Disconnect(it1->second.GetConnectionID());
    }

    osaSleep(1 * cmn_s);

    // Remove all processes
    ProcessMapType::iterator it2 = ProcessMap.begin();
    while (it2 != ProcessMap.end()) {
        RemoveProcess(it2->first);
        it2 = ProcessMap.begin();
    }

    // Stop internal thread
    ThreadDisconnectRunning = false;
    ThreadDisconnectFinished.Wait();
}

void * mtsManagerGlobal::ThreadDisconnectProcess(void * CMN_UNUSED(arg))
{
    while (ThreadDisconnectRunning) {
        DisconnectInternal();
        osaSleep(100 * cmn_ms);
    }

    ThreadDisconnectFinished.Raise();

    return 0;
}

mtsManagerGlobal::ConnectionIDListType * mtsManagerGlobal::GetConnectionsOfInterfaceProvidedOrOutput(
    const std::string & severProcessName, const std::string & serverComponentName,
    const std::string & interfaceProvidedName) const
{
    ComponentMapType * componentMap = ProcessMap.GetItem(severProcessName);
    if (componentMap == 0) return 0;

    InterfaceMapType * interfaceMap = componentMap->GetItem(serverComponentName);
    if (interfaceMap == 0) return 0;

    return interfaceMap->InterfaceProvidedOrOutputMap.GetItem(interfaceProvidedName);
}

mtsManagerGlobal::ConnectionIDListType * mtsManagerGlobal::GetConnectionsOfInterfaceProvidedOrOutput(
    const std::string & severProcessName, const std::string & serverComponentName,
    const std::string & interfaceProvidedName, InterfaceMapType ** interfaceMap) const
{
    ComponentMapType * componentMap = ProcessMap.GetItem(severProcessName);
    if (componentMap == 0) return 0;

    *interfaceMap = componentMap->GetItem(serverComponentName);
    if (*interfaceMap == 0) return 0;

    return (*interfaceMap)->InterfaceProvidedOrOutputMap.GetItem(interfaceProvidedName);
}

mtsManagerGlobal::ConnectionIDListType * mtsManagerGlobal::GetConnectionsOfInterfaceRequiredOrInput(
    const std::string & clientProcessName, const std::string & clientComponentName,
    const std::string & interfaceRequiredName) const
{
    ComponentMapType * componentMap = ProcessMap.GetItem(clientProcessName);
    if (componentMap == 0) return 0;

    InterfaceMapType * interfaceMap = componentMap->GetItem(clientComponentName);
    if (interfaceMap == 0) return 0;

    return interfaceMap->InterfaceRequiredOrInputMap.GetItem(interfaceRequiredName);
}

mtsManagerGlobal::ConnectionIDListType * mtsManagerGlobal::GetConnectionsOfInterfaceRequiredOrInput(
    const std::string & clientProcessName, const std::string & clientComponentName,
    const std::string & interfaceRequiredName, InterfaceMapType ** interfaceMap) const
{
    ComponentMapType * componentMap = ProcessMap.GetItem(clientProcessName);
    if (componentMap == 0) return 0;

    *interfaceMap = componentMap->GetItem(clientComponentName);
    if (*interfaceMap == 0) return 0;

    return (*interfaceMap)->InterfaceRequiredOrInputMap.GetItem(interfaceRequiredName);
}

bool mtsManagerGlobal::AddConnectionToInterfaceProvidedOrOutput(
    const std::string & serverProcessName, const std::string & serverComponentName,
    const std::string & interfaceProvidedName, const ConnectionIDType connectionID)
{
    InterfaceMapType * interfaceMap = 0;

    ProcessMapChange.Lock();

    mtsManagerGlobal::ConnectionIDListType * list = GetConnectionsOfInterfaceProvidedOrOutput(
        serverProcessName, serverComponentName, interfaceProvidedName, &interfaceMap);
    CMN_ASSERT(interfaceMap);

    if (!list) {
        // If this connection is the first one
        list = new ConnectionIDListType;
        (interfaceMap->InterfaceProvidedOrOutputMap.GetMap())[interfaceProvidedName] = list;
    } else {
        // Check duplicate connection id
        mtsManagerGlobal::ConnectionIDListType::const_iterator it = list->begin();
        const mtsManagerGlobal::ConnectionIDListType::const_iterator itEnd = list->end();
        for (; it != itEnd; ++it) {
            if (*it == connectionID) {
                CMN_LOG_CLASS_INIT_ERROR << "AddConnectedInterface: duplicate connection [ " << connectionID << " ] "
                    << "for \""
                    << GetInterfaceUID(serverProcessName, serverComponentName, interfaceProvidedName) 
                    << "\"" << std::endl;
                ProcessMapChange.Unlock();
                return false;
            }
        }
    }

    list->push_back(connectionID);

    ProcessMapChange.Unlock();

    std::stringstream ss;
    mtsManagerGlobal::ConnectionIDListType::const_iterator it = list->begin();
    const mtsManagerGlobal::ConnectionIDListType::const_iterator itEnd = list->end();
    for (; it != itEnd; ++it) {
        ss << *it << " ";
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "AddConnectedInterface: added connection [ " << connectionID << " ]"
        << " to interface provided \"" 
        << GetInterfaceUID(serverProcessName, serverComponentName, interfaceProvidedName)
        << "\" : " << ss.str() << std::endl;

    return true;
}

bool mtsManagerGlobal::AddConnectionToInterfaceRequiredOrInput(
    const std::string & clientProcessName, const std::string & clientComponentName,
    const std::string & interfaceRequiredName, const ConnectionIDType connectionID)
{
    InterfaceMapType * interfaceMap = 0;

    ProcessMapChange.Lock();

    mtsManagerGlobal::ConnectionIDListType * list = GetConnectionsOfInterfaceRequiredOrInput(
        clientProcessName, clientComponentName, interfaceRequiredName, &interfaceMap);
    CMN_ASSERT(interfaceMap);

    if (!list) {
        // If this connection is the first one
        list = new ConnectionIDListType;
        (interfaceMap->InterfaceRequiredOrInputMap.GetMap())[interfaceRequiredName] = list;
    } else {
        // Check duplicate connection id
        mtsManagerGlobal::ConnectionIDListType::const_iterator it = list->begin();
        const mtsManagerGlobal::ConnectionIDListType::const_iterator itEnd = list->end();
        for (; it != itEnd; ++it) {
            if (*it == connectionID) {
                CMN_LOG_CLASS_INIT_ERROR << "AddConnectedInterface: duplicate connection [ " << connectionID << " ] "
                    << "for \""
                    << GetInterfaceUID(clientProcessName, clientComponentName, interfaceRequiredName) 
                    << "\"" << std::endl;
                ProcessMapChange.Unlock();
                return false;
            }
        }
    }

    list->push_back(connectionID);

    ProcessMapChange.Unlock();

    std::stringstream ss;
    mtsManagerGlobal::ConnectionIDListType::const_iterator it = list->begin();
    const mtsManagerGlobal::ConnectionIDListType::const_iterator itEnd = list->end();
    for (; it != itEnd; ++it) {
        ss << *it << " ";
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "AddConnectedInterface: added connection [ " << connectionID << " ]"
        << " to interface required \"" 
        << GetInterfaceUID(clientProcessName, clientComponentName, interfaceRequiredName)
        << "\" : " << ss.str() << std::endl;

    return true;
}

bool mtsManagerGlobal::RemoveConnectionOfInterfaceProvidedOrOutput(
    const std::string & serverProcessName, const std::string & serverComponentName,
    const std::string & interfaceProvidedName, const ConnectionIDType connectionID)
{
    if (!FindInterfaceProvidedOrOutput(serverProcessName, serverComponentName, interfaceProvidedName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveConnectionOfInterfaceProvidedOrOutput: no interface found: [ " << connectionID << " ] "
            << "\"" << GetInterfaceUID(serverProcessName, serverComponentName, interfaceProvidedName) << "\"" << std::endl;
        return false;
    }

    InterfaceMapType * interfaceMap = 0;

    ProcessMapChange.Lock();

    mtsManagerGlobal::ConnectionIDListType * list = GetConnectionsOfInterfaceProvidedOrOutput(
        serverProcessName, serverComponentName, interfaceProvidedName, &interfaceMap);

    if (!list) {
        // If no connection is found
        ProcessMapChange.Unlock();
        return true;
    } else {
        mtsManagerGlobal::ConnectionIDListType::iterator it = list->begin();
        const mtsManagerGlobal::ConnectionIDListType::const_iterator itEnd = list->end();
        for (; it != itEnd; ++it) {
            if (*it == connectionID) {
                list->erase(it);
                CMN_LOG_CLASS_RUN_VERBOSE << "RemoveConnectionOfInterfaceProvidedOrOutput: removed connection id [ " 
                    << connectionID << " ] " << "from \""
                    << GetInterfaceUID(serverProcessName, serverComponentName, interfaceProvidedName) 
                    << "\"" << std::endl;
                ProcessMapChange.Unlock();
                return true;
            }
        }

        CMN_LOG_CLASS_RUN_VERBOSE << "RemoveConnectionOfInterfaceProvidedOrOutput: failed to remove connection id [ " 
            << connectionID << " ] " << "from \""
            << GetInterfaceUID(serverProcessName, serverComponentName, interfaceProvidedName) 
            << "\"" << std::endl;
        ProcessMapChange.Unlock();
        return false;
    }
}

bool mtsManagerGlobal::RemoveConnectionOfInterfaceRequiredOrInput(
    const std::string & clientProcessName, const std::string & clientComponentName,
    const std::string & interfaceRequiredName, const ConnectionIDType connectionID)
{
    if (!FindInterfaceRequiredOrInput(clientProcessName, clientComponentName, interfaceRequiredName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveConnectionOfInterfaceRequiredOrInput: no interface found: [ " << connectionID << " ] "
            << "\"" << GetInterfaceUID(clientProcessName, clientComponentName, interfaceRequiredName) << "\"" << std::endl;
        return false;
    }

    InterfaceMapType * interfaceMap = 0;

    ProcessMapChange.Lock();

    mtsManagerGlobal::ConnectionIDListType * list = GetConnectionsOfInterfaceRequiredOrInput(
        clientProcessName, clientComponentName, interfaceRequiredName, &interfaceMap);

    if (!list) {
        // If no connection is found
        ProcessMapChange.Unlock();
        return true;
    } else {
        mtsManagerGlobal::ConnectionIDListType::iterator it = list->begin();
        const mtsManagerGlobal::ConnectionIDListType::const_iterator itEnd = list->end();
        for (; it != itEnd; ++it) {
            if (*it == connectionID) {
                list->erase(it);
                CMN_LOG_CLASS_RUN_VERBOSE << "RemoveConnectionOfInterfaceRequiredOrInput: removed connection id [ " 
                    << connectionID << " ] " << "from \""
                    << GetInterfaceUID(clientProcessName, clientComponentName, interfaceRequiredName) 
                    << "\"" << std::endl;
                ProcessMapChange.Unlock();
                return true;
            }
        }

        CMN_LOG_CLASS_RUN_VERBOSE << "RemoveConnectionOfInterfaceRequiredOrInput: failed to remove connection id [ " 
            << connectionID << " ] " << "from \""
            << GetInterfaceUID(clientProcessName, clientComponentName, interfaceRequiredName) 
            << "\"" << std::endl;
        ProcessMapChange.Unlock();
        return false;
    }
}

mtsConnection * mtsManagerGlobal::GetConnectionInformation(const ConnectionIDType connectionID)
{
    const ConnectionMapType::iterator it = ConnectionMap.find(connectionID);
    if (it == ConnectionMap.end()) {
        return 0;
    } else {
        return &it->second;
    }
}

ConnectionIDType mtsManagerGlobal::GetConnectionID(const std::string & clientProcessName, 
        const std::string & clientComponentName, const std::string & interfaceRequiredName) const
{
    ConnectionMapType::const_iterator it = ConnectionMap.begin();
    const ConnectionMapType::const_iterator itEnd = ConnectionMap.end();

    mtsDescriptionConnection description;
    for (; it != itEnd; ++it) {
        it->second.GetDescriptionConnection(description);
        if ((description.Client.ProcessName == clientProcessName) &&
            (description.Client.ComponentName == clientComponentName) &&
            (description.Client.InterfaceName == interfaceRequiredName))
        {
            return description.ConnectionID;
        }
    }

    return 0;
}

bool mtsManagerGlobal::IsAlreadyConnected(const mtsDescriptionConnection & description) const
{
    const std::string serverProcessName   = description.Server.ProcessName;
    const std::string serverComponentName = description.Server.ComponentName;
    const std::string serverInterfaceName = description.Server.InterfaceName;
    const std::string clientProcessName   = description.Client.ProcessName;
    const std::string clientComponentName = description.Client.ComponentName;
    const std::string clientInterfaceName = description.Client.InterfaceName;

    // Check if provided interface exists
    if (!FindInterfaceProvidedOrOutput(serverProcessName, serverComponentName, serverInterfaceName)) {
        CMN_LOG_CLASS_RUN_ERROR << "IsAlreadyConnected: invalid provided interface: \""
            << GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceName)
            << "\"" << std::endl;
        return false;
    }

    // Check if required interface exists
    if (!FindInterfaceRequiredOrInput(clientProcessName, clientComponentName, clientInterfaceName)) {
        CMN_LOG_CLASS_RUN_ERROR << "IsAlreadyConnected: invalid required interface: \""
            << GetInterfaceUID(clientProcessName, clientComponentName, clientInterfaceName)
            << "\"" << std::endl;
        return false;
    }

    // Since a required interface can have only one connection with a provided interface,
    // a required interface in the connection map has unique connection id.  Based on this
    // property, we can check if two interfaces are already connected by searching the
    // connection id that corresponds to the required interface from a list of connection
    // id that the provided interface has.
    mtsManagerGlobal::ConnectionIDListType * list = 
        GetConnectionsOfInterfaceProvidedOrOutput(serverProcessName, serverComponentName, serverInterfaceName);
    if (!list) {
        // no connection
        return false;
    } else {
        ConnectionIDType connectionID = GetConnectionID(clientProcessName, clientComponentName, clientInterfaceName);

        ConnectionIDListType::const_iterator it = list->begin();
        const ConnectionIDListType::const_iterator itEnd = list->end();
        for (; it != itEnd; ++it) {
            if (*it == connectionID) {
                return true;
            }
        }
    }

    return false;
}

int mtsManagerGlobal::GetNumberOfInterfaces(const std::string & processName, const std::string & componentName,
                                            const bool includeInternalInterface) const
{
    if (!FindComponent(processName, componentName)) {
        CMN_LOG_CLASS_RUN_WARNING << "GetNumberOfInterfaces: no component found: "
            << "\"" << processName << ":" << componentName << "\"" << std::endl;
        return -1;
    }

    int numberOfInterfaceProvided = 0, numberOfInterfaceRequired = 0;
    InterfaceMapType * interfaceMap = ProcessMap.GetItem(processName)->GetItem(componentName);
    if (!interfaceMap) {
        CMN_LOG_CLASS_RUN_WARNING << "GetNumberOfInterfaces: no interface map found for "
            << "\"" << processName << ":" << componentName << "\"" << std::endl;
        return -1;
    }

    numberOfInterfaceProvided += interfaceMap->InterfaceProvidedOrOutputMap.size();
    numberOfInterfaceRequired += interfaceMap->InterfaceRequiredOrInputMap.size();

    // Exclude internal interfaces
    std::string interfaceName;
    if (!includeInternalInterface) {
        InterfaceMapElementType::const_iterator it, itEnd;
        if (mtsManagerComponentBase::IsManagerComponentServer(componentName)) {
            // InterfaceGCMRequired
            it = interfaceMap->InterfaceRequiredOrInputMap.begin();
            itEnd = interfaceMap->InterfaceRequiredOrInputMap.end();
            for (; it != itEnd; ++it) {
                if (mtsManagerComponentBase::IsNameOfInterfaceGCMRequired(it->first)) {
                    --numberOfInterfaceRequired;
                }
            }
            // InterfaceGCMProvided
            it = interfaceMap->InterfaceProvidedOrOutputMap.begin();
            itEnd = interfaceMap->InterfaceProvidedOrOutputMap.end();
            for (; it != itEnd; ++it) {
                if (mtsManagerComponentBase::IsNameOfInterfaceGCMProvided(it->first)) {
                    --numberOfInterfaceProvided;
                }
            }
        } else if (mtsManagerComponentBase::IsManagerComponentClient(componentName)) {
            // InterfaceLCMRequired or InterfaceComponentRequired
            it = interfaceMap->InterfaceRequiredOrInputMap.begin();
            itEnd = interfaceMap->InterfaceRequiredOrInputMap.end();
            for (; it != itEnd; ++it) {
                if (mtsManagerComponentBase::IsNameOfInterfaceLCMRequired(it->first) ||
                    mtsManagerComponentBase::IsNameOfInterfaceComponentRequired(it->first)) 
                {
                    --numberOfInterfaceRequired;
                }
            }
            // InterfaceLCMProvided or InterfaceComponentProvided
            it = interfaceMap->InterfaceProvidedOrOutputMap.begin();
            itEnd = interfaceMap->InterfaceProvidedOrOutputMap.end();
            for (; it != itEnd; ++it) {
                if (mtsManagerComponentBase::IsNameOfInterfaceLCMProvided(it->first) ||
                    mtsManagerComponentBase::IsNameOfInterfaceComponentProvided(it->first))
                {
                    --numberOfInterfaceProvided;
                }
            }
        } else {
            // InterfaceInternalRequired
            it = interfaceMap->InterfaceRequiredOrInputMap.begin();
            itEnd = interfaceMap->InterfaceRequiredOrInputMap.end();
            for (; it != itEnd; ++it) {
                if (mtsManagerComponentBase::IsNameOfInterfaceInternalRequired(it->first)) {
                    --numberOfInterfaceRequired;
                }
            }
            // InterfaceInternalProvided
            it = interfaceMap->InterfaceProvidedOrOutputMap.begin();
            itEnd = interfaceMap->InterfaceProvidedOrOutputMap.end();
            for (; it != itEnd; ++it) {
                if (mtsManagerComponentBase::IsNameOfInterfaceInternalProvided(it->first)) {
                    --numberOfInterfaceProvided;
                }
            }
        }
    }

    return (numberOfInterfaceProvided + numberOfInterfaceRequired);
}

//-------------------------------------------------------------------------
//  Process Management
//-------------------------------------------------------------------------
bool mtsManagerGlobal::AddProcess(const std::string & processName)
{
    // Check if the local component manager has already been registered.
    if (FindProcess(processName)) {
        CMN_LOG_CLASS_RUN_ERROR << "AddProcess: already registered process: " << "\"" << processName << "\"" << std::endl;
        return false;
    }

    ProcessMapChange.Lock();

    // Register to process map
    if (!ProcessMap.AddItem(processName, 0)) {
        CMN_LOG_CLASS_RUN_ERROR << "AddProcess: failed to add process to process map: " << "\"" << processName << "\"" << std::endl;
        ProcessMapChange.Unlock();
        return false;
    }

    ProcessMapChange.Unlock();

    return true;
}

#if CISST_MTS_HAS_ICE
bool mtsManagerGlobal::AddProcessObject(mtsManagerLocalInterface * localManagerObject, const bool isManagerProxyServer)
#else
bool mtsManagerGlobal::AddProcessObject(mtsManagerLocalInterface * localManagerObject, const bool CMN_UNUSED(isManagerProxyServer))
#endif
{
    // Name of new local component manager
    std::string processName;

    // localManagerObject can be either of type mtsManagerLocal (with 
    // isManagerProxyServer as false) or of type mtsManagerProxyServer
    // (with isManagerProxyServer as true).
#if CISST_MTS_HAS_ICE
    mtsManagerLocal * localLCM = 0;

    if (isManagerProxyServer) {
        if (LocalManagerConnected) {
            CMN_LOG_CLASS_RUN_ERROR << "AddProcessObject: local manager object has already been registered" << std::endl;
            return false;
        }

        mtsManagerProxyServer * managerProxyServer = dynamic_cast<mtsManagerProxyServer *>(localManagerObject);
        if (!managerProxyServer) {
            CMN_LOG_CLASS_RUN_ERROR << "AddProcessObject: invalid object type (mtsManagerProxyServer expected)" << std::endl;
            return false;
        }
        processName = managerProxyServer->GetProxyName();
    } else {
        if (LocalManager) {
            CMN_LOG_CLASS_RUN_ERROR << "AddProcessObject: \"local\" local manager object has already been registered" << std::endl;
            return false;
        }

        localLCM = dynamic_cast<mtsManagerLocal*>(localManagerObject);
        if (!localLCM) {
            CMN_LOG_CLASS_RUN_ERROR << "AddProcessObject: invalid type of \"local\" local manager object" << std::endl;
            return false;
        }
#endif
        processName = localManagerObject->GetProcessName();
#if CISST_MTS_HAS_ICE
    }
#endif

    // Check if the local component manager has already been registered.
    if (FindProcess(processName)) {
        // Update LocalManagerMap
        LocalManagerConnected = localManagerObject;

        CMN_LOG_CLASS_RUN_VERBOSE << "AddProcessObject: updated local manager object" << std::endl;
        return true;
    }

    // Register LCM name to process map
    ProcessMapChange.Lock();

    if (!ProcessMap.AddItem(processName, 0)) {
        CMN_LOG_CLASS_RUN_ERROR << "AddProcessObject: failed to add process to process map: \"" << processName << "\"" << std::endl;
        ProcessMapChange.Unlock();
        return false;
    }

    ProcessMapChange.Unlock();

    // Register LCM object to local manager object map
#if CISST_MTS_HAS_ICE
    if (!isManagerProxyServer) {
        LocalManager = localLCM;
    } else {
#endif
        LocalManagerConnected = localManagerObject;
#if CISST_MTS_HAS_ICE
    }
#endif

    return true;
}

bool mtsManagerGlobal::FindProcess(const std::string & processName) const
{
    return ProcessMap.FindItem(processName);
}

bool mtsManagerGlobal::RemoveProcess(const std::string & processName, const bool networkDisconnect)
{
    if (!FindProcess(processName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveProcess: no process found: " << "\"" << processName << "\"" << std::endl;
        return false;
    }

    bool ret = true;

    ProcessMapChange.Lock();

    ComponentMapType * componentMap = ProcessMap.GetItem(processName);

    // If disconnection event comes from the network layer, we should remove
    // component proxies and internal interfaces that the process involved in.
    if (componentMap) {
        if (networkDisconnect) {
            DisconnectedProcessCleanupMapChange.Lock();
            DisconnectedProcessCleanupMapType::iterator it = DisconnectedProcessCleanupMap.GetMap().find(processName);
            if (it != DisconnectedProcessCleanupMap.end()) {
                CleanupElementListType * list = it->second;

                CleanupElementListType::iterator it2 = list->begin();
                const CleanupElementListType::iterator it2End = list->end();
                std::string peerProcessName, proxyComponentName;
                for (; it2 != it2End; ++it2) {
                    peerProcessName = it2->ProcessName;
                    proxyComponentName = it2->ComponentProxyName;
                    // Remove the proxy components from GCM
                    ret &= RemoveComponent(peerProcessName, proxyComponentName, false);
                    // Remove the proxy components from LCM
                    if (peerProcessName == mtsManagerLocal::ProcessNameOfLCMWithGCM) {
                        if (!LocalManager->RemoveComponent(proxyComponentName, false)) {
                            CMN_LOG_CLASS_RUN_ERROR << "RemoveProcess: failed to remove component from LCM_with_GCM: " 
                                << "\"" << peerProcessName << ":" << proxyComponentName << "\"" 
                                << std::endl;
                            ret = false;
                        }
                    } 
#if CISST_MTS_HAS_ICE
                    else {
                        if (!LocalManagerConnected->RemoveComponentProxy(proxyComponentName, peerProcessName)) {
                            CMN_LOG_CLASS_RUN_ERROR << "RemoveProcess: failed to remove component from: " 
                                << "\"" << peerProcessName << ":" << proxyComponentName << "\"" 
                                << std::endl;
                            ret = false;
                        }
                    }
#endif
                }

                delete list;
                DisconnectedProcessCleanupMap.RemoveItem(processName);
            }
            DisconnectedProcessCleanupMapChange.Unlock();
        }

        // If the process being killed has components, they should be removed first.
        ComponentMapType::iterator it = componentMap->begin();
        while (it != componentMap->end()) {
            ret &= RemoveComponent(processName, it->first, false);
            it = componentMap->begin();
        }

        delete componentMap;
    }

    const std::string removedProcessName = processName;

    // Remove the process from process map
    ret &= ProcessMap.RemoveItem(processName);

    ProcessMapChange.Unlock();

    if (ret) {
        CMN_LOG_CLASS_RUN_VERBOSE << "RemoveProcess: removed process: " << "\"" << removedProcessName << "\"" << std::endl;
    } else {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveProcess: failed to remove process: " << "\"" << removedProcessName << "\"" << std::endl;
    }
    
    return ret;
}

mtsManagerLocalInterface * mtsManagerGlobal::GetProcessObject(const std::string & processName) const
{
    if (!ProcessMap.FindItem(processName)) {
        CMN_LOG_CLASS_RUN_ERROR << "GetProcessObject: no process found: " << "\"" << processName << "\"" << std::endl;
        return false;
    }

#if CISST_MTS_HAS_ICE
    if (LocalManager) {
        if (LocalManager->GetProcessName() == processName) {
            return LocalManager;
        }
    }
#endif

    return LocalManagerConnected;
}

std::vector<std::string> mtsManagerGlobal::GetIPAddress(void) const
{
    std::vector<std::string> ipAddresses;
    osaSocket::GetLocalhostIP(ipAddresses);

    if (ipAddresses.size() == 0) {
        CMN_LOG_CLASS_INIT_ERROR << "SetIPAddress: No network interface detected." << std::endl;
    } else {
        for (size_t i = 0; i < ipAddresses.size(); ++i) {
            CMN_LOG_CLASS_INIT_VERBOSE << "SetIPAddress: This machine's IP: [" << i << " ] " << ipAddresses[i] << std::endl;
        }
    }

    return ipAddresses;
}

void mtsManagerGlobal::GetIPAddress(std::vector<std::string> & ipAddresses) const
{
    osaSocket::GetLocalhostIP(ipAddresses);

    if (ipAddresses.size() == 0) {
        CMN_LOG_CLASS_INIT_ERROR << "SetIPAddress: No network interface detected." << std::endl;
    } else {
        for (size_t i = 0; i < ipAddresses.size(); ++i) {
            CMN_LOG_CLASS_INIT_VERBOSE << "SetIPAddress: This machine's IP: [" << i << " ] " << ipAddresses[i] << std::endl;
        }
    }
}

//-------------------------------------------------------------------------
//  Component Management
//-------------------------------------------------------------------------
bool mtsManagerGlobal::AddComponent(const std::string & processName, const std::string & componentName)
{
    if (!FindProcess(processName)) {
        CMN_LOG_CLASS_RUN_ERROR << "AddComponent: no process found: " << "\"" << processName << "\"" << std::endl;
        return false;
    }

    ProcessMapChange.Lock();

    ComponentMapType * componentMap = ProcessMap.GetItem(processName);

    // If the process did not register before
    if (componentMap == 0) {
        componentMap = new ComponentMapType(processName);
        (ProcessMap.GetMap())[processName] = componentMap;
    }

    // PK TEMP: special handling if componentName ends with "-END"
    // This was needed for JGraph component viewer, but is no longer needed for uDrawGraph component viewer.
    // If removed, need to generate AddComponentEvent elsewhere
    if (componentName.find("-END", componentName.length()-4) != std::string::npos) {
        if (ManagerComponentServer) {
            mtsDescriptionComponent componentInfo;
            componentInfo.ProcessName = processName;
            componentInfo.ComponentName = componentName.substr(0, componentName.length()-4);
            componentInfo.ClassName = "?";
            ManagerComponentServer->AddComponentEvent(componentInfo);
        }

        ProcessMapChange.Unlock();

        return true;
    }

    bool ret = componentMap->AddItem(componentName, 0);
    if (!ret) {
        CMN_LOG_CLASS_RUN_ERROR << "AddComponent: failed to add component: "
                                << "\"" << processName << ":" << componentName << "\"" << std::endl;
    }

    ProcessMapChange.Unlock();

    return ret;
}

bool mtsManagerGlobal::FindComponent(const std::string & processName, const std::string & componentName) const
{
    if (!FindProcess(processName)) {
        //CMN_LOG_CLASS_RUN_ERROR << "FindComponent: no process found: " << "\"" << processName << "\"" << std::endl;
        return false;
    }

    ComponentMapType * componentMap = ProcessMap.GetItem(processName);
    if (!componentMap) {
        return false;
    }

    return componentMap->FindItem(componentName);
}

bool mtsManagerGlobal::RemoveComponent(const std::string & processName, const std::string & componentName, const bool lock)
{
    // Check if the component exists
    if (!FindComponent(processName, componentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveComponent: no component found: "
                                << "\"" << processName << ":" << componentName << "\"" << std::endl;
        return false;
    }

    if (lock) {
        ProcessMapChange.Lock();
    }

    bool ret = true;
    ComponentMapType * componentMap = ProcessMap.GetItem(processName);
    if (!componentMap) {
        return true; // component has already been removed
    }

    // If the component being removed has interfaces, they should be removed first
    InterfaceMapType * interfaceMap = componentMap->GetItem(componentName);
    if (interfaceMap) {
        InterfaceMapElementType::iterator it;

        // Remove all the required interfaces that the component has
        it = interfaceMap->InterfaceRequiredOrInputMap.GetMap().begin();
        while (it != interfaceMap->InterfaceRequiredOrInputMap.GetMap().end()) {
            ret &= RemoveInterfaceRequiredOrInput(processName, componentName, it->first, false);
            it = interfaceMap->InterfaceRequiredOrInputMap.GetMap().begin();
        }

        // Remove all the provided interfaces that the component has
        it = interfaceMap->InterfaceProvidedOrOutputMap.GetMap().begin();
        while (it != interfaceMap->InterfaceProvidedOrOutputMap.GetMap().end()) {
            ret &= RemoveInterfaceProvidedOrOutput(processName, componentName, it->first, false);
            it = interfaceMap->InterfaceProvidedOrOutputMap.GetMap().begin();
        }

        delete interfaceMap;
    }

    const std::string removedComponentName = componentName;

    // Remove MCC proxy from LCM_with_GCM
    /*
#if CISST_MTS_HAS_ICE
    if (processName == mtsManagerLocal::ProcessNameOfLCMWithGCM) {
        if (!LocalManager->RemoveComponent(componentName, false)) {
            CMN_LOG_CLASS_RUN_ERROR << "RemoveComponent: failed to remove component from LCM: " 
                << "\"" << processName << ":" << componentName << "\"" << std::endl;
            ret = false;
        }
    }
#endif
    */

    // Remove the component from GCM's component map
    ret &= componentMap->RemoveItem(componentName);

    if (lock) {
        ProcessMapChange.Unlock();
    }

    if (ret) {
        CMN_LOG_CLASS_RUN_VERBOSE << "RemoveComponent: removed component: " << "\"" << processName << ":" << removedComponentName << "\"" << std::endl;
    } else {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveComponent: failed to remove component: " << "\"" << processName << ":" << removedComponentName << "\"" << std::endl;
    }

    return ret;
}

//-------------------------------------------------------------------------
//  Interface Management
//-------------------------------------------------------------------------
bool mtsManagerGlobal::AddInterfaceProvidedOrOutput(const std::string & processName, 
    const std::string & componentName, const std::string & interfaceName)
{
    if (!FindComponent(processName, componentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "AddInterfaceProvidedOrOutput: no component found: "
            << "\"" << processName << ":" << componentName << "\"" << std::endl;
        return false;
    }

    ProcessMapChange.Lock();

    ComponentMapType * componentMap = ProcessMap.GetItem(processName);
    CMN_ASSERT(componentMap);

    InterfaceMapType * interfaceMap = componentMap->GetItem(componentName);
    // If there is no provided interface that was added to the component before
    if (interfaceMap == 0) {
        interfaceMap = new InterfaceMapType;
        (componentMap->GetMap())[componentName] = interfaceMap;
    }

    // Add the provided interface
    if (!interfaceMap->InterfaceProvidedOrOutputMap.AddItem(interfaceName, 0)) {
        CMN_LOG_CLASS_RUN_ERROR << "AddInterfaceProvidedOrOutput: failed to add provided/output interface: "
            << "\"" << processName << ":" << componentName << ":" << interfaceName << "\"" << std::endl;
        ProcessMapChange.Unlock();
        return false;
    }

    ProcessMapChange.Unlock();

    return true;
}


bool mtsManagerGlobal::AddInterfaceRequiredOrInput(const std::string & processName, 
    const std::string & componentName, const std::string & interfaceName)
{
    if (!FindComponent(processName, componentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "AddInterfaceRequiredOrInput: can't find a registered component: "
                                << "\"" << processName << "\" - \"" << componentName << "\"" << std::endl;
        return false;
    }

    ProcessMapChange.Lock();

    ComponentMapType * componentMap = ProcessMap.GetItem(processName);
    CMN_ASSERT(componentMap);

    InterfaceMapType * interfaceMap = componentMap->GetItem(componentName);
    // If there is no required interface that was added to the component before
    if (interfaceMap == 0) {
        interfaceMap = new InterfaceMapType;
        (componentMap->GetMap())[componentName] = interfaceMap;
    }

    // Add the required interface
    if (!interfaceMap->InterfaceRequiredOrInputMap.AddItem(interfaceName, 0)) {
        CMN_LOG_CLASS_RUN_ERROR << "AddInterfaceRequiredOrInput: failed to add required/input interface: "
            << "\"" << processName << ":" << componentName << ":" << interfaceName << "\"" << std::endl;
        ProcessMapChange.Unlock();
        return false;
    }

    ProcessMapChange.Unlock();

    return true;
}

bool mtsManagerGlobal::FindInterfaceProvidedOrOutput(const std::string & processName,
                                                     const std::string & componentName,
                                                     const std::string & interfaceName) const
{
    if (!FindComponent(processName, componentName)) {
        //CMN_LOG_CLASS_RUN_ERROR << "FindInterfaceProvidedOrOutput: failed to find component: "
        //    << "\"" << processName << ":" << componentName << "\"" << std::endl;
        return false;
    }

    ComponentMapType * componentMap = ProcessMap.GetItem(processName);
    if (!componentMap) return false;

    InterfaceMapType * interfaceMap = componentMap->GetItem(componentName);
    if (!interfaceMap) return false;

    return interfaceMap->InterfaceProvidedOrOutputMap.FindItem(interfaceName);
}

bool mtsManagerGlobal::FindInterfaceRequiredOrInput(const std::string & processName,
                                                    const std::string & componentName,
                                                    const std::string & interfaceName) const
{
    if (!FindComponent(processName, componentName)) {
        //CMN_LOG_CLASS_RUN_ERROR << "FindInterfaceRequiredOrInput: failed to find component: "
        //    << "\"" << processName << ":" << componentName << "\"" << std::endl;
        return false;
    }

    ComponentMapType * componentMap = ProcessMap.GetItem(processName);
    if (!componentMap) return false;

    InterfaceMapType * interfaceMap = componentMap->GetItem(componentName);
    if (!interfaceMap) return false;

    return interfaceMap->InterfaceRequiredOrInputMap.FindItem(interfaceName);
}

bool mtsManagerGlobal::RemoveInterfaceProvidedOrOutput(const std::string & processName,
                                                       const std::string & componentName,
                                                       const std::string & interfaceName,
                                                       const bool lock)
{
    // Check if the process exists
    if (!ProcessMap.FindItem(processName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceProvidedOrOutput: no process found: \"" 
            << processName << "\"" << std::endl;
        return false;
    }

    if (lock) {
        ProcessMapChange.Lock();
    }

    // Check if the component exists
    ComponentMapType * componentMap = ProcessMap.GetItem(processName);
    if (!componentMap) {
        return true; // component has already been removed
    }

    if (!componentMap->FindItem(componentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceProvidedOrOutput: no component found: \"" 
            << processName << ":" << componentName << "\"" << std::endl;
        if (lock) {
            ProcessMapChange.Unlock();
        }
        return false;
    }

    // Check if the provided interface exists
    InterfaceMapType * interfaceMap = componentMap->GetItem(componentName);
    CMN_ASSERT(interfaceMap);

    if (!interfaceMap->InterfaceProvidedOrOutputMap.FindItem(interfaceName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceProvidedOrOutput: no provided interface found: \"" 
            << mtsManagerGlobal::GetInterfaceUID(processName, componentName, interfaceName) << "\"" << std::endl;
        if (lock) {
            ProcessMapChange.Unlock();
        }
        return false;
    }

    // When connectionMap is not NULL, all the connections that the provided 
    // interface has should be removed first
    bool ret = true;
    ConnectionIDListType * list = interfaceMap->InterfaceProvidedOrOutputMap.GetItem(interfaceName);
    if (list) {
        if (list->size()) {
            ConnectionIDListType::iterator it = list->begin();
            const ConnectionIDListType::iterator itEnd = list->end();
            for (; it != itEnd; ++it) {
                if (!Disconnect(*it)) {
                    CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceProvidedOrOutput: failed to disconnect (id: \"" << *it 
                        << "\"): \""
                        << mtsManagerGlobal::GetInterfaceUID(processName, componentName, interfaceName) << "\"" << std::endl;
                    return false;
                }
            }
            delete list;
        }
    }

    const std::string interfaceUID = mtsManagerGlobal::GetInterfaceUID(processName, componentName, interfaceName);

    // Remove the provided interface from provided interface map
    ret &= interfaceMap->InterfaceProvidedOrOutputMap.RemoveItem(interfaceName);

    if (lock) {
        ProcessMapChange.Unlock();
    }

    if (ret) {
        CMN_LOG_CLASS_RUN_VERBOSE << "RemoveInterfaceProvidedOrOutput: removed provided interface: \"" << interfaceUID << "\"" << std::endl;
    } else {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceProvidedOrOutput: failed to removed provided interface: \"" << interfaceUID << "\"" << std::endl;
    }

    return ret;
}

bool mtsManagerGlobal::RemoveInterfaceRequiredOrInput(const std::string & processName,
                                                      const std::string & componentName,
                                                      const std::string & interfaceName, 
                                                      const bool lock)
{
    // Check if the process exists
    if (!ProcessMap.FindItem(processName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceRequiredOrInput: no process found: \"" 
            << processName << "\"" << std::endl;
        return false;
    }

    if (lock) {
        ProcessMapChange.Lock();
    }

    // Check if the component exists
    ComponentMapType * componentMap = ProcessMap.GetItem(processName);
    if (!componentMap) {
        return true; // component has already been removed
    }

    if (!componentMap->FindItem(componentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceRequiredOrInput: no component found: \"" 
            << processName << ":" << componentName << "\"" << std::endl;
        if (lock) {
            ProcessMapChange.Unlock();
        }
        return false;
    }

    // Check if the required interface exists
    InterfaceMapType * interfaceMap = componentMap->GetItem(componentName);
    CMN_ASSERT(interfaceMap);

    if (!interfaceMap->InterfaceRequiredOrInputMap.FindItem(interfaceName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceRequiredOrInput: no required interface found: \"" 
            << mtsManagerGlobal::GetInterfaceUID(processName, componentName, interfaceName) << "\"" << std::endl;
        if (lock) {
            ProcessMapChange.Unlock();
        }
        return false;
    }

    // When connectionMap is not NULL, a connection that the required interface 
    // has should be removed first
    bool ret = true;
    ConnectionIDListType * list = interfaceMap->InterfaceRequiredOrInputMap.GetItem(interfaceName);
    if (list) {
        if (list->size()) {
            const ConnectionIDType id = *list->begin();
            if (!Disconnect(id)) {
                CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceRequiredOrInput: failed to disconnect (id: \"" << id << "\"): \""
                    << mtsManagerGlobal::GetInterfaceUID(processName, componentName, interfaceName) << "\"" << std::endl;
                return false;
            }
            delete list;
        }
    }

    const std::string interfaceUID = mtsManagerGlobal::GetInterfaceUID(processName, componentName, interfaceName);

    // Remove the required interface from required interface map
    ret &= interfaceMap->InterfaceRequiredOrInputMap.RemoveItem(interfaceName);

    if (lock) {
        ProcessMapChange.Unlock();
    }

    if (ret) {
        CMN_LOG_CLASS_RUN_VERBOSE << "RemoveInterfaceRequiredOrInput: removed required interface: \"" << interfaceUID << "\"" << std::endl;
    } else {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceRequiredOrInput: failed to removed required interface: \"" << interfaceUID << "\"" << std::endl;
    }

    return ret;
}

//-------------------------------------------------------------------------
//  Connection Management
//-------------------------------------------------------------------------
int mtsManagerGlobal::Connect(const std::string & requestProcessName,
                              const std::string & clientProcessName, const std::string & clientComponentName,
                              const std::string & clientInterfaceRequiredName,
                              const std::string & serverProcessName, const std::string & serverComponentName,
                              const std::string & serverInterfaceProvidedName)
{
    std::vector<std::string> options;
    std::stringstream allOptions;
    std::ostream_iterator< std::string > output(allOptions, " ");
    bool interfacesSwapped = false;

    // MJ: commented out the following check to allow the 3rd process (not client 
    // process nor server process) to be able to initiate a connection.
    //if (requestProcessName != clientProcessName && requestProcessName != serverProcessName) {
    //    CMN_LOG_CLASS_INIT_ERROR << "Connect: invalid process is requesting connection: " << requestProcessName << std::endl;
    //    return -1;
    //}

    // Check if the required interface specified exists
    if (!FindInterfaceRequiredOrInput(clientProcessName, clientComponentName, clientInterfaceRequiredName)) {
        // Check if by any chance the parameters have been swapped
        if (FindInterfaceRequiredOrInput(clientProcessName, serverComponentName, serverInterfaceProvidedName)) {
            interfacesSwapped = true;
        } else {
            GetNamesOfInterfacesRequiredOrInput(clientProcessName, clientComponentName, options);
            if (options.size() == 0) {
                allOptions << "no required/input interface available for this component";
            } else {
                allOptions << "available required/input interface(s): ";
                std::copy(options.begin(), options.end(), output);
            }
            CMN_LOG_CLASS_INIT_ERROR << "Connect: no required/input interface found: \""
                                     << GetInterfaceUID(clientProcessName, clientComponentName, clientInterfaceRequiredName)
                                     << "\", " << allOptions.str() << std::endl;
            return -1;
        }
    }

    // Check if the provided interface specified exists
    if (!FindInterfaceProvidedOrOutput(serverProcessName, serverComponentName, serverInterfaceProvidedName)) {
        // Check if the interfaces have really been swapped
        if (interfacesSwapped && FindInterfaceProvidedOrOutput(serverProcessName, clientComponentName, clientInterfaceRequiredName)) {
            interfacesSwapped = true;
        } else {
            interfacesSwapped = false;
            GetNamesOfInterfacesProvidedOrOutput(serverProcessName, serverComponentName, options);
            if (options.size() == 0) {
                allOptions << "no provided/output interface available for this component";
            } else {
                allOptions << "available provided/output interface(s): ";
                std::copy(options.begin(), options.end(), output);
            }
            CMN_LOG_CLASS_INIT_ERROR << "Connect: no provided/output interface found: \""
                                     << GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceProvidedName)
                                     << "\", " << allOptions.str() << std::endl;
            return -1;
        }
    }
    
    // Handle swapped components and interfaces names
    std::string clientComponentNameActual, clientInterfaceRequiredNameActual, serverComponentNameActual, serverInterfaceProvidedNameActual;
    if (!interfacesSwapped) {
        clientComponentNameActual = clientComponentName;
        clientInterfaceRequiredNameActual = clientInterfaceRequiredName;
        serverComponentNameActual = serverComponentName;
        serverInterfaceProvidedNameActual = serverInterfaceProvidedName;
    } else {
        clientComponentNameActual = serverComponentName;
        clientInterfaceRequiredNameActual = serverInterfaceProvidedName;
        serverComponentNameActual = clientComponentName;
        serverInterfaceProvidedNameActual = clientInterfaceRequiredName;
    }

    // Check if the two interfaces are already connected to each other
    bool isAlreadyConnected = IsAlreadyConnected(mtsDescriptionConnection(
        clientProcessName, clientComponentNameActual, clientInterfaceRequiredNameActual,
        serverProcessName, serverComponentNameActual, serverInterfaceProvidedNameActual));
    if (isAlreadyConnected) {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to connect - already connected interfaces: \""
                                 << GetInterfaceUID(clientProcessName, clientComponentNameActual, clientInterfaceRequiredNameActual)
                                 << "\" - \""
                                 << GetInterfaceUID(serverProcessName, serverComponentNameActual, serverInterfaceProvidedNameActual)
                                 << "\"" << std::endl;
        return -1;
    }

    // In case of remote connection
#if CISST_MTS_HAS_ICE
    if (clientProcessName != serverProcessName) {
        // Term definitions:
        // - Server manager: local component manager that manages server component
        // - Client manager: local component manager that manages client component

        // STEP 1. Check if all component proxies exist.  If not, create as needed.
        //
        // Check if the server manager has client component proxy.
        const std::string clientComponentProxyName = GetComponentProxyName(clientProcessName, clientComponentNameActual);
        if (!FindComponent(serverProcessName, clientComponentProxyName)) {
            // If not, create one.
            if (LocalManager) {
                if (LocalManager->GetProcessName() == serverProcessName) {
                    if (!LocalManager->CreateComponentProxy(clientComponentProxyName, serverProcessName)) {
                        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to create client component proxy "
                            << "\"" << clientComponentProxyName << "\" in server process "
                            << "\"" << serverProcessName << "\"" << std::endl;
                        return -1;
                    }
                } else {
                    if (!LocalManagerConnected->CreateComponentProxy(clientComponentProxyName, serverProcessName)) {
                        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to create client component proxy "
                            << "\"" << clientComponentProxyName << "\" in server process "
                            << "\"" << serverProcessName << "\"" << std::endl;
                        return -1;
                    }
                }
            } else {
                if (!LocalManagerConnected->CreateComponentProxy(clientComponentProxyName, serverProcessName)) {
                    CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to create client component proxy "
                        << "\"" << clientComponentProxyName << "\" in server process "
                        << "\"" << serverProcessName << "\"" << std::endl;
                    return -1;
                }

            }

            AddToDisconnectedProcessCleanup(clientProcessName, serverProcessName, clientComponentProxyName);

            CMN_LOG_CLASS_INIT_VERBOSE << "Connect: successfully created client component proxy "
                                       << "\"" << clientComponentProxyName << "\" in server process "
                                       << "\"" << serverProcessName << "\"" << std::endl;
        }

        // Check if the client manager has server component proxy.
        const std::string serverComponentProxyName = GetComponentProxyName(serverProcessName, serverComponentNameActual);
        if (!FindComponent(clientProcessName, serverComponentProxyName)) {
            // If not, create one.
            if (LocalManager) {
                if (LocalManager->GetProcessName() == clientProcessName) {
                    if (!LocalManager->CreateComponentProxy(serverComponentProxyName, clientProcessName)) {
                        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to create server component proxy "
                            << "\"" << serverComponentProxyName << "\" in client process "
                            << "\"" << clientProcessName << "\"" << std::endl;
                        return -1;
                    }
                } else {
                    if (!LocalManagerConnected->CreateComponentProxy(serverComponentProxyName, clientProcessName)) {
                        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to create server component proxy "
                                                 << "\"" << serverComponentProxyName << "\" in client process "
                                                 << "\"" << clientProcessName << "\"" << std::endl;
                        return -1;
                    }
                }
            } else {
                if (!LocalManagerConnected->CreateComponentProxy(serverComponentProxyName, clientProcessName)) {
                    CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to create server component proxy "
                                             << "\"" << serverComponentProxyName << "\" in client process "
                                             << "\"" << clientProcessName << "\"" << std::endl;
                    return -1;
                }

            }

            AddToDisconnectedProcessCleanup(serverProcessName, clientProcessName, serverComponentProxyName);

            CMN_LOG_CLASS_INIT_VERBOSE << "Connect: successfully created server component proxy "
                                       << "\"" << serverComponentProxyName << "\" in client process "
                                       << "\"" << clientProcessName << "\"" << std::endl;
        }

        // STEP 2. Check if all interface proxies exist.  If not, create proxies
        // as needed.
        //
        // Note that, under the current design, a provided interface can have
        // multiple connections with more than one required interface, whereas a 
        // required interface can have only one connection with a provided interface.
        // Thus, a required interface proxy is created whenever a new connection is
        // established while a provided interface proxy is created only once when
        // a client component does not have it.

        // Check if a provided interface proxy already exists at client side.
        bool foundProvidedInterfaceProxy = FindInterfaceProvidedOrOutput(
            clientProcessName, serverComponentProxyName, serverInterfaceProvidedNameActual);

        // Check if a required interface proxy already exists at server side.
        bool foundRequiredInterfaceProxy = FindInterfaceRequiredOrInput(
            serverProcessName, clientComponentProxyName, clientInterfaceRequiredNameActual);

        // Extract information about the two interfaces specified. The GCM then
        // deliver this information to the two local component managers so that 
        // they can create interface proxies as needed.

        // A required interface proxy at the server side shoulod be created first 
        // because the id of command proxies and event handler proxies at the 
        // client side are set as pointers to function proxy and event generator 
        // objects of the required interface proxy at the side side.

        // Create required interface proxy
        if (!foundRequiredInterfaceProxy) {
            // Extract required interface information from the client process
            InterfaceRequiredDescription requiredInterfaceDescription;

            if (LocalManager) {
                if (LocalManager->GetProcessName() == clientProcessName) {
                    if (!LocalManager->GetInterfaceRequiredDescription(
                            clientComponentNameActual, clientInterfaceRequiredNameActual, requiredInterfaceDescription, clientProcessName))
                    {
                        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to get required interface description: "
                                                 << GetInterfaceUID(clientProcessName, clientComponentNameActual, clientInterfaceRequiredNameActual) << std::endl;
                        return -1;
                    }
                } else {
                    if (!LocalManagerConnected->GetInterfaceRequiredDescription(
                            clientComponentNameActual, clientInterfaceRequiredNameActual, requiredInterfaceDescription, clientProcessName))
                    {
                        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to get required interface description: "
                                                 << GetInterfaceUID(clientProcessName, clientComponentNameActual, clientInterfaceRequiredNameActual) << std::endl;
                        return -1;
                    }
                }

                // Let the server process create required interface proxy
                if (LocalManager->GetProcessName() == serverProcessName) {
                    if (!LocalManager->CreateInterfaceRequiredProxy(
                            clientComponentProxyName, requiredInterfaceDescription, serverProcessName))
                    {
                        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to create required interface proxy in server process: "
                                                 << "\"" << clientComponentProxyName << "\" in \"" << serverProcessName << "\"" << std::endl;
                        return -1;
                    }
                } else {
                    if (!LocalManagerConnected->CreateInterfaceRequiredProxy(
                            clientComponentProxyName, requiredInterfaceDescription, serverProcessName))
                    {
                        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to create required interface proxy in server process: "
                                                 << "\"" << clientComponentProxyName << "\" in \"" << serverProcessName << "\"" << std::endl;
                        return -1;
                    }
                }
            } else {
                if (!LocalManagerConnected->GetInterfaceRequiredDescription(
                        clientComponentNameActual, clientInterfaceRequiredNameActual, requiredInterfaceDescription, clientProcessName))
                {
                    CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to get required interface description: "
                                             << GetInterfaceUID(clientProcessName, clientComponentNameActual, clientInterfaceRequiredNameActual) << std::endl;
                    return -1;
                }

                // Let the server process create required interface proxy
                if (!LocalManagerConnected->CreateInterfaceRequiredProxy(
                        clientComponentProxyName, requiredInterfaceDescription, serverProcessName))
                {
                    CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to create required interface proxy in server process: "
                                             << "\"" << clientComponentProxyName << "\" in \"" << serverProcessName << "\"" << std::endl;
                    return -1;
                }
            }
        }

        // Create provided interface proxy
        if (!foundProvidedInterfaceProxy) {
            // Extract provided interface information from the server process
            InterfaceProvidedDescription providedInterfaceDescription;
            if (LocalManager) {
                if (LocalManager->GetProcessName() == serverProcessName) {
                    if (!LocalManager->GetInterfaceProvidedDescription(
                            serverComponentName, serverInterfaceProvidedNameActual, providedInterfaceDescription, serverProcessName))
                    {
                        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to get provided interface description: "
                                                 << GetInterfaceUID(serverProcessName, serverComponentNameActual, serverInterfaceProvidedNameActual) << std::endl;
                        return -1;
                    }
                } else {
                    if (!LocalManagerConnected->GetInterfaceProvidedDescription(
                        serverComponentName, serverInterfaceProvidedNameActual, providedInterfaceDescription, serverProcessName))
                    {
                        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to get provided interface description: "
                            << GetInterfaceUID(serverProcessName, serverComponentNameActual, serverInterfaceProvidedNameActual) << std::endl;
                        return -1;
                    }

                }

                // Let the client process create provided interface proxy
                if (LocalManager->GetProcessName() == clientProcessName) {
                    if (!LocalManager->CreateInterfaceProvidedProxy(
                            serverComponentProxyName, providedInterfaceDescription, clientProcessName))
                    {
                        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to create provided interface proxy in client process: "
                                                 << "\"" << serverComponentProxyName << "\" in \"" << clientProcessName << "\"" << std::endl;
                        return -1;
                    }
                } else {
                    if (!LocalManagerConnected->CreateInterfaceProvidedProxy(
                            serverComponentProxyName, providedInterfaceDescription, clientProcessName))
                    {
                        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to create provided interface proxy in client process: "
                                                 << "\"" << serverComponentProxyName << "\" in \"" << clientProcessName << "\"" << std::endl;
                        return -1;
                    }
                }
            } else {
                if (!LocalManagerConnected->GetInterfaceProvidedDescription(
                        serverComponentName, serverInterfaceProvidedNameActual, providedInterfaceDescription, serverProcessName))
                {
                    CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to get provided interface description: "
                                             << GetInterfaceUID(serverProcessName, serverComponentNameActual, serverInterfaceProvidedNameActual) << std::endl;
                    return -1;
                }

                // Let the client process create provided interface proxy
                if (!LocalManagerConnected->CreateInterfaceProvidedProxy(
                        serverComponentProxyName, providedInterfaceDescription, clientProcessName))
                {
                    CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to create provided interface proxy in client process: "
                                             << "\"" << serverComponentProxyName << "\" in \"" << clientProcessName << "\"" << std::endl;
                    return -1;
                }
            }
        }
    }
#endif

    ConnectionChange.Lock();

    // Assign new connection id
    ConnectionIDType thisConnectionID = ConnectionID + 1;

    // STEP 3. Update ProcessMap
    //
    // Add this connection to required interface's connection list
    if (!AddConnectionToInterfaceRequiredOrInput(
            clientProcessName, clientComponentNameActual, clientInterfaceRequiredNameActual, thisConnectionID))
    {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to add connection information to required interface: \""
            << GetInterfaceUID(clientProcessName, clientComponentNameActual, clientInterfaceRequiredNameActual)
            << "\"" << std::endl;
        ConnectionChange.Unlock();
        return -1;
    }

    // Add this connection to provided interface's connection list
    if (!AddConnectionToInterfaceProvidedOrOutput(
            serverProcessName, serverComponentNameActual, serverInterfaceProvidedNameActual, thisConnectionID))
    {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to add connection information to provided interface: \""
            << GetInterfaceUID(serverProcessName, serverComponentNameActual, serverInterfaceProvidedNameActual)
            << "\"" << std::endl;
        ConnectionChange.Unlock();
        return -1;
    }

    // STEP 4. Update ConnectionMap
    //
    mtsDescriptionConnection description(
        clientProcessName, clientComponentNameActual, clientInterfaceRequiredNameActual,
        serverProcessName, serverComponentNameActual, serverInterfaceProvidedNameActual,
        thisConnectionID);
    mtsConnection connection(description, requestProcessName);

    ConnectionMap.insert(std::make_pair(thisConnectionID, connection));

    // STEP 5. Post-processings
    //
    // Increase connection id
    ++ConnectionID;

    // Send connection event to ManagerComponentServer
    if (ManagerComponentServer) {
        ManagerComponentServer->AddConnectionEvent(description);
    }

    ConnectionChange.Unlock();

    CMN_LOG_CLASS_INIT_VERBOSE << "Connect: successfully connected, new connection id [ " << thisConnectionID << " ] "
        << "for \""
        << GetInterfaceUID(clientProcessName, clientComponentNameActual, clientInterfaceRequiredNameActual) 
        << "\" - \""
        << GetInterfaceUID(serverProcessName, serverComponentNameActual, serverInterfaceProvidedNameActual) 
        << "\"" << std::endl;

    return thisConnectionID;
}

#if CISST_MTS_HAS_ICE
void mtsManagerGlobal::CheckConnectConfirmTimeout(void)
{
    if (ConnectionMap.empty()) return;

    mtsConnection * connection = 0;
    ConnectionMapType::iterator it = ConnectionMap.begin();
    while (it != ConnectionMap.end()) {
        connection = &it->second;

        // Skip timeout check if connection was already confirmed
        if (connection->IsConnected()) {
            ++it;
            continue;
        }

        // Timeout check
        if (!connection->CheckTimeout()) {
            ++it;
            continue;
        }

        // Active disconnect (to clean up pending connect request)
        mtsDescriptionConnection description;
        connection->GetDescriptionConnection(description);

        CMN_LOG_CLASS_INIT_ERROR << "ConnectCheckTimeout: Disconnect due to connect confirm timout: " << description << std::endl;

        if (!Disconnect(connection->GetConnectionID())) {
            CMN_LOG_CLASS_INIT_ERROR << "ConnectCheckTimeout: failed to disconnect pending connection (connection id: "
                << connection->GetConnectionID() << std::endl;
        } else {
            CMN_LOG_CLASS_INIT_VERBOSE << "ConnectCheckTimeout: successfully disconnected pending connection (connection id: "
                << connection->GetConnectionID() << std::endl;
        }

        // Reset iterator since Disconnect above invalidates ConnectionMap
        it = ConnectionMap.begin();
    }
}
#endif

bool mtsManagerGlobal::ConnectConfirm(const ConnectionIDType connectionID)
{
    ConnectionMapType::iterator it = ConnectionMap.find(connectionID);
    if (it == ConnectionMap.end()) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectConfirm: invalid connection id: " << connectionID << std::endl;
        return false;
    }

    // Mark connection as connected
    it->second.SetConnected();

    CMN_LOG_CLASS_INIT_VERBOSE << "ConnectConfirm: confirmed connection id [ " << connectionID << " ]" << std::endl;

    ShowInternalStructure();

    return true;
}

void mtsManagerGlobal::AddToDisconnectedProcessCleanup(const std::string & sourceProcessName, 
    const std::string & targetProcessName, const std::string & targetComponentProxyName)
{
    DisconnectedProcessCleanupMapChange.Lock();
    CleanupElementListType * list = DisconnectedProcessCleanupMap.GetItem(sourceProcessName);
    // If new source process
    if (!list) {
        list = new CleanupElementListType;
        DisconnectedProcessCleanupMap.AddItem(sourceProcessName, list);
    }

    CleanupElementType element;
    element.ProcessName = targetProcessName;
    element.ComponentProxyName = targetComponentProxyName;

    list->push_back(element);

    DisconnectedProcessCleanupMapChange.Unlock();
}

//  MJ: Design of Disconnect()
//
//  1. Any thread can call Disconnect() when peer-proxy disconnection or processing 
//     errors is detected.  Note that multiple disconnect requests with the 
//     same connection id can be made simultaneously.
//  2. Check if the connection id is already queued in the "disconnect waiting queue" 
//     or the "disconnected queue."  If yes, return true immediately (because 
//     the connection will be disconnected soon)
//  3. If no, push the connection id to the disconnect waiting queue which will 
//     be periodically processed.
//  4. In the separate disconnect processing thread which the GCM will internally 
//     create and manage, one connection id is dequeued from the disconnect waiting 
//     queue, gets processed, and is enqueued to the disconnected queue.
//
void mtsManagerGlobal::DisconnectInternal(void)
{
    if (QueueDisconnectWaiting.empty()) return;

    DisconnectQueueType::iterator it;
    ConnectionIDType connectionID;
    mtsConnection * connectionInfo;
    
#if !CISST_MTS_HAS_ICE
    std::string processName;
#endif
    std::string serverComponentName;
    std::string serverInterfaceName;
    std::string serverProcessName;
    std::string clientProcessName;
    std::string clientComponentName;
    std::string clientInterfaceName;
    mtsDescriptionConnection connection;

    while (!QueueDisconnectWaiting.empty()) {
        QueueDisconnectWaitingChange.Lock();
        it = QueueDisconnectWaiting.begin();
        QueueDisconnectWaitingChange.Unlock();

        connectionID = it->first;
        connectionInfo = GetConnectionInformation(connectionID);
        CMN_ASSERT(connectionInfo);

        connectionInfo->GetDescriptionConnection(connection);

        serverProcessName = connectionInfo->GetServerProcessName();
        serverComponentName = connectionInfo->GetServerComponentName();
        serverInterfaceName = connectionInfo->GetServerInterfaceProvidedName();
        clientProcessName = connectionInfo->GetClientProcessName();
        clientComponentName = connectionInfo->GetClientComponentName();
        clientInterfaceName = connectionInfo->GetClientInterfaceRequiredName();

#if !CISST_MTS_HAS_ICE
        // Step 1. Let LCM disconnect local connection
        CMN_ASSERT(serverProcessName == clientProcessName);
        processName = serverProcessName;

        ManagerComponentServer->ComponentDisconnect(processName, connection);
        osaSleep(100 * cmn_ms); // MJ TODO: remove this when ComponentDisconnect() becomes blocking

        // Step 2. Clean up GCM's internal data structure - Remove connection between user components
        if (!RemoveConnectionOfInterfaceRequiredOrInput(processName, clientComponentName, clientInterfaceName, connectionID)) {
            CMN_LOG_CLASS_RUN_ERROR << "Disconnect: failed to remove required interface's connection info: "
                << "[ " << connectionID << " ] - "
                << mtsManagerGlobal::GetInterfaceUID(processName, clientComponentName, clientInterfaceName)
                << std::endl;
        }
        if (!RemoveConnectionOfInterfaceProvidedOrOutput(processName, serverComponentName, serverInterfaceName, connectionID)) {
            CMN_LOG_CLASS_RUN_ERROR << "Disconnect: failed to remove provided interface's connection info: "
                << "[ " << connectionID << " ] - "
                << mtsManagerGlobal::GetInterfaceUID(processName, serverComponentName, serverInterfaceName)
                << std::endl;
        }

        // Step 3. Special handling of MCC-involved connections: 
        //
        // When removing connection between (InterfaceComponentRequired, InterfaceInternalProvided),
        // clean up required interface "InterfaceComponentRequiredFor(UserComponent)" of 
        // manager component "LCM_MCC" together.
        const std::string nameOfLCM_MCC = // LCM_MCC
            mtsManagerComponentBase::GetNameOfManagerComponentClient(mtsManagerLocal::ProcessNameOfLCMDefault);
        if ((serverComponentName != nameOfLCM_MCC) && (clientComponentName == nameOfLCM_MCC) &&
            (serverInterfaceName == mtsManagerComponentBase::GetNameOfInterfaceInternalProvided())) 
        {
            const std::string interfaceName = // InterfaceComponentRequiredForXXXX
                mtsManagerComponentBase::GetNameOfInterfaceComponentRequiredFor(serverComponentName);
            const std::string interfaceUID = mtsManagerGlobal::GetInterfaceUID(processName, clientComponentName, interfaceName);

            // Remove from LCM: see mtsManagerComponentClient::DisconnectLocally
            // Remove from GCM
            if (!RemoveInterfaceRequiredOrInput(processName, clientComponentName, interfaceName)) {
                CMN_LOG_CLASS_RUN_ERROR << "Disconnect: failed to remove InterfaceComponent's required interface from GCM: "
                    << "[ " << connectionID << " ] - \"" << interfaceUID << "\"" << std::endl;
            }
        }
#else
        // Step 1.  Disconnect Client from ServerProxy first.  This will pause 
        // client component (by LCM::DisconnectLocally()) for a short time.
        //
        // LCM::DisconnectLocally()
        const bool isMCCforGCM = ((serverProcessName == clientProcessName) & 
                                  (clientComponentName == mtsManagerComponentBase::GetNameOfManagerComponentClientFor(
                                    mtsManagerLocal::ProcessNameOfLCMWithGCM)));
        const std::string serverComponentProxyName = GetComponentProxyName(serverProcessName, serverComponentName);
        if (FindProcess(clientProcessName)) {
            mtsDescriptionConnection arg = connection;
            if (!isMCCforGCM) {
                arg.Server.ProcessName = clientProcessName;
                arg.Server.ComponentName = serverComponentProxyName;
            }
            ManagerComponentServer->ComponentDisconnect(clientProcessName, arg); // MJ TODO: should check return value
        }

        // Remove connection information of provided interface proxy from GCM
        if (FindInterfaceProvidedOrOutput(clientProcessName, serverComponentProxyName, serverInterfaceName)) {
            if (!RemoveConnectionOfInterfaceProvidedOrOutput(
                    clientProcessName, serverComponentProxyName, serverInterfaceName, connectionID))
            {
                CMN_LOG_CLASS_RUN_ERROR << "Disconnect: failed to remove provided interface connection info from client: "
                    << "[ " << connectionID << " ] "
                    << "\"" << mtsManagerGlobal::GetInterfaceUID(clientProcessName, serverComponentProxyName, serverInterfaceName)
                    << "\"" << std::endl;
            }
        }
        if (FindInterfaceRequiredOrInput(clientProcessName, clientComponentName, clientInterfaceName)) {
            if (!RemoveConnectionOfInterfaceRequiredOrInput(
                    clientProcessName, clientComponentName, clientInterfaceName, connectionID))
            {
                CMN_LOG_CLASS_RUN_ERROR << "Disconnect: failed to remove required interface connection info from client: "
                    << "[ " << connectionID << " ] "
                    << "\"" << mtsManagerGlobal::GetInterfaceUID(clientProcessName, clientComponentName, clientInterfaceName)
                    << "\"" << std::endl;
            }
        }

        // Remove provided interface proxy from LCM and GCM
        if (FindComponent(clientProcessName, serverComponentProxyName)) {
            // From LCM
            if (clientProcessName !=  mtsManagerLocal::ProcessNameOfLCMWithGCM) {
                if (FindProcess(clientProcessName))
                    LocalManagerConnected->RemoveInterfaceProvidedProxy(serverComponentProxyName, serverInterfaceName, clientProcessName);
            } else {
                LocalManager->RemoveInterfaceProvided(serverComponentProxyName, serverInterfaceName);
            }

            // From GCM
            if (FindProcess(clientProcessName))
                RemoveInterfaceProvidedOrOutput(clientProcessName, serverComponentProxyName, serverInterfaceName);

            // Remove server component proxy if necessary
            int numOfInterfaces = GetNumberOfInterfaces(clientProcessName, serverComponentProxyName, false);
            if (numOfInterfaces == 0) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Disconnect: remove empty server component proxy: "
                    << "[ " << connectionID << " ] "
                    << "\"" << clientProcessName << ":" << serverComponentProxyName << "\"" << std::endl;

                if (clientProcessName !=  mtsManagerLocal::ProcessNameOfLCMWithGCM) {
                    if (FindProcess(clientProcessName))
                        LocalManagerConnected->RemoveComponentProxy(serverComponentProxyName, clientProcessName);
                }
            }
        }

        // Step 2.  Disconnect ClientProxy from Server
        //
        // LCM::DisconnectLocally()
        const std::string clientComponentProxyName = GetComponentProxyName(clientProcessName, clientComponentName);
        if (FindProcess(serverProcessName)) {
            mtsDescriptionConnection arg = connection;
            if (!isMCCforGCM) {
                arg.Client.ProcessName = serverProcessName;
                arg.Client.ComponentName = clientComponentProxyName;
            }
            ManagerComponentServer->ComponentDisconnect(serverProcessName, arg); // MJ TODO: should check return value
        }

        // Remove connection information of required interface proxy from GCM
        if (FindInterfaceRequiredOrInput(serverProcessName, clientComponentProxyName, clientInterfaceName)) {
            if (!RemoveConnectionOfInterfaceRequiredOrInput(
                    serverProcessName, clientComponentProxyName, clientInterfaceName, connectionID))
            {
                CMN_LOG_CLASS_RUN_ERROR << "Disconnect: failed to remove required interface connection info from server: "
                    << "[ " << connectionID << " ] "
                    << "\"" << mtsManagerGlobal::GetInterfaceUID(serverProcessName, clientComponentProxyName, clientInterfaceName)
                    << "\"" << std::endl;
            }
        }
        if (FindInterfaceProvidedOrOutput(serverProcessName, serverComponentName, serverInterfaceName)) {
            if (!RemoveConnectionOfInterfaceProvidedOrOutput(
                    serverProcessName, serverComponentName, serverInterfaceName, connectionID))
            {
                CMN_LOG_CLASS_RUN_ERROR << "Disconnect: failed to remove provided interface connection info from server: "
                    << "[ " << connectionID << " ] "
                    << "\"" << mtsManagerGlobal::GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceName)
                    << "\"" << std::endl;
            }
        }

        // Remove required interface proxy from LCM and GCM
        if (FindComponent(serverProcessName, clientComponentProxyName)) {
            // From LCM
            if (serverProcessName !=  mtsManagerLocal::ProcessNameOfLCMWithGCM) {
                if (FindProcess(serverProcessName))
                    LocalManagerConnected->RemoveInterfaceRequiredProxy(clientComponentProxyName, clientInterfaceName, serverProcessName);
            } else {
                LocalManager->RemoveInterfaceRequired(clientComponentProxyName, clientInterfaceName);
            }

            // From GCM
            if (FindProcess(serverProcessName))
                RemoveInterfaceRequiredOrInput(serverProcessName, clientComponentProxyName, clientInterfaceName);

            // Remove client component proxy
            if (serverProcessName !=  mtsManagerLocal::ProcessNameOfLCMWithGCM) {
                if (FindProcess(serverProcessName))
                    LocalManagerConnected->RemoveComponentProxy(clientComponentProxyName, serverProcessName);
            } else {
                LocalManager->RemoveComponent(clientComponentProxyName);
            }
        }

        // Step 3. Special handling of MCC/MCS-involved connections
        //
        // Remove InterfaceGCMRequired instance (InterfaceGCMRequired - InterfaceLCMProvided)
        if (mtsManagerComponentBase::IsNameOfInterfaceLCMProvided(serverInterfaceName)) {
            if (!ManagerComponentServer->DisconnectCleanup(serverProcessName)) {
                CMN_LOG_CLASS_RUN_ERROR << "Disconnect: failed to clean up InterfaceGCM's required interface for connection "
                    << "[ " << connectionID << " ] - \"" << serverProcessName << "\"" << std::endl;
            }
            
            const std::string componentName = mtsManagerComponentBase::GetNameOfManagerComponentServer();
            const std::string interfaceName = mtsManagerComponentBase::GetNameOfInterfaceGCMRequiredFor(serverProcessName);
            const std::string interfaceUID = mtsManagerGlobal::GetInterfaceUID(clientProcessName, componentName, interfaceName);

            // Remove required interface from LCM
            if (FindComponent(clientProcessName, componentName)) {
                if (clientProcessName == mtsManagerLocal::ProcessNameOfLCMWithGCM) {
                    if (!LocalManager->RemoveInterfaceRequired(componentName, interfaceName)) {
                        CMN_LOG_CLASS_RUN_VERBOSE << "Disconnect: failed to remove required interface proxy: "
                            << "\"" << componentName << ":" << interfaceName << "\" from "
                            << "\"" << mtsManagerLocal::ProcessNameOfLCMWithGCM << "\"" << std::endl;
                    }
                } else {
                    if (FindProcess(clientProcessName))
                        LocalManagerConnected->RemoveInterfaceRequiredProxy(componentName, interfaceName, clientProcessName);
                }
            }
            // Remove required interface from GCM
            if (FindProcess(clientProcessName))
                RemoveInterfaceRequiredOrInput(clientProcessName, componentName, interfaceName);
        }
        // Remove InterfaceComponentRequired instance (InterfaceComponentRequired - InterfaceInternalProvided)
        else if (mtsManagerComponentBase::IsNameOfInterfaceInternalProvided(serverInterfaceName)) {
            if (isMCCforGCM) {
                const std::string componentName = mtsManagerComponentBase::GetNameOfManagerComponentClientFor(mtsManagerLocal::ProcessNameOfLCMWithGCM);
                const std::string interfaceName = clientInterfaceName;
                const std::string interfaceUID = mtsManagerGlobal::GetInterfaceUID(clientProcessName, componentName, clientInterfaceName);

                // Remove required interface from LCM
                if (FindComponent(clientProcessName, componentName)) {
                    if (clientProcessName ==  mtsManagerLocal::ProcessNameOfLCMWithGCM) {
                        if (!LocalManager->RemoveInterfaceRequired(componentName, interfaceName)) {
                            CMN_LOG_CLASS_RUN_VERBOSE << "Disconnect: failed to remove required interface proxy: "
                                << "\"" << componentName << ":" << interfaceName << "\" from "
                                << "\"" << mtsManagerLocal::ProcessNameOfLCMWithGCM << "\"" << std::endl;
                        }
                    } else {
                        if (FindProcess(clientProcessName))
                            LocalManagerConnected->RemoveInterfaceRequiredProxy(componentName, interfaceName, clientProcessName);
                    }
                }
                // Remove required interface from GCM
                if (FindProcess(clientProcessName))
                    RemoveInterfaceRequiredOrInput(clientProcessName, componentName, interfaceName);
            } else {
                //CMN_ASSERT(serverProcessName == clientProcessName);
                //CMN_ASSERT(clientComponentName == 
                //           mtsManagerComponentBase::GetNameOfManagerComponentClientFor(serverProcessName));
                //CMN_ASSERT(mtsManagerComponentBase::IsNameOfInterfaceComponentRequired(clientInterfaceName));

                const std::string interfaceUID = mtsManagerGlobal::GetInterfaceUID(clientProcessName, clientComponentName, clientInterfaceName);

                // Remove required interface from LCM
                if (FindProcess(clientProcessName))
                    LocalManagerConnected->RemoveInterfaceRequiredProxy(clientComponentName, clientInterfaceName, clientProcessName);
                // Remove required interface from GCM
                if (FindComponent(clientProcessName, clientComponentName))
                    RemoveInterfaceRequiredOrInput(clientProcessName, clientComponentName, clientInterfaceName);
            }
        }
#endif

        // Step 4. Clean up connection map
        ConnectionMapChange.Lock();
        ConnectionMapType::iterator itConnectionMap = ConnectionMap.find(connectionID);
        ConnectionMap.erase(itConnectionMap);
        ConnectionMapChange.Unlock();

        // Step 5. Enqueue the disconnected id to the disconnected queue
        // first, dequeue id from disconnect waiting queue
        QueueDisconnectWaitingChange.Lock();
        QueueDisconnectWaiting.erase(it);
        QueueDisconnectWaitingChange.Unlock();

        // enqueue id to disconnected queue
        // MJ: this is redundant check but intentionally added to make sure things work correctly
        DisconnectQueueType::const_iterator itDisconnected = QueueDisconnected.find(connectionID);
        CMN_ASSERT(itDisconnected == QueueDisconnected.end());
        QueueDisconnected.insert(std::make_pair(connectionID, connectionID));

        // Step 6. Send disconnection event to ManagerComponentServer
        if (ManagerComponentServer)
            ManagerComponentServer->RemoveConnectionEvent(connection);
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "Disconnect: successfully disconnected [ " << connectionID << " ] : "
                               << "\"" << serverComponentName << ":" << serverInterfaceName << " - " 
                               << clientComponentName << ":" << clientInterfaceName << "\"" << std::endl;
    ShowInternalStructure();
}

bool mtsManagerGlobal::Disconnect(const ConnectionIDType connectionID)
{
    DisconnectQueueType::const_iterator it1 = QueueDisconnectWaiting.find(connectionID);
    if (it1 != QueueDisconnectWaiting.end()) {
        return true;
    }

    DisconnectQueueType::const_iterator it2 = QueueDisconnected.find(connectionID);
    if (it2 != QueueDisconnected.end()) {
        return true;
    }

    ConnectionMapType::iterator it = ConnectionMap.find(connectionID);
    if (it == ConnectionMap.end()) {
        CMN_LOG_CLASS_INIT_ERROR << "Disconnect: invalid connection id: " << connectionID << std::endl;
        return false;
    }

    QueueDisconnectWaitingChange.Lock();
    QueueDisconnectWaiting.insert(std::make_pair(connectionID, connectionID));
    QueueDisconnectWaitingChange.Unlock();

    CMN_LOG_CLASS_INIT_VERBOSE << "Disconnect: queueud connection id [ " << connectionID
        << " ] to disconnect waiting queue" << std::endl;

    return true;
}

bool mtsManagerGlobal::Disconnect(const std::string & clientProcessName, const std::string & clientComponentName,
                                  const std::string & clientInterfaceRequiredName,
                                  const std::string & serverProcessName, const std::string & serverComponentName,
                                  const std::string & serverInterfaceProvidedName)
{
    ConnectionIDType id = GetConnectionID(clientProcessName, clientComponentName, clientInterfaceRequiredName);
    if (id == 0) {
        CMN_LOG_CLASS_INIT_ERROR << "Disconnect: no connection id found for "
            << "\"" << GetInterfaceUID(clientProcessName, clientComponentName, clientInterfaceRequiredName) << "\" - "
            << "\"" << GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceProvidedName) << std::endl;
        return false;
    }

    return Disconnect(id);
}

//-------------------------------------------------------------------------
//  Getters
//-------------------------------------------------------------------------
#if CISST_MTS_HAS_ICE
bool mtsManagerGlobal::IsProxyComponent(const std::string & componentName)
{
    // PK: Need to fix this to be more robust
    return (componentName.find("Proxy", componentName.length()-5) != std::string::npos);
}

const std::string mtsManagerGlobal::GetComponentProxyName(const std::string & processName, const std::string & componentName) 
{
    if (IsProxyComponent(componentName)) {
        return componentName;
    }

    return processName + "." + componentName + "Proxy";
}
#endif

const std::string mtsManagerGlobal::GetInterfaceUID(
    const std::string & processName, const std::string & componentName, const std::string & interfaceName)
{
    return processName + ":" + componentName + ":" + interfaceName;
}

void mtsManagerGlobal::GetNamesOfProcesses(std::vector<std::string>& namesOfProcesses) const
{
    std::vector<std::string> temp;
    ProcessMap.GetNames(temp);

    // Filter out local ICE proxy of type mtsManagerProxyServer.
    for (size_t i = 0; i < temp.size(); ++i) {
        if (temp[i] == "ManagerProxyServer") {
            continue;
        }
        namesOfProcesses.push_back(temp[i]);
    }
}

void mtsManagerGlobal::GetNamesOfComponents(const std::string & processName,
                                            std::vector<std::string>& namesOfComponents) const
{
    ComponentMapType * components = ProcessMap.GetItem(processName);
    if (!components) return;

    components->GetNames(namesOfComponents);
}

void mtsManagerGlobal::GetNamesOfInterfacesProvidedOrOutput(const std::string & processName,
                                                            const std::string & componentName,
                                                            std::vector<std::string>& namesOfInterfacesProvided) const
{
    ComponentMapType * components = ProcessMap.GetItem(processName);
    if (!components) return;

    InterfaceMapType * interfaces = components->GetItem(componentName);
    if (!interfaces) return;

    interfaces->InterfaceProvidedOrOutputMap.GetNames(namesOfInterfacesProvided);
}

void mtsManagerGlobal::GetNamesOfInterfacesRequiredOrInput(const std::string & processName,
                                                           const std::string & componentName,
                                                           std::vector<std::string>& namesOfInterfacesRequired) const
{
    ComponentMapType * components = ProcessMap.GetItem(processName);
    if (!components) return;

    InterfaceMapType * interfaces = components->GetItem(componentName);
    if (!interfaces) return;

    interfaces->InterfaceRequiredOrInputMap.GetNames(namesOfInterfacesRequired);

}

#if CISST_MTS_HAS_ICE
void mtsManagerGlobal::GetNamesOfCommands(const std::string & processName,
                                          const std::string & componentName,
                                          const std::string & providedInterfaceName,
                                          std::vector<std::string>& namesOfCommands) const
{
    if (LocalManager) {
        if (LocalManager->GetProcessName() == processName) {
            LocalManager->GetNamesOfCommands(namesOfCommands, componentName, providedInterfaceName, processName);
            return;
        }
    }

    if (LocalManagerConnected) {
        LocalManagerConnected->GetNamesOfCommands(namesOfCommands, componentName, providedInterfaceName, processName);
    }
}

void mtsManagerGlobal::GetNamesOfEventGenerators(const std::string & processName,
                                                 const std::string & componentName,
                                                 const std::string & providedInterfaceName,
                                                 std::vector<std::string>& namesOfEventGenerators) const
{
    if (LocalManager) {
        if (LocalManager->GetProcessName() == processName) {
            LocalManager->GetNamesOfEventGenerators(namesOfEventGenerators, componentName, providedInterfaceName, processName);
            return;
        }
    }

    if (LocalManagerConnected) {
        LocalManagerConnected->GetNamesOfEventGenerators(namesOfEventGenerators, componentName, providedInterfaceName, processName);
    }
}

void mtsManagerGlobal::GetNamesOfFunctions(const std::string & processName,
                                           const std::string & componentName,
                                           const std::string & requiredInterfaceName,
                                           std::vector<std::string>& namesOfFunctions) const
{
    if (LocalManager) {
        if (LocalManager->GetProcessName() == processName) {
            LocalManager->GetNamesOfFunctions(namesOfFunctions, componentName, requiredInterfaceName, processName);
            return;
        }
    }

    if (LocalManagerConnected) {
        LocalManagerConnected->GetNamesOfFunctions(namesOfFunctions, componentName, requiredInterfaceName, processName);
    }
}

void mtsManagerGlobal::GetNamesOfEventHandlers(const std::string & processName,
                                               const std::string & componentName,
                                               const std::string & requiredInterfaceName,
                                               std::vector<std::string>& namesOfEventHandlers) const
{
    if (LocalManager) {
        if (LocalManager->GetProcessName() == processName) {
            LocalManager->GetNamesOfEventHandlers(namesOfEventHandlers, componentName, requiredInterfaceName, processName);
            return;
        }
    }

    if (LocalManagerConnected) {
        LocalManagerConnected->GetNamesOfEventHandlers(namesOfEventHandlers, componentName, requiredInterfaceName, processName);
    }
}

void mtsManagerGlobal::GetDescriptionOfCommand(const std::string & processName,
                                               const std::string & componentName,
                                               const std::string & providedInterfaceName,
                                               const std::string & commandName,
                                               std::string & description) const
{
    if (LocalManager) {
        if (LocalManager->GetProcessName() == processName) {
            LocalManager->GetDescriptionOfCommand(description, componentName, providedInterfaceName, commandName, processName);
            return;
        }
    }

    if (LocalManagerConnected) {
        LocalManagerConnected->GetDescriptionOfCommand(description, componentName, providedInterfaceName, commandName, processName);
    }
}

void mtsManagerGlobal::GetDescriptionOfEventGenerator(const std::string & processName,
                                                      const std::string & componentName,
                                                      const std::string & providedInterfaceName,
                                                      const std::string & eventGeneratorName,
                                                      std::string & description) const
{
    if (LocalManager) {
        if (LocalManager->GetProcessName() == processName) {
            LocalManager->GetDescriptionOfEventGenerator(description, componentName, providedInterfaceName, eventGeneratorName, processName);
            return;
        }
    }

    if (LocalManagerConnected) {
        LocalManagerConnected->GetDescriptionOfEventGenerator(description, componentName, providedInterfaceName, eventGeneratorName, processName);
    }
}

void mtsManagerGlobal::GetDescriptionOfFunction(const std::string & processName,
                                                const std::string & componentName,
                                                const std::string & requiredInterfaceName,
                                                const std::string & functionName,
                                                std::string & description) const
{
    if (LocalManager) {
        if (LocalManager->GetProcessName() == processName) {
            LocalManager->GetDescriptionOfFunction(description, componentName, requiredInterfaceName, functionName, processName);
            return;
        }
    }

    if (LocalManagerConnected) {
        LocalManagerConnected->GetDescriptionOfFunction(description, componentName, requiredInterfaceName, functionName, processName);
    }
}

void mtsManagerGlobal::GetDescriptionOfEventHandler(const std::string & processName,
                                                    const std::string & componentName,
                                                    const std::string & requiredInterfaceName,
                                                    const std::string & eventHandlerName,
                                                    std::string & description) const
{
    if (LocalManager) {
        if (LocalManager->GetProcessName() == processName) {
            LocalManager->GetDescriptionOfEventHandler(description, componentName, requiredInterfaceName, eventHandlerName, processName);
            return;
        }
    }

    if (LocalManagerConnected) {
        LocalManagerConnected->GetDescriptionOfEventHandler(description, componentName, requiredInterfaceName, eventHandlerName, processName);
    }
}

void mtsManagerGlobal::GetArgumentInformation(const std::string & processName,
                                              const std::string & componentName,
                                              const std::string & providedInterfaceName,
                                              const std::string & commandName,
                                              std::string & argumentName,
                                              std::vector<std::string> & signalNames) const
{
    if (LocalManager) {
        if (LocalManager->GetProcessName() == processName) {
            LocalManager->GetArgumentInformation(argumentName, signalNames, componentName, providedInterfaceName, commandName, processName);
            return;
        }
    }

    if (LocalManagerConnected) {
        LocalManagerConnected->GetArgumentInformation(argumentName, signalNames, componentName, providedInterfaceName, commandName, processName);
    }
}

void mtsManagerGlobal::GetValuesOfCommand(const std::string & processName,
                                          const std::string & componentName,
                                          const std::string & providedInterfaceName,
                                          const std::string & commandName,
                                          const int scalarIndex,
                                          mtsManagerLocalInterface::SetOfValues & values) const
{
    if (LocalManager) {
        if (LocalManager->GetProcessName() == processName) {
            LocalManager->GetValuesOfCommand(values, componentName, providedInterfaceName, commandName, scalarIndex, processName);
            return;
        }
    }

    if (LocalManagerConnected) {
        LocalManagerConnected->GetValuesOfCommand(values, componentName, providedInterfaceName, commandName, scalarIndex, processName);
    }
}

#endif

//-------------------------------------------------------------------------
//  Networking
//-------------------------------------------------------------------------
#if CISST_MTS_HAS_ICE
bool mtsManagerGlobal::StartServer(void)
{
    // Create an instance of mtsComponentInterfaceProxyServer
    ProxyServer = new mtsManagerProxyServer("ManagerServerAdapter", mtsManagerProxyServer::GetManagerCommunicatorID());

    // Run proxy server
    if (!ProxyServer->StartProxy(this)) {
        CMN_LOG_CLASS_RUN_ERROR << "StartServer: Proxy failed to start: " << GetName() << std::endl;
        return false;
    }

    ProxyServer->GetLogger()->trace("mtsManagerGlobal", "Global component manager started.");

    // Register an instance of mtsComponentInterfaceProxyServer
    LocalManagerConnected = ProxyServer;

    return true;
}

bool mtsManagerGlobal::StopServer(void)
{
    if (!ProxyServer) {
        CMN_LOG_CLASS_RUN_ERROR << "StopServer: no proxy server found" << std::endl;
        return false;
    }

    if (!ProxyServer->IsActiveProxy()) {
        CMN_LOG_CLASS_RUN_ERROR << "StopServer: no active proxy server: " << "\"" << ProxyServer->GetProxyName() << "\"" << std::endl;
        return false;
    }

    LocalManagerConnected = 0;

    // Stop proxy server
    ProxyServer->GetLogger()->trace("mtsManagerGlobal", "Global component manager stopped.");
    ProxyServer->StopProxy();

    delete ProxyServer;

    return true;
}

bool mtsManagerGlobal::SetInterfaceProvidedProxyAccessInfo(const ConnectionIDType connectionID, const std::string & endpointInfo)
{
    mtsConnection * connection = GetConnectionInformation(connectionID);
    if (!connection) {
        CMN_LOG_CLASS_INIT_ERROR << "SetInterfaceProvidedProxyAccessInfo: no connection id found: " << connectionID << std::endl;
        return false;
    }

    connection->SetProxyAccessInfo(endpointInfo);

    CMN_LOG_CLASS_INIT_VERBOSE << "SetInterfaceProvidedProxyAccessInfo: (connection [ " << connectionID << " ]) "
        << "set proxy access information : " << endpointInfo << std::endl;

    return true;
}

bool mtsManagerGlobal::GetInterfaceProvidedProxyAccessInfo(const ConnectionIDType connectionID, std::string & endpointInfo)
{
    mtsConnection * connection = GetConnectionInformation(connectionID);
    if (!connection) {
        CMN_LOG_CLASS_INIT_ERROR << "GetInterfaceProvidedProxyAccessInfo: no connection id found: " << connectionID << std::endl;
        return false;
    }

    endpointInfo = connection->GetEndpointInfo();

    return true;
}

bool mtsManagerGlobal::GetInterfaceProvidedProxyAccessInfo(const std::string & clientProcessName,
    const std::string & serverProcessName, const std::string & serverComponentName, 
    const std::string & serverInterfaceProvidedName, std::string & endpointInfo)
{
    // Iteration may take a while
    ConnectionMapChange.Lock();

    ConnectionMapType::const_iterator it = ConnectionMap.begin();
    const ConnectionMapType::const_iterator itEnd = ConnectionMap.end();

    mtsDescriptionConnection description;
    for (; it != itEnd; ++it) {
        it->second.GetDescriptionConnection(description);
        if ((description.Client.ProcessName == clientProcessName) &&
            (description.Server.ProcessName == serverProcessName) &&
            (description.Server.ComponentName == serverComponentName) &&
            (description.Server.InterfaceName == serverInterfaceProvidedName))
        {
            endpointInfo = it->second.GetEndpointInfo();
            
            ConnectionMapChange.Unlock();

            return true;
        }
    }

    ConnectionMapChange.Unlock();

    return false;
}

bool mtsManagerGlobal::InitiateConnect(const ConnectionIDType connectionID)
{
    mtsConnection * connection = GetConnectionInformation(connectionID);
    if (!connection) {
        CMN_LOG_CLASS_INIT_ERROR << "InitiateConnect: invalid connection id: " << connectionID << std::endl;
        return false;
    }

    mtsManagerLocalInterface * localManagerClient = GetProcessObject(connection->GetClientProcessName());
    if (!localManagerClient) {
        CMN_LOG_CLASS_INIT_ERROR << "InitiateConnect: no LCM found that manages client process \"" << connection->GetClientProcessName() << "\"" << std::endl;
        return false;
    }

    mtsDescriptionConnection description;
    connection->GetDescriptionConnection(description);

    return localManagerClient->ConnectClientSideInterface(description, description.Client.ProcessName);
}

bool mtsManagerGlobal::ConnectServerSideInterfaceRequest(const ConnectionIDType connectionID)
{
    mtsConnection * connection = GetConnectionInformation(connectionID);
    if (!connection) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectServerSideInterfaceRequest: invalid connection id: " << connectionID << std::endl;
        return false;
    }

    // Get local component manager that manages the server component.
    mtsManagerLocalInterface * localManagerServer = GetProcessObject(connection->GetServerProcessName());
    if (!localManagerServer) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectServerSideInterfaceRequest: no server process "
                                 << "\"" << connection->GetServerProcessName() << "\" found" << std::endl;
        return false;
    }

    mtsDescriptionConnection description;
    connection->GetDescriptionConnection(description);

    return localManagerServer->ConnectServerSideInterface(description, description.Server.ProcessName);
}
#endif

void mtsManagerGlobal::GetListOfConnections(std::vector<mtsDescriptionConnection> & list) const
{
    mtsDescriptionConnection connection;

    ConnectionMapType::const_iterator it = ConnectionMap.begin();
    const ConnectionMapType::const_iterator itEnd = ConnectionMap.end();

    for (; it != itEnd; ++it) {
        // Check if this connection has been successfully established
        if (it->second.IsConnected())
            list.push_back(it->second.GetDescriptionConnection());
    }
}

void mtsManagerGlobal::ShowInternalStructure(void)
{
    return;

    std::stringstream ss;
    
    ss << "======= Process Map ==================" << std::endl;

    ProcessMapType::const_iterator itProcess = ProcessMap.begin();
    for (; itProcess != ProcessMap.end(); ++itProcess) {
        ss << "|P| " << itProcess->first << std::endl;
        ComponentMapType * componentMap = itProcess->second;
        if (componentMap) {
            ComponentMapType::const_iterator itComponent = componentMap->begin();
            for (; itComponent != componentMap->end(); ++itComponent) {
                ss << "\t|C| " << itComponent->first << std::endl;
                InterfaceMapType * intfcMap = itComponent->second;
                if (intfcMap) {
                    InterfaceMapElementType::const_iterator itPrvInt = intfcMap->InterfaceProvidedOrOutputMap.begin();
                    for (; itPrvInt != intfcMap->InterfaceProvidedOrOutputMap.end(); ++itPrvInt) {
                        ss << "\t\t|IP| " << itPrvInt->first << ": ";
                        ConnectionIDListType * list = itPrvInt->second;
                        if (list) {
                            ConnectionIDListType::const_iterator itList = itPrvInt->second->begin();
                            for (; itList != itPrvInt->second->end(); ++itList) {
                                ss << " " << *itList;
                            }
                        }
                        ss << std::endl;
                    }
                    InterfaceMapElementType::const_iterator itReqInt = intfcMap->InterfaceRequiredOrInputMap.begin();
                    for (; itReqInt != intfcMap->InterfaceRequiredOrInputMap.end(); ++itReqInt) {
                        ss << "\t\t|IR| " << itReqInt->first << ": ";
                        ConnectionIDListType * list = itReqInt->second;
                        if (list) {
                            ConnectionIDListType::const_iterator itList = itReqInt->second->begin();
                            for (; itList != itReqInt->second->end(); ++itList) {
                                ss << " " << *itList;
                            }
                        }
                        ss << std::endl;
                    }
                }
            }
        }
    }
    CMN_LOG_CLASS_RUN_VERBOSE << ss.str() << std::endl;

    ss.str("");
    ss << "======= Connection Map ===============" << std::endl;
    {
        ConnectionMapType::const_iterator it = ConnectionMap.begin();
        const ConnectionMapType::const_iterator itEnd = ConnectionMap.end();
        for (; it != itEnd; ++it) {
            ss << it->second << std::endl;
        }
    }
    CMN_LOG_CLASS_RUN_VERBOSE << ss.str() << std::endl;

    ss.str("");
    ss << "======= Disconnection Map ============" << std::endl;
    {
        ss << "DISCONNECT WAITING QUEUE: ";
        DisconnectQueueType::const_iterator it = QueueDisconnectWaiting.begin();
        DisconnectQueueType::const_iterator itEnd = QueueDisconnectWaiting.end();
        for (; it != itEnd; ++it) {
            ss << it->first << " ";
        }
        ss << std::endl;

        ss << "DISCONNECTED QUEUE: ";
        it = QueueDisconnected.begin();
        itEnd = QueueDisconnected.end();
        for (; it != itEnd; ++it) {
            ss << it->first << " ";
        }
    }
    CMN_LOG_CLASS_RUN_VERBOSE << ss.str() << std::endl;
}
