/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung
  Created on: 2010-01-12

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsComponentInterfaceProxyServer_h
#define _mtsComponentInterfaceProxyServer_h

#include <cisstMultiTask/mtsComponentInterfaceProxy.h>
#include <cisstMultiTask/mtsProxyBaseServer.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsParameterTypes.h>

#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsComponentInterfaceProxyServer :
    public mtsProxyBaseServer<mtsComponentProxy,
                              mtsComponentInterfaceProxy::ComponentInterfaceClientPrx,
                              unsigned int>
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    /*! Typedef for client proxy type */
    typedef mtsComponentInterfaceProxy::ComponentInterfaceClientPrx ComponentInterfaceClientProxyType;

    /*! Typedef for base type */
    typedef mtsProxyBaseServer<mtsComponentProxy, ComponentInterfaceClientProxyType, unsigned int> BaseServerType;

protected:
    /*! Definitions for send thread */
    class ComponentInterfaceServerI;
    typedef IceUtil::Handle<ComponentInterfaceServerI> ComponentInterfaceServerIPtr;
    ComponentInterfaceServerIPtr Sender;

    /*! Per-command argument serializer */
    typedef std::map<CommandIDType, mtsProxySerializer *> PerCommandSerializerMapType;
    PerCommandSerializerMapType PerCommandSerializerMap;

    /*! Map to retrieve connection information by client id.
        key=(client id)
        value=(an instance of mtsDescriptionConnection)

        This map is used to disconnect currently established connection when a
        network proxy client is detected as disconnected. */
    typedef std::map<ClientIDType, mtsDescriptionConnection> ConnectionStringMapType;
    ConnectionStringMapType ConnectionStringMap;

    /*! String key to set an implicit per-proxy context for connection id */
    static std::string ConnectionIDKey;

    /*! Communicator (proxy) ID to initialize mtsComponentInterfaceProxyServer.
        A component interface proxy client uses this ID to connect to a proxy
        server. */
    static std::string InterfaceCommunicatorID;

    /*! Instance counter used to set a short name of this thread */
    static unsigned int InstanceCounter;

    //-------------------------------------------------------------------------
    //  Proxy Implementation
    //-------------------------------------------------------------------------
    /*! Create a servant */
    Ice::ObjectPtr CreateServant() {
        Sender = new ComponentInterfaceServerI(IceCommunicator, IceLogger, this);
        return Sender;
    }

    /*! Start a send thread and wait for shutdown (this is a blocking method). */
    void StartServer();

    /*! Thread runner */
    static void Runner(ThreadArguments<mtsComponentProxy> * arguments);

    /*! Get a network proxy client object using clientID. If no network proxy
        client with the clientID is not connected or the proxy is inactive,
        this method returns NULL. */
    ComponentInterfaceClientProxyType * GetNetworkProxyClient(const ClientIDType clientID);

    /*! Monitor all the current connections */
    void MonitorConnections() {
        BaseServerType::Monitor();
    }

    /*! Event handler for client's disconnect event */
    bool OnClientDisconnect(const ClientIDType clientID);

    //-------------------------------------------------------------------------
    //  Event Handlers (Client -> Server)
    //-------------------------------------------------------------------------
    void ReceiveTestMessageFromClientToServer(const ConnectionIDType &connectionID, const std::string & str);

    /*! When a new client connects, add it to the client management list. */
    bool ReceiveAddClient(const ConnectionIDType & connectionID,
                          const std::string & connectingProxyName,
                          const unsigned int providedInterfaceProxyInstanceID,
                          ComponentInterfaceClientProxyType & clientProxy);

    bool ReceiveFetchEventGeneratorProxyPointers(const ConnectionIDType & connectionID,
                                                 const std::string & clientComponentName,
                                                 const std::string & requiredInterfaceName,
                                                 mtsComponentInterfaceProxy::EventGeneratorProxyPointerSet & eventGeneratorProxyPointers);

    void ReceiveExecuteEventVoid(const CommandIDType commandID);

    void ReceiveExecuteEventWriteSerialized(const CommandIDType commandID, const std::string & serializedArgument);

public:
    /*! Constructor and destructor */
    mtsComponentInterfaceProxyServer(const std::string & adapterName, const std::string & communicatorID);
    ~mtsComponentInterfaceProxyServer();

    /*! Entry point to run a proxy. */
    bool Start(mtsComponentProxy * owner);

    /*! Stop the proxy (clean up thread-related resources) */
    void Stop();

    /*! Register per-command (de)serializer */
    bool AddPerCommandSerializer(const CommandIDType commandID, mtsProxySerializer * serializer);

    /*! Register connection information which is used to clean up a logical
        connection when a network proxy client is detected as disconnected. */
    bool AddConnectionInformation(const unsigned int connectionID,
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName);

    //-------------------------------------------------------------------------
    //  Event Generators (Event Sender) : Server -> Client
    //-------------------------------------------------------------------------
    /*! Test method: broadcast string to all clients connected */
    void SendTestMessageFromServerToClient(const std::string & str);

    /*! Fetch function proxy pointers from the connected required interface
        proxy at server process. */
    bool SendFetchFunctionProxyPointers(
        const ClientIDType clientID, const std::string & requiredInterfaceName,
        mtsComponentInterfaceProxy::FunctionProxyPointerSet & functionProxyPointers);

    /*! Execute commands and events. This will call function proxies in the required
        interface proxy at the server process.
        clientID designates which network proxy client should execute a command
        and commandID represents which function proxy object should be called. */
    bool SendExecuteCommandVoid(const ClientIDType clientID, const CommandIDType commandID, const mtsBlockingType blocking);
    bool SendExecuteCommandWriteSerialized(const ClientIDType clientID, const CommandIDType commandID, const mtsGenericObject & argument, const mtsBlockingType blocking);
    bool SendExecuteCommandReadSerialized(const ClientIDType clientID, const CommandIDType commandID, mtsGenericObject & argument);
    bool SendExecuteCommandQualifiedReadSerialized(const ClientIDType clientID, const CommandIDType commandID, const mtsGenericObject & argumentIn, mtsGenericObject & argumentOut);

    //-------------------------------------------------------------------------
    //  Getters
    //-------------------------------------------------------------------------
    /*! Returns connection id key */
    inline static std::string GetConnectionIDKey() {
        return mtsComponentInterfaceProxyServer::ConnectionIDKey;
    }

    /*! Returns communicator (proxy) ID */
    inline static std::string GetInterfaceCommunicatorID() {
        return mtsComponentInterfaceProxyServer::InterfaceCommunicatorID;
    }

    //-------------------------------------------------------------------------
    //  Interface Implementation (refer to mtsComponentInterfaceProxy.ice)
    //-------------------------------------------------------------------------
protected:
    class ComponentInterfaceServerI :
        public mtsComponentInterfaceProxy::ComponentInterfaceServer, public IceUtil::Monitor<IceUtil::Mutex>
    {
    private:
        /*! Ice objects */
        Ice::CommunicatorPtr Communicator;
        IceUtil::ThreadPtr SenderThreadPtr;
        Ice::LoggerPtr IceLogger;

        /*! Network event handler */
        mtsComponentInterfaceProxyServer * ComponentInterfaceProxyServer;

    public:
        ComponentInterfaceServerI(
            const Ice::CommunicatorPtr& communicator,
            const Ice::LoggerPtr& logger,
            mtsComponentInterfaceProxyServer * componentInterfaceProxyServer);
        ~ComponentInterfaceServerI();

        /*! Proxy management */
        void Start();
        void Run();
        void Stop();

        /*! Getter */
        bool IsActiveProxy() const {
            return ComponentInterfaceProxyServer->IsActiveProxy();
        }

        //---------------------------------------
        //  Event Handlers (Client -> Server)
        //---------------------------------------
        /*! Test method */
        void TestMessageFromClientToServer(const std::string & str, const ::Ice::Current & current);

        /*! Add a client proxy. Called when a proxy client connects to server proxy. */
        bool AddClient(const std::string&, ::Ice::Int, const Ice::Identity&, const Ice::Current&);

        /*! Refresh current connection */
        void Refresh(const ::Ice::Current&);

        /*! Shutdown this session; prepare shutdown for safe and clean termination. */
        void Shutdown(const ::Ice::Current&);

        /*! Fetch pointers of event generator proxies from a provided interface
            proxy at server side. */
        bool FetchEventGeneratorProxyPointers(
            const std::string & clientComponentName, const std::string & requiredInterfaceName,
            mtsComponentInterfaceProxy::EventGeneratorProxyPointerSet & eventGeneratorProxyPointers,
            const ::Ice::Current & current) const;

        /*! Execute events */
        void ExecuteEventVoid(::Ice::Long, const ::Ice::Current&);
        void ExecuteEventWriteSerialized(::Ice::Long, const ::std::string &, const ::Ice::Current&);
    };
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsComponentInterfaceProxyServer)

#endif // _mtsComponentInterfaceProxyServer_h

