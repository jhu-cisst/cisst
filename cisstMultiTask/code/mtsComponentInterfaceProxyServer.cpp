/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2010-01-12

  (C) Copyright 2010-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaSleep.h>

#include "mtsProxyConfig.h"
#if IMPROVE_ICE_THREADING
#include <cisstOSAbstraction/osaThreadSignal.h>
#endif

#include "mtsComponentProxy.h"
#include "mtsComponentInterfaceProxyServer.h"
#include "mtsComponentInterfaceProxyClient.h"
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsManagerGlobal.h>

std::string mtsComponentInterfaceProxyServer::InterfaceCommunicatorID = "InterfaceCommunicator";
std::string mtsComponentInterfaceProxyServer::ConnectionIDKey = "InterfaceConnectionID";
unsigned int mtsComponentInterfaceProxyServer::InstanceCounter = 0;

mtsComponentInterfaceProxyServer::mtsComponentInterfaceProxyServer(
    const std::string & adapterName, const std::string & communicatorID)
    : BaseServerType("config.server", adapterName, communicatorID)
#if IMPROVE_ICE_THREADING
      , IceThreadInitEvent(0)
#endif
{
    ProxyName = "ComponentInterfaceProxyClient";

#if IMPROVE_ICE_THREADING
    IceThreadInitEvent = new osaThreadSignal;
#endif
}

mtsComponentInterfaceProxyServer::~mtsComponentInterfaceProxyServer()
{
    StopProxy();

#if IMPROVE_ICE_THREADING
    delete IceThreadInitEvent;
#endif
}

//-----------------------------------------------------------------------------
//  Proxy Start-up
//-----------------------------------------------------------------------------
bool mtsComponentInterfaceProxyServer::StartProxy(mtsComponentProxy * proxyOwner)
{
    // Initialize Ice object.
    IceInitialize();

    if (!InitSuccessFlag) {
        LogError(mtsComponentInterfaceProxyServer, "ICE proxy server Initialization failed");
        return false;
    }

    // Set the owner and name of this proxy object
    std::string suffix = "On";
    mtsManagerLocal * managerLocal = mtsManagerLocal::GetInstance();
    suffix += managerLocal->GetProcessName();
    SetProxyOwner(proxyOwner, suffix);

    // Create a worker thread here and returns immediately.
    ThreadArgumentsInfo.Proxy = this;
    ThreadArgumentsInfo.Runner = mtsComponentInterfaceProxyServer::Runner;

    // Set a short name of this thread as CIPS which means "Component Interface
    // Proxy Server." Such a condensed naming rule is required because a total
    // number of characters in a thread name is sometimes limited to a small
    // number (e.g. LINUX RTAI).
    std::stringstream ss;
    ss << "CIPS" << mtsComponentInterfaceProxyServer::InstanceCounter++;
    std::string threadName = ss.str();

    // Create worker thread. Note that it is created but is not yet running.
    WorkerThread.Create<ProxyWorker<mtsComponentProxy>, ThreadArguments<mtsComponentProxy>*>(
        &ProxyWorkerInfo, &ProxyWorker<mtsComponentProxy>::Run, &ThreadArgumentsInfo, threadName.c_str());

#if IMPROVE_ICE_THREADING
    // Wait for Ice thread to start
    IceThreadInitEvent->Wait();
#endif
    return true;
}

Ice::ObjectPtr mtsComponentInterfaceProxyServer::CreateServant(void)
{
    Sender = new ComponentInterfaceServerI(IceCommunicator, IceLogger, this);

    return Sender;
}

void mtsComponentInterfaceProxyServer::RemoveServant(void)
{
    Sender->Stop();

    // MJ TDOO: iterate all clients and stop/clean-up all proxies
    // CloseAllClients() - defined in mtsProxyBaseServer.h
    //Sender = 0;
}

void mtsComponentInterfaceProxyServer::StartServer()
{
    Sender->Start();

    ChangeProxyState(PROXY_STATE_ACTIVE);
#if IMPROVE_ICE_THREADING
    IceThreadInitEvent->Raise();
#endif

    // This is a blocking call that should run in a different thread.
    IceCommunicator->waitForShutdown();
}

void mtsComponentInterfaceProxyServer::Runner(ThreadArguments<mtsComponentProxy> * arguments)
{
    mtsComponentInterfaceProxyServer * ProxyServer =
        dynamic_cast<mtsComponentInterfaceProxyServer*>(arguments->Proxy);
    if (!ProxyServer) {
        CMN_LOG_RUN_ERROR << "mtsComponentInterfaceProxyServer: failed to create a proxy server." << std::endl;
        return;
    }

    ProxyServer->GetLogger()->trace("mtsComponentInterfaceProxyServer", "proxy server starts");

    try {
        ProxyServer->StartServer();
    } catch (const Ice::Exception& e) {
        std::string error("mtsComponentInterfaceProxyServer: ");
        error += e.what();
        ProxyServer->GetLogger()->error(error);
    } catch (...) {
        std::string error("mtsComponentInterfaceProxyServer: exception at mtsComponentInterfaceProxyServer::Runner()");
        ProxyServer->GetLogger()->error(error);
    }

    ProxyServer->GetLogger()->trace("mtsComponentInterfaceProxyServer", "Proxy server terminates");
    ProxyServer->StopProxy();
}

void mtsComponentInterfaceProxyServer::StopProxy()
{
    if (!IsActiveProxy()) return;

    try {
        BaseServerType::StopProxy();
        Sender->Stop();
    } catch (const Ice::Exception& e) {
        std::string error("mtsComponentInterfaceProxyServer: ");
        error += e.what();
        LogError(mtsComponentInterfaceProxyServer, error);
    }

    IceGUID = "";

    LogPrint(mtsComponentInterfaceProxyServer, "Stopped component interface proxy server");
}

bool mtsComponentInterfaceProxyServer::OnClientDisconnect(const ClientIDType clientID)
{
    if (!IsActiveProxy()) return true;

    LogWarning(mtsComponentInterfaceProxyServer, "COMP.INT.SERVER detected COMP.INT.CLIENT DISCONNECTION: \"" << clientID << "\"");

    // Get network proxy client serving the client with the clientID
    ComponentInterfaceClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsComponentInterfaceProxyServer, "OnClientDisconnect: no client proxy found with client id: \"" << clientID <<"\"");
        return false;
    }

    // Remove client from client list to prevent further requests from network layer
    if (!BaseServerType::RemoveClientByClientID(clientID)) {
        LogError(mtsComponentInterfaceProxyServer, "OnClientDisconnect: failed to remove client from client map: \"" << clientID <<"\"");
        return false;
    }

    // Get list of connection IDs related to this proxy
    ClientConnectionIDMapType::const_iterator it = ClientConnectionIDMap.find(clientID);
    if (it == ClientConnectionIDMap.end()) {
        LogError(mtsComponentInterfaceProxyServer, "OnClientDisconnect: no connection information found: \"" << clientID <<"\"");
        return false;
    }

    const ConnectionIDType connectionID = it->second;

    mtsManagerLocal * localManager = mtsManagerLocal::GetInstance();
    if (!localManager->Disconnect(connectionID)) {
        LogWarning(mtsComponentInterfaceProxyServer, "OnClientDisconnect: failed to request disconnection: connection id [ " << connectionID << " ]" << std::endl);
        return false;
    } else {
        LogPrint(mtsComponentInterfaceProxyServer, "OnClientDisconnect: requested disconnection, connection id [ " << connectionID << " ]" << std::endl);
        return true;
    }
}

void mtsComponentInterfaceProxyServer::MonitorConnections(void)
{
    BaseServerType::Monitor();
}

mtsComponentInterfaceProxyServer::ComponentInterfaceClientProxyType * mtsComponentInterfaceProxyServer::GetNetworkProxyClient(const ClientIDType clientID)
{
    ComponentInterfaceClientProxyType * clientProxy = GetClientByClientID(clientID);
    if (!clientProxy) {
        LogError(mtsComponentInterfaceProxyServer, "GetNetworkProxyClient: no client proxy connected with client id: " << clientID);
        return 0;
    }

    // Check if this network proxy server is active. We don't need to check if
    // a proxy client is still active since any disconnection or inactive proxy
    // has already been detected and taken care of.
    return (IsActiveProxy() ? clientProxy : 0);
}

//-------------------------------------------------------------------------
//  Event Handlers (Client -> Server)
//-------------------------------------------------------------------------
void mtsComponentInterfaceProxyServer::ReceiveTestMessageFromClientToServer(const IceConnectionIDType & iceConnectionID, const std::string & str)
{
    const ClientIDType clientID = GetClientID(iceConnectionID);

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyServer,
             "ReceiveTestMessageFromClientToServer: "
             << "\n..... ConnectionID: " << iceConnectionID
             << "\n..... Message: " << str);
#endif

    std::cout << "Server: received from Client " << clientID << ": " << str << std::endl;
}

bool mtsComponentInterfaceProxyServer::ReceiveAddClient(
    const IceConnectionIDType & iceConnectionID, const std::string & connectingProxyName,
    const unsigned int providedInterfaceProxyInstanceID, ComponentInterfaceClientProxyType & clientProxy)
{
    if (!AddProxyClient(connectingProxyName, providedInterfaceProxyInstanceID, iceConnectionID, clientProxy)) {
        LogError(mtsComponentInterfaceProxyServer, "ReceiveAddClient: failed to add proxy client: " << connectingProxyName);
        return false;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyServer,
             "ReceiveAddClient: added proxy client: "
             << "\n..... ConnectionID: " << iceConnectionID
             << "\n..... Proxy Name: " << connectingProxyName
             << "\n..... ClientID: " << providedInterfaceProxyInstanceID);
#endif

    return true;
}

bool mtsComponentInterfaceProxyServer::ReceiveFetchEventGeneratorProxyPointers(
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    const IceConnectionIDType & iceConnectionID,
#else
    const IceConnectionIDType & CMN_UNUSED(iceConnectionID),
#endif
    const std::string & clientComponentName,
    const std::string & requiredInterfaceName,
    mtsComponentInterfaceProxy::EventGeneratorProxyPointerSet & eventGeneratorProxyPointers)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyServer,
             "ReceiveFetchEventGeneratorProxyPointers: "
             << "\n..... ConnectionID: " << iceConnectionID
             << "\n..... Client component name: " << clientComponentName
             << "\n..... Required interface name: " << requiredInterfaceName);
#endif

    return ProxyOwner->GetEventGeneratorProxyPointer(clientComponentName, requiredInterfaceName, eventGeneratorProxyPointers);
}

bool mtsComponentInterfaceProxyServer::AddPerCommandSerializer(const mtsCommandIDType commandID, mtsProxySerializer * serializer)
{
    PerCommandSerializerMapType::const_iterator it = PerCommandSerializerMap.find(commandID);
    if (!serializer || it != PerCommandSerializerMap.end()) {
        LogError(mtsComponentInterfaceProxyServer, "AddPerCommandSerializer: failed to add per-command serializer" << std::endl);
        return false;
    }

    PerCommandSerializerMap[commandID] = serializer;

    return true;
}

bool mtsComponentInterfaceProxyServer::AddConnectionInformation(const ConnectionIDType connectionID)
{
    // MJ: use connection id as client id
    ClientConnectionIDMapType::const_iterator it = ClientConnectionIDMap.find(connectionID);
    if (it != ClientConnectionIDMap.end()) {
        LogError(mtsComponentInterfaceProxyServer, "OnClientDisconnect: no connection information found for connection id [ " << connectionID <<" ]");
        return false;
    }

    ClientConnectionIDMap.insert(std::make_pair(connectionID, connectionID));

    return true;
}

void mtsComponentInterfaceProxyServer::ReceiveExecuteEventVoid(const mtsCommandIDType commandID)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyServer, "ReceiveExecuteEventVoid: " << commandID);
#endif

    mtsMulticastCommandVoidProxy * eventVoidGeneratorProxy = reinterpret_cast<mtsMulticastCommandVoidProxy*>(commandID);
    if (!eventVoidGeneratorProxy) {
        LogError(mtsComponentInterfaceProxyServer, "ReceiveExecuteEventVoid: invalid proxy id of event void: " << commandID);
        return;
    }

    eventVoidGeneratorProxy->Execute(MTS_NOT_BLOCKING);
}

void mtsComponentInterfaceProxyServer::ReceiveExecuteEventWriteSerialized(const mtsCommandIDType commandID, const std::string & serializedArgument)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyServer, "ReceiveExecuteEventWriteSerialized: " << commandID << ", " << serializedArgument.size() << " bytes");
#endif


    mtsMulticastCommandWriteProxy * eventWriteGeneratorProxy = reinterpret_cast<mtsMulticastCommandWriteProxy*>(commandID);
    if (!eventWriteGeneratorProxy) {
        LogError(mtsComponentInterfaceProxyServer, "ReceiveExecuteEventWriteSerialized: invalid proxy id of event write: " << commandID);
        return;
    }

    // Get a per-command serializer.
    mtsProxySerializer * deserializer = eventWriteGeneratorProxy->GetSerializer();
    mtsGenericObject * argument = deserializer->DeSerialize(serializedArgument);
    if (!argument) {
        LogError(mtsComponentInterfaceProxyServer, "ReceiveExecuteEventWriteSerialized: Deserialization failed");
        return;
    }

    eventWriteGeneratorProxy->Execute(*argument, MTS_NOT_BLOCKING);

    // Release memory internally created by deserializer
    delete argument;
}

void mtsComponentInterfaceProxyServer::ReceiveExecuteEventReturnSerialized(const mtsCommandIDType commandID,
                                                                           const mtsObjectIDType resultAddress,
                                                                           const std::string & resultSerialized)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyServer, "ReceiveExecuteEventReturnSerialized: " << commandID << ", " << resultSerialized.size() << " bytes");
#endif
    mtsCommandBase * basePointer = reinterpret_cast<mtsCommandBase*>(commandID);
    mtsMulticastCommandVoidProxy * eventVoidGeneratorProxy = dynamic_cast<mtsMulticastCommandVoidProxy*>(basePointer);
    if (!eventVoidGeneratorProxy) {
        LogError(mtsComponentInterfaceProxyServer, "ReceiveExecuteEventReturnSerialized: invalid proxy id of event void: " << commandID);
        return;
    }
    // get a per-command serializer and deserialize where placeholder is
    mtsProxySerializer * deserializer = eventVoidGeneratorProxy->GetSerializer();
    mtsGenericObject * resultPointer = reinterpret_cast<mtsGenericObject *>(resultAddress);
    deserializer->DeSerialize(resultSerialized, *resultPointer);

    // wake up caller
    eventVoidGeneratorProxy->Execute(MTS_NOT_BLOCKING);
}

//-------------------------------------------------------------------------
//  Event Generators (Event Sender) : Server -> Client
//-------------------------------------------------------------------------
void mtsComponentInterfaceProxyServer::SendTestMessageFromServerToClient(const std::string & str)
{
    if (!IsActiveProxy()) return;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyServer, ">>>>> SEND: SendMessageFromServerToClient");
#endif

    // iterate client map -> send message to ALL clients (broadcasts)
    ComponentInterfaceClientProxyType * clientProxy;
    ClientIDMapType::iterator it = ClientIDMap.begin();
    ClientIDMapType::const_iterator itEnd = ClientIDMap.end();
    for (; it != itEnd; ++it) {
        clientProxy = &(it->second.ClientProxy);
        if (!clientProxy->get()) continue;
        try {
            (*clientProxy)->TestMessageFromServerToClient(str);
        } catch (const ::Ice::Exception & ex) {
            std::cerr << "Error: " << ex << std::endl;
            continue;
        }
    }
}

bool mtsComponentInterfaceProxyServer::SendFetchFunctionProxyPointers(
    const ClientIDType clientID, const std::string & requiredInterfaceName,
    mtsComponentInterfaceProxy::FunctionProxyPointerSet & functionProxyPointers)
{
    if (!IsActiveProxy()) return false;

    ComponentInterfaceClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy || !clientProxy->get()) {
        LogError(mtsComponentInterfaceProxyServer, "SendFetchFunctionProxyPointers: no proxy client found or inactive proxy: " << clientID);
        return false;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyServer, ">>>>> SEND: SendFetchFunctionProxyPointers: client id: " << clientID);
#endif

    try {
        return (*clientProxy)->FetchFunctionProxyPointers(requiredInterfaceName, functionProxyPointers);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsComponentInterfaceProxyServer, "SendFetchFunctionProxyPointers: network exception: " << ex);
        OnClientDisconnect(clientID);
        return false;
    }
}


bool mtsComponentInterfaceProxyServer::SendExecuteCommandVoid(const ClientIDType clientID, const mtsCommandIDType commandID,
                                                              const mtsBlockingType blocking,
                                                              mtsExecutionResult & executionResult)
{
    if (!IsActiveProxy()) return false;

    ComponentInterfaceClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy || !clientProxy->get()) {
        LogError(mtsComponentInterfaceProxyServer, "SendExecuteCommandVoid: no proxy client found or inactive proxy: " << clientID);
        return false;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyServer, ">>>>> SEND: SendExecuteCommandVoid: " << commandID << ", "
        << (blocking == MTS_BLOCKING ? "BLOCKING" : "NON-BLOCKING"));
#endif

    try {
        Ice::Byte executionResultIce;
        (*clientProxy)->ExecuteCommandVoid(commandID,
                                           (blocking == MTS_BLOCKING),
                                           executionResultIce);
        executionResult = static_cast<mtsExecutionResult::Enum>(executionResultIce);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsComponentInterfaceProxyServer, "SendExecuteCommandVoid: network exception: " << ex);
        OnClientDisconnect(clientID);
        executionResult = mtsExecutionResult::NETWORK_ERROR;
        return false;
    }

    return true;
}


bool mtsComponentInterfaceProxyServer::SendExecuteCommandWriteSerialized(const ClientIDType clientID, const mtsCommandIDType commandID,
                                                                         const mtsBlockingType blocking, mtsExecutionResult & executionResult,
                                                                         const mtsGenericObject & argument)
{
    if (!IsActiveProxy()) return false;

    ComponentInterfaceClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy || !clientProxy->get()) {
        LogError(mtsComponentInterfaceProxyServer, "SendExecuteCommandWriteSerialized: no proxy client found or inactive proxy: "
            << clientID << ", " << (blocking == MTS_BLOCKING ? "BLOCKING" : "NON-BLOCKING"));
        return false;
    }

    // Get per-command serializer
    mtsProxySerializer * serializer = PerCommandSerializerMap[commandID];
    if (!serializer) {
        LogError(mtsComponentInterfaceProxyServer, "SendExecuteCommandWriteSerialized: cannot find per-command serializer");
        return false;
    }

    // Serialize the argument
    std::string serializedArgument;
    serializer->Serialize(argument, serializedArgument);
    if (serializedArgument.empty()) {
        LogError(mtsComponentInterfaceProxyServer, "SendExecuteCommandWriteSerialized: serialization failure: " << argument.ToString());
        return false;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyServer, ">>>>> SEND: SendExecuteCommandWriteSerialized: " << commandID << ", " << serializedArgument.size() << " bytes"
             << ", " << (blocking == MTS_BLOCKING ? "BLOCKING" : "NON-BLOCKING"));
#endif

    try {
        Ice::Byte executionResultIce;
        (*clientProxy)->ExecuteCommandWriteSerialized(commandID,
                                                      serializedArgument,
                                                      (blocking == MTS_BLOCKING),
                                                      executionResultIce);
        executionResult = static_cast<mtsExecutionResult::Enum>(executionResultIce);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsComponentInterfaceProxyServer, "SendExecuteCommandWriteSerialized: network exception: " << ex);
        OnClientDisconnect(clientID);
        executionResult = mtsExecutionResult::NETWORK_ERROR;
        return false;
    }

    return true;
}


bool mtsComponentInterfaceProxyServer::SendExecuteCommandReadSerialized(const ClientIDType clientID, const mtsCommandIDType commandID,
                                                                        mtsExecutionResult & executionResult,
                                                                        mtsGenericObject & argument)
{
    if (!IsActiveProxy()) return false;

    ComponentInterfaceClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy || !clientProxy->get()) return false;

    // Argument placeholder of which value is set by the read command
    std::string serializedArgument;

    // Execute read command
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyServer, ">>>>> SEND: SendExecuteCommandReadSerialized: " << commandID);
#endif

    try {
        Ice::Byte executionResultIce;
        (*clientProxy)->ExecuteCommandReadSerialized(commandID,
                                                     serializedArgument,
                                                     executionResultIce);
        executionResult = static_cast<mtsExecutionResult::Enum>(executionResultIce);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsComponentInterfaceProxyServer, "SendExecuteCommandReadSerialized: network exception: " << ex);
        OnClientDisconnect(clientID);
        executionResult = mtsExecutionResult::NETWORK_ERROR;
        return false;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyServer, ">>>>> SEND: SendExecuteCommandReadSerialized: received " << serializedArgument.size() << " bytes");
#endif

    // Deserialize only if the command succeeded
    if (executionResult.GetResult() == mtsExecutionResult::COMMAND_SUCCEEDED) {
        // Deserialize the argument returned
        mtsProxySerializer * deserializer = PerCommandSerializerMap[commandID];
        if (!deserializer) {
            LogError(mtsComponentInterfaceProxyServer, "SendExecuteCommandReadSerialized: cannot find per-command serializer");
            return false;
        }
        deserializer->DeSerialize(serializedArgument, argument);
    }
    else
        LogError(mtsComponentInterfaceProxyServer, "SendExecuteCommandReadSerialized: command execution failed, " << executionResult);

    return true;
}


bool mtsComponentInterfaceProxyServer::SendExecuteCommandQualifiedReadSerialized(const ClientIDType clientID, const mtsCommandIDType commandID,
                                                                                 mtsExecutionResult & executionResult,
                                                                                 const mtsGenericObject & argumentIn, mtsGenericObject & argumentOut)
{
    if (!IsActiveProxy()) return false;

    ComponentInterfaceClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy || !clientProxy->get()) {
        LogError(mtsComponentInterfaceProxyServer, "SendExecuteCommandQualifiedReadSerialized: no proxy client found or inactive proxy: " << clientID);
        return false;
    }

    // Get per-command serializer
    mtsProxySerializer * serializer = PerCommandSerializerMap[commandID];
    if (!serializer) {
        LogError(mtsComponentInterfaceProxyServer, "SendExecuteCommandQualifiedReadSerialized: cannot find per-command serializer");
        return false;
    }

    // Serialize the input argument
    std::string serializedArgumentIn;
    serializer->Serialize(argumentIn, serializedArgumentIn);
    if (serializedArgumentIn.empty()) {
        LogError(mtsComponentInterfaceProxyServer, "SendExecuteCommandQualifiedReadSerialized: serialization failure: " << argumentIn.ToString());
        return false;
    }

    // Argument placeholder of which value is set by the qualified read command
    std::string serializedArgumentOut;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyServer, ">>>>> SEND: SendExecuteCommandQualifiedReadSerialized: " << commandID << ", " << serializedArgumentIn.size() << " bytes");
#endif

    try {
        Ice::Byte executionResultIce;
        (*clientProxy)->ExecuteCommandQualifiedReadSerialized(commandID,
                                                              serializedArgumentIn, serializedArgumentOut,
                                                              executionResultIce);
        executionResult = static_cast<mtsExecutionResult::Enum>(executionResultIce);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsComponentInterfaceProxyServer, "SendExecuteCommandQualifiedReadSerialized: network exception: " << ex);
        OnClientDisconnect(clientID);
        executionResult = mtsExecutionResult::NETWORK_ERROR;
        return false;
    }

    // Deserialize
    serializer->DeSerialize(serializedArgumentOut, argumentOut);

    return true;
}


bool mtsComponentInterfaceProxyServer::SendExecuteCommandVoidReturnSerialized(const ClientIDType clientID, const mtsCommandIDType commandID,
                                                                              mtsExecutionResult & executionResult,
                                                                              mtsGenericObject & result)
{
    if (!IsActiveProxy()) return false;

    ComponentInterfaceClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy || !clientProxy->get()) {
        LogError(mtsComponentInterfaceProxyServer, "SendExecuteCommandVoidReturnSerialized: no proxy client found or inactive proxy: " << clientID);
        return false;
    }

    // Get per-command serializer
    mtsProxySerializer * serializer = PerCommandSerializerMap[commandID];
    if (!serializer) {
        LogError(mtsComponentInterfaceProxyServer, "SendExecuteCommandVoidReturnSerialized: cannot find per-command serializer");
        return false;
    }

    // Argument placeholder of which value is set by the void return command
    std::string serializedResult;

    // Send address of result, only used by queued commands
    mtsObjectIDType resultAddress = reinterpret_cast<mtsObjectIDType>(&result);

    try {
        Ice::Byte executionResultIce;
        (*clientProxy)->ExecuteCommandVoidReturnSerialized(commandID,
                                                           resultAddress,
                                                           serializedResult,
                                                           executionResultIce);

        executionResult = static_cast<mtsExecutionResult::Enum>(executionResultIce);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsComponentInterfaceProxyServer, "SendExecuteCommandVoidReturnSerialized: network exception: " << ex);
        OnClientDisconnect(clientID);
        executionResult = mtsExecutionResult::NETWORK_ERROR;
        return false;
    }

    // Deserialize only if the command succeeded
    if (executionResult.GetResult() == mtsExecutionResult::COMMAND_SUCCEEDED) {
        serializer->DeSerialize(serializedResult, result);
    }

    return true;
}


bool mtsComponentInterfaceProxyServer::SendExecuteCommandWriteReturnSerialized(const ClientIDType clientID, const mtsCommandIDType commandID,
                                                                               mtsExecutionResult & executionResult,
                                                                               const mtsGenericObject & argument,
                                                                               mtsGenericObject & result)
{
    if (!IsActiveProxy()) return false;

    ComponentInterfaceClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy || !clientProxy->get()) {
        LogError(mtsComponentInterfaceProxyServer, "SendExecuteCommandWriteReturnSerialized: no proxy client found or inactive proxy: " << clientID);
        return false;
    }

    // Get per-command serializer
    mtsProxySerializer * serializer = PerCommandSerializerMap[commandID];
    if (!serializer) {
        LogError(mtsComponentInterfaceProxyServer, "SendExecuteCommandWriteReturnSerialized: cannot find per-command serializer");
        return false;
    }

    // Serialize the input argument
    std::string serializedArgument;
    serializer->Serialize(argument, serializedArgument);
    if (serializedArgument.empty()) {
        LogError(mtsComponentInterfaceProxyServer, "SendExecuteCommandWriteReturnSerialized: serialization failure: " << argument.ToString());
        return false;
    }

    // Argument placeholder of which value is set by the write return command
    std::string serializedResult;

    // Send address of result, only used by queued commands
    mtsObjectIDType resultAddress = reinterpret_cast<mtsObjectIDType>(&result);

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyServer, ">>>>> SEND: SendExecuteCommandWriteReturnSerialized: " << commandID << ", " << serializedResult.size() << " bytes");
#endif

    try {
        Ice::Byte executionResultIce;
        (*clientProxy)->ExecuteCommandWriteReturnSerialized(commandID,
                                                            serializedArgument,
                                                            resultAddress,
                                                            serializedResult,
                                                            executionResultIce);
        executionResult = static_cast<mtsExecutionResult::Enum>(executionResultIce);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsComponentInterfaceProxyServer, "SendExecuteCommandWriteReturnSerialized: network exception: " << ex);
        OnClientDisconnect(clientID);
        executionResult = mtsExecutionResult::NETWORK_ERROR;
        return false;
    }

    // Deserialize only if the command succeeded
    if (executionResult.GetResult() == mtsExecutionResult::COMMAND_SUCCEEDED) {
        serializer->DeSerialize(serializedResult, result);
    }

    return true;
}


//-------------------------------------------------------------------------
//  Definition by mtsComponentInterfaceProxy.ice
//-------------------------------------------------------------------------
mtsComponentInterfaceProxyServer::ComponentInterfaceServerI::ComponentInterfaceServerI(
    const Ice::CommunicatorPtr& communicator, const Ice::LoggerPtr& logger,
    mtsComponentInterfaceProxyServer * componentInterfaceProxyServer)
    : Communicator(communicator),
      SenderThreadPtr(new SenderThread<ComponentInterfaceServerIPtr>(this)),
      IceLogger(logger),
      ComponentInterfaceProxyServer(componentInterfaceProxyServer)
{
}

mtsComponentInterfaceProxyServer::ComponentInterfaceServerI::~ComponentInterfaceServerI()
{
    Stop();
}

void mtsComponentInterfaceProxyServer::ComponentInterfaceServerI::Start()
{
    ComponentInterfaceProxyServer->GetLogger()->trace("mtsComponentInterfaceProxyServer", "Send thread starts");

    SenderThreadPtr->start();
}

//#define _COMMUNICATION_TEST_
void mtsComponentInterfaceProxyServer::ComponentInterfaceServerI::Run()
{
#ifdef _COMMUNICATION_TEST_
    int count = 0;

    while (IsActiveProxy())
    {
        osaSleep(1 * cmn_s);
        std::cout << "\tServer [" << ComponentInterfaceProxyServer->GetProxyName() << "] running (" << ++count << ")" << std::endl;

        std::stringstream ss;
        ss << "Msg " << count << " from Server";

        ComponentInterfaceProxyServer->SendTestMessageFromServerToClient(ss.str());
    }
#else
    double lastTickChecked = 0.0, now;
    while (IsActiveProxy()) {
        now = osaGetTime();
        if (now < lastTickChecked + mtsProxyConfig::CheckPeriodForInterfaceConnections) {
            osaSleep(10 * cmn_ms);
            continue;
        }
        lastTickChecked = now;

        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
        try {
            if (ComponentInterfaceProxyServer) {
                ComponentInterfaceProxyServer->MonitorConnections();
            }
        } catch (const Ice::Exception & ex) {
            LogPrint(mtsComponentInterfaceProxyServer::ManagerServerI, "Server component disconnection detected: " << ex.what());
        }
    }
#endif
}

void mtsComponentInterfaceProxyServer::ComponentInterfaceServerI::Stop()
{
    if (!IsActiveProxy()) return;

    ComponentInterfaceProxyServer = 0;

    IceUtil::ThreadPtr callbackSenderThread;
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);

        notify();

        callbackSenderThread = SenderThreadPtr;

        // Prevent sender thread from sending any further message
        SenderThreadPtr->StopSend();
        SenderThreadPtr = 0; // Resolve cyclic dependency.
    }
    callbackSenderThread->getThreadControl().join();

    LogPrint(ComponentInterfaceServerI, "Stopped and destroyed callback thread to communicate with clients");
}

bool mtsComponentInterfaceProxyServer::ComponentInterfaceServerI::IsActiveProxy() const
{
    if (ComponentInterfaceProxyServer) {
        return ComponentInterfaceProxyServer->IsActiveProxy();
    } else {
        return false;
    }
}

//-----------------------------------------------------------------------------
//  Network Event Handlers
//-----------------------------------------------------------------------------
void mtsComponentInterfaceProxyServer::ComponentInterfaceServerI::TestMessageFromClientToServer(
    const std::string & str, const ::Ice::Current & current)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ComponentInterfaceServerI, "<<<<< RECV: TestMessageFromClientToServer");
#endif

    const IceConnectionIDType iceConnectionID = current.ctx.find(mtsComponentInterfaceProxyServer::ConnectionIDKey)->second;

    ComponentInterfaceProxyServer->ReceiveTestMessageFromClientToServer(iceConnectionID, str);
}

bool mtsComponentInterfaceProxyServer::ComponentInterfaceServerI::AddClient(
    const std::string & connectingProxyName, ::Ice::Int providedInterfaceProxyInstanceID,
    const ::Ice::Identity & identity, const ::Ice::Current& current)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
   LogPrint(ComponentInterfaceServerI, "<<<<< RECV: AddClient: " << connectingProxyName << " (" << Communicator->identityToString(identity) << ")");
#endif

    const IceConnectionIDType iceConnectionID = current.ctx.find(mtsComponentInterfaceProxyServer::ConnectionIDKey)->second;

    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);

    ComponentInterfaceClientProxyType clientProxy =
        ComponentInterfaceClientProxyType::uncheckedCast(current.con->createProxy(identity));

    return ComponentInterfaceProxyServer->ReceiveAddClient(iceConnectionID,
        connectingProxyName, (unsigned int) providedInterfaceProxyInstanceID, clientProxy);
}

void mtsComponentInterfaceProxyServer::ComponentInterfaceServerI::Refresh(const ::Ice::Current& current)
{
    const IceConnectionIDType iceConnectionID = current.ctx.find(mtsComponentInterfaceProxyServer::ConnectionIDKey)->second;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ComponentInterfaceServerI, "<<<<< RECV: Refresh: " << iceConnectionID);
#endif

    // MJ: Could implement session refresh here
}

void mtsComponentInterfaceProxyServer::ComponentInterfaceServerI::Shutdown(const ::Ice::Current& current)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ComponentInterfaceServerI, "<<<<< RECV: Shutdown");
#endif

    const IceConnectionIDType iceConnectionID = current.ctx.find(mtsComponentInterfaceProxyServer::ConnectionIDKey)->second;

    //MJ: Could have shutdown methods like ComponentInterfaceProxyServer->ShutdownSession(current);
}

bool mtsComponentInterfaceProxyServer::ComponentInterfaceServerI::FetchEventGeneratorProxyPointers(
    const std::string & clientComponentName, const std::string & requiredInterfaceName,
    mtsComponentInterfaceProxy::EventGeneratorProxyPointerSet & eventGeneratorProxyPointers,
    const ::Ice::Current & current) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ComponentInterfaceServerI, "<<<<< RECV: FetchEventGeneratorProxyPointers: " << clientComponentName << ", " << requiredInterfaceName);
#endif

    const IceConnectionIDType iceConnectionID = current.ctx.find(mtsComponentInterfaceProxyServer::ConnectionIDKey)->second;

    return ComponentInterfaceProxyServer->ReceiveFetchEventGeneratorProxyPointers(
        iceConnectionID, clientComponentName, requiredInterfaceName, eventGeneratorProxyPointers);
}

void mtsComponentInterfaceProxyServer::ComponentInterfaceServerI::ExecuteEventVoid(
    ::Ice::Long commandID, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ComponentInterfaceServerI, "<<<<< RECV: ExecuteEventVoid: " << commandID);
#endif

    ComponentInterfaceProxyServer->ReceiveExecuteEventVoid(commandID);
}

void mtsComponentInterfaceProxyServer::ComponentInterfaceServerI::ExecuteEventWriteSerialized(
    ::Ice::Long commandID, const ::std::string & argument, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ComponentInterfaceServerI, "<<<<< RECV: ExecuteEventWriteSerialized: " << commandID);
#endif

    ComponentInterfaceProxyServer->ReceiveExecuteEventWriteSerialized(commandID, argument);
}

void mtsComponentInterfaceProxyServer::ComponentInterfaceServerI::ExecuteEventReturnSerialized(::Ice::Long commandID,
                                                                                               ::Ice::Long resultAddress,
                                                                                               const ::std::string & result,
                                                                                               const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ComponentInterfaceServerI, "<<<<< RECV: ExecuteEventReturnSerialized: " << commandID);
#endif

    ComponentInterfaceProxyServer->ReceiveExecuteEventReturnSerialized(commandID, resultAddress, result);
}
