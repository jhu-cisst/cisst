/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung
  Created on: 2009-11-12

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsConfig.h>
#include <cisstOSAbstraction/osaSocket.h>
#include <cisstMultiTask/mtsManagerGlobal.h>

#if CISST_MTS_HAS_ICE
#include <cisstMultiTask/mtsManagerProxyServer.h>
#include <cisstMultiTask/mtsComponentProxy.h>
#endif // CISST_MTS_HAS_ICE


mtsManagerGlobal::mtsManagerGlobal() :
    ProcessMap("ProcessMap"),
    LocalManagerConnected(0), ConnectionID(0)
#if CISST_MTS_HAS_ICE
    , ProxyServer(0)
#endif
{
    ProcessMap.SetOwner(*this);
}

mtsManagerGlobal::~mtsManagerGlobal()
{
    Cleanup();
}

//-------------------------------------------------------------------------
//  Processing Methods
//-------------------------------------------------------------------------
bool mtsManagerGlobal::Cleanup(void)
{
    bool ret = true;

    // Remove all processes safely
    ProcessMapType::iterator itProcess = ProcessMap.begin();
    while (itProcess != ProcessMap.end()) {
        ret &= RemoveProcess(itProcess->first);
        itProcess = ProcessMap.begin();
    }

    // Remove all connection elements
    ConnectionElementMapType::const_iterator itConnection = ConnectionElementMap.begin();
    const ConnectionElementMapType::const_iterator itConnectionEnd = ConnectionElementMap.begin();
    for (; itConnection != itConnectionEnd; ++itConnection) {
        delete itConnection->second;
    }
    ConnectionElementMap.clear();

    return ret;
}

bool mtsManagerGlobal::AddConnectedInterface(ConnectionMapType * connectionMap,
    const std::string & processName, const std::string & componentName,
    const std::string & interfaceName, const bool isRemoteConnection)
{
    if (!connectionMap) return false;

    ConnectedInterfaceInfo * connectedInterfaceInfo =
        new ConnectedInterfaceInfo(processName, componentName, interfaceName, isRemoteConnection);

    std::string interfaceUID = GetInterfaceUID(processName, componentName, interfaceName);
    if (!connectionMap->AddItem(interfaceUID, connectedInterfaceInfo)) {
        CMN_LOG_CLASS_RUN_ERROR << "Cannot add peer interface's information: "
                                << GetInterfaceUID(processName, componentName, interfaceName) << std::endl;
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------
//  Process Management
//-------------------------------------------------------------------------
bool mtsManagerGlobal::AddProcess(const std::string & processName)
{
    // Check if the local component manager has already been registered.
    if (FindProcess(processName)) {
        CMN_LOG_CLASS_RUN_ERROR << "AddProcess: already registered process: " << processName << std::endl;
        return false;
    }

    // Register to process map
    if (!ProcessMap.AddItem(processName, 0)) {
        CMN_LOG_CLASS_RUN_ERROR << "AddProcess: failed to add process to process map: " << processName << std::endl;
        return false;
    }

    return true;
}

bool mtsManagerGlobal::AddProcessObject(mtsManagerLocalInterface * localManagerObject, const bool isManagerProxyServer)
{
    if (LocalManagerConnected) {
        CMN_LOG_CLASS_RUN_ERROR << "AddProcessObject: local manager object has already been registered." << std::endl;
        return false;
    }

    // Name of local component manager which is now connecting
    std::string processName;

    // If localManagerObject is of type mtsManagerProxyServer, process name can
    // be set without calling mtsManagerLocalInterface::GetProcessName()
    if (isManagerProxyServer) {
#if CISST_MTS_HAS_ICE
        mtsManagerProxyServer * managerProxyServer = dynamic_cast<mtsManagerProxyServer *>(localManagerObject);
        if (!managerProxyServer) {
            CMN_LOG_CLASS_RUN_ERROR << "AddProcessObject: invalid object type (mtsManagerProxyServer expected)" << std::endl;
            return false;
        }
        processName = managerProxyServer->GetProxyName();
#endif
    } else {
        processName = localManagerObject->GetProcessName();
    }

    // Check if the local component manager has already been registered.
    if (FindProcess(processName)) {
        // Update LocalManagerMap
        LocalManagerConnected = localManagerObject;

        CMN_LOG_CLASS_RUN_VERBOSE << "AddProcessObject: updated local manager object" << std::endl;
        return true;
    }

    // Register to process map
    if (!ProcessMap.AddItem(processName, 0)) {
        CMN_LOG_CLASS_RUN_ERROR << "AddProcessObject: failed to add process to process map: " << processName << std::endl;
        return false;
    }

    // Register to local manager object map
    LocalManagerConnected = localManagerObject;

    return true;
}

bool mtsManagerGlobal::FindProcess(const std::string & processName) const
{
    return ProcessMap.FindItem(processName);
}

bool mtsManagerGlobal::RemoveProcess(const std::string & processName)
{
    if (!FindProcess(processName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveProcess: Can't find registered process: " << processName << std::endl;
        return false;
    }

    bool ret = true;
    ComponentMapType * componentMap = ProcessMap.GetItem(processName);

    // When componentMap is not NULL, all components that the process manages
    // should be removed first.
    if (componentMap) {
        ComponentMapType::iterator it = componentMap->begin();
        while (it != componentMap->end()) {
            ret &= RemoveComponent(processName, it->first);
            it = componentMap->begin();
        }

        delete componentMap;
    }

    // Remove the process from process map
    ret &= ProcessMap.RemoveItem(processName);

    return ret;
}

mtsManagerLocalInterface * mtsManagerGlobal::GetProcessObject(const std::string & processName)
{
    if (!ProcessMap.FindItem(processName)) {
        CMN_LOG_CLASS_RUN_ERROR << "GetProcessObject: Can't find registered process: " << processName << std::endl;
        return false;
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
            CMN_LOG_CLASS_INIT_VERBOSE << "SetIPAddress: This machine's IP: [" << i << "] " << ipAddresses[i] << std::endl;
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
            CMN_LOG_CLASS_INIT_VERBOSE << "SetIPAddress: This machine's IP: [" << i << "] " << ipAddresses[i] << std::endl;
        }
    }
}


//-------------------------------------------------------------------------
//  Component Management
//-------------------------------------------------------------------------
bool mtsManagerGlobal::AddComponent(const std::string & processName, const std::string & componentName)
{
    if (!FindProcess(processName)) {
        CMN_LOG_CLASS_RUN_ERROR << "AddComponent: Can't find registered process: " << processName << std::endl;
        return false;
    }

    ComponentMapType * componentMap = ProcessMap.GetItem(processName);

    // If the process did not register before
    if (componentMap == 0) {
        componentMap = new ComponentMapType(processName);
        (ProcessMap.GetMap())[processName] = componentMap;
    }

    bool ret = componentMap->AddItem(componentName, 0);
    if (!ret) {
        CMN_LOG_CLASS_RUN_ERROR << "AddComponent: Can't add a component: "
                                << "\"" << processName << "\" - \"" << componentName << "\"" << std::endl;
    }

    return ret;
}

bool mtsManagerGlobal::FindComponent(const std::string & processName, const std::string & componentName) const
{
    if (!FindProcess(processName)) {
        CMN_LOG_CLASS_RUN_ERROR << "FindComponent: Can't find a registered process: " << processName << std::endl;
        return false;
    }

    ComponentMapType * componentMap = ProcessMap.GetItem(processName);
    if (!componentMap) {
        return false;
    }

    return componentMap->FindItem(componentName);
}

bool mtsManagerGlobal::RemoveComponent(const std::string & processName, const std::string & componentName)
{
    // Check if the component has been registered
    if (!FindComponent(processName, componentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveComponent: Can't find component: "
                                << processName << ":" << componentName << std::endl;
        return false;
    }

    bool ret = true;

    ComponentMapType * componentMap = ProcessMap.GetItem(processName);
    CMN_ASSERT(componentMap);

    // When interfaceMapType is not NULL, all interfaces that the component manages
    // should be removed first.
    InterfaceMapType * interfaceMap = componentMap->GetItem(componentName);
    if (interfaceMap) {
        ConnectedInterfaceMapType::iterator it;

        // Remove all the required interfaces that the process manage.
        it = interfaceMap->InterfaceRequiredOrInputMap.GetMap().begin();
        while (it != interfaceMap->InterfaceRequiredOrInputMap.GetMap().end()) {
            ret &= RemoveInterfaceRequiredOrInput(processName, componentName, it->first);
            it = interfaceMap->InterfaceRequiredOrInputMap.GetMap().begin();
        }

        // Remove all the provided interfaces that the process manage.
        it = interfaceMap->InterfaceProvidedOrOutputMap.GetMap().begin();
        while (it != interfaceMap->InterfaceProvidedOrOutputMap.GetMap().end()) {
            ret &= RemoveInterfaceProvidedOrOutput(processName, componentName, it->first);
            it = interfaceMap->InterfaceProvidedOrOutputMap.GetMap().begin();
        }

        delete interfaceMap;
    }

    // Remove the component from component map
    ret &= componentMap->RemoveItem(componentName);

    return ret;
}

//-------------------------------------------------------------------------
//  Interface Management
//-------------------------------------------------------------------------
bool mtsManagerGlobal::AddInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName,
                                                    const std::string & interfaceName, const bool isProxyInterface)
{
    if (!FindComponent(processName, componentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "AddInterfaceProvidedOrOutput: can't find a registered component: "
                                << "\"" << processName << "\" - \"" << componentName << "\"" << std::endl;
        return false;
    }

    ComponentMapType * componentMap = ProcessMap.GetItem(processName);
    InterfaceMapType * interfaceMap = componentMap->GetItem(componentName);

    // If the component did not have any interface before
    if (interfaceMap == 0) {
        interfaceMap = new InterfaceMapType;
        (componentMap->GetMap())[componentName] = interfaceMap;
    }

    // Add the interface
    if (!interfaceMap->InterfaceProvidedOrOutputMap.AddItem(interfaceName, 0)) {
        CMN_LOG_CLASS_RUN_ERROR << "AddInterfaceProvidedOrOutput: can't add a provided/output interface: \""
                                << interfaceName << "\"" << std::endl;
        return false;
    }

    interfaceMap->InterfaceProvidedOrOutputTypeMap[interfaceName] = isProxyInterface;
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

    ComponentMapType * componentMap = ProcessMap.GetItem(processName);
    InterfaceMapType * interfaceMap = componentMap->GetItem(componentName);

    // If the component did not registered before
    if (interfaceMap == 0) {
        interfaceMap = new InterfaceMapType;
        (componentMap->GetMap())[componentName] = interfaceMap;
    }

    // Add the interface
    if (!interfaceMap->InterfaceRequiredOrInputMap.AddItem(interfaceName, 0)) {
        CMN_LOG_CLASS_RUN_ERROR << "AddInterfaceRequiredOrInput: can't add a required/input interface: \""
                                << interfaceName << "\"" << std::endl;
        return false;
    }

    interfaceMap->InterfaceRequiredOrInputTypeMap[interfaceName] = isProxyInterface;
    return true;
}


bool mtsManagerGlobal::FindInterfaceProvidedOrOutput(const std::string & processName,
                                                     const std::string & componentName,
                                                     const std::string & interfaceName) const
{
    if (!FindComponent(processName, componentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "FindInterfaceProvidedOrOutput: can't find a registered component: "
                                << "\"" << processName << "\" - \"" << componentName << "\"" << std::endl;
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
        CMN_LOG_CLASS_RUN_ERROR << "FindInterfaceRequiredOrInput: can't find a registered component: "
                                << "\"" << processName << "\" - \"" << componentName << "\"" << std::endl;
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
                                                       const std::string & interfaceName)
{
    // Check existence of the process
    if (!ProcessMap.FindItem(processName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceProvided: Can't find registered process: " << processName << std::endl;
        return false;
    }

    // Check existence of the component
    ComponentMapType * componentMap = ProcessMap.GetItem(processName);
    if (!componentMap) return false;
    if (!componentMap->FindItem(componentName)) return false;

    // Check existence of the provided interface
    InterfaceMapType * interfaceMap = componentMap->GetItem(componentName);
    if (!interfaceMap) return false;
    if (!interfaceMap->InterfaceProvidedOrOutputMap.FindItem(interfaceName)) return false;

    // When connectionMap is not NULL, all connection information that the provided interface
    // has should be removed first.
    bool ret = true;
    ConnectionMapType * connectionMap = interfaceMap->InterfaceProvidedOrOutputMap.GetItem(interfaceName);
    if (connectionMap) {
        if (connectionMap->size()) {
            ConnectionMapType::iterator it = connectionMap->begin();
            while (it != connectionMap->end()) {
                Disconnect(it->second->GetProcessName(), it->second->GetComponentName(), it->second->GetInterfaceName(),
                    processName, componentName, interfaceName);
                it = connectionMap->begin();
            }
            delete connectionMap;
        }
    }

    // Remove the provided interface from provided interface map
    ret &= interfaceMap->InterfaceProvidedOrOutputMap.RemoveItem(interfaceName);

    return ret;
}

bool mtsManagerGlobal::RemoveInterfaceRequiredOrInput(const std::string & processName,
                                                      const std::string & componentName,
                                                      const std::string & interfaceName)
{
    // Check if the process exists
    if (!ProcessMap.FindItem(processName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveInterfaceRequired: Can't find registered process: " << processName << std::endl;
        return false;
    }

    // Check if the component exists
    ComponentMapType * componentMap = ProcessMap.GetItem(processName);
    CMN_ASSERT(componentMap);
    if (!componentMap->FindItem(componentName)) return false;

    // Check if the provided interface exists
    InterfaceMapType * interfaceMap = componentMap->GetItem(componentName);
    if (!interfaceMap) return false;
    if (!interfaceMap->InterfaceRequiredOrInputMap.FindItem(interfaceName)) return false;

    // When connectionMap is not NULL, all the connections that the provided
    // interface is connected to should be removed.
    bool ret = true;
    ConnectionMapType * connectionMap = interfaceMap->InterfaceRequiredOrInputMap.GetItem(interfaceName);
    if (connectionMap) {
        if (connectionMap->size()) {
            ConnectionMapType::iterator it = connectionMap->begin();
            while (it != connectionMap->end()) {
                Disconnect(processName, componentName, interfaceName,
                    it->second->GetProcessName(), it->second->GetComponentName(), it->second->GetInterfaceName());
                it = connectionMap->begin();
            }
            delete connectionMap;
        }
    }

    // Remove the required interface from provided interface map
    ret &= interfaceMap->InterfaceRequiredOrInputMap.RemoveItem(interfaceName);

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

    // Check requestProcessName validity
    if (requestProcessName != clientProcessName && requestProcessName != serverProcessName) {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: invalid process is requesting connection: " << requestProcessName << std::endl;
        return -1;
    }

    // Check if the required interface specified actually exist.
    if (!FindInterfaceRequiredOrInput(clientProcessName, clientComponentName, clientInterfaceRequiredName)) {
        GetNamesOfInterfacesRequiredOrInput(clientProcessName, clientComponentName, options);
        if (options.size() == 0) {
            allOptions << "there is no required/input interface for this component";
        } else {
            allOptions << "this component has the following required/input interface(s): ";
            std::copy(options.begin(), options.end(), output);
        }
        CMN_LOG_CLASS_INIT_ERROR << "Connect: required/input interface has not been added to manager \""
                                 << GetInterfaceUID(clientProcessName, clientComponentName, clientInterfaceRequiredName)
                                 << "\", " << allOptions.str() << std::endl;
        return -1;
    }

    // Check if the provided interface specified actually exist.
    if (!FindInterfaceProvidedOrOutput(serverProcessName, serverComponentName, serverInterfaceProvidedName)) {
        GetNamesOfInterfacesProvidedOrOutput(serverProcessName, serverComponentName, options);
        if (options.size() == 0) {
            allOptions << "there is no provided/output interface for this component";
        } else {
            allOptions << "this component has the following provided/output interface(s): ";
            std::copy(options.begin(), options.end(), output);
        }
        CMN_LOG_CLASS_INIT_ERROR << "Connect: provided/output interface has not be added to manager \""
                                 << GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceProvidedName)
                                 << "\", " << allOptions.str() << std::endl;
        return -1;
    }

    // Check if the two interfaces are already connected.
    int ret = IsAlreadyConnected(clientProcessName,
                                 clientComponentName,
                                 clientInterfaceRequiredName,
                                 serverProcessName,
                                 serverComponentName,
                                 serverInterfaceProvidedName);
    // When error occurs (due to non-existing components, etc)
    if (ret < 0) {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: "
                                 << "one or more processes, components, or interfaces are missing." << std::endl;
        return -1;
    }
    // When the two interfaces have already been connected to each other
    else if (ret > 0) {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: these two interfaces are already connected: "
                                 << GetInterfaceUID(clientProcessName, clientComponentName, clientInterfaceRequiredName)
                                 << " and "
                                 << GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceProvidedName)
                                 << std::endl;
        return -1;
    }

    // ConnectionID is updated only when the global component manager
    // successfully establishes a connection.
    const unsigned int thisConnectionID = ConnectionID;

    // In case of remote connection
#if CISST_MTS_HAS_ICE
    if (clientProcessName != serverProcessName) {
        // Term definitions:
        // - Server manager: local component manager that manages server components
        // - Client manager: local component manager that manages client components

        //
        // STEP 1. Check if all component proxies required exist.  If not, create
        // the proxies as needed.
        //
        // Check if the server manager has a client component proxy.
        const std::string clientComponentProxyName = GetComponentProxyName(clientProcessName, clientComponentName);
        if (!FindComponent(serverProcessName, clientComponentProxyName)) {
            // If not, create one.
            if (!LocalManagerConnected->CreateComponentProxy(clientComponentProxyName, serverProcessName)) {
                CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to create client component proxy "
                                         << "\"" << clientComponentProxyName << "\" in server process "
                                         << "\"" << serverProcessName << "\"" << std::endl;
                return -1;
            }
            CMN_LOG_CLASS_INIT_VERBOSE << "Connect: successfully created client component proxy "
                                       << "\"" << clientComponentProxyName << "\" in server process "
                                       << "\"" << serverProcessName << "\"" << std::endl;
        }

        // Check if the client manager has a server component proxy.
        const std::string serverComponentProxyName = GetComponentProxyName(serverProcessName, serverComponentName);
        if (!FindComponent(clientProcessName, serverComponentProxyName)) {
            // If not, create one.
            if (!LocalManagerConnected->CreateComponentProxy(serverComponentProxyName, clientProcessName)) {
                CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to create server component proxy "
                                         << "\"" << serverComponentProxyName << "\" in client process "
                                         << "\"" << clientProcessName << "\"" << std::endl;
                return -1;
            }
            CMN_LOG_CLASS_INIT_VERBOSE << "Connect: successfully created server component proxy "
                                       << "\"" << serverComponentProxyName << "\" in client process "
                                       << "\"" << clientProcessName << "\"" << std::endl;
        }

        //
        // STEP 2. Check if all interface proxies required exist.  If not, create
        // the proxies as needed.
        //
        // Note that, under the current design, a provided interface can be
        // connect with multiple required interfaces whereas a required interface
        // can only connect to a single provided interface.
        // A required interface proxy, thus, is created whenever it is missing in
        // a server component while a provided interface proxy is generated only
        // at the first time when a client component doesn't have it.

        // Check if a provided interface proxy already exists at client side.
        bool foundProvidedInterfaceProxy = FindInterfaceProvidedOrOutput(clientProcessName, serverComponentProxyName, serverInterfaceProvidedName);

        // Check if a required interface proxy already exists at server side.
        bool foundRequiredInterfaceProxy = FindInterfaceRequiredOrInput(serverProcessName, clientComponentProxyName, clientInterfaceRequiredName);

        // From the server manager and the client manager, extract information
        // about the two interfaces specified. The global component manager then
        // deliver the information to each local component manager so that they
        // can create interface proxies.
        // Note that a required interface proxy at the server side has to be
        // created first because pointers to function proxy objects of it are
        // required to set ids of command and event handler proxies' of provided
        // interface proxy at the client side.

        // Create required interface proxy
        if (!foundRequiredInterfaceProxy) {
            // Extract required interface information from the client process
            InterfaceRequiredDescription requiredInterfaceDescription;
            if (!LocalManagerConnected->GetInterfaceRequiredDescription(
                    clientComponentName, clientInterfaceRequiredName, requiredInterfaceDescription, clientProcessName))
            {
                CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to get required interface description: "
                                         << GetInterfaceUID(clientProcessName, clientComponentName, clientInterfaceRequiredName) << std::endl;
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

        // Create provided interface proxy
        if (!foundProvidedInterfaceProxy) {
            // Extract provided interface information from the server process
            InterfaceProvidedDescription providedInterfaceDescription;
            if (!LocalManagerConnected->GetInterfaceProvidedDescription(
                    serverComponentName, serverInterfaceProvidedName, providedInterfaceDescription, serverProcessName))
            {
                CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to get provided interface description: "
                                         << GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceProvidedName) << std::endl;
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
#endif

    InterfaceMapType * interfaceMap;

    // Connect client's required interface with server's provided interface.
    ConnectionMapType * connectionMap =
        GetConnectionsOfInterfaceRequiredOrInput(clientProcessName, clientComponentName, clientInterfaceRequiredName, &interfaceMap);
    // If the required interface has never connected to other provided interfaces
    if (connectionMap == 0) {
        // Create a connection map for the required interface
        connectionMap = new ConnectionMapType(clientInterfaceRequiredName);
        (interfaceMap->InterfaceRequiredOrInputMap.GetMap())[clientInterfaceRequiredName] = connectionMap;
    }

    // Add an element containing information about the connected provided interface
    bool isRemoteConnection = (clientProcessName != serverProcessName);
    if (!AddConnectedInterface(connectionMap, serverProcessName, serverComponentName, serverInterfaceProvidedName, isRemoteConnection)) {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to add information about connected provided interface." << std::endl;
        return -1;
    }

    // Connect server's provided interface with client's required interface.
    connectionMap = GetConnectionsOfInterfaceProvidedOrOutput(serverProcessName,
                                                              serverComponentName,
                                                              serverInterfaceProvidedName,
                                                              &interfaceMap);
    // If the provided interface has never been connected with other required interfaces
    if (connectionMap == 0) {
        // Create a connection map for the provided interface
        connectionMap = new ConnectionMapType(serverInterfaceProvidedName);
        (interfaceMap->InterfaceProvidedOrOutputMap.GetMap())[serverInterfaceProvidedName] = connectionMap;
    }

    // Add an element containing information about the connected provided interface
    if (!AddConnectedInterface(connectionMap, clientProcessName, clientComponentName, clientInterfaceRequiredName, isRemoteConnection)) {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to add information about connected required interface." << std::endl;

        // Before returning false, should clean up required interface's connection information
        Disconnect(clientProcessName, clientComponentName, clientInterfaceRequiredName,
                   serverProcessName, serverComponentName, serverInterfaceProvidedName);
        return -1;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "Connect: successfully connected: "
                               << GetInterfaceUID(clientProcessName, clientComponentName, clientInterfaceRequiredName) << " - "
                               << GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceProvidedName) << std::endl;


    // Create an instance of ConnectionElement
    ConnectionElement * element = new ConnectionElement(requestProcessName, thisConnectionID,
        clientProcessName, clientComponentName, clientInterfaceRequiredName,
        serverProcessName, serverComponentName, serverInterfaceProvidedName);

    ConnectionElementMapChange.Lock();
    ConnectionElementMap.insert(std::make_pair(ConnectionID, element));
    ConnectionElementMapChange.Unlock();

    // Increase counter for next connection id
    ++ConnectionID;

    return (int) thisConnectionID;
}

#if CISST_MTS_HAS_ICE
void mtsManagerGlobal::ConnectCheckTimeout()
{
    if (ConnectionElementMap.empty()) return;

    ConnectionElementMapChange.Lock();

    ConnectionElement * element;
    ConnectionElementMapType::iterator it = ConnectionElementMap.begin();
    for (; it != ConnectionElementMap.end(); ) {
        element = it->second;

        // Skip timeout check if connection has been already established.
        if (element->IsConnected()) {
            ++it;
            continue;
        }

        // Timeout check
        if (!element->CheckTimeout()) {
            ++it;
            continue;
        }

        CMN_LOG_CLASS_RUN_VERBOSE << "ConnectCheckTimeout: Disconnect due to timeout: Connection session id: " << element->GetConnectionID() << std::endl
                                  << "ConnectCheckTimeout: Remove an element: "
                                  << GetInterfaceUID(element->ClientProcessName, element->ClientComponentName, element->ClientInterfaceRequiredName) << " - "
                                  << GetInterfaceUID(element->ServerProcessName, element->ServerComponentName, element->ServerInterfaceProvidedName) << std::endl;

        // Remove an element
        delete element;
        ConnectionElementMap.erase(it++);
    }

    ConnectionElementMapChange.Unlock();
}
#endif

bool mtsManagerGlobal::ConnectConfirm(unsigned int connectionSessionID)
{
    ConnectionElementMapType::const_iterator it = ConnectionElementMap.find(connectionSessionID);
    if (it == ConnectionElementMap.end()) {
        CMN_LOG_CLASS_RUN_ERROR << "ConnectConfirm: invalid session id: " << connectionSessionID << std::endl;
        return false;
    }

    ConnectionElement * element = it->second;
    element->SetConnected();

    CMN_LOG_CLASS_RUN_VERBOSE << "ConnectConfirm: connection (id: " << connectionSessionID << ") is confirmed" << std::endl;

    return true;
}

bool mtsManagerGlobal::IsAlreadyConnected(const std::string & clientProcessName,
                                          const std::string & clientComponentName,
                                          const std::string & clientInterfaceRequiredName,
                                          const std::string & serverProcessName,
                                          const std::string & serverComponentName,
                                          const std::string & serverInterfaceProvidedName)
{
    // It is assumed that the existence of interfaces has already been checked before
    // calling this method.

    // Check if the required interface is connected to the provided interface
    ConnectionMapType * connectionMap =
        GetConnectionsOfInterfaceRequiredOrInput(clientProcessName, clientComponentName, clientInterfaceRequiredName);
    if (connectionMap) {
        if (connectionMap->FindItem(GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceProvidedName)))
        {
            CMN_LOG_CLASS_RUN_VERBOSE << "Already established connection" << std::endl;
            return true;
        }
    }

    // Check if the provided interface is connected to the required interface
    connectionMap = GetConnectionsOfInterfaceProvidedOrOutput(serverProcessName, serverComponentName, serverInterfaceProvidedName);
    if (connectionMap) {
        if (connectionMap->FindItem(GetInterfaceUID(clientProcessName, clientComponentName, clientInterfaceRequiredName)))
        {
            CMN_LOG_CLASS_RUN_VERBOSE << "Already established connection" << std::endl;
            return true;
        }
    }

    return false;
}

bool mtsManagerGlobal::Disconnect(const std::string & clientProcessName, const std::string & clientComponentName,
                                  const std::string & clientInterfaceRequiredName,
                                  const std::string & serverProcessName, const std::string & serverComponentName,
                                  const std::string & serverInterfaceProvidedName)
{
    // Get connection information
    ConnectionMapType * connectionMapOfInterfaceRequired =
        GetConnectionsOfInterfaceRequiredOrInput(clientProcessName, clientComponentName, clientInterfaceRequiredName);
    if (!connectionMapOfInterfaceRequired) {
        CMN_LOG_CLASS_RUN_ERROR << "Disconnect: failed to disconnect. Required interface has no connection: "
                                << GetInterfaceUID(clientProcessName, clientComponentName, clientInterfaceRequiredName) << std::endl;
        return false;
    }

    ConnectionMapType * connectionMapOfInterfaceProvided =
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
                mtsManagerProxyServer * proxyClient = dynamic_cast<mtsManagerProxyServer *>(localManagerServer);
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
    ConnectionElementMapType::iterator it = ConnectionElementMap.begin();
    for (; it != ConnectionElementMap.end(); ++it) {
        element = it->second;
        if (clientInterfaceUID == GetInterfaceUID(element->ClientProcessName, element->ClientComponentName, element->ClientInterfaceRequiredName)) {
            if (serverInterfaceUID == GetInterfaceUID(element->ServerProcessName, element->ServerComponentName, element->ServerInterfaceProvidedName)) {
                ConnectionElementMapChange.Lock();
                delete element;
                ConnectionElementMap.erase(it);
                ConnectionElementMapChange.Unlock();

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

    return true;
}

//-------------------------------------------------------------------------
//  Getters
//-------------------------------------------------------------------------
mtsManagerGlobal::ConnectionMapType *
mtsManagerGlobal::GetConnectionsOfInterfaceProvidedOrOutput(const std::string & severProcessName, const std::string & serverComponentName,
                                                            const std::string & providedInterfaceName, InterfaceMapType ** interfaceMap)
{
    ComponentMapType * componentMap = ProcessMap.GetItem(severProcessName);
    if (componentMap == 0) return 0;

    *interfaceMap = componentMap->GetItem(serverComponentName);
    if (*interfaceMap == 0) return 0;

    ConnectionMapType * connectionMap = (*interfaceMap)->InterfaceProvidedOrOutputMap.GetItem(providedInterfaceName);

    return connectionMap;
}

mtsManagerGlobal::ConnectionMapType *
mtsManagerGlobal::GetConnectionsOfInterfaceProvidedOrOutput(const std::string & severProcessName, const std::string & serverComponentName,
                                                            const std::string & providedInterfaceName) const
{
    ComponentMapType * componentMap = ProcessMap.GetItem(severProcessName);
    if (componentMap == 0) return 0;

    InterfaceMapType * interfaceMap = componentMap->GetItem(serverComponentName);
    if (interfaceMap == 0) return 0;

    ConnectionMapType * connectionMap = interfaceMap->InterfaceProvidedOrOutputMap.GetItem(providedInterfaceName);

    return connectionMap;
}

mtsManagerGlobal::ConnectionMapType *
mtsManagerGlobal::GetConnectionsOfInterfaceRequiredOrInput(const std::string & clientProcessName, const std::string & clientComponentName,
                                                           const std::string & requiredInterfaceName, InterfaceMapType ** interfaceMap)
{
    ComponentMapType * componentMap = ProcessMap.GetItem(clientProcessName);
    if (componentMap == 0) return 0;

    *interfaceMap = componentMap->GetItem(clientComponentName);
    if (*interfaceMap == 0) return 0;

    ConnectionMapType * connectionMap = (*interfaceMap)->InterfaceRequiredOrInputMap.GetItem(requiredInterfaceName);

    return connectionMap;
}

mtsManagerGlobal::ConnectionMapType *
mtsManagerGlobal::GetConnectionsOfInterfaceRequiredOrInput(const std::string & clientProcessName, const std::string & clientComponentName,
                                                           const std::string & requiredInterfaceName) const
{
    ComponentMapType * componentMap = ProcessMap.GetItem(clientProcessName);
    if (componentMap == 0) return 0;

    InterfaceMapType * interfaceMap = componentMap->GetItem(clientComponentName);
    if (interfaceMap == 0) return 0;

    ConnectionMapType * connectionMap = interfaceMap->InterfaceRequiredOrInputMap.GetItem(requiredInterfaceName);

    return connectionMap;
}

void mtsManagerGlobal::GetNamesOfProcesses(std::vector<std::string>& namesOfProcesses)
{
    std::vector<std::string> temp;
    ProcessMap.GetNames(temp);

    // Filter out mtsManagerProxyServer process which has an ICE proxy that serves
    // local component managers.
    // Filter out local ICE proxy of type mtsManagerProxyServer.
    for (size_t i = 0; i < temp.size(); ++i) {
        if (temp[i] == "ManagerProxyServer") {
            continue;
        }
        namesOfProcesses.push_back(temp[i]);
    }
}

void mtsManagerGlobal::GetNamesOfComponents(const std::string & processName,
                                            std::vector<std::string>& namesOfComponents)
{
    ComponentMapType * components = ProcessMap.GetItem(processName);
    if (!components) return;

    components->GetNames(namesOfComponents);
}

void mtsManagerGlobal::GetNamesOfInterfacesProvidedOrOutput(const std::string & processName,
                                                            const std::string & componentName,
                                                            std::vector<std::string>& namesOfInterfacesProvided)
{
    ComponentMapType * components = ProcessMap.GetItem(processName);
    if (!components) return;

    InterfaceMapType * interfaces = components->GetItem(componentName);
    if (!interfaces) return;

    interfaces->InterfaceProvidedOrOutputMap.GetNames(namesOfInterfacesProvided);
}

void mtsManagerGlobal::GetNamesOfInterfacesRequiredOrInput(const std::string & processName,
                                                           const std::string & componentName,
                                                           std::vector<std::string>& namesOfInterfacesRequired)
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
                                          std::vector<std::string>& namesOfCommands)
{
    if (!LocalManagerConnected) return;

    LocalManagerConnected->GetNamesOfCommands(namesOfCommands, componentName, providedInterfaceName, processName);
}

void mtsManagerGlobal::GetNamesOfEventGenerators(const std::string & processName,
                                                 const std::string & componentName,
                                                 const std::string & providedInterfaceName,
                                                 std::vector<std::string>& namesOfEventGenerators)
{
    if (!LocalManagerConnected) return;

    LocalManagerConnected->GetNamesOfEventGenerators(namesOfEventGenerators, componentName, providedInterfaceName, processName);
}

void mtsManagerGlobal::GetNamesOfFunctions(const std::string & processName,
                                           const std::string & componentName,
                                           const std::string & requiredInterfaceName,
                                           std::vector<std::string>& namesOfFunctions)
{
    if (!LocalManagerConnected) return;

    LocalManagerConnected->GetNamesOfFunctions(namesOfFunctions, componentName, requiredInterfaceName, processName);
}

void mtsManagerGlobal::GetNamesOfEventHandlers(const std::string & processName,
                                               const std::string & componentName,
                                               const std::string & requiredInterfaceName,
                                               std::vector<std::string>& namesOfEventHandlers)
{
    if (!LocalManagerConnected) return;

    LocalManagerConnected->GetNamesOfEventHandlers(namesOfEventHandlers, componentName, requiredInterfaceName, processName);
}

void mtsManagerGlobal::GetDescriptionOfCommand(const std::string & processName,
                                               const std::string & componentName,
                                               const std::string & providedInterfaceName,
                                               const std::string & commandName,
                                               std::string & description)
{
    if (!LocalManagerConnected) return;

    LocalManagerConnected->GetDescriptionOfCommand(description, componentName, providedInterfaceName, commandName, processName);
}

void mtsManagerGlobal::GetDescriptionOfEventGenerator(const std::string & processName,
                                                      const std::string & componentName,
                                                      const std::string & providedInterfaceName,
                                                      const std::string & eventGeneratorName,
                                                      std::string & description)
{
    if (!LocalManagerConnected) return;

    LocalManagerConnected->GetDescriptionOfEventGenerator(description, componentName, providedInterfaceName, eventGeneratorName, processName);
}

void mtsManagerGlobal::GetDescriptionOfFunction(const std::string & processName,
                                                const std::string & componentName,
                                                const std::string & requiredInterfaceName,
                                                const std::string & functionName,
                                                std::string & description)
{
    if (!LocalManagerConnected) return;

    LocalManagerConnected->GetDescriptionOfFunction(description, componentName, requiredInterfaceName, functionName, processName);
}

void mtsManagerGlobal::GetDescriptionOfEventHandler(const std::string & processName,
                                                    const std::string & componentName,
                                                    const std::string & requiredInterfaceName,
                                                    const std::string & eventHandlerName,
                                                    std::string & description)
{
    if (!LocalManagerConnected) return;

    LocalManagerConnected->GetDescriptionOfEventHandler(description, componentName, requiredInterfaceName, eventHandlerName, processName);
}

void mtsManagerGlobal::GetArgumentInformation(const std::string & processName,
                                              const std::string & componentName,
                                              const std::string & providedInterfaceName,
                                              const std::string & commandName,
                                              std::string & argumentName,
                                              std::vector<std::string> & signalNames)
{
    if (!LocalManagerConnected) return;

    LocalManagerConnected->GetArgumentInformation(argumentName, signalNames, componentName, providedInterfaceName, commandName, processName);
}

void mtsManagerGlobal::GetValuesOfCommand(const std::string & processName,
                                          const std::string & componentName,
                                          const std::string & providedInterfaceName,
                                          const std::string & commandName,
                                          const int scalarIndex,
                                          mtsManagerLocalInterface::SetOfValues & values)
{
    if (!LocalManagerConnected) return;

    LocalManagerConnected->GetValuesOfCommand(values, componentName, providedInterfaceName, commandName, scalarIndex, processName);
}

#endif

//-------------------------------------------------------------------------
//  Networking
//-------------------------------------------------------------------------
#if CISST_MTS_HAS_ICE
bool mtsManagerGlobal::StartServer()
{
    // Create an instance of mtsComponentInterfaceProxyServer
    ProxyServer = new mtsManagerProxyServer("ManagerServerAdapter", mtsManagerProxyServer::GetManagerCommunicatorID());

    // Run proxy server
    if (!ProxyServer->Start(this)) {
        CMN_LOG_CLASS_RUN_ERROR << "StartServer: Proxy failed to start: " << GetName() << std::endl;
        return false;
    }

    ProxyServer->GetLogger()->trace("mtsManagerGlobal", "Global component manager started.");

    // Register an instance of mtsComponentInterfaceProxyServer
    LocalManagerConnected = ProxyServer;

    return true;
}

bool mtsManagerGlobal::StopServer()
{
    if (!ProxyServer) {
        CMN_LOG_CLASS_RUN_ERROR << "StopServer: Proxy is not initialized" << std::endl;
        return false;
    }

    if (!ProxyServer->IsActiveProxy()) {
        CMN_LOG_CLASS_RUN_ERROR << "StopServer: Proxy is not running: " << GetName() << std::endl;
        return false;
    }

    LocalManagerConnected = 0;

    // Stop proxy server
    ProxyServer->Stop();
    ProxyServer->GetLogger()->trace("mtsManagerGlobal", "Global component manager stopped.");

    delete ProxyServer;

    return true;
}

bool mtsManagerGlobal::SetInterfaceProvidedProxyAccessInfo(
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName,
    const std::string & endpointInfo)
{
    // Get a connection map of the provided interface at server side.
    ConnectionMapType * connectionMap =
        GetConnectionsOfInterfaceProvidedOrOutput(serverProcessName, serverComponentName, serverInterfaceProvidedName);
    if (!connectionMap) {
        CMN_LOG_CLASS_RUN_ERROR << "SetInterfaceProvidedProxyAccessInfo: failed to get connection map: "
                                << serverProcessName << ":" << serverComponentName << ":" << serverInterfaceProvidedName << std::endl;
        return false;
    }

    // Get the information about the connected required interface
    const std::string requiredInterfaceUID = GetInterfaceUID(clientProcessName, clientComponentName, clientInterfaceRequiredName);
    mtsManagerGlobal::ConnectedInterfaceInfo * connectedInterfaceInfo = connectionMap->GetItem(requiredInterfaceUID);
    if (!connectedInterfaceInfo) {
        CMN_LOG_CLASS_RUN_ERROR << "SetInterfaceProvidedProxyAccessInfo: failed to get connection information"
                                << clientProcessName << ":" << clientComponentName << ":" << clientInterfaceRequiredName << std::endl;
        return false;
    }

    // Set server proxy access information
    connectedInterfaceInfo->SetProxyAccessInfo(endpointInfo);

    CMN_LOG_CLASS_RUN_VERBOSE << "SetInterfaceProvidedProxyAccessInfo: set proxy access info: " << endpointInfo << std::endl;

    return true;
}

bool mtsManagerGlobal::GetInterfaceProvidedProxyAccessInfo(
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName,
    std::string & endpointInfo)
{
    // Get a connection map of the provided interface at server side.
    ConnectionMapType * connectionMap =
        GetConnectionsOfInterfaceProvidedOrOutput(serverProcessName, serverComponentName, serverInterfaceProvidedName);
    if (!connectionMap) {
        CMN_LOG_CLASS_RUN_ERROR << "GetInterfaceProvidedProxyAccessInfo: failed to get connection map: "
                                << mtsManagerGlobal::GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceProvidedName) << std::endl;
        return false;
    }

    // Get the information about the connected required interface
    mtsManagerGlobal::ConnectedInterfaceInfo * connectedInterfaceInfo;
    // If a client's required interface is not specified
    if (clientProcessName == "" && clientComponentName == "" && clientInterfaceRequiredName == "") {
        mtsManagerGlobal::ConnectionMapType::const_iterator itFirst = connectionMap->begin();
        if (itFirst == connectionMap->end()) {
            CMN_LOG_CLASS_RUN_ERROR << "GetInterfaceProvidedProxyAccessInfo: failed to get connection information (no data): "
                                    << mtsManagerGlobal::GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceProvidedName) << std::endl;
            return false;
        }
        connectedInterfaceInfo = itFirst->second;
    }
    // If a client's required interface is specified
    else {
        const std::string requiredInterfaceUID = GetInterfaceUID(clientProcessName, clientComponentName, clientInterfaceRequiredName);
        connectedInterfaceInfo = connectionMap->GetItem(requiredInterfaceUID);
    }

    if (!connectedInterfaceInfo) {
        CMN_LOG_CLASS_RUN_ERROR << "GetInterfaceProvidedProxyAccessInfo: failed to get connection information"
                                << clientProcessName << ":" << clientComponentName << ":" << clientInterfaceRequiredName << std::endl;
        return false;
    }

    // Get server proxy access information
    endpointInfo = connectedInterfaceInfo->GetEndpointInfo();

    return true;
}

bool mtsManagerGlobal::InitiateConnect(const unsigned int connectionID,
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName)
{
    // Get local component manager that manages the client component.
    mtsManagerLocalInterface * localManagerClient = GetProcessObject(clientProcessName);
    if (!localManagerClient) {
        CMN_LOG_CLASS_RUN_ERROR << "InitiateConnect: Cannot find local component manager with client process: " << clientProcessName << std::endl;
        return false;
    }

    return localManagerClient->ConnectClientSideInterface(connectionID,
        clientProcessName, clientComponentName, clientInterfaceRequiredName,
        serverProcessName, serverComponentName, serverInterfaceProvidedName, clientProcessName);
}

bool mtsManagerGlobal::ConnectServerSideInterfaceRequest(const unsigned int connectionID,
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName)
{
    // Get local component manager that manages the server component.
    mtsManagerLocalInterface * localManagerServer = GetProcessObject(serverProcessName);
    if (!localManagerServer) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectServerSideInterfaceRequest: no server process "
                                 << "\"" << serverProcessName << "\" found in this process" << std::endl;
        return false;
    }

    return localManagerServer->ConnectServerSideInterface(connectionID,
        clientProcessName, clientComponentName, clientInterfaceRequiredName,
        serverProcessName, serverComponentName, serverInterfaceProvidedName, serverProcessName);
}
#endif

void mtsManagerGlobal::GetListOfConnections(std::vector<ConnectionStrings> & list) const
{
    ConnectionStrings connection;

    ConnectionElementMapType::const_iterator it = ConnectionElementMap.begin();
    const ConnectionElementMapType::const_iterator itEnd = ConnectionElementMap.end();

    for (; it != itEnd; ++it) {
        // Check if this connection has been successfully established
        if (!it->second->IsConnected()) {
            continue;
        }

        connection.ClientProcessName           = it->second->ClientProcessName;
        connection.ClientComponentName         = it->second->ClientComponentName;
        connection.ClientInterfaceRequiredName = it->second->ClientInterfaceRequiredName;
        connection.ServerProcessName           = it->second->ServerProcessName;
        connection.ServerComponentName         = it->second->ServerComponentName;
        connection.ServerInterfaceProvidedName = it->second->ServerInterfaceProvidedName;

        list.push_back(connection);
    }
}
