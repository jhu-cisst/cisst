/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsTaskManagerProxyClient.h 142 2009-03-11 23:02:34Z mjung5 $

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

#ifndef _mtsTaskManagerProxyClient_h
#define _mtsTaskManagerProxyClient_h

#include <cisstMultiTask/mtsProxyBaseClient.h>
#include <cisstMultiTask/mtsTaskManagerProxy.h>
#include <cisstMultiTask/mtsDeviceInterface.h>

#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  TODO: add class summary here
*/

class mtsTask;
class mtsTaskManager;

class CISST_EXPORT mtsTaskManagerProxyClient : public mtsProxyBaseClient<mtsTaskManager> {

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);    

public:
    mtsTaskManagerProxyClient(const std::string & propertyFileName, 
                              const std::string & objectIdentity);
    ~mtsTaskManagerProxyClient();

    /*! Entry point to run a proxy. */
    void Start(mtsTaskManager * callingTaskManager);

    /*! Change the proxy state as active. */
    void SetAsActiveProxy() {
        ChangeProxyState(mtsProxyBaseClient<mtsTaskManager>::PROXY_ACTIVE);
    }

    /*! Return true if the current proxy state is active. */
    bool IsActiveProxy(void) const {
        return (ProxyState == mtsProxyBaseClient<mtsTaskManager>::PROXY_ACTIVE);
    }

    /*! End the proxy. */
    void Stop();

protected:
    typedef mtsProxyBaseClient<mtsTaskManager> BaseType;

    /*! Send thread.
        We need a seperate send thread because the bi-directional communication is
        used between proxies. This is the major limitation of using bi-directional 
        communication. (Another approach is to use Glacier2.) */
    class TaskManagerClientI;
    typedef IceUtil::Handle<TaskManagerClientI> TaskManagerClientIPtr;
    TaskManagerClientIPtr Sender;

    /*! Global task manager proxy */
    mtsTaskManagerProxy::TaskManagerServerPrx GlobalTaskManagerProxy;

    /*! Create a proxy object and a send thread. */
    void CreateProxy() {
        GlobalTaskManagerProxy = 
            mtsTaskManagerProxy::TaskManagerServerPrx::checkedCast(ProxyObject);
        if (!GlobalTaskManagerProxy) {
            throw "Invalid proxy";
        }

        Sender = new TaskManagerClientI(IceCommunicator, IceLogger, GlobalTaskManagerProxy, this);
    }

    /*! Start a send thread and wait for shutdown (blocking call). */
    void StartClient();

    /*! Thread runner */
    static void Runner(ThreadArguments<mtsTaskManager> * arguments);

    /*! Clean up thread-related resources. */
    void OnEnd();

    //-------------------------------------------------------------------------
    //  Processing Methods
    //-------------------------------------------------------------------------
public:
    /*! Connect across networks. mtsTaskManager::Connect() calls this method internally. */
    mtsDeviceInterface * GetProvidedInterfaceProxy(
        const std::string & resourceTaskName, const std::string & providedInterfaceName,
        const std::string & userTaskName, const std::string & requiredInterfaceName,
        mtsTask * clientTask);

    //-------------------------------------------------------------------------
    //  Send Methods
    //-------------------------------------------------------------------------
public:
    void SendUpdateTaskManager();

    bool SendAddProvidedInterface(const std::string & newProvidedInterfaceName,
                                  const std::string & adapterName,
                                  const std::string & endpointInfo,
                                  const std::string & communicatorID,
                                  const std::string & taskName);

    bool SendAddRequiredInterface(const std::string & newRequiredInterfaceName,
                                  const std::string & taskName);

    bool SendIsRegisteredProvidedInterface(const std::string & serverTaskName,
                                           const std::string & providedInterfaceName) const;

    bool SendGetProvidedInterfaceAccessInfo(const std::string & serverTaskName,
                                            const std::string & providedInterfaceName,
                                            mtsTaskManagerProxy::ProvidedInterfaceAccessInfo & info) const;

    //-------------------------------------------------------------------------
    //  Definition by mtsTaskManagerProxy.ice
    //-------------------------------------------------------------------------
protected:
    class TaskManagerClientI : public mtsTaskManagerProxy::TaskManagerClient,
                               public IceUtil::Monitor<IceUtil::Mutex>
    {
    private:
        bool Runnable;
        Ice::CommunicatorPtr Communicator;
        
        IceUtil::ThreadPtr SenderThreadPtr;
        Ice::LoggerPtr Logger;
        mtsTaskManagerProxy::TaskManagerServerPrx Server;
        mtsTaskManagerProxyClient * TaskManagerClient;

    public:
        TaskManagerClientI(const Ice::CommunicatorPtr& communicator,                           
                           const Ice::LoggerPtr& logger,
                           const mtsTaskManagerProxy::TaskManagerServerPrx& server,
                           mtsTaskManagerProxyClient * taskManagerClient);

        void Start();
        void Run();
        void Stop();
    };
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsTaskManagerProxyClient)

#endif // _mtsTaskManagerProxyClient_h

