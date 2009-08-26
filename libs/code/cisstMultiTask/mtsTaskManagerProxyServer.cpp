/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsTaskManagerProxyServer.cpp 145 2009-03-18 23:32:40Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2009-03-17

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnAssert.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManagerProxyServer.h>
#include <cisstMultiTask/mtsTaskGlobal.h>

#include <sstream>

CMN_IMPLEMENT_SERVICES(mtsTaskManagerProxyServer);

#define TaskManagerProxyServerLogger(_log) IceLogger->trace("mtsTaskManagerProxyServer", _log)
#define TaskManagerProxyServerLoggerError(_log1, _log2) {\
        std::string s("mtsTaskManagerProxyServer: ");\
        s += _log1; s+= _log2;\
        IceLogger->error(s); }

//-----------------------------------------------------------------------------
//  Inner Class Definition
//-----------------------------------------------------------------------------
bool mtsTaskManagerProxyServer::TaskManagerClient::AddTaskGlobal(mtsTaskGlobal * taskGlobal)
{
    return GlobalTaskMap.AddItem(taskGlobal->GetTaskName(), taskGlobal);
}

mtsTaskGlobal * mtsTaskManagerProxyServer::TaskManagerClient::GetTaskGlobal(const std::string taskName)
{
    return GlobalTaskMap.GetItem(taskName);
}

mtsTaskManagerProxyServer::TaskManagerClient::~TaskManagerClient()
{
    GlobalTaskMap.DeleteAll();
}

//-----------------------------------------------------------------------------
//  Constructor, Destructor, Initializer
//-----------------------------------------------------------------------------
mtsTaskManagerProxyServer::mtsTaskManagerProxyServer(
    const std::string & adapterName, const std::string & endpointInfo,
    const std::string & communicatorID) 
    : BaseType(adapterName, endpointInfo, communicatorID)
{    
}

mtsTaskManagerProxyServer::~mtsTaskManagerProxyServer()
{
    // Add any resource clean-up related methods here, if any.
    TaskManagerMapType::iterator it = TaskManagerMap.begin();
    for (; it != TaskManagerMap.end(); ++it) {
        delete it->second;
    }
}

//-----------------------------------------------------------------------------
//  Proxy Start-up
//-----------------------------------------------------------------------------
void mtsTaskManagerProxyServer::Start(mtsTaskManager * callingTaskManager)
{
    // Initialize Ice object.
    IceInitialize();
    
    if (InitSuccessFlag) {
        // Create a worker thread here and returns immediately.
        ThreadArgumentsInfo.argument = callingTaskManager;
        ThreadArgumentsInfo.proxy = this;
        ThreadArgumentsInfo.Runner = mtsTaskManagerProxyServer::Runner;

        // Note that a worker thread is created but is not yet running.
        WorkerThread.Create<ProxyWorker<mtsTaskManager>, ThreadArguments<mtsTaskManager>*>(
            &ProxyWorkerInfo, &ProxyWorker<mtsTaskManager>::Run, &ThreadArgumentsInfo, "C-PRX");
    }
}

void mtsTaskManagerProxyServer::StartServer()
{
    Sender->Start();

    // This is a blocking call that should run in a different thread.
    IceCommunicator->waitForShutdown();
}

void mtsTaskManagerProxyServer::Runner(ThreadArguments<mtsTaskManager> * arguments)
{
    mtsTaskManagerProxyServer * ProxyServer = 
        dynamic_cast<mtsTaskManagerProxyServer*>(arguments->proxy);
    if (!ProxyServer) {
        CMN_LOG_RUN_ERROR << "mtsTaskManagerProxyServer: Failed to create a proxy server." << std::endl;
        return;
    }
    
    ProxyServer->GetLogger()->trace("mtsTaskManagerProxyServer", "Proxy server starts.....");    

    try {
        ProxyServer->SetAsActiveProxy();
        ProxyServer->StartServer();
    } catch (const Ice::Exception& e) {
        std::string error("mtsTaskManagerProxyServer: ");
        error += e.what();
        ProxyServer->GetLogger()->error(error);
    } catch (const char * msg) {
        std::string error("mtsTaskManagerProxyServer: ");
        error += msg;
        ProxyServer->GetLogger()->error(error);
    }

    ProxyServer->GetLogger()->trace("mtsTaskManagerProxyServer", "Proxy server terminates.....");

    ProxyServer->OnEnd();
}

void mtsTaskManagerProxyServer::Stop()
{
    OnEnd();
}

void mtsTaskManagerProxyServer::OnEnd()
{
    BaseType::OnEnd();

    Sender->Stop();
}

//-----------------------------------------------------------------------------
//  Task Manager Processing
//-----------------------------------------------------------------------------
mtsTaskManagerProxyServer::TaskManagerClient * mtsTaskManagerProxyServer::GetTaskManager(const TaskManagerIDType & taskManagerID)
{
    TaskManagerMapType::iterator it = TaskManagerMap.find(taskManagerID);

    if (it == TaskManagerMap.end()) {
        return 0;
    } else {
        return it->second;
    }
}

mtsTaskManagerProxyServer::TaskManagerClient * mtsTaskManagerProxyServer::GetTaskManagerByConnectionID(const ConnectionIDType & connectionID)
{
    ConnectionIDMapType::const_iterator it = ConnectionIDMap.find(connectionID);

    if (it == ConnectionIDMap.end()) {
        return 0;
    } else {
        return GetTaskManager(it->second);
    }
}

bool mtsTaskManagerProxyServer::RemoveTaskManager(const TaskManagerIDType & taskManagerID)
{
    TaskManagerMapType::iterator it = TaskManagerMap.find(taskManagerID);

    if (it == TaskManagerMap.end()) {
        TaskManagerProxyServerLoggerError("[RemoveTaskManager] Cannot find a task manager: ", taskManagerID);
        return false;
    } else {
        delete it->second;
        TaskManagerMap.erase(it);

        TaskManagerProxyServerLogger("[RemoveTaskManager] Removed the task manager: " + taskManagerID);
    }

    return true;
}

bool mtsTaskManagerProxyServer::RemoveTaskManagerByConnectionID(const ConnectionIDType & connectionID)
{
    ConnectionIDMapType::iterator it = ConnectionIDMap.find(connectionID);

    if (it == ConnectionIDMap.end()) {
        TaskManagerProxyServerLoggerError("[RemoveTaskManagerByConnectionID] Cannot find a task manager: ", connectionID);
        return false;
    } else {
        return RemoveTaskManager(it->second);
    }
}

//mtsTaskGlobal * mtsTaskManagerProxyServer::GetTask(const std::string & taskName)
//{
//    GlobalTaskMapType::iterator it = GlobalTaskMap.find(taskName);
//    if (it == GlobalTaskMap.end()) {
//        return 0;
//    } else {
//        return &it->second;
//    }
//}

//-----------------------------------------------------------------------------
//  Task Manager Proxy Server Processing
//-----------------------------------------------------------------------------
void mtsTaskManagerProxyServer::ReceiveAddClient(
    const ConnectionIDType & connectionID, const TaskManagerClientProxyType & clientProxy)
{
    TaskManagerClient * newTaskManagerClient = new TaskManagerClient(connectionID, clientProxy);
    
    TaskManagerMap.insert(make_pair(connectionID, newTaskManagerClient));

    TaskManagerProxyServerLogger("Added the task manager with connection id: " + connectionID);
}

bool mtsTaskManagerProxyServer::ReceiveUpdateTaskManagerClient(
    const ConnectionIDType & connectionID, const mtsTaskManagerProxy::TaskList& localTaskInfo)
{
    const TaskManagerIDType taskManagerID = localTaskInfo.taskManagerID;

    ConnectionIDMap.insert(make_pair(taskManagerID, connectionID));
    
    TaskManagerClient * taskManagerClient = GetTaskManagerByConnectionID(connectionID);
    if (!taskManagerClient) {
        TaskManagerProxyServerLoggerError("[UpdateTaskManagerClient] Cannot find a task manager: ", connectionID);
        return false;
    }

    TaskManagerProxyServerLogger("Adding tasks from connection id: " + connectionID);

    bool success = true;
    std::string taskName;
    mtsTaskManagerProxy::TaskNameSeq::const_iterator it = localTaskInfo.taskNames.begin();    
    for (; it != localTaskInfo.taskNames.end(); ++it) {
        taskName = *it;
        mtsTaskGlobal * newTask = new mtsTaskGlobal(taskName, taskManagerID);

        if (!taskManagerClient->AddTaskGlobal(newTask)) {
            TaskManagerProxyServerLoggerError("[UpdateTaskManagerClient] Add failed: ", taskName);
            success = false;
        } else {
            if (!TaskManagerMapByTaskName.AddItem(taskName, taskManagerClient)) {
                TaskManagerProxyServerLoggerError("[UpdateTaskManagerClient] Add failed (duplicate task name): ", taskName);
                success = false;
            } else {
                TaskManagerProxyServerLogger("Successfully added a task: " + taskName);
            }
        }

        TaskManagerProxyServerLogger(newTask->ShowTaskInfo());
    }

    return success;
}

void mtsTaskManagerProxyServer::OnClose()
{
    //
    //  TODO: Add OnClose() event handler.
    //

    // remove from TaskManagerMapByTaskName
    // remove from TaskManagerClient
    //RemoveTaskManagerByConnectionID();
}

bool mtsTaskManagerProxyServer::ReceiveAddProvidedInterface(
    const ConnectionIDType & connectionID, 
    const mtsTaskManagerProxy::ProvidedInterfaceAccessInfo & providedInterfaceAccessInfo)
{
    TaskManagerClient * taskManagerClient = GetTaskManagerByConnectionID(connectionID);
    if (!taskManagerClient) {
        TaskManagerProxyServerLoggerError("[AddProvidedInterface] Cannot find a task manager: ", connectionID);
        return false;
    }

    const std::string taskName = providedInterfaceAccessInfo.taskName;
    const std::string interfaceName = providedInterfaceAccessInfo.interfaceName;

    mtsTaskGlobal * taskGlobal = taskManagerClient->GetTaskGlobal(taskName);
    if (!taskGlobal) {
        TaskManagerProxyServerLoggerError("[AddProvidedInterface] Cannot find a global task: ", taskName);
        return false;
    }

    bool success = taskGlobal->AddProvidedInterface(providedInterfaceAccessInfo);
    if (success) {
        TaskManagerProxyServerLogger(
            "Successfully added a provided interface: " + 
            interfaceName + " @ " + taskName);
        TaskManagerProxyServerLogger(taskGlobal->ShowTaskInfo());        
    } else {
        TaskManagerProxyServerLoggerError(
            "[AddProvidedInterface] Failed to add a provided interface: ", 
            interfaceName + " @ " + taskName);
    }

    return success;
}

bool mtsTaskManagerProxyServer::ReceiveAddRequiredInterface(
    const ConnectionIDType & connectionID,
    const mtsTaskManagerProxy::RequiredInterfaceAccessInfo & requiredInterfaceAccessInfo)
{
    TaskManagerClient * taskManagerClient = GetTaskManagerByConnectionID(connectionID);
    if (!taskManagerClient) {
        TaskManagerProxyServerLoggerError("[AddRequiredInterface] Cannot find a task manager: ", connectionID);
        return false;
    }

    mtsTaskGlobal * taskGlobal = taskManagerClient->GetTaskGlobal(requiredInterfaceAccessInfo.taskName);
    if (!taskGlobal) {
        TaskManagerProxyServerLoggerError("[AddRequiredInterface] Cannot find a global task: ", requiredInterfaceAccessInfo.taskName);
        return false;
    }

    bool success = taskGlobal->AddRequiredInterface(requiredInterfaceAccessInfo);
    if (success) {
        TaskManagerProxyServerLogger(
            "Successfully added a required interface: " + 
            requiredInterfaceAccessInfo.interfaceName + " @ " +
            requiredInterfaceAccessInfo.taskName);
        TaskManagerProxyServerLogger(taskGlobal->ShowTaskInfo());        
    } else {
        TaskManagerProxyServerLoggerError(
            "[AddRequiredInterface] Failed to add a required interface: ",
            requiredInterfaceAccessInfo.interfaceName + " @ " +
            requiredInterfaceAccessInfo.taskName);
    }

    return success;
}

bool mtsTaskManagerProxyServer::ReceiveIsRegisteredProvidedInterface(
    const ConnectionIDType & connectionID,
    const std::string & taskName, const std::string & providedInterfaceName)
{
    TaskManagerClient * taskManagerClient = TaskManagerMapByTaskName.GetItem(taskName);
    if (!taskManagerClient) {
        TaskManagerProxyServerLoggerError("[IsRegisteredProvidedInterface] Cannot find a task manager with the task: ", taskName);
        return false;
    }

    mtsTaskGlobal * taskGlobal = taskManagerClient->GetTaskGlobal(taskName);
    if (!taskGlobal) {
        TaskManagerProxyServerLoggerError("[IsRegisteredProvidedInterface] Cannot find a global task: ", taskName);
        return false;
    }

    return taskGlobal->IsRegisteredProvidedInterface(providedInterfaceName);
}

bool mtsTaskManagerProxyServer::ReceiveGetProvidedInterfaceAccessInfo(
    const ConnectionIDType & connectionID,
    const std::string & taskName, const std::string & providedInterfaceName,
    mtsTaskManagerProxy::ProvidedInterfaceAccessInfo & info)
{
    TaskManagerClient * taskManagerClient = TaskManagerMapByTaskName.GetItem(taskName);
    if (!taskManagerClient) {
        TaskManagerProxyServerLoggerError("[GetProvidedInterfaceAccessInfo] Cannot find a task manager: ", connectionID);
        return false;
    }

    mtsTaskGlobal * taskGlobal = taskManagerClient->GetTaskGlobal(taskName);
    if (!taskGlobal) {
        TaskManagerProxyServerLoggerError("[GetProvidedInterfaceAccessInfo] Cannot find a global task: ", taskName);
        return false;
    }

    return taskGlobal->GetProvidedInterfaceAccessInfo(providedInterfaceName, info);
}

void mtsTaskManagerProxyServer::ReceiveNotifyInterfaceConnectionResult(
    const ConnectionIDType & connectionID,
    const bool isServerTask, const bool isSuccess,
    const std::string & userTaskName,     const std::string & requiredInterfaceName,
    const std::string & resourceTaskName, const std::string & providedInterfaceName)
{
    if (!isSuccess) {
        std::stringstream buf;
        buf << (isServerTask ? "Server task: " : "Client task: ")
            << resourceTaskName << " : " << providedInterfaceName << " - "
            << userTaskName << " : " << requiredInterfaceName;

        TaskManagerProxyServerLoggerError("[NotifyInterfaceConnectionResult] failed to connect - ", buf.str());
        return;
    } else {
        std::stringstream buf;
        buf << "[NotifyInterfaceConnectionResult] succeeded to connect - "
            << (isServerTask ? "Server task: " : "Client task: ")
            << resourceTaskName << " : " << providedInterfaceName << " - "
            << userTaskName << " : " << requiredInterfaceName;

        TaskManagerProxyServerLogger(buf.str());
    }

    //
    // TODO: IMPLEMENT ME
    //
}

//-------------------------------------------------------------------------
//  Send Methods
//-------------------------------------------------------------------------
/*
bool mtsTaskManagerProxyServer::SendConnectServerSide(
    TaskManagerClient * taskManagerWithServerTask,
    const std::string & userTaskName,     const std::string & requiredInterfaceName,
    const std::string & resourceTaskName, const std::string & providedInterfaceName)
{
    GetLogger()->trace("TMServer", ">>>>> SEND: ConnectServerSide: " 
            + resourceTaskName + " : " + providedInterfaceName + " - "
            + userTaskName + " : " + requiredInterfaceName);

    return taskManagerWithServerTask->GetClientProxy()->ConnectServerSide(
        userTaskName, requiredInterfaceName, resourceTaskName, providedInterfaceName);
}
*/

//-------------------------------------------------------------------------
//  Definition by mtsTaskManagerProxy.ice
//-------------------------------------------------------------------------
mtsTaskManagerProxyServer
::TaskManagerServerI::TaskManagerServerI(const Ice::CommunicatorPtr& communicator,
                                         const Ice::LoggerPtr& logger,
                                         mtsTaskManagerProxyServer * taskManagerServer):
    Communicator(communicator),
    Logger(logger),
    TaskManagerServer(taskManagerServer),
    Runnable(true),
    SenderThreadPtr(new SenderThread<TaskManagerServerIPtr>(this))
{
}

void mtsTaskManagerProxyServer::TaskManagerServerI::Start()
{
    //CMN_LOG_RUN_VERBOSE << "TaskManagerProxyServer: Send thread starts." << std::endl;
    TaskManagerServer->GetLogger()->trace(
        "mtsTaskManagerProxyServer", "Send thread starts");

    SenderThreadPtr->start();
}

void mtsTaskManagerProxyServer::TaskManagerServerI::Run()
{
#ifdef _COMMUNICATION_TEST_
    int num = 0;
#endif

    while(Runnable) {
        osaSleep(10 * cmn_ms);

#ifdef _COMMUNICATION_TEST_
        if(!clients.empty())
        {
            ++num;
            for(std::set<mtsTaskManagerProxy::TaskManagerClientPrx>::iterator p 
                = clients.begin(); p != clients.end(); ++p)
            {
                try
                {
                    std::cout << "server sends: " << num << std::endl;
                }
                catch(const IceUtil::Exception& ex)
                {
                    std::cerr << "removing client `" << Communicator->identityToString((*p)->ice_getIdentity()) << "':\n"
                        << ex << std::endl;

                    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
                    _clients.erase(*p);
                }
            }
        }
#endif
    }
}

void mtsTaskManagerProxyServer::TaskManagerServerI::Stop()
{
    if (!TaskManagerServer->IsActiveProxy()) return;

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
//  Task Manager Proxy Server Implementation
//-----------------------------------------------------------------------------
void mtsTaskManagerProxyServer::TaskManagerServerI::AddClient(
    const ::Ice::Identity & identity, const ::Ice::Current& current)
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);

    Logger->trace("TMServer", "<<<<< RECV: AddClient: " + Communicator->identityToString(identity));

    mtsTaskManagerProxy::TaskManagerClientPrx clientProxy = 
        mtsTaskManagerProxy::TaskManagerClientPrx::uncheckedCast(current.con->createProxy(identity));
    
    TaskManagerServer->ReceiveAddClient(Communicator->identityToString(identity), clientProxy);
}

void mtsTaskManagerProxyServer::TaskManagerServerI::Shutdown(const ::Ice::Current& current)
{
    Logger->trace("TMServer", "<<<<< RECV: Shutdown");

    // Set as true to represent that this connection (session) is going to be closed.
    // After this flag is set, no message is allowed to be sent to a server.
    TaskManagerServer->ShutdownSession(current);
}

void mtsTaskManagerProxyServer::TaskManagerServerI::UpdateTaskManager(
    const mtsTaskManagerProxy::TaskList& localTaskInfo, const ::Ice::Current& current)
{
    Logger->trace("TMServer", "<<<<< RECV: UpdateTaskManager: " + localTaskInfo.taskManagerID);

    //!!!!!!!!!!!!!
    // MJUNG: don't know why this doesn't work. FIXME later.
    //Ice::ImplicitContextPtr a = Communicator->getImplicitContext();
    //std::string s = a->get(CONNECTION_ID);
    // The following line does work.
    //std::string ss = current.ctx.find(CONNECTION_ID)->second;

    TaskManagerServer->ReceiveUpdateTaskManagerClient(
        //Communicator->getImplicitContext()->get(CONNECTION_ID), localTaskInfo);
        current.ctx.find(CONNECTION_ID)->second, localTaskInfo);
}

bool mtsTaskManagerProxyServer::TaskManagerServerI::AddProvidedInterface(
    const mtsTaskManagerProxy::ProvidedInterfaceAccessInfo & providedInterfaceAccessInfo,
    const ::Ice::Current & current)
{
    Logger->trace("TMServer", "<<<<< RECV: AddProvidedInterface: " 
        + providedInterfaceAccessInfo.taskName + ", " + providedInterfaceAccessInfo.interfaceName);

    return TaskManagerServer->ReceiveAddProvidedInterface(
        //Communicator->getImplicitContext()->get(CONNECTION_ID), providedInterfaceInfo);
        current.ctx.find(CONNECTION_ID)->second, providedInterfaceAccessInfo);
}

bool mtsTaskManagerProxyServer::TaskManagerServerI::AddRequiredInterface(
    const mtsTaskManagerProxy::RequiredInterfaceAccessInfo & requiredInterfaceAccessInfo,
    const ::Ice::Current & current)
{
    Logger->trace("TMServer", "<<<<< RECV: AddRequiredInterface: " 
        + requiredInterfaceAccessInfo.taskName + ", " + requiredInterfaceAccessInfo.interfaceName);

    return TaskManagerServer->ReceiveAddRequiredInterface(
        //Communicator->getImplicitContext()->get(CONNECTION_ID), requiredInterfaceAccessInfo);
        current.ctx.find(CONNECTION_ID)->second, requiredInterfaceAccessInfo);
}

bool mtsTaskManagerProxyServer::TaskManagerServerI::IsRegisteredProvidedInterface(
    const ::std::string & taskName, const ::std::string & providedInterfaceName,
    const ::Ice::Current & current) const
{
    Logger->trace("TMServer", "<<<<< RECV: IsRegisteredProvidedInterface: " 
        + taskName + ", " + providedInterfaceName);

    return TaskManagerServer->ReceiveIsRegisteredProvidedInterface(
        //Communicator->getImplicitContext()->get(CONNECTION_ID), taskName, providedInterfaceName);
        current.ctx.find(CONNECTION_ID)->second, taskName, providedInterfaceName);
}

bool mtsTaskManagerProxyServer::TaskManagerServerI::GetProvidedInterfaceAccessInfo(
    const ::std::string & taskName, const ::std::string & providedInterfaceName,
    mtsTaskManagerProxy::ProvidedInterfaceAccessInfo & info, const ::Ice::Current & current) const
{
    Logger->trace("TMServer", "<<<<< RECV: GetProvidedInterfaceAccessInfo: " 
        + taskName + ", " + providedInterfaceName);

    return TaskManagerServer->ReceiveGetProvidedInterfaceAccessInfo(
        //Communicator->getImplicitContext()->get(CONNECTION_ID), taskName, providedInterfaceName, info);
        current.ctx.find(CONNECTION_ID)->second, taskName, providedInterfaceName, info);
}

void mtsTaskManagerProxyServer::TaskManagerServerI::NotifyInterfaceConnectionResult(
    bool isServerTask, bool isSuccess, 
    const std::string & userTaskName,     const std::string & requiredInterfaceName, 
    const std::string & resourceTaskName, const std::string & providedInterfaceName, 
    const ::Ice::Current & current)
{
    Logger->trace("TMServer", "<<<<< RECV: NotifyInterfaceConnectionResult: " 
        + resourceTaskName + " : " + providedInterfaceName + " - "
        + userTaskName + " : " + requiredInterfaceName);

    TaskManagerServer->ReceiveNotifyInterfaceConnectionResult(
        //Communicator->getImplicitContext()->get(CONNECTION_ID),
        current.ctx.find(CONNECTION_ID)->second,
        isServerTask, isSuccess, 
        userTaskName, requiredInterfaceName, 
        resourceTaskName, providedInterfaceName);
}
