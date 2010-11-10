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

unsigned int mtsManagerProxyClient::InstanceCounter = 0;

void GetConnectionStringSet(mtsManagerProxy::ConnectionStringSet & connectionStringSet,
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName,
    const std::string & requestProcessName = "")
{
    connectionStringSet.ClientProcessName = clientProcessName;
    connectionStringSet.ClientComponentName = clientComponentName;
    connectionStringSet.ClientInterfaceRequiredName = clientInterfaceRequiredName;
    connectionStringSet.ServerProcessName = serverProcessName;
    connectionStringSet.ServerComponentName = serverComponentName;
    connectionStringSet.ServerInterfaceProvidedName = serverInterfaceProvidedName;
    connectionStringSet.RequestProcessName = requestProcessName;
}

mtsManagerProxyClient::mtsManagerProxyClient(const std::string & serverEndpointInfo)
    : ManagerServerProxy(0),
      BaseClientType("config.LCM", serverEndpointInfo)
{
    ProxyName = "ManagerProxyClient";
}

mtsManagerProxyClient::~mtsManagerProxyClient()
{
    StopProxy();
}

//-----------------------------------------------------------------------------
//  Proxy Start-up
//-----------------------------------------------------------------------------
bool mtsManagerProxyClient::StartProxy(mtsManagerLocal * proxyOwner)
{
    // Initialize Ice object
    IceInitialize();

    if (!InitSuccessFlag) {
        LogError(mtsManagerProxyClient, "ICE proxy client initialization failed");
        return false;
    }

    // Client configuration for bidirectional communication
    Ice::ObjectAdapterPtr adapter = IceCommunicator->createObjectAdapter("");
    Ice::Identity id;
    id.name = GetIceGUID();
    id.category = "";

    mtsManagerProxy::ManagerClientPtr client =
        new ManagerClientI(IceCommunicator, IceLogger, ManagerServerProxy, this);
    adapter->add(client, id);
    adapter->activate();
    ManagerServerProxy->ice_getConnection()->setAdapter(adapter);

    // Set an implicit context (per proxy context)
    IceCommunicator->getImplicitContext()->put(
        mtsManagerProxyServer::GetConnectionIDKey(), IceCommunicator->identityToString(id));

    // Set proxy owner and name of this proxy object
    SetProxyOwner(proxyOwner);

    // Connect to server proxy through adding this ICE proxy to server proxy
    if (!ManagerServerProxy->AddClient(GetProxyName(), id)) {
        LogError(mtsManagerProxyClient, "AddClient() failed: duplicate proxy name or identity");
        return false;
    }

    // Thread arguments for a worker thread
    ThreadArgumentsInfo.Proxy = this;
    ThreadArgumentsInfo.Runner = mtsManagerProxyClient::Runner;

    // Set a short name of this thread as "MPC" (Manager Proxy Client) to meet
    // the requirement that some of operating system have -- only a few characters
    // can be used as a thread name (e.g. Linux RTAI)
    std::stringstream ss;
    ss << "MPC" << mtsManagerProxyClient::InstanceCounter++;
    std::string threadName = ss.str();

    // Create worker thread (will get started later)
    WorkerThread.Create<ProxyWorker<mtsManagerLocal>, ThreadArguments<mtsManagerLocal>*>(
        &ProxyWorkerInfo, &ProxyWorker<mtsManagerLocal>::Run, &ThreadArgumentsInfo, threadName.c_str());

    return true;
}

void mtsManagerProxyClient::CreateProxy(void) 
{
    ManagerServerProxy = mtsManagerProxy::ManagerServerPrx::checkedCast(ProxyObject);
    if (!ManagerServerProxy) {
        throw "mtsManagerProxyClient: CreateProxy() failed - invalid proxy";
    }

    Server = new ManagerClientI(IceCommunicator, IceLogger, ManagerServerProxy, this);
}

void mtsManagerProxyClient::RemoveProxy(void) 
{
    Server->Stop();

    ManagerServerProxy = 0;
}

void mtsManagerProxyClient::StartClient(void)
{
    Server->Start();

    // This is a blocking call that should run in a different thread.
    IceCommunicator->waitForShutdown();
}

void mtsManagerProxyClient::Runner(ThreadArguments<mtsManagerLocal> * arguments)
{
    mtsManagerProxyClient * ProxyClient = dynamic_cast<mtsManagerProxyClient*>(arguments->Proxy);
    if (!ProxyClient) {
        CMN_LOG_RUN_ERROR << "mtsManagerProxyClient: failed to get proxy client" << std::endl;
        return;
    }

    ProxyClient->GetLogger()->trace("mtsManagerProxyClient", "proxy client starts");

    try {
        ProxyClient->ChangeProxyState(PROXY_STATE_ACTIVE);
        ProxyClient->StartClient();
    } catch (const Ice::Exception& e) {
        std::string error("mtsManagerProxyClient: ");
        error += e.what();
        ProxyClient->GetLogger()->error(error);
    } catch (...) {
        std::string error("mtsManagerProxyClient: exception at mtsManagerProxyClient::Runner()");
        ProxyClient->GetLogger()->error(error);
    } 

    ProxyClient->GetLogger()->trace("mtsManagerProxyClient", "proxy client terminates");

    ProxyClient->StopProxy();
}

void mtsManagerProxyClient::StopProxy()
{
    if (!IsActiveProxy()) return;

    // Let a server disconnect this client safely.
    //ManagerServerProxy->Shutdown();
    //ManagerServerProxy->ice_getConnection()->close(false); // close gracefully

    try {
        BaseClientType::StopProxy();
        Server->Stop();
    } catch (const Ice::Exception& e) {
        std::string error("mtsManagerProxyClient: ");
        error += e.what();
        LogError(mtsManagerProxyClient, error);
    }

    IceGUID = "";

    LogPrint(mtsManagerProxyClient, "Stopped manager proxy client");
}

bool mtsManagerProxyClient::OnServerDisconnect(const Ice::Exception & ex)
{
    // Ice - ConnectionLostException - forceful closure by peer
    // Ice - ForcedCloseConnectionException - after forceful closure by peer
    CMN_LOG_CLASS_RUN_ERROR << "LCM - Proxy \"" << ProxyName << "\" detected GLOBAL COMPONENT MANAGER DISCONNECTION "
                            << "(" << EndpointInfo << ")" << std::endl;
    
    StopProxy();

    return true;
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

bool mtsManagerProxyClient::AddInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName,
                                                         const std::string & interfaceName, const bool isProxyInterface)
{
    return SendAddInterfaceProvided(processName, componentName, interfaceName, isProxyInterface);
}

bool mtsManagerProxyClient::AddInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName,
                                                        const std::string & interfaceName, const bool isProxyInterface)
{
    return SendAddInterfaceRequired(processName, componentName, interfaceName, isProxyInterface);
}

bool mtsManagerProxyClient::FindInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName,
                                                          const std::string & interfaceName) const
{
    return const_cast<mtsManagerProxyClient*>(this)->SendFindInterfaceProvided(processName, componentName, interfaceName);
}

bool mtsManagerProxyClient::FindInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName,
                                                         const std::string & interfaceName) const
{
    return const_cast<mtsManagerProxyClient*>(this)->SendFindInterfaceRequired(processName, componentName, interfaceName);
}

bool mtsManagerProxyClient::RemoveInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName,
                                                            const std::string & interfaceName)
{
    return SendRemoveInterfaceProvided(processName, componentName, interfaceName);
}

bool mtsManagerProxyClient::RemoveInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName,
                                                           const std::string & interfaceName)
{
    return SendRemoveInterfaceRequired(processName, componentName, interfaceName);
}

int mtsManagerProxyClient::Connect(const std::string & requestProcessName,
                                   const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
                                   const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName)
{
    mtsManagerProxy::ConnectionStringSet connectionStringSet;
    GetConnectionStringSet(connectionStringSet,
                           clientProcessName, clientComponentName, clientInterfaceRequiredName,
                           serverProcessName, serverComponentName, serverInterfaceProvidedName,
                           requestProcessName);

    return SendConnect(connectionStringSet);
}

bool mtsManagerProxyClient::ConnectConfirm(unsigned int connectionSessionID)
{
    return SendConnectConfirm(connectionSessionID);
}

bool mtsManagerProxyClient::Disconnect(const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
                                       const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName)
{
    mtsManagerProxy::ConnectionStringSet connectionStringSet;
    GetConnectionStringSet(connectionStringSet,
                           clientProcessName, clientComponentName, clientInterfaceRequiredName,
                           serverProcessName, serverComponentName, serverInterfaceProvidedName);

    return SendDisconnect(connectionStringSet);
}

bool mtsManagerProxyClient::SetInterfaceProvidedProxyAccessInfo(
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName,
    const std::string & endpointInfo)
{
    mtsManagerProxy::ConnectionStringSet connectionStringSet;
    GetConnectionStringSet(connectionStringSet,
                           clientProcessName, clientComponentName, clientInterfaceRequiredName,
                           serverProcessName, serverComponentName, serverInterfaceProvidedName);

    return SendSetInterfaceProvidedProxyAccessInfo(connectionStringSet, endpointInfo);
}

bool mtsManagerProxyClient::GetInterfaceProvidedProxyAccessInfo(
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName,
    std::string & endpointInfo)
{
    mtsManagerProxy::ConnectionStringSet connectionStringSet;
    GetConnectionStringSet(connectionStringSet,
                           clientProcessName, clientComponentName, clientInterfaceRequiredName,
                           serverProcessName, serverComponentName, serverInterfaceProvidedName);

    return SendGetInterfaceProvidedProxyAccessInfo(connectionStringSet, endpointInfo);
}

bool mtsManagerProxyClient::InitiateConnect(const unsigned int connectionID,
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName)
{
    mtsManagerProxy::ConnectionStringSet connectionStringSet;
    GetConnectionStringSet(connectionStringSet,
                           clientProcessName, clientComponentName, clientInterfaceRequiredName,
                           serverProcessName, serverComponentName, serverInterfaceProvidedName);

    return SendInitiateConnect(connectionID, connectionStringSet);
}

bool mtsManagerProxyClient::ConnectServerSideInterfaceRequest(const unsigned int connectionID,
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName)
{
    mtsManagerProxy::ConnectionStringSet connectionStringSet;
    GetConnectionStringSet(connectionStringSet,
                           clientProcessName, clientComponentName, clientInterfaceRequiredName,
                           serverProcessName, serverComponentName, serverInterfaceProvidedName);

    return SendConnectServerSideInterfaceRequest(connectionID, connectionStringSet);
}

void mtsManagerProxyClient::GetListOfConnections(std::vector<mtsDescriptionConnection> & list) const
{
    // TODO: implement this if needed (MJUNG)
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

bool mtsManagerProxyClient::ReceiveCreateInterfaceProvidedProxy(const std::string & serverComponentProxyName, const ::mtsManagerProxy::InterfaceProvidedDescription & providedInterfaceDescription)
{
    // Convert providedInterfaceDescription into an object of type mtsInterfaceCommon::InterfaceProvidedDescription
    InterfaceProvidedDescription convertedDescription;
    mtsManagerProxyServer::ConvertInterfaceProvidedDescription(providedInterfaceDescription, convertedDescription);

    return ProxyOwner->CreateInterfaceProvidedProxy(serverComponentProxyName, convertedDescription);
}

bool mtsManagerProxyClient::ReceiveCreateInterfaceRequiredProxy(const std::string & clientComponentProxyName, const ::mtsManagerProxy::InterfaceRequiredDescription & requiredInterfaceDescription)
{
    // Convert requiredInterfaceDescription into an object of type mtsInterfaceCommon::InterfaceRequiredDescription
    InterfaceRequiredDescription convertedDescription;
    mtsManagerProxyServer::ConvertInterfaceRequiredDescription(requiredInterfaceDescription, convertedDescription);

    return ProxyOwner->CreateInterfaceRequiredProxy(clientComponentProxyName, convertedDescription);
}

bool mtsManagerProxyClient::ReceiveRemoveInterfaceProvidedProxy(const std::string & clientComponentProxyName, const std::string & providedInterfaceProxyName)
{
    return ProxyOwner->RemoveInterfaceProvidedProxy(clientComponentProxyName, providedInterfaceProxyName);
}

bool mtsManagerProxyClient::ReceiveRemoveInterfaceRequiredProxy(const std::string & serverComponentProxyName, const std::string & requiredInterfaceProxyName)
{
    return ProxyOwner->RemoveInterfaceRequiredProxy(serverComponentProxyName, requiredInterfaceProxyName);
}

bool mtsManagerProxyClient::ReceiveConnectServerSideInterface(const unsigned int connectionID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet)
{
    return ProxyOwner->ConnectServerSideInterface(connectionID,
        connectionStringSet.ClientProcessName, connectionStringSet.ClientComponentName, connectionStringSet.ClientInterfaceRequiredName,
        connectionStringSet.ServerProcessName, connectionStringSet.ServerComponentName, connectionStringSet.ServerInterfaceProvidedName);
}

bool mtsManagerProxyClient::ReceiveConnectClientSideInterface(const unsigned int connectionID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet)
{
    return ProxyOwner->ConnectClientSideInterface(connectionID,
        connectionStringSet.ClientProcessName, connectionStringSet.ClientComponentName, connectionStringSet.ClientInterfaceRequiredName,
        connectionStringSet.ServerProcessName, connectionStringSet.ServerComponentName, connectionStringSet.ServerInterfaceProvidedName);
}

bool mtsManagerProxyClient::ReceiveGetInterfaceProvidedDescription(const std::string & serverComponentName, const std::string & providedInterfaceName, ::mtsManagerProxy::InterfaceProvidedDescription & providedInterfaceDescription)
{
    InterfaceProvidedDescription src;

    if (!ProxyOwner->GetInterfaceProvidedDescription(serverComponentName, providedInterfaceName, src)) {
        LogError(mtsManagerProxyClient, "ReceiveGetInterfaceProvidedDescription() failed");
        return false;
    }

    // Convert mtsInterfaceCommon::InterfaceProvidedDescription to mtsManagerProxy::InterfaceProvidedDescription
    mtsManagerProxyServer::ConstructInterfaceProvidedDescriptionFrom(src, providedInterfaceDescription);

    return true;
}

bool mtsManagerProxyClient::ReceiveGetInterfaceRequiredDescription(const std::string & componentName, const std::string & requiredInterfaceName, ::mtsManagerProxy::InterfaceRequiredDescription & requiredInterfaceDescription)
{
    InterfaceRequiredDescription src;

    if (!ProxyOwner->GetInterfaceRequiredDescription(componentName, requiredInterfaceName, src)) {
        LogError(mtsManagerProxyClient, "ReceiveGetInterfaceRequiredDescription() failed");
        return false;
    }

    // Construct an instance of type InterfaceRequiredDescription from an object of type mtsInterfaceCommon::InterfaceRequiredDescription
    mtsManagerProxyServer::ConstructInterfaceRequiredDescriptionFrom(src, requiredInterfaceDescription);

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

void mtsManagerProxyClient::ReceiveGetArgumentInformation(const std::string & componentName, const std::string & providedInterfaceName, const std::string & commandName, std::string & argumentName, ::mtsManagerProxy::NamesOfSignals & signalNames) const
{
    ProxyOwner->GetArgumentInformation(argumentName, signalNames, componentName, providedInterfaceName, commandName);
}

void mtsManagerProxyClient::ReceiveGetValuesOfCommand(const std::string & componentName, const std::string & providedInterfaceName, const std::string & commandName, const int scalarIndex, ::mtsManagerProxy::SetOfValues & values) const
{
    mtsManagerLocalInterface::SetOfValues valuesCISSTtype;

    ProxyOwner->GetValuesOfCommand(valuesCISSTtype, componentName, providedInterfaceName, commandName, scalarIndex);

    mtsManagerProxyServer::ConstructValuesOfCommand(valuesCISSTtype, values);
}

std::string mtsManagerProxyClient::ReceiveGetProcessName()
{
    return ProxyOwner->GetProcessName();
}

::Ice::Int mtsManagerProxyClient::ReceiveGetCurrentInterfaceCount(const std::string & componentName)
{
    return ProxyOwner->GetTotalNumberOfInterfaces(componentName);
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
        OnServerDisconnect(ex);
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
        OnServerDisconnect(ex);
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
        OnServerDisconnect(ex);
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
        OnServerDisconnect(ex);
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
        OnServerDisconnect(ex);
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
        OnServerDisconnect(ex);
        return false;
    }
}

bool mtsManagerProxyClient::SendAddInterfaceProvided(const std::string & processName, const std::string & componentName, const std::string & interfaceName, bool isProxyInterface)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendAddInterfaceProvided: " << processName << ", " << componentName << ", " << interfaceName << ", " << isProxyInterface);
#endif

    try {
        return ManagerServerProxy->AddInterfaceProvided(processName, componentName, interfaceName, isProxyInterface);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendAddInterfaceProvided: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }
}

bool mtsManagerProxyClient::SendFindInterfaceProvided(const std::string & processName, const std::string & componentName, const std::string & interfaceName)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendFindInterfaceProvided: " << processName << ", " << componentName << ", " << interfaceName);
#endif

    try {
        return ManagerServerProxy->FindInterfaceProvided(processName, componentName, interfaceName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendFindInterfaceProvided: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }
}

bool mtsManagerProxyClient::SendRemoveInterfaceProvided(const std::string & processName, const std::string & componentName, const std::string & interfaceName)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendRemoveInterfaceProvided: " << processName << ", " << componentName << ", " << interfaceName);
#endif

    try {
        return ManagerServerProxy->RemoveInterfaceProvided(processName, componentName, interfaceName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendRemoveInterfaceProvided: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }
}

bool mtsManagerProxyClient::SendAddInterfaceRequired(const std::string & processName, const std::string & componentName, const std::string & interfaceName, bool isProxyInterface)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendAddInterfaceRequired: " << processName << ", " << componentName << ", " << interfaceName << ", " << isProxyInterface);
#endif

    try {
        return ManagerServerProxy->AddInterfaceRequired(processName, componentName, interfaceName, isProxyInterface);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendAddInterfaceRequired: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }
}

bool mtsManagerProxyClient::SendFindInterfaceRequired(const std::string & processName, const std::string & componentName, const std::string & interfaceName)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendFindInterfaceRequired: " << processName << ", " << componentName << ", " << interfaceName);
#endif

    try {
        return ManagerServerProxy->FindInterfaceRequired(processName, componentName, interfaceName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendFindInterfaceRequired: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }
}

bool mtsManagerProxyClient::SendRemoveInterfaceRequired(const std::string & processName, const std::string & componentName, const std::string & interfaceName)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendRemoveInterfaceRequired: " << processName << ", " << componentName << ", " << interfaceName);
#endif

    try {
        return ManagerServerProxy->RemoveInterfaceRequired(processName, componentName, interfaceName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendRemoveInterfaceRequired: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }
}

::Ice::Int mtsManagerProxyClient::SendConnect(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendConnect: " << connectionStringSet.ClientProcessName << ", " << connectionStringSet.ServerProcessName);
#endif

    try {
        return ManagerServerProxy->Connect(connectionStringSet);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendConnect: network exception: " << ex);
        OnServerDisconnect(ex);
        return -1; // See definition of this error code at mtsManagerGlobalInterface::Connect()
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
        OnServerDisconnect(ex);
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
        OnServerDisconnect(ex);
        return false;
    }
}

bool mtsManagerProxyClient::SendSetInterfaceProvidedProxyAccessInfo(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, const std::string & endpointInfo)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendSetInterfaceProvidedProxyAccessInfo: " << connectionStringSet.ClientProcessName << ", " << connectionStringSet.ServerProcessName << ", " << endpointInfo);
#endif

    try {
        return ManagerServerProxy->SetInterfaceProvidedProxyAccessInfo(connectionStringSet, endpointInfo);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendSetInterfaceProvidedProxyAccessInfo: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }
}

bool mtsManagerProxyClient::SendGetInterfaceProvidedProxyAccessInfo(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, std::string & endpointInfo)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendGetInterfaceProvidedProxyAccessInfo: " << connectionStringSet.ClientProcessName << ", " << connectionStringSet.ServerProcessName);
#endif

    try {
        return ManagerServerProxy->GetInterfaceProvidedProxyAccessInfo(connectionStringSet, endpointInfo);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendGetInterfaceProvidedProxyAccessInfo: network exception: " << ex);
        OnServerDisconnect(ex);
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
        OnServerDisconnect(ex);
        return false;
    }
}

bool mtsManagerProxyClient::SendConnectServerSideInterfaceRequest(const unsigned int connectionID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendConnectServerSideInterfaceRequest: " << connectionID << ", " << connectionStringSet.ClientProcessName << ", " << connectionStringSet.ServerProcessName);
#endif

    try {
        return ManagerServerProxy->ConnectServerSideInterfaceRequest(connectionID, connectionStringSet);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendConnectServerSideInterfaceRequest: network exception: " << ex);
        OnServerDisconnect(ex);
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
    ManagerProxyClient->GetLogger()->trace("mtsManagerProxyClient", "Server communication callback thread starts");

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
    while (IsActiveProxy()) {
        osaSleep(mtsProxyConfig::RefreshPeriodForManagers);
        try {
            Server->Refresh();
        } catch (const ::Ice::Exception & ex) {
            LogPrint(mtsManagerProxyClient, "refresh failed (" << Server->ice_toString() << ")" << std::endl << ex);
            if (ManagerProxyClient) {
                ManagerProxyClient->OnServerDisconnect(ex);
            }
        }
    }
#endif

    LogPrint(mtsManagerProxyClient, "mtsManagerProxyClient::ManagerClientI - terminated");
}

void mtsManagerProxyClient::ManagerClientI::Stop()
{
    if (!IsActiveProxy()) return;

    IceUtil::ThreadPtr callbackSenderThread;
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);

        notify();

        callbackSenderThread = SenderThreadPtr;
        SenderThreadPtr = 0;
    }
    callbackSenderThread->getThreadControl().join();

    ManagerProxyClient = 0;
    LogPrint(ManagerClientI, "Stopped and destroyed callback thread to communicate with server");
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

bool mtsManagerProxyClient::ManagerClientI::CreateInterfaceProvidedProxy(const std::string & serverComponentProxyName, const ::mtsManagerProxy::InterfaceProvidedDescription & providedInterfaceDescription, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: CreateInterfaceProvidedProxy: " << serverComponentProxyName << ", " << providedInterfaceDescription.InterfaceProvidedName);
#endif

    return ManagerProxyClient->ReceiveCreateInterfaceProvidedProxy(serverComponentProxyName, providedInterfaceDescription);
}

bool mtsManagerProxyClient::ManagerClientI::CreateInterfaceRequiredProxy(const std::string & clientComponentProxyName, const ::mtsManagerProxy::InterfaceRequiredDescription & requiredInterfaceDescription, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: CreateInterfaceRequiredProxy: " << clientComponentProxyName << ", " << requiredInterfaceDescription.InterfaceRequiredName);
#endif

    return ManagerProxyClient->ReceiveCreateInterfaceRequiredProxy(clientComponentProxyName, requiredInterfaceDescription);
}

bool mtsManagerProxyClient::ManagerClientI::RemoveInterfaceProvidedProxy(const std::string & clientComponentProxyName, const std::string & providedInterfaceProxyName, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: RemoveInterfaceProvidedProxy: " << clientComponentProxyName << ", " << providedInterfaceProxyName);
#endif

    return ManagerProxyClient->ReceiveRemoveInterfaceProvidedProxy(clientComponentProxyName, providedInterfaceProxyName);
}

bool mtsManagerProxyClient::ManagerClientI::RemoveInterfaceRequiredProxy(const std::string & serverComponentProxyName, const std::string & requiredInterfaceProxyName, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: RemoveInterfaceRequiredProxy: " << serverComponentProxyName << ", " << requiredInterfaceProxyName);
#endif

    return ManagerProxyClient->ReceiveRemoveInterfaceRequiredProxy(serverComponentProxyName, requiredInterfaceProxyName);
}

bool mtsManagerProxyClient::ManagerClientI::ConnectServerSideInterface(::Ice::Int connectionID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: ConnectServerSideInterface: " << connectionID);
#endif

    return ManagerProxyClient->ReceiveConnectServerSideInterface(connectionID, connectionStringSet);
}

bool mtsManagerProxyClient::ManagerClientI::ConnectClientSideInterface(::Ice::Int connectionID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: ConnectClientSideInterface: " << connectionID);
#endif

    return ManagerProxyClient->ReceiveConnectClientSideInterface(connectionID, connectionStringSet);
}

bool mtsManagerProxyClient::ManagerClientI::GetInterfaceProvidedDescription(const std::string & serverComponentName, const std::string & providedInterfaceName, ::mtsManagerProxy::InterfaceProvidedDescription & providedInterfaceDescription, const ::Ice::Current & CMN_UNUSED(current)) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: GetInterfaceProvidedDescription: " << serverComponentName << ":" << providedInterfaceName);
#endif

    return ManagerProxyClient->ReceiveGetInterfaceProvidedDescription(serverComponentName, providedInterfaceName, providedInterfaceDescription);
}

bool mtsManagerProxyClient::ManagerClientI::GetInterfaceRequiredDescription(const std::string & componentName, const std::string & requiredInterfaceName, ::mtsManagerProxy::InterfaceRequiredDescription & requiredInterfaceDescription, const ::Ice::Current & CMN_UNUSED(current)) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: GetInterfaceRequiredDescription" << componentName << ", " << requiredInterfaceName << requiredInterfaceDescription.InterfaceRequiredName);
#endif

    return ManagerProxyClient->ReceiveGetInterfaceRequiredDescription(componentName, requiredInterfaceName, requiredInterfaceDescription);
}

std::string mtsManagerProxyClient::ManagerClientI::GetProcessName(const ::Ice::Current & CMN_UNUSED(current)) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
	LogPrint(ManagerClientI, "<<<<< RECV: GetProcessName ");
#endif

    return ManagerProxyClient->ReceiveGetProcessName();
}

::Ice::Int mtsManagerProxyClient::ManagerClientI::GetTotalNumberOfInterfaces(const ::std::string & componentName, const ::Ice::Current & CMN_UNUSED(current)) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: GetTotalNumberOfInterfaces: " << componentName);
#endif

    return ManagerProxyClient->ReceiveGetCurrentInterfaceCount(componentName);
}


void mtsManagerProxyClient::ManagerClientI::GetNamesOfCommands(const std::string & componentName, const std::string & providedInterfaceName, ::mtsManagerProxy::NamesOfCommandsSequence & names, const ::Ice::Current & CMN_UNUSED(current)) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: GetNamesOfCommands: " << componentName << ", " << providedInterfaceName);
#endif

    ManagerProxyClient->ReceiveGetNamesOfCommands(componentName, providedInterfaceName, names);
}

void mtsManagerProxyClient::ManagerClientI::GetNamesOfEventGenerators(const std::string & componentName, const std::string & providedInterfaceName, ::mtsManagerProxy::NamesOfEventGeneratorsSequence & names, const ::Ice::Current & CMN_UNUSED(current)) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: GetNamesOfEventGenerators: " << componentName << ", " << providedInterfaceName);
#endif

    ManagerProxyClient->ReceiveGetNamesOfEventGenerators(componentName, providedInterfaceName, names);
}

void mtsManagerProxyClient::ManagerClientI::GetNamesOfFunctions(const std::string & componentName, const std::string & requiredInterfaceName, ::mtsManagerProxy::NamesOfFunctionsSequence & names, const ::Ice::Current & CMN_UNUSED(current)) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: GetNamesOfFunctions: " << componentName << ", " << requiredInterfaceName);
#endif

    ManagerProxyClient->ReceiveGetNamesOfFunctions(componentName, requiredInterfaceName, names);
}

void mtsManagerProxyClient::ManagerClientI::GetNamesOfEventHandlers(const std::string & componentName, const std::string & requiredInterfaceName, ::mtsManagerProxy::NamesOfEventHandlersSequence & names, const ::Ice::Current & CMN_UNUSED(current)) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: GetNamesOfEventHandlers: " << componentName << ", " << requiredInterfaceName);
#endif

    ManagerProxyClient->ReceiveGetNamesOfEventHandlers(componentName, requiredInterfaceName, names);
}

void mtsManagerProxyClient::ManagerClientI::GetDescriptionOfCommand(const std::string & componentName, const std::string & providedInterfaceName, const std::string & commandName, std::string & description, const ::Ice::Current & CMN_UNUSED(current)) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: GetDescriptionOfCommand: " << componentName << ", " << providedInterfaceName << ", " << commandName);
#endif

    ManagerProxyClient->ReceiveGetDescriptionOfCommand(componentName, providedInterfaceName, commandName, description);
}

void mtsManagerProxyClient::ManagerClientI::GetDescriptionOfEventGenerator(const std::string & componentName, const std::string & providedInterfaceName, const std::string & eventGeneratorName, std::string & description, const ::Ice::Current & CMN_UNUSED(current)) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: GetDescriptionOfEventGenerator: " << componentName << ", " << providedInterfaceName << ", " << eventGeneratorName);
#endif

    ManagerProxyClient->ReceiveGetDescriptionOfEventGenerator(componentName, providedInterfaceName, eventGeneratorName, description);
}

void mtsManagerProxyClient::ManagerClientI::GetDescriptionOfFunction(const std::string & componentName, const std::string & requiredInterfaceName, const std::string & functionName, std::string & description, const ::Ice::Current & CMN_UNUSED(current)) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: GetDescriptionOfFunction: " << componentName << ", " << requiredInterfaceName << ", " << functionName);
#endif

    ManagerProxyClient->ReceiveGetDescriptionOfFunction(componentName, requiredInterfaceName, functionName, description);
}

void mtsManagerProxyClient::ManagerClientI::GetDescriptionOfEventHandler(const std::string & componentName, const std::string & requiredInterfaceName, const std::string & eventHandlerName, std::string & description, const ::Ice::Current & CMN_UNUSED(current)) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: GetDescriptionOfEventHandler: " << componentName << ", " << requiredInterfaceName << ", " << eventHandlerName);
#endif

    ManagerProxyClient->ReceiveGetDescriptionOfEventHandler(componentName, requiredInterfaceName, eventHandlerName, description);
}

void mtsManagerProxyClient::ManagerClientI::GetArgumentInformation(const std::string & componentName, const std::string & providedInterfaceName,
        const std::string & commandName, std::string & argumentName, ::mtsManagerProxy::NamesOfSignals & signalNames, const ::Ice::Current & CMN_UNUSED(current)) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: GetArgumentInformation: " << componentName << ", " << providedInterfaceName << ", " << commandName);
#endif

    ManagerProxyClient->ReceiveGetArgumentInformation(componentName, providedInterfaceName, commandName, argumentName, signalNames);
}

void mtsManagerProxyClient::ManagerClientI::GetValuesOfCommand(const std::string & componentName, const std::string & providedInterfaceName, 
    const std::string & commandName, ::Ice::Int scalarIndex, ::mtsManagerProxy::SetOfValues & values, const ::Ice::Current & CMN_UNUSED(current)) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: GetValuesOfCommand: " << componentName << ", " << providedInterfaceName << ", " << commandName << ", " << scalarIndex);
#endif

    ManagerProxyClient->ReceiveGetValuesOfCommand(componentName, providedInterfaceName, commandName, scalarIndex, values);
}
