/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsDeviceInterfaceProxyClient.cpp 145 2009-03-18 23:32:40Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2009-04-24

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsDeviceInterfaceProxyClient.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsCommandVoidProxy.h>
#include <cisstMultiTask/mtsCommandWriteProxy.h>
#include <cisstMultiTask/mtsCommandReadProxy.h>
#include <cisstMultiTask/mtsCommandQualifiedReadProxy.h>
#include <cisstMultiTask/mtsRequiredInterface.h>
#include <cisstMultiTask/mtsDeviceProxy.h>
#include <cisstMultiTask/mtsTask.h>

CMN_IMPLEMENT_SERVICES(mtsDeviceInterfaceProxyClient);

#define DeviceInterfaceProxyClientLogger(_log) BaseType::IceLogger->trace("mtsDeviceInterfaceProxyClient", _log)
#define DeviceInterfaceProxyClientLoggerError(_log1, _log2) \
    {   std::stringstream s;\
        s << "mtsDeviceInterfaceProxyClient: " << _log1 << _log2;\
        BaseType::IceLogger->error(s.str());  }

mtsDeviceInterfaceProxyClient::mtsDeviceInterfaceProxyClient(
    const std::string & propertyFileName, const std::string & objectIdentity) :
        BaseType(propertyFileName, objectIdentity)
{
}

mtsDeviceInterfaceProxyClient::~mtsDeviceInterfaceProxyClient()
{
}

void mtsDeviceInterfaceProxyClient::Start(mtsTask * callingTask)
{
    // Initialize Ice object.
    // Notice that a worker thread is not created right now.
    IceInitialize();
    
    if (InitSuccessFlag) {
        // Client configuration for bidirectional communication
        Ice::ObjectAdapterPtr adapter = IceCommunicator->createObjectAdapter("");
        Ice::Identity ident;
        ident.name = GetGUID();
        ident.category = "";

        mtsDeviceInterfaceProxy::DeviceInterfaceClientPtr client = 
            new DeviceInterfaceClientI(IceCommunicator, IceLogger, DeviceInterfaceServerProxy, this);
        adapter->add(client, ident);
        adapter->activate();
        DeviceInterfaceServerProxy->ice_getConnection()->setAdapter(adapter);
        DeviceInterfaceServerProxy->AddClient(ident);

        // Create a worker thread here and returns immediately.
        ThreadArgumentsInfo.argument = callingTask;
        ThreadArgumentsInfo.proxy = this;        
        ThreadArgumentsInfo.Runner = mtsDeviceInterfaceProxyClient::Runner;

        WorkerThread.Create<ProxyWorker<mtsTask>, ThreadArguments<mtsTask>*>(
            &ProxyWorkerInfo, &ProxyWorker<mtsTask>::Run, &ThreadArgumentsInfo, 
            // Set the name of this thread as DIC which means Device Interface Client.
            // This is to avoid the thread name conflict with mtsTaskManagerProxyClient
            // class because Linux RTAI doesn't allow two threads to have the same thread name.
            "DIC");
    }
}

void mtsDeviceInterfaceProxyClient::StartClient()
{
    Sender->Start();

    // This is a blocking call that should run in a different thread.
    IceCommunicator->waitForShutdown();
}

void mtsDeviceInterfaceProxyClient::Runner(ThreadArguments<mtsTask> * arguments)
{
    mtsDeviceInterfaceProxyClient * ProxyClient = 
        dynamic_cast<mtsDeviceInterfaceProxyClient*>(arguments->proxy);
    if (!ProxyClient) {
        CMN_LOG_RUN_ERROR << "mtsDeviceInterfaceProxyClient: Failed to create a proxy server." << std::endl;
        return;
    }
    
    ProxyClient->SetConnectedTask(arguments->argument);

    ProxyClient->GetLogger()->trace("mtsDeviceInterfaceProxyClient", "Proxy server starts.....");

    try {
        ProxyClient->StartClient();        
    } catch (const Ice::Exception& e) {
        ProxyClient->GetLogger()->trace("mtsDeviceInterfaceProxyClient exception: ", e.what());
    } catch (const char * msg) {
        ProxyClient->GetLogger()->trace("mtsDeviceInterfaceProxyClient exception: ", msg);
    }

    ProxyClient->GetLogger()->trace("mtsDeviceInterfaceProxyClient", "Proxy server terminates.....");
}

void mtsDeviceInterfaceProxyClient::Stop()
{
    OnEnd();
}

void mtsDeviceInterfaceProxyClient::OnEnd()
{
    DeviceInterfaceProxyClientLogger("DeviceInterfaceProxy client ends.");

    // Let a server disconnect this client safely.
    // gcc says this doesn't exist
    DeviceInterfaceServerProxy->Shutdown();

    ShutdownSession();
    
    BaseType::OnEnd();
    
    Sender->Stop();
}

//-------------------------------------------------------------------------
//  Method to register per-command serializer
//-------------------------------------------------------------------------
bool mtsDeviceInterfaceProxyClient::AddPerCommandSerializer(
    const CommandIDType commandId, mtsProxySerializer * argumentSerializer)
{
    CMN_ASSERT(argumentSerializer);

    PerCommandSerializerMapType::const_iterator it = PerCommandSerializerMap.find(commandId);
    if (it != PerCommandSerializerMap.end()) {
        CMN_LOG_RUN_ERROR << "mtsDeviceInterfaceProxyClient: CommandId already exists." << std::endl;
        return false;
    }

    PerCommandSerializerMap[commandId] = argumentSerializer;

    return true;
}

//-------------------------------------------------------------------------
//  Methods to Receive and Process Events (Server -> Client)
//-------------------------------------------------------------------------
void mtsDeviceInterfaceProxyClient::ReceiveExecuteEventVoid(const CommandIDType commandId)
{
    mtsMulticastCommandVoid * eventVoidGeneratorProxy = 
        reinterpret_cast<mtsMulticastCommandVoid*>(commandId);
    CMN_ASSERT(eventVoidGeneratorProxy);

    eventVoidGeneratorProxy->Execute();
}

void mtsDeviceInterfaceProxyClient::ReceiveExecuteEventWriteSerialized(
    const CommandIDType commandId, const std::string argument)
{
    static char buf[1024];
    sprintf(buf, "ReceiveExecuteEventWriteSerialized: %lu bytes received", argument.size());
    IceLogger->trace("TIClient", buf);

    mtsMulticastCommandWriteProxy * eventWriteGeneratorProxy = 
        reinterpret_cast<mtsMulticastCommandWriteProxy*>(commandId);
    CMN_ASSERT(eventWriteGeneratorProxy);

    // Get a per-command serializer.
    mtsProxySerializer * deserializer = eventWriteGeneratorProxy->GetSerializer();
        
    mtsGenericObject * serializedArgument = deserializer->DeSerialize(argument);
    CMN_ASSERT(serializedArgument);

    eventWriteGeneratorProxy->Execute(*serializedArgument);
}

//-------------------------------------------------------------------------
//  Methods to Send Events
//-------------------------------------------------------------------------
bool mtsDeviceInterfaceProxyClient::SendGetProvidedInterfaceInfo(
    const std::string & providedInterfaceName,
    mtsDeviceInterfaceProxy::ProvidedInterfaceInfo & providedInterfaceInfo)
{
    if (!IsValidSession) return false;

    IceLogger->trace("TIClient", ">>>>> SEND: SendGetProvidedInterface");

    return DeviceInterfaceServerProxy->GetProvidedInterfaceInfo(
        providedInterfaceName, providedInterfaceInfo);
}

bool mtsDeviceInterfaceProxyClient::SendCreateClientProxies(
    const std::string & userTaskName, const std::string & requiredInterfaceName,
    const std::string & resourceTaskName, const std::string & providedInterfaceName)
{
    if (!IsValidSession) return false;

    IceLogger->trace("TIClient", ">>>>> SEND: SendCreateClientProxies");

    return DeviceInterfaceServerProxy->CreateClientProxies(
        userTaskName, requiredInterfaceName, resourceTaskName, providedInterfaceName);
}

bool mtsDeviceInterfaceProxyClient::SendConnectServerSide(
    const std::string & userTaskName, const std::string & requiredInterfaceName,
    const std::string & resourceTaskName, const std::string & providedInterfaceName)
{
    if (!IsValidSession) return false;

    IceLogger->trace("TIClient", ">>>>> SEND: SendConnectServerSide");

    return DeviceInterfaceServerProxy->ConnectServerSide(
        userTaskName, requiredInterfaceName, resourceTaskName, providedInterfaceName);
}

bool mtsDeviceInterfaceProxyClient::SendUpdateEventHandlerId(
    const std::string & clientTaskProxyName,
    const mtsDeviceInterfaceProxy::ListsOfEventGeneratorsRegistered & eventGeneratorProxies)
{
    if (!IsValidSession) return false;

    IceLogger->trace("TIClient", ">>>>> SEND: SendUpdateEventHandlerId");

    return DeviceInterfaceServerProxy->UpdateEventHandlerId(
        clientTaskProxyName, eventGeneratorProxies);
}

void mtsDeviceInterfaceProxyClient::SendGetCommandId(
    const std::string & clientTaskProxyName,
    mtsDeviceInterfaceProxy::FunctionProxySet & functionProxies)
{
    if (!IsValidSession) return;

    IceLogger->trace("TIClient", ">>>>> SEND: SendGetCommandId");

    DeviceInterfaceServerProxy->GetCommandId(clientTaskProxyName, functionProxies);
}

void mtsDeviceInterfaceProxyClient::SendExecuteCommandVoid(const CommandIDType commandId) const
{
    if (!IsValidSession) return;

    //Logger->trace("TIClient", ">>>>> SEND: SendExecuteCommandVoid");

    DeviceInterfaceServerProxy->ExecuteCommandVoid(commandId);
}

void mtsDeviceInterfaceProxyClient::SendExecuteCommandWriteSerialized(
    const CommandIDType commandId, const mtsGenericObject & argument)
{
    if (!IsValidSession) return;

    //Logger->trace("TIClient", ">>>>> SEND: SendExecuteCommandWriteSerialized");

    // Get a per-command serializer.
    mtsProxySerializer * serializer = PerCommandSerializerMap[commandId];
    if (!serializer) {
        CMN_LOG_RUN_ERROR << "mtsDeviceInterfaceProxyClient: cannot find serializer (commandWrite)." << std::endl;
        return;
    }

    // Serialize the argument passed.
    std::string serializedArgument;
    serializer->Serialize(argument, serializedArgument);
    if (serializedArgument.size() == 0) {
        CMN_LOG_RUN_ERROR << "mtsDeviceInterfaceProxyClient: serialization failure (commandWrite): " 
            << argument.ToString() << std::endl;
        return;
    }
    
    DeviceInterfaceServerProxy->ExecuteCommandWriteSerialized(commandId, serializedArgument);
}

void mtsDeviceInterfaceProxyClient::SendExecuteCommandReadSerialized(
    const CommandIDType commandId, mtsGenericObject & argument)
{
    if (!IsValidSession) return;

    //Logger->trace("TIClient", ">>>>> SEND: SendExecuteCommandReadSerialized");

    // Placeholder for an argument of which value is to be set by the peer.
    std::string serializedArgument;

    DeviceInterfaceServerProxy->ExecuteCommandReadSerialized(commandId, serializedArgument);

    // Deserialize the argument.
    // Get a per-command serializer.
    mtsProxySerializer * deserializer = PerCommandSerializerMap[commandId];
    if (!deserializer) {
        CMN_LOG_RUN_ERROR << "mtsDeviceInterfaceProxyClient: cannot find deserializer (commandRead)" << std::endl;
        return;
    }

    deserializer->DeSerialize(serializedArgument, argument);
}

void mtsDeviceInterfaceProxyClient::SendExecuteCommandQualifiedReadSerialized(
    const CommandIDType commandId, const mtsGenericObject & argument1, mtsGenericObject & argument2)
{
    if (!IsValidSession) return;

    //Logger->trace("TIClient", ">>>>> SEND: SendExecuteCommandQualifiedRead");
    
    // Get a per-command serializer.
    mtsProxySerializer * serializer = PerCommandSerializerMap[commandId];
    if (!serializer) {
        CMN_LOG_RUN_ERROR << "mtsDeviceInterfaceProxyClient: cannot find serializer (commandQRead)" << std::endl;
        return;
    }

    // Serialize the argument1.
    std::string serializedArgument1;
    serializer->Serialize(argument1, serializedArgument1);
    if (serializedArgument1.size() == 0) {
        CMN_LOG_RUN_ERROR << "mtsDeviceInterfaceProxyClient: serialization failure (commandQRead): " 
            << argument1.ToString() << std::endl;
        return;
    }

    // Placeholder for an argument of which value is to be set by the peer.
    std::string serializedArgument2;

    // Execute the command across networks
    DeviceInterfaceServerProxy->ExecuteCommandQualifiedReadSerialized(
        commandId, serializedArgument1, serializedArgument2);

    // Deserialize the argument2.
    serializer->DeSerialize(serializedArgument2, argument2);
}

//-------------------------------------------------------------------------
//  Send Methods
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
//  Definition by mtsDeviceInterfaceProxy.ice
//-------------------------------------------------------------------------
mtsDeviceInterfaceProxyClient::DeviceInterfaceClientI::DeviceInterfaceClientI(const Ice::CommunicatorPtr& communicator,                           
                                                                              const Ice::LoggerPtr& logger,
                                                                              const mtsDeviceInterfaceProxy::DeviceInterfaceServerPrx& server,
                                                                              mtsDeviceInterfaceProxyClient * deviceInterfaceClient):
    Runnable(true), 
    Communicator(communicator),
    SenderThreadPtr(new SenderThread<DeviceInterfaceClientIPtr>(this)),
    Logger(logger),
    Server(server),
    DeviceInterfaceClient(deviceInterfaceClient)
{
}

void mtsDeviceInterfaceProxyClient::DeviceInterfaceClientI::Start()
{
    DeviceInterfaceClient->GetLogger()->trace(
        "mtsDeviceInterfaceProxyClient", "Send thread starts");

    SenderThreadPtr->start();
}

void mtsDeviceInterfaceProxyClient::DeviceInterfaceClientI::Run()
{
    while (Runnable)
    {
        osaSleep(10 * cmn_ms);
    }
}

void mtsDeviceInterfaceProxyClient::DeviceInterfaceClientI::Stop()
{
    if (!DeviceInterfaceClient->IsActiveProxy()) return;

    IceUtil::ThreadPtr callbackSenderThread;
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);

        Runnable = false;
        notify();

        callbackSenderThread = SenderThreadPtr;
        SenderThreadPtr = 0; // Resolve cyclic dependency.
    }
    callbackSenderThread->getThreadControl().join();
}

//-----------------------------------------------------------------------------
//  Device Interface Proxy Client Implementation
//-----------------------------------------------------------------------------
//bool mtsDeviceInterfaceProxyClient::DeviceInterfaceClientI::GetListsOfEventGeneratorsRegistered(
//    const std::string & serverTaskProxyName,
//    const std::string & clientTaskName,
//    const std::string & requiredInterfaceName,
//    mtsDeviceInterfaceProxy::ListsOfEventGeneratorsRegistered & eventGeneratorProxies,
//    const ::Ice::Current&) const
//{
//    IceLogger->trace("TIClient", "<<<<< RECV: GetListsOfEventGeneratorsRegistered");
//
//    return DeviceInterfaceClient->ReceiveGetListsOfEventGeneratorsRegistered(
//        serverTaskProxyName, clientTaskName, 
//        requiredInterfaceName, eventGeneratorProxies);
//}

void mtsDeviceInterfaceProxyClient::DeviceInterfaceClientI::ExecuteEventVoid(
    ::Ice::IceCommandIDType commandId, const ::Ice::Current&)
{
    Logger->trace("TIClient", "<<<<< RECV: ExecuteEventVoid");

    DeviceInterfaceClient->ReceiveExecuteEventVoid(commandId);
}

void mtsDeviceInterfaceProxyClient::DeviceInterfaceClientI::ExecuteEventWriteSerialized(
    ::Ice::IceCommandIDType commandId, const ::std::string& argument, const ::Ice::Current&)
{
    Logger->trace("TIClient", "<<<<< RECV: ExecuteEventWriteSerialized");

    DeviceInterfaceClient->ReceiveExecuteEventWriteSerialized(commandId, argument);
}
