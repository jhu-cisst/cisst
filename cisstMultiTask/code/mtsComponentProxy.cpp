/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2009-12-18

  (C) Copyright 2009-2012 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "mtsComponentProxy.h"

#include <cisstCommon/cmnUnits.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include "mtsInterfaceRequiredProxy.h"
#include "mtsFunctionReadProxy.h"
#include "mtsFunctionWriteProxy.h"
#include "mtsFunctionQualifiedReadProxy.h"
#include "mtsFunctionVoidReturnProxy.h"
#include "mtsFunctionWriteReturnProxy.h"
#include "mtsCommandVoidProxyForReturnEvent.h"


mtsComponentProxy::FunctionProxyAndEventHandlerProxyMapElement::FunctionProxyAndEventHandlerProxyMapElement():
    FunctionVoidProxyMap("FunctionVoidProxyMap"),
    FunctionWriteProxyMap("FunctionWriteProxyMap"),
    FunctionReadProxyMap("FunctionReadProxyMap"),
    FunctionQualifiedReadProxyMap("FunctionQualifiedReadProxyMap"),
    FunctionVoidReturnProxyMap("FunctionVoidReturnProxyMap"),
    FunctionWriteReturnProxyMap("FunctionWriteReturnProxyMap"),
    EventGeneratorVoidProxyMap("EventGeneratorVoidProxyMap"),
    EventGeneratorWriteProxyMap("EventGeneratorWriteProxyMap")
{
    // Could pass mtsComponentProxy object to SetOwner().
}

mtsComponentProxy::mtsComponentProxy(const std::string & componentProxyName):
    mtsComponent(componentProxyName),
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
    InterfaceProvidedNetworkProxies.DeleteAll();
    InterfaceRequiredNetworkProxies.DeleteAll();
    FunctionProxyAndEventHandlerProxyMap.DeleteAll();
}



mtsInterfaceRequired * mtsComponentProxy::AddInterfaceRequiredWithoutSystemEventHandlers(const std::string & interfaceName,
                                                                                         mtsRequiredType required)
{
    mtsInterfaceRequired * interfaceRequired = new mtsInterfaceRequiredProxy(interfaceName, this, 0, required);
    return mtsComponent::AddInterfaceRequiredExisting(interfaceName, interfaceRequired);
}


//-----------------------------------------------------------------------------
//  Methods for Server Components
//-----------------------------------------------------------------------------
bool mtsComponentProxy::CreateInterfaceRequiredProxy(const mtsInterfaceRequiredDescription & requiredInterfaceDescription)
{
    const std::string requiredInterfaceName = requiredInterfaceDescription.InterfaceName;
    const mtsRequiredType isRequired = (requiredInterfaceDescription.IsRequired ? MTS_REQUIRED : MTS_OPTIONAL);

    // Create a local required interface (a required interface proxy)
    mtsInterfaceRequired * requiredInterfaceProxy = AddInterfaceRequiredWithoutSystemEventHandlers(requiredInterfaceName, isRequired);
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
    mtsFunctionVoidReturn * functionVoidReturnProxy;
    mtsFunctionWriteReturn * functionWriteReturnProxy;

    bool success;

    // Create void function proxies
    const std::vector<std::string> namesOfFunctionVoid = requiredInterfaceDescription.FunctionVoidNames;
    for (size_t i = 0; i < namesOfFunctionVoid.size(); ++i) {
        functionVoidProxy = new mtsFunctionVoid(true /* create function for proxy */);
        success = requiredInterfaceProxy->AddFunction(namesOfFunctionVoid[i], *functionVoidProxy);
        success &= mapElement->FunctionVoidProxyMap.AddItem(namesOfFunctionVoid[i], functionVoidProxy);
        if (!success) {
            bool removed = RemoveInterfaceRequired(requiredInterfaceName);
            CMN_ASSERT(removed);
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
            bool removed = RemoveInterfaceRequired(requiredInterfaceName);
            CMN_ASSERT(removed);
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
            bool removed = RemoveInterfaceRequired(requiredInterfaceName);
            CMN_ASSERT(removed);
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
            bool removed = RemoveInterfaceRequired(requiredInterfaceName);
            CMN_ASSERT(removed);
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceRequiredProxy: failed to add qualified read function proxy: \"" << namesOfFunctionQualifiedRead[i] << "\"" << std::endl;
            return false;
        }
    }

    // Create VoidReturn function proxies
    const std::vector<std::string> namesOfFunctionVoidReturn = requiredInterfaceDescription.FunctionVoidReturnNames;
    for (size_t i = 0; i < namesOfFunctionVoidReturn.size(); ++i) {
        functionVoidReturnProxy = new mtsFunctionVoidReturnProxy(requiredInterfaceProxy); // needs to know owner interface
        success = requiredInterfaceProxy->AddFunction(namesOfFunctionVoidReturn[i], *functionVoidReturnProxy);
        success &= mapElement->FunctionVoidReturnProxyMap.AddItem(namesOfFunctionVoidReturn[i], functionVoidReturnProxy);
        if (!success) {
            bool removed = RemoveInterfaceRequired(requiredInterfaceName);
            CMN_ASSERT(removed);
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceRequiredProxy: failed to add void return function proxy: \"" << namesOfFunctionVoidReturn[i] << "\"" << std::endl;
            return false;
        }
    }

    // Create WriteReturn function proxies
    const std::vector<std::string> namesOfFunctionWriteReturn = requiredInterfaceDescription.FunctionWriteReturnNames;
    for (size_t i = 0; i < namesOfFunctionWriteReturn.size(); ++i) {
        functionWriteReturnProxy = new mtsFunctionWriteReturnProxy(requiredInterfaceProxy); // needs to know owner interface
        success = requiredInterfaceProxy->AddFunction(namesOfFunctionWriteReturn[i], *functionWriteReturnProxy);
        success &= mapElement->FunctionWriteReturnProxyMap.AddItem(namesOfFunctionWriteReturn[i], functionWriteReturnProxy);
        if (!success) {
            bool removed = RemoveInterfaceRequired(requiredInterfaceName);
            CMN_ASSERT(removed);
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceRequiredProxy: failed to add write return function proxy: \"" << namesOfFunctionWriteReturn[i] << "\"" << std::endl;
            return false;
        }
    }

    // Create event handler proxies
    std::string eventName;

    // Create event handler proxies with CommandIDs set to zero and disable them
    // by default. It will be updated and enabled later by UpdateEventHandlerID(),

    // Create void event handler proxy
    mtsCommandVoidProxy * newEventVoidHandlerProxy = 0;
    for (size_t i = 0; i < requiredInterfaceDescription.EventHandlersVoid.size(); ++i) {
        eventName = requiredInterfaceDescription.EventHandlersVoid[i].Name;
        if (eventName == "BlockingCommandReturnExecuted") {
            newEventVoidHandlerProxy = new mtsCommandVoidProxyForReturnEvent(eventName, requiredInterfaceProxy);
        } else {
            newEventVoidHandlerProxy = new mtsCommandVoidProxy(eventName);
        }
        if (!requiredInterfaceProxy->EventHandlersVoid.AddItem(eventName, newEventVoidHandlerProxy)) {
            delete newEventVoidHandlerProxy;
            bool removed = RemoveInterfaceRequired(requiredInterfaceName);
            CMN_ASSERT(removed);
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceRequiredProxy: failed to add void event handler proxy: \"" << eventName << "\"" << std::endl;
            return false;
        }
    }

    // Create write event handler proxies
    std::stringstream streamBuffer;
    cmnDeSerializer deserializer(streamBuffer);

    mtsCommandWriteProxy * newEventWriteHandlerProxy = 0;
    mtsGenericObject * argumentPrototype = 0;
    for (size_t i = 0; i < requiredInterfaceDescription.EventHandlersWrite.size(); ++i) {
        eventName = requiredInterfaceDescription.EventHandlersWrite[i].Name;
        newEventWriteHandlerProxy = new mtsCommandWriteProxy(eventName);
        if (!requiredInterfaceProxy->EventHandlersWrite.AddItem(eventName, newEventWriteHandlerProxy)) {
            delete newEventWriteHandlerProxy;
            bool removed = RemoveInterfaceRequired(requiredInterfaceName);
            CMN_ASSERT(removed);
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceRequiredProxy: failed to add write event handler proxy: \"" << eventName << "\""<< std::endl;
            return false;
        }

        // argument deserialization
        streamBuffer.str("");
        streamBuffer << requiredInterfaceDescription.EventHandlersWrite[i].ArgumentPrototypeSerialized;
        try {
            argumentPrototype = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        } catch (const std::exception &e) {
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceRequiredProxy: write command argument deserialization failed: " << e.what() << std::endl;
            argumentPrototype = 0;
        }

        if (!argumentPrototype) {
            bool removed = RemoveInterfaceRequired(requiredInterfaceName);
            CMN_ASSERT(removed);
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
        // Network server deactivation
        clientProxy->StopProxy();
    }

    // Remove required interface proxy
    if (!RemoveInterfaceRequired(requiredInterfaceProxyName)) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveInterfaceRequiredProxy: failed to remove required interface proxy: " << requiredInterfaceProxyName << std::endl;
        return false;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "RemoveInterfaceRequiredProxy: removed required interface proxy: " << requiredInterfaceProxyName << std::endl;

    return true;
}

const std::string mtsComponentProxy::GetNameOfProvidedInterfaceInstance(
                                                                        const std::string & originalProvidedInterfaceName, const ConnectionIDType connectionID)
{
    std::stringstream ss;
    ss << originalProvidedInterfaceName << "ForConn" << connectionID;

    return ss.str();
}

//-----------------------------------------------------------------------------
//  Methods for Client Components
//-----------------------------------------------------------------------------
bool mtsComponentProxy::CreateInterfaceProvidedProxy(const mtsInterfaceProvidedDescription & providedInterfaceDescription)
{
    const std::string providedInterfaceName = providedInterfaceDescription.InterfaceName;

    // Create a local provided interface (a provided interface proxy)
    mtsInterfaceProvided * providedInterfaceProxy = AddInterfaceProvidedWithoutSystemEvents(providedInterfaceName, MTS_COMPONENT_POLICY, true /* for proxy */);
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
    mtsGenericObject
        * argumentPrototype = 0,
        * argument1Prototype = 0,
        * argument2Prototype = 0,
        * resultPrototype = 0;

    std::stringstream streamBuffer;
    cmnDeSerializer deserializer(streamBuffer);

    // Create void command proxies
    mtsCommandVoidProxy * newCommandVoid = 0;
    mtsCommandsVoidDescription::const_iterator itVoid = providedInterfaceDescription.CommandsVoid.begin();
    const mtsCommandsVoidDescription::const_iterator itVoidEnd = providedInterfaceDescription.CommandsVoid.end();
    for (; itVoid != itVoidEnd; ++itVoid) {
        commandName = itVoid->Name;
        newCommandVoid = new mtsCommandVoidProxy(commandName);
        if (!providedInterfaceProxy->AddCommandVoid(newCommandVoid)) {
            delete newCommandVoid;
            bool removed = RemoveInterfaceProvided(providedInterfaceName);
            CMN_ASSERT(removed);
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: failed to add void command proxy: " << commandName << std::endl;
            return false;
        }
    }

    // Create write command proxies
    mtsCommandWriteProxy * newCommandWrite = 0;
    mtsCommandsWriteDescription::const_iterator itWrite = providedInterfaceDescription.CommandsWrite.begin();
    const mtsCommandsWriteDescription::const_iterator itWriteEnd = providedInterfaceDescription.CommandsWrite.end();
    for (; itWrite != itWriteEnd; ++itWrite) {
        commandName = itWrite->Name;
        newCommandWrite = new mtsCommandWriteProxy(commandName);
        if (!providedInterfaceProxy->AddCommandWrite(newCommandWrite)) {
            delete newCommandWrite;
            bool removed = RemoveInterfaceProvided(providedInterfaceName);
            CMN_ASSERT(removed);
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: failed to add write command proxy: " << commandName << std::endl;
            return false;
        }

        // argument deserialization
        streamBuffer.str("");
        streamBuffer << itWrite->ArgumentPrototypeSerialized;
        try {
            argumentPrototype = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        } catch (const std::exception &e) {
            CMN_LOG_CLASS_INIT_WARNING << "CreateInterfaceProvidedProxy: failed to deserialize write command argument: " << e.what() << std::endl;
            argumentPrototype = 0;
        }

        if (!argumentPrototype) {
            CMN_LOG_CLASS_INIT_WARNING << "CreateInterfaceProvidedProxy: failed to create write command proxy: " << commandName << std::endl;
            newCommandWrite->SetArgumentPrototypeSerialized(itWrite->ArgumentPrototypeSerialized);
            newCommandWrite->SetArgumentSupported(false);
        } else {
            newCommandWrite->SetArgumentPrototype(argumentPrototype);
        }
    }

    // Create read command proxies
    mtsCommandReadProxy * newCommandRead = 0;
    mtsCommandsReadDescription::const_iterator itRead = providedInterfaceDescription.CommandsRead.begin();
    const mtsCommandsReadDescription::const_iterator itReadEnd = providedInterfaceDescription.CommandsRead.end();
    for (; itRead != itReadEnd; ++itRead) {
        commandName = itRead->Name;
        newCommandRead = new mtsCommandReadProxy(commandName);
        if (!providedInterfaceProxy->AddCommandRead(newCommandRead)) {
            delete newCommandRead;
            bool removed = RemoveInterfaceProvided(providedInterfaceName);
            CMN_ASSERT(removed);
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: failed to add read command proxy: " << commandName << std::endl;
            return false;
        }

        // argument deserialization
        streamBuffer.str("");
        streamBuffer << itRead->ArgumentPrototypeSerialized;
        try {
            argumentPrototype = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        } catch (const std::exception &e) {
            CMN_LOG_CLASS_INIT_WARNING << "CreateInterfaceProvidedProxy: read command argument deserialization failed: " << e.what() << std::endl;
            argumentPrototype = 0;
        }

        if (!argumentPrototype) {
            CMN_LOG_CLASS_INIT_WARNING << "CreateInterfaceProvidedProxy: failed to create read command proxy: " << commandName << std::endl;
            newCommandRead->SetArgumentPrototypeSerialized(itRead->ArgumentPrototypeSerialized);
            newCommandRead->SetArgumentSupported(false);
        } else {
            newCommandRead->SetArgumentPrototype(argumentPrototype);
        }
    }

    // Create qualified read command proxies
    mtsCommandQualifiedReadProxy * newCommandQualifiedRead = 0;
    mtsCommandsQualifiedReadDescription::const_iterator itQualifiedRead = providedInterfaceDescription.CommandsQualifiedRead.begin();
    const mtsCommandsQualifiedReadDescription::const_iterator itQualifiedReadEnd = providedInterfaceDescription.CommandsQualifiedRead.end();
    for (; itQualifiedRead != itQualifiedReadEnd; ++itQualifiedRead) {
        commandName = itQualifiedRead->Name;
        newCommandQualifiedRead = new mtsCommandQualifiedReadProxy(commandName);
        if (!providedInterfaceProxy->AddCommandQualifiedRead(newCommandQualifiedRead)) {
            delete newCommandQualifiedRead;
            bool removed = RemoveInterfaceProvided(providedInterfaceName);
            CMN_ASSERT(removed);
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: failed to add qualified read command proxy: " << commandName << std::endl;
            return false;
        }

        // argument1 deserialization
        streamBuffer.str("");
        streamBuffer << itQualifiedRead->Argument1PrototypeSerialized;
        try {
            argument1Prototype = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        } catch (const std::exception &e) {
            CMN_LOG_CLASS_INIT_WARNING << "CreateInterfaceProvidedProxy: qualified read command argument 1 deserialization failed: " << e.what() << std::endl;
            argument1Prototype = 0;
        }
        // argument2 deserialization
        streamBuffer.str("");
        streamBuffer << itQualifiedRead->Argument2PrototypeSerialized;
        try {
            argument2Prototype = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        } catch (const std::exception &e) {
            CMN_LOG_CLASS_INIT_WARNING << "CreateInterfaceProvidedProxy: qualified read command argument 2 deserialization failed: " << e.what() << std::endl;
            argument2Prototype = 0;
        }

        if (!argument1Prototype || !argument2Prototype) {
            CMN_LOG_CLASS_INIT_WARNING << "CreateInterfaceProvidedProxy: failed to create qualified read command proxy: " << commandName << std::endl;
            newCommandQualifiedRead->SetArgumentPrototypeSerialized(itQualifiedRead->Argument1PrototypeSerialized,
                                                                    itQualifiedRead->Argument2PrototypeSerialized);
            newCommandQualifiedRead->SetArgumentSupported(false);
        } else {
            newCommandQualifiedRead->SetArgumentPrototype(argument1Prototype, argument2Prototype);
        }
    }

    // Create void return command proxies
    mtsCommandVoidReturnProxy * newCommandVoidReturn = 0;
    mtsCommandsVoidReturnDescription::const_iterator itVoidReturn = providedInterfaceDescription.CommandsVoidReturn.begin();
    const mtsCommandsVoidReturnDescription::const_iterator itVoidReturnEnd = providedInterfaceDescription.CommandsVoidReturn.end();
    for (; itVoidReturn != itVoidReturnEnd; ++itVoidReturn) {
        commandName = itVoidReturn->Name;
        newCommandVoidReturn = new mtsCommandVoidReturnProxy(commandName);
        if (!providedInterfaceProxy->AddCommandVoidReturn(newCommandVoidReturn)) {
            delete newCommandVoidReturn;
            bool removed = RemoveInterfaceProvided(providedInterfaceName);
            CMN_ASSERT(removed);
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: failed to add void return command proxy: " << commandName << std::endl;
            return false;
        }

        // result deserialization
        streamBuffer.str("");
        streamBuffer << itVoidReturn->ResultPrototypeSerialized;
        try {
            resultPrototype = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        } catch (const std::exception &e) {
            CMN_LOG_CLASS_INIT_WARNING << "CreateInterfaceProvidedProxy: void return command result deserialization failed: " << e.what() << std::endl;
            resultPrototype = 0;
        }

        if (!resultPrototype) {
            CMN_LOG_CLASS_INIT_WARNING << "CreateInterfaceProvidedProxy: failed to create void return command proxy: " << commandName << std::endl;
            newCommandVoidReturn->SetResultPrototypeSerialized(itVoidReturn->ResultPrototypeSerialized);
            newCommandVoidReturn->SetArgumentSupported(false);
        } else {
            newCommandVoidReturn->SetResultPrototype(resultPrototype);
        }

    }

    // Create write return command proxies
    mtsCommandWriteReturnProxy * newCommandWriteReturn = 0;
    mtsCommandsWriteReturnDescription::const_iterator itWriteReturn = providedInterfaceDescription.CommandsWriteReturn.begin();
    const mtsCommandsWriteReturnDescription::const_iterator itWriteReturnEnd = providedInterfaceDescription.CommandsWriteReturn.end();
    for (; itWriteReturn != itWriteReturnEnd; ++itWriteReturn) {
        commandName = itWriteReturn->Name;
        newCommandWriteReturn = new mtsCommandWriteReturnProxy(commandName);
        if (!providedInterfaceProxy->AddCommandWriteReturn(newCommandWriteReturn)) {
            delete newCommandWriteReturn;
            bool removed = RemoveInterfaceProvided(providedInterfaceName);
            CMN_ASSERT(removed);
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: failed to add write return command proxy: " << commandName << std::endl;
            return false;
        }

        // argument deserialization
        streamBuffer.str("");
        streamBuffer << itWriteReturn->ArgumentPrototypeSerialized;
        try {
            argumentPrototype = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        } catch (const std::exception &e) {
            CMN_LOG_CLASS_INIT_WARNING << "CreateInterfaceProvidedProxy: write return command \"" << commandName
                                       << "\", argument deserialization failed: " << e.what() << std::endl;
            argumentPrototype = 0;
        }

        // result deserialization
        streamBuffer.str("");
        streamBuffer << itWriteReturn->ResultPrototypeSerialized;
        try {
            resultPrototype = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        } catch (const std::exception &e) {
            CMN_LOG_CLASS_INIT_WARNING << "CreateInterfaceProvidedProxy: write return command \"" << commandName
                                       << "\", result deserialization failed: " << e.what() << std::endl;
            resultPrototype = 0;
        }

        if (!argumentPrototype || !resultPrototype) {
            CMN_LOG_CLASS_INIT_WARNING << "CreateInterfaceProvidedProxy: failed to create write return command proxy: " << commandName << std::endl;
            newCommandWriteReturn->SetArgumentPrototypeSerialized(itWriteReturn->ArgumentPrototypeSerialized);
            newCommandWriteReturn->SetResultPrototypeSerialized(itWriteReturn->ResultPrototypeSerialized);
            newCommandWriteReturn->SetArgumentSupported(false);
        } else {
            newCommandWriteReturn->SetArgumentPrototype(argumentPrototype);
            newCommandWriteReturn->SetResultPrototype(resultPrototype);
        }
    }

    // Create event generator proxies
    std::string eventName;

    FunctionProxyAndEventHandlerProxyMapElement * mapElement
        = FunctionProxyAndEventHandlerProxyMap.GetItem(providedInterfaceName, CMN_LOG_LEVEL_RUN_VERBOSE);
    if (!mapElement) {
        mapElement = new FunctionProxyAndEventHandlerProxyMapElement;
    }

    // Create void event generator proxies
    mtsFunctionVoid * eventVoidGeneratorProxy = 0;
    mtsMulticastCommandVoidProxy * eventMulticastCommandVoidProxy;
    mtsEventsVoidDescription::const_iterator itEventVoid = providedInterfaceDescription.EventsVoid.begin();
    const mtsEventsVoidDescription::const_iterator itEventVoidEnd = providedInterfaceDescription.EventsVoid.end();
    for (; itEventVoid != itEventVoidEnd; ++itEventVoid) {
        eventName = itEventVoid->Name;
        eventVoidGeneratorProxy = new mtsFunctionVoid();
        if (!mapElement->EventGeneratorVoidProxyMap.AddItem(eventName, eventVoidGeneratorProxy)) {
            delete eventVoidGeneratorProxy;
            bool removed = RemoveInterfaceProvided(providedInterfaceName);
            CMN_ASSERT(removed);
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: failed to create event generator proxy: " << eventName << std::endl;
            return false;
        }

        eventMulticastCommandVoidProxy = new mtsMulticastCommandVoidProxy(eventName);

        if (!providedInterfaceProxy->AddEvent(eventName, eventMulticastCommandVoidProxy)) {
            delete eventMulticastCommandVoidProxy;
            bool removed = RemoveInterfaceProvided(providedInterfaceName);
            CMN_ASSERT(removed);
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: failed to add event multicast void command proxy: " << eventName << std::endl;
            return false;
        }

        if (!eventVoidGeneratorProxy->Bind(eventMulticastCommandVoidProxy)) {
            delete eventMulticastCommandVoidProxy;
            bool removed = RemoveInterfaceProvided(providedInterfaceName);
            CMN_ASSERT(removed);
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: failed to bind with event multicast void command proxy: " << eventName << std::endl;
            return false;
        }
    }

    // Create write event generator proxies
    mtsFunctionWrite * eventWriteGeneratorProxy;
    mtsMulticastCommandWriteProxy * eventMulticastCommandWriteProxy;
    mtsEventsWriteDescription::const_iterator itEventWrite = providedInterfaceDescription.EventsWrite.begin();
    const mtsEventsWriteDescription::const_iterator itEventWriteEnd = providedInterfaceDescription.EventsWrite.end();
    for (; itEventWrite != itEventWriteEnd; ++itEventWrite) {
        eventName = itEventWrite->Name;
        eventWriteGeneratorProxy = new mtsFunctionWrite();
        if (!mapElement->EventGeneratorWriteProxyMap.AddItem(eventName, eventWriteGeneratorProxy)) {
            delete eventWriteGeneratorProxy;
            bool removed = RemoveInterfaceProvided(providedInterfaceName);
            CMN_ASSERT(removed);
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: failed to add event write generator proxy pointer: " << eventName << std::endl;
            return false;
        }

        eventMulticastCommandWriteProxy = new mtsMulticastCommandWriteProxy(eventName);

        // event argument deserialization
        streamBuffer.str("");
        streamBuffer << itEventWrite->ArgumentPrototypeSerialized;
        try {
            argumentPrototype = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        } catch (const std::exception &e) {
            CMN_LOG_CLASS_INIT_WARNING << "CreateInterfaceProvidedProxy: argument deserialization for event write generator failed: "
                                       << e.what() << std::endl;
            argumentPrototype = 0;
        }
        if (!argumentPrototype) {
            CMN_LOG_CLASS_INIT_WARNING << "CreateInterfaceProvidedProxy: failed to create event write proxy: " << eventName << std::endl;
            eventMulticastCommandWriteProxy->SetArgumentPrototypeSerialized(itEventWrite->ArgumentPrototypeSerialized);
            eventMulticastCommandWriteProxy->SetArgumentSupported(false);
        } else {
            eventMulticastCommandWriteProxy->SetArgumentPrototype(argumentPrototype);
        }

        if (!providedInterfaceProxy->AddEvent(eventName, eventMulticastCommandWriteProxy)) {
            delete eventMulticastCommandWriteProxy;
            bool removed = RemoveInterfaceProvided(providedInterfaceName);
            CMN_ASSERT(removed);
            CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProvidedProxy: failed to add event multicast write command proxy: " << eventName << std::endl;
            return false;
        }

        if (!eventWriteGeneratorProxy->Bind(eventMulticastCommandWriteProxy)) {
            delete eventMulticastCommandWriteProxy;
            bool removed = RemoveInterfaceProvided(providedInterfaceName);
            CMN_ASSERT(removed);
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
    mtsComponentInterfaceProxyServer * serverProxy = InterfaceProvidedNetworkProxies.GetItem(providedInterfaceProxyName, CMN_LOG_LEVEL_RUN_VERBOSE);
    if (!serverProxy) {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveInterfaceProvidedProxy: cannot find proxy server: " << providedInterfaceProxyName << std::endl;
        return false;
    } else {
        // Network server deactivation
        serverProxy->StopProxy();
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
        CMN_LOG_CLASS_INIT_ERROR << "CreateInterfaceProxyClient: proxy failed to start for required interface "
                                 << "\"" << requiredInterfaceProxyName << "\", \"" << serverEndpointInfo << "\", [ " << connectionID << " ]" << std::endl;
        return false;
    }

    requiredInterfaceProxy->GetLogger()->trace(
                                               "mtsComponentProxy", "required interface proxy starts: " + requiredInterfaceProxyName);

    return true;
}

bool mtsComponentProxy::IsActiveProxy(const std::string & proxyName, const bool isProxyServer) const
{
    if (isProxyServer) {
        mtsComponentInterfaceProxyServer * providedInterfaceProxy = InterfaceProvidedNetworkProxies.GetItem(proxyName, CMN_LOG_LEVEL_RUN_VERBOSE);
        if (!providedInterfaceProxy) {
            CMN_LOG_CLASS_INIT_ERROR << "IsActiveProxy: Cannot find provided interface proxy: " << proxyName << std::endl;
            return false;
        }
        return providedInterfaceProxy->IsActiveProxy();
    } else {
        mtsComponentInterfaceProxyClient * requiredInterfaceProxy = InterfaceRequiredNetworkProxies.GetItem(proxyName, CMN_LOG_LEVEL_RUN_VERBOSE);
        if (!requiredInterfaceProxy) {
            CMN_LOG_CLASS_INIT_ERROR << "IsActiveProxy: Cannot find required interface proxy: " << proxyName << std::endl;
            return false;
        }
        return requiredInterfaceProxy->IsActiveProxy();
    }
}

bool mtsComponentProxy::UpdateEventHandlerProxyID(const std::string & clientComponentName, const std::string & clientInterfaceName)
{
    // Get required interface
    mtsInterfaceRequired * interfaceRequiredProxy = GetInterfaceRequired(clientInterfaceName);
    if (!interfaceRequiredProxy) {
        CMN_LOG_CLASS_INIT_ERROR << "UpdateEventHandlerProxyID: no required interface found: " << clientInterfaceName << std::endl;
        return false;
    }

    // Get network proxy client connected to the required interface proxy
    // of which name is 'clientInterfaceName.'
    mtsComponentInterfaceProxyClient * interfaceProxyClient = InterfaceRequiredNetworkProxies.GetItem(clientInterfaceName, CMN_LOG_LEVEL_RUN_VERBOSE);
    if (!interfaceProxyClient) {
        CMN_LOG_CLASS_INIT_ERROR << "UpdateEventHandlerProxyID: no network interface proxy client found for required interface: "
                                 << clientInterfaceName << std::endl;
        return false;
    }

    // Fetch pointers of event generator proxies from the connected provided
    // interface proxy at the client side.
    mtsComponentInterfaceProxy::EventGeneratorProxyPointerSet eventGeneratorProxyPointers;
    if (!interfaceProxyClient->SendFetchEventGeneratorProxyPointers(clientComponentName,
                                                                    clientInterfaceName,
                                                                    eventGeneratorProxyPointers))
        {
            CMN_LOG_CLASS_INIT_ERROR << "UpdateEventHandlerProxyID: failed to fetch event generator proxy pointers: "
                                     << clientComponentName << ":" << clientInterfaceName << std::endl;
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
                                             const std::string & serverInterfaceName, const std::string & clientInterfaceName)
{
    // User connection id as client id
    const unsigned int clientID = connectionID;

    // Get an instance of network proxy server that serves the provided interface.
    mtsComponentInterfaceProxyServer * interfaceProxyServer =
        InterfaceProvidedNetworkProxies.GetItem(serverInterfaceName, CMN_LOG_LEVEL_RUN_VERBOSE);
    if (!interfaceProxyServer) {
        CMN_LOG_CLASS_INIT_ERROR << "UpdateCommandProxyID: no network interface proxy server found: " << serverInterfaceName << std::endl;
        return false;
    }

    // Fetch function proxy pointers from a connected required interface proxy
    // at server side, which will be used to update ids of command proxies'.
    mtsComponentInterfaceProxy::FunctionProxyPointerSet functionProxyPointers;
    if (!interfaceProxyServer->SendFetchFunctionProxyPointers(
                                                              connectionID, clientInterfaceName, functionProxyPointers))
        {
            CMN_LOG_CLASS_INIT_ERROR << "UpdateCommandProxyID: failed to fetch function proxy pointers for connection id: " << connectionID << std::endl;
            return false;
        }

    // Get a provided interface proxy instance of which command proxies are going
    // to be updated.
    mtsInterfaceProvided * originalInterface = GetInterfaceProvided(serverInterfaceName);
    if (!originalInterface) {
        CMN_LOG_CLASS_INIT_ERROR << "UpdateCommandProxyID: failed to get provided interface: "
                                 << serverInterfaceName << std::endl;
        return false;
    }

    mtsInterfaceProvided * endUserInterface = originalInterface->FindEndUserInterfaceByName(clientInterfaceName);
    if (!endUserInterface) {
        CMN_LOG_CLASS_INIT_ERROR << "UpdateCommandProxyID: failed to get end user provided interface: "
                                 << clientInterfaceName << std::endl;
        return false;
    }

    // Set command proxy IDs in the provided interface proxy as the
    // function proxies' pointers fetched from server process.

    // Void command
    mtsCommandVoidProxy * commandVoid = 0;
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
    mtsCommandWriteProxy * commandWrite = 0;
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
    mtsCommandReadProxy * commandRead = 0;
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
    mtsCommandQualifiedReadProxy * commandQualifiedRead = 0;
    mtsComponentInterfaceProxy::FunctionProxySequence::const_iterator itQualifiedRead = functionProxyPointers.FunctionQualifiedReadProxies.begin();
    const mtsComponentInterfaceProxy::FunctionProxySequence::const_iterator itQualifiedReadEnd = functionProxyPointers.FunctionQualifiedReadProxies.end();
    for (; itQualifiedRead != itQualifiedReadEnd; ++itQualifiedRead) {
        commandQualifiedRead = dynamic_cast<mtsCommandQualifiedReadProxy*>(endUserInterface->GetCommandQualifiedRead(itQualifiedRead->Name));
        if (!commandQualifiedRead) {
            CMN_LOG_CLASS_INIT_ERROR << "UpdateCommandProxyID: failed to update command qualified read proxy id: "
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

    // VoidReturn command
    mtsCommandVoidReturnProxy * commandVoidReturn = 0;
    mtsComponentInterfaceProxy::FunctionProxySequence::const_iterator itVoidReturn = functionProxyPointers.FunctionVoidReturnProxies.begin();
    const mtsComponentInterfaceProxy::FunctionProxySequence::const_iterator itVoidReturnEnd = functionProxyPointers.FunctionVoidReturnProxies.end();
    for (; itVoidReturn != itVoidReturnEnd; ++itVoidReturn) {
        commandVoidReturn = dynamic_cast<mtsCommandVoidReturnProxy*>(endUserInterface->GetCommandVoidReturn(itVoidReturn->Name));
        if (!commandVoidReturn) {
            CMN_LOG_CLASS_INIT_ERROR << "UpdateCommandProxyID: failed to update command void return proxy id: "
                                     << itVoidReturn->Name << std::endl;
            return false;
        }
        // Set client ID and network proxy
        if (!commandVoidReturn->SetNetworkProxy(interfaceProxyServer, clientID)) {
            CMN_LOG_CLASS_INIT_ERROR << "UpdateCommandProxyID: failed to set network proxy for void return command: "
                                     << commandVoidReturn->GetName() << std::endl;
            return false;
        }
        // Set command qualified read proxy's id and enable this command
        commandVoidReturn->SetCommandID(itVoidReturn->FunctionProxyId);
        commandVoidReturn->Enable();
    }

    // WriteReturn command
    mtsCommandWriteReturnProxy * commandWriteReturn = 0;
    mtsComponentInterfaceProxy::FunctionProxySequence::const_iterator itWriteReturn = functionProxyPointers.FunctionWriteReturnProxies.begin();
    const mtsComponentInterfaceProxy::FunctionProxySequence::const_iterator itWriteReturnEnd = functionProxyPointers.FunctionWriteReturnProxies.end();
    for (; itWriteReturn != itWriteReturnEnd; ++itWriteReturn) {
        commandWriteReturn = dynamic_cast<mtsCommandWriteReturnProxy*>(endUserInterface->GetCommandWriteReturn(itWriteReturn->Name));
        if (!commandWriteReturn) {
            CMN_LOG_CLASS_INIT_ERROR << "UpdateCommandProxyID: failed to update command write return proxy id: "
                                     << itWriteReturn->Name << std::endl;
            return false;
        }
        // Set client ID and network proxy
        if (!commandWriteReturn->SetNetworkProxy(interfaceProxyServer, clientID)) {
            CMN_LOG_CLASS_INIT_ERROR << "UpdateCommandProxyID: failed to set network proxy for write return command: "
                                     << commandWriteReturn->GetName() << std::endl;
            return false;
        }
        // Set command qualified read proxy's id and enable this command
        commandWriteReturn->SetCommandID(itWriteReturn->FunctionProxyId);
        commandWriteReturn->Enable();
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
    FunctionProxyAndEventHandlerProxyMapElement * mapElement = FunctionProxyAndEventHandlerProxyMap.GetItem(requiredInterfaceName, CMN_LOG_LEVEL_RUN_VERBOSE);
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
        function.FunctionProxyId = reinterpret_cast<mtsCommandIDType>(itVoid->second);
        functionProxyPointers.FunctionVoidProxies.push_back(function);
    }

    // Write function proxy
    FunctionWriteProxyMapType::const_iterator itWrite = mapElement->FunctionWriteProxyMap.begin();
    const FunctionWriteProxyMapType::const_iterator itWriteEnd = mapElement->FunctionWriteProxyMap.end();
    for (; itWrite != itWriteEnd; ++itWrite) {
        function.Name = itWrite->first;
        function.FunctionProxyId = reinterpret_cast<mtsCommandIDType>(itWrite->second);
        functionProxyPointers.FunctionWriteProxies.push_back(function);
    }

    // Read function proxy
    FunctionReadProxyMapType::const_iterator itRead = mapElement->FunctionReadProxyMap.begin();
    const FunctionReadProxyMapType::const_iterator itReadEnd = mapElement->FunctionReadProxyMap.end();
    for (; itRead != itReadEnd; ++itRead) {
        function.Name = itRead->first;
        function.FunctionProxyId = reinterpret_cast<mtsCommandIDType>(itRead->second);
        functionProxyPointers.FunctionReadProxies.push_back(function);
    }

    // QualifiedRead function proxy
    FunctionQualifiedReadProxyMapType::const_iterator itQualifiedRead = mapElement->FunctionQualifiedReadProxyMap.begin();
    const FunctionQualifiedReadProxyMapType::const_iterator itQualifiedReadEnd = mapElement->FunctionQualifiedReadProxyMap.end();
    for (; itQualifiedRead != itQualifiedReadEnd; ++itQualifiedRead) {
        function.Name = itQualifiedRead->first;
        function.FunctionProxyId = reinterpret_cast<mtsCommandIDType>(itQualifiedRead->second);
        functionProxyPointers.FunctionQualifiedReadProxies.push_back(function);
    }

    // VoidReturn function proxy
    FunctionVoidReturnProxyMapType::const_iterator itVoidReturn = mapElement->FunctionVoidReturnProxyMap.begin();
    const FunctionVoidReturnProxyMapType::const_iterator itVoidReturnEnd = mapElement->FunctionVoidReturnProxyMap.end();
    for (; itVoidReturn != itVoidReturnEnd; ++itVoidReturn) {
        function.Name = itVoidReturn->first;
        function.FunctionProxyId = reinterpret_cast<mtsCommandIDType>(itVoidReturn->second);
        functionProxyPointers.FunctionVoidReturnProxies.push_back(function);
    }

    // WriteReturn function proxy
    FunctionWriteReturnProxyMapType::const_iterator itWriteReturn = mapElement->FunctionWriteReturnProxyMap.begin();
    const FunctionWriteReturnProxyMapType::const_iterator itWriteReturnEnd = mapElement->FunctionWriteReturnProxyMap.end();
    for (; itWriteReturn != itWriteReturnEnd; ++itWriteReturn) {
        function.Name = itWriteReturn->first;
        function.FunctionProxyId = reinterpret_cast<mtsCommandIDType>(itWriteReturn->second);
        functionProxyPointers.FunctionWriteReturnProxies.push_back(function);
    }

    return true;
}


bool mtsComponentProxy::GetEventGeneratorProxyPointer(const std::string & clientComponentName,
                                                      const std::string & requiredInterfaceName,
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
    mtsInterfaceProvided * providedInterface = const_cast<mtsInterfaceProvided*>(requiredInterface->GetConnectedInterface());
    if (!providedInterface) {
        CMN_LOG_CLASS_INIT_ERROR << "GetEventGeneratorProxyPointer: failed to get connected provided interface: "
                                 << clientComponentName << ":" << requiredInterfaceName << std::endl;
        return false;
    }

    mtsComponentInterfaceProxy::EventGeneratorProxyElement element;
    mtsCommandBase * eventGenerator = 0;

    std::vector<std::string> namesOfEventHandlersVoid = requiredInterface->GetNamesOfEventHandlersVoid();
    for (unsigned int i = 0; i < namesOfEventHandlersVoid.size(); ++i) {
        element.Name = namesOfEventHandlersVoid[i];
        eventGenerator = providedInterface->GetEventVoid(element.Name);
        if (!eventGenerator) {
            CMN_LOG_CLASS_INIT_ERROR << "GetEventGeneratorProxyPointer: no event void generator found: " << element.Name << std::endl;
            return false;
        }
        element.EventGeneratorProxyId = reinterpret_cast<mtsCommandIDType>(eventGenerator);
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
        element.EventGeneratorProxyId = reinterpret_cast<mtsCommandIDType>(eventGenerator);
        eventGeneratorProxyPointers.EventGeneratorWriteProxies.push_back(element);
    }

    return true;
}

std::string mtsComponentProxy::GetInterfaceProvidedUserName(
                                                            const std::string & processName, const std::string & componentName)
{
    return std::string(processName + ":" + componentName);
}

bool mtsComponentProxy::AddConnectionInformation(const std::string & serverInterfaceName, const ConnectionIDType connectionID)
{
    mtsComponentInterfaceProxyServer * interfaceProxyServer = InterfaceProvidedNetworkProxies.GetItem(serverInterfaceName, CMN_LOG_LEVEL_RUN_VERBOSE);
    if (!interfaceProxyServer) {
        CMN_LOG_CLASS_INIT_ERROR << "AddConnectionInformation: no server interface proxy found: " << serverInterfaceName << std::endl;
        return false;
    }

    return interfaceProxyServer->AddConnectionInformation(connectionID);
}
