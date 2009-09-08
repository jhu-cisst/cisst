/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsTaskManagerProxyServer.h 142 2009-03-11 23:02:34Z mjung5 $

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

#ifndef _mtsTaskManagerProxyServer_h
#define _mtsTaskManagerProxyServer_h

#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsTaskManagerProxy.h>
#include <cisstMultiTask/mtsProxyBaseServer.h>

#include <cisstMultiTask/mtsExport.h>

#include <sstream>

/*!
  \ingroup cisstMultiTask

  TODO: add class summary here
*/
class mtsTaskGlobal;

class CISST_EXPORT mtsTaskManagerProxyServer : public mtsProxyBaseServer<mtsTaskManager> {
    
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    /*! Typedef for proxy connection id and task manager id. */
    // MJUNG: ConnectionIDType is set as Ice::Identity which can be transformed to 
    // std::string by Communicator->identityToString(Ice::Identity)
    // (see http://www.zeroc.com/doc/Ice-3.3.1/reference/Ice/Identity.html)
    typedef std::string ConnectionIDType;
    typedef std::string TaskManagerIDType;

    mtsTaskManagerProxyServer(const std::string & adapterName,
                              const std::string & endpointInfo,
                              const std::string & communicatorID);        
    ~mtsTaskManagerProxyServer();

    /*! Entry point to run a proxy. */
    void Start(mtsTaskManager * callingTaskManager);

    /*! Change the proxy state as active. */
    void SetAsActiveProxy(void) {
        ChangeProxyState(mtsProxyBaseServer<mtsTaskManager>::PROXY_ACTIVE);
    }

    /*! Return true if the current proxy state is active. */
    bool IsActiveProxy(void) const {
        return (ProxyState == mtsProxyBaseServer<mtsTaskManager>::PROXY_ACTIVE);
    }

    void ShutdownSession(const Ice::Current & current) {
        current.adapter->getCommunicator()->shutdown();
        mtsProxyBaseServer<mtsTaskManager>::ShutdownSession();
    }

    /*! End the proxy. */
    void Stop();

protected:
    /*! Typedef for base type. */
    typedef mtsProxyBaseServer<mtsTaskManager> BaseType;

    /*! Typedef for client proxy. */
    typedef mtsTaskManagerProxy::TaskManagerClientPrx TaskManagerClientProxyType;

    class TaskManagerClient {
        const ConnectionIDType ConnectionID;
        const TaskManagerClientProxyType ClientProxy;

        /*! Map to manage tasks by task name. */
        typedef cmnNamedMap<mtsTaskGlobal> GlobalTaskMapType;
        GlobalTaskMapType GlobalTaskMap;

        TaskManagerIDType TaskManagerID;

    public:
        TaskManagerClient(const ConnectionIDType & connectionID, 
                          const TaskManagerClientProxyType & clientProxy)
            : ConnectionID(connectionID), ClientProxy(clientProxy)
        {}

        ~TaskManagerClient();

        bool AddTaskGlobal(mtsTaskGlobal * taskGlobal);
        mtsTaskGlobal * GetTaskGlobal(const std::string taskName);

        //---------------------------------------------------------------------
        // Getters and Setters
        //---------------------------------------------------------------------
        ConnectionIDType GetConnectionID() const {
            return ConnectionID;
        }

        const TaskManagerClientProxyType GetClientProxy() const {
            return ClientProxy;
        }

        TaskManagerIDType GetTaskManagerID() const {
            return TaskManagerID;
        }

        void SetTaskManagerID(const TaskManagerIDType & taskManagerID) {
            TaskManagerID = taskManagerID;
        }
    };

    /*! Typedef for task manager id and proxy connection id (Ice::Identity). */
    typedef std::map<TaskManagerIDType, ConnectionIDType> ConnectionIDMapType;
    ConnectionIDMapType ConnectionIDMap;

    /*! Typedef for proxy connection id and task manager client pointer. */
    typedef std::map<ConnectionIDType, TaskManagerClient*> TaskManagerMapType;
    TaskManagerMapType TaskManagerMap;

    /*! Typedef to directly access TaskManagerClient objects with a key of a task name. 
        This assumes that a task name should be unique across networks. */
    typedef cmnNamedMap<TaskManagerClient> TaskManagerMapByTaskNameType;
    TaskManagerMapByTaskNameType TaskManagerMapByTaskName;
    
    /*! Definitions for send thread */
    class TaskManagerServerI;
    typedef IceUtil::Handle<TaskManagerServerI> TaskManagerServerIPtr;
    TaskManagerServerIPtr Sender;
    
    //-------------------------------------------------------------------------
    //  Processing Methods
    //-------------------------------------------------------------------------
    /*! Check if a specific task manager exists. */
    TaskManagerClient * GetTaskManager(const TaskManagerIDType & taskManagerID);
    TaskManagerClient * GetTaskManagerByConnectionID(const ConnectionIDType & connectionID);

    /*! Remove a specific task manager. */
    bool RemoveTaskManager(const TaskManagerIDType & taskManagerID);
    bool RemoveTaskManagerByConnectionID(const ConnectionIDType & connectionID);

    //-------------------------------------------------------------------------
    //  Methods to Receive and Process Events
    //-------------------------------------------------------------------------
    /*! When a new client connects, add it to the client management list. */
    void ReceiveAddClient(const ConnectionIDType & connectionID, 
                          const TaskManagerClientProxyType & clientProxy);

    /*! Update the information on the newly connected task manager. */
    bool ReceiveUpdateTaskManagerClient(const ConnectionIDType & connectionID,
                                        const ::mtsTaskManagerProxy::TaskList& localTaskInfo);

    /*! Add a new provided interface. */
    bool ReceiveAddProvidedInterface(
        const ConnectionIDType & connectionID,
        const mtsTaskManagerProxy::ProvidedInterfaceAccessInfo & providedInterfaceAccessInfo);

    /*! Add a new required interface. */
    bool ReceiveAddRequiredInterface(
        const ConnectionIDType & connectionID,
        const ::mtsTaskManagerProxy::RequiredInterfaceAccessInfo & requiredInterfaceAccessInfo);

    /*! Check if the provided interface has been registered before. */
    bool ReceiveIsRegisteredProvidedInterface(
        const ConnectionIDType & connectionID,
        const std::string & taskName, const std::string & providedInterfaceName);

    /*! Get the information about the provided interface. */
    bool ReceiveGetProvidedInterfaceAccessInfo(
        const ConnectionIDType & connectionID,
        const std::string & taskName, const std::string & providedInterfaceName,
        mtsTaskManagerProxy::ProvidedInterfaceAccessInfo & info);

    /*! Inform the global task manager of the fact that connect() succeeded. */
    void ReceiveNotifyInterfaceConnectionResult(
        const ConnectionIDType & connectionID,
        const bool isServerTask, const bool isSuccess,
        const std::string & userTaskName,     const std::string & requiredInterfaceName,
        const std::string & resourceTaskName, const std::string & providedInterfaceName);

    //-------------------------------------------------------------------------
    //  Proxy Implementation
    //-------------------------------------------------------------------------
    /*! Create a servant which serves TaskManager clients. */
    Ice::ObjectPtr CreateServant() {
        Sender = new TaskManagerServerI(IceCommunicator, IceLogger, this);
        return Sender;
    }
    
    /*! Start a send thread and wait for shutdown (blocking call). */
    void StartServer();

    /*! Thread runner */
    static void Runner(ThreadArguments<mtsTaskManager> * arguments);

    /*! Clean up thread-related resources. */
    void OnEnd();

    /*! Resource clean-up when a client disconnects or is disconnected.
        MJUNG: Right now, this method is not called because we don't detect the 
        closure of the connection, which should be soon to be fixed. */
    void OnClose();
    
    //-------------------------------------------------------------------------
    //  Definition by mtsTaskManagerProxy.ice
    //-------------------------------------------------------------------------
    class TaskManagerServerI : public mtsTaskManagerProxy::TaskManagerServer,
                               public IceUtil::Monitor<IceUtil::Mutex> 
    {
    private:
        Ice::CommunicatorPtr Communicator;
        Ice::LoggerPtr Logger;
        mtsTaskManagerProxyServer * TaskManagerServer;
        bool Runnable;        
        IceUtil::ThreadPtr SenderThreadPtr;

    public:
        TaskManagerServerI(const Ice::CommunicatorPtr& communicator, 
                           const Ice::LoggerPtr& logger,
                           mtsTaskManagerProxyServer * taskManagerServer);

        void Start();
        void Run();
        void Stop();

        void AddClient(const Ice::Identity&, const Ice::Current&);
        void Shutdown(const ::Ice::Current&);

        void UpdateTaskManager(const mtsTaskManagerProxy::TaskList&, const Ice::Current&);
        bool AddProvidedInterface(const mtsTaskManagerProxy::ProvidedInterfaceAccessInfo&, const Ice::Current&);
        bool AddRequiredInterface(const mtsTaskManagerProxy::RequiredInterfaceAccessInfo&, const Ice::Current&);
        bool IsRegisteredProvidedInterface(const std::string&, const ::std::string&, const Ice::Current&) const;
        bool GetProvidedInterfaceAccessInfo(const std::string&, const std::string&, mtsTaskManagerProxy::ProvidedInterfaceAccessInfo & info, const Ice::Current&) const;
        void NotifyInterfaceConnectionResult(bool, bool, const ::std::string&, const ::std::string&, const ::std::string&, const ::std::string&, const Ice::Current&);
    };
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsTaskManagerProxyServer)

#endif // _mtsTaskManagerProxyServer_h

