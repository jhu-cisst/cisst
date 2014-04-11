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

#ifndef _mtsManagerProxyClient_h
#define _mtsManagerProxyClient_h

#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsManagerProxy.h>
#include "mtsProxyBaseClient.h"
#include <cisstMultiTask/mtsManagerGlobalInterface.h>

#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsManagerProxyClient :
    public mtsProxyBaseClient<mtsManagerLocal>, public mtsManagerGlobalInterface
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    /*! Typedef for base type. */
    typedef mtsProxyBaseClient<mtsManagerLocal> BaseClientType;

    /*! Typedef for connected server proxy. */
    typedef mtsManagerProxy::ManagerServerPrx ManagerServerProxyType;
    ManagerServerProxyType ManagerServerProxy;

protected:
    /*! Definitions of callback thread to communicate with server */
    class ManagerClientI;
    typedef IceUtil::Handle<ManagerClientI> ManagerClientIPtr;
    ManagerClientIPtr Server;

#if IMPROVE_ICE_THREADING
    /*! Signal to wait for internal Ice threads to run */
    osaThreadSignal * IceThreadInitEvent;
#endif

    /*! Instance counter used to set a short name of this thread */
    static unsigned int InstanceCounter;

    //-------------------------------------------------------------------------
    //  Proxy Implementation
    //-------------------------------------------------------------------------
    /*! Create GCM proxy (server) and callback thread to communicate with
        server.
        This methods gets called by the base class (mtsProxyBaseClient). */
    void CreateProxy(void);

    /*! Destroy connected GCM proxy including callback thread to communicate
        with server.
        This methods gets called by the base class (mtsProxyBaseClient). */
    void RemoveProxy(void);

    /*! Start callback thread to communicate with server (blocking call).
        Internally, mtsManagerProxyClient::ManagerClientI::Start() is called. */
    void StartClient(void);

    /*! Called when server disconnection is detected or any exception occurs. */
    void OnServerDisconnect(const Ice::Exception & ex);

    /*! Runner for callback thread to communicate with server */
    static void Runner(ThreadArguments<mtsManagerLocal> * arguments);

    //-------------------------------------------------------------------------
    //  Event Handlers : Server -> Client
    //-------------------------------------------------------------------------
    void ReceiveTestMessageFromServerToClient(const std::string & str) const;

    // Proxy object control (creation and removal)
    bool ReceiveCreateComponentProxy(const std::string & componentProxyName);
    bool ReceiveRemoveComponentProxy(const std::string & componentProxyName);
    bool ReceiveCreateInterfaceProvidedProxy(const std::string & serverComponentProxyName, const ::mtsManagerProxy::InterfaceProvidedDescription & providedInterfaceDescription);
    bool ReceiveCreateInterfaceRequiredProxy(const std::string & clientComponentProxyName, const ::mtsManagerProxy::InterfaceRequiredDescription & requiredInterfaceDescription);
    bool ReceiveRemoveInterfaceProvidedProxy(const std::string & componentProxyName, const std::string & providedInterfaceProxyName);
    bool ReceiveRemoveInterfaceRequiredProxy(const std::string & componentProxyName, const std::string & requiredInterfaceProxyName);

    // Connection management
    bool ReceiveConnectServerSideInterface(const ConnectionIDType connectionID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet);
    bool ReceiveConnectClientSideInterface(const ConnectionIDType connectionID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet);

    // Getters
    bool ReceiveGetInterfaceProvidedDescription(const std::string & serverComponentName, const std::string & providedInterfaceName,
                                                ::mtsManagerProxy::InterfaceProvidedDescription & providedInterfaceDescription);
    bool ReceiveGetInterfaceRequiredDescription(const std::string & componentName, const std::string & requiredInterfaceName,
                                                ::mtsManagerProxy::InterfaceRequiredDescription & requiredInterfaceDescription);

    // Getters for component inspector
    void ReceiveGetNamesOfCommands(const std::string & componentName, const std::string & providedInterfaceName,
                                   ::mtsManagerProxy::NamesOfCommandsSequence & names) const;
    void ReceiveGetNamesOfEventGenerators(const std::string & componentName, const std::string & providedInterfaceName,
                                          ::mtsManagerProxy::NamesOfEventGeneratorsSequence & names) const;
    void ReceiveGetNamesOfFunctions(const std::string & componentName, const std::string & requiredInterfaceName,
                                    ::mtsManagerProxy::NamesOfFunctionsSequence & names) const;
    void ReceiveGetNamesOfEventHandlers(const std::string & componentName, const std::string & requiredInterfaceName,
                                        ::mtsManagerProxy::NamesOfEventHandlersSequence & names) const;

    void ReceiveGetDescriptionOfCommand(const std::string & componentName, const std::string & providedInterfaceName,
                                        const std::string & commandName, std::string & description) const;
    void ReceiveGetDescriptionOfEventGenerator(const std::string & componentName, const std::string & providedInterfaceName,
                                               const std::string & eventGeneratorName, std::string & description) const;
    void ReceiveGetDescriptionOfFunction(const std::string & componentName, const std::string & requiredInterfaceName,
                                         const std::string & functionName, std::string & description) const;
    void ReceiveGetDescriptionOfEventHandler(const std::string & componentName, const std::string & requiredInterfaceName,
                                             const std::string & eventHandlerName, std::string & description) const;

    std::string ReceiveGetProcessName();
    ::Ice::Int ReceiveGetCurrentInterfaceCount(const std::string & componentName);

public:
    /*! Constructor and destructor */
    mtsManagerProxyClient(const std::string & serverEndpointInfo);
    ~mtsManagerProxyClient();

    /*! Entry point to run a proxy. */
    bool StartProxy(mtsManagerLocal * proxyOwner);

    /*! Stop the proxy (clean up thread-related resources) */
    void StopProxy(void);

    //-------------------------------------------------------------------------
    //  Implementation of mtsManagerGlobalInterface
    //  (See mtsManagerGlobalInterface.h for details)
    //-------------------------------------------------------------------------
    //  Process Management
    bool AddProcess(const std::string & processName);
    bool FindProcess(const std::string & processName) const;
    bool RemoveProcess(const std::string & processName, const bool networkDisconnect);

    //  Component Management
    bool AddComponent(const std::string & processName, const std::string & componentName);
    bool FindComponent(const std::string & processName, const std::string & componentName) const;
    bool RemoveComponent(const std::string & processName, const std::string & componentName, const bool lock = true);

    //  Interface Management
    bool AddInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName, const std::string & interfaceName);
    bool AddInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName, const std::string & interfaceName);
    bool FindInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName, const std::string & interfaceName) const;
    bool FindInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName, const std::string & interfaceName) const;
    bool RemoveInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName, const std::string & interfaceName, const bool lock = true);
    bool RemoveInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName, const std::string & interfaceName, const bool lock = true);

    //  Connection Management
    ConnectionIDType Connect(const std::string & requestProcessName,
                             const std::string & clientProcessName, const std::string & clientComponentName,
                             const std::string & clientInterfaceName,
                             const std::string & serverProcessName, const std::string & serverComponentName,
                             const std::string & serverInterfaceName);
    bool ConnectConfirm(const ConnectionIDType connectionID);
    bool Disconnect(const ConnectionIDType connectionID);
    bool Disconnect(const std::string & clientProcessName, const std::string & clientComponentName,
                    const std::string & clientInterfaceName,
                    const std::string & serverProcessName, const std::string & serverComponentName,
                    const std::string & serverInterfaceName);

    //  Networking
    bool SetInterfaceProvidedProxyAccessInfo(const ConnectionIDType connectionID, const std::string & endpointInfo);

    bool GetInterfaceProvidedProxyAccessInfo(const ConnectionIDType connectionID, std::string & endpointInfo);

    bool GetInterfaceProvidedProxyAccessInfo(const std::string & clientProcessName,
        const std::string & serverProcessName, const std::string & serverComponentName,
        const std::string & serverInterfaceName, std::string & endpointInfo);

    bool InitiateConnect(const ConnectionIDType connectionID);

    bool ConnectServerSideInterfaceRequest(const ConnectionIDType connectionID);

    void GetListOfConnections(std::vector<mtsDescriptionConnection> & list) const;

    //-------------------------------------------------------------------------
    //  Event Generators (Event Sender) : Client -> Server
    //-------------------------------------------------------------------------
    void SendTestMessageFromClientToServer(const std::string & str) const;

    // Process Management
    bool SendAddProcess(const std::string & processName);
    bool SendFindProcess(const std::string & processName);
    bool SendRemoveProcess(const std::string & processName);

    // Component Management
    bool SendAddComponent(const std::string & processName, const std::string & componentName);
    bool SendFindComponent(const std::string & processName, const std::string & componentName);
    bool SendRemoveComponent(const std::string & processName, const std::string & componentName);

    bool SendAddInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName, const std::string & interfaceName);
    bool SendFindInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName, const std::string & interfaceName);
    bool SendRemoveInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName, const std::string & interfaceName);

    bool SendAddInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName, const std::string & interfaceName);
    bool SendFindInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName, const std::string & interfaceName);
    bool SendRemoveInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName, const std::string & interfaceName);

    // Connection Management
    ::Ice::Int SendConnect(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet);
    bool SendConnectConfirm(::Ice::Int connectionID);
    bool SendDisconnect(::Ice::Int connectionID);
    bool SendDisconnect(const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet);

    // Networking
    bool SendSetInterfaceProvidedProxyAccessInfo(const ConnectionIDType connectionID, const std::string & endpointInfo);
    bool SendGetInterfaceProvidedProxyAccessInfo(const ConnectionIDType connectionID, std::string & endpointInfo);
    bool SendGetInterfaceProvidedProxyAccessInfo(const std::string & clientProcessName,
        const std::string & serverProcessName, const std::string & serverComponentName,
        const std::string & serverInterfaceName, std::string & endpointInfo);
    bool SendInitiateConnect(::Ice::Int connectionID);
    bool SendConnectServerSideInterfaceRequest(const ConnectionIDType connectionID);

    //-------------------------------------------------------------------------
    //  Definition by mtsInterfaceProvidedProxy.ice
    //-------------------------------------------------------------------------
protected:
    class ManagerClientI :
        public mtsManagerProxy::ManagerClient, public IceUtil::Monitor<IceUtil::Mutex>
    {
    private:
        /*! Ice objects */
        Ice::CommunicatorPtr Communicator;
        SenderThread<ManagerClientIPtr> * SenderThreadPtr;
        Ice::LoggerPtr IceLogger;

        /*! Network event processor */
        mtsManagerProxyClient * ManagerProxyClient;

        /*! Connected server proxy */
        mtsManagerProxy::ManagerServerPrx Server;

    public:
        /*! Constructor and destructor */
        ManagerClientI(
            const Ice::CommunicatorPtr& communicator,
            const Ice::LoggerPtr& logger,
            const mtsManagerProxy::ManagerServerPrx& server,
            mtsManagerProxyClient * ManagerClient);
        ~ManagerClientI();

        /*! Proxy management */
        void Start();
        void Run();
        void Stop();

        /*! Getter */
        bool IsActiveProxy() const;

        //-------------------------------------------------
        //  Event handlers (Server -> Client)
        //-------------------------------------------------
        /*! Test method */
        void TestMessageFromServerToClient(const std::string & str, const ::Ice::Current & current);

        /*! Proxy object control (creation and removal) */
        bool CreateComponentProxy(const std::string & componentProxyName, const ::Ice::Current & current);
        bool RemoveComponentProxy(const std::string & componentProxyName, const ::Ice::Current & current);
        bool CreateInterfaceProvidedProxy(const std::string & serverComponentProxyName, const ::mtsManagerProxy::InterfaceProvidedDescription & providedInterfaceDescription, const ::Ice::Current & current);
        bool CreateInterfaceRequiredProxy(const std::string & clientComponentProxyName, const ::mtsManagerProxy::InterfaceRequiredDescription & requiredInterfaceDescription, const ::Ice::Current & current);
        bool RemoveInterfaceProvidedProxy(const std::string & componentProxyName, const std::string & providedInterfaceProxyName, const ::Ice::Current & current);
        bool RemoveInterfaceRequiredProxy(const std::string & componentProxyName, const std::string & requiredInterfaceProxyName, const ::Ice::Current & current);

        /*! Connection management */
        bool ConnectServerSideInterface(::Ice::Int connectionID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, const ::Ice::Current & current);
        bool ConnectClientSideInterface(::Ice::Int connectionID, const ::mtsManagerProxy::ConnectionStringSet & connectionStringSet, const ::Ice::Current & current);

        /*! Getters */
        bool GetInterfaceProvidedDescription(const std::string & serverComponentName, const std::string & providedInterfaceName, ::mtsManagerProxy::InterfaceProvidedDescription & providedInterfaceDescription, const ::Ice::Current &) const;
        bool GetInterfaceRequiredDescription(const std::string & componentName, const std::string & requiredInterfaceName, ::mtsManagerProxy::InterfaceRequiredDescription & requiredInterfaceDescription, const ::Ice::Current &) const;

        /*! Getters for component inspector */
        void GetNamesOfCommands(const std::string & componentName, const std::string & providedInterfaceName, ::mtsManagerProxy::NamesOfCommandsSequence & names, const ::Ice::Current & current) const;
        void GetNamesOfEventGenerators(const std::string & componentName, const std::string & providedInterfaceName, ::mtsManagerProxy::NamesOfEventGeneratorsSequence  & names, const ::Ice::Current & current) const;
        void GetNamesOfFunctions(const std::string & componentName, const std::string & requiredInterfaceName, ::mtsManagerProxy::NamesOfFunctionsSequence & names, const ::Ice::Current & current) const;
        void GetNamesOfEventHandlers(const std::string & componentName, const std::string & requiredInterfaceName, ::mtsManagerProxy::NamesOfEventHandlersSequence & names, const ::Ice::Current & current) const;

        void GetDescriptionOfCommand(const std::string & componentName, const std::string & providedInterfaceName, const std::string & commandName, std::string & description, const ::Ice::Current & current) const;
        void GetDescriptionOfEventGenerator(const std::string & componentName, const std::string & providedInterfaceName, const std::string & eventGeneratorName, std::string & description, const ::Ice::Current & current) const;
        void GetDescriptionOfFunction(const std::string & componentName, const std::string & requiredInterfaceName, const std::string & eventGeneratorName, std::string & description, const ::Ice::Current & current) const;
        void GetDescriptionOfEventHandler(const std::string & componentName, const std::string & requiredInterfaceName, const std::string & eventHandlerName, std::string & description, const ::Ice::Current & current) const;

        std::string GetProcessName(const ::Ice::Current &) const;
    };
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsManagerProxyClient)

#endif // _mtsManagerProxyClient_h
