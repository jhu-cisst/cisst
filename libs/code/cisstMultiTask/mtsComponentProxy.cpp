/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung
  Created on: 2009-12-18

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsComponentProxy.h>

#include <cisstCommon/cmnUnits.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsRequiredInterface.h>
#include <cisstMultiTask/mtsFunctionReadOrWriteProxy.h>
#include <cisstMultiTask/mtsFunctionQualifiedReadOrWriteProxy.h>

CMN_IMPLEMENT_SERVICES(mtsComponentProxy);

mtsComponentProxy::mtsComponentProxy(const std::string & componentProxyName)
: mtsDevice(componentProxyName), ProvidedInterfaceProxyInstanceID(0)
{
}

mtsComponentProxy::~mtsComponentProxy()
{
    // Clean up all internal resources

    // Stop all provided interface proxies running.
    ProvidedInterfaceNetworkProxyMapType::MapType::const_iterator itProvidedProxy = ProvidedInterfaceNetworkProxies.begin();
    const ProvidedInterfaceNetworkProxyMapType::MapType::const_iterator itProvidedProxyEnd = ProvidedInterfaceNetworkProxies.end();
    for (; itProvidedProxy != itProvidedProxyEnd; ++itProvidedProxy) {
        itProvidedProxy->second->Stop();
    }
    ProvidedInterfaceNetworkProxies.DeleteAll();

    // Stop all required interface proxies running.
    RequiredInterfaceNetworkProxyMapType::MapType::const_iterator itRequiredProxy = RequiredInterfaceNetworkProxies.begin();
    const RequiredInterfaceNetworkProxyMapType::MapType::const_iterator itRequiredProxyEnd = RequiredInterfaceNetworkProxies.end();
    for (; itRequiredProxy != itRequiredProxyEnd; ++itRequiredProxy) {
        itRequiredProxy->second->Stop();
    }
    RequiredInterfaceNetworkProxies.DeleteAll();


    ProvidedInterfaceProxyInstanceMapType::const_iterator itInstance = ProvidedInterfaceProxyInstanceMap.begin();
    const ProvidedInterfaceProxyInstanceMapType::const_iterator itInstanceEnd = ProvidedInterfaceProxyInstanceMap.end();
    for (; itInstance != itInstanceEnd; ++itInstance) {
        delete itInstance->second;
    }

    FunctionProxyAndEventHandlerProxyMap.DeleteAll();
}

//-----------------------------------------------------------------------------
//  Methods for Server Components
//-----------------------------------------------------------------------------
bool mtsComponentProxy::CreateRequiredInterfaceProxy(const RequiredInterfaceDescription & requiredInterfaceDescription)
{
    const std::string requiredInterfaceName = requiredInterfaceDescription.RequiredInterfaceName;

    // Check if the interface name is unique
    mtsRequiredInterface * requiredInterface = GetRequiredInterface(requiredInterfaceName);
    if (requiredInterface) {
        CMN_LOG_CLASS_RUN_ERROR << "CreateRequiredInterfaceProxy: can't create required interface proxy: "
            << "duplicate name: " << requiredInterfaceName << std::endl;
        return false;
    }

    // Create a required interface proxy
    mtsRequiredInterface * requiredInterfaceProxy = new mtsRequiredInterface(requiredInterfaceName);

    // Store function proxy pointers and event handler proxy pointers to assign
    // command proxies' id at server side.
    FunctionProxyAndEventHandlerProxyMapElement * mapElement = new FunctionProxyAndEventHandlerProxyMapElement;

    // Populate the new required interface
    mtsFunctionVoid * functionVoidProxy;
    mtsFunctionWrite * functionWriteProxy;
    mtsFunctionRead * functionReadProxy;
    mtsFunctionQualifiedRead * functionQualifiedReadProxy;

    bool success;

    // Create void function proxies
    const std::vector<std::string> namesOfFunctionVoid = requiredInterfaceDescription.FunctionVoidNames;
    for (unsigned int i = 0; i < namesOfFunctionVoid.size(); ++i) {
        functionVoidProxy = new mtsFunctionVoid();
        success = requiredInterfaceProxy->AddFunction(namesOfFunctionVoid[i], *functionVoidProxy);
        success &= mapElement->FunctionVoidProxyMap.AddItem(namesOfFunctionVoid[i], functionVoidProxy);
        if (!success) {
            delete requiredInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateRequiredInterfaceProxy: failed to add void function proxy: " << namesOfFunctionVoid[i] << std::endl;
            return false;
        }
    }

    // Create write function proxies
    const std::vector<std::string> namesOfFunctionWrite = requiredInterfaceDescription.FunctionWriteNames;
    for (unsigned int i = 0; i < namesOfFunctionWrite.size(); ++i) {
        functionWriteProxy = new mtsFunctionWriteProxy();
        success = requiredInterfaceProxy->AddFunction(namesOfFunctionWrite[i], *functionWriteProxy);
        success &= mapElement->FunctionWriteProxyMap.AddItem(namesOfFunctionWrite[i], functionWriteProxy);
        if (!success) {
            delete requiredInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateRequiredInterfaceProxy: failed to add write function proxy: " << namesOfFunctionWrite[i] << std::endl;
            return false;
        }
    }

    // Create read function proxies
    const std::vector<std::string> namesOfFunctionRead = requiredInterfaceDescription.FunctionReadNames;
    for (unsigned int i = 0; i < namesOfFunctionRead.size(); ++i) {
        functionReadProxy = new mtsFunctionReadProxy();
        success = requiredInterfaceProxy->AddFunction(namesOfFunctionRead[i], *functionReadProxy);
        success &= mapElement->FunctionReadProxyMap.AddItem(namesOfFunctionRead[i], functionReadProxy);
        if (!success) {
            delete requiredInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateRequiredInterfaceProxy: failed to add read function proxy: " << namesOfFunctionRead[i] << std::endl;
            return false;
        }
    }

    // Create QualifiedRead function proxies
    const std::vector<std::string> namesOfFunctionQualifiedRead = requiredInterfaceDescription.FunctionQualifiedReadNames;
    for (unsigned int i = 0; i < namesOfFunctionQualifiedRead.size(); ++i) {
        functionQualifiedReadProxy = new mtsFunctionQualifiedReadProxy();
        success = requiredInterfaceProxy->AddFunction(namesOfFunctionQualifiedRead[i], *functionQualifiedReadProxy);
        success &= success &= mapElement->FunctionQualifiedReadProxyMap.AddItem(namesOfFunctionQualifiedRead[i], functionQualifiedReadProxy);
        if (!success) {
            delete requiredInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateRequiredInterfaceProxy: failed to add qualified read function proxy: " << namesOfFunctionQualifiedRead[i] << std::endl;
            return false;
        }
    }

    // Create event handler proxies
    std::string eventName;

    // Create event handler proxies with CommandIDs set to zero and disable them
    // by default. It will be updated and enabled later by UpdateEventHandlerID(),

    // Create void event handler proxy
    mtsCommandVoidProxy * newEventVoidHandlerProxy = NULL;
    for (unsigned int i = 0; i < requiredInterfaceDescription.EventHandlersVoid.size(); ++i) {
        eventName = requiredInterfaceDescription.EventHandlersVoid[i].Name;
        newEventVoidHandlerProxy = new mtsCommandVoidProxy(eventName);
        if (!requiredInterfaceProxy->EventHandlersVoid.AddItem(eventName, newEventVoidHandlerProxy)) {
            delete newEventVoidHandlerProxy;
            delete requiredInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateRequiredInterfaceProxy: failed to add void event handler proxy: " << eventName << std::endl;
            return false;
        }
    }

    // Create write event handler proxies
    std::stringstream streamBuffer;
    cmnDeSerializer deserializer(streamBuffer);

    mtsCommandWriteProxy * newEventWriteHandlerProxy = NULL;
    mtsGenericObject * argumentPrototype = NULL;
    for (unsigned int i = 0; i < requiredInterfaceDescription.EventHandlersWrite.size(); ++i) {
        eventName = requiredInterfaceDescription.EventHandlersWrite[i].Name;
        newEventWriteHandlerProxy = new mtsCommandWriteProxy(eventName);
        if (!requiredInterfaceProxy->EventHandlersWrite.AddItem(eventName, newEventWriteHandlerProxy)) {
            delete newEventWriteHandlerProxy;
            delete requiredInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateRequiredInterfaceProxy: failed to add write event handler proxy: " << eventName << std::endl;
            return false;
        }

        // argument deserialization
        streamBuffer.str("");
        streamBuffer << requiredInterfaceDescription.EventHandlersWrite[i].ArgumentPrototypeSerialized;
        try {
            argumentPrototype = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        } catch (std::exception e) {
            CMN_LOG_CLASS_RUN_ERROR << "CreateRequiredInterfaceProxy: write command argument deserialization failed: " << e.what() << std::endl;
            argumentPrototype = NULL;
        }

        if (!argumentPrototype) {
            delete requiredInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateRequiredInterfaceProxy: failed to create event write handler proxy: " << eventName << std::endl;
            return false;
        }
        newEventWriteHandlerProxy->SetArgumentPrototype(argumentPrototype);
    }

    // Add the required interface proxy to the component
    if (!AddRequiredInterface(requiredInterfaceName, requiredInterfaceProxy)) {
        CMN_LOG_CLASS_RUN_ERROR << "CreateRequiredInterfaceProxy: failed to add required interface proxy: " << requiredInterfaceName << std::endl;
        delete requiredInterfaceProxy;
        return false;
    }

    // Add to function proxy and event handler proxy map
    if (!FunctionProxyAndEventHandlerProxyMap.AddItem(requiredInterfaceName, mapElement)) {
        CMN_LOG_CLASS_RUN_ERROR << "CreateRequiredInterfaceProxy: failed to add proxy map: " << requiredInterfaceName << std::endl;
        return false;
    }

    CMN_LOG_CLASS_RUN_VERBOSE << "CreateRequiredInterfaceProxy: added required interface proxy: " << requiredInterfaceName << std::endl;

    return true;
}

bool mtsComponentProxy::RemoveRequiredInterfaceProxy(const std::string & requiredInterfaceProxyName)
{
    if (!RequiredInterfaces.FindItem(requiredInterfaceProxyName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveRequiredInterfaceProxy: cannot find required interface proxy: " << requiredInterfaceProxyName << std::endl;
        return false;
    }

    // Get a pointer to the provided interface proxy
    mtsRequiredInterface * requiredInterfaceProxy = RequiredInterfaces.GetItem(requiredInterfaceProxyName);
    if (!requiredInterfaceProxy) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveRequiredInterfaceProxy: This should not happen" << std::endl;
        return false;
    }

    // Remove the provided interface proxy from map
    if (!RequiredInterfaces.RemoveItem(requiredInterfaceProxyName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveRequiredInterfaceProxy: cannot remove required interface proxy: " << requiredInterfaceProxyName << std::endl;
        return false;
    }

    delete requiredInterfaceProxy;

    CMN_LOG_CLASS_RUN_VERBOSE << "RemoveRequiredInterfaceProxy: removed required interface proxy: " << requiredInterfaceProxyName << std::endl;
    return true;
}

//-----------------------------------------------------------------------------
//  Methods for Client Components
//-----------------------------------------------------------------------------
bool mtsComponentProxy::CreateProvidedInterfaceProxy(const ProvidedInterfaceDescription & providedInterfaceDescription)
{
    const std::string providedInterfaceName = providedInterfaceDescription.ProvidedInterfaceName;

    // Create a local provided interface (a provided interface proxy) but it
    // is not immediately added to the component. It can be added only after
    // all proxy objects (command proxies and event proxies) are confirmed to
    // be successfully created.
    mtsProvidedInterface * providedInterfaceProxy = new mtsDeviceInterface(providedInterfaceName, this);

    // Create command proxies according to the information about the original
    // provided interface. CommandId is initially set to zero and will be
    // updated later by GetCommandId().

    // Because argument prototypes in the interface description was serialized,
    // they should be deserialized in order to be used to recover orignial argument
    // prototypes.
    std::string commandName;
    mtsGenericObject * argumentPrototype = NULL,
                     * argument1Prototype = NULL,
                     * argument2Prototype = NULL;

    std::stringstream streamBuffer;
    cmnDeSerializer deserializer(streamBuffer);

    // Create void command proxies
    mtsCommandVoidProxy * newCommandVoid = NULL;
    CommandVoidVector::const_iterator itVoid = providedInterfaceDescription.CommandsVoid.begin();
    const CommandVoidVector::const_iterator itVoidEnd = providedInterfaceDescription.CommandsVoid.end();
    for (; itVoid != itVoidEnd; ++itVoid) {
        commandName = itVoid->Name;
        newCommandVoid = new mtsCommandVoidProxy(commandName);
        if (!providedInterfaceProxy->GetCommandVoidMap().AddItem(commandName, newCommandVoid)) {
            delete newCommandVoid;
            delete providedInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: failed to add void command proxy: " << commandName << std::endl;
            return false;
        }
    }

    // Create write command proxies
    mtsCommandWriteProxy * newCommandWrite = NULL;
    CommandWriteVector::const_iterator itWrite = providedInterfaceDescription.CommandsWrite.begin();
    const CommandWriteVector::const_iterator itWriteEnd = providedInterfaceDescription.CommandsWrite.end();
    for (; itWrite != itWriteEnd; ++itWrite) {
        commandName = itWrite->Name;
        newCommandWrite = new mtsCommandWriteProxy(commandName);
        if (!providedInterfaceProxy->GetCommandWriteMap().AddItem(commandName, newCommandWrite)) {
            delete newCommandWrite;
            delete providedInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: failed to add write command proxy: " << commandName << std::endl;
            return false;
        }

        // argument deserialization
        streamBuffer.str("");
        streamBuffer << itWrite->ArgumentPrototypeSerialized;
        try {
            argumentPrototype = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        } catch (std::exception e) {
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: write command argument deserialization failed: " << e.what() << std::endl;
            argumentPrototype = NULL;
        }

        if (!argumentPrototype) {
            delete providedInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: failed to create write command proxy: " << commandName << std::endl;
            return false;
        }
        newCommandWrite->SetArgumentPrototype(argumentPrototype);
    }

    // Create read command proxies
    mtsCommandReadProxy * newCommandRead = NULL;
    CommandReadVector::const_iterator itRead = providedInterfaceDescription.CommandsRead.begin();
    const CommandReadVector::const_iterator itReadEnd = providedInterfaceDescription.CommandsRead.end();
    for (; itRead != itReadEnd; ++itRead) {
        commandName = itRead->Name;
        newCommandRead = new mtsCommandReadProxy(commandName);
        if (!providedInterfaceProxy->GetCommandReadMap().AddItem(commandName, newCommandRead)) {
            delete newCommandRead;
            delete providedInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: failed to add read command proxy: " << commandName << std::endl;
            return false;
        }

        // argument deserialization
        streamBuffer.str("");
        streamBuffer << itRead->ArgumentPrototypeSerialized;
        try {
            argumentPrototype = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        } catch (std::exception e) {
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: read command argument deserialization failed: " << e.what() << std::endl;
            argumentPrototype = NULL;
        }

        if (!argumentPrototype) {
            delete providedInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: failed to create read command proxy: " << commandName << std::endl;
            return false;
        }
        newCommandRead->SetArgumentPrototype(argumentPrototype);
    }

    // Create qualified read command proxies
    mtsCommandQualifiedReadProxy * newCommandQualifiedRead = NULL;
    CommandQualifiedReadVector::const_iterator itQualifiedRead = providedInterfaceDescription.CommandsQualifiedRead.begin();
    const CommandQualifiedReadVector::const_iterator itQualifiedReadEnd = providedInterfaceDescription.CommandsQualifiedRead.end();
    for (; itQualifiedRead != itQualifiedReadEnd; ++itQualifiedRead) {
        commandName = itQualifiedRead->Name;
        newCommandQualifiedRead = new mtsCommandQualifiedReadProxy(commandName);
        if (!providedInterfaceProxy->GetCommandQualifiedReadMap().AddItem(commandName, newCommandQualifiedRead)) {
            delete newCommandQualifiedRead;
            delete providedInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: failed to add qualified read command proxy: " << commandName << std::endl;
            return false;
        }

        // argument1 deserialization
        streamBuffer.str("");
        streamBuffer << itQualifiedRead->Argument1PrototypeSerialized;
        try {
            argument1Prototype = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        } catch (std::exception e) {
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: qualified read command argument 1 deserialization failed: " << e.what() << std::endl;
            argument1Prototype = NULL;
        }
        // argument2 deserialization
        streamBuffer.str("");
        streamBuffer << itQualifiedRead->Argument2PrototypeSerialized;
        try {
            argument2Prototype = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        } catch (std::exception e) {
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: qualified read command argument 2 deserialization failed: " << e.what() << std::endl;
            argument2Prototype = NULL;
        }

        if (!argument1Prototype || !argument2Prototype) {
            delete providedInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: failed to create qualified read command proxy: " << commandName << std::endl;
            return false;
        }
        newCommandQualifiedRead->SetArgumentPrototype(argument1Prototype, argument2Prototype);
    }

    // Create event generator proxies
    std::string eventName;

    FunctionProxyAndEventHandlerProxyMapElement * mapElement
        = FunctionProxyAndEventHandlerProxyMap.GetItem(providedInterfaceName);
    if (!mapElement) {
        mapElement = new FunctionProxyAndEventHandlerProxyMapElement;
    }

    // Create void event generator proxies
    mtsFunctionVoid * eventVoidGeneratorProxy = NULL;
    // TODO: Isn't mtsMulticastCommandVoidProxy needed?
    //mtsMulticastCommandVoidProxy eventMulticastCommandVoidProxy;
    EventVoidVector::const_iterator itEventVoid = providedInterfaceDescription.EventsVoid.begin();
    const EventVoidVector::const_iterator itEventVoidEnd = providedInterfaceDescription.EventsVoid.end();
    for (; itEventVoid != itEventVoidEnd; ++itEventVoid) {
        eventName = itEventVoid->Name;
        eventVoidGeneratorProxy = new mtsFunctionVoid();
        if (!mapElement->EventGeneratorVoidProxyMap.AddItem(eventName, eventVoidGeneratorProxy)) {
            delete eventVoidGeneratorProxy;
            delete providedInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: failed to create event generator proxy: " << eventName << std::endl;
            return false;
        }
        if (!eventVoidGeneratorProxy->Bind(providedInterfaceProxy->AddEventVoid(eventName))) {
            delete providedInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: failed to create event generator proxy: " << eventName << std::endl;
            return false;
        }
    }

    // Create write event generator proxies
    mtsFunctionWrite * eventWriteGeneratorProxy;
    mtsMulticastCommandWriteProxy * eventMulticastCommandWriteProxy;
    EventWriteVector::const_iterator itEventWrite = providedInterfaceDescription.EventsWrite.begin();
    const EventWriteVector::const_iterator itEventWriteEnd = providedInterfaceDescription.EventsWrite.end();
    for (; itEventWrite != itEventWriteEnd; ++itEventWrite) {
        eventName = itEventWrite->Name;
        eventWriteGeneratorProxy = new mtsFunctionWrite();//new mtsFunctionWriteProxy();
        if (!mapElement->EventGeneratorWriteProxyMap.AddItem(eventName, eventWriteGeneratorProxy)) {
            delete eventWriteGeneratorProxy;
            delete providedInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: failed to add event write generator proxy pointer: " << eventName << std::endl;
            return false;
        }

        eventMulticastCommandWriteProxy = new mtsMulticastCommandWriteProxy(eventName);

        // event argument deserialization
        streamBuffer.str("");
        streamBuffer << itEventWrite->ArgumentPrototypeSerialized;
        try {
            argumentPrototype = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        } catch (std::exception e) {
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: argument deserialization for event write generator failed: " << e.what() << std::endl;
            argumentPrototype = NULL;
        }
        if (!argumentPrototype) {
            delete eventMulticastCommandWriteProxy;
            delete providedInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: failed to create write event proxy: " << eventName << std::endl;
            return false;
        }
        eventMulticastCommandWriteProxy->SetArgumentPrototype(argumentPrototype);

        if (!providedInterfaceProxy->AddEvent(eventName, eventMulticastCommandWriteProxy)) {
            delete eventMulticastCommandWriteProxy;
            delete providedInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: failed to add event multicast write command proxy: " << eventName << std::endl;
            return false;
        }

        if (!eventWriteGeneratorProxy->Bind(eventMulticastCommandWriteProxy)) {
            delete eventMulticastCommandWriteProxy;
            delete providedInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: failed to bind with event multicast write command proxy: " << eventName << std::endl;
            return false;
        }
    }

    // Add the provided interface proxy to the component
    if (!ProvidedInterfaces.AddItem(providedInterfaceName, providedInterfaceProxy)) {
        CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: failed to add provided interface proxy: " << providedInterfaceName << std::endl;
        delete providedInterfaceProxy;
        return false;
    }

    CMN_LOG_CLASS_RUN_VERBOSE << "CreateProvidedInterfaceProxy: added provided interface proxy: " << providedInterfaceName << std::endl;

    return true;
}

bool mtsComponentProxy::RemoveProvidedInterfaceProxy(const std::string & providedInterfaceProxyName)
{
    // Get network objects to remove
    mtsComponentInterfaceProxyServer * serverProxy = ProvidedInterfaceNetworkProxies.GetItem(providedInterfaceProxyName);
    if (!serverProxy) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveProvidedInterfaceProxy: cannot find proxy server: " << providedInterfaceProxyName << std::endl;
        return false;
    } else {
        // Network server deactivation and resource clean up
        delete serverProxy;
        ProvidedInterfaceNetworkProxies.RemoveItem(providedInterfaceProxyName);
    }

    // Get logical objects to remove
    if (!ProvidedInterfaces.FindItem(providedInterfaceProxyName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveProvidedInterfaceProxy: cannot find provided interface proxy: " << providedInterfaceProxyName << std::endl;
        return false;
    }
    mtsProvidedInterface * providedInterfaceProxy = ProvidedInterfaces.GetItem(providedInterfaceProxyName);
    if (!providedInterfaceProxy) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveProvidedInterfaceProxy: cannot find provided interface proxy instance: " << providedInterfaceProxyName << std::endl;
        return false;
    } else {
        delete providedInterfaceProxy;
        ProvidedInterfaces.RemoveItem(providedInterfaceProxyName);
    }

    CMN_LOG_CLASS_RUN_VERBOSE << "RemoveProvidedInterfaceProxy: removed provided interface proxy: " << providedInterfaceProxyName << std::endl;

    return true;
}

bool mtsComponentProxy::CreateInterfaceProxyServer(const std::string & providedInterfaceProxyName,
                                                   std::string & endpointAccessInfo,
                                                   std::string & communicatorID)
{
    mtsManagerLocal * managerLocal = mtsManagerLocal::GetInstance();

    // Generate parameters to initialize server proxy
    std::string adapterName("ComponentInterfaceServerAdapter");
    adapterName += "_";
    adapterName += providedInterfaceProxyName;
    communicatorID = mtsComponentInterfaceProxyServer::GetInterfaceCommunicatorID();

    // Create an instance of mtsComponentInterfaceProxyServer
    mtsComponentInterfaceProxyServer * providedInterfaceProxy =
        new mtsComponentInterfaceProxyServer(adapterName, communicatorID);

    // Add it to provided interface proxy object map
    if (!ProvidedInterfaceNetworkProxies.AddItem(providedInterfaceProxyName, providedInterfaceProxy)) {
        CMN_LOG_CLASS_RUN_ERROR << "CreateInterfaceProxyServer: "
            << "Cannot register provided interface proxy: " << providedInterfaceProxyName << std::endl;
        return false;
    }

    // Run provided interface proxy (i.e., component interface proxy server)
    if (!providedInterfaceProxy->Start(this)) {
        CMN_LOG_CLASS_RUN_ERROR << "CreateInterfaceProxyServer: proxy failed to start: " << providedInterfaceProxyName << std::endl;
        return false;
    }

    providedInterfaceProxy->GetLogger()->trace(
        "mtsComponentProxy", "provided interface proxy starts: " + providedInterfaceProxyName);

    // Return this server's endpoint information
    endpointAccessInfo = providedInterfaceProxy->GetEndpointInfo();

    return true;
}

bool mtsComponentProxy::CreateInterfaceProxyClient(const std::string & requiredInterfaceProxyName,
                                                   const std::string & serverEndpointInfo,
                                                   const std::string & communicatorID,
                                                   const unsigned int providedInterfaceProxyInstanceID)
{
    // Create an instance of mtsComponentInterfaceProxyClient
    mtsComponentInterfaceProxyClient * requiredInterfaceProxy =
        new mtsComponentInterfaceProxyClient(serverEndpointInfo, providedInterfaceProxyInstanceID);

    // Add it to required interface proxy object map
    if (!RequiredInterfaceNetworkProxies.AddItem(requiredInterfaceProxyName, requiredInterfaceProxy)) {
        CMN_LOG_CLASS_RUN_ERROR << "CreateInterfaceProxyClient: "
            << "cannot register required interface proxy: " << requiredInterfaceProxyName << std::endl;
        return false;
    }

    // Run required interface proxy (i.e., component interface proxy client)
    if (!requiredInterfaceProxy->Start(this)) {
        CMN_LOG_CLASS_RUN_ERROR << "CreateInterfaceProxyClient: proxy failed to start: " << requiredInterfaceProxyName << std::endl;
        return false;
    }

    requiredInterfaceProxy->GetLogger()->trace(
        "mtsComponentProxy", "required interface proxy starts: " + requiredInterfaceProxyName);

    return true;
}

bool mtsComponentProxy::IsActiveProxy(const std::string & proxyName, const bool isProxyServer) const
{
    if (isProxyServer) {
        mtsComponentInterfaceProxyServer * providedInterfaceProxy = ProvidedInterfaceNetworkProxies.GetItem(proxyName);
        if (!providedInterfaceProxy) {
            CMN_LOG_CLASS_RUN_ERROR << "IsActiveProxy: Cannot find provided interface proxy: " << proxyName << std::endl;
            return false;
        }
        return providedInterfaceProxy->IsActiveProxy();
    } else {
        mtsComponentInterfaceProxyClient * requiredInterfaceProxy = RequiredInterfaceNetworkProxies.GetItem(proxyName);
        if (!requiredInterfaceProxy) {
            CMN_LOG_CLASS_RUN_ERROR << "IsActiveProxy: Cannot find required interface proxy: " << proxyName << std::endl;
            return false;
        }
        return requiredInterfaceProxy->IsActiveProxy();
    }
}

bool mtsComponentProxy::UpdateEventHandlerProxyID(const std::string & clientComponentName, const std::string & requiredInterfaceName)
{
    // Note that this method is only called by a server process.

    // Get network proxy client connected to the required interface proxy
    // of which name is 'requiredInterfaceName.'
    mtsComponentInterfaceProxyClient * interfaceProxyClient = RequiredInterfaceNetworkProxies.GetItem(requiredInterfaceName);
    if (!interfaceProxyClient) {
        CMN_LOG_CLASS_RUN_ERROR << "UpdateEventHandlerProxyID: no interface proxy client found: " << requiredInterfaceName << std::endl;
        return false;
    }

    // Fetch pointers of event generator proxies from the connected provided
    // interface proxy at the client side.
    mtsComponentInterfaceProxy::EventGeneratorProxyPointerSet eventGeneratorProxyPointers;
    if (!interfaceProxyClient->SendFetchEventGeneratorProxyPointers(clientComponentName, requiredInterfaceName, eventGeneratorProxyPointers))
    {
        CMN_LOG_CLASS_RUN_ERROR << "UpdateEventHandlerProxyID: failed to fetch event generator proxy pointers: " << requiredInterfaceName << std::endl;
        return false;
    }

    mtsComponentInterfaceProxy::EventGeneratorProxySequence::const_iterator it;
    mtsComponentInterfaceProxy::EventGeneratorProxySequence::const_iterator itEnd;

    mtsRequiredInterface * requiredInterface = GetRequiredInterface(requiredInterfaceName);
    if (!requiredInterface) {
        CMN_LOG_CLASS_RUN_ERROR << "UpdateEventHandlerProxyID: no required interface found: " << requiredInterfaceName << std::endl;
        return false;
    }

    mtsCommandVoidProxy * eventHandlerVoid;
    it = eventGeneratorProxyPointers.EventGeneratorVoidProxies.begin();
    itEnd = eventGeneratorProxyPointers.EventGeneratorVoidProxies.end();
    for (; it != itEnd; ++it) {
        // Get event handler proxy of which id is current zero and which is disabled
        eventHandlerVoid = dynamic_cast<mtsCommandVoidProxy*>(requiredInterface->EventHandlersVoid.GetItem(it->Name));
        if (!eventHandlerVoid) {
            CMN_LOG_CLASS_RUN_ERROR << "UpdateEventHandlerProxyID: cannot find event void handler proxy: " << it->Name << std::endl;
            return false;
        }
        // Set client ID and network proxy. Note that SetNetworkProxy() should
        // be called before SetCommandID().
        if (!eventHandlerVoid->SetNetworkProxy(interfaceProxyClient)) {
            CMN_LOG_CLASS_RUN_ERROR << "UpdateEventHandlerProxyID:: failed to set network proxy: " << it->Name << std::endl;
            return false;
        }
        eventHandlerVoid->SetCommandID(it->EventGeneratorProxyId);
        eventHandlerVoid->Enable();
    }

    mtsCommandWriteProxy * eventHandlerWrite;
    it = eventGeneratorProxyPointers.EventGeneratorWriteProxies.begin();
    itEnd = eventGeneratorProxyPointers.EventGeneratorWriteProxies.end();
    for (; it != itEnd; ++it) {
        // Get event handler proxy which is disabled and of which id is current zero
        eventHandlerWrite = dynamic_cast<mtsCommandWriteProxy*>(requiredInterface->EventHandlersWrite.GetItem(it->Name));
        if (!eventHandlerWrite) {
            CMN_LOG_CLASS_RUN_ERROR << "UpdateEventHandlerProxyID: cannot find event Write handler proxy: " << it->Name << std::endl;
            return false;
        }
        // Set client ID and network proxy. Note that SetNetworkProxy() should
        // be called before SetCommandID().
        if (!eventHandlerWrite->SetNetworkProxy(interfaceProxyClient)) {
            CMN_LOG_CLASS_RUN_ERROR << "UpdateEventHandlerProxyID:: failed to set network proxy: " << it->Name << std::endl;
            return false;
        }
        eventHandlerWrite->SetCommandID(it->EventGeneratorProxyId);
        eventHandlerWrite->Enable();
    }

    return true;
}

bool mtsComponentProxy::UpdateCommandProxyID(
    const std::string & serverProvidedInterfaceName, const std::string & clientComponentName,
    const std::string & clientRequiredInterfaceName, const unsigned int providedInterfaceProxyInstanceID)
{
    const unsigned int clientID = providedInterfaceProxyInstanceID;

    // Note that this method is only called by a client process.

    // Get a network proxy server that corresponds to 'serverProvidedInterfaceName'
    mtsComponentInterfaceProxyServer * interfaceProxyServer =
        ProvidedInterfaceNetworkProxies.GetItem(serverProvidedInterfaceName);
    if (!interfaceProxyServer) {
        CMN_LOG_CLASS_RUN_ERROR << "UpdateCommandProxyID: no interface proxy server found: " << serverProvidedInterfaceName << std::endl;
        return false;
    }

    // Fetch function proxy pointers from a connected required interface proxy
    // at server side, which will be used to set command proxies' IDs.
    mtsComponentInterfaceProxy::FunctionProxyPointerSet functionProxyPointers;
    if (!interfaceProxyServer->SendFetchFunctionProxyPointers(
            providedInterfaceProxyInstanceID, clientRequiredInterfaceName, functionProxyPointers))
    {
        CMN_LOG_CLASS_RUN_ERROR << "UpdateCommandProxyID: failed to fetch function proxy pointers: "
            << clientRequiredInterfaceName << " @ " << providedInterfaceProxyInstanceID << std::endl;
        return false;
    }

    // Get a provided interface proxy instance of which command proxies are updated.
    ProvidedInterfaceProxyInstanceMapType::const_iterator it =
        ProvidedInterfaceProxyInstanceMap.find(providedInterfaceProxyInstanceID);
    if (it == ProvidedInterfaceProxyInstanceMap.end()) {
        CMN_LOG_CLASS_RUN_ERROR << "UpdateCommandProxyID: failed to fetch provided interface proxy instance: "
            << providedInterfaceProxyInstanceID << std::endl;
        return false;
    }
    mtsProvidedInterface * instance = it->second;

    // Set command proxy IDs in the provided interface proxy instance as the
    // function proxies' pointers fetched from server process.

    // Void command
    mtsCommandVoidProxy * commandVoid = NULL;
    mtsComponentInterfaceProxy::FunctionProxySequence::const_iterator itVoid = functionProxyPointers.FunctionVoidProxies.begin();
    const mtsComponentInterfaceProxy::FunctionProxySequence::const_iterator itVoidEnd= functionProxyPointers.FunctionVoidProxies.end();
    for (; itVoid != itVoidEnd; ++itVoid) {
        commandVoid = dynamic_cast<mtsCommandVoidProxy*>(instance->GetCommandVoid(itVoid->Name));
        if (!commandVoid) {
            CMN_LOG_CLASS_RUN_ERROR << "UpdateCommandProxyID:: failed to update command void proxy id: " << itVoid->Name << std::endl;
            return false;
        }
        // Set client ID and network proxy. Note that SetNetworkProxy() should
        // be called before SetCommandID().
        if (!commandVoid->SetNetworkProxy(interfaceProxyServer, clientID)) {
            CMN_LOG_CLASS_RUN_ERROR << "UpdateCommandProxyID:: failed to set network proxy: " << itVoid->Name << std::endl;
            return false;
        }
        // Set command void proxy's id and enable this command
        commandVoid->SetCommandID(itVoid->FunctionProxyId);
        commandVoid->Enable();
    }

    // Write command
    mtsCommandWriteProxy * commandWrite = NULL;
    mtsComponentInterfaceProxy::FunctionProxySequence::const_iterator itWrite = functionProxyPointers.FunctionWriteProxies.begin();
    const mtsComponentInterfaceProxy::FunctionProxySequence::const_iterator itWriteEnd = functionProxyPointers.FunctionWriteProxies.end();
    for (; itWrite != itWriteEnd; ++itWrite) {
        commandWrite = dynamic_cast<mtsCommandWriteProxy*>(instance->GetCommandWrite(itWrite->Name));
        if (!commandWrite) {
            CMN_LOG_CLASS_RUN_ERROR << "UpdateCommandProxyID:: failed to update command write proxy id: " << itWrite->Name << std::endl;
            return false;
        }
        // Set client ID and network proxy
        if (!commandWrite->SetNetworkProxy(interfaceProxyServer, clientID)) {
            CMN_LOG_CLASS_RUN_ERROR << "UpdateCommandProxyID:: failed to set network proxy: " << itWrite->Name << std::endl;
            return false;
        }
        // Set command write proxy's id and enable this command
        commandWrite->SetCommandID(itWrite->FunctionProxyId);
        commandWrite->Enable();
    }

    // Read command
    mtsCommandReadProxy * commandRead = NULL;
    mtsComponentInterfaceProxy::FunctionProxySequence::const_iterator itRead = functionProxyPointers.FunctionReadProxies.begin();
    const mtsComponentInterfaceProxy::FunctionProxySequence::const_iterator itReadEnd = functionProxyPointers.FunctionReadProxies.end();
    for (; itRead != itReadEnd; ++itRead) {
        commandRead = dynamic_cast<mtsCommandReadProxy*>(instance->GetCommandRead(itRead->Name));
        if (!commandRead) {
            CMN_LOG_CLASS_RUN_ERROR << "UpdateCommandProxyID:: failed to update command read proxy id: " << itRead->Name << std::endl;
            return false;
        }
        // Set client ID and network proxy
        if (!commandRead->SetNetworkProxy(interfaceProxyServer, clientID)) {
            CMN_LOG_CLASS_RUN_ERROR << "UpdateCommandProxyID:: failed to set network proxy: " << itRead->Name << std::endl;
            return false;
        }
        // Set command read proxy's id and enable this command
        commandRead->SetCommandID(itRead->FunctionProxyId);
        commandRead->Enable();
    }

    // QualifiedRead command
    mtsCommandQualifiedReadProxy * commandQualifiedRead = NULL;
    mtsComponentInterfaceProxy::FunctionProxySequence::const_iterator itQualifiedRead = functionProxyPointers.FunctionQualifiedReadProxies.begin();
    const mtsComponentInterfaceProxy::FunctionProxySequence::const_iterator itQualifiedReadEnd = functionProxyPointers.FunctionQualifiedReadProxies.end();
    for (; itQualifiedRead != itQualifiedReadEnd; ++itQualifiedRead) {
        commandQualifiedRead = dynamic_cast<mtsCommandQualifiedReadProxy*>(instance->GetCommandQualifiedRead(itQualifiedRead->Name));
        if (!commandQualifiedRead) {
            CMN_LOG_CLASS_RUN_ERROR << "UpdateCommandProxyID:: failed to update command qualifiedRead proxy id: " << itQualifiedRead->Name << std::endl;
            return false;
        }
        // Set client ID and network proxy
        if (!commandQualifiedRead->SetNetworkProxy(interfaceProxyServer, clientID)) {
            CMN_LOG_CLASS_RUN_ERROR << "UpdateCommandProxyID:: failed to set network proxy: " << itQualifiedRead->Name << std::endl;
            return false;
        }
        // Set command qualified read proxy's id and enable this command
        commandQualifiedRead->SetCommandID(itQualifiedRead->FunctionProxyId);
        commandQualifiedRead->Enable();
    }

    return true;
}

mtsProvidedInterface * mtsComponentProxy::CreateProvidedInterfaceInstance(
    const mtsProvidedInterface * providedInterfaceProxy, unsigned int & instanceID)
{
    // Create a new instance of provided interface proxy
    mtsProvidedInterface * providedInterfaceInstance = new mtsProvidedInterface;

    // Clone command object proxies
    mtsDeviceInterface::CommandVoidMapType::const_iterator itVoidBegin =
        providedInterfaceProxy->CommandsVoid.begin();
    mtsDeviceInterface::CommandVoidMapType::const_iterator itVoidEnd =
        providedInterfaceProxy->CommandsVoid.end();
    providedInterfaceInstance->CommandsVoid.GetMap().insert(itVoidBegin, itVoidEnd);

    mtsDeviceInterface::CommandWriteMapType::const_iterator itWriteBegin =
        providedInterfaceProxy->CommandsWrite.begin();
    mtsDeviceInterface::CommandWriteMapType::const_iterator itWriteEnd =
        providedInterfaceProxy->CommandsWrite.end();
    providedInterfaceInstance->CommandsWrite.GetMap().insert(itWriteBegin, itWriteEnd);

    mtsDeviceInterface::CommandReadMapType::const_iterator itReadBegin =
        providedInterfaceProxy->CommandsRead.begin();
    mtsDeviceInterface::CommandReadMapType::const_iterator itReadEnd =
        providedInterfaceProxy->CommandsRead.end();
    providedInterfaceInstance->CommandsRead.GetMap().insert(itReadBegin, itReadEnd);

    mtsDeviceInterface::CommandQualifiedReadMapType::const_iterator itQualifiedReadBegin =
        providedInterfaceProxy->CommandsQualifiedRead.begin();
    mtsDeviceInterface::CommandQualifiedReadMapType::const_iterator itQualifiedReadEnd =
        providedInterfaceProxy->CommandsQualifiedRead.end();
    providedInterfaceInstance->CommandsQualifiedRead.GetMap().insert(itQualifiedReadBegin, itQualifiedReadEnd);

    mtsDeviceInterface::EventVoidMapType::const_iterator itEventVoidGeneratorBegin =
        providedInterfaceProxy->EventVoidGenerators.begin();
    mtsDeviceInterface::EventVoidMapType::const_iterator itEventVoidGeneratorEnd =
        providedInterfaceProxy->EventVoidGenerators.end();
    providedInterfaceInstance->EventVoidGenerators.GetMap().insert(itEventVoidGeneratorBegin, itEventVoidGeneratorEnd);

    mtsDeviceInterface::EventWriteMapType::const_iterator itEventWriteGeneratorBegin =
        providedInterfaceProxy->EventWriteGenerators.begin();
    mtsDeviceInterface::EventWriteMapType::const_iterator itEventWriteGeneratorEnd =
        providedInterfaceProxy->EventWriteGenerators.end();
    providedInterfaceInstance->EventWriteGenerators.GetMap().insert(itEventWriteGeneratorBegin, itEventWriteGeneratorEnd);

    // Assign a new provided interface proxy instance id
    instanceID = ++ProvidedInterfaceProxyInstanceID;
    ProvidedInterfaceProxyInstanceMap.insert(std::make_pair(instanceID, providedInterfaceInstance));

    return providedInterfaceInstance;
}

bool mtsComponentProxy::GetFunctionProxyPointers(const std::string & requiredInterfaceName,
    mtsComponentInterfaceProxy::FunctionProxyPointerSet & functionProxyPointers)
{
    // Get required interface proxy
    mtsRequiredInterface * requiredInterfaceProxy = GetRequiredInterface(requiredInterfaceName);
    if (!requiredInterfaceProxy) {
        CMN_LOG_CLASS_RUN_ERROR << "GetFunctionProxyPointers: failed to get required interface proxy: " << requiredInterfaceName << std::endl;
        return false;
    }

    // Get function proxy and event handler proxy map element
    FunctionProxyAndEventHandlerProxyMapElement * mapElement = FunctionProxyAndEventHandlerProxyMap.GetItem(requiredInterfaceName);
    if (!mapElement) {
        CMN_LOG_CLASS_RUN_ERROR << "GetFunctionProxyPointers: failed to get proxy map element: " << requiredInterfaceName << std::endl;
        return false;
    }

    mtsComponentInterfaceProxy::FunctionProxyInfo function;

    // Void function proxy
    FunctionVoidProxyMapType::const_iterator itVoid = mapElement->FunctionVoidProxyMap.begin();
    const FunctionVoidProxyMapType::const_iterator itVoidEnd = mapElement->FunctionVoidProxyMap.end();
    for (; itVoid != itVoidEnd; ++itVoid) {
        function.Name = itVoid->first;
        function.FunctionProxyId = reinterpret_cast<CommandIDType>(itVoid->second);
        functionProxyPointers.FunctionVoidProxies.push_back(function);
    }

    // Write function proxy
    FunctionWriteProxyMapType::const_iterator itWrite = mapElement->FunctionWriteProxyMap.begin();
    const FunctionWriteProxyMapType::const_iterator itWriteEnd = mapElement->FunctionWriteProxyMap.end();
    for (; itWrite != itWriteEnd; ++itWrite) {
        function.Name = itWrite->first;
        function.FunctionProxyId = reinterpret_cast<CommandIDType>(itWrite->second);
        functionProxyPointers.FunctionWriteProxies.push_back(function);
    }

    // Read function proxy
    FunctionReadProxyMapType::const_iterator itRead = mapElement->FunctionReadProxyMap.begin();
    const FunctionReadProxyMapType::const_iterator itReadEnd = mapElement->FunctionReadProxyMap.end();
    for (; itRead != itReadEnd; ++itRead) {
        function.Name = itRead->first;
        function.FunctionProxyId = reinterpret_cast<CommandIDType>(itRead->second);
        functionProxyPointers.FunctionReadProxies.push_back(function);
    }

    // QualifiedRead function proxy
    FunctionQualifiedReadProxyMapType::const_iterator itQualifiedRead = mapElement->FunctionQualifiedReadProxyMap.begin();
    const FunctionQualifiedReadProxyMapType::const_iterator itQualifiedReadEnd = mapElement->FunctionQualifiedReadProxyMap.end();
    for (; itQualifiedRead != itQualifiedReadEnd; ++itQualifiedRead) {
        function.Name = itQualifiedRead->first;
        function.FunctionProxyId = reinterpret_cast<CommandIDType>(itQualifiedRead->second);
        functionProxyPointers.FunctionQualifiedReadProxies.push_back(function);
    }

    return true;
}

bool mtsComponentProxy::GetEventGeneratorProxyPointer(
    const std::string & clientComponentName, const std::string & requiredInterfaceName,
    mtsComponentInterfaceProxy::EventGeneratorProxyPointerSet & eventGeneratorProxyPointers)
{
    mtsManagerLocal * localManager = mtsManagerLocal::GetInstance();
    mtsDevice * clientComponent = localManager->GetComponent(clientComponentName);
    if (!clientComponent) {
        CMN_LOG_CLASS_RUN_ERROR << "GetEventGeneratorProxyPointer: no client component found: " << clientComponentName << std::endl;
        return false;
    }

    mtsRequiredInterface * requiredInterface = clientComponent->GetRequiredInterface(requiredInterfaceName);
    if (!requiredInterface) {
        CMN_LOG_CLASS_RUN_ERROR << "GetEventGeneratorProxyPointer: no required interface found: " << requiredInterfaceName << std::endl;
        return false;
    }
    mtsProvidedInterface * providedInterface = requiredInterface->GetConnectedInterface();
    if (!providedInterface) {
        CMN_LOG_CLASS_RUN_ERROR << "GetEventGeneratorProxyPointer: no connected provided interface found" << std::endl;
        return false;
    }

    mtsComponentInterfaceProxy::EventGeneratorProxyElement element;
    mtsCommandBase * eventGenerator = NULL;

    std::vector<std::string> namesOfEventHandlersVoid = requiredInterface->GetNamesOfEventHandlersVoid();
    for (unsigned int i = 0; i < namesOfEventHandlersVoid.size(); ++i) {
        element.Name = namesOfEventHandlersVoid[i];
        eventGenerator = providedInterface->EventVoidGenerators.GetItem(element.Name);
        if (!eventGenerator) {
            CMN_LOG_CLASS_RUN_ERROR << "GetEventGeneratorProxyPointer: no event void generator found: " << element.Name << std::endl;
            return false;
        }
        element.EventGeneratorProxyId = reinterpret_cast<CommandIDType>(eventGenerator);
        eventGeneratorProxyPointers.EventGeneratorVoidProxies.push_back(element);
    }

    std::vector<std::string> namesOfEventHandlersWrite = requiredInterface->GetNamesOfEventHandlersWrite();
    for (unsigned int i = 0; i < namesOfEventHandlersWrite.size(); ++i) {
        element.Name = namesOfEventHandlersWrite[i];
        eventGenerator = providedInterface->EventWriteGenerators.GetItem(element.Name);
        if (!eventGenerator) {
            CMN_LOG_CLASS_RUN_ERROR << "GetEventGeneratorProxyPointer: no event write generator found: " << element.Name << std::endl;
            return false;
        }
        element.EventGeneratorProxyId = reinterpret_cast<CommandIDType>(eventGenerator);
        eventGeneratorProxyPointers.EventGeneratorWriteProxies.push_back(element);
    }

    return true;
}

//-------------------------------------------------------------------------
//  Utilities
//-------------------------------------------------------------------------
void mtsComponentProxy::ExtractProvidedInterfaceDescription(
    mtsDeviceInterface * providedInterface, ProvidedInterfaceDescription & providedInterfaceDescription)
{
    if (!providedInterface) return;

    // Serializer initialization
    std::stringstream streamBuffer;
    cmnSerializer serializer(streamBuffer);

    // Extract void commands
    CommandVoidElement elementCommandVoid;
    mtsDeviceInterface::CommandVoidMapType::MapType::const_iterator itVoid = providedInterface->CommandsVoid.begin();
    const mtsDeviceInterface::CommandVoidMapType::MapType::const_iterator itVoidEnd = providedInterface->CommandsVoid.end();
    for (; itVoid != itVoidEnd; ++itVoid) {
        elementCommandVoid.Name = itVoid->second->GetName();
        providedInterfaceDescription.CommandsVoid.push_back(elementCommandVoid);
    }

    // Extract write commands
    CommandWriteElement elementCommandWrite;
    mtsDeviceInterface::CommandWriteMapType::MapType::const_iterator itWrite = providedInterface->CommandsWrite.begin();
    const mtsDeviceInterface::CommandWriteMapType::MapType::const_iterator itWriteEnd = providedInterface->CommandsWrite.end();
    for (; itWrite != itWriteEnd; ++itWrite) {
        elementCommandWrite.Name = itWrite->second->GetName();
        // argument serialization
        streamBuffer.str("");
        serializer.Serialize(*(itWrite->second->GetArgumentPrototype()));
        elementCommandWrite.ArgumentPrototypeSerialized = streamBuffer.str();
        providedInterfaceDescription.CommandsWrite.push_back(elementCommandWrite);
    }

    // Extract read commands
    CommandReadElement elementCommandRead;
    mtsDeviceInterface::CommandReadMapType::MapType::const_iterator itRead = providedInterface->CommandsRead.begin();
    const mtsDeviceInterface::CommandReadMapType::MapType::const_iterator itReadEnd = providedInterface->CommandsRead.end();
    for (; itRead != itReadEnd; ++itRead) {
        elementCommandRead.Name = itRead->second->GetName();
        // argument serialization
        streamBuffer.str("");
        serializer.Serialize(*(itRead->second->GetArgumentPrototype()));
        elementCommandRead.ArgumentPrototypeSerialized = streamBuffer.str();
        providedInterfaceDescription.CommandsRead.push_back(elementCommandRead);
    }

    // Extract qualified read commands
    CommandQualifiedReadElement elementCommandQualifiedRead;
    mtsDeviceInterface::CommandQualifiedReadMapType::MapType::const_iterator itQualifiedRead = providedInterface->CommandsQualifiedRead.begin();
    const mtsDeviceInterface::CommandQualifiedReadMapType::MapType::const_iterator itQualifiedReadEnd = providedInterface->CommandsQualifiedRead.end();
    for (; itQualifiedRead != itQualifiedReadEnd; ++itQualifiedRead) {
        elementCommandQualifiedRead.Name = itQualifiedRead->second->GetName();
        // argument1 serialization
        streamBuffer.str("");
        serializer.Serialize(*(itQualifiedRead->second->GetArgument1Prototype()));
        elementCommandQualifiedRead.Argument1PrototypeSerialized = streamBuffer.str();
        // argument2 serialization
        streamBuffer.str("");
        serializer.Serialize(*(itQualifiedRead->second->GetArgument2Prototype()));
        elementCommandQualifiedRead.Argument2PrototypeSerialized = streamBuffer.str();
        providedInterfaceDescription.CommandsQualifiedRead.push_back(elementCommandQualifiedRead);
    }

    // Extract void events
    EventVoidElement elementEventVoid;
    mtsDeviceInterface::EventVoidMapType::MapType::const_iterator itEventVoid = providedInterface->EventVoidGenerators.begin();
    const mtsDeviceInterface::EventVoidMapType::MapType::const_iterator itEventVoidEnd = providedInterface->EventVoidGenerators.end();
    for (; itEventVoid != itEventVoidEnd; ++itEventVoid) {
        elementEventVoid.Name = itEventVoid->second->GetName();
        providedInterfaceDescription.EventsVoid.push_back(elementEventVoid);
    }

    // Extract write events
    EventWriteElement elementEventWrite;
    mtsDeviceInterface::EventWriteMapType::MapType::const_iterator itEventWrite = providedInterface->EventWriteGenerators.begin();
    const mtsDeviceInterface::EventWriteMapType::MapType::const_iterator itEventWriteEnd = providedInterface->EventWriteGenerators.end();
    for (; itEventWrite != itEventWriteEnd; ++itEventWrite) {
        elementEventWrite.Name = itEventWrite->second->GetName();
        // argument serialization
        streamBuffer.str("");
        serializer.Serialize(*(itEventWrite->second->GetArgumentPrototype()));
        elementEventWrite.ArgumentPrototypeSerialized = streamBuffer.str();
        providedInterfaceDescription.EventsWrite.push_back(elementEventWrite);
    }
}

void mtsComponentProxy::ExtractRequiredInterfaceDescription(
    mtsRequiredInterface * requiredInterface, RequiredInterfaceDescription & requiredInterfaceDescription)
{
    // Serializer initialization
    std::stringstream streamBuffer;
    cmnSerializer serializer(streamBuffer);

    // Extract void functions
    requiredInterfaceDescription.FunctionVoidNames = requiredInterface->GetNamesOfCommandPointersVoid();
    // Extract write functions
    requiredInterfaceDescription.FunctionWriteNames = requiredInterface->GetNamesOfCommandPointersWrite();
    // Extract read functions
    requiredInterfaceDescription.FunctionReadNames = requiredInterface->GetNamesOfCommandPointersRead();
    // Extract qualified read functions
    requiredInterfaceDescription.FunctionQualifiedReadNames = requiredInterface->GetNamesOfCommandPointersQualifiedRead();

    // Extract void event handlers
    CommandVoidElement elementEventVoidHandler;
    mtsRequiredInterface::EventHandlerVoidMapType::MapType::const_iterator itVoid = requiredInterface->EventHandlersVoid.begin();
    const mtsRequiredInterface::EventHandlerVoidMapType::MapType::const_iterator itVoidEnd = requiredInterface->EventHandlersVoid.end();
    for (; itVoid != itVoidEnd; ++itVoid) {
        elementEventVoidHandler.Name = itVoid->second->GetName();
        requiredInterfaceDescription.EventHandlersVoid.push_back(elementEventVoidHandler);
    }

    // Extract write event handlers
    CommandWriteElement elementEventWriteHandler;
    mtsRequiredInterface::EventHandlerWriteMapType::MapType::const_iterator itWrite = requiredInterface->EventHandlersWrite.begin();
    const mtsRequiredInterface::EventHandlerWriteMapType::MapType::const_iterator itWriteEnd = requiredInterface->EventHandlersWrite.end();
    for (; itWrite != itWriteEnd; ++itWrite) {
        elementEventWriteHandler.Name = itWrite->second->GetName();
        // argument serialization
        streamBuffer.str("");
        serializer.Serialize(*(itWrite->second->GetArgumentPrototype()));
        elementEventWriteHandler.ArgumentPrototypeSerialized = streamBuffer.str();
        requiredInterfaceDescription.EventHandlersWrite.push_back(elementEventWriteHandler);
    }
}

bool mtsComponentProxy::AddConnectionInformation(const unsigned int providedInterfaceProxyInstanceID,
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName)
{
    mtsComponentInterfaceProxyServer * interfaceProxyServer = ProvidedInterfaceNetworkProxies.GetItem(serverProvidedInterfaceName);
    if (!interfaceProxyServer) {
        CMN_LOG_CLASS_RUN_ERROR << "AddConnectionInformation: no interface proxy server found: " << serverProvidedInterfaceName << std::endl;
        return false;
    }

    return interfaceProxyServer->AddConnectionInformation(providedInterfaceProxyInstanceID,
        clientProcessName, clientComponentName, clientRequiredInterfaceName,
        serverProcessName, serverComponentName, serverProvidedInterfaceName);
}
