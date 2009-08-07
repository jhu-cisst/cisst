/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsDeviceProxy.cpp 291 2009-04-28 01:49:13Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2009-06-30

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsDeviceProxy.h>
#include <cisstMultiTask/mtsDeviceInterface.h>
#include <cisstMultiTask/mtsDeviceInterfaceProxy.h>
#include <cisstMultiTask/mtsDeviceInterfaceProxyClient.h>

CMN_IMPLEMENT_SERVICES(mtsDeviceProxy)

//-----------------------------------------------------------------------------
//  Common Methods
//-----------------------------------------------------------------------------
mtsDeviceProxy::~mtsDeviceProxy()
{
    FunctionVoidProxyMap.DeleteAll();
    FunctionWriteProxyMap.DeleteAll();
    FunctionReadProxyMap.DeleteAll();
    FunctionQualifiedReadProxyMap.DeleteAll();
    EventVoidGeneratorProxyMap.DeleteAll();
    EventWriteGeneratorProxyMap.DeleteAll();
}

/* Server task proxy naming rule:
    
   Server-TS:PI-TC:RI

   where TS: server task name
         PI: provided interface name
         TC: client task name
         RI: required interface name
*/
std::string mtsDeviceProxy::GetServerTaskProxyName(
    const std::string & resourceTaskName, const std::string & providedInterfaceName,
    const std::string & userTaskName, const std::string & requiredInterfaceName)
{
    return "Server-" +
           resourceTaskName + ":" +      // TS
           providedInterfaceName + "-" + // PI
           userTaskName + ":" +          // TC
           requiredInterfaceName;        // RI
}

/* Client task proxy naming rule:
    
   Client-TS:PI-TC:RI

   where TS: server task name
         PI: provided interface name
         TC: client task name
         RI: required interface name
*/
std::string mtsDeviceProxy::GetClientTaskProxyName(
    const std::string & resourceTaskName, const std::string & providedInterfaceName,
    const std::string & userTaskName, const std::string & requiredInterfaceName)
{
    return "Client-" +
           resourceTaskName + ":" +      // TS
           providedInterfaceName + "-" + // PI
           userTaskName + ":" +          // TC
           requiredInterfaceName;        // RI
}

//-----------------------------------------------------------------------------
//  Methods that run at client side
//-----------------------------------------------------------------------------
void mtsDeviceProxy::UpdateCommandId(
    const mtsDeviceInterfaceProxy::FunctionProxySet & functionProxies)
{
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();

    const std::string serverTaskProxyName = functionProxies.ServerTaskProxyName;
    mtsDevice * serverTaskProxy = taskManager->GetDevice(serverTaskProxyName);
    CMN_ASSERT(serverTaskProxy);

    mtsProvidedInterface * providedInterfaceProxy = 
        serverTaskProxy->GetProvidedInterface(functionProxies.ProvidedInterfaceProxyName);
    CMN_ASSERT(providedInterfaceProxy);

    //mtsCommandVoidProxy * commandVoid = NULL;
    //mtsDeviceInterfaceProxy::FunctionProxySequence::const_iterator it = 
    //    functionProxies.FunctionVoidProxies.begin();
    //for (; it != functionProxies.FunctionVoidProxies.end(); ++it) {
    //    commandVoid = dynamic_cast<mtsCommandVoidProxy*>(
    //        providedInterfaceProxy->GetCommandVoid(it->Name));
    //    CMN_ASSERT(commandVoid);
    //    commandVoid->SetCommandId(it->FunctionProxyId);
    //}

    // Replace a command id value with an actual pointer to the function
    // pointer at server side (this resolves thread synchronization issue).
#define UPDATE_COMMAND_ID(_commandType)\
    mtsCommand##_commandType##Proxy * command##_commandType = NULL;\
    mtsDeviceInterfaceProxy::FunctionProxySequence::const_iterator it##_commandType = \
        functionProxies.Function##_commandType##Proxies.begin();\
    for (; it##_commandType != functionProxies.Function##_commandType##Proxies.end(); ++it##_commandType) {\
        command##_commandType = dynamic_cast<mtsCommand##_commandType##Proxy*>(\
            providedInterfaceProxy->GetCommand##_commandType(it##_commandType->Name));\
        if (command##_commandType)\
            command##_commandType->SetCommandId(it##_commandType->FunctionProxyId);\
    }

    UPDATE_COMMAND_ID(Void);
    UPDATE_COMMAND_ID(Write);
    UPDATE_COMMAND_ID(Read);
    UPDATE_COMMAND_ID(QualifiedRead);
}

mtsProvidedInterface * mtsDeviceProxy::CreateProvidedInterfaceProxy(
    mtsDeviceInterfaceProxyClient * proxyClient,
    const mtsDeviceInterfaceProxy::ProvidedInterfaceInfo & providedInterfaceInfo)
{
    if (!proxyClient) {
        CMN_LOG_RUN_ERROR << "CreateProvidedInterfaceProxy: NULL required interface proxy." << std::endl;
        return NULL;
    }

    // Create a local provided interface (a provided interface proxy).
    mtsDeviceInterface * providedInterfaceProxy = 
        AddProvidedInterface(providedInterfaceInfo.InterfaceName);
    if (!providedInterfaceProxy) {
        CMN_LOG_RUN_ERROR << "CreateProvidedInterfaceProxy: AddProvidedInterface failed." << std::endl;
        return NULL;
    }

    // Create command proxies.
    // CommandId is initially set to zero meaning that it needs to be updated later.
    // An actual value will be assigned later when UpdateCommandId() is executed.
    int commandId = 0;
    std::string commandName;

#define ADD_COMMAND_PROXY_BEGIN(_commandType) \
    {\
        mtsCommand##_commandType##Proxy * newCommand##_commandType = NULL;\
        mtsDeviceInterfaceProxy::Command##_commandType##Sequence::const_iterator it\
            = providedInterfaceInfo.Commands##_commandType.begin();\
        for (; it != providedInterfaceInfo.Commands##_commandType.end(); ++it) {\
            commandName = it->Name;
#define ADD_COMMAND_PROXY_END \
        }\
    }

    // 2-1) Void
    ADD_COMMAND_PROXY_BEGIN(Void)
        newCommandVoid = new mtsCommandVoidProxy(commandId, proxyClient, commandName);
        providedInterfaceProxy->GetCommandVoidMap().AddItem(commandName, newCommandVoid);
    ADD_COMMAND_PROXY_END

    // 2-2) Write
    ADD_COMMAND_PROXY_BEGIN(Write)
        newCommandWrite = new mtsCommandWriteProxy(commandId, proxyClient, commandName);
        providedInterfaceProxy->GetCommandWriteMap().AddItem(commandName, newCommandWrite);
    ADD_COMMAND_PROXY_END

    // 2-3) Read
    ADD_COMMAND_PROXY_BEGIN(Read)
        newCommandRead = new mtsCommandReadProxy(commandId, proxyClient, commandName);
        providedInterfaceProxy->GetCommandReadMap().AddItem(commandName, newCommandRead);
    ADD_COMMAND_PROXY_END

    // 2-4) QualifiedRead
    ADD_COMMAND_PROXY_BEGIN(QualifiedRead)
        newCommandQualifiedRead = new mtsCommandQualifiedReadProxy(commandId, proxyClient, commandName);
        providedInterfaceProxy->GetCommandQualifiedReadMap().AddItem(commandName, newCommandQualifiedRead);
    ADD_COMMAND_PROXY_END

#undef ADD_COMMAND_PROXY_BEGIN
#undef ADD_COMMAND_PROXY_END

    // 3) Create event generator proxies.
    std::string eventName;

    mtsFunctionVoid * eventVoidGeneratorProxy = NULL;
    mtsDeviceInterfaceProxy::EventVoidSequence::const_iterator itEventVoid =
        providedInterfaceInfo.EventsVoid.begin();
    for (; itEventVoid != providedInterfaceInfo.EventsVoid.end(); ++itEventVoid) {
        eventName = itEventVoid->Name;
        eventVoidGeneratorProxy = new mtsFunctionVoid();        
        CMN_ASSERT(EventVoidGeneratorProxyMap.AddItem(eventName, eventVoidGeneratorProxy));
        
        CMN_ASSERT(eventVoidGeneratorProxy->Bind(providedInterfaceProxy->AddEventVoid(eventName)));
    }

    mtsFunctionWrite * eventWriteGeneratorProxy = NULL;
    mtsMulticastCommandWriteProxy * eventMulticastCommandProxy = NULL;

    mtsDeviceInterfaceProxy::EventWriteSequence::const_iterator itEventWrite =
        providedInterfaceInfo.EventsWrite.begin();
    for (; itEventWrite != providedInterfaceInfo.EventsWrite.end(); ++itEventWrite) {
        eventName = itEventWrite->Name;
        eventWriteGeneratorProxy = new mtsFunctionWrite();
        CMN_ASSERT(EventWriteGeneratorProxyMap.AddItem(eventName, eventWriteGeneratorProxy));

        eventMulticastCommandProxy = new mtsMulticastCommandWriteProxy(eventName);
        CMN_ASSERT(providedInterfaceProxy->AddEvent(eventName, eventMulticastCommandProxy));
        CMN_ASSERT(eventWriteGeneratorProxy->Bind(eventMulticastCommandProxy));
    }
    
    return providedInterfaceProxy;
}

void mtsDeviceProxy::GetEventGeneratorProxyPointers(
    mtsProvidedInterface * providedInterfaceProxy,
    mtsRequiredInterface * requiredInterface,
    mtsDeviceInterfaceProxy::ListsOfEventGeneratorsRegistered & eventGeneratorProxies)
{
    // Get the lists of event handlers registered.
    mtsDeviceInterfaceProxy::EventGeneratorProxyElement element;
    mtsCommandBase * eventGenerator = NULL;

    //std::vector<std::string> namesOfEventHandlersVoid = 
    //    requiredInterface->GetNamesOfEventHandlersVoid();
    //for (unsigned int i = 0; i < namesOfEventHandlersVoid.size(); ++i) {
    //    element.Name = namesOfEventHandlersVoid[i];
    //    eventGenerator = providedInterfaceProxy->EventVoidGenerators.GetItem(element.Name);
    //    CMN_ASSERT(eventGenerator);
    //    element.ProxyId = reinterpret_cast<int>(eventGenerator);
    //    eventGeneratorProxies.EventGeneratorVoidProxies.push_back(element);
    //}
#define GET_EVENT_GENERATOR_PROXY_BEGIN(_type)\
    std::vector<std::string> namesOfEventHandlers##_type = \
        requiredInterface->GetNamesOfEventHandlers##_type();\
    for (unsigned int i = 0; i < namesOfEventHandlers##_type.size(); ++i) {\
        element.Name = namesOfEventHandlers##_type[i];\
        eventGenerator = providedInterfaceProxy->Event##_type##Generators.GetItem(element.Name);\
        CMN_ASSERT(eventGenerator);\
        element.ProxyId = reinterpret_cast<int>(eventGenerator);\
        eventGeneratorProxies.EventGenerator##_type##Proxies.push_back(element);
#define GET_EVENT_GENERATOR_PROXY_END\
    }

    GET_EVENT_GENERATOR_PROXY_BEGIN(Void);
    GET_EVENT_GENERATOR_PROXY_END;

    GET_EVENT_GENERATOR_PROXY_BEGIN(Write);
    GET_EVENT_GENERATOR_PROXY_END;
}

//-----------------------------------------------------------------------------
//  Methods that run at server side
//-----------------------------------------------------------------------------
mtsRequiredInterface * mtsDeviceProxy::CreateRequiredInterfaceProxy(
    mtsProvidedInterface * providedInterface, const std::string & requiredInterfaceName,
    mtsDeviceInterfaceProxyServer * proxyServer)
{
    // Create a required Interface proxy (mtsRequiredInterface).
    mtsRequiredInterface * requiredInterfaceProxy = AddRequiredInterface(requiredInterfaceName);
    if (!requiredInterfaceProxy) {
        CMN_LOG_RUN_ERROR << "CreateRequiredInterfaceProxy: Cannot add required interface: "
            << requiredInterfaceName << std::endl;
        return NULL;
    }

    // Now, populate a required Interface proxy.
    
    // 1. Function proxies
    mtsFunctionVoid  * functionVoidProxy = NULL;
    mtsFunctionWrite * functionWriteProxy = NULL;
    mtsFunctionRead  * functionReadProxy = NULL;
    mtsFunctionQualifiedRead * functionQualifiedReadProxy = NULL;

    //std::vector<std::string> namesOfCommandsVoid = providedInterface.GetNamesOfCommandsVoid();
    //for (unsigned int i = 0; i < namesOfCommandsVoid.size(); ++i) {
    //    functionVoidProxy = new mtsFunctionVoid(providedInterface, namesOfCommandsVoid[i]);
    //    CMN_ASSERT(FunctionVoidProxyMap.AddItem(namesOfCommandsVoid[i], functionVoidProxy));
    //    CMN_ASSERT(requiredInterfaceProxy->AddFunction(namesOfCommandsVoid[i], *functionVoidProxy));
    //}
#define ADD_FUNCTION_PROXY_BEGIN(_commandType)\
    std::vector<std::string> namesOfCommands##_commandType = providedInterface->GetNamesOfCommands##_commandType##();\
    for (unsigned int i = 0; i < namesOfCommands##_commandType.size(); ++i) {\
        function##_commandType##Proxy = new mtsFunction##_commandType##(providedInterface, namesOfCommands##_commandType##[i]);\
        CMN_ASSERT(Function##_commandType##ProxyMap.AddItem(namesOfCommands##_commandType[i], function##_commandType##Proxy));\
        CMN_ASSERT(requiredInterfaceProxy->AddFunction(namesOfCommands##_commandType##[i], *function##_commandType##Proxy));
#define ADD_FUNCTION_PROXY_END\
    }

    std::vector<std::string> namesOfCommandsVoid = providedInterface->GetNamesOfCommandsVoid();
    for (unsigned int i = 0; i < namesOfCommandsVoid.size(); ++i) {
        functionVoidProxy = new mtsFunctionVoid(providedInterface, namesOfCommandsVoid[i]);
        CMN_ASSERT(FunctionVoidProxyMap.AddItem(namesOfCommandsVoid[i], functionVoidProxy)); 
        CMN_ASSERT(requiredInterfaceProxy->AddFunction(namesOfCommandsVoid[i], *functionVoidProxy));
    }

    std::vector<std::string> namesOfCommandsWrite = providedInterface->GetNamesOfCommandsWrite();
    for (unsigned int i = 0; i < namesOfCommandsWrite.size(); ++i) {
        functionWriteProxy = new mtsFunctionWrite(providedInterface, namesOfCommandsWrite[i]);
        CMN_ASSERT(FunctionWriteProxyMap.AddItem(namesOfCommandsWrite[i], functionWriteProxy)); 
        CMN_ASSERT(requiredInterfaceProxy->AddFunction(namesOfCommandsWrite[i], *functionWriteProxy));
    }

    std::vector<std::string> namesOfCommandsRead = providedInterface->GetNamesOfCommandsRead();
    for (unsigned int i = 0; i < namesOfCommandsRead.size(); ++i) {
        functionReadProxy = new mtsFunctionRead(providedInterface, namesOfCommandsRead[i]);
        CMN_ASSERT(FunctionReadProxyMap.AddItem(namesOfCommandsRead[i], functionReadProxy)); 
        CMN_ASSERT(requiredInterfaceProxy->AddFunction(namesOfCommandsRead[i], *functionReadProxy));
    }

    std::vector<std::string> namesOfCommandsQualifiedRead = providedInterface->GetNamesOfCommandsQualifiedRead();
    for (unsigned int i = 0; i < namesOfCommandsQualifiedRead.size(); ++i) {
        functionQualifiedReadProxy = new mtsFunctionQualifiedRead(providedInterface, namesOfCommandsQualifiedRead[i]);
        CMN_ASSERT(FunctionQualifiedReadProxyMap.AddItem(namesOfCommandsQualifiedRead[i], functionQualifiedReadProxy)); 
        CMN_ASSERT(requiredInterfaceProxy->AddFunction(namesOfCommandsQualifiedRead[i], *functionQualifiedReadProxy));
    }


    // 2. Event handler proxies.
    std::string eventName;

    // All CommandId values are initially set to zero meaning that they need 
    // to be updated later. An actual value will be assigned by the client when 
    // the server execute GetListsOfEventGeneratorsRegistered() method.
    // Also note that all events are disabled by default. Commands that are
    // actually bounded and used at the client will only be enabled by the
    // execution of the GetListsOfEventGeneratorsRegistered() method.
    mtsCommandVoidProxy * actualEventVoidCommandProxy = NULL;
    std::vector<std::string> namesOfEventsVoid = providedInterface->GetNamesOfEventsVoid();
    for (unsigned int i = 0; i < namesOfEventsVoid.size(); ++i) {
        eventName = namesOfEventsVoid[i];
        actualEventVoidCommandProxy = new mtsCommandVoidProxy(NULL, proxyServer, eventName);
        actualEventVoidCommandProxy->Disable();

        CMN_ASSERT(requiredInterfaceProxy->EventHandlersVoid.AddItem(
            eventName, actualEventVoidCommandProxy));
        CMN_ASSERT(EventHandlerVoidProxyMap.AddItem(eventName, actualEventVoidCommandProxy));
    }

    mtsCommandWriteProxy * actualEventWriteCommandProxy = NULL;    
    std::vector<std::string> namesOfEventsWrite = providedInterface->GetNamesOfEventsWrite();
    for (unsigned int i = 0; i < namesOfEventsWrite.size(); ++i) {
        eventName = namesOfEventsWrite[i];
        actualEventWriteCommandProxy = new mtsCommandWriteProxy(NULL, proxyServer, eventName);
        actualEventWriteCommandProxy->Disable();

        CMN_ASSERT(EventHandlerWriteProxyMap.AddItem(eventName, actualEventWriteCommandProxy));
        CMN_ASSERT(requiredInterfaceProxy->EventHandlersWrite.AddItem(
            eventName, actualEventWriteCommandProxy));        
    }

    // Using AllocateResources(), get pointers which have been allocated for this 
    // required interface and are thread-safe to use.
    unsigned int userId;
    std::string userName = requiredInterfaceProxy->GetName() + "Proxy";
    userId = providedInterface->AllocateResources(userName);

    // Connect to the original device or task that provides allocated resources.
    requiredInterfaceProxy->ConnectTo(providedInterface);
    if (!requiredInterfaceProxy->BindCommandsAndEvents(userId)) {
        CMN_LOG_RUN_ERROR << "CreateRequiredInterfaceProxy: BindCommandsAndEvents failed: "
            << userName << " with userId = " << userId << std::endl;
        return NULL;
    }

    return requiredInterfaceProxy;
}

void mtsDeviceProxy::GetFunctionPointers(
    mtsDeviceInterfaceProxy::FunctionProxySet & functionProxySet)
{
    mtsDeviceInterfaceProxy::FunctionProxyInfo element;

    FunctionVoidProxyMapType::MapType::const_iterator it;
    it = FunctionVoidProxyMap.GetMap().begin();
    //for (; it != FunctionVoidProxyMap.GetMap().end(); ++it) {
    //    element.Name = it->first;
    //    element.FunctionProxyId = reinterpret_cast<int>(it->second);
    //    functionProxy.FunctionVoidProxies.push_back(element);
    //}
#define GET_FUNCTION_PROXY_BEGIN(_commandType)\
    Function##_commandType##ProxyMapType::MapType::const_iterator it##_commandType;\
    it##_commandType = Function##_commandType##ProxyMap.GetMap().begin();\
    for (; it##_commandType != Function##_commandType##ProxyMap.GetMap().end(); ++it##_commandType) {\
        element.Name = it##_commandType->first;\
        element.FunctionProxyId = reinterpret_cast<int>(it##_commandType->second);\
        functionProxySet.Function##_commandType##Proxies.push_back(element)
#define GET_FUNCTION_PROXY_END\
    }

    GET_FUNCTION_PROXY_BEGIN(Void);
    GET_FUNCTION_PROXY_END;

    GET_FUNCTION_PROXY_BEGIN(Write);
    GET_FUNCTION_PROXY_END;

    GET_FUNCTION_PROXY_BEGIN(Read);
    GET_FUNCTION_PROXY_END;

    GET_FUNCTION_PROXY_BEGIN(QualifiedRead);
    GET_FUNCTION_PROXY_END;
}

void mtsDeviceProxy::UpdateEventHandlerId(
    const mtsDeviceInterfaceProxy::ListsOfEventGeneratorsRegistered eventHandlers)
{
    mtsDeviceInterfaceProxy::EventGeneratorProxySequence::const_iterator it;

    //mtsCommandVoidProxy * eventHandlerVoid = NULL;
    //it = eventHandlers.EventGeneratorVoidProxies.begin();
    //for (; it != eventHandlers.EventGeneratorVoidProxies.end(); ++it) {
    //    eventHandlerVoid = EventHandlerVoidProxyMap.GetItem(it->Name);
    //}
    //    CMN_ASSERT(eventHandlerVoid);

    //    eventHandlerVoid->Enable();
    //    eventHandlerVoid->SetCommandId(it->ProxyId);
    //}
#define UPDATE_EVENT_HANDLER_ID(_type)\
    mtsCommand##_type##Proxy * eventHandler##_type = NULL;\
    it = eventHandlers.EventGenerator##_type##Proxies.begin();\
    for (; it != eventHandlers.EventGenerator##_type##Proxies.end(); ++it) {\
        eventHandler##_type = EventHandler##_type##ProxyMap.GetItem(it->Name);\
        CMN_ASSERT(eventHandler##_type);\
        eventHandler##_type->Enable();\
        eventHandler##_type->SetCommandId(it->ProxyId);\
    }

    UPDATE_EVENT_HANDLER_ID(Void);
    UPDATE_EVENT_HANDLER_ID(Write);
}
