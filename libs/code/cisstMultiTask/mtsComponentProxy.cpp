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
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsFunctionReadProxy.h>
#include <cisstMultiTask/mtsFunctionWriteProxy.h>
#include <cisstMultiTask/mtsFunctionQualifiedReadProxy.h>

mtsComponentProxy::mtsComponentProxy(const std::string & componentProxyName)
  : mtsComponent(componentProxyName),
    InterfaceProvidedNetworkProxies("InterfaceProvidedNetworkProxies"),
    InterfaceRequiredNetworkProxies("InterfaceRequiredNetworkProxies"),
    FunctionProxyAndEventHandlerProxyMap("FunctionProxyAndEventHandlerProxyMap")
{
    InterfaceProvidedNetworkProxies.SetOwner(*this);
    InterfaceRequiredNetworkProxies.SetOwner(*this);
    FunctionProxyAndEventHandlerProxyMap.SetOwner(*this);

}

mtsComponentProxy::~mtsComponentProxy()
{
    // Clean up all internal resources

    // Stop all provided interface proxies running.
    InterfaceProvidedNetworkProxyMapType::MapType::const_iterator itProvidedProxy = InterfaceProvidedNetworkProxies.begin();
    const InterfaceProvidedNetworkProxyMapType::MapType::const_iterator itProvidedProxyEnd = InterfaceProvidedNetworkProxies.end();
    for (; itProvidedProxy != itProvidedProxyEnd; ++itProvidedProxy) {
        itProvidedProxy->second->StopProxy();
    }
    InterfaceProvidedNetworkProxies.DeleteAll();

    // Stop all required interface proxies running.
    InterfaceRequiredNetworkProxyMapType::MapType::const_iterator itRequiredProxy = InterfaceRequiredNetworkProxies.begin();
    const InterfaceRequiredNetworkProxyMapType::MapType::const_iterator itRequiredProxyEnd = InterfaceRequiredNetworkProxies.end();
    for (; itRequiredProxy != itRequiredProxyEnd; ++itRequiredProxy) {
        itRequiredProxy->second->StopProxy();
    }
    InterfaceRequiredNetworkProxies.DeleteAll();

    FunctionProxyAndEventHandlerProxyMap.DeleteAll();
}

//-----------------------------------------------------------------------------
//  Methods for Server Components
//-----------------------------------------------------------------------------
bool mtsComponentProxy::CreateInterfaceRequiredProxy(const InterfaceRequiredDescription & requiredInterfaceDescription)
{
    const std::string requiredInterfaceName = requiredInterfaceDescription.InterfaceRequiredName;
    const mtsRequiredType isRequired = (requiredInterfaceDescription.IsRequired ? MTS_REQUIRED : MTS_OPTIONAL);

    // Create a local required interface (a required interface proxy)
    mtsInterfaceRequired * requiredInterfaceProxy = AddInterfaceRequired(requiredInterfaceName, isRequired);
    if (!requiredInterfaceProxy) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceRequiredProxy: failed to add required interface proxy: " << requiredInterfaceName << std::endl;
        return false;
    }

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
    for (size_t i = 0; i < namesOfFunctionVoid.size(); ++i) {
        functionVoidProxy = new mtsFunctionVoid();
        success = requiredInterfaceProxy->AddFunction(namesOfFunctionVoid[i], *functionVoidProxy);
        success &= mapElement->FunctionVoidProxyMap.AddItem(namesOfFunctionVoid[i], functionVoidProxy);
        if (!success) {
            CMN_ASSERT(RemoveInterfaceRequired(requiredInterfaceName));
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceRequiredProxy: failed to add void function proxy: \"" << namesOfFunctionVoid[i] << "\"" << std::endl;
            return false;
        }
    }

    // Create write function proxies
    const std::vector<std::string> namesOfFunctionWrite = requiredInterfaceDescription.FunctionWriteNames;
    for (size_t i = 0; i < namesOfFunctionWrite.size(); ++i) {
        functionWriteProxy = new mtsFunctionWriteProxy();
        success = requiredInterfaceProxy->AddFunction(namesOfFunctionWrite[i], *functionWriteProxy);
        success &= mapElement->FunctionWriteProxyMap.AddItem(namesOfFunctionWrite[i], functionWriteProxy);
        if (!success) {
            CMN_ASSERT(RemoveInterfaceRequired(requiredInterfaceName));
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceRequiredProxy: failed to add write function proxy: \"" << namesOfFunctionWrite[i] << "\"" << std::endl;
            return false;
        }
    }

    // Create read function proxies
    const std::vector<std::string> namesOfFunctionRead = requiredInterfaceDescription.FunctionReadNames;
    for (size_t i = 0; i < namesOfFunctionRead.size(); ++i) {
        functionReadProxy = new mtsFunctionReadProxy();
        success = requiredInterfaceProxy->AddFunction(namesOfFunctionRead[i], *functionReadProxy);
        success &= mapElement->FunctionReadProxyMap.AddItem(namesOfFunctionRead[i], functionReadProxy);
        if (!success) {
            CMN_ASSERT(RemoveInterfaceRequired(requiredInterfaceName));
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceRequiredProxy: failed to add read function proxy: \"" << namesOfFunctionRead[i] << "\"" << std::endl;
            return false;
        }
    }

    // Create QualifiedRead function proxies
    const std::vector<std::string> namesOfFunctionQualifiedRead = requiredInterfaceDescription.FunctionQualifiedReadNames;
    for (size_t i = 0; i < namesOfFunctionQualifiedRead.size(); ++i) {
        functionQualifiedReadProxy = new mtsFunctionQualifiedReadProxy();
        success = requiredInterfaceProxy->AddFunction(namesOfFunctionQualifiedRead[i], *functionQualifiedReadProxy);
        success &= mapElement->FunctionQualifiedReadProxyMap.AddItem(namesOfFunctionQualifiedRead[i], functionQualifiedReadProxy);
        if (!success) {
            CMN_ASSERT(RemoveInterfaceRequired(requiredInterfaceName));
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceRequiredProxy: failed to add qualified read function proxy: \"" << namesOfFunctionQualifiedRead[i] << "\"" << std::endl;
            return false;
        }
    }

    // Create event handler proxies
    std::string eventName;

    // Create event handler proxies with CommandIDs set to zero and disable them
    // by default. It will be updated and enabled later by UpdateEventHandlerID(),

    // Create void event handler proxy
    mtsCommandVoidProxy * newEventVoidHandlerProxy = NULL;
    for (size_t i = 0; i < requiredInterfaceDescription.EventHandlersVoid.size(); ++i) {
        eventName = requiredInterfaceDescription.EventHandlersVoid[i].Name;
        newEventVoidHandlerProxy = new mtsCommandVoidProxy(eventName);
        if (!requiredInterfaceProxy->EventHandlersVoid.AddItem(eventName, newEventVoidHandlerProxy)) {
            delete newEventVoidHandlerProxy;
            CMN_ASSERT(RemoveInterfaceRequired(requiredInterfaceName));
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceRequiredProxy: failed to add void event handler proxy: \"" << eventName << "\"" << std::endl;
            return false;
        }
    }

    // Create write event handler proxies
    std::stringstream streamBuffer;
    cmnDeSerializer deserializer(streamBuffer);

    mtsCommandWriteProxy * newEventWriteHandlerProxy = NULL;
    mtsGenericObject * argumentPrototype = NULL;
    for (size_t i = 0; i < requiredInterfaceDescription.EventHandlersWrite.size(); ++i) {
        eventName = requiredInterfaceDescription.EventHandlersWrite[i].Name;
        newEventWriteHandlerProxy = new mtsCommandWriteProxy(eventName);
        if (!requiredInterfaceProxy->EventHandlersWrite.AddItem(eventName, newEventWriteHandlerProxy)) {
            delete newEventWriteHandlerProxy;
            CMN_ASSERT(RemoveInterfaceRequired(requiredInterfaceName));
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceRequiredProxy: failed to add write event handler proxy: \"" << eventName << "\""<< std::endl;
            return false;
        }

        // argument deserialization
        streamBuffer.str("");
        streamBuffer << requiredInterfaceDescription.EventHandlersWrite[i].ArgumentPrototypeSerialized;
        try {
            argumentPrototype = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        } catch (std::exception e) {
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceRequiredProxy: write command argument deserialization failed: " << e.what() << std::endl;
            argumentPrototype = NULL;
        }

        if (!argumentPrototype) {
            CMN_ASSERT(RemoveInterfaceRequired(requiredInterfaceName));
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceRequiredProxy: failed to create event write handler proxy: " << eventName << std::endl;
            return false;
        }
        newEventWriteHandlerProxy->SetArgumentPrototype(argumentPrototype);
    }

    // Add to function proxy and event handler proxy map
    if (!FunctionProxyAndEventHandlerProxyMap.AddItem(requiredInterfaceName, mapElement)) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceRequiredProxy: failed to add proxy map: " << requiredInterfaceName << std::endl;
        return false;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "CreateInterfaceRequiredProxy: added required interface proxy: " << requiredInterfaceName << std::endl;

    return true;
}

bool mtsComponentProxy::RemoveInterfaceRequiredProxy(const std::string & requiredInterfaceProxyName)
{
    // Get network objects to remove
    mtsComponentInterfaceProxyClient * clientProxy = InterfaceRequiredNetworkProxies.GetItem(requiredInterfaceProxyName, CMN_LOG_LEVEL_RUN_VERBOSE);
    if (!clientProxy) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveInterfaceRequiredProxy: cannot find proxy client " << requiredInterfaceProxyName 
                                 << " in proxy component " << GetName() << std::endl;
        return false;
    } else {
        // Network server deactivation and resource clean up
        clientProxy->StopProxy();
        delete clientProxy;
        InterfaceRequiredNetworkProxies.RemoveItem(requiredInterfaceProxyName);
    }

    // Remove required interface proxy
    if (!RemoveInterfaceRequired(requiredInterfaceProxyName)) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveInterfaceRequiredProxy: failed to remove required interface proxy: " << requiredInterfaceProxyName << std::endl;
        return false;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "RemoveInterfaceRequiredProxy: removed required interface proxy: " << requiredInterfaceProxyName << std::endl;

    return true;
}

//-----------------------------------------------------------------------------
//  Methods for Client Components
//-----------------------------------------------------------------------------
bool mtsComponentProxy::CreateInterfaceProvidedProxy(const InterfaceProvidedDescription & providedInterfaceDescription)
{
    const std::string providedInterfaceName = providedInterfaceDescription.InterfaceProvidedName;

    // Create a local provided interface (a provided interface proxy)
    mtsInterfaceProvided * providedInterfaceProxy = AddInterfaceProvided(providedInterfaceName);
    if (!providedInterfaceProxy) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: failed to add provided interface proxy: " << providedInterfaceName << std::endl;
        return false;
    }

    // Create command proxies according to the information about the original
    // provided interface. CommandId is initially set to zero and will be
    // updated later by GetCommandId().

    // Since argument prototypes in the interface description have been serialized,
    // they need to be deserialized.
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
        if (!providedInterfaceProxy->AddCommandVoid(newCommandVoid)) {
            delete newCommandVoid;
            CMN_ASSERT(RemoveInterfaceProvided(providedInterfaceName));
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: failed to add void command proxy: " << commandName << std::endl;
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
        if (!providedInterfaceProxy->AddCommandWrite(newCommandWrite)) {
            delete newCommandWrite;
            CMN_ASSERT(RemoveInterfaceProvided(providedInterfaceName));
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: failed to add write command proxy: " << commandName << std::endl;
            return false;
        }

        // argument deserialization
        streamBuffer.str("");
        streamBuffer << itWrite->ArgumentPrototypeSerialized;
        try {
            argumentPrototype = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        } catch (std::exception e) {
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: failed to deserialize write command argument: " << e.what() << std::endl;
            argumentPrototype = NULL;
        }

        if (!argumentPrototype) {
            CMN_ASSERT(RemoveInterfaceProvided(providedInterfaceName));
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: failed to create write command proxy: " << commandName << std::endl;
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
        if (!providedInterfaceProxy->AddCommandRead(newCommandRead)) {
            delete newCommandRead;
            CMN_ASSERT(RemoveInterfaceProvided(providedInterfaceName));
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: failed to add read command proxy: " << commandName << std::endl;
            return false;
        }

        // argument deserialization
        streamBuffer.str("");
        streamBuffer << itRead->ArgumentPrototypeSerialized;
        try {
            argumentPrototype = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        } catch (std::exception e) {
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: read command argument deserialization failed: " << e.what() << std::endl;
            argumentPrototype = NULL;
        }

        if (!argumentPrototype) {
            CMN_ASSERT(RemoveInterfaceProvided(providedInterfaceName));
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: failed to create read command proxy: " << commandName << std::endl;
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
        if (!providedInterfaceProxy->AddCommandQualifiedRead(newCommandQualifiedRead)) {
            delete newCommandQualifiedRead;
            CMN_ASSERT(RemoveInterfaceProvided(providedInterfaceName));
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: failed to add qualified read command proxy: " << commandName << std::endl;
            return false;
        }

        // argument1 deserialization
        streamBuffer.str("");
        streamBuffer << itQualifiedRead->Argument1PrototypeSerialized;
        try {
            argument1Prototype = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        } catch (std::exception e) {
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: qualified read command argument 1 deserialization failed: " << e.what() << std::endl;
            argument1Prototype = NULL;
        }
        // argument2 deserialization
        streamBuffer.str("");
        streamBuffer << itQualifiedRead->Argument2PrototypeSerialized;
        try {
            argument2Prototype = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        } catch (std::exception e) {
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: qualified read command argument 2 deserialization failed: " << e.what() << std::endl;
            argument2Prototype = NULL;
        }

        if (!argument1Prototype || !argument2Prototype) {
            CMN_ASSERT(RemoveInterfaceProvided(providedInterfaceName));
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: failed to create qualified read command proxy: " << commandName << std::endl;
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
            CMN_ASSERT(RemoveInterfaceProvided(providedInterfaceName));
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: failed to create event generator proxy: " << eventName << std::endl;
            return false;
        }
        if (!eventVoidGeneratorProxy->Bind(providedInterfaceProxy->AddEventVoid(eventName))) {
            CMN_ASSERT(RemoveInterfaceProvided(providedInterfaceName));
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: failed to create event generator proxy: " << eventName << std::endl;
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
            CMN_ASSERT(RemoveInterfaceProvided(providedInterfaceName));
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: failed to add event write generator proxy pointer: " << eventName << std::endl;
            return false;
        }

        eventMulticastCommandWriteProxy = new mtsMulticastCommandWriteProxy(eventName);

        // event argument deserialization
        streamBuffer.str("");
        streamBuffer << itEventWrite->ArgumentPrototypeSerialized;
        try {
            argumentPrototype = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        } catch (std::exception e) {
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: argument deserialization for event write generator failed: " << e.what() << std::endl;
            argumentPrototype = NULL;
        }
        if (!argumentPrototype) {
            delete eventMulticastCommandWriteProxy;
            CMN_ASSERT(RemoveInterfaceProvided(providedInterfaceName));
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: failed to create write event proxy: " << eventName << std::endl;
            return false;
        }
        eventMulticastCommandWriteProxy->SetArgumentPrototype(argumentPrototype);

        if (!providedInterfaceProxy->AddEvent(eventName, eventMulticastCommandWriteProxy)) {
            delete eventMulticastCommandWriteProxy;
            CMN_ASSERT(RemoveInterfaceProvided(providedInterfaceName));
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: failed to add event multicast write command proxy: " << eventName << std::endl;
            return false;
        }

        if (!eventWriteGeneratorProxy->Bind(eventMulticastCommandWriteProxy)) {
            delete eventMulticastCommandWriteProxy;
            CMN_ASSERT(RemoveInterfaceProvided(providedInterfaceName));
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: failed to bind with event multicast write command proxy: " << eventName << std::endl;
            return false;
        }
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "CreateInterfaceProvidedProxy: added provided interface proxy: " << providedInterfaceName << std::endl;

    return true;
}

bool mtsComponentProxy::RemoveInterfaceProvidedProxy(const std::string & providedInterfaceProxyName)
{
    // Get network objects to remove
    mtsComponentInterfaceProxyServer * serverProxy = InterfaceProvidedNetworkProxies.GetItem(providedInterfaceProxyName);
    if (!serverProxy) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveInterfaceProvidedProxy: cannot find proxy server: " << providedInterfaceProxyName << std::endl;
        return false;
    } else {
        // Network server deactivation and resource clean up
        serverProxy->StopProxy();
        delete serverProxy;
        InterfaceProvidedNetworkProxies.RemoveItem(providedInterfaceProxyName);
    }

    // Remove required interface proxy
    if (!RemoveInterfaceProvided(providedInterfaceProxyName)) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveInterfaceProvidedProxy: failed to remove provided interface proxy: " << providedInterfaceProxyName << std::endl;
        return false;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "RemoveInterfaceProvidedProxy: removed provided interface proxy: " << providedInterfaceProxyName << std::endl;

    return true;
}

bool mtsComponentProxy::CreateInterfaceProxyServer(const std::string & providedInterfaceProxyName,
                                                   std::string & endpointAccessInfo,
                                                   std::string & communicatorID)
{
    // Generate parameters to initialize server proxy
    std::string adapterName("ComponentInterfaceServerAdapter");
    adapterName += "_";
    adapterName += providedInterfaceProxyName;
    communicatorID = mtsComponentInterfaceProxyServer::GetInterfaceCommunicatorID();

    // Create an instance of mtsComponentInterfaceProxyServer
    mtsComponentInterfaceProxyServer * providedInterfaceProxy =
        new mtsComponentInterfaceProxyServer(adapterName, communicatorID);

    // Add it to provided interface proxy object map
    if (!InterfaceProvidedNetworkProxies.AddItem(providedInterfaceProxyName, providedInterfaceProxy)) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProxyServer: "
            << "Cannot register provided interface proxy: " << providedInterfaceProxyName << std::endl;
        return false;
    }

    // Run provided interface proxy (i.e., component interface proxy server)
    if (!providedInterfaceProxy->StartProxy(this)) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProxyServer: proxy failed to start: " << providedInterfaceProxyName << std::endl;
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
                                                   const ConnectionIDType connectionID)
{
    // Create an instance of mtsComponentInterfaceProxyClient
    mtsComponentInterfaceProxyClient * requiredInterfaceProxy =
        new mtsComponentInterfaceProxyClient(serverEndpointInfo, connectionID);

    // Add it to required interface proxy object map
    if (!InterfaceRequiredNetworkProxies.AddItem(requiredInterfaceProxyName, requiredInterfaceProxy)) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProxyClient: "
            << "cannot register required interface proxy: " << requiredInterfaceProxyName << std::endl;
        return false;
    }

    // Run required interface proxy (i.e., component interface proxy client)
    if (!requiredInterfaceProxy->StartProxy(this)) {
        CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProxyClient: proxy failed to start: " << requiredInterfaceProxyName << std::endl;
        return false;
    }

    requiredInterfaceProxy->GetLogger()->trace(
        "mtsComponentProxy", "required interface proxy starts: " + requiredInterfaceProxyName);

    return true;
}

bool mtsComponentProxy::IsActiveProxy(const std::string & proxyName, const bool isProxyServer) const
{
    if (isProxyServer) {
        mtsComponentInterfaceProxyServer * providedInterfaceProxy = InterfaceProvidedNetworkProxies.GetItem(proxyName);
        if (!providedInterfaceProxy) {
            CMN_LOG_CLASS_INIT_ERROR << "IsActiveProxy: Cannot find provided interface proxy: " << proxyName << std::endl;
            return false;
        }
        return providedInterfaceProxy->IsActiveProxy();
    } else {
        mtsComponentInterfaceProxyClient * requiredInterfaceProxy = InterfaceRequiredNetworkProxies.GetItem(proxyName);
        if (!requiredInterfaceProxy) {
            CMN_LOG_CLASS_INIT_ERROR << "IsActiveProxy: Cannot find required interface proxy: " << proxyName << std::endl;
            return false;
        }
        return requiredInterfaceProxy->IsActiveProxy();
    }
}

bool mtsComponentProxy::UpdateEventHandlerProxyID(const std::string & clientComponentName, const std::string & clientInterfaceRequiredName)
{
    // Get required interface
    mtsInterfaceRequired * interfaceRequiredProxy = GetInterfaceRequired(clientInterfaceRequiredName);
    if (!interfaceRequiredProxy) {
        CMN_LOG_CLASS_INIT_ERROR << "UpdateEventHandlerProxyID: no required interface found: " << clientInterfaceRequiredName << std::endl;
        return false;
    }

    // Get network proxy client connected to the required interface proxy
    // of which name is 'clientInterfaceRequiredName.'
    mtsComponentInterfaceProxyClient * interfaceProxyClient = InterfaceRequiredNetworkProxies.GetItem(clientInterfaceRequiredName);
    if (!interfaceProxyClient) {
        CMN_LOG_CLASS_INIT_ERROR << "UpdateEventHandlerProxyID: no network interface proxy client found for required interface: " 
            << clientInterfaceRequiredName << std::endl;
        return false;
    }

    // Fetch pointers of event generator proxies from the connected provided
    // interface proxy at the client side.
    mtsComponentInterfaceProxy::EventGeneratorProxyPointerSet eventGeneratorProxyPointers;
    if (!interfaceProxyClient->SendFetchEventGeneratorProxyPointers(
            clientComponentName, clientInterfaceRequiredName, eventGeneratorProxyPointers)) 
    {
        CMN_LOG_CLASS_INIT_ERROR << "UpdateEventHandlerProxyID: failed to fetch event generator proxy pointers: " 
            << clientComponentName << ":" << clientInterfaceRequiredName << std::endl;
        return false;
    }

    mtsComponentInterfaceProxy::EventGeneratorProxySequence::const_iterator it;
    mtsComponentInterfaceProxy::EventGeneratorProxySequence::const_iterator itEnd;

    // Update event void handlers
    mtsCommandVoid * eventHandlerVoidBase;
    mtsCommandVoidProxy * eventHandlerVoid;
    it = eventGeneratorProxyPointers.EventGeneratorVoidProxies.begin();
    itEnd = eventGeneratorProxyPointers.EventGeneratorVoidProxies.end();
    for (; it != itEnd; ++it) {
        // Get event handler proxy (of which id is currently zero)
        eventHandlerVoidBase = interfaceRequiredProxy->GetEventHandlerVoid(it->Name);
        if (!eventHandlerVoidBase) {
            CMN_LOG_CLASS_INIT_ERROR << "UpdateEventHandlerProxyID: no event void handler found: " << it->Name << std::endl;
            return false;
        }
        eventHandlerVoid = dynamic_cast<mtsCommandVoidProxy*>(eventHandlerVoidBase);
        CMN_ASSERT(eventHandlerVoid);

        // Set client ID and network proxy. Note that SetNetworkProxy() should
        // be called before SetCommandID().
        if (!eventHandlerVoid->SetNetworkProxy(interfaceProxyClient)) {
            CMN_LOG_CLASS_INIT_ERROR << "UpdateEventHandlerProxyID:: failed to set network proxy: " 
                << eventHandlerVoid->GetName() << std::endl;
            return false;
        }

        // Set event handler proxy's id
        eventHandlerVoid->SetCommandID(it->EventGeneratorProxyId);
        // Enable event handler (initially, an event handler is disabled when created)
        eventHandlerVoid->Enable();
    }

    // Update event write handlers
    mtsCommandWriteBase * eventHandlerWriteBase;
    mtsCommandWriteProxy * eventHandlerWrite;
    it = eventGeneratorProxyPointers.EventGeneratorWriteProxies.begin();
    itEnd = eventGeneratorProxyPointers.EventGeneratorWriteProxies.end();
    for (; it != itEnd; ++it) {
        // Get event handler proxy (of which id is currently zero)
        eventHandlerWriteBase = interfaceRequiredProxy->GetEventHandlerWrite(it->Name);
        if (!eventHandlerWriteBase) {
            CMN_LOG_CLASS_INIT_ERROR << "UpdateEventHandlerProxyID: no event write handler found: " << it->Name << std::endl;
            return false;
        }
        eventHandlerWrite = dynamic_cast<mtsCommandWriteProxy*>(eventHandlerWriteBase);
        CMN_ASSERT(eventHandlerWrite);

        // Set client ID and network proxy. Note that SetNetworkProxy() should
        // be called before SetCommandID().
        if (!eventHandlerWrite->SetNetworkProxy(interfaceProxyClient)) {
            CMN_LOG_CLASS_INIT_ERROR << "UpdateEventHandlerProxyID:: failed to set network proxy: " 
                << eventHandlerWrite->GetName() << std::endl;
            return false;
        }

        // Set event handler proxy's id
        eventHandlerWrite->SetCommandID(it->EventGeneratorProxyId);
        // Enable event handler (initially, an event handler is disabled when created)
        eventHandlerWrite->Enable();
    }

    return true;
}

bool mtsComponentProxy::UpdateCommandProxyID(const ConnectionIDType connectionID,
    const std::string & serverInterfaceProvidedName, const std::string & clientInterfaceRequiredName)
{
    // User connection id as client id
    const unsigned int clientID = connectionID;

    // Get an instance of network proxy server that serves the provided interface.
    mtsComponentInterfaceProxyServer * interfaceProxyServer = 
        InterfaceProvidedNetworkProxies.GetItem(serverInterfaceProvidedName);
    if (!interfaceProxyServer) {
        CMN_LOG_CLASS_INIT_ERROR << "UpdateCommandProxyID: no network interface proxy server found: " << serverInterfaceProvidedName << std::endl;
        return false;
    }

    // Fetch function proxy pointers from a connected required interface proxy
    // at server side, which will be used to update ids of command proxies'.
    mtsComponentInterfaceProxy::FunctionProxyPointerSet functionProxyPointers;
    if (!interfaceProxyServer->SendFetchFunctionProxyPointers(
            connectionID, clientInterfaceRequiredName, functionProxyPointers))
    {
        CMN_LOG_CLASS_INIT_ERROR << "UpdateCommandProxyID: failed to fetch function proxy pointers for connection id: " << connectionID << std::endl;
        return false;
    }

    // Get a provided interface proxy instance of which command proxies are going
    // to be updated.
    mtsInterfaceProvided * endUserInterface = GetInterfaceProvided(serverInterfaceProvidedName);
    if (!endUserInterface) {
        CMN_LOG_CLASS_INIT_ERROR << "GetEventGeneratorProxyPointer: failed to get provided interface: "
            << serverInterfaceProvidedName << std::endl;
        return false;
    }

    // Set command proxy IDs in the provided interface proxy as the
    // function proxies' pointers fetched from server process.

    // Void command
    mtsCommandVoidProxy * commandVoid = NULL;
    mtsComponentInterfaceProxy::FunctionProxySequence::const_iterator itVoid = functionProxyPointers.FunctionVoidProxies.begin();
    const mtsComponentInterfaceProxy::FunctionProxySequence::const_iterator itVoidEnd= functionProxyPointers.FunctionVoidProxies.end();
    for (; itVoid != itVoidEnd; ++itVoid) {
        commandVoid = dynamic_cast<mtsCommandVoidProxy*>(endUserInterface->GetCommandVoid(itVoid->Name));
        if (!commandVoid) {
            CMN_LOG_CLASS_INIT_ERROR << "UpdateCommandProxyID: failed to update command void proxy id: " 
                << itVoid->Name << std::endl;
            return false;
        }
        // Set client ID and network proxy. Note that SetNetworkProxy() should
        // be called before SetCommandID().
        if (!commandVoid->SetNetworkProxy(interfaceProxyServer, clientID)) {
            CMN_LOG_CLASS_INIT_ERROR << "UpdateCommandProxyID: failed to set network proxy for command void: "
                << commandVoid->GetName() << std::endl;
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
        commandWrite = dynamic_cast<mtsCommandWriteProxy*>(endUserInterface->GetCommandWrite(itWrite->Name));
        if (!commandWrite) {
            CMN_LOG_CLASS_INIT_ERROR << "UpdateCommandProxyID: failed to update command write proxy id: " 
                << itWrite->Name << std::endl;
            return false;
        }
        // Set client ID and network proxy
        if (!commandWrite->SetNetworkProxy(interfaceProxyServer, clientID)) {
            CMN_LOG_CLASS_INIT_ERROR << "UpdateCommandProxyID: failed to set network proxy for command write: " 
                << commandWrite->GetName() << std::endl;
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
        commandRead = dynamic_cast<mtsCommandReadProxy*>(endUserInterface->GetCommandRead(itRead->Name));
        if (!commandRead) {
            CMN_LOG_CLASS_INIT_ERROR << "UpdateCommandProxyID: failed to update command read proxy id: " 
                << itRead->Name << std::endl;
            return false;
        }
        // Set client ID and network proxy
        if (!commandRead->SetNetworkProxy(interfaceProxyServer, clientID)) {
            CMN_LOG_CLASS_INIT_ERROR << "UpdateCommandProxyID: failed to set network proxy for command read: " 
                << commandRead->GetName() << std::endl;
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
        commandQualifiedRead = dynamic_cast<mtsCommandQualifiedReadProxy*>(endUserInterface->GetCommandQualifiedRead(itQualifiedRead->Name));
        if (!commandQualifiedRead) {
            CMN_LOG_CLASS_INIT_ERROR << "UpdateCommandProxyID: failed to update command qualifiedRead proxy id: "
                << itQualifiedRead->Name << std::endl;
            return false;
        }
        // Set client ID and network proxy
        if (!commandQualifiedRead->SetNetworkProxy(interfaceProxyServer, clientID)) {
            CMN_LOG_CLASS_INIT_ERROR << "UpdateCommandProxyID: failed to set network proxy for qualified read command: " 
                << commandQualifiedRead->GetName() << std::endl;
            return false;
        }
        // Set command qualified read proxy's id and enable this command
        commandQualifiedRead->SetCommandID(itQualifiedRead->FunctionProxyId);
        commandQualifiedRead->Enable();
    }

    return true;
}

bool mtsComponentProxy::GetFunctionProxyPointers(const std::string & requiredInterfaceName,
    mtsComponentInterfaceProxy::FunctionProxyPointerSet & functionProxyPointers)
{
    // Get required interface proxy
    mtsInterfaceRequired * requiredInterfaceProxy = GetInterfaceRequired(requiredInterfaceName);
    if (!requiredInterfaceProxy) {
        CMN_LOG_CLASS_INIT_ERROR << "GetFunctionProxyPointers: failed to get required interface proxy: " << requiredInterfaceName << std::endl;
        return false;
    }

    // Get function proxy and event handler proxy map element
    FunctionProxyAndEventHandlerProxyMapElement * mapElement = FunctionProxyAndEventHandlerProxyMap.GetItem(requiredInterfaceName);
    if (!mapElement) {
        CMN_LOG_CLASS_INIT_ERROR << "GetFunctionProxyPointers: failed to get proxy map element: " << requiredInterfaceName << std::endl;
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
    mtsManagerLocal * localManager = mtsManagerLocal::GetSafeInstance();
    mtsComponent * clientComponent = localManager->GetComponent(clientComponentName);
    if (!clientComponent) {
        CMN_LOG_CLASS_INIT_ERROR << "GetEventGeneratorProxyPointer: no client component found: " << clientComponentName << std::endl;
        return false;
    }

    mtsInterfaceRequired * requiredInterface = clientComponent->GetInterfaceRequired(requiredInterfaceName);
    if (!requiredInterface) {
        CMN_LOG_CLASS_INIT_ERROR << "GetEventGeneratorProxyPointer: no required interface found: " << requiredInterfaceName << std::endl;
        return false;
    }
    mtsInterfaceProvided * providedInterface = dynamic_cast<mtsInterfaceProvided *>(
        const_cast<mtsInterfaceProvidedOrOutput*>(requiredInterface->GetConnectedInterface()));
    if (!providedInterface) {
        CMN_LOG_CLASS_INIT_ERROR << "GetEventGeneratorProxyPointer: failed to get connected provided interface: "
            << clientComponentName << ":" << requiredInterfaceName << std::endl;
        return false;
    }

    mtsComponentInterfaceProxy::EventGeneratorProxyElement element;
    mtsCommandBase * eventGenerator = NULL;

    std::vector<std::string> namesOfEventHandlersVoid = requiredInterface->GetNamesOfEventHandlersVoid();
    for (unsigned int i = 0; i < namesOfEventHandlersVoid.size(); ++i) {
        element.Name = namesOfEventHandlersVoid[i];
        eventGenerator = providedInterface->GetEventVoid(element.Name);
        if (!eventGenerator) {
            CMN_LOG_CLASS_INIT_ERROR << "GetEventGeneratorProxyPointer: no event void generator found: " << element.Name << std::endl;
            return false;
        }
        element.EventGeneratorProxyId = reinterpret_cast<CommandIDType>(eventGenerator);
        eventGeneratorProxyPointers.EventGeneratorVoidProxies.push_back(element);
    }

    std::vector<std::string> namesOfEventHandlersWrite = requiredInterface->GetNamesOfEventHandlersWrite();
    for (unsigned int i = 0; i < namesOfEventHandlersWrite.size(); ++i) {
        element.Name = namesOfEventHandlersWrite[i];
        eventGenerator = providedInterface->GetEventWrite(element.Name);
        if (!eventGenerator) {
            CMN_LOG_CLASS_INIT_ERROR << "GetEventGeneratorProxyPointer: no event write generator found: " << element.Name << std::endl;
            return false;
        }
        element.EventGeneratorProxyId = reinterpret_cast<CommandIDType>(eventGenerator);
        eventGeneratorProxyPointers.EventGeneratorWriteProxies.push_back(element);
    }

    return true;
}

std::string mtsComponentProxy::GetInterfaceProvidedUserName(
    const std::string & processName, const std::string & componentName)
{
    return std::string(processName + ":" + componentName);
}

//-------------------------------------------------------------------------
//  Utilities
//-------------------------------------------------------------------------
void mtsComponentProxy::ExtractInterfaceProvidedDescription(
    mtsInterfaceProvided * interfaceProvided, InterfaceProvidedDescription & providedInterfaceDescription)
{
    if (!interfaceProvided) return;

    // Make sure we use an end user interface
    mtsManagerLocal * localManager = mtsManagerLocal::GetInstance();
    mtsInterfaceProvided * endUserInterface = interfaceProvided->GetEndUserInterface(localManager->GetEndUserName());
    CMN_ASSERT(endUserInterface);

    // Serializer initialization
    std::stringstream streamBuffer;
    cmnSerializer serializer(streamBuffer);

    // Get information about commands and events.  Note that we fetch void 
    // command objects from non-queued command container assuming the non-
    // queued container is updated for both queued and non-queued commands.

    // Extract void commands
    mtsCommandVoid * voidCommand;
    CommandVoidElement elementCommandVoid;
    const std::vector<std::string> namesOfVoidCommand = endUserInterface->GetNamesOfCommandsVoid();
    for (size_t i = 0; i < namesOfVoidCommand.size(); ++i) {
        voidCommand = endUserInterface->GetCommandVoid(namesOfVoidCommand[i]);
        if (!voidCommand) {
            CMN_LOG_RUN_ERROR << "ExtractInterfaceProvidedDescription: null void command: " 
                << namesOfVoidCommand[i] << std::endl;
            continue;
        }

        elementCommandVoid.Name = voidCommand->GetName();
        providedInterfaceDescription.CommandsVoid.push_back(elementCommandVoid);
    }

    // Extract write commands
    mtsCommandWriteBase * writeCommand;
    CommandWriteElement elementCommandWrite;
    const std::vector<std::string> namesOfWriteCommand = endUserInterface->GetNamesOfCommandsWrite();
    for (size_t i = 0; i < namesOfWriteCommand.size(); ++i) {
        writeCommand = endUserInterface->GetCommandWrite(namesOfWriteCommand[i]);
        if (!writeCommand) {
            CMN_LOG_RUN_ERROR << "ExtractInterfaceProvidedDescription: null write command: " 
                << namesOfWriteCommand[i] << std::endl;
            continue;
        }

        elementCommandWrite.Name = writeCommand->GetName();
        // serialize argument
        streamBuffer.str("");
        serializer.Serialize(*(writeCommand->GetArgumentPrototype()));
        elementCommandWrite.ArgumentPrototypeSerialized = streamBuffer.str();
        providedInterfaceDescription.CommandsWrite.push_back(elementCommandWrite);
    }

    // Extract read commands
    mtsCommandRead * readCommand;
    CommandReadElement elementCommandRead;
    const std::vector<std::string> namesOfReadCommand = endUserInterface->GetNamesOfCommandsRead();
    for (size_t i = 0; i < namesOfReadCommand.size(); ++i) {
        readCommand = endUserInterface->GetCommandRead(namesOfReadCommand[i]);
        if (!readCommand) {
            CMN_LOG_RUN_ERROR << "ExtractInterfaceProvidedDescription: null read command: " 
                << namesOfReadCommand[i] << std::endl;
            continue;
        }

        elementCommandRead.Name = readCommand->GetName();
        // serialize argument
        streamBuffer.str("");
        serializer.Serialize(*(readCommand->GetArgumentPrototype()));
        elementCommandRead.ArgumentPrototypeSerialized = streamBuffer.str();
        providedInterfaceDescription.CommandsRead.push_back(elementCommandRead);
    }

    // Extract qualified read commands
    mtsCommandQualifiedRead * qualifiedReadCommand;
    CommandQualifiedReadElement elementCommandQualifiedRead;
    const std::vector<std::string> namesOfQualifiedReadCommand = endUserInterface->GetNamesOfCommandsQualifiedRead();
    for (size_t i = 0; i < namesOfQualifiedReadCommand.size(); ++i) {
        qualifiedReadCommand = endUserInterface->GetCommandQualifiedRead(namesOfQualifiedReadCommand[i]);
        if (!qualifiedReadCommand) {
            CMN_LOG_RUN_ERROR << "ExtractInterfaceProvidedDescription: null qualified read command: " 
                << namesOfQualifiedReadCommand[i] << std::endl;
            continue;
        }

        elementCommandQualifiedRead.Name = qualifiedReadCommand->GetName();
        // serialize argument1
        streamBuffer.str("");
        serializer.Serialize(*(qualifiedReadCommand->GetArgument1Prototype()));
        elementCommandQualifiedRead.Argument1PrototypeSerialized = streamBuffer.str();
        // serialize argument2
        streamBuffer.str("");
        serializer.Serialize(*(qualifiedReadCommand->GetArgument2Prototype()));
        elementCommandQualifiedRead.Argument2PrototypeSerialized = streamBuffer.str();
        providedInterfaceDescription.CommandsQualifiedRead.push_back(elementCommandQualifiedRead);
    }

    // Extract void events
    mtsMulticastCommandVoid * voidEvent;
    EventVoidElement elementEventVoid;
    const std::vector<std::string> namesOfVoidEvent = endUserInterface->GetNamesOfEventsVoid();
    for (size_t i = 0; i < namesOfVoidEvent.size(); ++i) {
        voidEvent = endUserInterface->GetEventVoid(namesOfVoidEvent[i]);
        if (!voidEvent) {
            CMN_LOG_RUN_ERROR << "ExtractInterfaceProvidedDescription: null void event: " 
                << namesOfVoidEvent[i] << std::endl;
            continue;
        }
        elementEventVoid.Name = voidEvent->GetName();
        providedInterfaceDescription.EventsVoid.push_back(elementEventVoid);
    }

    // Extract write events
    mtsMulticastCommandWriteBase * writeEvent;
    EventWriteElement elementEventWrite;
    const std::vector<std::string> namesOfWriteEvent = endUserInterface->GetNamesOfEventsWrite();
    for (size_t i = 0; i < namesOfWriteEvent.size(); ++i) {
        writeEvent = endUserInterface->GetEventWrite(namesOfWriteEvent[i]);
        if (!writeEvent) {
            CMN_LOG_RUN_ERROR << "ExtractInterfaceProvidedDescription: null write event: " 
                << namesOfWriteEvent[i] << std::endl;
            continue;
        }
        elementEventWrite.Name = writeEvent->GetName();
        // serialize argument
        streamBuffer.str("");
        serializer.Serialize(*(writeEvent->GetArgumentPrototype()));
        elementEventWrite.ArgumentPrototypeSerialized = streamBuffer.str();
        providedInterfaceDescription.EventsWrite.push_back(elementEventWrite);
    }
}

void mtsComponentProxy::ExtractInterfaceRequiredDescription(
    mtsInterfaceRequired * requiredInterface, InterfaceRequiredDescription & requiredInterfaceDescription)
{
    // Serializer initialization
    std::stringstream streamBuffer;
    cmnSerializer serializer(streamBuffer);

    // Extract "IsRequired" attribute
    requiredInterfaceDescription.IsRequired = requiredInterface->IsRequired();

    // Extract void functions
    requiredInterfaceDescription.FunctionVoidNames = requiredInterface->GetNamesOfFunctionsVoid();
    // Extract write functions
    requiredInterfaceDescription.FunctionWriteNames = requiredInterface->GetNamesOfFunctionsWrite();
    // Extract read functions
    requiredInterfaceDescription.FunctionReadNames = requiredInterface->GetNamesOfFunctionsRead();
    // Extract qualified read functions
    requiredInterfaceDescription.FunctionQualifiedReadNames = requiredInterface->GetNamesOfFunctionsQualifiedRead();

    // Extract void event handlers
    CommandVoidElement elementEventVoidHandler;
    mtsInterfaceRequired::EventHandlerVoidMapType::MapType::const_iterator itVoid = requiredInterface->EventHandlersVoid.begin();
    const mtsInterfaceRequired::EventHandlerVoidMapType::MapType::const_iterator itVoidEnd = requiredInterface->EventHandlersVoid.end();
    for (; itVoid != itVoidEnd; ++itVoid) {
        elementEventVoidHandler.Name = itVoid->second->GetName();
        requiredInterfaceDescription.EventHandlersVoid.push_back(elementEventVoidHandler);
    }

    // Extract write event handlers
    CommandWriteElement elementEventWriteHandler;
    mtsInterfaceRequired::EventHandlerWriteMapType::MapType::const_iterator itWrite = requiredInterface->EventHandlersWrite.begin();
    const mtsInterfaceRequired::EventHandlerWriteMapType::MapType::const_iterator itWriteEnd = requiredInterface->EventHandlersWrite.end();
    for (; itWrite != itWriteEnd; ++itWrite) {
        elementEventWriteHandler.Name = itWrite->second->GetName();
        // argument serialization
        streamBuffer.str("");
        serializer.Serialize(*(itWrite->second->GetArgumentPrototype()));
        elementEventWriteHandler.ArgumentPrototypeSerialized = streamBuffer.str();
        requiredInterfaceDescription.EventHandlersWrite.push_back(elementEventWriteHandler);
    }
}

bool mtsComponentProxy::AddConnectionInformation(const std::string & serverInterfaceProvidedName, const ConnectionIDType connectionID)
{
    mtsComponentInterfaceProxyServer * interfaceProxyServer = InterfaceProvidedNetworkProxies.GetItem(serverInterfaceProvidedName);
    if (!interfaceProxyServer) {
        CMN_LOG_CLASS_INIT_ERROR << "AddConnectionInformation: no server interface proxy found: " << serverInterfaceProvidedName << std::endl;
        return false;
    }

    return interfaceProxyServer->AddConnectionInformation(connectionID);
}
