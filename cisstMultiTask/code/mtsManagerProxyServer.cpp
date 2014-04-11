/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2010-01-20

  (C) Copyright 2010-2012 Johns Hopkins University (JHU), All Rights
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

#include "mtsManagerProxyServer.h"
#include <cisstMultiTask/mtsManagerGlobal.h>
#include <cisstMultiTask/mtsManagerComponentClient.h>

std::string mtsManagerProxyServer::ManagerCommunicatorID = "ManagerServerCommunicator";
std::string mtsManagerProxyServer::ConnectionIDKey = "ManagerConnectionID";
unsigned int mtsManagerProxyServer::InstanceCounter = 0;

/*! Construct mtsManagerProxy::ConnectionStringSet structure */
void ConstructConnectionStringSet(
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceName,
    ::mtsManagerProxy::ConnectionStringSet & connectionStringSet)
{
    connectionStringSet.ClientProcessName = clientProcessName;
    connectionStringSet.ClientComponentName = clientComponentName;
    connectionStringSet.ClientInterfaceName = clientInterfaceName;
    connectionStringSet.ServerProcessName = serverProcessName;
    connectionStringSet.ServerComponentName = serverComponentName;
    connectionStringSet.ServerInterfaceName = serverInterfaceName;
}

mtsManagerProxyServer::mtsManagerProxyServer(const std::string & adapterName, const std::string & communicatorID)
    : BaseServerType("config.GCM", adapterName, communicatorID, false)
#if IMPROVE_ICE_THREADING
      , IceThreadInitEvent(0)
#endif
{
    ProxyName = "ManagerProxyServer";

#if IMPROVE_ICE_THREADING
    IceThreadInitEvent = new osaThreadSignal;
#endif
}

mtsManagerProxyServer::~mtsManagerProxyServer()
{
    StopProxy();

#if IMPROVE_ICE_THREADING
    delete IceThreadInitEvent;
#endif
}

std::string mtsManagerProxyServer::GetConfigFullName(const std::string & propertyFileName)
{
    cmnPath path;
#if CISST_HAS_IOS
    path.Add("./");
#else
    path.AddRelativeToCisstShare("cisstMultiTask/Ice");
    path.AddFromEnvironment("PATH", cmnPath::TAIL);
#endif
    return path.Find(propertyFileName);
}

std::string mtsManagerProxyServer::GetGCMPortNumberAsString()
{
    Ice::InitializationData initData;
    initData.properties = Ice::createProperties();
    initData.properties->load(GetConfigFullName("config.GCM"));

    return initData.properties->getProperty("GCM.Port");
}

int mtsManagerProxyServer::GetGCMConnectTimeout()
{
    Ice::InitializationData initData;
    initData.properties = Ice::createProperties();
    initData.properties->load(GetConfigFullName("config.GCM"));

    const std::string connectTimeoutString =
        initData.properties->getProperty("Ice.Override.ConnectTimeout");

    return atoi(connectTimeoutString.c_str());
}

//-----------------------------------------------------------------------------
//  Proxy Start-up
//-----------------------------------------------------------------------------
bool mtsManagerProxyServer::StartProxy(mtsManagerGlobal * proxyOwner)
{
    // Initialize Ice object.
    IceInitialize();

    if (!InitSuccessFlag) {
        LogError(mtsManagerProxyServer, "ICE proxy Initialization failed");
        return false;
    }

    // Register this proxy to the owner
    if (!proxyOwner->AddProcessObject(this, true)) {
        LogError(mtsManagerProxyServer, "failed to register proxy server to Global Component Manager");
        return false;
    }

    // Set the owner of this proxy object
    SetProxyOwner(proxyOwner);

    // Create a worker thread here and returns immediately.
    ThreadArgumentsInfo.Proxy = this;
    ThreadArgumentsInfo.Runner = mtsManagerProxyServer::Runner;

    // Set a short name of this thread as MPS which means "Manager Proxy Server."
    // Such a condensed naming rule is required because a total number of
    // characters in a thread name is sometimes limited to a small number (e.g.
    // LINUX RTAI).
    std::stringstream ss;
    ss << "MPS" << mtsManagerProxyServer::InstanceCounter++;
    std::string threadName = ss.str();

    // Create worker thread. It is created here but does not get started here.
    WorkerThread.Create<ProxyWorker<mtsManagerGlobal>, ThreadArguments<mtsManagerGlobal>*>(
        &ProxyWorkerInfo, &ProxyWorker<mtsManagerGlobal>::Run, &ThreadArgumentsInfo, threadName.c_str());

#if IMPROVE_ICE_THREADING
    // Wait for Ice thread to start
    IceThreadInitEvent->Wait();
#endif

    return true;
}

Ice::ObjectPtr mtsManagerProxyServer::CreateServant(void)
{
    Sender = new ManagerServerI(IceCommunicator, IceLogger, this);

    return Sender;
}

void mtsManagerProxyServer::RemoveServant(void)
{
    Sender->Stop();

    // MJ TDOO: iterate all clients and stop/clean-up all proxies
    // CloseAllClients() - defined in mtsProxyBaseServer.h
    //Sender = 0;
}

void mtsManagerProxyServer::StartServer(void)
{
    Sender->Start();

    ChangeProxyState(PROXY_STATE_ACTIVE);
#if IMPROVE_ICE_THREADING
    IceThreadInitEvent->Raise();
#endif

    // This is a blocking call that should run in a different thread.
    IceCommunicator->waitForShutdown();
}

void mtsManagerProxyServer::Runner(ThreadArguments<mtsManagerGlobal> * arguments)
{
    mtsManagerProxyServer * ProxyServer = dynamic_cast<mtsManagerProxyServer*>(arguments->Proxy);
    if (!ProxyServer) {
        CMN_LOG_RUN_ERROR << "mtsManagerProxyServer: failed to get proxy server" << std::endl;
        return;
    }

    ProxyServer->GetLogger()->trace("mtsManagerProxyServer", "proxy server starts");

    try {
        ProxyServer->StartServer();
    } catch (const Ice::Exception& e) {
        std::string error("mtsManagerProxyServer: ");
        error += e.what();
        ProxyServer->GetLogger()->error(error);
    } catch (...) {
        std::string error("mtsManagerProxyServer: exception at mtsManagerProxyServer::Runner()");
        ProxyServer->GetLogger()->error(error);
    }
}

void mtsManagerProxyServer::StopProxy(void)
{
    if (!IsActiveProxy()) return;

    try {
        BaseServerType::StopProxy();
        Sender->Stop();
    } catch (const Ice::Exception& e) {
        std::string error("mtsManagerProxyServer: ");
        error += e.what();
        LogError(mtsManagerProxyServer, error);
    }

    IceGUID = "";

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, "Stopped manager proxy server");
#endif
}

bool mtsManagerProxyServer::OnClientDisconnect(const ClientIDType clientID)
{
    if (!IsActiveProxy()) return true;

    const std::string processName(clientID);

    LogError(mtsManagerProxyServer, "Global Component Manager detected LCM DISCONNECTION: \"" << processName << "\"");

    // Get network proxy client serving the client with the processName
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(processName);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "OnClientDisconnect: no client proxy found with client id: \"" << processName << "\"");
        return false;
    }

    // Remove client from client list to prevent further requests from network layer
    if (!BaseServerType::RemoveClientByClientID(processName)) {
        LogError(mtsManagerProxyServer, "OnClientDisconnect: failed to remove client from client map: \"" << processName << "\"");
        return false;
    }

    // Remove disconnected process from GCM
    if (!ProxyOwner->RemoveProcess(processName, true)) {
        LogError(mtsManagerProxyServer, "OnClientDisconnect: failed to remove disconnected process: \"" << processName << "\"");
        return false;
    }

    // Remove MCC proxy from GCM
    /* MJ TODO: remove this later
    const std::string nameOfMCCProxy = mtsManagerGlobal::GetComponentProxyName(
        processName, mtsManagerComponentBase::GetNameOfManagerComponentClientFor(processName));
    if (!ProxyOwner->RemoveComponent(mtsManagerLocal::ProcessNameOfLCMWithGCM, nameOfMCCProxy)) {
        LogError(mtsManagerProxyServer, "OnClientDisconnect: failed to remove MCC proxy for disconnected process: \"" << processName << "\"");
        return false;
    }
    //*/

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, "OnClientDisconnect: successfully removed disconnected process: \"" << processName << "\"");
#endif

    return true;
}

void mtsManagerProxyServer::MonitorConnections(void)
{
    BaseServerType::Monitor();
}

void mtsManagerProxyServer::CheckConnectConfirmTimeout(void)
{
    ProxyOwner->CheckConnectConfirmTimeout();
}

mtsManagerProxyServer::ManagerClientProxyType * mtsManagerProxyServer::GetNetworkProxyClient(const ClientIDType clientID)
{
    ManagerClientProxyType * clientProxy = GetClientByClientID(clientID);
    if (!clientProxy) {
        return 0;
    }

    // Check if this network proxy server is active. We don't need to check if
    // a proxy client is still active since any disconnection or inactive proxy
    // has already been detected and taken care of.
    return (IsActiveProxy() ? clientProxy : 0);
}

void mtsManagerProxyServer::ConvertInterfaceProvidedDescription(
    const ::mtsManagerProxy::InterfaceProvidedDescription & src, mtsInterfaceProvidedDescription & dest)
{
    // Initialize destination structure
    dest.CommandsVoid.clear();
    dest.CommandsWrite.clear();
    dest.CommandsRead.clear();
    dest.CommandsQualifiedRead.clear();
    dest.CommandsVoidReturn.clear();
    dest.CommandsWriteReturn.clear();
    dest.EventsVoid.clear();
    dest.EventsWrite.clear();

    // Conversion of provided interface name
    dest.InterfaceName = src.InterfaceName;

    // Conversion of command void vector
    mtsCommandVoidDescription commandVoid;
    ::mtsManagerProxy::CommandVoidSequence::const_iterator itVoid = src.CommandsVoid.begin();
    const ::mtsManagerProxy::CommandVoidSequence::const_iterator itVoidEnd = src.CommandsVoid.end();
    for (; itVoid != itVoidEnd; ++itVoid) {
        commandVoid.Name = itVoid->Name;
        dest.CommandsVoid.push_back(commandVoid);
    }

    // Conversion of command write vector
    mtsCommandWriteDescription commandWrite;
    ::mtsManagerProxy::CommandWriteSequence::const_iterator itWrite = src.CommandsWrite.begin();
    const ::mtsManagerProxy::CommandWriteSequence::const_iterator itWriteEnd = src.CommandsWrite.end();
    for (; itWrite != itWriteEnd; ++itWrite) {
        commandWrite.Name = itWrite->Name;
        commandWrite.ArgumentPrototypeSerialized = itWrite->ArgumentPrototypeSerialized;
        dest.CommandsWrite.push_back(commandWrite);
    }

    // Conversion of command read vector
    mtsCommandReadDescription commandRead;
    ::mtsManagerProxy::CommandReadSequence::const_iterator itRead = src.CommandsRead.begin();
    const ::mtsManagerProxy::CommandReadSequence::const_iterator itReadEnd = src.CommandsRead.end();
    for (; itRead != itReadEnd; ++itRead) {
        commandRead.Name = itRead->Name;
        commandRead.ArgumentPrototypeSerialized = itRead->ArgumentPrototypeSerialized;
        dest.CommandsRead.push_back(commandRead);
    }

    // Conversion of command qualified read vector
    mtsCommandQualifiedReadDescription commandQualifiedRead;
    ::mtsManagerProxy::CommandQualifiedReadSequence::const_iterator itQualifiedRead = src.CommandsQualifiedRead.begin();
    const ::mtsManagerProxy::CommandQualifiedReadSequence::const_iterator itQualifiedReadEnd = src.CommandsQualifiedRead.end();
    for (; itQualifiedRead != itQualifiedReadEnd; ++itQualifiedRead) {
        commandQualifiedRead.Name = itQualifiedRead->Name;
        commandQualifiedRead.Argument1PrototypeSerialized = itQualifiedRead->Argument1PrototypeSerialized;
        commandQualifiedRead.Argument2PrototypeSerialized = itQualifiedRead->Argument2PrototypeSerialized;
        dest.CommandsQualifiedRead.push_back(commandQualifiedRead);
    }

    // Conversion of command void return vector
    mtsCommandVoidReturnDescription commandVoidReturn;
    ::mtsManagerProxy::CommandVoidReturnSequence::const_iterator itVoidReturn = src.CommandsVoidReturn.begin();
    const ::mtsManagerProxy::CommandVoidReturnSequence::const_iterator itVoidReturnEnd = src.CommandsVoidReturn.end();
    for (; itVoidReturn != itVoidReturnEnd; ++itVoidReturn) {
        commandVoidReturn.Name = itVoidReturn->Name;
        commandVoidReturn.ResultPrototypeSerialized = itVoidReturn->ResultPrototypeSerialized;
        dest.CommandsVoidReturn.push_back(commandVoidReturn);
    }

    // Conversion of command write return vector
    mtsCommandWriteReturnDescription commandWriteReturn;
    ::mtsManagerProxy::CommandWriteReturnSequence::const_iterator itWriteReturn = src.CommandsWriteReturn.begin();
    const ::mtsManagerProxy::CommandWriteReturnSequence::const_iterator itWriteReturnEnd = src.CommandsWriteReturn.end();
    for (; itWriteReturn != itWriteReturnEnd; ++itWriteReturn) {
        commandWriteReturn.Name = itWriteReturn->Name;
        commandWriteReturn.ArgumentPrototypeSerialized = itWriteReturn->ArgumentPrototypeSerialized;
        commandWriteReturn.ResultPrototypeSerialized = itWriteReturn->ResultPrototypeSerialized;
        dest.CommandsWriteReturn.push_back(commandWriteReturn);
    }

    // Conversion of event void generator vector
    mtsEventVoidDescription eventVoid;
    ::mtsManagerProxy::EventVoidSequence::const_iterator itEventVoid = src.EventsVoid.begin();
    const ::mtsManagerProxy::EventVoidSequence::const_iterator itEventVoidEnd = src.EventsVoid.end();
    for (; itEventVoid != itEventVoidEnd; ++itEventVoid) {
        eventVoid.Name = itEventVoid->Name;
        dest.EventsVoid.push_back(eventVoid);
    }

    // Conversion of event write generator vector
    mtsEventWriteDescription eventWrite;
    ::mtsManagerProxy::EventWriteSequence::const_iterator itEventWrite = src.EventsWrite.begin();
    const ::mtsManagerProxy::EventWriteSequence::const_iterator itEventWriteEnd = src.EventsWrite.end();
    for (; itEventWrite != itEventWriteEnd; ++itEventWrite) {
        eventWrite.Name = itEventWrite->Name;
        eventWrite.ArgumentPrototypeSerialized = itEventWrite->ArgumentPrototypeSerialized;
        dest.EventsWrite.push_back(eventWrite);
    }
}

void mtsManagerProxyServer::ConvertInterfaceRequiredDescription(
    const ::mtsManagerProxy::InterfaceRequiredDescription & src, mtsInterfaceRequiredDescription & dest)
{
    // Initialize destination structure
    dest.FunctionVoidNames.clear();
    dest.FunctionWriteNames.clear();
    dest.FunctionReadNames.clear();
    dest.FunctionQualifiedReadNames.clear();
    dest.FunctionVoidReturnNames.clear();
    dest.FunctionWriteReturnNames.clear();
    dest.EventHandlersVoid.clear();
    dest.EventHandlersWrite.clear();

    // Conversion of required interface name
    dest.InterfaceName = src.InterfaceName;

    // Conversion of "IsRequired" attribute
    dest.IsRequired = src.IsRequired;

    // Conversion of function void vector
    dest.FunctionVoidNames.insert(dest.FunctionVoidNames.begin(), src.FunctionVoidNames.begin(), src.FunctionVoidNames.end());

    // Conversion of function write vector
    dest.FunctionWriteNames.insert(dest.FunctionWriteNames.begin(), src.FunctionWriteNames.begin(), src.FunctionWriteNames.end());

    // Conversion of function read vector
    dest.FunctionReadNames.insert(dest.FunctionReadNames.begin(), src.FunctionReadNames.begin(), src.FunctionReadNames.end());

    // Conversion of function qualified read vector
    dest.FunctionQualifiedReadNames.insert(dest.FunctionQualifiedReadNames.begin(), src.FunctionQualifiedReadNames.begin(), src.FunctionQualifiedReadNames.end());

    // Conversion of function void return vector
    dest.FunctionVoidReturnNames.insert(dest.FunctionVoidReturnNames.begin(), src.FunctionVoidReturnNames.begin(), src.FunctionVoidReturnNames.end());

    // Conversion of function write return vector
    dest.FunctionWriteReturnNames.insert(dest.FunctionWriteReturnNames.begin(), src.FunctionWriteReturnNames.begin(), src.FunctionWriteReturnNames.end());

    // Conversion of event void handler vector
    mtsCommandVoidDescription eventVoidHandler;
    ::mtsManagerProxy::CommandVoidSequence::const_iterator itEventVoid = src.EventHandlersVoid.begin();
    const ::mtsManagerProxy::CommandVoidSequence::const_iterator itEventVoidEnd = src.EventHandlersVoid.end();
    for (; itEventVoid != itEventVoidEnd; ++itEventVoid) {
        eventVoidHandler.Name = itEventVoid->Name;
        dest.EventHandlersVoid.push_back(eventVoidHandler);
    }

    // Conversion of event write handler vector
    mtsCommandWriteDescription eventWriteHandler;
    ::mtsManagerProxy::CommandWriteSequence::const_iterator itEventWrite = src.EventHandlersWrite.begin();
    const ::mtsManagerProxy::CommandWriteSequence::const_iterator itEventWriteEnd = src.EventHandlersWrite.end();
    for (; itEventWrite != itEventWriteEnd; ++itEventWrite) {
        eventWriteHandler.Name = itEventWrite->Name;
        eventWriteHandler.ArgumentPrototypeSerialized = itEventWrite->ArgumentPrototypeSerialized;
        dest.EventHandlersWrite.push_back(eventWriteHandler);
    }
}

void mtsManagerProxyServer::ConstructInterfaceProvidedDescriptionFrom(
    const mtsInterfaceProvidedDescription & src, ::mtsManagerProxy::InterfaceProvidedDescription & dest)
{
    // Initialize destination structure
    dest.CommandsVoid.clear();
    dest.CommandsWrite.clear();
    dest.CommandsRead.clear();
    dest.CommandsQualifiedRead.clear();
    dest.CommandsVoidReturn.clear();
    dest.CommandsWriteReturn.clear();
    dest.EventsVoid.clear();
    dest.EventsWrite.clear();

    // Construct provided interface name
    dest.InterfaceName = src.InterfaceName;

    // Construct command void vector
    ::mtsManagerProxy::CommandVoidElement commandVoid;
    mtsCommandsVoidDescription::const_iterator itVoid = src.CommandsVoid.begin();
    const mtsCommandsVoidDescription::const_iterator itVoidEnd = src.CommandsVoid.end();
    for (; itVoid != itVoidEnd; ++itVoid) {
        commandVoid.Name = itVoid->Name;
        dest.CommandsVoid.push_back(commandVoid);
    }

    // Construct command write vector
    ::mtsManagerProxy::CommandWriteElement commandWrite;
    mtsCommandsWriteDescription::const_iterator itWrite = src.CommandsWrite.begin();
    const mtsCommandsWriteDescription::const_iterator itWriteEnd = src.CommandsWrite.end();
    for (; itWrite != itWriteEnd; ++itWrite) {
        commandWrite.Name = itWrite->Name;
        commandWrite.ArgumentPrototypeSerialized = itWrite->ArgumentPrototypeSerialized;
        dest.CommandsWrite.push_back(commandWrite);
    }

    // Construct command read vector
    ::mtsManagerProxy::CommandReadElement commandRead;
    mtsCommandsReadDescription::const_iterator itRead = src.CommandsRead.begin();
    const mtsCommandsReadDescription::const_iterator itReadEnd = src.CommandsRead.end();
    for (; itRead != itReadEnd; ++itRead) {
        commandRead.Name = itRead->Name;
        commandRead.ArgumentPrototypeSerialized = itRead->ArgumentPrototypeSerialized;
        dest.CommandsRead.push_back(commandRead);
    }

    // Construct command QualifiedRead vector
    ::mtsManagerProxy::CommandQualifiedReadElement commandQualifiedRead;
    mtsCommandsQualifiedReadDescription::const_iterator itQualifiedRead = src.CommandsQualifiedRead.begin();
    const mtsCommandsQualifiedReadDescription::const_iterator itQualifiedReadEnd = src.CommandsQualifiedRead.end();
    for (; itQualifiedRead != itQualifiedReadEnd; ++itQualifiedRead) {
        commandQualifiedRead.Name = itQualifiedRead->Name;
        commandQualifiedRead.Argument1PrototypeSerialized = itQualifiedRead->Argument1PrototypeSerialized;
        commandQualifiedRead.Argument2PrototypeSerialized = itQualifiedRead->Argument2PrototypeSerialized;
        dest.CommandsQualifiedRead.push_back(commandQualifiedRead);
    }

    // Construct command VoidReturn vector
    ::mtsManagerProxy::CommandVoidReturnElement commandVoidReturn;
    mtsCommandsVoidReturnDescription::const_iterator itVoidReturn = src.CommandsVoidReturn.begin();
    const mtsCommandsVoidReturnDescription::const_iterator itVoidReturnEnd = src.CommandsVoidReturn.end();
    for (; itVoidReturn != itVoidReturnEnd; ++itVoidReturn) {
        commandVoidReturn.Name = itVoidReturn->Name;
        commandVoidReturn.ResultPrototypeSerialized = itVoidReturn->ResultPrototypeSerialized;
        dest.CommandsVoidReturn.push_back(commandVoidReturn);
    }

    // Construct command WriteReturn vector
    ::mtsManagerProxy::CommandWriteReturnElement commandWriteReturn;
    mtsCommandsWriteReturnDescription::const_iterator itWriteReturn = src.CommandsWriteReturn.begin();
    const mtsCommandsWriteReturnDescription::const_iterator itWriteReturnEnd = src.CommandsWriteReturn.end();
    for (; itWriteReturn != itWriteReturnEnd; ++itWriteReturn) {
        commandWriteReturn.Name = itWriteReturn->Name;
        commandWriteReturn.ArgumentPrototypeSerialized = itWriteReturn->ArgumentPrototypeSerialized;
        commandWriteReturn.ResultPrototypeSerialized = itWriteReturn->ResultPrototypeSerialized;
        dest.CommandsWriteReturn.push_back(commandWriteReturn);
    }

    // Construct event void generator vector
    ::mtsManagerProxy::EventVoidElement eventVoidGenerator;
    mtsEventsVoidDescription::const_iterator itEventVoid = src.EventsVoid.begin();
    const mtsEventsVoidDescription::const_iterator itEventVoidEnd = src.EventsVoid.end();
    for (; itEventVoid != itEventVoidEnd; ++itEventVoid) {
        eventVoidGenerator.Name = itEventVoid->Name;
        dest.EventsVoid.push_back(eventVoidGenerator);
    }

    // Construct event write generator vector
    ::mtsManagerProxy::EventWriteElement eventWriteGenerator;
    mtsEventsWriteDescription::const_iterator itEventWrite = src.EventsWrite.begin();
    const mtsEventsWriteDescription::const_iterator itEventWriteEnd = src.EventsWrite.end();
    for (; itEventWrite != itEventWriteEnd; ++itEventWrite) {
        eventWriteGenerator.Name = itEventWrite->Name;
        eventWriteGenerator.ArgumentPrototypeSerialized = itEventWrite->ArgumentPrototypeSerialized;
        dest.EventsWrite.push_back(eventWriteGenerator);
    }
}

void mtsManagerProxyServer::ConstructInterfaceRequiredDescriptionFrom(
    const mtsInterfaceRequiredDescription & src, ::mtsManagerProxy::InterfaceRequiredDescription & dest)
{
    // Initialize destination structure
    dest.FunctionVoidNames.clear();
    dest.FunctionWriteNames.clear();
    dest.FunctionReadNames.clear();
    dest.FunctionQualifiedReadNames.clear();
    dest.FunctionVoidReturnNames.clear();
    dest.FunctionWriteReturnNames.clear();
    dest.EventHandlersVoid.clear();
    dest.EventHandlersWrite.clear();

    // Construct required interface name
    dest.InterfaceName = src.InterfaceName;

    // Construct "IsRequired" attribute
    dest.IsRequired = src.IsRequired;

    // Construct function void vector
    dest.FunctionVoidNames.insert(dest.FunctionVoidNames.begin(), src.FunctionVoidNames.begin(), src.FunctionVoidNames.end());

    // Construct function write vector
    dest.FunctionWriteNames.insert(dest.FunctionWriteNames.begin(), src.FunctionWriteNames.begin(), src.FunctionWriteNames.end());

    // Construct function read vector
    dest.FunctionReadNames.insert(dest.FunctionReadNames.begin(), src.FunctionReadNames.begin(), src.FunctionReadNames.end());

    // Construct function qualified read vector
    dest.FunctionQualifiedReadNames.insert(dest.FunctionQualifiedReadNames.begin(), src.FunctionQualifiedReadNames.begin(), src.FunctionQualifiedReadNames.end());

    // Construct function void return vector
    dest.FunctionVoidReturnNames.insert(dest.FunctionVoidReturnNames.begin(), src.FunctionVoidReturnNames.begin(), src.FunctionVoidReturnNames.end());

    // Construct function write return vector
    dest.FunctionWriteReturnNames.insert(dest.FunctionWriteReturnNames.begin(), src.FunctionWriteReturnNames.begin(), src.FunctionWriteReturnNames.end());

    // Construct event void handler vector
    ::mtsManagerProxy::CommandVoidElement eventVoidHandler;
    mtsCommandsVoidDescription::const_iterator itEventVoid = src.EventHandlersVoid.begin();
    const mtsCommandsVoidDescription::const_iterator itEventVoidEnd = src.EventHandlersVoid.end();
    for (; itEventVoid != itEventVoidEnd; ++itEventVoid) {
        eventVoidHandler.Name = itEventVoid->Name;
        dest.EventHandlersVoid.push_back(eventVoidHandler);
    }

    // Construct event write handler vector
    ::mtsManagerProxy::CommandWriteElement eventWriteHandler;
    mtsCommandsWriteDescription::const_iterator itEventWrite = src.EventHandlersWrite.begin();
    const mtsCommandsWriteDescription::const_iterator itEventWriteEnd = src.EventHandlersWrite.end();
    for (; itEventWrite != itEventWriteEnd; ++itEventWrite) {
        eventWriteHandler.Name = itEventWrite->Name;
        eventWriteHandler.ArgumentPrototypeSerialized = itEventWrite->ArgumentPrototypeSerialized;
        dest.EventHandlersWrite.push_back(eventWriteHandler);
    }
}

//-------------------------------------------------------------------------
//  Implementation of mtsManagerLocalInterface
//  (See mtsManagerLocalInterface.h for comments)
//-------------------------------------------------------------------------
bool mtsManagerProxyServer::CreateComponentProxy(const std::string & componentProxyName, const std::string & listenerID)
{
    return SendCreateComponentProxy(componentProxyName, listenerID);
}

bool mtsManagerProxyServer::RemoveComponentProxy(const std::string & componentProxyName, const std::string & listenerID)
{
    return SendRemoveComponentProxy(componentProxyName, listenerID);
}

bool mtsManagerProxyServer::CreateInterfaceProvidedProxy(const std::string & serverComponentProxyName,
    const mtsInterfaceProvidedDescription & providedInterfaceDescription, const std::string & listenerID)
{
    // Convert providedInterfaceDescription to an object of type mtsManagerProxy::InterfaceProvidedDescription
    mtsManagerProxy::InterfaceProvidedDescription interfaceDescription;
    mtsManagerProxyServer::ConstructInterfaceProvidedDescriptionFrom(providedInterfaceDescription, interfaceDescription);

    return SendCreateInterfaceProvidedProxy(serverComponentProxyName, interfaceDescription, listenerID);
}

bool mtsManagerProxyServer::CreateInterfaceRequiredProxy(const std::string & clientComponentProxyName,
    const mtsInterfaceRequiredDescription & requiredInterfaceDescription, const std::string & listenerID)
{
    // Convert requiredInterfaceDescription to an object of type mtsManagerProxy::InterfaceRequiredDescription
    mtsManagerProxy::InterfaceRequiredDescription interfaceDescription;
    mtsManagerProxyServer::ConstructInterfaceRequiredDescriptionFrom(requiredInterfaceDescription, interfaceDescription);

    return SendCreateInterfaceRequiredProxy(clientComponentProxyName, interfaceDescription, listenerID);
}

bool mtsManagerProxyServer::RemoveInterfaceProvidedProxy(
    const std::string & componentProxyName, const std::string & providedInterfaceProxyName, const std::string & listenerID)
{
    return SendRemoveInterfaceProvidedProxy(componentProxyName, providedInterfaceProxyName, listenerID);
}

bool mtsManagerProxyServer::RemoveInterfaceRequiredProxy(
    const std::string & componentProxyName, const std::string & requiredInterfaceProxyName, const std::string & listenerID)
{
    return SendRemoveInterfaceRequiredProxy(componentProxyName, requiredInterfaceProxyName, listenerID);
}

bool mtsManagerProxyServer::ConnectServerSideInterface(const mtsDescriptionConnection & description, const std::string & listenerID)
{
    // Create an instance of mtsManagerProxy::ConnectionStringSet out of a set of strings
    ::mtsManagerProxy::ConnectionStringSet connectionStringSet;
    ConstructConnectionStringSet(
        description.Client.ProcessName, description.Client.ComponentName, description.Client.InterfaceName,
        description.Server.ProcessName, description.Server.ComponentName, description.Server.InterfaceName,
        connectionStringSet);

    return SendConnectServerSideInterface(description.ConnectionID, connectionStringSet, listenerID);
}

bool mtsManagerProxyServer::ConnectClientSideInterface(const mtsDescriptionConnection & description, const std::string & listenerID)
{
    // Create an instance of mtsManagerProxy::ConnectionStringSet out of a set of strings given
    ::mtsManagerProxy::ConnectionStringSet connectionStringSet;
    ConstructConnectionStringSet(
        description.Client.ProcessName, description.Client.ComponentName, description.Client.InterfaceName,
        description.Server.ProcessName, description.Server.ComponentName, description.Server.InterfaceName,
        connectionStringSet);

    return SendConnectClientSideInterface(description.ConnectionID, connectionStringSet, listenerID);
}

bool mtsManagerProxyServer::GetInterfaceProvidedDescription(
    const std::string & serverComponentName, const std::string & providedInterfaceName,
    mtsInterfaceProvidedDescription & providedInterfaceDescription, const std::string & listenerID)
{
    mtsManagerProxy::InterfaceProvidedDescription src;

    if (!SendGetInterfaceProvidedDescription(serverComponentName, providedInterfaceName, src, listenerID)) {
        LogError(mtsManagerProxyServer, "GetInterfaceProvidedDescription() failed");
        return false;
    }

    // Convert mtsManagerProxy::InterfaceProvidedDescription to mtsInterfaceCommon::InterfaceProvidedDescription
    mtsManagerProxyServer::ConvertInterfaceProvidedDescription(src, providedInterfaceDescription);

    return true;
}

bool mtsManagerProxyServer::GetInterfaceRequiredDescription(
    const std::string & componentName, const std::string & requiredInterfaceName,
    mtsInterfaceRequiredDescription & requiredInterfaceDescription, const std::string & listenerID)
{
    mtsManagerProxy::InterfaceRequiredDescription src;

    if (!SendGetInterfaceRequiredDescription(componentName, requiredInterfaceName, src, listenerID)) {
        LogError(mtsManagerProxyServer, "GetInterfaceRequiredDescription() failed");
        return false;
    }

    // Construct an instance of type InterfaceRequiredDescription from an object of type mtsManagerProxy::InterfaceRequiredDescription
    mtsManagerProxyServer::ConvertInterfaceRequiredDescription(src, requiredInterfaceDescription);

    return true;
}

const std::string mtsManagerProxyServer::GetProcessName(const std::string & listenerID) const
{
    mtsManagerProxyServer * nonConst = const_cast<mtsManagerProxyServer *>(this);
    return nonConst->SendGetProcessName(listenerID);
}

void mtsManagerProxyServer::GetNamesOfCommands(std::vector<std::string>& namesOfCommands,
                                               const std::string & componentName, const std::string & providedInterfaceName,
                                               const std::string & listenerID)
{
    SendGetNamesOfCommands(namesOfCommands, componentName, providedInterfaceName, listenerID);
}

void mtsManagerProxyServer::GetNamesOfEventGenerators(std::vector<std::string>& namesOfEventGenerators,
                                                      const std::string & componentName, const std::string & providedInterfaceName,
                                                      const std::string & listenerID)
{
    SendGetNamesOfEventGenerators(namesOfEventGenerators, componentName, providedInterfaceName, listenerID);
}

void mtsManagerProxyServer::GetNamesOfFunctions(std::vector<std::string>& namesOfFunctions,
                                                const std::string & componentName, const std::string & requiredInterfaceName,
                                                const std::string & listenerID)
{
    SendGetNamesOfFunctions(namesOfFunctions, componentName, requiredInterfaceName, listenerID);
}

void mtsManagerProxyServer::GetNamesOfEventHandlers(std::vector<std::string>& namesOfEventHandlers,
                                                    const std::string & componentName, const std::string & requiredInterfaceName,
                                                    const std::string & listenerID)
{
    SendGetNamesOfEventHandlers(namesOfEventHandlers, componentName, requiredInterfaceName, listenerID);
}

void mtsManagerProxyServer::GetDescriptionOfCommand(std::string & description,
                                                    const std::string & componentName, const std::string & providedInterfaceName, const std::string & commandName,
                                                    const std::string & listenerID)
{
    SendGetDescriptionOfCommand(description, componentName, providedInterfaceName, commandName, listenerID);
}

void mtsManagerProxyServer::GetDescriptionOfEventGenerator(std::string & description,
                                                           const std::string & componentName, const std::string & providedInterfaceName, const std::string & eventGeneratorName,
                                                           const std::string & listenerID)
{
    SendGetDescriptionOfEventGenerator(description, componentName, providedInterfaceName, eventGeneratorName, listenerID);
}

void mtsManagerProxyServer::GetDescriptionOfFunction(std::string & description,
                                                     const std::string & componentName, const std::string & requiredInterfaceName, const std::string & functionName,
                                                     const std::string & listenerID)
{
    SendGetDescriptionOfFunction(description, componentName, requiredInterfaceName, functionName, listenerID);
}

void mtsManagerProxyServer::GetDescriptionOfEventHandler(std::string & description,
                                                         const std::string & componentName, const std::string & requiredInterfaceName, const std::string & eventHandlerName,
                                                         const std::string & listenerID)
{
    SendGetDescriptionOfEventHandler(description, componentName, requiredInterfaceName, eventHandlerName, listenerID);
}

//-------------------------------------------------------------------------
//  Event Handlers (Client -> Server)
//-------------------------------------------------------------------------
void mtsManagerProxyServer::ReceiveTestMessageFromClientToServer(
    const IceConnectionIDType & iceConnectionID, const std::string & str)
{
    const ClientIDType clientID = GetClientID(iceConnectionID);

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer,
             "ReceiveTestMessageFromClientToServer: "
             << "\n..... ConnectionID: " << iceConnectionID
             << "\n..... Message: " << str);
#endif

    std::cout << "Server: received from Client " << clientID << ": " << str << std::endl;
}

bool mtsManagerProxyServer::ReceiveAddClient(
    const IceConnectionIDType & iceConnectionID, const std::string & processName,
    ManagerClientProxyType & clientProxy)
{
    if (!AddProxyClient(processName, processName, iceConnectionID, clientProxy)) {
        LogError(mtsManagerProxyServer, "ReceiveAddClient: failed to add proxy client: " << processName);
        return false;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer,
             "ReceiveAddClient: added proxy client: "
             << "\n..... ConnectionID: " << iceConnectionID
             << "\n..... Process name: " << processName);
#endif

    return true;
}

//
// TODO: Implement ReceiveShutdown()
//

bool mtsManagerProxyServer::ReceiveAddProcess(const std::string & processName)
{
    return ProxyOwner->AddProcess(processName);
}

bool mtsManagerProxyServer::ReceiveFindProcess(const std::string & processName) const
{
    return ProxyOwner->FindProcess(processName);
}

bool mtsManagerProxyServer::ReceiveRemoveProcess(const std::string & processName)
{
    return OnClientDisconnect(processName);
}

bool mtsManagerProxyServer::ReceiveAddComponent(const std::string & processName, const std::string & componentName)
{
    return ProxyOwner->AddComponent(processName, componentName);
}

bool mtsManagerProxyServer::ReceiveFindComponent(const std::string & processName, const std::string & componentName) const
{
    return ProxyOwner->FindComponent(processName, componentName);
}

bool mtsManagerProxyServer::ReceiveRemoveComponent(const std::string & processName, const std::string & componentName)
{
    return ProxyOwner->RemoveComponent(processName, componentName);
}

bool mtsManagerProxyServer::ReceiveAddInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName, const std::string & interfaceName)
{
    return ProxyOwner->AddInterfaceProvidedOrOutput(processName, componentName, interfaceName);
}

bool mtsManagerProxyServer::ReceiveFindInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName, const std::string & interfaceName) const
{
    return ProxyOwner->FindInterfaceProvidedOrOutput(processName, componentName, interfaceName);
}

bool mtsManagerProxyServer::ReceiveRemoveInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName, const std::string & interfaceName)
{
    return ProxyOwner->RemoveInterfaceProvidedOrOutput(processName, componentName, interfaceName);
}

bool mtsManagerProxyServer::ReceiveAddInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName, const std::string & interfaceName)
{
    return ProxyOwner->AddInterfaceRequiredOrInput(processName, componentName, interfaceName);
}

bool mtsManagerProxyServer::ReceiveFindInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName, const std::string & interfaceName) const
{
    return ProxyOwner->FindInterfaceRequiredOrInput(processName, componentName, interfaceName);
}

bool mtsManagerProxyServer::ReceiveRemoveInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName, const std::string & interfaceName)
{
    return ProxyOwner->RemoveInterfaceRequiredOrInput(processName, componentName, interfaceName);
}

::Ice::Int mtsManagerProxyServer::ReceiveConnect(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet)
{
    return ProxyOwner->Connect(connectionStringSet.RequestProcessName,
                               connectionStringSet.ClientProcessName, connectionStringSet.ClientComponentName, connectionStringSet.ClientInterfaceName,
                               connectionStringSet.ServerProcessName, connectionStringSet.ServerComponentName, connectionStringSet.ServerInterfaceName);
}

bool mtsManagerProxyServer::ReceiveConnectConfirm(::Ice::Int connectionID)
{
    return ProxyOwner->ConnectConfirm(connectionID);
}

bool mtsManagerProxyServer::ReceiveDisconnectWithID(::Ice::Int connectionID)
{
    return ProxyOwner->Disconnect(connectionID);
}

bool mtsManagerProxyServer::ReceiveDisconnect(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet)
{
    return ProxyOwner->Disconnect(
        connectionStringSet.ClientProcessName, connectionStringSet.ClientComponentName, connectionStringSet.ClientInterfaceName,
        connectionStringSet.ServerProcessName, connectionStringSet.ServerComponentName, connectionStringSet.ServerInterfaceName);
}

bool mtsManagerProxyServer::ReceiveSetInterfaceProvidedProxyAccessInfo(const ConnectionIDType connectionID, const std::string & endpointInfo)
{
    return ProxyOwner->SetInterfaceProvidedProxyAccessInfo(connectionID, endpointInfo);
}

bool mtsManagerProxyServer::ReceiveGetInterfaceProvidedProxyAccessInfo(const ConnectionIDType connectionID, std::string & endpointInfo)
{
    return ProxyOwner->GetInterfaceProvidedProxyAccessInfo(connectionID, endpointInfo);
}

bool mtsManagerProxyServer::ReceiveGetInterfaceProvidedProxyAccessInfo(const std::string & clientProcessName,
    const std::string & serverProcessName, const std::string & serverComponentName,
    const std::string & serverInterfaceName, std::string & endpointInfo)
{
    return ProxyOwner->GetInterfaceProvidedProxyAccessInfo(clientProcessName, serverProcessName, serverComponentName, serverInterfaceName, endpointInfo);
}

bool mtsManagerProxyServer::ReceiveInitiateConnect(const ConnectionIDType connectionID)
{
    return ProxyOwner->InitiateConnect(connectionID);
}

bool mtsManagerProxyServer::ReceiveConnectServerSideInterfaceRequest(const ConnectionIDType connectionID)
{
    return ProxyOwner->ConnectServerSideInterfaceRequest(connectionID);
}

//-------------------------------------------------------------------------
//  Event Generators (Event Sender) : Server -> Client
//-------------------------------------------------------------------------
void mtsManagerProxyServer::SendTestMessageFromServerToClient(const std::string & str)
{
    if (!IsActiveProxy()) return;

    // iterate client map -> send message to ALL clients (broadcasts)
    ManagerClientProxyType * clientProxy;
    ClientIDMapType::iterator it = ClientIDMap.begin();
    ClientIDMapType::const_iterator itEnd = ClientIDMap.end();
    for (; it != itEnd; ++it) {
        clientProxy = &(it->second.ClientProxy);
        try {
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendTestMessageFromServerToClient: " << str);
#endif
            if (!(*clientProxy).get()) continue;
            (*clientProxy)->TestMessageFromServerToClient(str);
        }
        catch (const ::Ice::Exception & ex)
        {
            std::cerr << "Error: " << ex << std::endl;
            if (!OnClientDisconnect(it->second.ClientID)) {
                std::cerr << "Failed to remove client: " << it->second.ClientID << std::endl;
            }
            continue;
        }
    }
}

bool mtsManagerProxyServer::SendCreateComponentProxy(const std::string & componentProxyName, const std::string & clientID)
{
    if (!IsActiveProxy()) return false;

    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy || !clientProxy->get()) {
        LogError(mtsManagerProxyServer, "SendCreateComponentProxy: invalid client id (" << clientID << ") or inactive server proxy");
        return false;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendCreateComponentProxy: " << componentProxyName << ", " << clientID);
#endif

    try {
        return (*clientProxy)->CreateComponentProxy(componentProxyName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyServer, "SendCreateComponentProxy: network exception: " << ex);
        OnClientDisconnect(clientID);
        return false;
    }
}

bool mtsManagerProxyServer::SendRemoveComponentProxy(const std::string & componentProxyName, const std::string & clientID)
{
    if (!IsActiveProxy()) return false;

    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy || !clientProxy->get()) {
        LogError(mtsManagerProxyServer, "SendRemoveComponentProxy: invalid client id (" << clientID << ") or inactive server proxy");
        return false;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendRemoveComponentProxy: " << componentProxyName << ", " << clientID);
#endif

    try {
        return (*clientProxy)->RemoveComponentProxy(componentProxyName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyServer, "SendRemoveComponentProxy: " << componentProxyName << ", " << clientID << ": network exception: " << ex);
        OnClientDisconnect(clientID);
        return false;
    }
}

bool mtsManagerProxyServer::SendCreateInterfaceProvidedProxy(const std::string & serverComponentProxyName,
    const ::mtsManagerProxy::InterfaceProvidedDescription & providedInterfaceDescription, const std::string & clientID)
{
    if (!IsActiveProxy()) return false;

    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy || !clientProxy->get()) {
        LogError(mtsManagerProxyServer, "SendCreateInterfaceProvidedProxy: invalid client id (" << clientID << ") or inactive server proxy");
        return false;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendCreateInterfaceProvidedProxy: " << serverComponentProxyName << ", " << providedInterfaceDescription.InterfaceName << ", " << clientID);
#endif

    try {
        return (*clientProxy)->CreateInterfaceProvidedProxy(serverComponentProxyName, providedInterfaceDescription);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyServer, "SendCreateInterfaceProvidedProxy: network exception: " << ex);
        OnClientDisconnect(clientID);
        return false;
    }
}

bool mtsManagerProxyServer::SendCreateInterfaceRequiredProxy(const std::string & clientComponentProxyName,
    const ::mtsManagerProxy::InterfaceRequiredDescription & requiredInterfaceDescription, const std::string & clientID)
{
    if (!IsActiveProxy()) return false;

    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy || !clientProxy->get()) {
        LogError(mtsManagerProxyServer, "SendCreateInterfaceRequiredProxy: invalid client id (" << clientID << ") or inactive server proxy");
        return false;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendCreateInterfaceRequiredProxy: " << clientComponentProxyName << ", " << requiredInterfaceDescription.InterfaceName << ", " << clientID);
#endif

    try {
        return (*clientProxy)->CreateInterfaceRequiredProxy(clientComponentProxyName, requiredInterfaceDescription);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyServer, "SendCreateInterfaceRequiredProxy: network exception: " << ex);
        OnClientDisconnect(clientID);
        return false;
    }
}

bool mtsManagerProxyServer::SendRemoveInterfaceProvidedProxy(const std::string & componentProxyName,
    const std::string & providedInterfaceProxyName, const std::string & clientID)
{
    if (!IsActiveProxy()) return false;

    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy || !clientProxy->get()) {
        LogError(mtsManagerProxyServer, "SendRemoveInterfaceProvidedProxy: invalid listenerID (" << clientID << ") or inactive server proxy");
        return false;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendRemoveInterfaceProvidedProxy: " << componentProxyName << ", " << providedInterfaceProxyName << ", " << clientID);
#endif

    try {
        return (*clientProxy)->RemoveInterfaceProvidedProxy(componentProxyName, providedInterfaceProxyName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyServer, "SendRemoveInterfaceProvidedProxy: network exception: " << ex);
        OnClientDisconnect(clientID);
        return false;
    }
}

bool mtsManagerProxyServer::SendRemoveInterfaceRequiredProxy(const std::string & componentProxyName,
    const std::string & requiredInterfaceProxyName, const std::string & clientID)
{
    if (!IsActiveProxy()) return false;

    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy || !clientProxy->get()) {
        LogError(mtsManagerProxyServer, "SendRemoveInterfaceRequiredProxy: invalid listenerID (" << clientID << ") or inactive server proxy");
        return false;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendRemoveInterfaceRequiredProxy: " << componentProxyName << ", " << requiredInterfaceProxyName << ", " << clientID);
#endif

    try {
        return (*clientProxy)->RemoveInterfaceRequiredProxy(componentProxyName, requiredInterfaceProxyName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyServer, "SendRemoveInterfaceRequiredProxy: network exception: " << ex);
        OnClientDisconnect(clientID);
        return false;
    }
}

bool mtsManagerProxyServer::SendConnectServerSideInterface(const ConnectionIDType connectionID,
    const ::mtsManagerProxy::ConnectionStringSet & connectionStrings, const std::string & clientID)
{
    if (!IsActiveProxy()) return false;

    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy || !clientProxy->get()) {
        LogError(mtsManagerProxyServer, "SendConnectServerSideInterface: invalid listenerID (" << clientID << ") or inactive server proxy");
        return false;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendConnectServerSideInterface: " << clientID);
#endif

    try {
        return (*clientProxy)->ConnectServerSideInterface(connectionID, connectionStrings);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyServer, "SendConnectServerSideInterface: network exception: " << ex);
        OnClientDisconnect(clientID);
        return false;
    }
}

bool mtsManagerProxyServer::SendConnectClientSideInterface(::Ice::Int connectionID,
    const ::mtsManagerProxy::ConnectionStringSet & connectionStrings, const std::string & clientID)
{
    if (!IsActiveProxy()) return false;

    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy || !clientProxy->get()) {
        LogError(mtsManagerProxyServer, "SendConnectClientSideInterface: invalid listenerID (" << clientID << ") or inactive server proxy");
        return false;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendConnectClientSideInterface: " << connectionID << ", " << clientID);
#endif

    try {
        return (*clientProxy)->ConnectClientSideInterface(connectionID, connectionStrings);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyServer, "SendConnectClientSideInterface: network exception: " << ex);
        OnClientDisconnect(clientID);
        return false;
    }
}

bool mtsManagerProxyServer::SendGetInterfaceProvidedDescription(
    const std::string & serverComponentName, const std::string & providedInterfaceName,
    ::mtsManagerProxy::InterfaceProvidedDescription & providedInterfaceDescription, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy || !clientProxy->get()) {
        LogError(mtsManagerProxyServer, "SendGetInterfaceProvidedDescription: invalid listenerID (" << clientID << ") or inactive server proxy");
        return false;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendGetInterfaceProvidedDescription: " << serverComponentName << ":" << providedInterfaceName << ", " << clientID);
#endif

    try {
        return (*clientProxy)->GetInterfaceProvidedDescription(serverComponentName, providedInterfaceName, providedInterfaceDescription);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyServer, "SendGetInterfaceProvidedDescription: network exception: " << ex);
        OnClientDisconnect(clientID);
        return false;
    }
}

bool mtsManagerProxyServer::SendGetInterfaceRequiredDescription(const std::string & componentName,
    const std::string & requiredInterfaceName, ::mtsManagerProxy::InterfaceRequiredDescription & requiredInterfaceDescription,
    const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy || !clientProxy->get()) {
        LogError(mtsManagerProxyServer, "SendGetInterfaceRequiredDescription: invalid listenerID (" << clientID << ") or inactive server proxy");
        return false;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendGetInterfaceRequiredDescription: " << componentName << ", " << requiredInterfaceName << ", " << clientID);
#endif

    try {
        return (*clientProxy)->GetInterfaceRequiredDescription(componentName, requiredInterfaceName, requiredInterfaceDescription);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyServer, "SendGetInterfaceRequiredDescription: network exception: " << ex);
        OnClientDisconnect(clientID);
        return false;
    }
}

void mtsManagerProxyServer::SendGetNamesOfCommands(std::vector<std::string>& namesOfCommands,
    const std::string & componentName, const std::string & providedInterfaceName, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy || !clientProxy->get()) {
        LogError(mtsManagerProxyServer, "SendGetNamesOfCommands: invalid listenerID (" << clientID << ") or inactive server proxy");
        return;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendGetNamesOfCommands: " << componentName << ", " << providedInterfaceName << ", " << clientID);
#endif

    try {
        (*clientProxy)->GetNamesOfCommands(componentName, providedInterfaceName, namesOfCommands);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyServer, "SendGetNamesOfCommands: network exception: " << ex);
        OnClientDisconnect(clientID);
        return;
    }
}

void mtsManagerProxyServer::SendGetNamesOfEventGenerators(std::vector<std::string>& namesOfEventGenerators,
    const std::string & componentName, const std::string & providedInterfaceName, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy || !clientProxy->get()) {
        LogError(mtsManagerProxyServer, "SendGetNamesOfEventGenerators: invalid listenerID (" << clientID << ") or inactive server proxy");
        return;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendGetNamesOfEventGenerators: " << componentName << ", " << providedInterfaceName << ", " << clientID);
#endif

    try {
        (*clientProxy)->GetNamesOfEventGenerators(componentName, providedInterfaceName, namesOfEventGenerators);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyServer, "SendGetNamesOfEventGenerators: network exception: " << ex);
        OnClientDisconnect(clientID);
        return;
    }
}

void mtsManagerProxyServer::SendGetNamesOfFunctions(std::vector<std::string>& namesOfFunctions,
    const std::string & componentName, const std::string & requiredInterfaceName, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy || !clientProxy->get()) {
        LogError(mtsManagerProxyServer, "SendGetNamesOfFunctions: invalid listenerID (" << clientID << ") or inactive server proxy");
        return;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendGetNamesOfFunctions: " << componentName << ", " << requiredInterfaceName << ", " << clientID);
#endif

    try {
        (*clientProxy)->GetNamesOfFunctions(componentName, requiredInterfaceName, namesOfFunctions);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyServer, "SendGetNamesOfFunctions: network exception: " << ex);
        OnClientDisconnect(clientID);
        return;
    }
}

void mtsManagerProxyServer::SendGetNamesOfEventHandlers(std::vector<std::string>& namesOfEventHandlers,
    const std::string & componentName, const std::string & requiredInterfaceName, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy || !clientProxy->get()) {
        LogError(mtsManagerProxyServer, "SendGetNamesOfEventHandlers: invalid listenerID (" << clientID << ") or inactive server proxy");
        return;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendGetNamesOfEventHandlers: " << componentName << ", " << requiredInterfaceName << ", " << clientID);
#endif

    try {
        (*clientProxy)->GetNamesOfEventHandlers(componentName, requiredInterfaceName, namesOfEventHandlers);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyServer, "SendGetNamesOfEventHandlers: network exception: " << ex);
        OnClientDisconnect(clientID);
        return;
    }
}

void mtsManagerProxyServer::SendGetDescriptionOfCommand(std::string & description, const std::string & componentName,
    const std::string & providedInterfaceName, const std::string & commandName, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy || !clientProxy->get()) {
        LogError(mtsManagerProxyServer, "SendGetDescriptionOfCommand: invalid listenerID (" << clientID << ") or inactive server proxy");
        return;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendGetDescriptionOfCommand: " << componentName << ", " << providedInterfaceName << ", " << commandName << ", " << clientID);
#endif

    try {
        (*clientProxy)->GetDescriptionOfCommand(componentName, providedInterfaceName, commandName, description);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyServer, "SendGetDescriptionOfCommand: network exception: " << ex);
        OnClientDisconnect(clientID);
        return;
    }
}

void mtsManagerProxyServer::SendGetDescriptionOfEventGenerator(std::string & description, const std::string & componentName,
    const std::string & providedInterfaceName, const std::string & eventGeneratorName, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy || !clientProxy->get()) {
        LogError(mtsManagerProxyServer, "SendGetDescriptionOfEventGenerator: invalid listenerID (" << clientID << ") or inactive server proxy");
        return;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendGetDescriptionOfEventGenerator: " << componentName << ", " << providedInterfaceName << ", " << eventGeneratorName << ", " << clientID);
#endif

    try {
        (*clientProxy)->GetDescriptionOfEventGenerator(componentName, providedInterfaceName, eventGeneratorName, description);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyServer, "SendGetDescriptionOfEventGenerator: network exception: " << ex);
        OnClientDisconnect(clientID);
        return;
    }
}

void mtsManagerProxyServer::SendGetDescriptionOfFunction(std::string & description, const std::string & componentName,
    const std::string & requiredInterfaceName, const std::string & functionName, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy || !clientProxy->get()) {
        LogError(mtsManagerProxyServer, "SendGetDescriptionOfFunction: invalid listenerID (" << clientID << ") or inactive server proxy");
        return;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendGetDescriptionOfFunction: " << componentName << ", " << requiredInterfaceName << ", " << functionName << ", " << clientID);
#endif

    try {
        (*clientProxy)->GetDescriptionOfFunction(componentName, requiredInterfaceName, functionName, description);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyServer, "SendGetDescriptionOfFunction: network exception: " << ex);
        OnClientDisconnect(clientID);
        return;
    }
}

void mtsManagerProxyServer::SendGetDescriptionOfEventHandler(std::string & description, const std::string & componentName,
    const std::string & requiredInterfaceName, const std::string & eventHandlerName, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy || !clientProxy->get()) {
        LogError(mtsManagerProxyServer, "SendGetDescriptionOfEventHandler: invalid listenerID (" << clientID << ") or inactive server proxy");
        return;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendGetDescriptionOfEventHandler: " << componentName << ", " << requiredInterfaceName << ", " << eventHandlerName << ", " << clientID);
#endif

    try {
        (*clientProxy)->GetDescriptionOfEventHandler(componentName, requiredInterfaceName, eventHandlerName, description);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyServer, "SendGetDescriptionOfEventHandler: network exception: " << ex);
        OnClientDisconnect(clientID);
        return;
    }
}

std::string mtsManagerProxyServer::SendGetProcessName(const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy || !clientProxy->get()) {
        LogError(mtsManagerProxyServer, "SendGetProcessName: invalid listenerID (" << clientID << ") or inactive server proxy");
        return "";
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendGetProcessName: " << clientID);
#endif

    try {
        return (*clientProxy)->GetProcessName();
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyServer, "SendGetProcessName: network exception: " << ex);
        OnClientDisconnect(clientID);
        return "";
    }
}

//-------------------------------------------------------------------------
//  Definition by mtsComponentInterfaceProxy.ice
//-------------------------------------------------------------------------
mtsManagerProxyServer::ManagerServerI::ManagerServerI(
    const Ice::CommunicatorPtr& communicator, const Ice::LoggerPtr& logger,
    mtsManagerProxyServer * managerProxyServer)
    : Communicator(communicator),
      SenderThreadPtr(new SenderThread<ManagerServerIPtr>(this)),
      IceLogger(logger),
      ManagerProxyServer(managerProxyServer)
{
}

mtsManagerProxyServer::ManagerServerI::~ManagerServerI()
{
    Stop();
}

void mtsManagerProxyServer::ManagerServerI::Start()
{
    ManagerProxyServer->GetLogger()->trace("mtsManagerProxyServer", "Send thread starts");

    SenderThreadPtr->start();
}

//#define _COMMUNICATION_TEST_
void mtsManagerProxyServer::ManagerServerI::Run()
{
#ifdef _COMMUNICATION_TEST_
    int count = 0;

    while (IsActiveProxy()) {
        osaSleep(1 * cmn_s);
        std::cout << "\tServer [" << ManagerProxyServer->GetProxyName() << "] running (" << ++count << ")" << std::endl;

        std::stringstream ss;
        ss << "Msg " << count << " from Server";

        ManagerProxyServer->SendTestMessageFromServerToClient(ss.str());
    }
#else
    double lastTickChecked = 0.0, now;
    while (IsActiveProxy()) {
        now = osaGetTime();
        if (now < lastTickChecked + mtsProxyConfig::CheckPeriodForManagerConnections) {
            osaSleep(10 * cmn_ms);
            continue;
        }
        lastTickChecked = now;

        // If a pending connection fails to be confirmed by LCM, it should be
        // cleaned up
        if (ManagerProxyServer) {
            ManagerProxyServer->CheckConnectConfirmTimeout();
        }

        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
        try {
            ManagerProxyServer->MonitorConnections();
        } catch (const Ice::Exception & ex) {
            LogError(mtsManagerProxyServer::ManagerServerI, "Process (LCM) disconnection detected: " << ex.what());
        }
    }
#endif
}

void mtsManagerProxyServer::ManagerServerI::Stop()
{
    if (!IsActiveProxy()) return;

    ManagerProxyServer = 0;

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

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer::ManagerServerI,
        "Stopped and destroyed callback thread to communicate with clients");
#endif
}

 bool mtsManagerProxyServer::ManagerServerI::IsActiveProxy() const
 {
    if (ManagerProxyServer) {
        return ManagerProxyServer->IsActiveProxy();
    } else {
        return false;
    }
}

//-----------------------------------------------------------------------------
//  Network Event Handlers
//-----------------------------------------------------------------------------
void mtsManagerProxyServer::ManagerServerI::TestMessageFromClientToServer(
    const std::string & str, const ::Ice::Current & current)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: TestMessageFromClientToServer");
#endif

    const IceConnectionIDType iceConnectionID = current.ctx.find(mtsManagerProxyServer::GetConnectionIDKey())->second;

    ManagerProxyServer->ReceiveTestMessageFromClientToServer(iceConnectionID, str);
}

bool mtsManagerProxyServer::ManagerServerI::AddClient(
    const std::string & processName, const ::Ice::Identity & identity, const ::Ice::Current& current)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
   LogPrint(ManagerServerI, "<<<<< RECV: AddClient: " << processName << " (" << Communicator->identityToString(identity) << ")");
#endif

    const IceConnectionIDType iceConnectionID = current.ctx.find(mtsManagerProxyServer::GetConnectionIDKey())->second;

    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);

    ManagerClientProxyType clientProxy =
        ManagerClientProxyType::uncheckedCast(current.con->createProxy(identity));

    return ManagerProxyServer->ReceiveAddClient(iceConnectionID, processName, clientProxy);
}

void mtsManagerProxyServer::ManagerServerI::Refresh(const ::Ice::Current& current)
{
    const IceConnectionIDType iceConnectionID = current.ctx.find(mtsManagerProxyServer::GetConnectionIDKey())->second;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: Refresh: " << iceConnectionID);
#endif

    // MJ: could refresh session here
}

void mtsManagerProxyServer::ManagerServerI::Shutdown(const ::Ice::Current& current)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: Shutdown");
#endif

    const IceConnectionIDType iceConnectionID = current.ctx.find(mtsManagerProxyServer::GetConnectionIDKey())->second;

    // MJ: could have shutdown like ComponentInterfaceProxyServer->ShutdownSession(current);
}

bool mtsManagerProxyServer::ManagerServerI::AddProcess(const std::string & processName, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: AddProcess: " << processName);
#endif

    return ManagerProxyServer->ReceiveAddProcess(processName);
}

bool mtsManagerProxyServer::ManagerServerI::FindProcess(const std::string & processName, const ::Ice::Current & CMN_UNUSED(current)) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: FindProcess: " << processName);
#endif

    return ManagerProxyServer->ReceiveFindProcess(processName);
}

bool mtsManagerProxyServer::ManagerServerI::RemoveProcess(const std::string & processName, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: RemoveProcess: " << processName);
#endif

    return ManagerProxyServer->ReceiveRemoveProcess(processName);
}

bool mtsManagerProxyServer::ManagerServerI::AddComponent(const std::string & processName, const std::string & componentName, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: AddComponent: " << processName << ", " << componentName);
#endif

    return ManagerProxyServer->ReceiveAddComponent(processName, componentName);
}

bool mtsManagerProxyServer::ManagerServerI::FindComponent(const std::string & processName, const std::string & componentName, const ::Ice::Current & CMN_UNUSED(current)) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: FindComponent: " << processName << ", " << componentName);
#endif

    return ManagerProxyServer->ReceiveFindComponent(processName, componentName);
}

bool mtsManagerProxyServer::ManagerServerI::RemoveComponent(const std::string & processName, const std::string & componentName, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: RemoveComponent: " << processName << ", " << componentName);
#endif

    return ManagerProxyServer->ReceiveRemoveComponent(processName, componentName);
}

bool mtsManagerProxyServer::ManagerServerI::AddInterfaceProvidedOrOutput(
    const std::string & processName, const std::string & componentName,
    const std::string & interfaceName, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: AddInterfaceProvidedOrOutput: " << processName << ", " << componentName << ", " << interfaceName);
#endif

    return ManagerProxyServer->ReceiveAddInterfaceProvidedOrOutput(processName, componentName, interfaceName);
}

bool mtsManagerProxyServer::ManagerServerI::FindInterfaceProvidedOrOutput(
    const std::string & processName, const std::string & componentName,
    const std::string & interfaceName, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: FindInterfaceProvidedOrOutput: " << processName << ", " << componentName << ", " << interfaceName);
#endif

    return ManagerProxyServer->ReceiveFindInterfaceProvidedOrOutput(processName, componentName, interfaceName);
}

bool mtsManagerProxyServer::ManagerServerI::RemoveInterfaceProvidedOrOutput(
    const std::string & processName, const std::string & componentName,
    const std::string & interfaceName, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: RemoveInterfaceProvidedOrOutput: " << processName << ", " << componentName << ", " << interfaceName);
#endif

    return ManagerProxyServer->ReceiveRemoveInterfaceProvidedOrOutput(processName, componentName, interfaceName);
}

bool mtsManagerProxyServer::ManagerServerI::AddInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName, const std::string & interfaceName, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: AddInterfaceRequiredOrInput: " << processName << ", " << componentName << ", " << interfaceName);
#endif

    return ManagerProxyServer->ReceiveAddInterfaceRequiredOrInput(processName, componentName, interfaceName);
}

bool mtsManagerProxyServer::ManagerServerI::FindInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName, const std::string & interfaceName, const ::Ice::Current &) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: FindInterfaceRequiredOrInput: " << processName << ", " << componentName << ", " << interfaceName);
#endif

    return ManagerProxyServer->ReceiveFindInterfaceRequiredOrInput(processName, componentName, interfaceName);
}

bool mtsManagerProxyServer::ManagerServerI::RemoveInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName, const std::string & interfaceName, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: RemoveInterfaceRequiredOrInput: " << processName << ", " << componentName << ", " << interfaceName);
#endif

    return ManagerProxyServer->ReceiveRemoveInterfaceRequiredOrInput(processName, componentName, interfaceName);
}

::Ice::Int mtsManagerProxyServer::ManagerServerI::Connect(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: Connect: "
        << connectionStringSet.ClientProcessName << ":" << connectionStringSet.ClientComponentName << ":" << connectionStringSet.ClientInterfaceName
        << "-"
        << connectionStringSet.ServerProcessName << ":" << connectionStringSet.ServerComponentName << ":" << connectionStringSet.ServerInterfaceName);
#endif

    return ManagerProxyServer->ReceiveConnect(connectionStringSet);
}

bool mtsManagerProxyServer::ManagerServerI::ConnectConfirm(::Ice::Int connectionID, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: ConnectConfirm: " << connectionID);
#endif

    return ManagerProxyServer->ReceiveConnectConfirm(connectionID);
}

bool mtsManagerProxyServer::ManagerServerI::DisconnectWithID(::Ice::Int connectionID, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: DisconnectWithID: " << connectionID);
#endif

    return ManagerProxyServer->ReceiveDisconnectWithID(connectionID);
}

bool mtsManagerProxyServer::ManagerServerI::Disconnect(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: Disconnect: "
        << connectionStringSet.ClientProcessName << ":" << connectionStringSet.ClientComponentName << ":" << connectionStringSet.ClientInterfaceName
        << "-"
        << connectionStringSet.ServerProcessName << ":" << connectionStringSet.ServerComponentName << ":" << connectionStringSet.ServerInterfaceName);
#endif

    return ManagerProxyServer->ReceiveDisconnect(connectionStringSet);
}

bool mtsManagerProxyServer::ManagerServerI::SetInterfaceProvidedProxyAccessInfo(::Ice::Int connectionID, const std::string & endpointInfo, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: SetInterfaceProvidedProxyAccessInfo: " << connectionID << " - " << endpointInfo);
#endif

    return ManagerProxyServer->ReceiveSetInterfaceProvidedProxyAccessInfo(connectionID, endpointInfo);
}

bool mtsManagerProxyServer::ManagerServerI::GetInterfaceProvidedProxyAccessInfoWithID(::Ice::Int connectionID, std::string & endpointInfo, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: GetInterfaceProvidedProxyAccessInfo: " << connectionID);
#endif

    return ManagerProxyServer->ReceiveGetInterfaceProvidedProxyAccessInfo(connectionID, endpointInfo);
}

bool mtsManagerProxyServer::ManagerServerI::GetInterfaceProvidedProxyAccessInfo(const std::string & clientProcessName,
    const std::string & serverProcessName, const std::string & serverComponentName,
    const std::string & serverInterfaceName, std::string & endpointInfo,
    const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: GetInterfaceProvidedProxyAccessInfo: " << clientProcessName << ", "
        << mtsManagerGlobal::GetInterfaceUID(serverProcessName, serverComponentName, serverInterfaceName));
#endif

    return ManagerProxyServer->ReceiveGetInterfaceProvidedProxyAccessInfo(clientProcessName, serverProcessName, serverComponentName, serverInterfaceName, endpointInfo);
}

bool mtsManagerProxyServer::ManagerServerI::InitiateConnect(::Ice::Int connectionID, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: InitiateConnect: " << connectionID);

#endif

    return ManagerProxyServer->ReceiveInitiateConnect(connectionID);
}

bool mtsManagerProxyServer::ManagerServerI::ConnectServerSideInterfaceRequest(::Ice::Int connectionID, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: ConnectServerSideInterfaceRequest: " << connectionID);
#endif

    return ManagerProxyServer->ReceiveConnectServerSideInterfaceRequest(connectionID);
}
