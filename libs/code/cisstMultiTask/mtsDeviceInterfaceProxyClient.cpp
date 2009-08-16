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
            &ProxyWorkerInfo, &ProxyWorker<mtsTask>::Run, &ThreadArgumentsInfo, "S-PRX");
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

    ProxyClient->GetLogger()->trace("mtsDeviceInterfaceProxyClient", "Proxy client thread starts.");

    try {
        ProxyClient->StartClient();        
    } catch (const Ice::Exception& e) {
        ProxyClient->GetLogger()->trace("mtsDeviceInterfaceProxyClient exception: ", e.what());
    } catch (const char * msg) {
        ProxyClient->GetLogger()->trace("mtsDeviceInterfaceProxyClient exception: ", msg);
    }

    ProxyClient->GetLogger()->trace("mtsDeviceInterfaceProxyClient", "Proxy client thread ends.");
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
//  Methods to Receive and Process Events (Server -> Client)
//-------------------------------------------------------------------------
//bool mtsDeviceInterfaceProxyClient::ReceiveGetListsOfEventGeneratorsRegistered(
//    const std::string & serverTaskProxyName,
//    const std::string & clientTaskName,
//    const std::string & requiredInterfaceName,
//    mtsDeviceInterfaceProxy::ListsOfEventGeneratorsRegistered & eventGeneratorProxies) const
//{ 
//    /*
//    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
//
//    // Get the client task to access the required interface.
//    mtsDevice * clientTask = taskManager->GetDevice(clientTaskName);
//    if (!clientTask) {
//        clientTask = dynamic_cast<mtsDevice*>(taskManager->GetTask(clientTaskName));
//    }
//    CMN_ASSERT(clientTask);
//
//    // Get the required interface.
//    mtsRequiredInterface * requiredInterface = 
//        clientTask->GetRequiredInterface(requiredInterfaceName);
//    CMN_ASSERT(requiredInterface);
//    
//    // Get the provided interface proxy.
//    mtsProvidedInterface * providedInterfaceProxy = 
//        requiredInterface->GetConnectedInterface();
//    CMN_ASSERT(providedInterfaceProxy);
//
//    // Get the server task proxy.
//    mtsDeviceProxy * serverTaskProxy = dynamic_cast<mtsDeviceProxy*>(
//        taskManager->GetDevice(serverTaskProxyName));
//    CMN_ASSERT(serverTaskProxy);
//
//    serverTaskProxy->GetEventGeneratorProxyPointers(
//        providedInterfaceProxy, requiredInterface, eventGeneratorProxies);
//        */
//
//    return true;
//}

void mtsDeviceInterfaceProxyClient::ReceiveExecuteEventVoid(const int commandId)
{
    mtsMulticastCommandVoid * eventVoidGeneratorProxy = 
        reinterpret_cast<mtsMulticastCommandVoid*>(commandId);
    CMN_ASSERT(eventVoidGeneratorProxy);

    eventVoidGeneratorProxy->Execute();
}

void mtsDeviceInterfaceProxyClient::ReceiveExecuteEventWriteSerialized(
    const int commandId, const std::string argument)
{
    static char buf[1024];
    sprintf(buf, "ReceiveExecuteEventWriteSerialized: %d bytes received", argument.size());
    IceLogger->trace("TIClient", buf);

    // Deserialization
    DeSerializationBuffer.str("");
    DeSerializationBuffer << argument;
    
    mtsMulticastCommandWriteProxy * eventWriteGeneratorProxy = 
        reinterpret_cast<mtsMulticastCommandWriteProxy*>(commandId);
    CMN_ASSERT(eventWriteGeneratorProxy);
        
    const mtsGenericObject * obj = dynamic_cast<mtsGenericObject *>(DeSerializer->DeSerialize());
    CMN_ASSERT(obj);
    //(*eventWriteGeneratorProxy)(*obj);
    eventWriteGeneratorProxy->Execute(*obj);
}

//-------------------------------------------------------------------------
//  Methods to Send Events
//-------------------------------------------------------------------------
const bool mtsDeviceInterfaceProxyClient::SendGetProvidedInterfaceInfo(
    const std::string & providedInterfaceName,
    mtsDeviceInterfaceProxy::ProvidedInterfaceInfo & providedInterfaceInfo)
{
    if (!IsValidSession) return false;

    IceLogger->trace("TIClient", ">>>>> SEND: SendGetProvidedInterface");

    return DeviceInterfaceServerProxy->GetProvidedInterfaceInfo(
        providedInterfaceName, providedInterfaceInfo);
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

void mtsDeviceInterfaceProxyClient::SendExecuteCommandVoid(const int commandId) const
{
    if (!IsValidSession) return;

    //Logger->trace("TIClient", ">>>>> SEND: SendExecuteCommandVoid");

    DeviceInterfaceServerProxy->ExecuteCommandVoid(commandId);
}

void mtsDeviceInterfaceProxyClient::SendExecuteCommandWriteSerialized(
    const int commandId, const mtsGenericObject & argument)
{
    if (!IsValidSession) return;

    //Logger->trace("TIClient", ">>>>> SEND: SendExecuteCommandWriteSerialized");

    // Serialization
    std::string serializedData;
    Serialize(argument, serializedData);
    
    DeviceInterfaceServerProxy->ExecuteCommandWriteSerialized(commandId, serializedData);
}

void mtsDeviceInterfaceProxyClient::SendExecuteCommandReadSerialized(
    const int commandId, mtsGenericObject & argument)
{
    if (!IsValidSession) return;

    //Logger->trace("TIClient", ">>>>> SEND: SendExecuteCommandReadSerialized");

    std::string serializedData;

    DeviceInterfaceServerProxy->ExecuteCommandReadSerialized(commandId, serializedData);

    // Deserialization
    DeSerializationBuffer.str("");
    DeSerializationBuffer << serializedData;
    DeSerializer->DeSerialize(argument);
}

void mtsDeviceInterfaceProxyClient::SendExecuteCommandQualifiedReadSerialized(
    const int commandId, const mtsGenericObject & argument1, mtsGenericObject & argument2)
{
    if (!IsValidSession) return;

    //Logger->trace("TIClient", ">>>>> SEND: SendExecuteCommandQualifiedRead");
    
    // Serialization for argument1 (write)
    std::string serializedData;
    Serialize(argument1, serializedData);

    // Deserialization for argument2 (read)
    std::string serializedDataFromServer;

    // Execute the command across networks
    DeviceInterfaceServerProxy->ExecuteCommandQualifiedReadSerialized(
        commandId, serializedData, serializedDataFromServer);

    // Deserialization
    DeSerializationBuffer.str("");
    DeSerializationBuffer << serializedDataFromServer;
    DeSerializer->DeSerialize(argument2);
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
    Sender(new SendThread<DeviceInterfaceClientIPtr>(this)),
    Logger(logger),
    Server(server),
    DeviceInterfaceClient(deviceInterfaceClient)
{
}

void mtsDeviceInterfaceProxyClient::DeviceInterfaceClientI::Start()
{
    DeviceInterfaceClient->GetLogger()->trace(
        "mtsDeviceInterfaceProxyClient", "Send thread starts");

    Sender->start();
}

void mtsDeviceInterfaceProxyClient::DeviceInterfaceClientI::Run()
{
    while (Runnable)
    {
        timedWait(IceUtil::Time::milliSeconds(10));
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

        callbackSenderThread = Sender;
        Sender = 0; // Resolve cyclic dependency.
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
    ::Ice::Int commandId, const ::Ice::Current&)
{
    Logger->trace("TIClient", "<<<<< RECV: ExecuteEventVoid");

    DeviceInterfaceClient->ReceiveExecuteEventVoid(commandId);
}

void mtsDeviceInterfaceProxyClient::DeviceInterfaceClientI::ExecuteEventWriteSerialized(
    ::Ice::Int commandId, const ::std::string& argument, const ::Ice::Current&)
{
    Logger->trace("TIClient", "<<<<< RECV: ExecuteEventWriteSerialized");

    DeviceInterfaceClient->ReceiveExecuteEventWriteSerialized(commandId, argument);
}
