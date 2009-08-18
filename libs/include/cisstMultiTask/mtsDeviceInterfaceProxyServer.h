/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsDeviceInterfaceProxyServer.h 142 2009-03-11 23:02:34Z mjung5 $

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

#ifndef _mtsDeviceInterfaceProxyServer_h
#define _mtsDeviceInterfaceProxyServer_h

#include <cisstCommon/cmnDeSerializer.h>
#include <cisstMultiTask/mtsDeviceInterface.h>
#include <cisstMultiTask/mtsDeviceInterfaceProxy.h>
#include <cisstMultiTask/mtsProxyBaseServer.h>

#include <cisstMultiTask/mtsExport.h>

//#include <string>

/*!
  \ingroup cisstMultiTask

  TODO: add class summary here
*/
class mtsTask;
class mtsCommandVoidProxy;
class mtsCommandWriteProxy;

class CISST_EXPORT mtsDeviceInterfaceProxyServer : public mtsProxyBaseServer<mtsTask> {
    
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    mtsDeviceInterfaceProxyServer(const std::string& adapterName,
                                  const std::string& endpointInfo,
                                  const std::string& communicatorID);
    ~mtsDeviceInterfaceProxyServer();
    
    /*! Entry point to run a proxy. */
    void Start(mtsTask * callingTask);

    /*! Change the proxy state as active. */
    void SetAsActiveProxy() {
        ChangeProxyState(mtsProxyBaseServer<mtsTask>::PROXY_ACTIVE);
    }

    /*! Return true if the current proxy state is active. */
    const bool IsActiveProxy() const {
        return (ProxyState == mtsProxyBaseServer<mtsTask>::PROXY_ACTIVE);
    }

    void ShutdownSession(const Ice::Current & current) {
        current.adapter->getCommunicator()->shutdown();
        mtsProxyBaseServer<mtsTask>::ShutdownSession();
    }

    /*! Stop the proxy. */
    void Stop();

    /*! Set a server task connected to this proxy server. This server task has 
        to provide at least one provided interface. */
    void SetConnectedTask(mtsTask * serverTask) { ConnectedTask = serverTask; }

protected:
    /*! Typedef for base type. */
    typedef mtsProxyBaseServer<mtsTask> BaseType;

    /*! Typedef for client proxy. */
    typedef mtsDeviceInterfaceProxy::DeviceInterfaceClientPrx DeviceInterfaceClientProxyType;

    /*! Pointer to the task connected. */
    mtsTask * ConnectedTask;

    /*! Connected client object. */
    DeviceInterfaceClientProxyType ConnectedClient;

    //-------------------------------------------------------------------------
    //  Proxy Implementation
    //-------------------------------------------------------------------------
    /*! Create a servant which serves TaskManager clients. */
    Ice::ObjectPtr CreateServant() {
        Sender = new DeviceInterfaceServerI(IceCommunicator, IceLogger, this);
        return Sender;
    }
    
    /*! Start a send thread and wait for shutdown (blocking call). */
    void StartServer();

    /*! Thread runner */
    static void Runner(ThreadArguments<mtsTask> * arguments);

    /*! Definitions for send thread */
    class DeviceInterfaceServerI;
    typedef IceUtil::Handle<DeviceInterfaceServerI> DeviceInterfaceServerIPtr;
    DeviceInterfaceServerIPtr Sender;

    /*! Clean up thread-related resources. */
    void OnEnd();

    /*! Resource clean-up */
    void OnClose();

    //-------------------------------------------------------------------------
    //  Processing Methods
    //-------------------------------------------------------------------------
    /*! Get the local provided interface from the task manager by name. */
    mtsProvidedInterface * GetProvidedInterface(
        const std::string resourceDeviceName, const std::string providedInterfaceName) const;

    //-------------------------------------------------------------------------
    //  Methods to Receive and Process Events (Client -> Server)
    //-------------------------------------------------------------------------
    /*! When a new client connects, add it to the client management list. */
    void ReceiveAddClient(const DeviceInterfaceClientProxyType & clientProxy);

    /*! Update the information of all tasks. */
    const bool ReceiveGetProvidedInterfaceInfo(
        const std::string & providedInterfaceName,
        mtsDeviceInterfaceProxy::ProvidedInterfaceInfo & providedInterfaceInfo);

    /*! Connect at server side. 
        This method creates a client task proxy (mtsDeviceProxy) and a required
        interface proxy (mtsRequiredInterface) at server side. */
    bool ReceiveConnectServerSide(
        const std::string & userTaskName, const std::string & requiredInterfaceName,
        const std::string & resourceTaskName, const std::string & providedInterfaceName);

    /*! Update event handler proxy id at server side and enable them if used. 
        Proxy id is replaced with a pointer to an actual event generator command 
        object at client side. */
    bool ReceiveUpdateEventHandlerId(
        const std::string & clientTaskProxyName, 
        const mtsDeviceInterfaceProxy::ListsOfEventGeneratorsRegistered & eventHandlers) const;

    /*! Update command id. */
    void ReceiveGetCommandId(
        const std::string & clientTaskProxyName,
        mtsDeviceInterfaceProxy::FunctionProxySet & functionProxies);

    /*! Execute actual command objects. */
    void ReceiveExecuteCommandVoid(const CommandIDType commandId) const;
    void ReceiveExecuteCommandWriteSerialized(const CommandIDType commandId, const std::string & argument);
    void ReceiveExecuteCommandReadSerialized(const CommandIDType commandId, std::string & argument);
    void ReceiveExecuteCommandQualifiedReadSerialized(const CommandIDType commandId, const std::string & argument1, std::string & argument2);

    //-------------------------------------------------------------------------
    //  Methods to Send Events (Server -> Client)
    //-------------------------------------------------------------------------
public:
    void SendExecuteEventVoid(const CommandIDType commandId) const;
    void SendExecuteEventWriteSerialized(const CommandIDType commandId, const mtsGenericObject & argument);

    //-------------------------------------------------------------------------
    //  Definition by mtsDeviceInterfaceProxy.ice
    //-------------------------------------------------------------------------
protected:
    class DeviceInterfaceServerI : public mtsDeviceInterfaceProxy::DeviceInterfaceServer,
                                   public IceUtil::Monitor<IceUtil::Mutex> 
    {
    private:
        Ice::CommunicatorPtr Communicator;
        Ice::LoggerPtr Logger;
        mtsDeviceInterfaceProxyServer * DeviceInterfaceServer;
        bool Runnable;
        IceUtil::ThreadPtr Sender;

    public:
        DeviceInterfaceServerI(const Ice::CommunicatorPtr& communicator, 
                             const Ice::LoggerPtr& logger,
                             mtsDeviceInterfaceProxyServer * DeviceInterfaceServer);

        void Start();
        void Run();
        void Stop();

        void AddClient(const ::Ice::Identity&, const ::Ice::Current&);
        void Shutdown(const ::Ice::Current&);

        bool GetProvidedInterfaceInfo(const std::string &,
                                      ::mtsDeviceInterfaceProxy::ProvidedInterfaceInfo&,
                                      const ::Ice::Current&) const;
        bool ConnectServerSide(
            const std::string & userTaskName, const std::string & requiredInterfaceName,
            const std::string & resourceTaskName, const std::string & providedInterfaceName,
            const ::Ice::Current&);

        bool UpdateEventHandlerId(
            const std::string & clientTaskProxyName,
            const mtsDeviceInterfaceProxy::ListsOfEventGeneratorsRegistered & eventGeneratorProxies, 
            const ::Ice::Current&) const;

        void GetCommandId(
            const std::string & clientTaskProxyName,
            mtsDeviceInterfaceProxy::FunctionProxySet&, const ::Ice::Current&) const;

        void ExecuteCommandVoid(::Ice::IceCommandIDType, const ::Ice::Current&);
        void ExecuteCommandWriteSerialized(::Ice::IceCommandIDType, const ::std::string&, const ::Ice::Current&);
        void ExecuteCommandReadSerialized(::Ice::IceCommandIDType, ::std::string&, const ::Ice::Current&);
        void ExecuteCommandQualifiedReadSerialized(::Ice::IceCommandIDType, const ::std::string&, ::std::string&, const ::Ice::Current&);

    };
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsDeviceInterfaceProxyServer)

#endif // _mtsDeviceInterfaceProxyServer_h

