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

#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsManagerProxyServer.h>
#include <cisstMultiTask/mtsManagerGlobal.h>

std::string mtsManagerProxyServer::ManagerCommunicatorID = "ManagerServerCommunicator";
std::string mtsManagerProxyServer::ConnectionIDKey = "ManagerConnectionID";
unsigned int mtsManagerProxyServer::InstanceCounter = 0;

mtsManagerProxyServer::mtsManagerProxyServer(const std::string & adapterName, const std::string & communicatorID)
    : BaseServerType(ICE_PROPERTY_FILE_ROOT"config.GCM", adapterName, communicatorID, false)
{
    ProxyName = "ManagerProxyServer";
}

mtsManagerProxyServer::~mtsManagerProxyServer()
{
    Stop();
}

std::string mtsManagerProxyServer::GetGCMPortNumberAsString()
{
    Ice::InitializationData initData;
    initData.properties = Ice::createProperties();
    initData.properties->load(ICE_PROPERTY_FILE_ROOT"config.GCM");

    return initData.properties->getProperty("GCM.Port");
}

int mtsManagerProxyServer::GetGCMConnectTimeout()
{
    Ice::InitializationData initData;
    initData.properties = Ice::createProperties();
    initData.properties->load(ICE_PROPERTY_FILE_ROOT"config.GCM");

    const std::string connectTimeoutString =
        initData.properties->getProperty("Ice.Override.ConnectTimeout");

    return atoi(connectTimeoutString.c_str());
}

//-----------------------------------------------------------------------------
//  Proxy Start-up
//-----------------------------------------------------------------------------
bool mtsManagerProxyServer::Start(mtsManagerGlobal * proxyOwner)
{
    // Initialize Ice object.
    IceInitialize();

    if (!InitSuccessFlag) {
        LogError(mtsManagerProxyServer, "ICE proxy Initialization failed");
        return false;
    }

    // Register this proxy to the owner
    if (!proxyOwner->AddProcessObject(this, true)) {
        LogError(mtsManagerProxyServer, "failed to register proxy server to the global component manager");
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

    // Create worker thread. Note that it is created but is not yet running.
    WorkerThread.Create<ProxyWorker<mtsManagerGlobal>, ThreadArguments<mtsManagerGlobal>*>(
        &ProxyWorkerInfo, &ProxyWorker<mtsManagerGlobal>::Run, &ThreadArgumentsInfo, threadName.c_str());

    return true;
}

void mtsManagerProxyServer::StartServer()
{
    Sender->Start();

    // This is a blocking call that should be run in a different thread.
    IceCommunicator->waitForShutdown();
}

void mtsManagerProxyServer::Runner(ThreadArguments<mtsManagerGlobal> * arguments)
{
    mtsManagerProxyServer * ProxyServer =
        dynamic_cast<mtsManagerProxyServer*>(arguments->Proxy);
    if (!ProxyServer) {
        CMN_LOG_RUN_ERROR << "mtsManagerProxyServer: Failed to create a proxy server." << std::endl;
        return;
    }

    ProxyServer->GetLogger()->trace("mtsManagerProxyServer", "Proxy server starts.....");

    try {
        ProxyServer->SetAsActiveProxy();
        ProxyServer->StartServer();
    } catch (const Ice::Exception& e) {
        std::string error("mtsManagerProxyServer: ");
        error += e.what();
        ProxyServer->GetLogger()->error(error);
    } catch (const char * msg) {
        std::string error("mtsManagerProxyServer: ");
        error += msg;
        ProxyServer->GetLogger()->error(error);
    }

    ProxyServer->GetLogger()->trace("mtsManagerProxyServer", "Proxy server terminates.....");

    ProxyServer->Stop();
}

void mtsManagerProxyServer::Stop()
{
    LogPrint(mtsManagerProxyServer, "ManagerProxy server stops.");

    BaseServerType::Stop();

    Sender->Stop();
}

void mtsManagerProxyServer::ConnectCheckTimeout()
{
    ProxyOwner->ConnectCheckTimeout();
}

bool mtsManagerProxyServer::OnClientDisconnect(const ClientIDType clientID)
{
    if (!IsActiveProxy()) return true;

    // Get network proxy client serving the client with the clientID
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "OnClientDisconnect: no client proxy found with client id: " << clientID);
        return false;
    }

    // Remove client from client list. This prevents further network processing
    // requests from being executed.
    if (!BaseServerType::RemoveClientByClientID(clientID)) {
        LogError(mtsManagerProxyServer, "OnClientDisconnect: failed to remove client from client map: " << clientID);
        return false;
    }

    // Remove the process logically
    if (!ProxyOwner->RemoveProcess(clientID)) {
        LogError(mtsManagerProxyServer, "OnClientDisconnect: failed to remove process: " << clientID);
        return false;
    }

    LogPrint(mtsManagerProxyServer, "OnClientDisconnect: successfully removed process: " << clientID);

    return true;
}

mtsManagerProxyServer::ManagerClientProxyType * mtsManagerProxyServer::GetNetworkProxyClient(const ClientIDType clientID)
{
    ManagerClientProxyType * clientProxy = GetClientByClientID(clientID);
    if (!clientProxy) {
        return NULL;
    }

    // Check if this network proxy server is active. We don't need to check if
    // a proxy client is still active since any disconnection or inactive proxy
    // has already been detected and taken care of.
    return (IsActiveProxy() ? clientProxy : NULL);
}

void mtsManagerProxyServer::ConstructConnectionStringSet(
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName,
    ::mtsManagerProxy::ConnectionStringSet & connectionStringSet)
{
    connectionStringSet.ClientProcessName = clientProcessName;
    connectionStringSet.ClientComponentName = clientComponentName;
    connectionStringSet.ClientRequiredInterfaceName = clientRequiredInterfaceName;
    connectionStringSet.ServerProcessName = serverProcessName;
    connectionStringSet.ServerComponentName = serverComponentName;
    connectionStringSet.ServerProvidedInterfaceName = serverProvidedInterfaceName;
}

void mtsManagerProxyServer::ConvertProvidedInterfaceDescription(
    const ::mtsManagerProxy::ProvidedInterfaceDescription & src, ProvidedInterfaceDescription & dest)
{
    // Initialize destination structure
    dest.CommandsVoid.clear();
    dest.CommandsWrite.clear();
    dest.CommandsRead.clear();
    dest.CommandsQualifiedRead.clear();
    dest.EventsVoid.clear();
    dest.EventsWrite.clear();

    // Conversion of provided interface name
    dest.ProvidedInterfaceName = src.ProvidedInterfaceName;

    // Conversion of command void vector
    mtsInterfaceCommon::CommandVoidElement commandVoid;
    ::mtsManagerProxy::CommandVoidSequence::const_iterator itVoid = src.CommandsVoid.begin();
    const ::mtsManagerProxy::CommandVoidSequence::const_iterator itVoidEnd = src.CommandsVoid.end();
    for (; itVoid != itVoidEnd; ++itVoid) {
        commandVoid.Name = itVoid->Name;
        dest.CommandsVoid.push_back(commandVoid);
    }

    // Conversion of command write vector
    mtsInterfaceCommon::CommandWriteElement commandWrite;
    ::mtsManagerProxy::CommandWriteSequence::const_iterator itWrite = src.CommandsWrite.begin();
    const ::mtsManagerProxy::CommandWriteSequence::const_iterator itWriteEnd = src.CommandsWrite.end();
    for (; itWrite != itWriteEnd; ++itWrite) {
        commandWrite.Name = itWrite->Name;
        commandWrite.ArgumentPrototypeSerialized = itWrite->ArgumentPrototypeSerialized;
        dest.CommandsWrite.push_back(commandWrite);
    }

    // Conversion of command read vector
    mtsInterfaceCommon::CommandReadElement commandRead;
    ::mtsManagerProxy::CommandReadSequence::const_iterator itRead = src.CommandsRead.begin();
    const ::mtsManagerProxy::CommandReadSequence::const_iterator itReadEnd = src.CommandsRead.end();
    for (; itRead != itReadEnd; ++itRead) {
        commandRead.Name = itRead->Name;
        commandRead.ArgumentPrototypeSerialized = itRead->ArgumentPrototypeSerialized;
        dest.CommandsRead.push_back(commandRead);
    }

    // Conversion of command qualified read vector
    mtsInterfaceCommon::CommandQualifiedReadElement commandQualifiedRead;
    ::mtsManagerProxy::CommandQualifiedReadSequence::const_iterator itQualifiedRead = src.CommandsQualifiedRead.begin();
    const ::mtsManagerProxy::CommandQualifiedReadSequence::const_iterator itQualifiedReadEnd = src.CommandsQualifiedRead.end();
    for (; itQualifiedRead != itQualifiedReadEnd; ++itQualifiedRead) {
        commandQualifiedRead.Name = itQualifiedRead->Name;
        commandQualifiedRead.Argument1PrototypeSerialized = itQualifiedRead->Argument1PrototypeSerialized;
        commandQualifiedRead.Argument2PrototypeSerialized = itQualifiedRead->Argument2PrototypeSerialized;
        dest.CommandsQualifiedRead.push_back(commandQualifiedRead);
    }

    // Conversion of event void generator vector
    mtsInterfaceCommon::EventVoidElement eventVoid;
    ::mtsManagerProxy::EventVoidSequence::const_iterator itEventVoid = src.EventsVoid.begin();
    const ::mtsManagerProxy::EventVoidSequence::const_iterator itEventVoidEnd = src.EventsVoid.end();
    for (; itEventVoid != itEventVoidEnd; ++itEventVoid) {
        eventVoid.Name = itEventVoid->Name;
        dest.EventsVoid.push_back(eventVoid);
    }

    // Conversion of event write generator vector
    mtsInterfaceCommon::EventWriteElement eventWrite;
    ::mtsManagerProxy::EventWriteSequence::const_iterator itEventWrite = src.EventsWrite.begin();
    const ::mtsManagerProxy::EventWriteSequence::const_iterator itEventWriteEnd = src.EventsWrite.end();
    for (; itEventWrite != itEventWriteEnd; ++itEventWrite) {
        eventWrite.Name = itEventWrite->Name;
        eventWrite.ArgumentPrototypeSerialized = itEventWrite->ArgumentPrototypeSerialized;
        dest.EventsWrite.push_back(eventWrite);
    }
}

void mtsManagerProxyServer::ConvertRequiredInterfaceDescription(
    const ::mtsManagerProxy::RequiredInterfaceDescription & src, RequiredInterfaceDescription & dest)
{
    // Initialize destination structure
    dest.FunctionVoidNames.clear();
    dest.FunctionWriteNames.clear();
    dest.FunctionReadNames.clear();
    dest.FunctionQualifiedReadNames.clear();
    dest.EventHandlersVoid.clear();
    dest.EventHandlersWrite.clear();

    // Conversion of required interface name
    dest.RequiredInterfaceName = src.RequiredInterfaceName;

    // Conversion of function void vector
    dest.FunctionVoidNames.insert(dest.FunctionVoidNames.begin(), src.FunctionVoidNames.begin(), src.FunctionVoidNames.end());

    // Conversion of function write vector
    dest.FunctionWriteNames.insert(dest.FunctionWriteNames.begin(), src.FunctionWriteNames.begin(), src.FunctionWriteNames.end());

    // Conversion of function read vector
    dest.FunctionReadNames.insert(dest.FunctionReadNames.begin(), src.FunctionReadNames.begin(), src.FunctionReadNames.end());

    // Conversion of function qualified read vector
    dest.FunctionQualifiedReadNames.insert(dest.FunctionQualifiedReadNames.begin(), src.FunctionQualifiedReadNames.begin(), src.FunctionQualifiedReadNames.end());

    // Conversion of event void handler vector
    mtsInterfaceCommon::CommandVoidElement eventVoidHandler;
    ::mtsManagerProxy::CommandVoidSequence::const_iterator itEventVoid = src.EventHandlersVoid.begin();
    const ::mtsManagerProxy::CommandVoidSequence::const_iterator itEventVoidEnd = src.EventHandlersVoid.end();
    for (; itEventVoid != itEventVoidEnd; ++itEventVoid) {
        eventVoidHandler.Name = itEventVoid->Name;
        dest.EventHandlersVoid.push_back(eventVoidHandler);
    }

    // Conversion of event write handler vector
    mtsInterfaceCommon::CommandWriteElement eventWriteHandler;
    ::mtsManagerProxy::CommandWriteSequence::const_iterator itEventWrite = src.EventHandlersWrite.begin();
    const ::mtsManagerProxy::CommandWriteSequence::const_iterator itEventWriteEnd = src.EventHandlersWrite.end();
    for (; itEventWrite != itEventWriteEnd; ++itEventWrite) {
        eventWriteHandler.Name = itEventWrite->Name;
        eventWriteHandler.ArgumentPrototypeSerialized = itEventWrite->ArgumentPrototypeSerialized;
        dest.EventHandlersWrite.push_back(eventWriteHandler);
    }
}

void mtsManagerProxyServer::ConvertValuesOfCommand(
    const ::mtsManagerProxy::SetOfValues & src, mtsManagerLocalInterface::SetOfValues & dest)
{
    ValuePair value;
    Values valueSet;

    for (unsigned int i = 0; i < src.size(); ++i) {
        valueSet.clear();
        for (unsigned int j = 0; j < src[i].size(); ++j) {
            value.Timestamp.sec = static_cast<long>(src[i][j].Timestamp.sec);
            value.Timestamp.nsec = static_cast<long>(src[i][j].Timestamp.nsec);
            value.Value = src[i][j].Value;
            valueSet.push_back(value);
        }
        dest.push_back(valueSet);
    }
}

void mtsManagerProxyServer::ConstructValuesOfCommand(
    const mtsManagerLocalInterface::SetOfValues & src, ::mtsManagerProxy::SetOfValues & dest)
{
    ::mtsManagerProxy::ValuePair value;
    ::mtsManagerProxy::Values valueSet;

    for (unsigned int i = 0; i < src.size(); ++i) {
        valueSet.clear();
        for (unsigned int j = 0; j < src[i].size(); ++j) {
            value.Timestamp.sec = src[i][j].Timestamp.sec;
            value.Timestamp.nsec = src[i][j].Timestamp.nsec;
            value.Value = src[i][j].Value;
            valueSet.push_back(value);
        }
        dest.push_back(valueSet);
    }
}

void mtsManagerProxyServer::ConstructProvidedInterfaceDescriptionFrom(
    const ProvidedInterfaceDescription & src, ::mtsManagerProxy::ProvidedInterfaceDescription & dest)
{
    // Initialize destination structure
    dest.CommandsVoid.clear();
    dest.CommandsWrite.clear();
    dest.CommandsRead.clear();
    dest.CommandsQualifiedRead.clear();
    dest.EventsVoid.clear();
    dest.EventsWrite.clear();

    // Construct provided interface name
    dest.ProvidedInterfaceName = src.ProvidedInterfaceName;

    // Construct command void vector
    ::mtsManagerProxy::CommandVoidElement commandVoid;
    CommandVoidVector::const_iterator itVoid = src.CommandsVoid.begin();
    const CommandVoidVector::const_iterator itVoidEnd = src.CommandsVoid.end();
    for (; itVoid != itVoidEnd; ++itVoid) {
        commandVoid.Name = itVoid->Name;
        dest.CommandsVoid.push_back(commandVoid);
    }

    // Construct command write vector
    ::mtsManagerProxy::CommandWriteElement commandWrite;
    CommandWriteVector::const_iterator itWrite = src.CommandsWrite.begin();
    const CommandWriteVector::const_iterator itWriteEnd = src.CommandsWrite.end();
    for (; itWrite != itWriteEnd; ++itWrite) {
        commandWrite.Name = itWrite->Name;
        commandWrite.ArgumentPrototypeSerialized = itWrite->ArgumentPrototypeSerialized;
        dest.CommandsWrite.push_back(commandWrite);
    }

    // Construct command read vector
    ::mtsManagerProxy::CommandReadElement commandRead;
    CommandReadVector::const_iterator itRead = src.CommandsRead.begin();
    const CommandReadVector::const_iterator itReadEnd = src.CommandsRead.end();
    for (; itRead != itReadEnd; ++itRead) {
        commandRead.Name = itRead->Name;
        commandRead.ArgumentPrototypeSerialized = itRead->ArgumentPrototypeSerialized;
        dest.CommandsRead.push_back(commandRead);
    }

    // Construct command QualifiedRead vector
    ::mtsManagerProxy::CommandQualifiedReadElement commandQualifiedRead;
    CommandQualifiedReadVector::const_iterator itQualifiedRead = src.CommandsQualifiedRead.begin();
    const CommandQualifiedReadVector::const_iterator itQualifiedReadEnd = src.CommandsQualifiedRead.end();
    for (; itQualifiedRead != itQualifiedReadEnd; ++itQualifiedRead) {
        commandQualifiedRead.Name = itQualifiedRead->Name;
        commandQualifiedRead.Argument1PrototypeSerialized = itQualifiedRead->Argument1PrototypeSerialized;
        commandQualifiedRead.Argument2PrototypeSerialized = itQualifiedRead->Argument2PrototypeSerialized;
        dest.CommandsQualifiedRead.push_back(commandQualifiedRead);
    }

    // Construct event void generator vector
    ::mtsManagerProxy::EventVoidElement eventVoidGenerator;
    EventVoidVector::const_iterator itEventVoid = src.EventsVoid.begin();
    const EventVoidVector::const_iterator itEventVoidEnd = src.EventsVoid.end();
    for (; itEventVoid != itEventVoidEnd; ++itEventVoid) {
        eventVoidGenerator.Name = itEventVoid->Name;
        dest.EventsVoid.push_back(eventVoidGenerator);
    }

    // Construct event write generator vector
    ::mtsManagerProxy::EventWriteElement eventWriteGenerator;
    EventWriteVector::const_iterator itEventWrite = src.EventsWrite.begin();
    const EventWriteVector::const_iterator itEventWriteEnd = src.EventsWrite.end();
    for (; itEventWrite != itEventWriteEnd; ++itEventWrite) {
        eventWriteGenerator.Name = itEventWrite->Name;
        eventWriteGenerator.ArgumentPrototypeSerialized = itEventWrite->ArgumentPrototypeSerialized;
        dest.EventsWrite.push_back(eventWriteGenerator);
    }
}

void mtsManagerProxyServer::ConstructRequiredInterfaceDescriptionFrom(
    const RequiredInterfaceDescription & src, ::mtsManagerProxy::RequiredInterfaceDescription & dest)
{
    // Initialize destination structure
    dest.FunctionVoidNames.clear();
    dest.FunctionWriteNames.clear();
    dest.FunctionReadNames.clear();
    dest.FunctionQualifiedReadNames.clear();
    dest.EventHandlersVoid.clear();
    dest.EventHandlersWrite.clear();

    // Construct required interface name
    dest.RequiredInterfaceName = src.RequiredInterfaceName;

    // Construct function void vector
    dest.FunctionVoidNames.insert(dest.FunctionVoidNames.begin(), src.FunctionVoidNames.begin(), src.FunctionVoidNames.end());

    // Construct function write vector
    dest.FunctionWriteNames.insert(dest.FunctionWriteNames.begin(), src.FunctionWriteNames.begin(), src.FunctionWriteNames.end());

    // Construct function read vector
    dest.FunctionReadNames.insert(dest.FunctionReadNames.begin(), src.FunctionReadNames.begin(), src.FunctionReadNames.end());

    // Construct function qualified read vector
    dest.FunctionQualifiedReadNames.insert(dest.FunctionQualifiedReadNames.begin(), src.FunctionQualifiedReadNames.begin(), src.FunctionQualifiedReadNames.end());

    // Construct event void handler vector
    ::mtsManagerProxy::CommandVoidElement eventVoidHandler;
    mtsInterfaceCommon::CommandVoidVector::const_iterator itEventVoid = src.EventHandlersVoid.begin();
    const mtsInterfaceCommon::CommandVoidVector::const_iterator itEventVoidEnd = src.EventHandlersVoid.end();
    for (; itEventVoid != itEventVoidEnd; ++itEventVoid) {
        eventVoidHandler.Name = itEventVoid->Name;
        dest.EventHandlersVoid.push_back(eventVoidHandler);
    }

    // Construct event write handler vector
    ::mtsManagerProxy::CommandWriteElement eventWriteHandler;
    mtsInterfaceCommon::CommandWriteVector::const_iterator itEventWrite = src.EventHandlersWrite.begin();
    const mtsInterfaceCommon::CommandWriteVector::const_iterator itEventWriteEnd = src.EventHandlersWrite.end();
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

bool mtsManagerProxyServer::CreateProvidedInterfaceProxy(const std::string & serverComponentProxyName,
    const ProvidedInterfaceDescription & providedInterfaceDescription, const std::string & listenerID)
{
    // Convert providedInterfaceDescription to an object of type mtsManagerProxy::ProvidedInterfaceDescription
    mtsManagerProxy::ProvidedInterfaceDescription interfaceDescription;
    mtsManagerProxyServer::ConstructProvidedInterfaceDescriptionFrom(providedInterfaceDescription, interfaceDescription);

    return SendCreateProvidedInterfaceProxy(serverComponentProxyName, interfaceDescription, listenerID);
}

bool mtsManagerProxyServer::CreateRequiredInterfaceProxy(const std::string & clientComponentProxyName,
    const RequiredInterfaceDescription & requiredInterfaceDescription, const std::string & listenerID)
{
    // Convert requiredInterfaceDescription to an object of type mtsManagerProxy::RequiredInterfaceDescription
    mtsManagerProxy::RequiredInterfaceDescription interfaceDescription;
    mtsManagerProxyServer::ConstructRequiredInterfaceDescriptionFrom(requiredInterfaceDescription, interfaceDescription);

    return SendCreateRequiredInterfaceProxy(clientComponentProxyName, interfaceDescription, listenerID);
}

bool mtsManagerProxyServer::RemoveProvidedInterfaceProxy(
    const std::string & clientComponentProxyName, const std::string & providedInterfaceProxyName, const std::string & listenerID)
{
    return SendRemoveProvidedInterfaceProxy(clientComponentProxyName, providedInterfaceProxyName, listenerID);
}

bool mtsManagerProxyServer::RemoveRequiredInterfaceProxy(
    const std::string & serverComponentProxyName, const std::string & requiredInterfaceProxyName, const std::string & listenerID)
{
    return SendRemoveRequiredInterfaceProxy(serverComponentProxyName, requiredInterfaceProxyName, listenerID);
}

bool mtsManagerProxyServer::ConnectServerSideInterface(
    const int userId, const unsigned int providedInterfaceProxyInstanceID,
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName, const std::string & listenerID)
{
    // Create an instance of mtsManagerProxy::ConnectionStringSet out of a set of strings given
    ::mtsManagerProxy::ConnectionStringSet connectionStringSet;
    ConstructConnectionStringSet(
        clientProcessName, clientComponentName, clientRequiredInterfaceName,
        serverProcessName, serverComponentName, serverProvidedInterfaceName,
        connectionStringSet);

    return SendConnectServerSideInterface(userId, providedInterfaceProxyInstanceID, connectionStringSet, listenerID);
}

bool mtsManagerProxyServer::ConnectClientSideInterface(const unsigned int connectionID,
    const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName, const std::string & listenerID)
{
    // Create an instance of mtsManagerProxy::ConnectionStringSet out of a set of strings given
    ::mtsManagerProxy::ConnectionStringSet connectionStringSet;
    ConstructConnectionStringSet(
        clientProcessName, clientComponentName, clientRequiredInterfaceName,
        serverProcessName, serverComponentName, serverProvidedInterfaceName,
        connectionStringSet);

    return SendConnectClientSideInterface(connectionID, connectionStringSet, listenerID);
}

int mtsManagerProxyServer::PreAllocateResources(const std::string & userName,
    const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName, const std::string & listenerID)
{
    return SendPreAllocateResources(userName, serverProcessName, serverComponentName, serverProvidedInterfaceName, listenerID);
}

bool mtsManagerProxyServer::GetProvidedInterfaceDescription(
    const unsigned int userId, const std::string & serverComponentName, const std::string & providedInterfaceName,
    ProvidedInterfaceDescription & providedInterfaceDescription, const std::string & listenerID)
{
    mtsManagerProxy::ProvidedInterfaceDescription src;

    if (!SendGetProvidedInterfaceDescription(userId, serverComponentName, providedInterfaceName, src, listenerID)) {
        LogError(mtsManagerProxyServer, "GetProvidedInterfaceDescription() failed");
        return false;
    }

    // Convert mtsManagerProxy::ProvidedInterfaceDescription to mtsInterfaceCommon::ProvidedInterfaceDescription
    mtsManagerProxyServer::ConvertProvidedInterfaceDescription(src, providedInterfaceDescription);

    return true;
}

bool mtsManagerProxyServer::GetRequiredInterfaceDescription(
    const std::string & componentName, const std::string & requiredInterfaceName,
    RequiredInterfaceDescription & requiredInterfaceDescription, const std::string & listenerID)
{
    mtsManagerProxy::RequiredInterfaceDescription src;

    if (!SendGetRequiredInterfaceDescription(componentName, requiredInterfaceName, src, listenerID)) {
        LogError(mtsManagerProxyServer, "GetRequiredInterfaceDescription() failed");
        return false;
    }

    // Construct an instance of type RequiredInterfaceDescription from an object of type mtsManagerProxy::RequiredInterfaceDescription
    mtsManagerProxyServer::ConvertRequiredInterfaceDescription(src, requiredInterfaceDescription);

    return true;
}

const std::string mtsManagerProxyServer::GetProcessName(const std::string & listenerID)
{
    return SendGetProcessName(listenerID);
}

void mtsManagerProxyServer::GetNamesOfCommands(std::vector<std::string>& namesOfCommands,
    const std::string & componentName, const std::string & providedInterfaceName, const std::string & listenerID)
{
    SendGetNamesOfCommands(namesOfCommands, componentName, providedInterfaceName, listenerID);
}

void mtsManagerProxyServer::GetNamesOfEventGenerators(std::vector<std::string>& namesOfEventGenerators,
    const std::string & componentName, const std::string & providedInterfaceName, const std::string & listenerID)
{
    SendGetNamesOfEventGenerators(namesOfEventGenerators, componentName, providedInterfaceName, listenerID);
}

void mtsManagerProxyServer::GetNamesOfFunctions(std::vector<std::string>& namesOfFunctions,
    const std::string & componentName, const std::string & requiredInterfaceName, const std::string & listenerID)
{
    SendGetNamesOfFunctions(namesOfFunctions, componentName, requiredInterfaceName, listenerID);
}

void mtsManagerProxyServer::GetNamesOfEventHandlers(std::vector<std::string>& namesOfEventHandlers,
    const std::string & componentName, const std::string & requiredInterfaceName, const std::string & listenerID)
{
    SendGetNamesOfEventHandlers(namesOfEventHandlers, componentName, requiredInterfaceName, listenerID);
}

void mtsManagerProxyServer::GetDescriptionOfCommand(std::string & description,
    const std::string & componentName, const std::string & providedInterfaceName, const std::string & commandName, const std::string & listenerID)
{
    SendGetDescriptionOfCommand(description, componentName, providedInterfaceName, commandName, listenerID);
}

void mtsManagerProxyServer::GetDescriptionOfEventGenerator(std::string & description,
    const std::string & componentName, const std::string & providedInterfaceName, const std::string & eventGeneratorName, const std::string & listenerID)
{
    SendGetDescriptionOfEventGenerator(description, componentName, providedInterfaceName, eventGeneratorName, listenerID);
}

void mtsManagerProxyServer::GetDescriptionOfFunction(std::string & description,
    const std::string & componentName, const std::string & requiredInterfaceName, const std::string & functionName, const std::string & listenerID)
{
    SendGetDescriptionOfFunction(description, componentName, requiredInterfaceName, functionName, listenerID);
}

void mtsManagerProxyServer::GetDescriptionOfEventHandler(std::string & description,
    const std::string & componentName, const std::string & requiredInterfaceName, const std::string & eventHandlerName, const std::string & listenerID)
{
    SendGetDescriptionOfEventHandler(description, componentName, requiredInterfaceName, eventHandlerName, listenerID);
}

void mtsManagerProxyServer::GetArgumentInformation(std::string & argumentName, std::vector<std::string> & signalNames,
    const std::string & componentName, const std::string & providedInterfaceName, const std::string & commandName, const std::string & listenerID)
{
    SendGetArgumentInformation(argumentName, signalNames, componentName, providedInterfaceName, commandName, listenerID);
}

void mtsManagerProxyServer::GetValuesOfCommand(SetOfValues & values,
    const std::string & componentName, const std::string & providedInterfaceName, const std::string & commandName, const int scalarIndex, const std::string & listenerID)
{
    SendGetValuesOfCommand(values, componentName, providedInterfaceName, commandName, scalarIndex, listenerID);
}

int mtsManagerProxyServer::GetCurrentInterfaceCount(const std::string & componentName, const std::string & listenerID)
{
    return SendGetCurrentInterfaceCount(componentName, listenerID);
}

//-------------------------------------------------------------------------
//  Event Handlers (Client -> Server)
//-------------------------------------------------------------------------
void mtsManagerProxyServer::ReceiveTestMessageFromClientToServer(
    const ConnectionIDType & connectionID, const std::string & str)
{
    const ClientIDType clientID = GetClientID(connectionID);

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer,
             "ReceiveTestMessageFromClientToServer: "
             << "\n..... ConnectionID: " << connectionID
             << "\n..... Message: " << str);
#endif

    std::cout << "Server: received from Client " << clientID << ": " << str << std::endl;
}

bool mtsManagerProxyServer::ReceiveAddClient(
    const ConnectionIDType & connectionID, const std::string & processName,
    ManagerClientProxyType & clientProxy)
{
    if (!AddProxyClient(processName, processName, connectionID, clientProxy)) {
        LogError(mtsManagerProxyServer, "ReceiveAddClient: failed to add proxy client: " << processName);
        return false;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer,
             "ReceiveAddClient: added proxy client: "
             << "\n..... ConnectionID: " << connectionID
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

bool mtsManagerProxyServer::ReceiveAddProvidedInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName, const bool isProxyInterface)
{
    return ProxyOwner->AddProvidedInterface(processName, componentName, interfaceName, isProxyInterface);
}

bool mtsManagerProxyServer::ReceiveFindProvidedInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName) const
{
    return ProxyOwner->FindProvidedInterface(processName, componentName, interfaceName);
}

bool mtsManagerProxyServer::ReceiveRemoveProvidedInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName)
{
    return ProxyOwner->RemoveProvidedInterface(processName, componentName, interfaceName);
}

bool mtsManagerProxyServer::ReceiveAddRequiredInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName, const bool isProxyInterface)
{
    return ProxyOwner->AddRequiredInterface(processName, componentName, interfaceName, isProxyInterface);
}

bool mtsManagerProxyServer::ReceiveFindRequiredInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName) const
{
    return ProxyOwner->FindRequiredInterface(processName, componentName, interfaceName);
}

bool mtsManagerProxyServer::ReceiveRemoveRequiredInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName)
{
    return ProxyOwner->RemoveRequiredInterface(processName, componentName, interfaceName);
}

::Ice::Int mtsManagerProxyServer::ReceiveConnect(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, int & userId)
{
    return ProxyOwner->Connect(connectionStringSet.RequestProcessName,
        connectionStringSet.ClientProcessName, connectionStringSet.ClientComponentName, connectionStringSet.ClientRequiredInterfaceName,
        connectionStringSet.ServerProcessName, connectionStringSet.ServerComponentName, connectionStringSet.ServerProvidedInterfaceName,
        userId);
}

bool mtsManagerProxyServer::ReceiveConnectConfirm(::Ice::Int connectionSessionID)
{
    return ProxyOwner->ConnectConfirm(connectionSessionID);
}

bool mtsManagerProxyServer::ReceiveDisconnect(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet)
{
    return ProxyOwner->Disconnect(
        connectionStringSet.ClientProcessName, connectionStringSet.ClientComponentName, connectionStringSet.ClientRequiredInterfaceName,
        connectionStringSet.ServerProcessName, connectionStringSet.ServerComponentName, connectionStringSet.ServerProvidedInterfaceName);
}

bool mtsManagerProxyServer::ReceiveSetProvidedInterfaceProxyAccessInfo(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, const std::string & endpointInfo)
{
    return ProxyOwner->SetProvidedInterfaceProxyAccessInfo(
        connectionStringSet.ClientProcessName, connectionStringSet.ClientComponentName, connectionStringSet.ClientRequiredInterfaceName,
        connectionStringSet.ServerProcessName, connectionStringSet.ServerComponentName, connectionStringSet.ServerProvidedInterfaceName,
        endpointInfo);
}

bool mtsManagerProxyServer::ReceiveGetProvidedInterfaceProxyAccessInfo(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, std::string & endpointInfo)
{
    return ProxyOwner->GetProvidedInterfaceProxyAccessInfo(
        connectionStringSet.ClientProcessName, connectionStringSet.ClientComponentName, connectionStringSet.ClientRequiredInterfaceName,
        connectionStringSet.ServerProcessName, connectionStringSet.ServerComponentName, connectionStringSet.ServerProvidedInterfaceName,
        endpointInfo);
}

bool mtsManagerProxyServer::ReceiveInitiateConnect(const unsigned int connectionID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet)
{
    return ProxyOwner->InitiateConnect(connectionID,
        connectionStringSet.ClientProcessName, connectionStringSet.ClientComponentName, connectionStringSet.ClientRequiredInterfaceName,
        connectionStringSet.ServerProcessName, connectionStringSet.ServerComponentName, connectionStringSet.ServerProvidedInterfaceName);
}

bool mtsManagerProxyServer::ReceiveConnectServerSideInterfaceRequest(const unsigned int connectionID, const unsigned int providedInterfaceProxyInstanceID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet)
{
    return ProxyOwner->ConnectServerSideInterfaceRequest(connectionID, providedInterfaceProxyInstanceID,
        connectionStringSet.ClientProcessName, connectionStringSet.ClientComponentName, connectionStringSet.ClientRequiredInterfaceName,
        connectionStringSet.ServerProcessName, connectionStringSet.ServerComponentName, connectionStringSet.ServerProvidedInterfaceName);
}

//-------------------------------------------------------------------------
//  Event Generators (Event Sender) : Server -> Client
//-------------------------------------------------------------------------
void mtsManagerProxyServer::SendTestMessageFromServerToClient(const std::string & str)
{
    if (!this->IsActiveProxy()) return;

    // iterate client map -> send message to ALL clients (broadcasts)
    ManagerClientProxyType * clientProxy;
    ClientIDMapType::iterator it = ClientIDMap.begin();
    ClientIDMapType::const_iterator itEnd = ClientIDMap.end();
    for (; it != itEnd; ++it) {
        clientProxy = &(it->second.ClientProxy);
        try
        {
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendTestMessageFromServerToClient: " << str);
#endif
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
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
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
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
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

bool mtsManagerProxyServer::SendCreateProvidedInterfaceProxy(const std::string & serverComponentProxyName,
    const ::mtsManagerProxy::ProvidedInterfaceDescription & providedInterfaceDescription, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendCreateProvidedInterfaceProxy: invalid client id (" << clientID << ") or inactive server proxy");
        return false;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendCreateProvidedInterfaceProxy: " << serverComponentProxyName << ", " << providedInterfaceDescription.ProvidedInterfaceName << ", " << clientID);
#endif

    try {
        return (*clientProxy)->CreateProvidedInterfaceProxy(serverComponentProxyName, providedInterfaceDescription);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyServer, "SendCreateProvidedInterfaceProxy: network exception: " << ex);
        OnClientDisconnect(clientID);
        return false;
    }
}

bool mtsManagerProxyServer::SendCreateRequiredInterfaceProxy(const std::string & clientComponentProxyName,
    const ::mtsManagerProxy::RequiredInterfaceDescription & requiredInterfaceDescription, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendCreateRequiredInterfaceProxy: invalid client id (" << clientID << ") or inactive server proxy");
        return false;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendCreateRequiredInterfaceProxy: " << clientComponentProxyName << ", " << requiredInterfaceDescription.RequiredInterfaceName << ", " << clientID);
#endif

    try {
        return (*clientProxy)->CreateRequiredInterfaceProxy(clientComponentProxyName, requiredInterfaceDescription);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyServer, "SendCreateRequiredInterfaceProxy: network exception: " << ex);
        OnClientDisconnect(clientID);
        return false;
    }
}

bool mtsManagerProxyServer::SendRemoveProvidedInterfaceProxy(const std::string & clientComponentProxyName,
    const std::string & providedInterfaceProxyName, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendRemoveProvidedInterfaceProxy: invalid listenerID (" << clientID << ") or inactive server proxy");
        return false;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendRemoveProvidedInterfaceProxy: " << clientComponentProxyName << ", " << providedInterfaceProxyName << ", " << clientID);
#endif

    return (*clientProxy)->RemoveProvidedInterfaceProxy(clientComponentProxyName, providedInterfaceProxyName);
}

bool mtsManagerProxyServer::SendRemoveRequiredInterfaceProxy(const std::string & serverComponentProxyName,
    const std::string & requiredInterfaceProxyName, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendRemoveRequiredInterfaceProxy: invalid listenerID (" << clientID << ") or inactive server proxy");
        return false;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendRemoveRequiredInterfaceProxy: " << serverComponentProxyName << ", " << requiredInterfaceProxyName << ", " << clientID);
#endif

    try {
        return (*clientProxy)->RemoveRequiredInterfaceProxy(serverComponentProxyName, requiredInterfaceProxyName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyServer, "SendRemoveRequiredInterfaceProxy: network exception: " << ex);
        OnClientDisconnect(clientID);
        return false;
    }
}

bool mtsManagerProxyServer::SendConnectServerSideInterface(const int userId, const unsigned int providedInterfaceProxyInstanceID,
    const ::mtsManagerProxy::ConnectionStringSet & connectionStrings, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendConnectServerSideInterface: invalid listenerID (" << clientID << ") or inactive server proxy");
        return false;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendConnectServerSideInterface: " << userId << ", " << providedInterfaceProxyInstanceID << ", " << clientID);
#endif

    return (*clientProxy)->ConnectServerSideInterface(userId, providedInterfaceProxyInstanceID, connectionStrings);
}

bool mtsManagerProxyServer::SendConnectClientSideInterface(::Ice::Int connectionID,
    const ::mtsManagerProxy::ConnectionStringSet & connectionStrings, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
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

int mtsManagerProxyServer::SendPreAllocateResources(const std::string & userName,
    const std::string & serverProcessName, const std::string & serverComponentName, 
    const std::string & serverProvidedInterfaceName, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendPreAllocateResources: invalid listenerID (" << clientID << ") or inactive server proxy");
        return -1;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendPreAllocateResources: " << userName << ", " << serverProcessName << ":" << serverComponentName << ":" << serverProvidedInterfaceName);
#endif

    try {
        return (*clientProxy)->PreAllocateResources(userName, serverProcessName, serverComponentName, serverProvidedInterfaceName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyServer, "SendPreAllocateResources: network exception: " << ex);
        OnClientDisconnect(clientID);
        return -1;
    }
}

bool mtsManagerProxyServer::SendGetProvidedInterfaceDescription(
    const unsigned int userId, const std::string & serverComponentName, const std::string & providedInterfaceName,
    ::mtsManagerProxy::ProvidedInterfaceDescription & providedInterfaceDescription, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendGetProvidedInterfaceDescription: invalid listenerID (" << clientID << ") or inactive server proxy");
        return false;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendGetProvidedInterfaceDescription: " << userId << ", " << serverComponentName << ":" << providedInterfaceName << ", " << clientID);
#endif

    try {
        return (*clientProxy)->GetProvidedInterfaceDescription(userId, serverComponentName, providedInterfaceName, providedInterfaceDescription);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyServer, "SendGetProvidedInterfaceDescription: network exception: " << ex);
        OnClientDisconnect(clientID);
        return false;
    }
}

bool mtsManagerProxyServer::SendGetRequiredInterfaceDescription(const std::string & componentName,
    const std::string & requiredInterfaceName, ::mtsManagerProxy::RequiredInterfaceDescription & requiredInterfaceDescription, 
    const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendGetRequiredInterfaceDescription: invalid listenerID (" << clientID << ") or inactive server proxy");
        return false;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendGetRequiredInterfaceDescription: " << componentName << ", " << requiredInterfaceName << ", " << clientID);
#endif

    return (*clientProxy)->GetRequiredInterfaceDescription(componentName, requiredInterfaceName, requiredInterfaceDescription);
}

void mtsManagerProxyServer::SendGetNamesOfCommands(std::vector<std::string>& namesOfCommands,
    const std::string & componentName, const std::string & providedInterfaceName, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendGetNamesOfCommands: invalid listenerID (" << clientID << ") or inactive server proxy");
        return;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendGetNamesOfCommands: " << componentName << ", " << providedInterfaceName << ", " << clientID);
#endif

    (*clientProxy)->GetNamesOfCommands(componentName, providedInterfaceName, namesOfCommands);
}

void mtsManagerProxyServer::SendGetNamesOfEventGenerators(std::vector<std::string>& namesOfEventGenerators,
    const std::string & componentName, const std::string & providedInterfaceName, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendGetNamesOfEventGenerators: invalid listenerID (" << clientID << ") or inactive server proxy");
        return;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendGetNamesOfEventGenerators: " << componentName << ", " << providedInterfaceName << ", " << clientID);
#endif

    (*clientProxy)->GetNamesOfEventGenerators(componentName, providedInterfaceName, namesOfEventGenerators);
}

void mtsManagerProxyServer::SendGetNamesOfFunctions(std::vector<std::string>& namesOfFunctions,
    const std::string & componentName, const std::string & requiredInterfaceName, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendGetNamesOfFunctions: invalid listenerID (" << clientID << ") or inactive server proxy");
        return;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendGetNamesOfFunctions: " << componentName << ", " << requiredInterfaceName << ", " << clientID);
#endif

    (*clientProxy)->GetNamesOfFunctions(componentName, requiredInterfaceName, namesOfFunctions);
}

void mtsManagerProxyServer::SendGetNamesOfEventHandlers(std::vector<std::string>& namesOfEventHandlers,
    const std::string & componentName, const std::string & requiredInterfaceName, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendGetNamesOfEventHandlers: invalid listenerID (" << clientID << ") or inactive server proxy");
        return;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendGetNamesOfEventHandlers: " << componentName << ", " << requiredInterfaceName << ", " << clientID);
#endif

    (*clientProxy)->GetNamesOfEventHandlers(componentName, requiredInterfaceName, namesOfEventHandlers);
}

void mtsManagerProxyServer::SendGetDescriptionOfCommand(std::string & description, const std::string & componentName, 
    const std::string & providedInterfaceName, const std::string & commandName, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendGetDescriptionOfCommand: invalid listenerID (" << clientID << ") or inactive server proxy");
        return;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendGetDescriptionOfCommand: " << componentName << ", " << providedInterfaceName << ", " << commandName << ", " << clientID);
#endif

    (*clientProxy)->GetDescriptionOfCommand(componentName, providedInterfaceName, commandName, description);
}

void mtsManagerProxyServer::SendGetDescriptionOfEventGenerator(std::string & description, const std::string & componentName, 
    const std::string & providedInterfaceName, const std::string & eventGeneratorName, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendGetDescriptionOfEventGenerator: invalid listenerID (" << clientID << ") or inactive server proxy");
        return;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendGetDescriptionOfEventGenerator: " << componentName << ", " << providedInterfaceName << ", " << eventGeneratorName << ", " << clientID);
#endif

    (*clientProxy)->GetDescriptionOfEventGenerator(componentName, providedInterfaceName, eventGeneratorName, description);
}

void mtsManagerProxyServer::SendGetDescriptionOfFunction(std::string & description, const std::string & componentName, 
    const std::string & requiredInterfaceName, const std::string & functionName, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendGetDescriptionOfFunction: invalid listenerID (" << clientID << ") or inactive server proxy");
        return;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendGetDescriptionOfFunction: " << componentName << ", " << requiredInterfaceName << ", " << functionName << ", " << clientID);
#endif

    (*clientProxy)->GetDescriptionOfFunction(componentName, requiredInterfaceName, functionName, description);
}

void mtsManagerProxyServer::SendGetDescriptionOfEventHandler(std::string & description, const std::string & componentName, 
    const std::string & requiredInterfaceName, const std::string & eventHandlerName, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendGetDescriptionOfEventHandler: invalid listenerID (" << clientID << ") or inactive server proxy");
        return;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendGetDescriptionOfEventHandler: " << componentName << ", " << requiredInterfaceName << ", " << eventHandlerName << ", " << clientID);
#endif

    (*clientProxy)->GetDescriptionOfEventHandler(componentName, requiredInterfaceName, eventHandlerName, description);
}

void mtsManagerProxyServer::SendGetArgumentInformation(std::string & argumentName, std::vector<std::string> & signalNames,
    const std::string & componentName, const std::string & providedInterfaceName, const std::string & commandName, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendGetArgumentInformation: invalid listenerID (" << clientID << ") or inactive server proxy");
        return;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendGetArgumentInformation: " << componentName << ", " << providedInterfaceName << ", " << commandName << ", " << clientID);
#endif

    (*clientProxy)->GetArgumentInformation(componentName, providedInterfaceName, commandName, argumentName, signalNames);
}

void mtsManagerProxyServer::SendGetValuesOfCommand(SetOfValues & values, const std::string & componentName, 
    const std::string & providedInterfaceName, const std::string & commandName, const int scalarIndex, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendGetValuesOfCommand: invalid listenerID (" << clientID << ") or inactive server proxy");
        return;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendGetValuesOfCommand: " << componentName << ", " << providedInterfaceName << ", " << commandName << ", " << scalarIndex << ", " << clientID);
#endif

    ::mtsManagerProxy::SetOfValues valuesICEtype;

    (*clientProxy)->GetValuesOfCommand(componentName, providedInterfaceName, commandName, scalarIndex, valuesICEtype);

    ConvertValuesOfCommand(valuesICEtype, values);
}

std::string mtsManagerProxyServer::SendGetProcessName(const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
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

::Ice::Int mtsManagerProxyServer::SendGetCurrentInterfaceCount(const std::string & componentName, const std::string & clientID)
{
    ManagerClientProxyType * clientProxy = GetNetworkProxyClient(clientID);
    if (!clientProxy) {
        LogError(mtsManagerProxyServer, "SendGetCurrentInterfaceCount: invalid listenerID (" << clientID << ") or inactive server proxy");
        return -1;
    }

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(mtsManagerProxyServer, ">>>>> SEND: SendGetCurrentInterfaceCount: " << componentName << ", " << clientID);
#endif

    try {
        return (*clientProxy)->GetCurrentInterfaceCount(componentName);
    } catch (const ::Ice::Exception & ex) {
        LogError(mtsManagerProxyServer, "SendGetCurrentInterfaceCount: network exception: " << ex);
        OnClientDisconnect(clientID);
        return -1;
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

    // Sleep for some time enough for Run() loop to terminate
    osaSleep(1 * cmn_s);
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

    while (IsActiveProxy())
    {
        osaSleep(1 * cmn_s);
        std::cout << "\tServer [" << ManagerProxyServer->GetProxyName() << "] running (" << ++count << ")" << std::endl;

        std::stringstream ss;
        ss << "Msg " << count << " from Server";

        ManagerProxyServer->SendTestMessageFromServerToClient(ss.str());
    }
#else
    while(IsActiveProxy())
    {
        // Check connection element map to cancel timed out connection elements
        ManagerProxyServer->ConnectCheckTimeout();

        //IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
        // Check connections at every 1 second
        ManagerProxyServer->MonitorConnections();
        osaSleep(1 * cmn_s);
    }
#endif
}

void mtsManagerProxyServer::ManagerServerI::Stop()
{
    if (!ManagerProxyServer->IsActiveProxy()) return;

    // TODO: Review the following codes
    IceUtil::ThreadPtr callbackSenderThread;
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);

        // Change this from active to 'prepare stop(?)'
        notify();

        callbackSenderThread = SenderThreadPtr;
        SenderThreadPtr = 0; // Resolve cyclic dependency.
    }
    callbackSenderThread->getThreadControl().join();
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

    const ConnectionIDType connectionID = current.ctx.find(mtsManagerProxyServer::GetConnectionIDKey())->second;

    ManagerProxyServer->ReceiveTestMessageFromClientToServer(connectionID, str);
}

bool mtsManagerProxyServer::ManagerServerI::AddClient(
    const std::string & processName, const ::Ice::Identity & identity, const ::Ice::Current& current)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
   LogPrint(ManagerServerI, "<<<<< RECV: AddClient: " << processName << " (" << Communicator->identityToString(identity) << ")");
#endif

    const ConnectionIDType connectionID = current.ctx.find(mtsManagerProxyServer::GetConnectionIDKey())->second;

    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);

    ManagerClientProxyType clientProxy =
        ManagerClientProxyType::uncheckedCast(current.con->createProxy(identity));

    return ManagerProxyServer->ReceiveAddClient(connectionID, processName, clientProxy);
}

void mtsManagerProxyServer::ManagerServerI::Refresh(const ::Ice::Current& current)
{
    const ConnectionIDType connectionID = current.ctx.find(mtsManagerProxyServer::GetConnectionIDKey())->second;

#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: Refresh: " << connectionID);
#endif

    //
    // TODO: Refresh this session
    //
}

void mtsManagerProxyServer::ManagerServerI::Shutdown(const ::Ice::Current& current)
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: Shutdown");
#endif

    const ConnectionIDType connectionID = current.ctx.find(mtsManagerProxyServer::GetConnectionIDKey())->second;

    // TODO:
    // Set as true to represent that this connection (session) is going to be closed.
    // After this flag is set, no message is allowed to be sent to a server.
    //ComponentInterfaceProxyServer->ShutdownSession(current);
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

bool mtsManagerProxyServer::ManagerServerI::AddProvidedInterface(
    const std::string & processName, const std::string & componentName,
    const std::string & interfaceName, bool isProxyInterface, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: AddProvidedInterface: " << processName << ", " << componentName << ", " << interfaceName << ", " << isProxyInterface);
#endif

    return ManagerProxyServer->ReceiveAddProvidedInterface(processName, componentName, interfaceName, isProxyInterface);
}

bool mtsManagerProxyServer::ManagerServerI::FindProvidedInterface(
    const std::string & processName, const std::string & componentName,
    const std::string & interfaceName, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: FindProvidedInterface: " << processName << ", " << componentName << ", " << interfaceName);
#endif

    return ManagerProxyServer->ReceiveFindProvidedInterface(processName, componentName, interfaceName);
}

bool mtsManagerProxyServer::ManagerServerI::RemoveProvidedInterface(
    const std::string & processName, const std::string & componentName,
    const std::string & interfaceName, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: RemoveProvidedInterface: " << processName << ", " << componentName << ", " << interfaceName);
#endif

    return ManagerProxyServer->ReceiveRemoveProvidedInterface(processName, componentName, interfaceName);
}

bool mtsManagerProxyServer::ManagerServerI::AddRequiredInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName, bool isProxyInterface, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: AddRequiredInterface: " << processName << ", " << componentName << ", " << interfaceName << ", " << isProxyInterface);
#endif

    return ManagerProxyServer->ReceiveAddRequiredInterface(processName, componentName, interfaceName, isProxyInterface);
}

bool mtsManagerProxyServer::ManagerServerI::FindRequiredInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName, const ::Ice::Current &) const
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: FindRequiredInterface: " << processName << ", " << componentName << ", " << interfaceName);
#endif

    return ManagerProxyServer->ReceiveFindRequiredInterface(processName, componentName, interfaceName);
}

bool mtsManagerProxyServer::ManagerServerI::RemoveRequiredInterface(const std::string & processName, const std::string & componentName, const std::string & interfaceName, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: RemoveRequiredInterface: " << processName << ", " << componentName << ", " << interfaceName);
#endif

    return ManagerProxyServer->ReceiveRemoveRequiredInterface(processName, componentName, interfaceName);
}

::Ice::Int mtsManagerProxyServer::ManagerServerI::Connect(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, ::Ice::Int & userId, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: Connect: "
        << connectionStringSet.ClientProcessName << ":" << connectionStringSet.ClientComponentName << ":" << connectionStringSet.ClientRequiredInterfaceName
        << "-"
        << connectionStringSet.ServerProcessName << ":" << connectionStringSet.ServerComponentName << ":" << connectionStringSet.ServerProvidedInterfaceName);
#endif

    return ManagerProxyServer->ReceiveConnect(connectionStringSet, userId);
}

bool mtsManagerProxyServer::ManagerServerI::ConnectConfirm(::Ice::Int connectionSessionID, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: ConnectConfirm: " << connectionSessionID);
#endif

    return ManagerProxyServer->ReceiveConnectConfirm(connectionSessionID);
}

bool mtsManagerProxyServer::ManagerServerI::Disconnect(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: Disconnect: "
        << connectionStringSet.ClientProcessName << ":" << connectionStringSet.ClientComponentName << ":" << connectionStringSet.ClientRequiredInterfaceName
        << "-"
        << connectionStringSet.ServerProcessName << ":" << connectionStringSet.ServerComponentName << ":" << connectionStringSet.ServerProvidedInterfaceName);
#endif

    return ManagerProxyServer->ReceiveDisconnect(connectionStringSet);
}

bool mtsManagerProxyServer::ManagerServerI::SetProvidedInterfaceProxyAccessInfo(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, const std::string & endpointInfo, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: SetProvidedInterfaceProxyAccessInfo: " << endpointInfo);
#endif

    return ManagerProxyServer->ReceiveSetProvidedInterfaceProxyAccessInfo(connectionStringSet, endpointInfo);
}

bool mtsManagerProxyServer::ManagerServerI::GetProvidedInterfaceProxyAccessInfo(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, std::string & endpointInfo, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: GetProvidedInterfaceProxyAccessInfo");
#endif

    return ManagerProxyServer->ReceiveGetProvidedInterfaceProxyAccessInfo(connectionStringSet, endpointInfo);
}

bool mtsManagerProxyServer::ManagerServerI::InitiateConnect(::Ice::Int connectionID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: InitiateConnect: " << connectionID << ", "
        << connectionStringSet.ClientProcessName << ":" << connectionStringSet.ClientComponentName << ":" << connectionStringSet.ClientRequiredInterfaceName
        << "-"
        << connectionStringSet.ServerProcessName << ":" << connectionStringSet.ServerComponentName << ":" << connectionStringSet.ServerProvidedInterfaceName);

#endif

    return ManagerProxyServer->ReceiveInitiateConnect(connectionID, connectionStringSet);
}

bool mtsManagerProxyServer::ManagerServerI::ConnectServerSideInterfaceRequest(::Ice::Int connectionID, ::Ice::Int providedInterfaceProxyInstanceID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, const ::Ice::Current & CMN_UNUSED(current))
{
#ifdef ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG
    LogPrint(ManagerServerI, "<<<<< RECV: ConnectServerSideInterfaceRequest: " << connectionID << ", " << providedInterfaceProxyInstanceID << ", "
        << connectionStringSet.ClientProcessName << ":" << connectionStringSet.ClientComponentName << ":" << connectionStringSet.ClientRequiredInterfaceName
        << "-"
        << connectionStringSet.ServerProcessName << ":" << connectionStringSet.ServerComponentName << ":" << connectionStringSet.ServerProvidedInterfaceName);
#endif

    return ManagerProxyServer->ReceiveConnectServerSideInterfaceRequest(connectionID, providedInterfaceProxyInstanceID, connectionStringSet);
}
