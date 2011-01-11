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
    ManagerComponentServer(0), ThreadDisconnectRunning(true)
#if CISST_MTS_HAS_ICE
    , ProxyServer(0)
#endif
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
    // Remove all processes
    ProcessMapType::iterator it = ProcessMap.begin();
    while (it != ProcessMap.end()) {
        RemoveProcess(it->first);
        it = ProcessMap.begin();
    }

    // Remove all established connections
    /* TODO: check this later
    ConnectionsMapType::const_iterator it = ConnectionsMap.begin();
    const ConnectionsMapType::const_iterator itEnd = ConnectionsMap.end();
    for (; it != itEnd; ++it) {
        delete it->second;
    }
    ConnectionsMap.clear();
    */

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
        mtsManagerGlobal::ConnectionIDListType::const_iterator it = list->begin();
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
        mtsManagerGlobal::ConnectionIDListType::const_iterator it = list->begin();
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

int mtsManagerGlobal::GetNumberOfInterfaces(const std::string & processName, const std::string & componentName) const
{
    if (!FindComponent(processName, componentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "GetNumberOfInterfaces: no component found: "
            << "\"" << processName << ":" << componentName << "\"" << std::endl;
        return -1;
    }

    const unsigned int numberOfInterfaceProvided = 
        ProcessMap.GetItem(processName)->GetItem(componentName)->InterfaceProvidedOrOutputMap.size();
    const unsigned int numberOfInterfaceRequired = 
        ProcessMap.GetItem(processName)->GetItem(componentName)->InterfaceRequiredOrInputMap.size();

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

bool mtsManagerGlobal::AddProcessObject(mtsManagerLocalInterface * localManagerObject, const bool isManagerProxyServer)
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

bool mtsManagerGlobal::RemoveProcess(const std::string & processName)
{
    if (!FindProcess(processName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveProcess: no process found: " << "\"" << processName << "\"" << std::endl;
        return false;
    }

    bool ret = true;

    ProcessMapChange.Lock();

    ComponentMapType * componentMap = ProcessMap.GetItem(processName);

    // If the process being killed has components, they should be removed first.
    if (componentMap) {
        ComponentMapType::iterator it = componentMap->begin();
        while (it != componentMap->end()) {
            ret &= RemoveComponent(processName, it->first, false);
            it = componentMap->begin();
        }

        delete componentMap;
    }

    // Remove the process from process map
    ret &= ProcessMap.RemoveItem(processName);

    ProcessMapChange.Unlock();

    CMN_LOG_CLASS_RUN_VERBOSE << "RemoveProcess: removed process: " << "\"" << processName << "\"" << std::endl;

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
        CMN_LOG_CLASS_RUN_ERROR << "FindComponent: no process found: " << "\"" << processName << "\"" << std::endl;
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
    CMN_LOG_CLASS_INIT_VERBOSE << "zzzzzzzzzzzz 3333333333333333333" << componentName << std::endl;

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
    CMN_ASSERT(componentMap);

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

    // Remove the component from the component map
    ret &= componentMap->RemoveItem(componentName);

    if (lock) {
        ProcessMapChange.Unlock();
    }

    CMN_LOG_CLASS_RUN_VERBOSE << "RemoveComponent: removed component: " << "\"" << processName << ":" << componentName << "\"" << std::endl;

    return ret;
}

//-------------------------------------------------------------------------
//  Interface Management
//-------------------------------------------------------------------------
bool mtsManagerGlobal::AddInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName,
                                                    const std::string & interfaceName, const bool isProxyInterface)
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

    // smmy
    //interfaceMap->InterfaceProvidedOrOutputTypeMap[interfaceName] = isProxyInterface;

    ProcessMapChange.Unlock();

    return true;
}


bool mtsManagerGlobal::AddInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName,
                                                   const std::string & interfaceName, const bool isProxyInterface)
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

    //interfaceMap->InterfaceRequiredOrInputTypeMap[interfaceName] = isProxyInterface;

    ProcessMapChange.Unlock();

    return true;
}

bool mtsManagerGlobal::FindInterfaceProvidedOrOutput(const std::string & processName,
                                                     const std::string & componentName,
                                                     const std::string & interfaceName) const
{
    if (!FindComponent(processName, componentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "FindInterfaceProvidedOrOutput: failed to find component: "
            << "\"" << processName << ":" << componentName << "\"" << std::endl;
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
        CMN_LOG_CLASS_RUN_ERROR << "FindInterfaceRequiredOrInput: failed to find component: "
            << "\"" << processName << ":" << componentName << "\"" << std::endl;
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
    CMN_ASSERT(componentMap);

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
            while (it != list->end()) {
                if (!Disconnect(*it)) {
                    CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceProvidedOrOutput: failed to disconnect (id: \"" << *it 
                        << "\"): \""
                        << mtsManagerGlobal::GetInterfaceUID(processName, componentName, interfaceName) << "\"" << std::endl;
                }
                it = list->begin();
            }
            delete list;
        }
    }

    // Remove the provided interface from provided interface map
    ret &= interfaceMap->InterfaceProvidedOrOutputMap.RemoveItem(interfaceName);

    if (lock) {
        ProcessMapChange.Unlock();
    }

    CMN_LOG_CLASS_RUN_VERBOSE << "RemoveInterfaceProvidedOrOutput: removed provided interface: \"" 
        << mtsManagerGlobal::GetInterfaceUID(processName, componentName, interfaceName) << "\"" << std::endl;

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
    CMN_ASSERT(componentMap);

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
        CMN_ASSERT(list->size() == 1);

        const ConnectionIDType id = *list->begin();
        if (!Disconnect(id)) {
            CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceRequiredOrInput: failed to disconnect (id: \"" << id << "\"): \""
                << mtsManagerGlobal::GetInterfaceUID(processName, componentName, interfaceName) << "\"" << std::endl;
            return false;
        }
        delete list;
    }

    // Remove the required interface from required interface map
    ret &= interfaceMap->InterfaceRequiredOrInputMap.RemoveItem(interfaceName);

    if (lock) {
        ProcessMapChange.Unlock();
    }

    CMN_LOG_CLASS_RUN_VERBOSE << "RemoveInterfaceRequiredOrInput: removed required interface: \"" 
        << mtsManagerGlobal::GetInterfaceUID(processName, componentName, interfaceName) << "\"" << std::endl;

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

    ConnectionChange.Lock();

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
                        ConnectionChange.Unlock();
                        return -1;
                    }
                } else {
                    if (!LocalManagerConnected->CreateComponentProxy(clientComponentProxyName, serverProcessName)) {
                        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to create client component proxy "
                            << "\"" << clientComponentProxyName << "\" in server process "
                            << "\"" << serverProcessName << "\"" << std::endl;
                        ConnectionChange.Unlock();
                        return -1;
                    }
                }
            } else {
                if (!LocalManagerConnected->CreateComponentProxy(clientComponentProxyName, serverProcessName)) {
                    CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to create client component proxy "
                        << "\"" << clientComponentProxyName << "\" in server process "
                        << "\"" << serverProcessName << "\"" << std::endl;
                    ConnectionChange.Unlock();
                    return -1;
                }

            }
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
                        ConnectionChange.Unlock();
                        return -1;
                    }
                } else {
                    if (!LocalManagerConnected->CreateComponentProxy(serverComponentProxyName, clientProcessName)) {
                        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to create server component proxy "
                                                 << "\"" << serverComponentProxyName << "\" in client process "
                                                 << "\"" << clientProcessName << "\"" << std::endl;
                        ConnectionChange.Unlock();
                        return -1;
                    }
                }
            } else {
                if (!LocalManagerConnected->CreateComponentProxy(serverComponentProxyName, clientProcessName)) {
                    CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to create server component proxy "
                                             << "\"" << serverComponentProxyName << "\" in client process "
                                             << "\"" << clientProcessName << "\"" << std::endl;
                    ConnectionChange.Unlock();
                    return -1;
                }

            }
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
                        ConnectionChange.Unlock();
                        return -1;
                    }
                } else {
                    if (!LocalManagerConnected->GetInterfaceRequiredDescription(
                            clientComponentNameActual, clientInterfaceRequiredNameActual, requiredInterfaceDescription, clientProcessName))
                    {
                        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to get required interface description: "
                                                 << GetInterfaceUID(clientProcessName, clientComponentNameActual, clientInterfaceRequiredNameActual) << std::endl;
                        ConnectionChange.Unlock();
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
                        ConnectionChange.Unlock();
                        return -1;
                    }
                } else {
                    if (!LocalManagerConnected->CreateInterfaceRequiredProxy(
                            clientComponentProxyName, requiredInterfaceDescription, serverProcessName))
                    {
                        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to create required interface proxy in server process: "
                                                 << "\"" << clientComponentProxyName << "\" in \"" << serverProcessName << "\"" << std::endl;
                        ConnectionChange.Unlock();
                        return -1;
                    }
                }
            } else {
                if (!LocalManagerConnected->GetInterfaceRequiredDescription(
                        clientComponentNameActual, clientInterfaceRequiredNameActual, requiredInterfaceDescription, clientProcessName))
                {
                    CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to get required interface description: "
                                             << GetInterfaceUID(clientProcessName, clientComponentNameActual, clientInterfaceRequiredNameActual) << std::endl;
                    ConnectionChange.Unlock();
                    return -1;
                }

                // Let the server process create required interface proxy
                if (!LocalManagerConnected->CreateInterfaceRequiredProxy(
                        clientComponentProxyName, requiredInterfaceDescription, serverProcessName))
                {
                    CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to create required interface proxy in server process: "
                                             << "\"" << clientComponentProxyName << "\" in \"" << serverProcessName << "\"" << std::endl;
                    ConnectionChange.Unlock();
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
                        ConnectionChange.Unlock();
                        return -1;
                    }
                } else {
                    if (!LocalManagerConnected->GetInterfaceProvidedDescription(
                        serverComponentName, serverInterfaceProvidedNameActual, providedInterfaceDescription, serverProcessName))
                    {
                        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to get provided interface description: "
                            << GetInterfaceUID(serverProcessName, serverComponentNameActual, serverInterfaceProvidedNameActual) << std::endl;
                        ConnectionChange.Unlock();
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
                        ConnectionChange.Unlock();
                        return -1;
                    }
                } else {
                    if (!LocalManagerConnected->CreateInterfaceProvidedProxy(
                            serverComponentProxyName, providedInterfaceDescription, clientProcessName))
                    {
                        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to create provided interface proxy in client process: "
                                                 << "\"" << serverComponentProxyName << "\" in \"" << clientProcessName << "\"" << std::endl;
                        ConnectionChange.Unlock();
                        return -1;
                    }
                }
            } else {
                if (!LocalManagerConnected->GetInterfaceProvidedDescription(
                        serverComponentName, serverInterfaceProvidedNameActual, providedInterfaceDescription, serverProcessName))
                {
                    CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to get provided interface description: "
                                             << GetInterfaceUID(serverProcessName, serverComponentNameActual, serverInterfaceProvidedNameActual) << std::endl;
                    ConnectionChange.Unlock();
                    return -1;
                }

                // Let the client process create provided interface proxy
                if (!LocalManagerConnected->CreateInterfaceProvidedProxy(
                        serverComponentProxyName, providedInterfaceDescription, clientProcessName))
                {
                    CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to create provided interface proxy in client process: "
                                             << "\"" << serverComponentProxyName << "\" in \"" << clientProcessName << "\"" << std::endl;
                    ConnectionChange.Unlock();
                    return -1;
                }
            }
        }
    }
#endif

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

    return true;
}

//  MJ: Design of Disconnect()
//
//  1. A thread calls Disconnect() due to peer-proxy disconnection, processing 
//     errors, etc.  Keep in mind that multiple disconnect requests with the 
//     same connection id can be made simultaneously.
//  2. Check if the connection id is found in the "disconnect waiting queue" 
//     and the "disconnected queue."  If yes, return true immediately (because 
//     the connection will be soon disconnected)
//  3. If no, push the connection id to the disconnect waiting queue which will 
//     be periodically processed.
//  4. In the separate disconnect processing thread, the disconnect waiting 
//     queue gets processed and the processed id is dequeued out of it and pushed 
//     to the disconnected queue.
//
void mtsManagerGlobal::DisconnectInternal(void)
{
    if (QueueDisconnectWaiting.empty()) return;

    QueueDisconnectWaitingChange.Lock();

    DisconnectQueueType::iterator it;
    ConnectionIDType connectionID;
    mtsConnection * connectionInfo;

    std::string serverComponentName;
    std::string serverInterfaceName;
    std::string serverProcessName;
    std::string clientProcessName;
    std::string clientComponentName;
    std::string clientInterfaceName;

#if CISST_MTS_HAS_ICE
    bool remoteConnection;
    mtsManagerLocalInterface *localManagerServer, *localManagerClient;
#else
    mtsManagerLocalInterface * localManager;
    std::string processName;
#endif

    while (!QueueDisconnectWaiting.empty()) {
        it = QueueDisconnectWaiting.begin();
        
        connectionID = it->first;
        connectionInfo = GetConnectionInformation(connectionID);
        CMN_ASSERT(connectionInfo);

        serverProcessName = connectionInfo->GetServerProcessName();
        serverComponentName = connectionInfo->GetServerComponentName();
        serverInterfaceName = connectionInfo->GetServerInterfaceProvidedName();
        clientProcessName = connectionInfo->GetClientProcessName();
        clientComponentName = connectionInfo->GetClientComponentName();
        clientInterfaceName = connectionInfo->GetClientInterfaceRequiredName();

#if CISST_MTS_HAS_ICE
        // 1. Proxy Clean-up in case of networked configuration
        //
        // 1-1. Disconnect and clean up server process
        //
        // 1-1-1. LCM::DisconnectInternal
        remoteConnection = connectionInfo->IsRemoteConnection();

        localManagerServer = GetProcessObject(serverProcessName);
        CMN_ASSERT(localManagerServer);

        if (remoteConnection) {
            //
            //////////////////////////////////////////////// smmygol: REVIEW
            // which method(s) does have internal call to GCM to disconnect/remove sth?
            //
            // 1-1-2. Remove required interface proxy
            const std::string clientComponentProxyName = GetComponentProxyName(clientProcessName, clientComponentName);
            if (!localManagerServer->RemoveInterfaceRequiredProxy(
                    clientComponentProxyName, clientInterfaceName, serverProcessName)) 
            {
                CMN_LOG_CLASS_RUN_ERROR << "Disconnect: failed to remove required interface proxy at server side: "
                    << "[ " << connectionID << " ] "
                    << mtsManagerGlobal::GetInterfaceUID(serverProcessName, clientComponentProxyName, clientInterfaceName)
                    << std::endl;
                CMN_ASSERT(false);
                // smmy: for testing
                //continue;
            }

            // 1-1-3. Clean up GCM's internal data structure
            if (!RemoveConnectionOfInterfaceRequiredOrInput(
                    serverProcessName, clientComponentProxyName, clientInterfaceName))
            {
                CMN_LOG_CLASS_RUN_ERROR << "Disconnect: failed to remove connection info at server side: "
                    << "[ " << connectionID << " ] "
                    << mtsManagerGlobal::GetInterfaceUID(serverProcessName, clientComponentProxyName, clientInterfaceName)
                    << std::endl;
                CMN_ASSERT(false);
                // smmy: for testing
                //continue;
            }

            // 1-1-4. Check if client component proxy should be removed
            //        (proxy component should be removed if it has no proxy interface)
            const int numOfInterfaces = GetNumberOfInterfaces(serverProcessName, clientComponentProxyName);
            if (numOfInterfaces == -1) {
                CMN_LOG_CLASS_RUN_ERROR << "Disconnect: failed to get total number of interfaces: "
                    << "[ " << connectionID << " ] "
                    << "\"" << serverProcessName << ":" << clientComponentProxyName << "\"" << std::endl;
                CMN_ASSERT(false);
                // smmy: for testing
                //continue;
            } else {
                if (numOfInterfaces == 0) {
                    CMN_LOG_CLASS_RUN_VERBOSE << "Disconnect: remove empty proxy component: "
                        << "[ " << connectionID << " ] "
                        << "\"" << serverProcessName << ":" << clientComponentProxyName << "\"" << std::endl;

                    if (!localManagerServer->RemoveComponentProxy(clientComponentProxyName, serverProcessName)) {
                        CMN_LOG_CLASS_RUN_ERROR << "Disconnect: failed to remove proxy component: "
                            << "[ " << connectionID << " ] "
                            << "\"" << serverProcessName << std::endl;
                        CMN_ASSERT(false);
                        // smmy: for testing
                        //continue;
                    }
                }
            }

        }

        // 1-2. Disconnect and clean up client process
        //
        // 1-2-1. LCM::DisconnectInternal
        localManagerClient = GetProcessObject(clientProcessName);
        CMN_ASSERT(localManagerServer);

        if (remoteConnection) {
            // 1-2-2. Remove provided interface proxy if necessary
            const std::string serverComponentProxyName = GetComponentProxyName(serverProcessName, serverComponentName);
            ConnectionIDListType * list = GetConnectionsOfInterfaceProvidedOrOutput(
                clientProcessName, serverComponentProxyName, serverInterfaceName);\
            CMN_ASSERT(list);
            /*
            if (!localManagerServer->RemoveInterfaceRequiredProxy(
                    clientComponentProxyName, clientInterfaceName, serverProcessName)) 
            {
                CMN_LOG_CLASS_RUN_ERROR << "Disconnect: failed to remove required interface proxy at server side: "
                    << "[ " << connectionID << " ] "
                    << mtsManagerGlobal::GetInterfaceUID(serverProcessName, clientComponentProxyName, clientInterfaceName)
                    << std::endl;
                CMN_ASSERT(false);
                // smmy: for testing
                //continue;
            }
            */

            // 1-2-3. Check if client component proxy should be removed
            //        (proxy component should be removed if it has no proxy interface)
            const int numOfInterfaces = GetNumberOfInterfaces(serverProcessName, clientComponentProxyName);
            if (numOfInterfaces == -1) {
                CMN_LOG_CLASS_RUN_ERROR << "Disconnect: failed to get total number of interfaces: "
                    << "[ " << connectionID << " ] "
                    << "\"" << serverProcessName << ":" << clientComponentProxyName << "\"" << std::endl;
                CMN_ASSERT(false);
                // smmy: for testing
                //continue;
            } else {
                if (numOfInterfaces == 0) {
                    CMN_LOG_CLASS_RUN_VERBOSE << "Disconnect: remove empty proxy component: "
                        << "[ " << connectionID << " ] "
                        << "\"" << serverProcessName << ":" << clientComponentProxyName << "\"" << std::endl;

                    if (!localManagerServer->RemoveComponentProxy(clientComponentProxyName, serverProcessName)) {
                        CMN_LOG_CLASS_RUN_ERROR << "Disconnect: failed to remove proxy component: "
                            << "[ " << connectionID << " ] "
                            << "\"" << serverProcessName << std::endl;
                        CMN_ASSERT(false);
                        // smmy: for testing
                        //continue;
                    }
                }
            }
        }
#else
        // 1. Let LCM disconnect local connection
        CMN_ASSERT(serverProcessName == clientProcessName);
        processName = serverProcessName;

        localManager = GetProcessObject(processName);

        if (!localManager) {
            CMN_LOG_CLASS_RUN_ERROR << "Disconnect: invalid LCM for connection [ " << connectionID << " ] " << std::endl;
            // MJ: for testing
            CMN_ASSERT(false);
            //continue;
        }

        if (!localManager->DisconnectLocally(serverComponentName, serverInterfaceName, 
                                             clientComponentName, clientInterfaceName))
        {
            CMN_LOG_CLASS_RUN_ERROR << "Disconnect: LCM failed to disconnect: [ " << connectionID << " ] "
                << "\"" << serverComponentName << ":" << serverInterfaceName << " - " 
                << "\"" << clientComponentName << ":" << clientInterfaceName << std::endl;
            CMN_ASSERT(false);
            // smmy: for testing
            //continue;
        }

        // 2. Clean up GCM's internal data structure
        //if (FindComponent(processName, clientComponentName)) {
            if (!RemoveConnectionOfInterfaceRequiredOrInput(processName, clientComponentName, clientInterfaceName, connectionID)) {
                CMN_LOG_CLASS_RUN_ERROR << "Disconnect: failed to remove required interface's connection info: "
                    << "[ " << connectionID << " ] - "
                    << mtsManagerGlobal::GetInterfaceUID(processName, clientComponentName, clientInterfaceName)
                    << std::endl;
                CMN_ASSERT(false);
                // smmy: for testing
                //continue;
            }
        //}

        if (FindComponent(processName, serverComponentName)) {
            if (!RemoveConnectionOfInterfaceProvidedOrOutput(processName, serverComponentName, serverInterfaceName, connectionID)) {
                CMN_LOG_CLASS_RUN_ERROR << "Disconnect: failed to remove provided interface's connection info: "
                    << "[ " << connectionID << " ] - "
                    << mtsManagerGlobal::GetInterfaceUID(processName, serverComponentName, serverInterfaceName)
                    << std::endl;
                CMN_ASSERT(false);
                // smmy: for testing
                //continue;
            }
        }
#endif

        // 3. Enqueue the disconnected id to the disconnected queue
        // first, dequeue id from disconnect waiting queue
        QueueDisconnectWaiting.erase(it);

        // enqueue id to disconnected queue
        // MJ: this is redundant check but intentionally added to make sure things work correctly
        DisconnectQueueType::const_iterator itDisconnected = QueueDisconnected.find(connectionID);
        CMN_ASSERT(itDisconnected == QueueDisconnected.end());
        QueueDisconnected.insert(std::make_pair(connectionID, connectionID));
    }

    QueueDisconnectWaitingChange.Unlock();
    
    CMN_LOG_CLASS_INIT_VERBOSE << "zzzzzzzzzzzz 222222222222222222222222" << std::endl;

    CMN_LOG_CLASS_INIT_VERBOSE << "Disconnect: successfully disconnected [ " << connectionID << " ] : "
                               << "\"" << serverComponentName << ":" << serverInterfaceName << " - " 
                               << clientComponentName << ":" << clientInterfaceName << "\"" << std::endl;

    /*
    // Get connection information
    ConnectionIDListType * connectionMapOfInterfaceRequired =
        GetConnectionsOfInterfaceRequiredOrInput(clientProcessName, clientComponentName, clientInterfaceRequiredName);
    if (!connectionMapOfInterfaceRequired) {
        CMN_LOG_CLASS_RUN_ERROR << "Disconnect: failed to disconnect. Required interface has no connection: "
                                << GetInterfaceUID(clientProcessName, clientComponentName, clientInterfaceRequiredName) << std::endl;
        return false;
    }

    ConnectionIDListType * connectionMapOfInterfaceProvided =
        GetConnectionsOfInterfaceProvidedOrOutput(serverProcessName, serverComponentName, serverInterfaceProvidedName);
    if (!connectionMapOfInterfaceProvided) {
        CMN_LOG_CLASS_RUN_ERROR << "Disconnect: failed to disconnect. Provided interface has no connection: "
                                << GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceProvidedName) << std::endl;
        return false;
    }

    bool remoteConnection = false;  // true if the connection to be disconnected is a remote connection.
    std::string interfaceUID;
    ConnectedInterfaceInfo * connectionInfo;

    // Remove required interfaces' connection information
    if (connectionMapOfInterfaceRequired->size()) {
        // Get an element that contains connection information
        interfaceUID = GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceProvidedName);
        connectionInfo = connectionMapOfInterfaceRequired->GetItem(interfaceUID);

        // Release allocated memory
        if (connectionInfo) {
            remoteConnection = connectionInfo->IsRemoteConnection();
            delete connectionInfo;
        }

        // Remove connection information
        if (connectionMapOfInterfaceRequired->FindItem(interfaceUID)) {
            if (!connectionMapOfInterfaceRequired->RemoveItem(interfaceUID)) {
                CMN_LOG_CLASS_RUN_ERROR << "Disconnect: failed to update connection map in server process" << std::endl;
                return false;
            }

            // If the required interface is a proxy object (not an original interface),
            // it should be removed when the connection is disconnected.
#if CISST_MTS_HAS_ICE
            if (remoteConnection) {
                mtsManagerLocalInterface * localManagerServer = GetProcessObject(serverProcessName);
                const std::string clientComponentProxyName = GetComponentProxyName(clientProcessName, clientComponentName);
                // Check if network proxy client is active. It is possible that
                // a local manager server is disconnected due to crashes or any
                // other reason. This check prevents redundant error messages.
                mtsManagerProxyServe r* proxyClient = dynamic_cast<mtsManagerProxyServer *>(localManagerServer);
                if (proxyClient) {
                    if (proxyClient->GetNetworkProxyClient(serverProcessName)) {
                        if (localManagerServer->RemoveInterfaceRequiredProxy(clientComponentProxyName, clientInterfaceRequiredName, serverProcessName)) {
                            // If no interface exists on the component proxy, it should be removed.
                            const int interfaceCount = localManagerServer->GetTotalNumberOfInterfaces(clientComponentProxyName, serverProcessName);
                            if (interfaceCount != -1) { // no component found
                                if (interfaceCount == 0) { // remove components with no active interface
                                    CMN_LOG_CLASS_RUN_VERBOSE <<"Disconnect: remove client component proxy with no active interface: " << clientComponentProxyName << std::endl;
                                    if (!localManagerServer->RemoveComponentProxy(clientComponentProxyName, serverProcessName)) {
                                        CMN_LOG_CLASS_RUN_ERROR << "Disconnect: failed to remove client component proxy: "
                                                                << clientComponentProxyName << " on " << serverProcessName << std::endl;
                                        return false;
                                    }
                                }
                            }
                        } else {
                            CMN_LOG_CLASS_RUN_WARNING << "Disconnect: failed to update local component manager at server side" << std::endl;
                        }
                    }
                }
            }
#endif
        }
    }

    // Remove provided interfaces' connection information
    if (connectionMapOfInterfaceProvided->size()) {
        // Get an element that contains connection information
        interfaceUID = GetInterfaceUID(clientProcessName, clientComponentName, clientInterfaceRequiredName);
        connectionInfo = connectionMapOfInterfaceProvided->GetItem(interfaceUID);

        // Release allocated memory
        if (connectionInfo) {
            remoteConnection = connectionInfo->IsRemoteConnection();
            delete connectionInfo;
        }

        // Remove connection information
        if (connectionMapOfInterfaceProvided->FindItem(interfaceUID)) {
            if (!connectionMapOfInterfaceProvided->RemoveItem(interfaceUID)) {
                CMN_LOG_CLASS_RUN_ERROR << "Disconnect: failed to update connection map in client process" << std::endl;
                return false;
            }

            // If the required interface is a proxy object (not an original interface),
            // it should be removed when the connection is disconnected.
#if CISST_MTS_HAS_ICE
            if (remoteConnection) {
                mtsManagerLocalInterface * localManagerClient = GetProcessObject(clientProcessName);
                const std::string serverComponentProxyName = GetComponentProxyName(serverProcessName, serverComponentName);
                // Check if network proxy client is active. It is possible that
                // a local manager server is disconnected due to crashes or any
                // other reason. This check prevents redundant error messages.
                mtsManagerProxyServer * proxyClient = dynamic_cast<mtsManagerProxyServer *>(localManagerClient);
                if (proxyClient) {
                    if (proxyClient->GetNetworkProxyClient(clientProcessName)) {
                        if (localManagerClient->RemoveInterfaceProvidedProxy(serverComponentProxyName, serverInterfaceProvidedName, clientProcessName)) {
                            // If no interface exists on the component proxy, it should be removed.
                            const int interfaceCount = localManagerClient->GetTotalNumberOfInterfaces(serverComponentProxyName, clientProcessName);
                            if (interfaceCount != -1) {
                                if (interfaceCount == 0) {
                                    CMN_LOG_CLASS_RUN_VERBOSE <<"Disconnect: remove server component proxy with no active interface: " << serverComponentProxyName << std::endl;
                                    if (!localManagerClient->RemoveComponentProxy(serverComponentProxyName, clientProcessName)) {
                                        CMN_LOG_CLASS_RUN_ERROR << "Disconnect: failed to remove server component proxy: "
                                                                << serverComponentProxyName << " on " << clientProcessName << std::endl;
                                        return false;
                                    }
                                }
                            }
                        } else {
                            CMN_LOG_CLASS_RUN_WARNING << "Disconnect: failed to update local component manager at client side" << std::endl;
                        }
                    }
                }
            }
#endif
        }
    }

    // Remove connection element corresponding to this connection
    bool removed = false;
    const std::string clientInterfaceUID = GetInterfaceUID(clientProcessName, clientComponentName, clientInterfaceRequiredName);
    const std::string serverInterfaceUID = GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceProvidedName);
    ConnectionElement * element;
    ConnectionMapType::iterator it = ConnectionMap.begin();
    for (; it != ConnectionMap.end(); ++it) {
        element = it->second;
        if (clientInterfaceUID == GetInterfaceUID(element->ClientProcessName, element->ClientComponentName, element->ClientInterfaceRequiredName)) {
            if (serverInterfaceUID == GetInterfaceUID(element->ServerProcessName, element->ServerComponentName, element->ServerInterfaceProvidedName)) {
                ConnectionChange.Lock();
                delete element;
                ConnectionMap.erase(it);
                ConnectionChange.Unlock();

                removed = true;
                break;
            }
        }
    }

    if (!removed) {
        CMN_LOG_CLASS_RUN_VERBOSE << "Disconnect: failed to remove connection element: "
                                  << GetInterfaceUID(clientProcessName, clientComponentName, clientInterfaceRequiredName) << " - "
                                  << GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceProvidedName) << std::endl;
        return false;
    }

    CMN_LOG_CLASS_RUN_VERBOSE << "Disconnect: successfully disconnected: "
                              << GetInterfaceUID(clientProcessName, clientComponentName, clientInterfaceRequiredName) << " - "
                              << GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceProvidedName) << std::endl;
    */
}

bool mtsManagerGlobal::Disconnect(const ConnectionIDType connectionID)
{
    QueueDisconnectWaitingChange.Lock();

    DisconnectQueueType::const_iterator it1 = QueueDisconnectWaiting.find(connectionID);
    if (it1 != QueueDisconnectWaiting.end()) {
        QueueDisconnectWaitingChange.Unlock();
        return true;
    }

    DisconnectQueueType::const_iterator it2 = QueueDisconnected.find(connectionID);
    if (it2 != QueueDisconnected.end()) {
        QueueDisconnectWaitingChange.Unlock();
        return true;
    }

    ConnectionMapType::iterator it = ConnectionMap.find(connectionID);
    if (it == ConnectionMap.end()) {
        CMN_LOG_CLASS_INIT_ERROR << "Disconnect: invalid connection id: " << connectionID << std::endl;
        QueueDisconnectWaitingChange.Unlock();
        return false;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "zzzzzzzzzzzz 111111111111111111111111111" << std::endl;
    QueueDisconnectWaiting.insert(std::make_pair(connectionID, connectionID));

    CMN_LOG_CLASS_INIT_VERBOSE << "Disconnect: queueud connection id \"" << connectionID
        << "\" to disconnect waiting queue" << std::endl;

    QueueDisconnectWaitingChange.Unlock();

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

bool mtsManagerGlobal::GetInterfaceProvidedProxyAccessInfo(
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
        if ((description.Server.ProcessName == serverProcessName) &&
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

    return localManagerClient->ConnectClientSideInterface(description);
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

    return localManagerServer->ConnectServerSideInterface(description);
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
