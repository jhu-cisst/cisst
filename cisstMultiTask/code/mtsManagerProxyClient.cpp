/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2010-01-20

  (C) Copyright 2010-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaSleep.h>
#include <cisstOSAbstraction/osaCriticalSection.h>

#include "mtsProxyConfig.h"
#if IMPROVE_ICE_THREADING
#include <cisstOSAbstraction/osaThreadSignal.h>
#endif

#include "mtsManagerProxyClient.h"
#include "mtsManagerProxyServer.h"
#include <cisstMultiTask/mtsFunctionVoid.h>

unsigned int mtsManagerProxyClient::InstanceCounter = 0;

void GetConnectionStringSet(mtsManagerProxy::ConnectionStringSet & connectionStringSet,
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceName,
    const std::string & requestProcessName = "")
{
    connectionStringSet.ClientProcessName = clientProcessName;
    connectionStringSet.ClientComponentName = clientComponentName;
    connectionStringSet.ClientInterfaceName = clientInterfaceName;
    connectionStringSet.ServerProcessName = serverProcessName;
    connectionStringSet.ServerComponentName = serverComponentName;
    connectionStringSet.ServerInterfaceName = serverInterfaceName;
    connectionStringSet.RequestProcessName = requestProcessName;
}

mtsManagerProxyClient::mtsManagerProxyClient(const std::string & serverEndpointInfo)
    : BaseClientType("config.LCM", serverEndpointInfo), ManagerServerProxy(0)
#if IMPROVE_ICE_THREADING
      , IceThreadInitEvent(0)
#endif
{
    ProxyName = "ManagerProxyClient";

#if IMPROVE_ICE_THREADING
    IceThreadInitEvent = new osaThreadSignal;
#endif
}

mtsManagerProxyClient::~mtsManagerProxyClient()
{
    StopProxy();

#if IMPROVE_ICE_THREADING
    delete IceThreadInitEvent;
#endif
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

#if IMPROVE_ICE_THREADING
    // Wait for Ice thread to start
    IceThreadInitEvent->Wait();
#endif

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

    ChangeProxyState(PROXY_STATE_ACTIVE);
#if IMPROVE_ICE_THREADING
    IceThreadInitEvent->Raise();
#endif

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

    try {
        Server->Stop();
        BaseClientType::StopProxy();
    } catch (const Ice::Exception& e) {
        std::string error("mtsManagerProxyClient: ");
        error += e.what();
        LogError(mtsManagerProxyClient, error);
    }

    IceGUID = "";
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, "Stopped manager proxy client");
#endif
}

void mtsManagerProxyClient::OnServerDisconnect(const Ice::Exception & ex)
{
    // Multiple proxy threads can detect server disconnect
    static osaCriticalSection cs;

    cs.Enter();

    if (!IsActiveProxy() || !ProxyOwner->IsGCMActive()) {
        cs.Leave();
        return; // already detected disconnection
    }

    // Ice - ConnectionLostException - forceful closure by peer
    // Ice - ForcedCloseConnectionException - after forceful closure by peer
    CMN_LOG_CLASS_RUN_WARNING << ex << std::endl;
    CMN_LOG_CLASS_RUN_ERROR << "Process \"" << ProxyOwner->GetProcessName() 
        << "\" detected GLOBAL COMPONENT MANAGER DISCONNECTION" << std::endl;

    StopProxy();

    // GCM has been disconnected
    ProxyOwner->SetGCMConnected(false);

    cs.Leave();
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

bool mtsManagerProxyClient::RemoveProcess(const std::string & processName, const bool CMN_UNUSED(networkDisconnect))
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

bool mtsManagerProxyClient::RemoveComponent(const std::string & processName, const std::string & componentName, const bool CMN_UNUSED(lock))
{
    return SendRemoveComponent(processName, componentName);
}

bool mtsManagerProxyClient::AddInterfaceProvidedOrOutput(const std::string & processName,
                                                         const std::string & componentName, const std::string & interfaceName)
{
    return SendAddInterfaceProvidedOrOutput(processName, componentName, interfaceName);
}

bool mtsManagerProxyClient::AddInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName,
                                                        const std::string & interfaceName)
{
    return SendAddInterfaceRequiredOrInput(processName, componentName, interfaceName);
}

bool mtsManagerProxyClient::FindInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName,
                                                          const std::string & interfaceName) const
{
    return const_cast<mtsManagerProxyClient*>(this)->SendFindInterfaceProvidedOrOutput(processName, componentName, interfaceName);
}

bool mtsManagerProxyClient::FindInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName,
                                                         const std::string & interfaceName) const
{
    return const_cast<mtsManagerProxyClient*>(this)->SendFindInterfaceRequiredOrInput(processName, componentName, interfaceName);
}

bool mtsManagerProxyClient::RemoveInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName,
                                                            const std::string & interfaceName, const bool CMN_UNUSED(lock))
{
    return SendRemoveInterfaceProvidedOrOutput(processName, componentName, interfaceName);
}

bool mtsManagerProxyClient::RemoveInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName,
                                                           const std::string & interfaceName, const bool CMN_UNUSED(lock))
{
    return SendRemoveInterfaceRequiredOrInput(processName, componentName, interfaceName);
}

ConnectionIDType mtsManagerProxyClient::Connect(const std::string & requestProcessName,
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceName)
{
    mtsManagerProxy::ConnectionStringSet connectionStringSet;
    GetConnectionStringSet(connectionStringSet,
                           clientProcessName, clientComponentName, clientInterfaceName,
                           serverProcessName, serverComponentName, serverInterfaceName,
                           requestProcessName);

    return (ConnectionIDType) SendConnect(connectionStringSet);
}

bool mtsManagerProxyClient::ConnectConfirm(const ConnectionIDType connectionID)
{
    return SendConnectConfirm(connectionID);
}

bool mtsManagerProxyClient::Disconnect(const ConnectionIDType connectionID)
{
    return SendDisconnect(connectionID);
}

bool mtsManagerProxyClient::Disconnect(const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceName,
                                       const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceName)
{
    mtsManagerProxy::ConnectionStringSet connectionStringSet;
    GetConnectionStringSet(connectionStringSet,
                           clientProcessName, clientComponentName, clientInterfaceName,
                           serverProcessName, serverComponentName, serverInterfaceName);

    return SendDisconnect(connectionStringSet);
}

bool mtsManagerProxyClient::SetInterfaceProvidedProxyAccessInfo(const ConnectionIDType connectionID, const std::string & endpointInfo)
{
    return SendSetInterfaceProvidedProxyAccessInfo(connectionID, endpointInfo);
}

bool mtsManagerProxyClient::GetInterfaceProvidedProxyAccessInfo(const ConnectionIDType connectionID, std::string & endpointInfo)
{
    return SendGetInterfaceProvidedProxyAccessInfo(connectionID, endpointInfo);
}

bool mtsManagerProxyClient::GetInterfaceProvidedProxyAccessInfo(const std::string & clientProcessName,
    const std::string & serverProcessName, const std::string & serverComponentName,
    const std::string & serverInterfaceName, std::string & endpointInfo)
{
    return SendGetInterfaceProvidedProxyAccessInfo(clientProcessName, serverProcessName, serverComponentName, serverInterfaceName, endpointInfo);
}

bool mtsManagerProxyClient::InitiateConnect(const ConnectionIDType connectionID)
{
    return SendInitiateConnect(connectionID);
}

bool mtsManagerProxyClient::ConnectServerSideInterfaceRequest(const ConnectionIDType connectionID)
{
    return SendConnectServerSideInterfaceRequest(connectionID);
}

void mtsManagerProxyClient::GetListOfConnections(std::vector<mtsDescriptionConnection> & CMN_UNUSED(list)) const
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
    mtsInterfaceProvidedDescription convertedDescription;
    mtsManagerProxyServer::ConvertInterfaceProvidedDescription(providedInterfaceDescription, convertedDescription);

    return ProxyOwner->CreateInterfaceProvidedProxy(serverComponentProxyName, convertedDescription);
}

bool mtsManagerProxyClient::ReceiveCreateInterfaceRequiredProxy(const std::string & clientComponentProxyName, const ::mtsManagerProxy::InterfaceRequiredDescription & requiredInterfaceDescription)
{
    // Convert requiredInterfaceDescription into an object of type mtsInterfaceCommon::InterfaceRequiredDescription
    mtsInterfaceRequiredDescription convertedDescription;
    mtsManagerProxyServer::ConvertInterfaceRequiredDescription(requiredInterfaceDescription, convertedDescription);

    return ProxyOwner->CreateInterfaceRequiredProxy(clientComponentProxyName, convertedDescription);
}

bool mtsManagerProxyClient::ReceiveRemoveInterfaceProvidedProxy(const std::string & componentProxyName, const std::string & providedInterfaceProxyName)
{
    return ProxyOwner->RemoveInterfaceProvidedProxy(componentProxyName, providedInterfaceProxyName);
}

bool mtsManagerProxyClient::ReceiveRemoveInterfaceRequiredProxy(const std::string & componentProxyName, const std::string & requiredInterfaceProxyName)
{
    return ProxyOwner->RemoveInterfaceRequiredProxy(componentProxyName, requiredInterfaceProxyName);
}

bool mtsManagerProxyClient::ReceiveConnectServerSideInterface(const ConnectionIDType connectionID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet)
{
    mtsDescriptionConnection connection;

    connection.ConnectionID = connectionID;
    connection.Server.ProcessName   = connectionStringSet.ServerProcessName;
    connection.Server.ComponentName = connectionStringSet.ServerComponentName;
    connection.Server.InterfaceName = connectionStringSet.ServerInterfaceName;
    connection.Client.ProcessName   = connectionStringSet.ClientProcessName;
    connection.Client.ComponentName = connectionStringSet.ClientComponentName;
    connection.Client.InterfaceName = connectionStringSet.ClientInterfaceName;

    return ProxyOwner->ConnectServerSideInterface(connection);
}

bool mtsManagerProxyClient::ReceiveConnectClientSideInterface(const ConnectionIDType connectionID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet)
{
    mtsDescriptionConnection connection;

    connection.ConnectionID = connectionID;
    connection.Server.ProcessName   = connectionStringSet.ServerProcessName;
    connection.Server.ComponentName = connectionStringSet.ServerComponentName;
    connection.Server.InterfaceName = connectionStringSet.ServerInterfaceName;
    connection.Client.ProcessName   = connectionStringSet.ClientProcessName;
    connection.Client.ComponentName = connectionStringSet.ClientComponentName;
    connection.Client.InterfaceName = connectionStringSet.ClientInterfaceName;

    return ProxyOwner->ConnectClientSideInterface(connection);
}

bool mtsManagerProxyClient::ReceiveGetInterfaceProvidedDescription(const std::string & serverComponentName, const std::string & providedInterfaceName, ::mtsManagerProxy::InterfaceProvidedDescription & providedInterfaceDescription)
{
    mtsInterfaceProvidedDescription src;

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
    mtsInterfaceRequiredDescription src;

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

std::string mtsManagerProxyClient::ReceiveGetProcessName()
{
    return ProxyOwner->GetProcessName();
}

//-------------------------------------------------------------------------
//  Event Generators (Event Sender) : Client -> Server
//-------------------------------------------------------------------------
void mtsManagerProxyClient::SendTestMessageFromClientToServer(const std::string & str) const
{
    if (!IsActiveProxy()) return;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: MessageFromClientToServer");
#endif

    ManagerServerProxy->TestMessageFromClientToServer(str);
}

bool mtsManagerProxyClient::SendAddProcess(const std::string & processName)
{
    if (!IsActiveProxy()) {
        return false;
    }

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
    if (!IsActiveProxy()) return false;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendFindProcess: " << processName);
#endif

    try {
        if (!ManagerServerProxy.get()) {
            return false;
        }
        return ManagerServerProxy->FindProcess(processName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendFindProcess: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }
}

bool mtsManagerProxyClient::SendRemoveProcess(const std::string & processName)
{
    if (!IsActiveProxy()) return false;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendRemoveProcess: " << processName);
#endif

    try {
        if (!ManagerServerProxy.get()) {
            return false;
        }
        return ManagerServerProxy->RemoveProcess(processName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendRemoveProcess: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }
}

bool mtsManagerProxyClient::SendAddComponent(const std::string & processName, const std::string & componentName)
{
    if (!IsActiveProxy()) return false;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendAddComponent: " << processName << ", " << componentName);
#endif

    try {
        if (!ManagerServerProxy.get()) {
            return false;
        }
        return ManagerServerProxy->AddComponent(processName, componentName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendAddComponent: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }
}

bool mtsManagerProxyClient::SendFindComponent(const std::string & processName, const std::string & componentName)
{
    if (!IsActiveProxy()) return false;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendFindComponent: " << processName << ", " << componentName);
#endif

    try {
        if (!ManagerServerProxy.get()) {
            return false;
        }
        return ManagerServerProxy->FindComponent(processName, componentName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendFindComponent: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }
}

bool mtsManagerProxyClient::SendRemoveComponent(const std::string & processName, const std::string & componentName)
{
    if (!IsActiveProxy()) return false;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendRemoveComponent: " << processName << ", " << componentName);
#endif

    try {
        if (!ManagerServerProxy.get()) {
            return false;
        }
        return ManagerServerProxy->RemoveComponent(processName, componentName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendRemoveComponent: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }
}

bool mtsManagerProxyClient::SendAddInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName, const std::string & interfaceName)
{
    if (!IsActiveProxy()) return false;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendAddInterfaceProvidedOrOutput: " << processName << ", " << componentName << ", " << interfaceName);
#endif

    try {
        if (!ManagerServerProxy.get()) {
            return false;
        }
        return ManagerServerProxy->AddInterfaceProvidedOrOutput(processName, componentName, interfaceName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendAddInterfaceProvidedOrOutput: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }
}

bool mtsManagerProxyClient::SendFindInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName, const std::string & interfaceName)
{
    if (!IsActiveProxy()) return false;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendFindInterfaceProvidedOrOutput: " << processName << ", " << componentName << ", " << interfaceName);
#endif

    try {
        if (!ManagerServerProxy.get()) {
            return false;
        }
        return ManagerServerProxy->FindInterfaceProvidedOrOutput(processName, componentName, interfaceName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendFindInterfaceProvidedOrOutput: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }
}

bool mtsManagerProxyClient::SendRemoveInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName, const std::string & interfaceName)
{
    if (!IsActiveProxy()) return false;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendRemoveInterfaceProvidedOrOutput: " << processName << ", " << componentName << ", " << interfaceName);
#endif

    try {
        if (!ManagerServerProxy.get()) {
            return false;
        }
        return ManagerServerProxy->RemoveInterfaceProvidedOrOutput(processName, componentName, interfaceName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendRemoveInterfaceProvidedOrOutput: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }
}

bool mtsManagerProxyClient::SendAddInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName, const std::string & interfaceName)
{
    if (!IsActiveProxy()) return false;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendAddInterfaceRequiredOrInput: " << processName << ", " << componentName << ", " << interfaceName);
#endif

    try {
        if (!ManagerServerProxy.get()) {
            return false;
        }
        return ManagerServerProxy->AddInterfaceRequiredOrInput(processName, componentName, interfaceName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendAddInterfaceRequiredOrInput: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }
}

bool mtsManagerProxyClient::SendFindInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName, const std::string & interfaceName)
{
    if (!IsActiveProxy()) return false;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendFindInterfaceRequiredOrInput: " << processName << ", " << componentName << ", " << interfaceName);
#endif

    try {
        if (!ManagerServerProxy.get()) {
            return false;
        }
        return ManagerServerProxy->FindInterfaceRequiredOrInput(processName, componentName, interfaceName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendFindInterfaceRequiredOrInput: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }
}

bool mtsManagerProxyClient::SendRemoveInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName, const std::string & interfaceName)
{
    if (!IsActiveProxy()) return false;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendRemoveInterfaceRequiredOrInput: " << processName << ", " << componentName << ", " << interfaceName);
#endif

    try {
        if (!ManagerServerProxy.get()) {
            return false;
        }
        return ManagerServerProxy->RemoveInterfaceRequiredOrInput(processName, componentName, interfaceName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendRemoveInterfaceRequiredOrInput: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }
}

::Ice::Int mtsManagerProxyClient::SendConnect(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet)
{
    if (!IsActiveProxy()) return (::Ice::Int) InvalidConnectionID;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendConnect: " << connectionStringSet.ClientProcessName << ", " << connectionStringSet.ServerProcessName);
#endif

    try {
        if (!ManagerServerProxy.get()) {
            return false;
        }
        return ManagerServerProxy->Connect(connectionStringSet);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendConnect: network exception: " << ex);
        OnServerDisconnect(ex);
        return (::Ice::Int) InvalidConnectionID;
    }
}

bool mtsManagerProxyClient::SendConnectConfirm(::Ice::Int connectionID)
{
    if (!IsActiveProxy()) return false;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendConnectConfirm: " << connectionID);
#endif

    try {
        if (!ManagerServerProxy.get()) {
            return false;
        }
        return ManagerServerProxy->ConnectConfirm(connectionID);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendConnectConfirm: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }
}

bool mtsManagerProxyClient::SendDisconnect(::Ice::Int connectionID)
{
    if (!IsActiveProxy()) return false;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendDisconnect: " << connectionID);
#endif

    try {
        if (!ManagerServerProxy.get()) {
            return false;
        }
        return ManagerServerProxy->DisconnectWithID(connectionID);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendDisconnect: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }
}

bool mtsManagerProxyClient::SendDisconnect(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet)
{
    if (!IsActiveProxy()) return false;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendDisconnect: " << connectionStringSet.ClientProcessName << ", " << connectionStringSet.ServerProcessName);
#endif

    try {
        if (!ManagerServerProxy.get()) {
            return false;
        }
        return ManagerServerProxy->Disconnect(connectionStringSet);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendDisconnect: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }
}

bool mtsManagerProxyClient::SendSetInterfaceProvidedProxyAccessInfo(const ConnectionIDType connectionID, const std::string & endpointInfo)
{
    if (!IsActiveProxy()) return false;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendSetInterfaceProvidedProxyAccessInfo: " << connectionID << " - " << endpointInfo);
#endif

    try {
        if (!ManagerServerProxy.get()) {
            return false;
        }
        return ManagerServerProxy->SetInterfaceProvidedProxyAccessInfo(connectionID, endpointInfo);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendSetInterfaceProvidedProxyAccessInfo: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }
}

bool mtsManagerProxyClient::SendGetInterfaceProvidedProxyAccessInfo(const ConnectionIDType connectionID, std::string & endpointInfo)
{
    if (!IsActiveProxy()) return false;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendGetInterfaceProvidedProxyAccessInfo: " << connectionID);
#endif

    try {
        if (!ManagerServerProxy.get()) {
            return false;
        }
        return ManagerServerProxy->GetInterfaceProvidedProxyAccessInfoWithID(connectionID, endpointInfo);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendGetInterfaceProvidedProxyAccessInfo: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }
}

bool mtsManagerProxyClient::SendGetInterfaceProvidedProxyAccessInfo(const std::string & clientProcessName,
    const std::string & serverProcessName, const std::string & serverComponentName,
    const std::string & serverInterfaceName, std::string & endpointInfo)
{
    if (!IsActiveProxy()) return false;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendGetInterfaceProvidedProxyAccessInfo: " << clientProcessName << ", "
        << mtsManagerGlobal::GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceName));
#endif

    try {
        if (!ManagerServerProxy.get()) {
            return false;
        }
        return ManagerServerProxy->GetInterfaceProvidedProxyAccessInfo(clientProcessName, serverProcessName, serverComponentName, serverInterfaceName, endpointInfo);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendGetInterfaceProvidedProxyAccessInfo: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }
}

bool mtsManagerProxyClient::SendInitiateConnect(::Ice::Int connectionID)
{
    if (!IsActiveProxy()) return false;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendInitiateConnect: " << connectionID);
#endif

    try {
        if (!ManagerServerProxy.get()) {
            return false;
        }
        return ManagerServerProxy->InitiateConnect(connectionID);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyClient, "SendInitiateConnect: network exception: " << ex);
        OnServerDisconnect(ex);
        return false;
    }
}

bool mtsManagerProxyClient::SendConnectServerSideInterfaceRequest(const ConnectionIDType connectionID)
{
    if (!IsActiveProxy()) return false;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, ">>>>> SEND: SendConnectServerSideInterfaceRequest: " << connectionID);
#endif

    try {
        if (!ManagerServerProxy.get()) {
            return false;
        }
        return ManagerServerProxy->ConnectServerSideInterfaceRequest(connectionID);
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
    double lastTickChecked = 0.0, now;
    while (IsActiveProxy()) {
        now = osaGetTime();
        if (now < lastTickChecked + mtsProxyConfig::RefreshPeriodForManagers) {
            osaSleep(10 * cmn_ms);
            continue;
        }
        lastTickChecked = now;

        try {
            Server->Refresh();
        } catch (const ::Ice::Exception & ex) {
            LogError(mtsManagerProxyClient, "refresh failed (" << Server->ice_toString() << ")" << std::endl << ex);
            if (ManagerProxyClient) {
                ManagerProxyClient->OnServerDisconnect(ex);
            }
        }
    }
#endif

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyClient, "mtsManagerProxyClient::ManagerClientI - terminated");
#endif
}

void mtsManagerProxyClient::ManagerClientI::Stop()
{
    if (!IsActiveProxy()) return;

    ManagerProxyClient = 0;

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
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "Stopped and destroyed callback thread to communicate with server");
#endif
}

bool mtsManagerProxyClient::ManagerClientI::IsActiveProxy() const
{
    if (ManagerProxyClient) {
        return ManagerProxyClient->IsActiveProxy();
    } else {
        return false;
    }
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
    LogPrint(ManagerClientI, "<<<<< RECV: CreateInterfaceProvidedProxy: " << serverComponentProxyName << ", " << providedInterfaceDescription.InterfaceName);
#endif

    return ManagerProxyClient->ReceiveCreateInterfaceProvidedProxy(serverComponentProxyName, providedInterfaceDescription);
}

bool mtsManagerProxyClient::ManagerClientI::CreateInterfaceRequiredProxy(const std::string & clientComponentProxyName, const ::mtsManagerProxy::InterfaceRequiredDescription & requiredInterfaceDescription, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: CreateInterfaceRequiredProxy: " << clientComponentProxyName << ", " << requiredInterfaceDescription.InterfaceName);
#endif

    return ManagerProxyClient->ReceiveCreateInterfaceRequiredProxy(clientComponentProxyName, requiredInterfaceDescription);
}

bool mtsManagerProxyClient::ManagerClientI::RemoveInterfaceProvidedProxy(const std::string & componentProxyName, const std::string & providedInterfaceProxyName, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: RemoveInterfaceProvidedProxy: " << componentProxyName << ", " << providedInterfaceProxyName);
#endif

    return ManagerProxyClient->ReceiveRemoveInterfaceProvidedProxy(componentProxyName, providedInterfaceProxyName);
}

bool mtsManagerProxyClient::ManagerClientI::RemoveInterfaceRequiredProxy(const std::string & componentProxyName, const std::string & requiredInterfaceProxyName, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerClientI, "<<<<< RECV: RemoveInterfaceRequiredProxy: " << componentProxyName << ", " << requiredInterfaceProxyName);
#endif

    return ManagerProxyClient->ReceiveRemoveInterfaceRequiredProxy(componentProxyName, requiredInterfaceProxyName);
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
    LogPrint(ManagerClientI, "<<<<< RECV: GetInterfaceRequiredDescription" << componentName << ", " << requiredInterfaceName << requiredInterfaceDescription.InterfaceName);
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
