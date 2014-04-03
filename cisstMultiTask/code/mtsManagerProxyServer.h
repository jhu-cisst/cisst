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

#ifndef _mtsManagerProxyServer_h
#define _mtsManagerProxyServer_h

#include "mtsProxyBaseServer.h"
#include "mtsManagerProxy.h"
#include <cisstMultiTask/mtsManagerLocalInterface.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>

#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsManagerProxyServer :
    public mtsProxyBaseServer<mtsManagerGlobal, mtsManagerProxy::ManagerClientPrx, std::string>,
    public mtsManagerLocalInterface
{
    friend class mtsManagerGlobal;

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    /*! Typedef for client proxy type */
    typedef mtsManagerProxy::ManagerClientPrx ManagerClientProxyType;

    /*! Typedef for base type */
    typedef mtsProxyBaseServer<mtsManagerGlobal, ManagerClientProxyType, std::string> BaseServerType;

protected:
    /*! Definitions of send thread */
    class ManagerServerI;
    typedef IceUtil::Handle<ManagerServerI> ManagerServerIPtr;
    ManagerServerIPtr Sender;

#if IMPROVE_ICE_THREADING
    /*! Signal to wait for internal Ice threads to run */
    osaThreadSignal * IceThreadInitEvent;
#endif

    /*! Instance counter used to set a short name of this thread */
    static unsigned int InstanceCounter;

    /*! String key to set an implicit per-proxy context for connection id */
    static std::string ConnectionIDKey;

    /*! Communicator (proxy) ID to initialize mtsManagerProxyServer. A manager
        proxy client uses this ID to connect to proxy server. */
    static std::string ManagerCommunicatorID;

    //-------------------------------------------------------------------------
    //  Proxy Implementation
    //-------------------------------------------------------------------------
    /*! Create servant */
    Ice::ObjectPtr CreateServant(void);

    /*! Start send thread and wait for shutdown (this is a blocking method). */
    void StartServer();

    /*! Thread runner */
    static void Runner(ThreadArguments<mtsManagerGlobal> * arguments);

    /*! Get a network proxy client using clientID. If no network proxy client
        with the clientID is found or it's inactive proxy, returns NULL. */
    ManagerClientProxyType * GetNetworkProxyClient(const ClientIDType clientID);

    /*! Monitor all the current connections */
    void MonitorConnections(void);

    /*! Check connection timeout */
    void CheckConnectConfirmTimeout(void);

    /*! Remove servant */
    void RemoveServant(void);

    /*! Event handler for client's disconnect event */
    bool OnClientDisconnect(const ClientIDType clientID);

    //-------------------------------------------------------------------------
    //  Event Handlers (Client -> Server)
    //-------------------------------------------------------------------------
    /*! Test method */
    void ReceiveTestMessageFromClientToServer(const IceConnectionIDType & iceConnectionID, const std::string & str);

    /*! When a new client connects, add it to the client management list. */
    bool ReceiveAddClient(const IceConnectionIDType & iceConnectionID,
                          const std::string & connectingProxyName,
                          ManagerClientProxyType & clientProxy);

    /*! Shutdown this session; prepare shutdown for safe and clean termination. */
    void ReceiveShutdown(const ::Ice::Current&);

    /*! Process Management */
    bool ReceiveAddProcess(const std::string & processName);
    bool ReceiveFindProcess(const std::string & processName) const;
    bool ReceiveRemoveProcess(const std::string & processName);

    /*! Component Management */
    bool ReceiveAddComponent(const std::string & processName, const std::string & componentName);
    bool ReceiveFindComponent(const std::string & processName, const std::string & componentName) const;
    bool ReceiveRemoveComponent(const std::string & processName, const std::string & componentName);

    bool ReceiveAddInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName, const std::string & interfaceName);
    bool ReceiveFindInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName, const std::string & interfaceName) const;
    bool ReceiveRemoveInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName, const std::string & interfaceName);

    bool ReceiveAddInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName, const std::string & interfaceName);
    bool ReceiveFindInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName, const std::string & interfaceName) const;
    bool ReceiveRemoveInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName, const std::string & interfaceName);

    /*! Connection Management */
    ::Ice::Int ReceiveConnect(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet);
    bool ReceiveConnectConfirm(::Ice::Int connectionID);
    bool ReceiveDisconnect(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet);
    bool ReceiveDisconnectWithID(::Ice::Int connectionID);

    /*! Networking */
    bool ReceiveSetInterfaceProvidedProxyAccessInfo(const ConnectionIDType connectionID, const std::string & endpointInfo);
    bool ReceiveGetInterfaceProvidedProxyAccessInfo(const ConnectionIDType connectionID, std::string & endpointInfo);
    bool ReceiveGetInterfaceProvidedProxyAccessInfo(const std::string & clientProcessName,
        const std::string & serverProcessName, const std::string & serverComponentName,
        const std::string & serverInterfaceName, std::string & endpointInfo);
    bool ReceiveInitiateConnect(const ConnectionIDType connectionID);
    bool ReceiveConnectServerSideInterfaceRequest(const ConnectionIDType connectionID);

public:
    /*! Constructor and destructor */
    mtsManagerProxyServer(const std::string & adapterName, const std::string & communicatorID);
    ~mtsManagerProxyServer();

    /*! Entry point to run a proxy */
    bool StartProxy(mtsManagerGlobal * owner);

    /*! Stop the proxy (clean up thread-related resources) */
    void StopProxy();

    /*! Data structure converters */
    static void ConvertInterfaceProvidedDescription(
        const ::mtsManagerProxy::InterfaceProvidedDescription & src, mtsInterfaceProvidedDescription & dest);

    static void ConvertInterfaceRequiredDescription(
        const ::mtsManagerProxy::InterfaceRequiredDescription & src, mtsInterfaceRequiredDescription & dest);

    static void ConstructInterfaceProvidedDescriptionFrom(
        const mtsInterfaceProvidedDescription & src, ::mtsManagerProxy::InterfaceProvidedDescription & dest);

    static void ConstructInterfaceRequiredDescriptionFrom(
        const mtsInterfaceRequiredDescription & src, ::mtsManagerProxy::InterfaceRequiredDescription & dest);

    //-------------------------------------------------------------------------
    //  Implementation of mtsManagerLocalInterface
    //
    //  See mtsManagerLocalInterface.h for detailed documentation.
    //-------------------------------------------------------------------------
    //  Proxy Object Control (Creation, Removal)
    bool CreateComponentProxy(const std::string & componentProxyName, const std::string & listenerID = "");

    bool RemoveComponentProxy(const std::string & componentProxyName, const std::string & listenerID = "");

    bool CreateInterfaceProvidedProxy(const std::string & serverComponentProxyName,
        const mtsInterfaceProvidedDescription & providedInterfaceDescription, const std::string & listenerID = "");

    bool CreateInterfaceRequiredProxy(const std::string & clientComponentProxyName,
        const mtsInterfaceRequiredDescription & requiredInterfaceDescription, const std::string & listenerID = "");

    bool RemoveInterfaceProvidedProxy(
        const std::string & componentProxyName, const std::string & providedInterfaceProxyName, const std::string & listenerID = "");

    bool RemoveInterfaceRequiredProxy(
        const std::string & componentProxyName, const std::string & requiredInterfaceProxyName, const std::string & listenerID = "");

    //  Connection Management
    bool ConnectServerSideInterface(const mtsDescriptionConnection & description, const std::string & listenerID = "");

    bool ConnectClientSideInterface(const mtsDescriptionConnection & description, const std::string & listenerID = "");

    //  Getters
    bool GetInterfaceProvidedDescription(
        const std::string & serverComponentName, const std::string & providedInterfaceName,
        mtsInterfaceProvidedDescription & providedInterfaceDescription, const std::string & listenerID = "");

    bool GetInterfaceRequiredDescription(const std::string & componentName, const std::string & requiredInterfaceName,
        mtsInterfaceRequiredDescription & requiredInterfaceDescription, const std::string & listenerID = "");

    const std::string GetProcessName(const std::string & listenerID = "") const;

    void GetNamesOfCommands(std::vector<std::string>& namesOfCommands,
                            const std::string & componentName,
                            const std::string & providedInterfaceName,
                            const std::string & listenerID = "");

    void GetNamesOfEventGenerators(std::vector<std::string>& namesOfEventGenerators,
                                   const std::string & componentName,
                                   const std::string & providedInterfaceName,
                                   const std::string & listenerID = "");

    void GetNamesOfFunctions(std::vector<std::string>& namesOfFunctions,
                             const std::string & componentName,
                             const std::string & requiredInterfaceName,
                             const std::string & listenerID = "");

    void GetNamesOfEventHandlers(std::vector<std::string>& namesOfEventHandlers,
                                 const std::string & componentName,
                                 const std::string & requiredInterfaceName,
                                 const std::string & listenerID = "");

    void GetDescriptionOfCommand(std::string & description,
                                 const std::string & componentName,
                                 const std::string & providedInterfaceName,
                                 const std::string & commandName,
                                 const std::string & listenerID = "");

    void GetDescriptionOfEventGenerator(std::string & description,
                                        const std::string & componentName,
                                        const std::string & providedInterfaceName,
                                        const std::string & eventGeneratorName,
                                        const std::string & listenerID = "");

    void GetDescriptionOfFunction(std::string & description,
                                  const std::string & componentName,
                                  const std::string & requiredInterfaceName,
                                  const std::string & functionName,
                                  const std::string & listenerID = "");

    void GetDescriptionOfEventHandler(std::string & description,
                                      const std::string & componentName,
                                      const std::string & requiredInterfaceName,
                                      const std::string & eventHandlerName,
                                      const std::string & listenerID = "");

    //-------------------------------------------------------------------------
    //  Event Generators (Event Sender) : Server -> Client
    //-------------------------------------------------------------------------
    /*! Test method: broadcast string to all clients connected */
    void SendTestMessageFromServerToClient(const std::string & str);

    /*! Proxy object control (creation and removal) */
    bool SendCreateComponentProxy(const std::string & componentProxyName, const std::string & clientID);

    bool SendRemoveComponentProxy(const std::string & componentProxyName, const std::string & clientID);

    bool SendCreateInterfaceProvidedProxy(const std::string & serverComponentProxyName,
        const ::mtsManagerProxy::InterfaceProvidedDescription & providedInterfaceDescription,
        const std::string & clientID);

    bool SendCreateInterfaceRequiredProxy(const std::string & clientComponentProxyName,
        const ::mtsManagerProxy::InterfaceRequiredDescription & requiredInterfaceDescription,
        const std::string & clientID);

    bool SendRemoveInterfaceProvidedProxy(const std::string & componentProxyName,
        const std::string & providedInterfaceProxyName, const std::string & clientID);

    bool SendRemoveInterfaceRequiredProxy(const std::string & componentProxyName,
        const std::string & requiredInterfaceProxyName, const std::string & clientID);

    /*! Connection management */
    bool SendConnectServerSideInterface(const ConnectionIDType connectionID,
        const ::mtsManagerProxy::ConnectionStringSet & connectionStrings, const std::string & clientID);

    bool SendConnectClientSideInterface(::Ice::Int connectionID,
        const ::mtsManagerProxy::ConnectionStringSet & connectionStrings, const std::string & clientID);

    /*! Getters */
    bool SendGetInterfaceProvidedDescription(
        const std::string & serverComponentName, const std::string & providedInterfaceName,
        ::mtsManagerProxy::InterfaceProvidedDescription & providedInterfaceDescription,
        const std::string & clientID);

    bool SendGetInterfaceRequiredDescription(
        const std::string & componentName, const std::string & requiredInterfaceName,
        ::mtsManagerProxy::InterfaceRequiredDescription & requiredInterfaceDescription,
        const std::string & clientID);

    void SendGetNamesOfCommands(std::vector<std::string>& namesOfCommands,
                                const std::string & componentName,
                                const std::string & providedInterfaceName,
                                const std::string & clientID);

    void SendGetNamesOfEventGenerators(std::vector<std::string>& namesOfEventGenerators,
                                       const std::string & componentName,
                                       const std::string & providedInterfaceName,
                                       const std::string & clientID);

    void SendGetNamesOfFunctions(std::vector<std::string>& namesOfFunctions,
                                 const std::string & componentName,
                                 const std::string & requiredInterfaceName,
                                 const std::string & clientID);

    void SendGetNamesOfEventHandlers(std::vector<std::string>& namesOfEventHandlers,
                                     const std::string & componentName,
                                     const std::string & requiredInterfaceName,
                                     const std::string & clientID);

    void SendGetDescriptionOfCommand(std::string & description,
                                     const std::string & componentName,
                                     const std::string & providedInterfaceName,
                                     const std::string & commandName,
                                     const std::string & clientID);

    void SendGetDescriptionOfEventGenerator(std::string & description,
                                            const std::string & componentName,
                                            const std::string & providedInterfaceName,
                                            const std::string & eventGeneratorName,
                                            const std::string & clientID);

    void SendGetDescriptionOfFunction(std::string & description,
                                      const std::string & componentName,
                                      const std::string & requiredInterfaceName,
                                      const std::string & functionName,
                                      const std::string & clientID);

    void SendGetDescriptionOfEventHandler(std::string & description,
                                          const std::string & componentName,
                                          const std::string & requiredInterfaceName,
                                          const std::string & eventHandlerName,
                                          const std::string & clientID);

    std::string SendGetProcessName(const std::string & clientID);

    ::Ice::Int SendGetCurrentInterfaceCount(const std::string & componentName, const std::string & clientID);

    //-------------------------------------------------------------------------
    //  Getters
    //-------------------------------------------------------------------------
    /*! Returns connection id key */
    inline static std::string GetConnectionIDKey() {
        return mtsManagerProxyServer::ConnectionIDKey;
    }

    /*! Returns communicator (proxy) ID */
    inline static std::string GetManagerCommunicatorID() {
        return mtsManagerProxyServer::ManagerCommunicatorID;
    }

    /*! Returns full path name for config file */
    static std::string GetConfigFullName(const std::string &propertyFileName);

    /*! Returns the port number that the global component manager uses */
    static std::string GetGCMPortNumberAsString();

    /*! Returns connection timeout value (msec) from GCM's configuration file */
    static int GetGCMConnectTimeout();

    //-------------------------------------------------------------------------
    //  Definition by mtsManagerProxy.ice
    //-------------------------------------------------------------------------
protected:
    class ManagerServerI :
        public mtsManagerProxy::ManagerServer, public IceUtil::Monitor<IceUtil::Mutex>
    {
    private:
        /*! Ice objects */
        Ice::CommunicatorPtr Communicator;
        SenderThread<ManagerServerIPtr> * SenderThreadPtr;
        Ice::LoggerPtr IceLogger;

        /*! Network event handler */
        mtsManagerProxyServer * ManagerProxyServer;

    public:
        ManagerServerI(
            const Ice::CommunicatorPtr& communicator,
            const Ice::LoggerPtr& logger,
            mtsManagerProxyServer * ManagerProxyServer);
        ~ManagerServerI();

        /*! Proxy management */
        void Start();
        void Run();
        void Stop();

        /*! Getter */
        bool IsActiveProxy() const;

        //---------------------------------------
        //  Event Handlers (Client -> Server)
        //---------------------------------------
        /*! Test method */
        void TestMessageFromClientToServer(const std::string & str, const ::Ice::Current & current);

        /*! Add a client proxy. Called when a proxy client connects to server proxy */
        bool AddClient(const std::string & processName, const Ice::Identity&, const Ice::Current&);

        /*! Refresh current connection */
        void Refresh(const ::Ice::Current&);

        /*! Shutdown this session; prepare shutdown for safe and clean termination */
        void Shutdown(const ::Ice::Current&);

        /*! Process Management */
        bool AddProcess(const std::string & processName, const ::Ice::Current & CMN_UNUSED(current));
        bool FindProcess(const std::string & processName, const ::Ice::Current & CMN_UNUSED(current)) const;
        bool RemoveProcess(const std::string & processName, const ::Ice::Current & CMN_UNUSED(current));

        /*! Component Management */
        bool AddComponent(const std::string & processName, const std::string & componentName, const ::Ice::Current & CMN_UNUSED(current));
        bool FindComponent(const std::string & processName, const std::string & componentName, const ::Ice::Current & CMN_UNUSED(current)) const;
        bool RemoveComponent(const std::string & processName, const std::string & componentName, const ::Ice::Current & CMN_UNUSED(current));

        bool AddInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName, const std::string & interfaceName, const ::Ice::Current & CMN_UNUSED(current));
        bool FindInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName, const std::string & interfaceName, const ::Ice::Current & CMN_UNUSED(current));
        bool RemoveInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName, const std::string & interfaceName, const ::Ice::Current & CMN_UNUSED(current));

        bool AddInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName, const std::string & interfaceName, const ::Ice::Current & current);
        bool FindInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName, const std::string & interfaceName, const ::Ice::Current &) const;
        bool RemoveInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName, const std::string & interfaceName, const ::Ice::Current & current);

        /*! Connection Management */
        ::Ice::Int Connect(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, const ::Ice::Current & current);
        bool ConnectConfirm(::Ice::Int connectionID, const ::Ice::Current & current);
        bool DisconnectWithID(::Ice::Int connectionID, const ::Ice::Current & current);
        bool Disconnect(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, const ::Ice::Current & current);

        /*! Networking */
        bool SetInterfaceProvidedProxyAccessInfo(::Ice::Int connectionID, const std::string & endpointInfo, const ::Ice::Current & current);
        bool GetInterfaceProvidedProxyAccessInfo(const std::string & clientProcessName,
            const std::string & serverProcessName, const std::string & serverComponentName,
            const std::string & serverInterfaceName, std::string & endpointInfo, const ::Ice::Current & current);
        bool GetInterfaceProvidedProxyAccessInfoWithID(::Ice::Int connectionID, std::string & endpointInfo, const ::Ice::Current & current);
        bool InitiateConnect(::Ice::Int connectionID, const ::Ice::Current & current);
        bool ConnectServerSideInterfaceRequest(::Ice::Int connectionID, const ::Ice::Current & current);
    };
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsManagerProxyServer)

#endif // _mtsManagerProxyServer_h
