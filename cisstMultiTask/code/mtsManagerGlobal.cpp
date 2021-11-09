/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Min Yang Jung
  Created on: 2009-11-12

  (C) Copyright 2009-2019 Johns Hopkins University (JHU), All Rights Reserved.

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
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsConnection.h>

#include <iostream>
#include <iterator>

mtsManagerGlobal::mtsManagerGlobal() :
    ProcessMap("ProcessMap"),
    LocalManager(0),
    LocalManagerConnected(0), ConnectionID(0),
    ProxyServer(0),
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
    // MJ TEMP
#if 0
    // Disconnect all connections
    ConnectionMapType::const_iterator it1 = ConnectionMap.begin();
    const ConnectionMapType::const_iterator itEnd1 = ConnectionMap.end();
    for (; it1 != itEnd1; ++it1) {
        Disconnect(it1->second.GetConnectionID());
    }

    osaSleep(1 * cmn_s); // MJ TEMP: better handling without sleep?

    // Remove all processes
    ProcessMapType::iterator it2 = ProcessMap.begin();
    while (it2 != ProcessMap.end()) {
        RemoveProcess(it2->first);
        it2 = ProcessMap.begin();
    }
#endif

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
    const std::string & serverProcessName, const std::string & serverComponentName,
    const std::string & interfaceName) const
{
    ComponentMapType * componentMap = ProcessMap.GetItem(serverProcessName);
    if (componentMap == 0) return 0;

    InterfaceMapType * interfaceMap = componentMap->GetItem(serverComponentName);
    if (interfaceMap == 0) return 0;

    return interfaceMap->InterfaceProvidedOrOutputMap.GetItem(interfaceName);
}

mtsManagerGlobal::ConnectionIDListType * mtsManagerGlobal::GetConnectionsOfInterfaceProvidedOrOutput(
    const std::string & serverProcessName, const std::string & serverComponentName,
    const std::string & interfaceName, InterfaceMapType ** interfaceMap) const
{
    ComponentMapType * componentMap = ProcessMap.GetItem(serverProcessName);
    if (componentMap == 0) return 0;

    *interfaceMap = componentMap->GetItem(serverComponentName);
    if (*interfaceMap == 0) return 0;

    return (*interfaceMap)->InterfaceProvidedOrOutputMap.GetItem(interfaceName);
}

mtsManagerGlobal::ConnectionIDListType * mtsManagerGlobal::GetConnectionsOfInterfaceRequiredOrInput(
    const std::string & clientProcessName, const std::string & clientComponentName,
    const std::string & interfaceName) const
{
    ComponentMapType * componentMap = ProcessMap.GetItem(clientProcessName);
    if (componentMap == 0) return 0;

    InterfaceMapType * interfaceMap = componentMap->GetItem(clientComponentName);
    if (interfaceMap == 0) return 0;

    return interfaceMap->InterfaceRequiredOrInputMap.GetItem(interfaceName);
}

mtsManagerGlobal::ConnectionIDListType * mtsManagerGlobal::GetConnectionsOfInterfaceRequiredOrInput(
    const std::string & clientProcessName, const std::string & clientComponentName,
    const std::string & interfaceName, InterfaceMapType ** interfaceMap) const
{
    ComponentMapType * componentMap = ProcessMap.GetItem(clientProcessName);
    if (componentMap == 0) return 0;

    *interfaceMap = componentMap->GetItem(clientComponentName);
    if (*interfaceMap == 0) return 0;

    return (*interfaceMap)->InterfaceRequiredOrInputMap.GetItem(interfaceName);
}

bool mtsManagerGlobal::AddConnectionToInterfaceProvidedOrOutput(
    const std::string & serverProcessName, const std::string & serverComponentName,
    const std::string & interfaceName, const ConnectionIDType connectionID)
{
    InterfaceMapType * interfaceMap = 0;

    ProcessMapChange.Lock();

    mtsManagerGlobal::ConnectionIDListType * list = GetConnectionsOfInterfaceProvidedOrOutput(
        serverProcessName, serverComponentName, interfaceName, &interfaceMap);
    CMN_ASSERT(interfaceMap);

    if (!list) {
        // If this connection is the first one
        list = new ConnectionIDListType;
        (interfaceMap->InterfaceProvidedOrOutputMap.GetMap())[interfaceName] = list;
    } else {
        // Check duplicate connection id
        mtsManagerGlobal::ConnectionIDListType::const_iterator it = list->begin();
        const mtsManagerGlobal::ConnectionIDListType::const_iterator itEnd = list->end();
        for (; it != itEnd; ++it) {
            if (*it == connectionID) {
                CMN_LOG_CLASS_INIT_ERROR << "AddConnectedInterface: duplicate connection [ " << connectionID << " ] "
                    << "for \""
                    << GetInterfaceUID(serverProcessName, serverComponentName, interfaceName)
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
        << GetInterfaceUID(serverProcessName, serverComponentName, interfaceName)
        << "\" : " << ss.str() << std::endl;

    return true;
}

bool mtsManagerGlobal::AddConnectionToInterfaceRequiredOrInput(
    const std::string & clientProcessName, const std::string & clientComponentName,
    const std::string & interfaceName, const ConnectionIDType connectionID)
{
    InterfaceMapType * interfaceMap = 0;

    ProcessMapChange.Lock();

    mtsManagerGlobal::ConnectionIDListType * list = GetConnectionsOfInterfaceRequiredOrInput(
        clientProcessName, clientComponentName, interfaceName, &interfaceMap);
    CMN_ASSERT(interfaceMap);

    if (!list) {
        // If this connection is the first one
        list = new ConnectionIDListType;
        (interfaceMap->InterfaceRequiredOrInputMap.GetMap())[interfaceName] = list;
    } else {
        // Check duplicate connection id
        mtsManagerGlobal::ConnectionIDListType::const_iterator it = list->begin();
        const mtsManagerGlobal::ConnectionIDListType::const_iterator itEnd = list->end();
        for (; it != itEnd; ++it) {
            if (*it == connectionID) {
                CMN_LOG_CLASS_INIT_ERROR << "AddConnectedInterface: duplicate connection [ " << connectionID << " ] "
                    << "for \""
                    << GetInterfaceUID(clientProcessName, clientComponentName, interfaceName)
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
        << GetInterfaceUID(clientProcessName, clientComponentName, interfaceName)
        << "\" : " << ss.str() << std::endl;

    return true;
}

bool mtsManagerGlobal::RemoveConnectionOfInterfaceProvidedOrOutput(
    const std::string & serverProcessName, const std::string & serverComponentName,
    const std::string & interfaceName, const ConnectionIDType connectionID)
{
    if (!FindInterfaceProvidedOrOutput(serverProcessName, serverComponentName, interfaceName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveConnectionOfInterfaceProvidedOrOutput: no interface found: [ " << connectionID << " ] "
            << "\"" << GetInterfaceUID(serverProcessName, serverComponentName, interfaceName) << "\"" << std::endl;
        return false;
    }

    InterfaceMapType * interfaceMap = 0;

    ProcessMapChange.Lock();

    mtsManagerGlobal::ConnectionIDListType * list = GetConnectionsOfInterfaceProvidedOrOutput(
        serverProcessName, serverComponentName, interfaceName, &interfaceMap);

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
                    << GetInterfaceUID(serverProcessName, serverComponentName, interfaceName)
                    << "\"" << std::endl;
                ProcessMapChange.Unlock();
                return true;
            }
        }

        CMN_LOG_CLASS_RUN_VERBOSE << "RemoveConnectionOfInterfaceProvidedOrOutput: failed to remove connection id [ "
            << connectionID << " ] " << "from \""
            << GetInterfaceUID(serverProcessName, serverComponentName, interfaceName)
            << "\"" << std::endl;
        ProcessMapChange.Unlock();
        return false;
    }
}

bool mtsManagerGlobal::RemoveConnectionOfInterfaceRequiredOrInput(
    const std::string & clientProcessName, const std::string & clientComponentName,
    const std::string & interfaceName, const ConnectionIDType connectionID)
{
    if (!FindInterfaceRequiredOrInput(clientProcessName, clientComponentName, interfaceName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveConnectionOfInterfaceRequiredOrInput: no interface found: [ " << connectionID << " ] "
            << "\"" << GetInterfaceUID(clientProcessName, clientComponentName, interfaceName) << "\"" << std::endl;
        return false;
    }

    InterfaceMapType * interfaceMap = 0;

    ProcessMapChange.Lock();

    mtsManagerGlobal::ConnectionIDListType * list = GetConnectionsOfInterfaceRequiredOrInput(
        clientProcessName, clientComponentName, interfaceName, &interfaceMap);

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
                    << GetInterfaceUID(clientProcessName, clientComponentName, interfaceName)
                    << "\"" << std::endl;
                ProcessMapChange.Unlock();
                return true;
            }
        }

        CMN_LOG_CLASS_RUN_VERBOSE << "RemoveConnectionOfInterfaceRequiredOrInput: failed to remove connection id [ "
            << connectionID << " ] " << "from \""
            << GetInterfaceUID(clientProcessName, clientComponentName, interfaceName)
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
        const std::string & clientComponentName, const std::string & interfaceName) const
{
    ConnectionMapType::const_iterator it = ConnectionMap.begin();
    const ConnectionMapType::const_iterator itEnd = ConnectionMap.end();

    mtsDescriptionConnection description;
    for (; it != itEnd; ++it) {
        it->second.GetDescriptionConnection(description);
        if ((description.Client.ProcessName == clientProcessName) &&
            (description.Client.ComponentName == clientComponentName) &&
            (description.Client.InterfaceName == interfaceName))
        {
            return description.ConnectionID;
        }
    }

    return InvalidConnectionID;
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
        if (connectionID == InvalidConnectionID) {
            return false;
        }

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

    size_t numberOfInterfaceProvided = 0, numberOfInterfaceRequired = 0;
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

bool mtsManagerGlobal::AddProcessObject(mtsManagerLocalInterface * localManagerObject, const bool CMN_UNUSED(isManagerProxyServer))
{
    // Name of new local component manager
    std::string processName;

    // localManagerObject can be either of type mtsManagerLocal (with
    // isManagerProxyServer as false) or of type mtsManagerProxyServer
    // (with isManagerProxyServer as true).
    processName = localManagerObject->GetProcessName();

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
    LocalManagerConnected = localManagerObject;

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

    // If network layer generates an event of process disconnect, all component proxies and
    // internal interface proxies of the process should be removed.
    if (componentMap) {
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
        return 0;
    }
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
ConnectionIDType mtsManagerGlobal::Connect(const std::string & requestProcessName,
                                           const std::string & clientProcessName, const std::string & clientComponentName,
                                           const std::string & clientInterfaceName,
                                           const std::string & serverProcessName, const std::string & serverComponentName,
                                           const std::string & serverInterfaceName)
{
    std::vector<std::string> options;
    std::stringstream allOptions;
    std::ostream_iterator< std::string > output(allOptions, ", ");
    bool interfacesSwapped = false;

    // MJ: commented out the following check to allow the 3rd process (not client
    // process nor server process) to be able to initiate a connection.
    //if (requestProcessName != clientProcessName && requestProcessName != serverProcessName) {
    //    CMN_LOG_CLASS_INIT_ERROR << "Connect: invalid process is requesting connection: " << requestProcessName << std::endl;
    //    return -1;
    //}

    // Check if the required interface specified exists
    if (!FindInterfaceRequiredOrInput(clientProcessName, clientComponentName, clientInterfaceName)) {
        // Check if by any chance the parameters have been swapped
        if (FindInterfaceRequiredOrInput(clientProcessName, serverComponentName, serverInterfaceName)) {
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
                                     << GetInterfaceUID(clientProcessName, clientComponentName, clientInterfaceName)
                                     << "\", " << allOptions.str() << std::endl;
            return InvalidConnectionID;
        }
    }

    // Check if the provided interface specified exists
    if (!FindInterfaceProvidedOrOutput(serverProcessName, serverComponentName, serverInterfaceName)) {
        // Check if the interfaces have really been swapped
        if (interfacesSwapped && FindInterfaceProvidedOrOutput(serverProcessName, clientComponentName, clientInterfaceName)) {
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
                                     << GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceName)
                                     << "\", " << allOptions.str() << std::endl;
            return InvalidConnectionID;
        }
    }

    // Handle swapped components and interfaces names
    std::string clientComponentNameActual, clientInterfaceNameActual, serverComponentNameActual, serverInterfaceNameActual;
    if (!interfacesSwapped) {
        clientComponentNameActual = clientComponentName;
        clientInterfaceNameActual = clientInterfaceName;
        serverComponentNameActual = serverComponentName;
        serverInterfaceNameActual = serverInterfaceName;
    } else {
        clientComponentNameActual = serverComponentName;
        clientInterfaceNameActual = serverInterfaceName;
        serverComponentNameActual = clientComponentName;
        serverInterfaceNameActual = clientInterfaceName;
    }

    // Check if the two interfaces are already connected to each other
    bool isAlreadyConnected = IsAlreadyConnected(mtsDescriptionConnection(
        clientProcessName, clientComponentNameActual, clientInterfaceNameActual,
        serverProcessName, serverComponentNameActual, serverInterfaceNameActual));
    if (isAlreadyConnected) {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to connect - already connected interfaces: \""
                                 << GetInterfaceUID(clientProcessName, clientComponentNameActual, clientInterfaceNameActual)
                                 << "\" - \""
                                 << GetInterfaceUID(serverProcessName, serverComponentNameActual, serverInterfaceNameActual)
                                 << "\"" << std::endl;
        return InvalidConnectionID;
    }

    ConnectionChange.Lock();

    // Assign new connection id
    ConnectionIDType thisConnectionID = ConnectionID;

    // STEP 3. Update ProcessMap
    //
    // Add this connection to required interface's connection list
    if (!AddConnectionToInterfaceRequiredOrInput(
            clientProcessName, clientComponentNameActual, clientInterfaceNameActual, thisConnectionID))
    {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to add connection information to required interface: \""
            << GetInterfaceUID(clientProcessName, clientComponentNameActual, clientInterfaceNameActual)
            << "\"" << std::endl;
        ConnectionChange.Unlock();
        return InvalidConnectionID;
    }

    // Add this connection to provided interface's connection list
    if (!AddConnectionToInterfaceProvidedOrOutput(
            serverProcessName, serverComponentNameActual, serverInterfaceNameActual, thisConnectionID))
    {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to add connection information to provided interface: \""
            << GetInterfaceUID(serverProcessName, serverComponentNameActual, serverInterfaceNameActual)
            << "\"" << std::endl;
        ConnectionChange.Unlock();
        return InvalidConnectionID;
    }

    // STEP 4. Update ConnectionMap
    //
    mtsDescriptionConnection description(
        clientProcessName, clientComponentNameActual, clientInterfaceNameActual,
        serverProcessName, serverComponentNameActual, serverInterfaceNameActual,
        thisConnectionID);
    mtsConnection connection(description, requestProcessName);

    ConnectionMap.insert(std::make_pair(thisConnectionID, connection));

    // STEP 5. Post-processings
    //
    // Increase connection id
    if (ConnectionID + 1 == InvalidConnectionID) {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: connection id approached its upper limit: " << InvalidConnectionID << std::endl;
        ConnectionChange.Unlock();
        return InvalidConnectionID;
    } else {
        ++ConnectionID;
    }

    ConnectionChange.Unlock();

    // Send connection event to ManagerComponentServer
    if (ManagerComponentServer) {
        ManagerComponentServer->AddConnectionEvent(description);
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "Connect: successfully connected, new connection id [ " << thisConnectionID << " ] "
        << "for \""
        << GetInterfaceUID(clientProcessName, clientComponentNameActual, clientInterfaceNameActual)
        << "\" - \""
        << GetInterfaceUID(serverProcessName, serverComponentNameActual, serverInterfaceNameActual)
        << "\"" << std::endl;

    return thisConnectionID;
}

void mtsManagerGlobal::CheckConnectConfirmTimeout(void)
{
    CMN_LOG_CLASS_INIT_WARNING << "CheckConnectConfirmTimeout called when CISST_MTS_HAS_ICE is false" << std::endl;
}

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

    // MJ: for testing and debugging
    //ShowInternalStructure();

    return true;
}

void mtsManagerGlobal::AddToDisconnectedProcessCleanup(const std::string & CMN_UNUSED(sourceProcessName),
    const std::string & CMN_UNUSED(targetProcessName), const std::string & CMN_UNUSED(targetComponentProxyName))
{
    CMN_LOG_CLASS_INIT_WARNING << "AddToDisconnectedProcessCleanup called when CISST_MTS_HAS_ICE is false" << std::endl;
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
    bool localConfiguration;

    std::string processName;
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
        serverInterfaceName = connectionInfo->GetServerInterfaceName();
        clientProcessName = connectionInfo->GetClientProcessName();
        clientComponentName = connectionInfo->GetClientComponentName();
        clientInterfaceName = connectionInfo->GetClientInterfaceName();

        localConfiguration = ((serverProcessName == clientProcessName) &&
                               serverProcessName == mtsManagerLocal::ProcessNameOfLCMDefault);

        if (!localConfiguration) {
            CMN_LOG_CLASS_RUN_ERROR << "Disconnect: invalid process names requested for disconnection: "
                                    << "\"" << serverProcessName << "\", \"" << clientProcessName << "\"" << std::endl;
            continue;
        }

        // Step 1. Let LCM disconnect local connection
        processName = serverProcessName;

        ManagerComponentServer->ComponentDisconnect(processName, connection);
        osaSleep(100 * cmn_ms); // Give time to the GCM to actually process disconnection request

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
            mtsManagerComponentBase::GetNameOfManagerComponentClientFor(mtsManagerLocal::ProcessNameOfLCMDefault);
        if ((serverComponentName != nameOfLCM_MCC) && (clientComponentName == nameOfLCM_MCC) &&
            mtsManagerComponentBase::IsNameOfInterfaceInternalProvided(serverInterfaceName))
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
#if CMN_ASSERT_IS_DEFINED
        DisconnectQueueType::const_iterator itDisconnected =
#endif
            QueueDisconnected.find(connectionID);
        CMN_ASSERT(itDisconnected == QueueDisconnected.end());
        QueueDisconnected.insert(std::make_pair(connectionID, connectionID));

        // Step 6. Send disconnection event to ManagerComponentServer
        if (ManagerComponentServer)
            ManagerComponentServer->RemoveConnectionEvent(connection);

        CMN_LOG_CLASS_INIT_VERBOSE << "Disconnect: successfully disconnected [ " << connectionID << " ] : "
                                << "\"" << serverComponentName << ":" << serverInterfaceName << " - "
                                << clientComponentName << ":" << clientInterfaceName << "\"" << std::endl;
    }

    // MJ: for testing and debugging
    //ShowInternalStructure();
}

bool mtsManagerGlobal::Disconnect(const std::string & clientProcessName, const std::string & clientComponentName,
                                  const std::string & clientInterfaceName,
                                  const std::string & serverProcessName, const std::string & serverComponentName,
                                  const std::string & serverInterfaceName)
{
    ConnectionIDType id = GetConnectionID(clientProcessName, clientComponentName, clientInterfaceName);
    if (id == InvalidConnectionID) {
        CMN_LOG_CLASS_INIT_ERROR << "Disconnect: no connection id found for "
            << "\"" << GetInterfaceUID(clientProcessName, clientComponentName, clientInterfaceName) << "\" - "
            << "\"" << GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceName) << std::endl;
        return false;
    }

    return Disconnect(id);
}

bool mtsManagerGlobal::Disconnect(const mtsDescriptionConnection &connection)
{
    if (connection.ConnectionID == InvalidConnectionID) {
        return Disconnect(connection.Client.ProcessName, connection.Client.ComponentName, connection.Client.InterfaceName,
                          connection.Server.ProcessName, connection.Server.ComponentName, connection.Server.InterfaceName);
    } else {
        return Disconnect(connection.ConnectionID);
    }
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
        CMN_LOG_CLASS_INIT_VERBOSE << "Disconnect: invalid connection id: " << connectionID << std::endl;
        return false;
    }

    QueueDisconnectWaitingChange.Lock();
    QueueDisconnectWaiting.insert(std::make_pair(connectionID, connectionID));
    QueueDisconnectWaitingChange.Unlock();

    CMN_LOG_CLASS_INIT_VERBOSE << "Disconnect: queued connection id [ " << connectionID
        << " ] to disconnect waiting queue" << std::endl;

    return true;
}

//-------------------------------------------------------------------------
//  Getters
//-------------------------------------------------------------------------
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
                                                            std::vector<std::string>& namesOfInterfaces) const
{
    ComponentMapType * components = ProcessMap.GetItem(processName);
    if (!components) return;

    InterfaceMapType * interfaces = components->GetItem(componentName);
    if (!interfaces) return;

    interfaces->InterfaceProvidedOrOutputMap.GetNames(namesOfInterfaces);
}

void mtsManagerGlobal::GetNamesOfInterfacesRequiredOrInput(const std::string & processName,
                                                           const std::string & componentName,
                                                           std::vector<std::string>& namesOfInterfaces) const
{
    ComponentMapType * components = ProcessMap.GetItem(processName);
    if (!components) return;

    InterfaceMapType * interfaces = components->GetItem(componentName);
    if (!interfaces) return;

    interfaces->InterfaceRequiredOrInputMap.GetNames(namesOfInterfaces);

}

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


//-------------------------------------------------------------------------
//  Networking
//-------------------------------------------------------------------------
bool mtsManagerGlobal::StartServer(void)
{
    CMN_LOG_CLASS_INIT_WARNING << "StartServer called when CISST_MTS_HAS_ICE is false" << std::endl;
    return true;
}

bool mtsManagerGlobal::StopServer(void)
{
    CMN_LOG_CLASS_INIT_WARNING << "StopServer called when CISST_MTS_HAS_ICE is false" << std::endl;
    return true;
}

bool mtsManagerGlobal::SetInterfaceProvidedProxyAccessInfo(const ConnectionIDType CMN_UNUSED(connectionID), const std::string & CMN_UNUSED(endpointInfo))
{
    CMN_LOG_CLASS_INIT_WARNING << "SetInterfaceProvidedProxyAccessInfo called when CISST_MTS_HAS_ICE is false" << std::endl;
    return true;
}

bool mtsManagerGlobal::GetInterfaceProvidedProxyAccessInfo(const ConnectionIDType CMN_UNUSED(connectionID), std::string & CMN_UNUSED(endpointInfo))
{
    CMN_LOG_CLASS_INIT_WARNING << "GetInterfaceProvidedProxyAccessInfo called when CISST_MTS_HAS_ICE is false" << std::endl;
    return true;
}

bool mtsManagerGlobal::GetInterfaceProvidedProxyAccessInfo(const std::string & CMN_UNUSED(clientProcessName),
    const std::string & CMN_UNUSED(serverProcessName), const std::string & CMN_UNUSED(serverComponentName),
    const std::string & CMN_UNUSED(serverInterfaceName), std::string & CMN_UNUSED(endpointInfo))
{
    CMN_LOG_CLASS_INIT_WARNING << "GetInterfaceProvidedProxyAccessInfo called when CISST_MTS_HAS_ICE is false" << std::endl;
    return false;
}

bool mtsManagerGlobal::InitiateConnect(const ConnectionIDType CMN_UNUSED(connectionID))
{
    CMN_LOG_CLASS_INIT_WARNING << "InitiateConnect called when CISST_MTS_HAS_ICE is false" << std::endl;
    return false;
}

bool mtsManagerGlobal::ConnectServerSideInterfaceRequest(const ConnectionIDType CMN_UNUSED(connectionID))
{
    CMN_LOG_CLASS_INIT_WARNING << "ConnectServerSideInterfaceRequest called when CISST_MTS_HAS_ICE is false" << std::endl;
    return false;
}

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
    std::stringstream ss;

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
