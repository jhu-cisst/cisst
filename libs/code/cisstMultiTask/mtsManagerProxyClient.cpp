/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung
  Created on: 2010-01-20

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsManagerProxyClient.h>
#include <cisstMultiTask/mtsManagerProxyServer.h>
#include <cisstMultiTask/mtsFunctionVoid.h>

#include <cisstOSAbstraction/osaSleep.h>

CMN_IMPLEMENT_SERVICES(mtsManagerProxyClient);

unsigned int mtsManagerProxyClient::InstanceCounter = 0;

mtsManagerProxyClient::mtsManagerProxyClient(const std::string & serverEndpointInfo)
    : BaseClientType(ICE_PROPERTY_FILE_ROOT"config.LCM", serverEndpointInfo)
{
    ProxyName = "ManagerProxyClient";
}

mtsManagerProxyClient::~mtsManagerProxyClient()
{
    Stop();
}

//-----------------------------------------------------------------------------
//  Proxy Start-up
//-----------------------------------------------------------------------------
bool mtsManagerProxyClient::Start(mtsManagerLocal * proxyOwner)
{
    // Initialize Ice object.
    BaseClientType::IceInitialize();

    if (!InitSuccessFlag) {
        LogError(mtsManagerProxyClient, "ICE proxy initialization failed");
        return false;
    }

    // Client configuration for bidirectional communication
    Ice::ObjectAdapterPtr adapter = IceCommunicator->createObjectAdapter("");
    Ice::Identity ident;
    ident.name = GetGUID();
    ident.category = "";

    mtsManagerProxy::ManagerClientPtr client =
        new ManagerClientI(IceCommunicator, IceLogger, ManagerServerProxy, this);
    adapter->add(client, ident);
    adapter->activate();
    ManagerServerProxy->ice_getConnection()->setAdapter(adapter);

    // Set an implicit context (per proxy context)
    IceCommunicator->getImplicitContext()->put(
        mtsManagerProxyServer::GetConnectionIDKey(), IceCommunicator->identityToString(ident));

    // Set proxy owner and name of this proxy object
    SetProxyOwner(proxyOwner);

    // Connect to server proxy through adding this ICE proxy to server proxy
    if (!ManagerServerProxy->AddClient(GetProxyName(), ident)) {
        LogError(mtsManagerProxyClient, "AddClient() failed: duplicate proxy name or identity");
        return false;
    }

    // Create a worker thread here but is not running yet.
    ThreadArgumentsInfo.Proxy = this;
    ThreadArgumentsInfo.Runner = mtsManagerProxyClient::Runner;

    // Set a short name of this thread as MPC which means "Manager Proxy Client."
    // Such a condensed naming rule is required because a total number of
    // characters in a thread name is sometimes limited to a small number (e.g.
    // LINUX RTAI).
    std::stringstream ss;
    ss << "MPC" << mtsManagerProxyClient::InstanceCounter++;
    std::string threadName = ss.str();

    // Create worker thread. Note that it is created but is not yet running.
    WorkerThread.Create<ProxyWorker<mtsManagerLocal>, ThreadArguments<mtsManagerLocal>*>(
        &ProxyWorkerInfo, &ProxyWorker<mtsManagerLocal>::Run, &ThreadArgumentsInfo, threadName.c_str());

    return true;
}

void mtsManagerProxyClient::StartClient()
{
    Sender->Start();

    // This is a blocking call that should be run in a different thread.
    IceCommunicator->waitForShutdown();
}

void mtsManagerProxyClient::Runner(ThreadArguments<mtsManagerLocal> * arguments)
{
    mtsManagerProxyClient * ProxyClient =
        dynamic_cast<mtsManagerProxyClient*>(arguments->Proxy);
    if (!ProxyClient) {
        CMN_LOG_RUN_ERROR << "mtsManagerProxyClient: failed to create a proxy client." << std::endl;
        return;
    }

    ProxyClient->GetLogger()->trace("mtsManagerProxyClient", "proxy client starts");

    try {
        ProxyClient->SetAsActiveProxy();
        ProxyClient->StartClient();
    } catch (const Ice::Exception& e) {
        std::string error("mtsManagerProxyClient: ");
        error += e.what();
        ProxyClient->GetLogger()->error(error);
    } catch (const char * msg) {
        std::string error("mtsManagerProxyClient: ");
        error += msg;
        ProxyClient->GetLogger()->error(error);
    }

    ProxyClient->GetLogger()->trace("mtsManagerProxyClient", "proxy client terminates");

    ProxyClient->Stop();
}

void mtsManagerProxyClient::Stop()
{
    if (!IsActiveProxy()) return;

    LogPrint(mtsManagerProxyClient, "ManagerProxy client stops.");

    // Let a server disconnect this client safely.
    //ManagerServerProxy->Shutdown();
    //ManagerServerProxy->ice_getConnection()->close(false); // close gracefully

    BaseClientType::Stop();
}

bool mtsManagerProxyClient::OnServerDisconnect()
{
    Stop();

    return true;
}

void mtsManagerProxyClient::GetConnectionStringSet(mtsManagerProxy::ConnectionStringSet & connectionStringSet,
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName,
    const std::string & requestProcessName)
{
    connectionStringSet.ClientProcessName = clientProcessName;
    connectionStringSet.ClientComponentName = clientComponentName;
    connectionStringSet.ClientRequiredInterfaceName = clientRequiredInterfaceName;
    connectionStringSet.ServerProcessName = serverProcessName;
    connectionStringSet.ServerComponentName = serverComponentName;
    connectionStringSet.ServerProvidedInterfaceName = serverProvidedInterfaceName;
    connectionStringSet.RequestProcessName = requestProcessName;
}

//-------------------------------------------------------------------------
//  Implementation of mtsManagerGlobalInterface
//  (See mtsManagerGlobalInterface.h for details)
//-------------------------------------------------------------------------
bool mtsManagerProxyClient::AddProcess(const std::string & processName)
{
    return SendAddProcess(processName);
}

bool mtsManagerProxyClient::FindProcess(const std::string & processName) const
{
    return const_cast<mtsManagerProxyClient*>(this)->SendFindProcess(processName);
}

bool mtsManagerProxyClient::RemoveProcess(const std::string & processName)
{
    return SendRemoveProcess(processName);
}

bool mtsManagerProxyClient::AddComponent(const std::string & processName, const std::string & componentName)
{
    return SendAddComponent(processName, componentName);
}

bool mtsManagerProxyClient::FindComponent(const std::string & processName, const std::string & componentName) const
{
    return const_cast<mtsManagerProxyClient*>(this)->SendFindComponent(processName, componentName);
}

bool mtsManagerProxyClient::RemoveComponent(const std::string & processName, const std::string & componentName)
{
    return SendRemoveComponent(processName, componentName);
}

bool mtsManagerProxyClient::AddProvidedInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName, const bool isProxyInterface)
{
    return SendAddProvidedInterface(processName, componentName, interfaceName, isProxyInterface);
}

bool mtsManagerProxyClient::AddRequiredInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName, const bool isProxyInterface)
{
    return SendAddRequiredInterface(processName, componentName, interfaceName, isProxyInterface);
}

bool mtsManagerProxyClient::FindProvidedInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName) const
{
    return const_cast<mtsManagerProxyClient*>(this)->SendFindProvidedInterface(processName, componentName, interfaceName);
}

bool mtsManagerProxyClient::FindRequiredInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName) const
{
    return const_cast<mtsManagerProxyClient*>(this)->SendFindRequiredInterface(processName, componentName, interfaceName);
}

bool mtsManagerProxyClient::RemoveProvidedInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName)
{
    return SendRemoveProvidedInterface(processName, componentName, interfaceName);
}

bool mtsManagerProxyClient::RemoveRequiredInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName)
{
    return SendRemoveRequiredInterface(processName, componentName, interfaceName);
}

unsigned int mtsManagerProxyClient::Connect(const std::string & requestProcessName,
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName)
{
    mtsManagerProxy::ConnectionStringSet connectionStringSet;
    GetConnectionStringSet(connectionStringSet,
                           clientProcessName, clientComponentName, clientRequiredInterfaceName,
                           serverProcessName, serverComponentName, serverProvidedInterfaceName,
                           requestProcessName);

    return SendConnect(connectionStringSet);
}

bool mtsManagerProxyClient::ConnectConfirm(unsigned int connectionSessionID)
{
    return SendConnectConfirm(connectionSessionID);
}

bool mtsManagerProxyClient::Disconnect(
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName)
{
    mtsManagerProxy::ConnectionStringSet connectionStringSet;
    GetConnectionStringSet(connectionStringSet,
                           clientProcessName, clientComponentName, clientRequiredInterfaceName,
                           serverProcessName, serverComponentName, serverProvidedInterfaceName);

    return SendDisconnect(connectionStringSet);
}

bool mtsManagerProxyClient::SetProvidedInterfaceProxyAccessInfo(
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName,
    const std::string & endpointInfo)
{
    mtsManagerProxy::ConnectionStringSet connectionStringSet;
    GetConnectionStringSet(connectionStringSet,
                           clientProcessName, clientComponentName, clientRequiredInterfaceName,
                           serverProcessName, serverComponentName, serverProvidedInterfaceName);

    return SendSetProvidedInterfaceProxyAccessInfo(connectionStringSet, endpointInfo);
}

bool mtsManagerProxyClient::GetProvidedInterfaceProxyAccessInfo(
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName,
    std::string & endpointInfo)
{
    mtsManagerProxy::ConnectionStringSet connectionStringSet;
    GetConnectionStringSet(connectionStringSet,
                           clientProcessName, clientComponentName, clientRequiredInterfaceName,
                           serverProcessName, serverComponentName, serverProvidedInterfaceName);

    return SendGetProvidedInterfaceProxyAccessInfo(connectionStringSet, endpointInfo);
}

bool mtsManagerProxyClient::InitiateConnect(const unsigned int connectionID,
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName)
{
    mtsManagerProxy::ConnectionStringSet connectionStringSet;
    GetConnectionStringSet(connectionStringSet,
                           clientProcessName, clientComponentName, clientRequiredInterfaceName,
                           serverProcessName, serverComponentName, serverProvidedInterfaceName);

    return SendInitiateConnect(connectionID, connectionStringSet);
}

bool mtsManagerProxyClient::ConnectServerSideInterface(const unsigned int providedInterfaceProxyInstanceID,
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName)
{
    ::Ice::Int thisProvidedInterfaceProxyInstanceID = providedInterfaceProxyInstanceID;

    mtsManagerProxy::ConnectionStringSet connectionStringSet;
    GetConnectionStringSet(connectionStringSet,
                           clientProcessName, clientComponentName, clientRequiredInterfaceName,
                           serverProcessName, serverComponentName, serverProvidedInterfaceName);

    return SendConnectServerSideInterface(thisProvidedInterfaceProxyInstanceID, connectionStringSet);
}

//-------------------------------------------------------------------------
//  Event Handlers (Server -> Client)
//-------------------------------------------------------------------------
void mtsManagerProxyClient::ReceiveTestMessageFromServerToClient(const std::string & str) const
{
    std::cout << "Client received (Server -> Client): " << str << std::endl;
}

bool mtsManagerProxyClient::ReceiveCreateComponentProxy(const ::std::string & componentProxyName)
{
    return ProxyOwner->CreateComponentProxy(componentProxyName);
}

bool mtsManagerProxyClient::ReceiveRemoveComponentProxy(const ::std::string & componentProxyName)
{
    return ProxyOwner->RemoveComponentProxy(componentProxyName);
}

bool mtsManagerProxyClient::ReceiveCreateProvidedInterfaceProxy(const std::string & serverComponentProxyName, const ::mtsManagerProxy::ProvidedInterfaceDescription & providedInterfaceDescription)
{
    // Convert providedInterfaceDescription into an object of type mtsInterfaceCommon::ProvidedInterfaceDescription
    ProvidedInterfaceDescription convertedDescription;
    mtsManagerProxyServer::ConvertProvidedInterfaceDescription(providedInterfaceDescription, convertedDescription);

    return ProxyOwner->CreateProvidedInterfaceProxy(serverComponentProxyName, convertedDescription);
}

bool mtsManagerProxyClient::ReceiveCreateRequiredInterfaceProxy(const std::string & clientComponentProxyName, const ::mtsManagerProxy::RequiredInterfaceDescription & requiredInterfaceDescription)
{
    // Convert requiredInterfaceDescription into an object of type mtsInterfaceCommon::RequiredInterfaceDescription
    RequiredInterfaceDescription convertedDescription;
    mtsManagerProxyServer::ConvertRequiredInterfaceDescription(requiredInterfaceDescription, convertedDescription);

    return ProxyOwner->CreateRequiredInterfaceProxy(clientComponentProxyName, convertedDescription);
}

bool mtsManagerProxyClient::ReceiveRemoveProvidedInterfaceProxy(const std::string & clientComponentProxyName, const std::string & providedInterfaceProxyName)
{
    return ProxyOwner->RemoveProvidedInterfaceProxy(clientComponentProxyName, providedInterfaceProxyName);
}

bool mtsManagerProxyClient::ReceiveRemoveRequiredInterfaceProxy(const std::string & serverComponentProxyName, const std::string & requiredInterfaceProxyName)
{
    return ProxyOwner->RemoveRequiredInterfaceProxy(serverComponentProxyName, requiredInterfaceProxyName);
}

bool mtsManagerProxyClient::ReceiveConnectServerSideInterface(::Ice::Int providedInterfaceProxyInstanceID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet)
{
    return ProxyOwner->ConnectServerSideInterface(providedInterfaceProxyInstanceID,
        connectionStringSet.ClientProcessName, connectionStringSet.ClientComponentName, connectionStringSet.ClientRequiredInterfaceName,
        connectionStringSet.ServerProcessName, connectionStringSet.ServerComponentName, connectionStringSet.ServerProvidedInterfaceName);
}

bool mtsManagerProxyClient::ReceiveConnectClientSideInterface(::Ice::Int connectionID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet)
{
    return ProxyOwner->ConnectClientSideInterface(connectionID,
        connectionStringSet.ClientProcessName, connectionStringSet.ClientComponentName, connectionStringSet.ClientRequiredInterfaceName,
        connectionStringSet.ServerProcessName, connectionStringSet.ServerComponentName, connectionStringSet.ServerProvidedInterfaceName);
}

bool mtsManagerProxyClient::ReceiveGetProvidedInterfaceDescription(const std::string & componentName, const std::string & providedInterfaceName, ::mtsManagerProxy::ProvidedInterfaceDescription & providedInterfaceDescription)
{
    ProvidedInterfaceDescription src;

    if (!ProxyOwner->GetProvidedInterfaceDescription(componentName, providedInterfaceName, src)) {
        LogError(mtsManagerProxyClient, "ReceiveGetProvidedInterfaceDescription() failed");
        return false;
    }

    // Construct an instance of type ProvidedInterfaceDescription from an object of type mtsInterfaceCommon::ProvidedInterfaceDescription
    mtsManagerProxyServer::ConstructProvidedInterfaceDescriptionFrom(src, providedInterfaceDescription);

    return true;
}

bool mtsManagerProxyClient::ReceiveGetRequiredInterfaceDescription(const std::string & componentName, const std::string & requiredInterfaceName, ::mtsManagerProxy::RequiredInterfaceDescription & requiredInterfaceDescription)
{
    RequiredInterfaceDescription src;

    if (!ProxyOwner->GetRequiredInterfaceDescription(componentName, requiredInterfaceName, src)) {
        LogError(mtsManagerProxyClient, "ReceiveGetRequiredInterfaceDescription() failed");
        return false;
    }

    // Construct an instance of type RequiredInterfaceDescription from an object of type mtsInterfaceCommon::RequiredInterfaceDescription
    mtsManagerProxyServer::ConstructRequiredInterfaceDescriptionFrom(src, requiredInterfaceDescription);

    return true;
}

void mtsManagerProxyClient::ReceiveGetNamesOfCommands(const std::string & componentName, const std::string & providedInterfaceName, ::mtsManagerProxy::NamesOfCommandsSequence & names) const
{
    ProxyOwner->GetNamesOfCommands(names, componentName, providedInterfaceName);
}

void mtsManagerProxyClient::ReceiveGetNamesOfEventGenerators(const std::string & componentName, const std::string & providedInterfaceName, ::mtsManagerProxy::NamesOfEventGeneratorsSequence & names) const
{
    ProxyOwner->GetNamesOfEventGenerators(names, componentName, providedInterfaceName);
}

void mtsManagerProxyClient::ReceiveGetNamesOfFunctions(const std::string & componentName, const std::string & requiredInterfaceName, ::mtsManagerProxy::NamesOfFunctionsSequence & names) const
{
    ProxyOwner->GetNamesOfFunctions(names, componentName, requiredInterfaceName);
}

void mtsManagerProxyClient::ReceiveGetNamesOfEventHandlers(const std::string & componentName, const std::string & requiredInterfaceName, ::mtsManagerProxy::NamesOfEventHandlersSequence & names) const
{
    ProxyOwner->GetNamesOfEventHandlers(names, componentName, requiredInterfaceName);
}

void mtsManagerProxyClient::ReceiveGetDescriptionOfCommand(const std::string & componentName, const std::string & providedInterfaceName, const std::string & commandName, std::string & description) const
{
    ProxyOwner->GetDescriptionOfCommand(description, componentName, providedInterfaceName, commandName);
}

void mtsManagerProxyClient::ReceiveGetDescriptionOfEventGenerator(const std::string & componentName, const std::string & providedInterfaceName, const std::string & eventGeneratorName, std::string & description) const
{
    ProxyOwner->GetDescriptionOfEventGenerator(description, componentName, providedInterfaceName, eventGeneratorName);
}

void mtsManagerProxyClient::ReceiveGetDescriptionOfFunction(const std::string & componentName, const std::string & requiredInterfaceName, const std::string & functionName, std::string & description) const
{
    ProxyOwner->GetDescriptionOfFunction(description, componentName, requiredInterfaceName, functionName);
}

void mtsManagerProxyClient::ReceiveGetDescriptionOfEventHandler(const std::string & componentName, const std::string & requiredInterfaceName, const std::string & eventHandlerName, std::string & description) const
{
    ProxyOwner->GetDescriptionOfEventHandler(description, componentName, requiredInterfaceName, eventHandlerName);
}

std::string mtsManagerProxyClient::ReceiveGetProcessName()
{
    return ProxyOwner->GetProcessName();
}

::Ice::Int mtsManagerProxyClient::ReceiveGetCurrentInterfaceCount(const std::string & componentName)
{
    return ProxyOwner->GetCurrentInterfaceCount(componentName);
}

//-------------------------------------------------------------------------
//  Event Generators (Event Sender) : Client -> Server
//-------------------------------------------------------------------------
void mtsManagerProxyClient::SendTestMessageFromClientToServer(const std::string & str) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: MessageFromClientToServer");
#endif

    ManagerServerProxy->TestMessageFromClientToServer(str);
}

bool mtsManagerProxyClient::SendAddProcess(const std::string & processName)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendAddProcess: " << processName);
#endif

    try {
        return ManagerServerProxy->AddProcess(processName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendAddProcess: network exception: " << ex);
        OnServerDisconnect();
        return false;
    }
}

bool mtsManagerProxyClient::SendFindProcess(const std::string & processName)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendFindProcess: " << processName);
#endif

    try {
        return ManagerServerProxy->FindProcess(processName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendFindProcess: network exception: " << ex);
        OnServerDisconnect();
        return false;
    }
}

bool mtsManagerProxyClient::SendRemoveProcess(const std::string & processName)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendRemoveProcess: " << processName);
#endif

    try {
        return ManagerServerProxy->RemoveProcess(processName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendRemoveProcess: network exception: " << ex);
        OnServerDisconnect();
        return false;
    }
}

bool mtsManagerProxyClient::SendAddComponent(const std::string & processName, const std::string & componentName)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendAddComponent: " << processName << ", " << componentName);
#endif

    try {
        return ManagerServerProxy->AddComponent(processName, componentName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendAddComponent: network exception: " << ex);
        OnServerDisconnect();
        return false;
    }
}

bool mtsManagerProxyClient::SendFindComponent(const std::string & processName, const std::string & componentName)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendFindComponent: " << processName << ", " << componentName);
#endif

    try {
        return ManagerServerProxy->FindComponent(processName, componentName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendFindComponent: network exception: " << ex);
        OnServerDisconnect();
        return false;
    }
}

bool mtsManagerProxyClient::SendRemoveComponent(const std::string & processName, const std::string & componentName)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendRemoveComponent: " << processName << ", " << componentName);
#endif

    try {
        return ManagerServerProxy->RemoveComponent(processName, componentName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendRemoveComponent: network exception: " << ex);
        OnServerDisconnect();
        return false;
    }
}

bool mtsManagerProxyClient::SendAddProvidedInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName, bool isProxyInterface)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendAddProvidedInterface: " << processName << ", " << componentName << ", " << interfaceName << ", " << isProxyInterface);
#endif

    try {
        return ManagerServerProxy->AddProvidedInterface(processName, componentName, interfaceName, isProxyInterface);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendAddProvidedInterface: network exception: " << ex);
        OnServerDisconnect();
        return false;
    }
}

bool mtsManagerProxyClient::SendFindProvidedInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendFindProvidedInterface: " << processName << ", " << componentName << ", " << interfaceName);
#endif

    try {
        return ManagerServerProxy->FindProvidedInterface(processName, componentName, interfaceName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendFindProvidedInterface: network exception: " << ex);
        OnServerDisconnect();
        return false;
    }
}

bool mtsManagerProxyClient::SendRemoveProvidedInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendRemoveProvidedInterface: " << processName << ", " << componentName << ", " << interfaceName);
#endif

    try {
        return ManagerServerProxy->RemoveProvidedInterface(processName, componentName, interfaceName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendRemoveProvidedInterface: network exception: " << ex);
        OnServerDisconnect();
        return false;
    }
}

bool mtsManagerProxyClient::SendAddRequiredInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName, bool isProxyInterface)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendAddRequiredInterface: " << processName << ", " << componentName << ", " << interfaceName << ", " << isProxyInterface);
#endif

    try {
        return ManagerServerProxy->AddRequiredInterface(processName, componentName, interfaceName, isProxyInterface);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendAddRequiredInterface: network exception: " << ex);
        OnServerDisconnect();
        return false;
    }
}

bool mtsManagerProxyClient::SendFindRequiredInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendFindRequiredInterface: " << processName << ", " << componentName << ", " << interfaceName);
#endif

    try {
        return ManagerServerProxy->FindRequiredInterface(processName, componentName, interfaceName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendFindRequiredInterface: network exception: " << ex);
        OnServerDisconnect();
        return false;
    }
}

bool mtsManagerProxyClient::SendRemoveRequiredInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendRemoveRequiredInterface: " << processName << ", " << componentName << ", " << interfaceName);
#endif

    try {
        return ManagerServerProxy->RemoveRequiredInterface(processName, componentName, interfaceName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendRemoveRequiredInterface: network exception: " << ex);
        OnServerDisconnect();
        return false;
    }
}

::Ice::Int mtsManagerProxyClient::SendConnect(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendConnectConfirm: " << connectionStringSet.ClientProcessName << ", " << connectionStringSet.ServerProcessName);
#endif

    try {
        return ManagerServerProxy->Connect(connectionStringSet);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendConnect: network exception: " << ex);
        OnServerDisconnect();
        return 0; // 0 is defined as error in mtsManagerGlobalInterface::CONNECT_ERROR
    }
}

bool mtsManagerProxyClient::SendConnectConfirm(::Ice::Int connectionSessionID)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendConnectConfirm: " << connectionSessionID);
#endif

    try {
        return ManagerServerProxy->ConnectConfirm(connectionSessionID);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendConnectConfirm: network exception: " << ex);
        OnServerDisconnect();
        return false;
    }
}

bool mtsManagerProxyClient::SendDisconnect(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendDisconnect: " << connectionStringSet.ClientProcessName << ", " << connectionStringSet.ServerProcessName);
#endif

    try {
        return ManagerServerProxy->Disconnect(connectionStringSet);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendDisconnect: network exception: " << ex);
        OnServerDisconnect();
        return false;
    }
}

bool mtsManagerProxyClient::SendSetProvidedInterfaceProxyAccessInfo(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, const std::string & endpointInfo)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendSetProvidedInterfaceProxyAccessInfo: " << connectionStringSet.ClientProcessName << ", " << connectionStringSet.ServerProcessName << ", " << endpointInfo);
#endif

    try {
        return ManagerServerProxy->SetProvidedInterfaceProxyAccessInfo(connectionStringSet, endpointInfo);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendSetProvidedInterfaceProxyAccessInfo: network exception: " << ex);
        OnServerDisconnect();
        return false;
    }
}

bool mtsManagerProxyClient::SendGetProvidedInterfaceProxyAccessInfo(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, std::string & endpointInfo)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendGetProvidedInterfaceProxyAccessInfo: " << connectionStringSet.ClientProcessName << ", " << connectionStringSet.ServerProcessName);
#endif

    try {
        return ManagerServerProxy->GetProvidedInterfaceProxyAccessInfo(connectionStringSet, endpointInfo);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendGetProvidedInterfaceProxyAccessInfo: network exception: " << ex);
        OnServerDisconnect();
        return false;
    }
}

bool mtsManagerProxyClient::SendInitiateConnect(::Ice::Int connectionID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendInitiateConnect: " << connectionID << ", " << connectionStringSet.ClientProcessName << ", " << connectionStringSet.ServerProcessName);
#endif

    try {
        return ManagerServerProxy->InitiateConnect(connectionID, connectionStringSet);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendInitiateConnect: network exception: " << ex);
        OnServerDisconnect();
        return false;
    }
}

bool mtsManagerProxyClient::SendConnectServerSideInterface(::Ice::Int providedInterfaceProxyInstanceID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendConnectServerSideInterface: " << providedInterfaceProxyInstanceID << ", " << connectionStringSet.ClientProcessName << ", " << connectionStringSet.ServerProcessName);
#endif

    try {
        return ManagerServerProxy->ConnectServerSideInterface(providedInterfaceProxyInstanceID, connectionStringSet);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendConnectServerSideInterface: network exception: " << ex);
        OnServerDisconnect();
        return false;
    }
}

//-------------------------------------------------------------------------
//  Definition by mtsManagerProxy.ice
//-------------------------------------------------------------------------
mtsManagerProxyClient::ManagerClientI::ManagerClientI(
    const Ice::CommunicatorPtr& communicator,
    const Ice::LoggerPtr& logger,
    const mtsManagerProxy::ManagerServerPrx& server,
    mtsManagerProxyClient * ManagerClient)
    : Communicator(communicator),
      SenderThreadPtr(new SenderThread<ManagerClientIPtr>(this)),
      IceLogger(logger),
      ManagerProxyClient(ManagerClient),
      Server(server)
{
}

mtsManagerProxyClient::ManagerClientI::~ManagerClientI()
{
    Stop();

    // Sleep for some time enough for Run() loop to terminate
    osaSleep(1 * cmn_s);
}

void mtsManagerProxyClient::ManagerClientI::Start()
{
    ManagerProxyClient->GetLogger()->trace("mtsManagerProxyClient", "Send thread starts");

    SenderThreadPtr->start();
}

//#define _COMMUNICATION_TEST_
void mtsManagerProxyClient::ManagerClientI::Run()
{
#ifdef _COMMUNICATION_TEST_
    int count = 0;

    while (IsActiveProxy()) {
        osaSleep(1 * cmn_s);
        std::cout << "\tClient [" << ManagerProxyClient->GetProxyName() << "] running (" << ++count << ")" << std::endl;

        std::stringstream ss;
        ss << "Msg " << count << " from Client " << ManagerProxyClient->GetProxyName();

        ManagerProxyClient->SendTestMessageFromClientToServer(ss.str());
    }
#else
    while (this->IsActiveProxy())
    {
        osaSleep(5 * cmn_s);

        try {
            Server->Refresh();
        } catch (const ::Ice::Exception & ex) {
            LogPrint(mtsManagerProxyClient, "Refresh failed: " << Server->ice_toString() << "\n" << ex);
            if (ManagerProxyClient) {
                ManagerProxyClient->OnServerDisconnect();
            }
        }
    }
#endif
}

void mtsManagerProxyClient::ManagerClientI::Stop()
{
    if (!IsActiveProxy()) return;

    LogPrint(ManagerClientI, "Stop and destroy callback sender");

    ManagerProxyClient = NULL;

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
void mtsManagerProxyClient::ManagerClientI::TestMessageFromServerToClient(
    const std::string & str, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: TestMessageFromServerToClient");
#endif

    ManagerProxyClient->ReceiveTestMessageFromServerToClient(str);
}

bool mtsManagerProxyClient::ManagerClientI::CreateComponentProxy(const std::string & componentProxyName, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: CreateComponentProxy: " << componentProxyName);
#endif

    return ManagerProxyClient->ReceiveCreateComponentProxy(componentProxyName);
}

bool mtsManagerProxyClient::ManagerClientI::RemoveComponentProxy(const std::string & componentProxyName, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: RemoveComponentProxy: " << componentProxyName);
#endif

    return ManagerProxyClient->ReceiveRemoveComponentProxy(componentProxyName);
}

bool mtsManagerProxyClient::ManagerClientI::CreateProvidedInterfaceProxy(const std::string & serverComponentProxyName, const ::mtsManagerProxy::ProvidedInterfaceDescription & providedInterfaceDescription, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: CreateProvidedInterfaceProxy: " << serverComponentProxyName << ", " << providedInterfaceDescription.ProvidedInterfaceName);
#endif

    return ManagerProxyClient->ReceiveCreateProvidedInterfaceProxy(serverComponentProxyName, providedInterfaceDescription);
}

bool mtsManagerProxyClient::ManagerClientI::CreateRequiredInterfaceProxy(const std::string & clientComponentProxyName, const ::mtsManagerProxy::RequiredInterfaceDescription & requiredInterfaceDescription, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: CreateRequiredInterfaceProxy: " << clientComponentProxyName << ", " << requiredInterfaceDescription.RequiredInterfaceName);
#endif

    return ManagerProxyClient->ReceiveCreateRequiredInterfaceProxy(clientComponentProxyName, requiredInterfaceDescription);
}

bool mtsManagerProxyClient::ManagerClientI::RemoveProvidedInterfaceProxy(const std::string & clientComponentProxyName, const std::string & providedInterfaceProxyName, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: RemoveProvidedInterfaceProxy: " << clientComponentProxyName << ", " << providedInterfaceProxyName);
#endif

    return ManagerProxyClient->ReceiveRemoveProvidedInterfaceProxy(clientComponentProxyName, providedInterfaceProxyName);
}

bool mtsManagerProxyClient::ManagerClientI::RemoveRequiredInterfaceProxy(const std::string & serverComponentProxyName, const std::string & requiredInterfaceProxyName, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: RemoveRequiredInterfaceProxy: " << serverComponentProxyName << ", " << requiredInterfaceProxyName);
#endif

    return ManagerProxyClient->ReceiveRemoveRequiredInterfaceProxy(serverComponentProxyName, requiredInterfaceProxyName);
}

bool mtsManagerProxyClient::ManagerClientI::ConnectServerSideInterface(::Ice::Int providedInterfaceProxyInstanceID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: ConnectServerSideInterface: " << providedInterfaceProxyInstanceID);
#endif

    return ManagerProxyClient->ReceiveConnectServerSideInterface(providedInterfaceProxyInstanceID, connectionStringSet);
}

bool mtsManagerProxyClient::ManagerClientI::ConnectClientSideInterface(::Ice::Int connectionID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: ConnectClientSideInterface: " << connectionID);
#endif

    return ManagerProxyClient->ReceiveConnectClientSideInterface(connectionID, connectionStringSet);
}

bool mtsManagerProxyClient::ManagerClientI::GetProvidedInterfaceDescription(const std::string & componentName, const std::string & providedInterfaceName, ::mtsManagerProxy::ProvidedInterfaceDescription & providedInterfaceDescription, const ::Ice::Current &) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: GetProvidedInterfaceDescription: " << componentName << ", " << providedInterfaceName << providedInterfaceDescription.ProvidedInterfaceName);
#endif

    return ManagerProxyClient->ReceiveGetProvidedInterfaceDescription(componentName, providedInterfaceName, providedInterfaceDescription);
}

bool mtsManagerProxyClient::ManagerClientI::GetRequiredInterfaceDescription(const std::string & componentName, const std::string & requiredInterfaceName, ::mtsManagerProxy::RequiredInterfaceDescription & requiredInterfaceDescription, const ::Ice::Current &) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: GetRequiredInterfaceDescription" << componentName << ", " << requiredInterfaceName << requiredInterfaceDescription.RequiredInterfaceName);
#endif

    return ManagerProxyClient->ReceiveGetRequiredInterfaceDescription(componentName, requiredInterfaceName, requiredInterfaceDescription);
}

std::string mtsManagerProxyClient::ManagerClientI::GetProcessName(const ::Ice::Current & CMN_UNUSED(current)) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
	LogPrint(ManagerClientI, "<<<<< RECV: GetProcessName ");
#endif

    return ManagerProxyClient->ReceiveGetProcessName();
}

::Ice::Int mtsManagerProxyClient::ManagerClientI::GetCurrentInterfaceCount(const ::std::string & componentName, const ::Ice::Current & CMN_UNUSED(current)) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: GetCurrentInterfaceCount: " << componentName);
#endif

    return ManagerProxyClient->ReceiveGetCurrentInterfaceCount(componentName);
}


void mtsManagerProxyClient::ManagerClientI::GetNamesOfCommands(const std::string & componentName, const std::string & providedInterfaceName, ::mtsManagerProxy::NamesOfCommandsSequence & names, const ::Ice::Current & CMN_UNUSED(current)) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: GetNamesOfCommands: " << componentName << ", " << providedInterfaceName);
#endif

    return ManagerProxyClient->ReceiveGetNamesOfCommands(componentName, providedInterfaceName, names);
}

void mtsManagerProxyClient::ManagerClientI::GetNamesOfEventGenerators(const std::string & componentName, const std::string & providedInterfaceName, ::mtsManagerProxy::NamesOfEventGeneratorsSequence & names, const ::Ice::Current & CMN_UNUSED(current)) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: GetNamesOfEventGenerators: " << componentName << ", " << providedInterfaceName);
#endif

    return ManagerProxyClient->ReceiveGetNamesOfEventGenerators(componentName, providedInterfaceName, names);
}

void mtsManagerProxyClient::ManagerClientI::GetNamesOfFunctions(const std::string & componentName, const std::string & requiredInterfaceName, ::mtsManagerProxy::NamesOfFunctionsSequence & names, const ::Ice::Current & CMN_UNUSED(current)) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: GetNamesOfFunctions: " << componentName << ", " << requiredInterfaceName);
#endif

    return ManagerProxyClient->ReceiveGetNamesOfFunctions(componentName, requiredInterfaceName, names);
}

void mtsManagerProxyClient::ManagerClientI::GetNamesOfEventHandlers(const std::string & componentName, const std::string & requiredInterfaceName, ::mtsManagerProxy::NamesOfEventHandlersSequence & names, const ::Ice::Current & CMN_UNUSED(current)) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: GetNamesOfEventHandlers: " << componentName << ", " << requiredInterfaceName);
#endif

    return ManagerProxyClient->ReceiveGetNamesOfEventHandlers(componentName, requiredInterfaceName, names);
}

void mtsManagerProxyClient::ManagerClientI::GetDescriptionOfCommand(const std::string & componentName, const std::string & providedInterfaceName, const std::string & commandName, std::string & description, const ::Ice::Current & CMN_UNUSED(current)) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: GetDescriptionOfCommand: " << componentName << ", " << providedInterfaceName << ", " << commandName);
#endif

    return ManagerProxyClient->ReceiveGetDescriptionOfCommand(componentName, providedInterfaceName, commandName, description);
}

void mtsManagerProxyClient::ManagerClientI::GetDescriptionOfEventGenerator(const std::string & componentName, const std::string & providedInterfaceName, const std::string & eventGeneratorName, std::string & description, const ::Ice::Current & CMN_UNUSED(current)) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: GetDescriptionOfEventGenerator: " << componentName << ", " << providedInterfaceName << ", " << eventGeneratorName);
#endif

    return ManagerProxyClient->ReceiveGetDescriptionOfEventGenerator(componentName, providedInterfaceName, eventGeneratorName, description);
}

void mtsManagerProxyClient::ManagerClientI::GetDescriptionOfFunction(const std::string & componentName, const std::string & requiredInterfaceName, const std::string & functionName, std::string & description, const ::Ice::Current & CMN_UNUSED(current)) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: GetDescriptionOfFunction: " << componentName << ", " << requiredInterfaceName << ", " << functionName);
#endif

    return ManagerProxyClient->ReceiveGetDescriptionOfFunction(componentName, requiredInterfaceName, functionName, description);
}

void mtsManagerProxyClient::ManagerClientI::GetDescriptionOfEventHandler(const std::string & componentName, const std::string & requiredInterfaceName, const std::string & eventHandlerName, std::string & description, const ::Ice::Current & CMN_UNUSED(current)) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: GetDescriptionOfEventHandler: " << componentName << ", " << requiredInterfaceName << ", " << eventHandlerName);
#endif

    return ManagerProxyClient->ReceiveGetDescriptionOfEventHandler(componentName, requiredInterfaceName, eventHandlerName, description);
}
