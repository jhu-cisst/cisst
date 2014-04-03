/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2010-01-12

  (C) Copyright 2010-2011 Johns Hopkins University (JHU), All Rights
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
#include "mtsProxyBaseServer.h"
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
    /*! Callback thread for bi-directional communication with server */
    class ComponentInterfaceServerI;
    typedef IceUtil::Handle<ComponentInterfaceServerI> ComponentInterfaceServerIPtr;
    ComponentInterfaceServerIPtr Sender;

    /*! Per-command argument serializer */
    typedef std::map<mtsCommandIDType, mtsProxySerializer *> PerCommandSerializerMapType;
    PerCommandSerializerMapType PerCommandSerializerMap;

    /*! Map to retrieve connection information by client id.
      key=(client id)
      value=(an instance of mtsDescriptionConnection)

      This map is used to disconnect interface proxies when any error occurs or
      Ice proxy disconnection is detected.

      typedef std::map<ClientIDType, mtsDescriptionConnection> ConnectionStringMapType;
      ConnectionStringMapType ConnectionStringMap;
    */
    typedef std::map<ClientIDType, ConnectionIDType> ClientConnectionIDMapType;
    ClientConnectionIDMapType ClientConnectionIDMap;

#if IMPROVE_ICE_THREADING
    /*! Signal to wait for internal Ice threads to run */
    osaThreadSignal * IceThreadInitEvent;
#endif

    /*! String key to set implicit per-proxy context for connection id */
    static std::string ConnectionIDKey;

    /*! Communicator (proxy) id to initialize mtsComponentInterfaceProxyServer.
      A component interface proxy client uses this id to connect to a proxy
      server. */
    static std::string InterfaceCommunicatorID;

    /*! Instance counter used to set a short name of this thread */
    static unsigned int InstanceCounter;

    //-------------------------------------------------------------------------
    //  Proxy Implementation
    //-------------------------------------------------------------------------
    /*! Create servant */
    Ice::ObjectPtr CreateServant(void);

    /*! Start send thread and wait for shutdown (this is a blocking method). */
    void StartServer(void);

    /*! Thread runner */
    static void Runner(ThreadArguments<mtsComponentProxy> * arguments);

    /*! Get a network proxy client using clientID. If no network proxy client
      with the clientID is found or it's inactive proxy, returns NULL. */
    ComponentInterfaceClientProxyType * GetNetworkProxyClient(const ClientIDType clientID);

    /*! Monitor all the current connections */
    void MonitorConnections(void);

    /*! Remove servant */
    void RemoveServant(void);

    /*! Event handler for client's disconnect event */
    bool OnClientDisconnect(const ClientIDType clientID);

    //-------------------------------------------------------------------------
    //  Event Handlers (Client -> Server)
    //-------------------------------------------------------------------------
    void ReceiveTestMessageFromClientToServer(const IceConnectionIDType & iceConnectionID, const std::string & str);

    /*! When a new client connects, add it to the client management list. */
    bool ReceiveAddClient(const IceConnectionIDType & iceConnectionID,
                          const std::string & connectingProxyName,
                          const unsigned int providedInterfaceProxyInstanceID,
                          ComponentInterfaceClientProxyType & clientProxy);

    bool ReceiveFetchEventGeneratorProxyPointers(const IceConnectionIDType & iceConnectionID,
                                                 const std::string & clientComponentName,
                                                 const std::string & requiredInterfaceName,
                                                 mtsComponentInterfaceProxy::EventGeneratorProxyPointerSet & eventGeneratorProxyPointers);

    void ReceiveExecuteEventVoid(const mtsCommandIDType commandID);

    void ReceiveExecuteEventWriteSerialized(const mtsCommandIDType commandID, const std::string & serializedArgument);

    void ReceiveExecuteEventReturnSerialized(const mtsCommandIDType commandID, const mtsObjectIDType resultAddress,
                                             const std::string & result);

 public:
    /*! Constructor and destructor */
    mtsComponentInterfaceProxyServer(const std::string & adapterName, const std::string & communicatorID);
    ~mtsComponentInterfaceProxyServer();

    /*! Entry point to run a proxy. */
    bool StartProxy(mtsComponentProxy * owner);

    /*! Stop the proxy (clean up thread-related resources) */
    void StopProxy(void);

    /*! Register per-command (de)serializer */
    bool AddPerCommandSerializer(const mtsCommandIDType commandID, mtsProxySerializer * serializer);

    /*! Register connection information which is used to clean up a logical
      connection when a network proxy disconnection is detected. */
    bool AddConnectionInformation(const ConnectionIDType connectionID);

    //-------------------------------------------------------------------------
    //  Event Generators (Event Sender) : Server -> Client
    //-------------------------------------------------------------------------
    /*! Test method: broadcast string to all clients connected */
    void SendTestMessageFromServerToClient(const std::string & str);

    /*! Fetch function proxy pointers from the connected required interface
      proxy at server process. */
    bool SendFetchFunctionProxyPointers(const ClientIDType clientID, const std::string & requiredInterfaceName,
                                        mtsComponentInterfaceProxy::FunctionProxyPointerSet & functionProxyPointers);

    /*! Execute commands and events. This will call function proxies in the required
      interface proxy at the server process.
      clientID designates which network proxy client should execute a command
      and commandID represents which function proxy object should be called. */
    bool SendExecuteCommandVoid(const ClientIDType clientID, const mtsCommandIDType commandID,
                                const mtsBlockingType blocking, mtsExecutionResult & executionResult);
    bool SendExecuteCommandWriteSerialized(const ClientIDType clientID, const mtsCommandIDType commandID,
                                           const mtsBlockingType blocking, mtsExecutionResult & executionResult,
                                           const mtsGenericObject & argument);
    bool SendExecuteCommandReadSerialized(const ClientIDType clientID, const mtsCommandIDType commandID,
                                          mtsExecutionResult & executionResult,
                                          mtsGenericObject & argument);
    bool SendExecuteCommandQualifiedReadSerialized(const ClientIDType clientID, const mtsCommandIDType commandID,
                                                   mtsExecutionResult & executionResult,
                                                   const mtsGenericObject & argumentIn, mtsGenericObject & argumentOut);
    bool SendExecuteCommandVoidReturnSerialized(const ClientIDType clientID, const mtsCommandIDType commandID,
                                                mtsExecutionResult & executionResult,
                                                mtsGenericObject & result);
    bool SendExecuteCommandWriteReturnSerialized(const ClientIDType clientID, const mtsCommandIDType commandID,
                                                 mtsExecutionResult & executionResult,
                                                 const mtsGenericObject & argument,
                                                 mtsGenericObject & result);

    //-------------------------------------------------------------------------
    //  Getters
    //-------------------------------------------------------------------------
    /*! Returns connection id key */
    inline static const std::string GetConnectionIDKey(void) {
        return mtsComponentInterfaceProxyServer::ConnectionIDKey;
    }

    /*! Returns communicator (proxy) ID */
    inline static const std::string GetInterfaceCommunicatorID(void) {
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
        SenderThread<ComponentInterfaceServerIPtr> * SenderThreadPtr;
        Ice::LoggerPtr IceLogger;

        /*! Network event handler */
        mtsComponentInterfaceProxyServer * ComponentInterfaceProxyServer;

    public:
        ComponentInterfaceServerI(const Ice::CommunicatorPtr& communicator,
                                  const Ice::LoggerPtr& logger,
                                  mtsComponentInterfaceProxyServer * componentInterfaceProxyServer);
        ~ComponentInterfaceServerI();

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

        /*! Add a client proxy. Called when a proxy client connects to server proxy. */
        bool AddClient(const std::string&, ::Ice::Int, const Ice::Identity&, const Ice::Current&);

        /*! Refresh current connection */
        void Refresh(const ::Ice::Current&);

        /*! Shutdown this session; prepare shutdown for safe and clean termination. */
        void Shutdown(const ::Ice::Current&);

        /*! Fetch pointers of event generator proxies from a provided interface
          proxy at server side. */
        bool FetchEventGeneratorProxyPointers(const std::string & clientComponentName, const std::string & requiredInterfaceName,
                                              mtsComponentInterfaceProxy::EventGeneratorProxyPointerSet & eventGeneratorProxyPointers,
                                              const ::Ice::Current & current) const;

        /*! Execute events */
        void ExecuteEventVoid(::Ice::Long, const ::Ice::Current&);
        void ExecuteEventWriteSerialized(::Ice::Long, const ::std::string &, const ::Ice::Current&);
        void ExecuteEventReturnSerialized(::Ice::Long commandId, ::Ice::Long resultAddress, const ::std::string & result, const ::Ice::Current&);
    };
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsComponentInterfaceProxyServer)

#endif // _mtsComponentInterfaceProxyServer_h

