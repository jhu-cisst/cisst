/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsDeviceInterfaceProxyClient.h 142 2009-03-11 23:02:34Z mjung5 $

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

#ifndef _mtsDeviceInterfaceProxyClient_h
#define _mtsDeviceInterfaceProxyClient_h

//#include <cisstOSAbstraction/osaMutex.h>
#include <cisstMultiTask/mtsDeviceInterface.h>
#include <cisstMultiTask/mtsProxyBaseClient.h>
#include <cisstMultiTask/mtsDeviceInterfaceProxy.h>

#include <cisstMultiTask/mtsExport.h>

#include <map>

/*!
  \ingroup cisstMultiTask

  TODO: add class summary here
*/

// TODO: ADD the following line in the forward declaration.h (???)
class mtsProxySerializer;

class CISST_EXPORT mtsDeviceInterfaceProxyClient : public mtsProxyBaseClient<mtsTask> {
    
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    mtsDeviceInterfaceProxyClient(const std::string & propertyFileName, 
                                  const std::string & identityName);
    ~mtsDeviceInterfaceProxyClient();

    /*! Set a client task connected to this proxy client. Currently, this client task 
        can have the same number of the provided interfaces provided by the server task,
        which means only 1:1 connection between a provided interface and a required
        interface is allowed at this moment. */
    void SetConnectedTask(mtsTask * clientTask) { ConnectedTask = clientTask; }

    /*! Entry point to run a proxy. */
    void Start(mtsTask * callingTask);

    /*! Change the proxy state as active. */
    void SetAsActiveProxy(void) {
        ChangeProxyState(mtsProxyBaseClient<mtsTask>::PROXY_ACTIVE);
    }

    /*! Return true if the current proxy state is active. */
    bool IsActiveProxy(void) const {
        return (ProxyState == mtsProxyBaseClient<mtsTask>::PROXY_ACTIVE);
    }

    void ShutdownSession(void) {
        mtsProxyBaseClient<mtsTask>::ShutdownSession();
    }

    /*! Stop the proxy. */
    void Stop(void);

protected:
    /*! Typedef for base type. */
    typedef mtsProxyBaseClient<mtsTask> BaseType;

    /*! Typedef for server proxy. */
    typedef mtsDeviceInterfaceProxy::DeviceInterfaceServerPrx DeviceInterfaceServerProxyType;

    /*! Typedef for per-command proxy serializer. */
    typedef std::map<CommandIDType, mtsProxySerializer *> PerCommandSerializerMapType;

    /*! Pointer to the task connected. */
    mtsTask * ConnectedTask;

    /*! Mutex to prevent the ICE library from calling a callback function before 
        the previous function call has not completed yet. */
    //osaMutex CommandExecution;

    /*! Connected server object */
    DeviceInterfaceServerProxyType DeviceInterfaceServerProxy;

    /*! Per-command proxy serializer container. */
    PerCommandSerializerMapType PerCommandSerializerMap;
    
    //-------------------------------------------------------------------------
    //  Processing Methods
    //-------------------------------------------------------------------------
    /*! Send thread set up. */
    class DeviceInterfaceClientI;
    typedef IceUtil::Handle<DeviceInterfaceClientI> DeviceInterfaceClientIPtr;
    DeviceInterfaceClientIPtr Sender;

    /*! Create a proxy object and a send thread. */
    void CreateProxy() {
        DeviceInterfaceServerProxy = 
            mtsDeviceInterfaceProxy::DeviceInterfaceServerPrx::checkedCast(ProxyObject);
        if (!DeviceInterfaceServerProxy) {
            throw "Invalid proxy";
        }

        Sender = new DeviceInterfaceClientI(IceCommunicator, IceLogger, DeviceInterfaceServerProxy, this);
    }

    /*! Start a send thread and wait for shutdown (blocking call). */
    void StartClient();

    /*! Thread runner */
    static void Runner(ThreadArguments<mtsTask> * arguments);

    /*! Clean up thread-related resources. */
    void OnEnd();

public:
    //-------------------------------------------------------------------------
    //  Method to register per-command serializer
    //-------------------------------------------------------------------------
    bool AddPerCommandSerializer(
        const CommandIDType commandId, mtsProxySerializer * argumentSerializer);

    //-------------------------------------------------------------------------
    //  Methods to Receive and Process Events (Server -> Client)
    //-------------------------------------------------------------------------
    void ReceiveExecuteEventVoid(const CommandIDType commandId);
    void ReceiveExecuteEventWriteSerialized(const CommandIDType commandId, const std::string argument);

    //-------------------------------------------------------------------------
    //  Methods to Send Events (Client -> Server)
    //-------------------------------------------------------------------------
    bool SendGetProvidedInterfaceInfo(
        const std::string & providedInterfaceName,
        mtsDeviceInterfaceProxy::ProvidedInterfaceInfo & providedInterfaceInfo);

    bool SendCreateClientProxies(
        const std::string & userTaskName, const std::string & requiredInterfaceName,
        const std::string & resourceTaskName, const std::string & providedInterfaceName);

    bool SendConnectServerSide(
        const std::string & userTaskName, const std::string & requiredInterfaceName,
        const std::string & resourceTaskName, const std::string & providedInterfaceName);

    bool SendUpdateEventHandlerId(
        const std::string & clientTaskProxyName,
        const mtsDeviceInterfaceProxy::ListsOfEventGeneratorsRegistered & eventGeneratorProxies);

    void SendGetCommandId(
        const std::string & clientTaskProxyName,
        mtsDeviceInterfaceProxy::FunctionProxySet & functionProxies);

    void SendExecuteCommandVoid(const CommandIDType commandId) const;
    void SendExecuteCommandWriteSerialized(const CommandIDType commandId, const mtsGenericObject & argument);
    void SendExecuteCommandReadSerialized(const CommandIDType commandId, mtsGenericObject & argument);
    void SendExecuteCommandQualifiedReadSerialized(
        const CommandIDType commandId, const mtsGenericObject & argument1, mtsGenericObject & argument2);

    //-------------------------------------------------------------------------
    //  Definition by mtsDeviceInterfaceProxy.ice
    //-------------------------------------------------------------------------
protected:
    class DeviceInterfaceClientI : public mtsDeviceInterfaceProxy::DeviceInterfaceClient,
                                   public IceUtil::Monitor<IceUtil::Mutex>
    {
    private:
        bool Runnable;
        Ice::CommunicatorPtr Communicator;        
        IceUtil::ThreadPtr SenderThreadPtr;
        Ice::LoggerPtr Logger;
        mtsDeviceInterfaceProxy::DeviceInterfaceServerPrx Server;
        mtsDeviceInterfaceProxyClient * DeviceInterfaceClient;

    public:
        DeviceInterfaceClientI(const Ice::CommunicatorPtr& communicator,                           
                               const Ice::LoggerPtr& logger,
                               const mtsDeviceInterfaceProxy::DeviceInterfaceServerPrx& server,
                               mtsDeviceInterfaceProxyClient * deviceInterfaceClient);

        void Start();
        void Run();
        void Stop();

        // Server -> Client
        void ExecuteEventVoid(::Ice::IceCommandIDType, const ::Ice::Current&);
        void ExecuteEventWriteSerialized(::Ice::IceCommandIDType, const ::std::string&, const ::Ice::Current&);
    };
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsDeviceInterfaceProxyClient)

#endif // _mtsDeviceInterfaceProxyClient_h
