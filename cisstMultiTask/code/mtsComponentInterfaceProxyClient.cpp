/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2010-01-13

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
#include "mtsComponentInterfaceProxyClient.h"
#include <cisstMultiTask/mtsFunctionVoid.h>
#include "mtsFunctionReadProxy.h"
#include "mtsFunctionWriteProxy.h"
#include "mtsFunctionQualifiedReadProxy.h"
#include "mtsFunctionVoidReturnProxy.h"
#include "mtsFunctionWriteReturnProxy.h"

unsigned int mtsComponentInterfaceProxyClient::InstanceCounter = 0;

mtsComponentInterfaceProxyClient::mtsComponentInterfaceProxyClient(
    const std::string & serverEndpointInfo, const unsigned int connectionID)
    : BaseClientType("config.client", serverEndpointInfo),
      ComponentInterfaceServerProxy(0),
      ConnectionID(connectionID)
#if IMPROVE_ICE_THREADING
      , IceThreadInitEvent(0)
#endif
{
    ProxyName = "ComponentInterfaceProxyServer";

#if IMPROVE_ICE_THREADING
    IceThreadInitEvent = new osaThreadSignal;
#endif
}

mtsComponentInterfaceProxyClient::~mtsComponentInterfaceProxyClient()
{
    StopProxy();

#if IMPROVE_ICE_THREADING
    delete IceThreadInitEvent;
#endif
}

//-----------------------------------------------------------------------------
//  Proxy Start-up
//-----------------------------------------------------------------------------
bool mtsComponentInterfaceProxyClient::StartProxy(mtsComponentProxy * proxyOwner)
{
    // Initialize Ice object.
    IceInitialize();

    if (!InitSuccessFlag) {
        LogError(mtsComponentInterfaceProxyClient, "ICE proxy client initialization failed");
        return false;
    }

    // Client configuration for bidirectional communication
    Ice::ObjectAdapterPtr adapter = IceCommunicator->createObjectAdapter("");
    Ice::Identity id;
    id.name = GetIceGUID();
    id.category = "";

    Client = new ComponentInterfaceClientI(IceCommunicator, IceLogger, ComponentInterfaceServerProxy, this);
    adapter->add(Client, id);
    adapter->activate();
    ComponentInterfaceServerProxy->ice_getConnection()->setAdapter(adapter);

    // Set implicit context (per proxy context)
    IceCommunicator->getImplicitContext()->put(mtsComponentInterfaceProxyServer::GetConnectionIDKey(),
                                               IceCommunicator->identityToString(id));

    // Set proxy owner and name of this proxy object
    std::string thisProcessName = "On";
    mtsManagerLocal * managerLocal = mtsManagerLocal::GetInstance();
    thisProcessName += managerLocal->GetProcessName();

    SetProxyOwner(proxyOwner, thisProcessName);

    // Connect to server proxy by adding this ICE proxy client to server
    if (!ComponentInterfaceServerProxy->AddClient(GetProxyName(), (::Ice::Int) ConnectionID, id)) {
        LogError(mtsComponentInterfaceProxyClient, "AddClient() failed: duplicate proxy name or identity");
        return false;
    }

    // Create a worker thread here but is not running yet.
    ThreadArgumentsInfo.Proxy = this;
    ThreadArgumentsInfo.Runner = mtsComponentInterfaceProxyClient::Runner;

    // Set a short name of this thread as "CIPC" (Component Interface Proxy
    // Client) to meet the requirement that some of operating system have --
    // only a few characters can be used as a thread name (e.g. Linux RTAI)
    std::stringstream ss;
    ss << "CIPC" << mtsComponentInterfaceProxyClient::InstanceCounter++;
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

void mtsComponentInterfaceProxyClient::CreateProxy(void)
{
    ComponentInterfaceServerProxy = mtsComponentInterfaceProxy::ComponentInterfaceServerPrx::checkedCast(ProxyObject);
    if (!ComponentInterfaceServerProxy) {
        throw "mtsComponentInterfaceProxyClient: CreateProxy() failed - invalid proxy";
    }

    Server = new ComponentInterfaceClientI(IceCommunicator, IceLogger, ComponentInterfaceServerProxy, this);
}

void mtsComponentInterfaceProxyClient::RemoveProxy(void)
{
    Server->Stop();

    ComponentInterfaceServerProxy = 0;
}

void mtsComponentInterfaceProxyClient::StartClient(void)
{
    Server->Start();

    ChangeProxyState(PROXY_STATE_ACTIVE);
#if IMPROVE_ICE_THREADING
    IceThreadInitEvent->Raise();
#endif

    // This is a blocking call that should run in a different thread.
    IceCommunicator->waitForShutdown();
}

void mtsComponentInterfaceProxyClient::Runner(ThreadArguments<mtsComponentProxy> * arguments)
{
    mtsComponentInterfaceProxyClient * ProxyClient =
        dynamic_cast<mtsComponentInterfaceProxyClient*>(arguments->Proxy);
    if (!ProxyClient) {
        CMN_LOG_RUN_ERROR << "mtsComponentInterfaceProxyClient: failed to get proxy client." << std::endl;
        return;
    }

    ProxyClient->GetLogger()->trace("mtsComponentInterfaceProxyClient", "proxy client starts");

    try {
        ProxyClient->StartClient();
    } catch (const Ice::Exception& e) {
        std::string error("mtsComponentInterfaceProxyClient: ");
        error += e.what();
        ProxyClient->GetLogger()->error(error);
    } catch (...) {
        std::string error("mtsComponentInterfaceProxyClient: exception at mtsComponentInterfaceProxyClient::Runner()");
        ProxyClient->GetLogger()->error(error);
    }
}

void mtsComponentInterfaceProxyClient::StopProxy()
{
    if (!IsActiveProxy()) return;

    try {
        BaseClientType::StopProxy();
        Server->Stop();
        Client->Stop();
    } catch (const Ice::Exception& e) {
        std::string error("mtsComponentInterfaceProxyClient: ");
        error += e.what();
        LogError(mtsManagerProxyClient, error);
    }

    IceGUID = "";

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, "Stopped component interface proxy client");
#endif
}

void mtsComponentInterfaceProxyClient::OnServerDisconnect(const Ice::Exception & ex)
{
    if (!IsActiveProxy()) {
        return; // already detected disconnection
    }

    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    if (LCM->IsGCMActive()) {
        // Ice - ConnectionLostException - forceful closure by peer
        // Ice - ForcedCloseConnectionException - after forceful closure by peer
        CMN_LOG_CLASS_RUN_WARNING << ex << std::endl;
        CMN_LOG_CLASS_RUN_WARNING << "Component interface proxy \"" << ProxyName << "\" detected SERVER COMPONENT DISCONNECTION "
            << "(connection id: \"" << ConnectionID << "\")" << std::endl;
    }

    StopProxy();
}

bool mtsComponentInterfaceProxyClient::AddPerEventSerializer(const mtsCommandIDType commandID, mtsProxySerializer * serializer)
{
    PerEventSerializerMapType::const_iterator it = PerEventSerializerMap.find(commandID);
    if (!serializer || it != PerEventSerializerMap.end()) {
        LogError(mtsComponentInterfaceProxyClient, "failed to add per-event serializer" << std::endl);
        return false;
    }

    PerEventSerializerMap[commandID] = serializer;

    return true;
}

//-------------------------------------------------------------------------
//  Event Handlers (Server -> Client)
//-------------------------------------------------------------------------
void mtsComponentInterfaceProxyClient::ReceiveTestMessageFromServerToClient(const std::string & str) const
{
    std::cout << "Client received (Server -> Client): " << str << std::endl;
}

bool mtsComponentInterfaceProxyClient::ReceiveFetchFunctionProxyPointers(
    const std::string & requiredInterfaceName, mtsComponentInterfaceProxy::FunctionProxyPointerSet & functionProxyPointers) const
{
    // Get proxy owner object (of type mtsComponentProxy)
    mtsComponentProxy * proxyOwner = this->ProxyOwner;
    if (!proxyOwner) {
        LogError(mtsComponentInterfaceProxyClient, "ReceiveFetchFunctionProxyPointers: invalid proxy owner");
        return false;
    }

    return proxyOwner->GetFunctionProxyPointers(requiredInterfaceName, functionProxyPointers);
}

void mtsComponentInterfaceProxyClient::ReceiveExecuteCommandVoid(const mtsCommandIDType commandID,
                                                                 const mtsBlockingType blocking, mtsExecutionResult & executionResult)
{
    mtsFunctionVoid * functionVoid = reinterpret_cast<mtsFunctionVoid *>(commandID);
    if (!functionVoid) {
        LogError(mtsComponentInterfaceProxyClient, "ReceiveExecuteCommandVoid: invalid proxy id of function void: " << commandID);
        executionResult = mtsExecutionResult::INVALID_COMMAND_ID;
        return;
    }

    // Execute the command
    if (blocking == MTS_BLOCKING) {
        executionResult = (*functionVoid).ExecuteBlocking();
    } else {
        executionResult = (*functionVoid)();
    }
}

void mtsComponentInterfaceProxyClient::ReceiveExecuteCommandWriteSerialized(const mtsCommandIDType commandID,
                                                                            const mtsBlockingType blocking, mtsExecutionResult & executionResult,
                                                                            const std::string & serializedArgument)
{
    mtsFunctionWriteProxy * functionWriteProxy = reinterpret_cast<mtsFunctionWriteProxy*>(commandID);
    if (!functionWriteProxy) {
        LogError(mtsComponentInterfaceProxyClient, "ReceiveExecuteCommandWriteSerialized: invalid proxy id of function write: " << commandID);
        executionResult = mtsExecutionResult::INVALID_COMMAND_ID;
        return;
    }

    // Deserialize
    mtsProxySerializer * deserializer = functionWriteProxy->GetSerializer();
    mtsGenericObject * argument = deserializer->DeSerialize(serializedArgument);
    if (!argument) {
        LogError(mtsComponentInterfaceProxyClient, "ReceiveExecuteCommandWriteSerialized: Deserialization failed");
        executionResult = mtsExecutionResult::DESERIALIZATION_ERROR;
        return;
    }

    // Execute the command
    if (blocking == MTS_BLOCKING) {
        executionResult = (*functionWriteProxy).ExecuteBlocking(*argument);
    } else {
        executionResult = (*functionWriteProxy)(*argument);
    }

    // Release memory internally created by deserializer
    delete argument;
}

void mtsComponentInterfaceProxyClient::ReceiveExecuteCommandReadSerialized(const mtsCommandIDType commandID,
                                                                           mtsExecutionResult & executionResult,
                                                                           std::string & serializedArgument)
{
    mtsFunctionReadProxy * functionReadProxy = reinterpret_cast<mtsFunctionReadProxy*>(commandID);
    if (!functionReadProxy) {
        LogError(mtsComponentInterfaceProxyClient, "ReceiveExecuteCommandReadSerialized: invalid proxy id of function read: " << commandID);
        executionResult = mtsExecutionResult::INVALID_COMMAND_ID;
        return;
    }

    // Create a temporary argument which includes dynamic allocation internally.
    // Therefore, this object should be deallocated manually.
    mtsGenericObject * tempArgument =
        dynamic_cast<mtsGenericObject *>(functionReadProxy->GetCommand()->GetArgumentPrototype()->Services()->Create());
    if (!tempArgument) {
        LogError(mtsComponentInterfaceProxyClient, "ReceiveExecuteCommandReadSerialized: failed to create a temporary argument");
        executionResult = mtsExecutionResult::ARGUMENT_DYNAMIC_CREATION_FAILED;
        return;
    }

    // Execute the command
    executionResult = (*functionReadProxy)(*tempArgument);

    // Serialize
    mtsProxySerializer * serializer = functionReadProxy->GetSerializer();
    serializer->Serialize(*tempArgument, serializedArgument);

    // Deallocate memory
    delete tempArgument;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyClient, "ReceiveExecuteCommandReadSerialized: sent " << serializedArgument.size() << " bytes");
#endif
}

void mtsComponentInterfaceProxyClient::ReceiveExecuteCommandQualifiedReadSerialized(const mtsCommandIDType commandID,
                                                                                    mtsExecutionResult & executionResult,
                                                                                    const std::string & serializedArgumentIn, std::string & serializedArgumentOut)
{
    mtsFunctionQualifiedReadProxy * functionQualifiedReadProxy = reinterpret_cast<mtsFunctionQualifiedReadProxy*>(commandID);
    if (!functionQualifiedReadProxy) {
        LogError(mtsComponentInterfaceProxyClient, "ReceiveExecuteCommandQualifiedReadSerialized: invalid proxy id of function qualified read: " << commandID);
        executionResult = mtsExecutionResult::INVALID_COMMAND_ID;
        return;
    }

    // Deserialize
    mtsProxySerializer * deserializer = functionQualifiedReadProxy->GetSerializer();
    mtsGenericObject * argumentIn = deserializer->DeSerialize(serializedArgumentIn);
    if (!argumentIn) {
        LogError(mtsComponentInterfaceProxyClient, "ReceiveExecuteCommandQualifiedReadSerialized: Deserialization failed");
        executionResult = mtsExecutionResult::DESERIALIZATION_ERROR;
        return;
    }

    // Create a temporary argument which includes dynamic allocation internally.
    // Therefore, this object should be deallocated manually.
    mtsGenericObject * tempArgumentOut =
        dynamic_cast<mtsGenericObject *>(functionQualifiedReadProxy->GetCommand()->GetArgument2Prototype()->Services()->Create());
    if (!tempArgumentOut) {
        LogError(mtsComponentInterfaceProxyClient, "ReceiveExecuteCommandQualifiedReadSerialized: failed to create a temporary argument");
        executionResult = mtsExecutionResult::ARGUMENT_DYNAMIC_CREATION_FAILED;
        // release memory internally allocated by deserializer
        delete argumentIn;
        return;
    }

    // Execute the command
    executionResult = (*functionQualifiedReadProxy)(*argumentIn, *tempArgumentOut);

    // Serialize
    deserializer->Serialize(*tempArgumentOut, serializedArgumentOut);

    // Release memory internally created by deserializer
    delete argumentIn;
    // Deallocate memory
    delete tempArgumentOut;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyClient, "ReceiveExecuteCommandQualifiedReadSerialized: sent " << serializedArgumentOut.size() << " bytes");
#endif
}


void mtsComponentInterfaceProxyClient::ReceiveExecuteCommandVoidReturnSerialized(const mtsCommandIDType commandID,
                                                                                 const mtsObjectIDType resultAddress,
                                                                                 mtsExecutionResult & executionResult,
                                                                                 std::string & serializedResult)
{
    mtsFunctionVoidReturnProxy * functionVoidReturnProxy = reinterpret_cast<mtsFunctionVoidReturnProxy*>(commandID);
    if (!functionVoidReturnProxy) {
        LogError(mtsComponentInterfaceProxyClient, "ReceiveExecuteCommandVoidReturnSerialized: invalid proxy id of function void return: " << commandID);
        executionResult = mtsExecutionResult::INVALID_COMMAND_ID;
        return;
    }

    // Function proxy stores the result placeholder, needs to create on first run
    if (functionVoidReturnProxy->GetResultPointer() == 0) {
        mtsGenericObject * result =
            dynamic_cast<mtsGenericObject *>(functionVoidReturnProxy->GetCommand()->GetResultPrototype()->Services()->Create());
        if (result == 0) {
            LogError(mtsComponentInterfaceProxyClient, "ReceiveExecuteCommandVoidReturnSerialized: failed to create a temporary argument");
            executionResult = mtsExecutionResult::ARGUMENT_DYNAMIC_CREATION_FAILED;
            return;
        }
        functionVoidReturnProxy->SetResultPointer(result);
    }

    // Store remote address for return value
    functionVoidReturnProxy->SetRemoteResultPointer(resultAddress);

    // Execute the command
    executionResult = (*functionVoidReturnProxy)(*(functionVoidReturnProxy->GetResultPointer()));

    // Event will need to know which function was blocked
    if (executionResult.GetResult() == mtsExecutionResult::COMMAND_QUEUED) {
        functionVoidReturnProxy->SetAsLastFunction();
    }

    // Serialize the result if the command is not queued - if the command is queued, the result will be sent later
    if (executionResult.GetResult() == mtsExecutionResult::COMMAND_SUCCEEDED) {
        mtsProxySerializer * deserializer = functionVoidReturnProxy->GetSerializer();
        deserializer->Serialize(*(functionVoidReturnProxy->GetResultPointer()),
                                serializedResult);
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyClient, "ReceiveExecuteCommandVoidReturnSerialized: sent " << serializedResult.size() << " bytes");
#endif
}


void mtsComponentInterfaceProxyClient::ReceiveExecuteCommandWriteReturnSerialized(const mtsCommandIDType commandID,
                                                                                  const std::string & serializedArgument,
                                                                                  const mtsObjectIDType resultAddress,
                                                                                  mtsExecutionResult & executionResult,
                                                                                  std::string & serializedResult)
{
    mtsFunctionWriteReturnProxy * functionWriteReturnProxy = reinterpret_cast<mtsFunctionWriteReturnProxy*>(commandID);
    if (!functionWriteReturnProxy) {
        LogError(mtsComponentInterfaceProxyClient, "ReceiveExecuteCommandWriteReturnSerialized: invalid proxy id of function write return: " << commandID);
        executionResult = mtsExecutionResult::INVALID_COMMAND_ID;
        return;
    }

    // Deserialize
    mtsProxySerializer * deserializer = functionWriteReturnProxy->GetSerializer();

    // Function proxy can store the result place holder, needs to create on first run
    if (functionWriteReturnProxy->GetArgumentPointer() == 0) {
        mtsGenericObject * argument =
            dynamic_cast<mtsGenericObject *>(functionWriteReturnProxy->GetCommand()->GetArgumentPrototype()->Services()->Create());
        if (argument == 0) {
            LogError(mtsComponentInterfaceProxyClient, "ReceiveExecuteCommandWriteReturnSerialized: failed to create a placeholder for argument");
            executionResult = mtsExecutionResult::ARGUMENT_DYNAMIC_CREATION_FAILED;
            return;
        }
        functionWriteReturnProxy->SetArgumentPointer(argument);
    }

    // Function proxy can store the result place holder, needs to create on first run
    if (functionWriteReturnProxy->GetResultPointer() == 0) {
        mtsGenericObject * result =
            dynamic_cast<mtsGenericObject *>(functionWriteReturnProxy->GetCommand()->GetResultPrototype()->Services()->Create());
        if (result == 0) {
            LogError(mtsComponentInterfaceProxyClient, "ReceiveExecuteCommandWriteReturnSerialized: failed to create a placeholder for return value");
            executionResult = mtsExecutionResult::ARGUMENT_DYNAMIC_CREATION_FAILED;
            return;
        }
        functionWriteReturnProxy->SetResultPointer(result);
    }

    // Try to deserialize argument, this might fail
    if (!deserializer->DeSerialize(serializedArgument,
                                   *(functionWriteReturnProxy->GetArgumentPointer()))) {
        LogError(mtsComponentInterfaceProxyClient, "ReceiveExecuteCommandWriteReturnSerialized: deserialization failed");
        executionResult = mtsExecutionResult::DESERIALIZATION_ERROR;
        return;
    }

    // Store remote address for return value
    functionWriteReturnProxy->SetRemoteResultPointer(resultAddress);

    // Execute the command
    executionResult = (*functionWriteReturnProxy)(*(functionWriteReturnProxy->GetArgumentPointer()),
                                                  *(functionWriteReturnProxy->GetResultPointer()));

    // Event will need to know which function was blocked
    if (executionResult.GetResult() == mtsExecutionResult::COMMAND_QUEUED) {
        functionWriteReturnProxy->SetAsLastFunction();
    }

    // Serialize the result if the command is not queued - if the command is queued, the result will be sent later
    if (executionResult.GetResult() == mtsExecutionResult::COMMAND_SUCCEEDED) {
        deserializer->Serialize(*(functionWriteReturnProxy->GetResultPointer()),
                                serializedResult);
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyClient, "ReceiveExecuteCommandWriteReturnSerialized: sent " << serializedResult.size() << " bytes");
#endif
}


//-------------------------------------------------------------------------
//  Event Generators (Event Sender) : Client -> Server
//-------------------------------------------------------------------------
void mtsComponentInterfaceProxyClient::SendTestMessageFromClientToServer(const std::string & str) const
{
    if (!IsActiveProxy()) return;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyClient, ">>>>> SEND: MessageFromClientToServer");
#endif

    ComponentInterfaceServerProxy->TestMessageFromClientToServer(str);
}

bool mtsComponentInterfaceProxyClient::SendFetchEventGeneratorProxyPointers(
    const std::string & clientComponentName, const std::string & requiredInterfaceName,
    mtsComponentInterfaceProxy::EventGeneratorProxyPointerSet & eventGeneratorProxyPointers)
{
    if (!IsActiveProxy()) return false;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyClient, ">>>>> SEND: FetchEventGeneratorProxyPointers: " << clientComponentName << ":" << requiredInterfaceName);
#endif

    try {
        return ComponentInterfaceServerProxy->FetchEventGeneratorProxyPointers(
            clientComponentName, requiredInterfaceName, eventGeneratorProxyPointers);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsComponentInterfaceProxyServer, "SendFetchEventGeneratorProxyPointers: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }
}


bool mtsComponentInterfaceProxyClient::SendExecuteEventVoid(const mtsCommandIDType commandID)
{
    if (!IsActiveProxy()) return false;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyClient, ">>>>> SEND: SendExecuteEventVoid: " << commandID);
#endif

    try {
        ComponentInterfaceServerProxy->ExecuteEventVoid(commandID);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsComponentInterfaceProxyServer, "SendExecuteEventVoid: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }

    return true;
}


bool mtsComponentInterfaceProxyClient::SendExecuteEventWriteSerialized(const mtsCommandIDType commandID, const mtsGenericObject & argument)
{
    if (!IsActiveProxy()) return false;

    // Get per-event serializer.
    mtsProxySerializer * serializer = PerEventSerializerMap[commandID];
    if (!serializer) {
        LogError(mtsComponentInterfaceProxyClient, "SendExecuteEventWriteSerialized: cannot find per-event serializer");
        return false;
    }

    // Serialize the argument passed.
    std::string serializedArgument;
    serializer->Serialize(argument, serializedArgument);
    if (serializedArgument.empty()) {
        LogError(mtsComponentInterfaceProxyClient, "SendExecuteEventWriteSerialized: serialization failure: " << argument.ToString());
        return false;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyClient, ">>>>> SEND: SendExecuteEventWriteSerialized: " << commandID);
#endif

    try {
        ComponentInterfaceServerProxy->ExecuteEventWriteSerialized(commandID, serializedArgument);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsComponentInterfaceProxyClient, "SendExecuteEventWriteSerialized: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }

    return true;
}


bool mtsComponentInterfaceProxyClient::SendExecuteEventReturnSerialized(const mtsCommandIDType commandID,
                                                                        const mtsObjectIDType resultAddress,
                                                                        const std::string & result)
{
    if (!IsActiveProxy()) return false;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyClient, ">>>>> SEND: SendExecuteEventReturnSerialized: " << commandID);
#endif

    try {
        ComponentInterfaceServerProxy->ExecuteEventReturnSerialized(commandID, resultAddress, result);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsComponentInterfaceProxyClient, "SendExecuteEventReturnSerialized: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }

    return true;
}


//-------------------------------------------------------------------------
//  Definition by mtsComponentInterfaceProxy.ice
//-------------------------------------------------------------------------
mtsComponentInterfaceProxyClient::ComponentInterfaceClientI::ComponentInterfaceClientI(
    const Ice::CommunicatorPtr& communicator,
    const Ice::LoggerPtr& logger,
    const mtsComponentInterfaceProxy::ComponentInterfaceServerPrx& server,
    mtsComponentInterfaceProxyClient * componentInterfaceProxyClient)
    : Communicator(communicator),
      SenderThreadPtr(new SenderThread<ComponentInterfaceClientIPtr>(this)),
      IceLogger(logger),
      ComponentInterfaceProxyClient(componentInterfaceProxyClient),
      Server(server)
{
}

mtsComponentInterfaceProxyClient::ComponentInterfaceClientI::~ComponentInterfaceClientI()
{
    Stop();
}

void mtsComponentInterfaceProxyClient::ComponentInterfaceClientI::Start()
{
    ComponentInterfaceProxyClient->GetLogger()->trace("mtsComponentInterfaceProxyClient", "Send thread starts");

    SenderThreadPtr->start();
}

//#define _COMMUNICATION_TEST_
void mtsComponentInterfaceProxyClient::ComponentInterfaceClientI::Run()
{
#ifdef _COMMUNICATION_TEST_
    int count = 0;

    while (this->IsActiveProxy()) {
        osaSleep(1 * cmn_s);
        std::cout << "\tClient [" << ComponentInterfaceProxyClient->GetProxyName() << "] running (" << ++count << ")" << std::endl;

        std::stringstream ss;
        ss << "Msg " << count << " from Client " << ComponentInterfaceProxyClient->GetProxyName();

        ComponentInterfaceProxyClient->SendTestMessageFromClientToServer(ss.str());
    }
#else
    double lastTickChecked = 0.0, now;
    while (IsActiveProxy()) {
        now = osaGetTime();
        if (now < lastTickChecked + mtsProxyConfig::RefreshPeriodForInterfaces) {
            osaSleep(10 * cmn_ms);
            continue;
        }
        lastTickChecked = now;

        try {
            Server->Refresh();
        } catch (const ::Ice::Exception & ex) {
            LogPrint(mtsComponentInterfaceProxyClient, "refresh failed (" << Server->ice_toString() << ")" << std::endl << ex);
            if (ComponentInterfaceProxyClient) {
                ComponentInterfaceProxyClient->OnServerDisconnect(ex);
            }
        }
    }
#endif

    LogPrint(mtsManagerProxyClient, "mtsComponentInterfaceProxyClient::ComponentInterfaceClientI - terminated");
}

void mtsComponentInterfaceProxyClient::ComponentInterfaceClientI::Stop()
{
    if (!IsActiveProxy()) return;

    ComponentInterfaceProxyClient = 0;

    IceUtil::ThreadPtr callbackSenderThread;
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);

        notify();

        callbackSenderThread = SenderThreadPtr;

        // Prevent sender thread from sending any further message
        SenderThreadPtr->StopSend();
        SenderThreadPtr = 0;
    }
    callbackSenderThread->getThreadControl().join();

    LogPrint(ComponentInterfaceClientI, "Stopped and destroyed callback thread to communicate with server");
}

bool mtsComponentInterfaceProxyClient::ComponentInterfaceClientI::IsActiveProxy(void) const
{
    if (ComponentInterfaceProxyClient) {
        return ComponentInterfaceProxyClient->IsActiveProxy();
    } else {
        return false;
    }
}

//-----------------------------------------------------------------------------
//  Network Event handlers (Server -> Client)
//-----------------------------------------------------------------------------
void
mtsComponentInterfaceProxyClient
::ComponentInterfaceClientI
::TestMessageFromServerToClient(const std::string & str,
                                const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyClient, "<<<<< RECV: TestMessageFromServerToClient");
#endif

    ComponentInterfaceProxyClient->ReceiveTestMessageFromServerToClient(str);
}


bool
mtsComponentInterfaceProxyClient
::ComponentInterfaceClientI
::FetchFunctionProxyPointers(const std::string & requiredInterfaceName,
                             mtsComponentInterfaceProxy::FunctionProxyPointerSet & functionProxyPointers,
                             const ::Ice::Current & CMN_UNUSED(current)) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyClient, "<<<<< RECV: FetchFunctionProxyPointers: " << requiredInterfaceName);
#endif

    return ComponentInterfaceProxyClient->ReceiveFetchFunctionProxyPointers(requiredInterfaceName, functionProxyPointers);
}


void
mtsComponentInterfaceProxyClient
::ComponentInterfaceClientI
::ExecuteCommandVoid(::Ice::Long commandID,
                     bool blocking,
                     ::Ice::Byte & executionResultByte,
                     const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyClient, "<<<<< RECV: ExecuteCommandVoid: " << commandID << ", "
        << (blocking ? "BLOCKING" : "NON-BLOCKING"));
#endif
    mtsExecutionResult executionResult;
    if (blocking) {
        ComponentInterfaceProxyClient->ReceiveExecuteCommandVoid(commandID, MTS_BLOCKING, executionResult);
    } else {
        ComponentInterfaceProxyClient->ReceiveExecuteCommandVoid(commandID, MTS_NOT_BLOCKING, executionResult);
    }
    executionResultByte = static_cast< ::Ice::Byte>(executionResult.GetResult());
}

void
mtsComponentInterfaceProxyClient
::ComponentInterfaceClientI
::ExecuteCommandWriteSerialized(::Ice::Long commandID,
                                const ::std::string & argument, bool blocking,
                                ::Ice::Byte & executionResultByte,
                                const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyClient, "<<<<< RECV: ExecuteCommandWriteSerialized: " << commandID << ", "
        << argument.size() << ", " << (blocking ? "BLOCKING" : "NON-BLOCKING"));
#endif
    mtsExecutionResult executionResult;
    if (blocking) {
        ComponentInterfaceProxyClient->ReceiveExecuteCommandWriteSerialized(commandID, MTS_BLOCKING, executionResult, argument);
    } else {
        ComponentInterfaceProxyClient->ReceiveExecuteCommandWriteSerialized(commandID, MTS_NOT_BLOCKING, executionResult, argument);
    }
    executionResultByte = static_cast< ::Ice::Byte>(executionResult.GetResult());
}


void
mtsComponentInterfaceProxyClient
::ComponentInterfaceClientI
::ExecuteCommandReadSerialized(::Ice::Long commandID,
                               ::std::string & argument,
                               ::Ice::Byte & executionResultByte,
                               const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyClient, "<<<<< RECV: ExecuteCommandReadSerialized: " << commandID << ", " << argument.size());
#endif
    mtsExecutionResult executionResult;
    ComponentInterfaceProxyClient->ReceiveExecuteCommandReadSerialized(commandID, executionResult, argument);
    executionResultByte = static_cast< ::Ice::Byte>(executionResult.GetResult());
}


void
mtsComponentInterfaceProxyClient
::ComponentInterfaceClientI
::ExecuteCommandQualifiedReadSerialized(::Ice::Long commandID,
                                        const ::std::string & argumentIn, ::std::string & argumentOut,
                                        ::Ice::Byte & executionResultByte,
                                        const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyClient, "<<<<< RECV: ExecuteCommandQualifiedReadSerialized: " << commandID << ", " << argumentIn.size());
#endif
    mtsExecutionResult executionResult;
    ComponentInterfaceProxyClient->ReceiveExecuteCommandQualifiedReadSerialized(commandID, executionResult, argumentIn, argumentOut);
    executionResultByte = static_cast< ::Ice::Byte>(executionResult.GetResult());
}


void
mtsComponentInterfaceProxyClient
::ComponentInterfaceClientI
::ExecuteCommandVoidReturnSerialized(::Ice::Long commandID,
                                     ::Ice::Long resultAddress,
                                     ::std::string & result,
                                     ::Ice::Byte & executionResultByte,
                                     const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyClient, "<<<<< RECV: ExecuteCommandVoidReturnSerialized: " << commandID << ", " << resultAddress);
#endif
    mtsExecutionResult executionResult;
    ComponentInterfaceProxyClient->ReceiveExecuteCommandVoidReturnSerialized(commandID, resultAddress, executionResult, result);
    executionResultByte = static_cast< ::Ice::Byte>(executionResult.GetResult());
}


void
mtsComponentInterfaceProxyClient
::ComponentInterfaceClientI
::ExecuteCommandWriteReturnSerialized(::Ice::Long commandID,
                                      const ::std::string & argument,
                                      ::Ice::Long resultAddress,
                                      ::std::string & result,
                                      ::Ice::Byte & executionResultByte,
                                      const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyClient, "<<<<< RECV: ExecuteCommandWriteReturnSerialized: " << commandID << ", " << resultAddress);
#endif
    mtsExecutionResult executionResult;
    ComponentInterfaceProxyClient->ReceiveExecuteCommandWriteReturnSerialized(commandID, argument, resultAddress, executionResult, result);
    executionResultByte = static_cast< ::Ice::Byte>(executionResult.GetResult());
}
