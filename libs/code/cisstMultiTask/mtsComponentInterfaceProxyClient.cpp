/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung
  Created on: 2010-01-13

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsComponentProxy.h>
#include <cisstMultiTask/mtsComponentInterfaceProxyClient.h>
#include <cisstMultiTask/mtsFunctionVoid.h>
#include <cisstMultiTask/mtsFunctionReadProxy.h>
#include <cisstMultiTask/mtsFunctionWriteProxy.h>
#include <cisstMultiTask/mtsFunctionQualifiedReadProxy.h>

#include <cisstOSAbstraction/osaSleep.h>

unsigned int mtsComponentInterfaceProxyClient::InstanceCounter = 0;

mtsComponentInterfaceProxyClient::mtsComponentInterfaceProxyClient(
    const std::string & serverEndpointInfo, const unsigned int connectionID)
    : BaseClientType("config.client", serverEndpointInfo),
      ConnectionID(connectionID)
{
    ProxyName = "ComponentInterfaceProxyServer";
}

mtsComponentInterfaceProxyClient::~mtsComponentInterfaceProxyClient()
{
    Stop();
}

//-----------------------------------------------------------------------------
//  Proxy Start-up
//-----------------------------------------------------------------------------
bool mtsComponentInterfaceProxyClient::Start(mtsComponentProxy * proxyOwner)
{
    // Initialize Ice object.
    IceInitialize();

    if (!InitSuccessFlag) {
        LogError(mtsComponentInterfaceProxyClient, "ICE proxy client initialization failed");
        return false;
    }

    // Client configuration for bidirectional communication
    Ice::ObjectAdapterPtr adapter = IceCommunicator->createObjectAdapter("");
    Ice::Identity ident;
    ident.name = GetGUID();
    ident.category = "";

    mtsComponentInterfaceProxy::ComponentInterfaceClientPtr client =
        new ComponentInterfaceClientI(IceCommunicator, IceLogger, ComponentInterfaceServerProxy, this);
    adapter->add(client, ident);
    adapter->activate();
    ComponentInterfaceServerProxy->ice_getConnection()->setAdapter(adapter);

    // Set an implicit context (per proxy context)
    IceCommunicator->getImplicitContext()->put(
        mtsComponentInterfaceProxyServer::GetConnectionIDKey(), IceCommunicator->identityToString(ident));

    // Set proxy owner and name of this proxy object
    std::string thisProcessName = "On";
    mtsManagerLocal * managerLocal = mtsManagerLocal::GetInstance();
    thisProcessName += managerLocal->GetProcessName();

    SetProxyOwner(proxyOwner, thisProcessName);

    // Connect to server proxy by adding this ICE proxy client to server
    if (!ComponentInterfaceServerProxy->AddClient(GetProxyName(), (::Ice::Int) ConnectionID, ident)) {
        LogError(mtsComponentInterfaceProxyClient, "AddClient() failed: duplicate proxy name or identity");
        return false;
    }

    // Create a worker thread here but is not running yet.
    ThreadArgumentsInfo.Proxy = this;
    ThreadArgumentsInfo.Runner = mtsComponentInterfaceProxyClient::Runner;

    // Set a short name of this thread as CIPC which means "Component Interface
    // Proxy Client." Such a condensed naming rule is required because a total
    // number of characters in a thread name is sometimes limited to a small
    // number (e.g. LINUX RTAI).
    std::stringstream ss;
    ss << "CIPC" << mtsComponentInterfaceProxyClient::InstanceCounter++;
    std::string threadName = ss.str();

    // Create worker thread. Note that it is created but is not yet running.
    WorkerThread.Create<ProxyWorker<mtsComponentProxy>, ThreadArguments<mtsComponentProxy>*>(
        &ProxyWorkerInfo, &ProxyWorker<mtsComponentProxy>::Run, &ThreadArgumentsInfo, threadName.c_str());

    return true;
}

void mtsComponentInterfaceProxyClient::StartClient()
{
    Sender->Start();

    // This is a blocking call that should be run in a different thread.
    IceCommunicator->waitForShutdown();
}

void mtsComponentInterfaceProxyClient::Runner(ThreadArguments<mtsComponentProxy> * arguments)
{
    mtsComponentInterfaceProxyClient * ProxyClient =
        dynamic_cast<mtsComponentInterfaceProxyClient*>(arguments->Proxy);
    if (!ProxyClient) {
        CMN_LOG_RUN_ERROR << "mtsComponentInterfaceProxyClient: failed to create a proxy client." << std::endl;
        return;
    }

    ProxyClient->GetLogger()->trace("mtsComponentInterfaceProxyClient", "proxy client starts");

    try {
        ProxyClient->SetAsActiveProxy();
        ProxyClient->StartClient();
    } catch (const Ice::Exception& e) {
        std::string error("mtsComponentInterfaceProxyClient: ");
        error += e.what();
        ProxyClient->GetLogger()->error(error);
    } catch (const char * msg) {
        std::string error("mtsComponentInterfaceProxyClient: ");
        error += msg;
        ProxyClient->GetLogger()->error(error);
    } catch (...) {
        std::string error("mtsComponentInterfaceProxyClient: exception at mtsComponentInterfaceProxyClient::Runner()");
        ProxyClient->GetLogger()->error(error);
    } 

    ProxyClient->GetLogger()->trace("mtsComponentInterfaceProxyClient", "Proxy client terminates");

    ProxyClient->Stop();
}

void mtsComponentInterfaceProxyClient::Stop()
{
    if (!IsActiveProxy()) return;

    LogPrint(mtsComponentInterfaceProxyClient, "ComponentInterfaceProxy client stops.");

    // Let a server disconnect this client safely.
    //ManagerServerProxy->Shutdown();
    //ComponentInterfaceServerProxy->ice_getConnection()->close(false); // close gracefully

    try {
        BaseClientType::Stop();
    } catch (const Ice::Exception& e) {
        std::string error("mtsComponentInterfaceProxyClient: ");
        error += e.what();
        LogError(mtsManagerProxyClient, error);
    }
}

bool mtsComponentInterfaceProxyClient::OnServerDisconnect()
{
    Stop();

    return true;
}

bool mtsComponentInterfaceProxyClient::AddPerEventSerializer(const CommandIDType commandID, mtsProxySerializer * serializer)
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

void mtsComponentInterfaceProxyClient::ReceiveExecuteCommandVoid(const CommandIDType commandID, const mtsBlockingType blocking)
{
    mtsFunctionVoid * functionVoid = reinterpret_cast<mtsFunctionVoid *>(commandID);
    if (!functionVoid) {
        LogError(mtsComponentInterfaceProxyClient, "ReceiveExecuteCommandVoid: invalid proxy id of function void: " << commandID);
        return;
    }

    // Execute the command
        (*functionVoid)();
/*
    if (blocking == MTS_BLOCKING) {
        (*functionVoid).ExecuteBlocking();
    } else {
        (*functionVoid)();
    }
*/
}

void mtsComponentInterfaceProxyClient::ReceiveExecuteCommandWriteSerialized(const CommandIDType commandID, const std::string & serializedArgument, const mtsBlockingType blocking)
{
    mtsFunctionWriteProxy * functionWriteProxy = reinterpret_cast<mtsFunctionWriteProxy*>(commandID);
    if (!functionWriteProxy) {
        LogError(mtsComponentInterfaceProxyClient, "ReceiveExecuteCommandWriteSerialized: invalid proxy id of function write: " << commandID);
        return;
    }

    // Deserialize
    mtsProxySerializer * deserializer = functionWriteProxy->GetSerializer();
    mtsGenericObject * argument = deserializer->DeSerialize(serializedArgument);
    if (!argument) {
        LogError(mtsComponentInterfaceProxyClient, "ReceiveExecuteCommandWriteSerialized: Deserialization failed");
        return;
    }

    // Execute the command
        (*functionWriteProxy)(*argument);
/*
    if (blocking == MTS_BLOCKING) {
        (*functionWriteProxy).ExecuteBlocking(*argument);
    } else {
        (*functionWriteProxy)(*argument);
    }
*/
}

void mtsComponentInterfaceProxyClient::ReceiveExecuteCommandReadSerialized(const CommandIDType commandID, std::string & serializedArgument)
{
    mtsFunctionReadProxy * functionReadProxy = reinterpret_cast<mtsFunctionReadProxy*>(commandID);
    if (!functionReadProxy) {
        LogError(mtsComponentInterfaceProxyClient, "ReceiveExecuteCommandReadSerialized: invalid proxy id of function read: " << commandID);
        return;
    }

    // Create a temporary argument which includes dynamic allocation internally.
    // Therefore, this object should be deallocated manually.
    mtsGenericObject * tempArgument =
        dynamic_cast<mtsGenericObject *>(functionReadProxy->GetCommand()->GetArgumentPrototype()->Services()->Create());
    if (!tempArgument) {
        LogError(mtsComponentInterfaceProxyClient, "ReceiveExecuteCommandReadSerialized: failed to create a temporary argument");
        return;
    }

    // Execute the command
    (*functionReadProxy)(*tempArgument);

    // Serialize
    mtsProxySerializer * serializer = functionReadProxy->GetSerializer();
    serializer->Serialize(*tempArgument, serializedArgument);

    // Deallocate memory
    delete tempArgument;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyClient, "ReceiveExecuteCommandReadSerialized: sent " << serializedArgument.size() << " bytes");
#endif
}

void mtsComponentInterfaceProxyClient::ReceiveExecuteCommandQualifiedReadSerialized(const CommandIDType commandID, const std::string & serializedArgumentIn, std::string & serializedArgumentOut)
{
    mtsFunctionQualifiedReadProxy * functionQualifiedReadProxy = reinterpret_cast<mtsFunctionQualifiedReadProxy*>(commandID);
    if (!functionQualifiedReadProxy) {
        LogError(mtsComponentInterfaceProxyClient, "ReceiveExecuteCommandQualifiedReadSerialized: invalid proxy id of function qualified read: " << commandID);
        return;
    }

    // Deserialize
    mtsProxySerializer * deserializer = functionQualifiedReadProxy->GetSerializer();
    mtsGenericObject * argumentIn = deserializer->DeSerialize(serializedArgumentIn);
    if (!argumentIn) {
        LogError(mtsComponentInterfaceProxyClient, "ReceiveExecuteCommandQualifiedReadSerialized: Deserialization failed");
        return;
    }

    // Create a temporary argument which includes dynamic allocation internally.
    // Therefore, this object should be deallocated manually.
    mtsGenericObject * tempArgumentOut = dynamic_cast<mtsGenericObject *>(
        functionQualifiedReadProxy->GetCommand()->GetArgument2ClassServices()->Create());
    if (!tempArgumentOut) {
        LogError(mtsComponentInterfaceProxyClient, "ReceiveExecuteCommandQualifiedReadSerialized: failed to create a temporary argument");
        return;
    }

    // Execute the command
    (*functionQualifiedReadProxy)(*argumentIn, *tempArgumentOut);

    // Serialize
    deserializer->Serialize(*tempArgumentOut, serializedArgumentOut);

    // Deallocate memory
    delete tempArgumentOut;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyClient, "ReceiveExecuteCommandQualifiedReadSerialized: sent " << serializedArgumentOut.size() << " bytes");
#endif
}

//-------------------------------------------------------------------------
//  Event Generators (Event Sender) : Client -> Server
//-------------------------------------------------------------------------
void mtsComponentInterfaceProxyClient::SendTestMessageFromClientToServer(const std::string & str) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyClient, ">>>>> SEND: MessageFromClientToServer");
#endif

    ComponentInterfaceServerProxy->TestMessageFromClientToServer(str);
}

bool mtsComponentInterfaceProxyClient::SendFetchEventGeneratorProxyPointers(
    const std::string & clientComponentName, const std::string & requiredInterfaceName,
    mtsComponentInterfaceProxy::EventGeneratorProxyPointerSet & eventGeneratorProxyPointers)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyClient, ">>>>> SEND: FetchEventGeneratorProxyPointers: " << clientComponentName << ":" << requiredInterfaceName);
#endif

    try {
        return ComponentInterfaceServerProxy->FetchEventGeneratorProxyPointers(
            clientComponentName, requiredInterfaceName, eventGeneratorProxyPointers);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsComponentInterfaceProxyServer, "SendFetchEventGeneratorProxyPointers: network exception: " << ex);
        OnServerDisconnect();
        return false;
    }
}

bool mtsComponentInterfaceProxyClient::SendExecuteEventVoid(const CommandIDType commandID)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyClient, ">>>>> SEND: SendExecuteEventVoid: " << commandID);
#endif

    try {
        ComponentInterfaceServerProxy->ExecuteEventVoid(commandID);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsComponentInterfaceProxyServer, "SendExecuteEventVoid: network exception: " << ex);
        OnServerDisconnect();
        return false;
    }

    return true;
}

bool mtsComponentInterfaceProxyClient::SendExecuteEventWriteSerialized(const CommandIDType commandID, const mtsGenericObject & argument)
{
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
        LogError(mtsComponentInterfaceProxyServer, "SendExecuteEventWriteSerialized: network exception: " << ex);
        OnServerDisconnect();
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
    mtsComponentInterfaceProxyClient * componentInterfaceClient)
    : Communicator(communicator),
      SenderThreadPtr(new SenderThread<ComponentInterfaceClientIPtr>(this)),
      IceLogger(logger),
      ComponentInterfaceProxyClient(componentInterfaceClient),
      Server(server)
{
}

mtsComponentInterfaceProxyClient::ComponentInterfaceClientI::~ComponentInterfaceClientI()
{
    Stop();

    // Sleep for some time enough for Run() loop to terminate
    osaSleep(1 * cmn_s);
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
    while (this->IsActiveProxy())
    {
        osaSleep(5 * cmn_s);

        try {
            Server->Refresh();
        } catch (const ::Ice::Exception & ex) {
            LogPrint(mtsComponentInterfaceProxyClient, "Refresh failed: " << Server->ice_toString() << "\n" << ex);
            if (ComponentInterfaceProxyClient) {
                ComponentInterfaceProxyClient->OnServerDisconnect();
            }
        }
    }
#endif
}

void mtsComponentInterfaceProxyClient::ComponentInterfaceClientI::Stop()
{
    if (!IsActiveProxy()) return;

    LogPrint(ComponentInterfaceClientI, "Stop and destroy callback sender");

    ComponentInterfaceProxyClient = NULL;

    IceUtil::ThreadPtr callbackSenderThread;
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);

        notify();

        callbackSenderThread = SenderThreadPtr;
        SenderThreadPtr = 0;
    }
    callbackSenderThread->getThreadControl().join();
}

//-----------------------------------------------------------------------------
//  Network Event handlers (Server -> Client)
//-----------------------------------------------------------------------------
void mtsComponentInterfaceProxyClient::ComponentInterfaceClientI::TestMessageFromServerToClient(
    const std::string & str, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyClient, "<<<<< RECV: TestMessageFromServerToClient");
#endif

    ComponentInterfaceProxyClient->ReceiveTestMessageFromServerToClient(str);
}

bool mtsComponentInterfaceProxyClient::ComponentInterfaceClientI::FetchFunctionProxyPointers(
    const std::string & requiredInterfaceName, mtsComponentInterfaceProxy::FunctionProxyPointerSet & functionProxyPointers,
    const ::Ice::Current & CMN_UNUSED(current)) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyClient, "<<<<< RECV: FetchFunctionProxyPointers: " << requiredInterfaceName);
#endif

    return ComponentInterfaceProxyClient->ReceiveFetchFunctionProxyPointers(requiredInterfaceName, functionProxyPointers);
}

void mtsComponentInterfaceProxyClient::ComponentInterfaceClientI::ExecuteCommandVoid(
    ::Ice::Long commandID, bool blocking, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyClient, "<<<<< RECV: ExecuteCommandVoid: " << commandID << ", "
        << (blocking ? "BLOCKING" : "NON-BLOCKING"));
#endif

    if (blocking) {
        ComponentInterfaceProxyClient->ReceiveExecuteCommandVoid(commandID, MTS_BLOCKING);
    } else {
        ComponentInterfaceProxyClient->ReceiveExecuteCommandVoid(commandID, MTS_NOT_BLOCKING);
    }
}

void mtsComponentInterfaceProxyClient::ComponentInterfaceClientI::ExecuteCommandWriteSerialized(
    ::Ice::Long commandID, const ::std::string & argument, bool blocking, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyClient, "<<<<< RECV: ExecuteCommandWriteSerialized: " << commandID << ", " 
        << argument.size() << ", " << (blocking ? "BLOCKING" : "NON-BLOCKING"));
#endif

    if (blocking) {
        ComponentInterfaceProxyClient->ReceiveExecuteCommandWriteSerialized(commandID, argument, MTS_BLOCKING);
    } else {
        ComponentInterfaceProxyClient->ReceiveExecuteCommandWriteSerialized(commandID, argument, MTS_NOT_BLOCKING);
    }
}

void mtsComponentInterfaceProxyClient::ComponentInterfaceClientI::ExecuteCommandReadSerialized(
    ::Ice::Long commandID, ::std::string & argument, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyClient, "<<<<< RECV: ExecuteCommandReadSerialized: " << commandID << ", " << argument.size());
#endif

    ComponentInterfaceProxyClient->ReceiveExecuteCommandReadSerialized(commandID, argument);
}

void mtsComponentInterfaceProxyClient::ComponentInterfaceClientI::ExecuteCommandQualifiedReadSerialized(
    ::Ice::Long commandID, const ::std::string & argumentIn, ::std::string & argumentOut, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsComponentInterfaceProxyClient, "<<<<< RECV: ExecuteCommandQualifiedReadSerialized: " << commandID << ", " << argumentIn.size());
#endif

    ComponentInterfaceProxyClient->ReceiveExecuteCommandQualifiedReadSerialized(commandID, argumentIn, argumentOut);
}
