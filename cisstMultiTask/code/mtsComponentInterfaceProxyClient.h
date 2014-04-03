/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2010-01-13

  (C) Copyright 2010-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsComponentInterfaceProxyClient_h
#define _mtsComponentInterfaceProxyClient_h

#include <cisstMultiTask/mtsComponentInterfaceProxy.h>
#include "mtsProxyBaseClient.h"
#include <cisstMultiTask/mtsForwardDeclarations.h>

#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsComponentInterfaceProxyClient :
    public mtsProxyBaseClient<mtsComponentProxy>, public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    /*! Typedef for base type. */
    typedef mtsProxyBaseClient<mtsComponentProxy> BaseClientType;

    /*! Typedef for connected server proxy. */
    typedef mtsComponentInterfaceProxy::ComponentInterfaceServerPrx ComponentInterfaceServerProxyType;
    ComponentInterfaceServerProxyType ComponentInterfaceServerProxy;

protected:
    /*! Callback thread for bi-directional communication with server */
    class ComponentInterfaceClientI;
    typedef IceUtil::Handle<ComponentInterfaceClientI> ComponentInterfaceClientIPtr;
    ComponentInterfaceClientIPtr Server;
    ComponentInterfaceClientIPtr Client;

    /*! Typedef for per-event argument serializer */
    typedef std::map<mtsCommandIDType, mtsProxySerializer *> PerEventSerializerMapType;
    PerEventSerializerMapType PerEventSerializerMap;

    /*! Connection id that this proxy runs for. mtsComponentInterfaceProxyServer
        handles multiple network proxy clients (because multiple required
        interfaces can connect to a provided interface) with unique key as this
        connection id. */
    ConnectionIDType ConnectionID;

#if IMPROVE_ICE_THREADING
    /*! Signal to wait for internal Ice threads to run */
    osaThreadSignal * IceThreadInitEvent;
#endif

    /*! Instance counter used to set a short name of this thread */
    static unsigned int InstanceCounter;

    //-------------------------------------------------------------------------
    //  Proxy Implementation
    //-------------------------------------------------------------------------
    /*! Create provided interface proxy (server) and callback thread to
        communicate with server.
        This methods gets called by the base class (mtsProxyBaseClient). */
    void CreateProxy(void);

    /*! Destroy connected provided interface proxy including callback thread to
        communicate with server.
        This methods gets called by the base class (mtsProxyBaseClient). */
    void RemoveProxy(void);

    /*! Start callback thread to communicate with server (blocking call).
        Internally, mtsManagerProxyClient::ManagerClientI::Start() is called. */
    void StartClient(void);

    /*! Called when server disconnection is detected or any exception occurs. */
    void OnServerDisconnect(const Ice::Exception & ex);

    /*! Runner for server communication callback thread */
    static void Runner(ThreadArguments<mtsComponentProxy> * arguments);

    //-------------------------------------------------------------------------
    //  Event Handlers : Server -> Client
    //-------------------------------------------------------------------------
    /*! Test method */
    void ReceiveTestMessageFromServerToClient(const std::string & str) const;

    /*! Fetch pointers of function proxies from a required interface proxy at
        server side */
    bool ReceiveFetchFunctionProxyPointers(const std::string & requiredInterfaceName,
        mtsComponentInterfaceProxy::FunctionProxyPointerSet & functionProxyPointers) const;

    /*! Execute commands */
    void ReceiveExecuteCommandVoid(const mtsCommandIDType commandID,
                                   const mtsBlockingType blocking, mtsExecutionResult & executionResult);
    void ReceiveExecuteCommandWriteSerialized(const mtsCommandIDType commandID,
                                              const mtsBlockingType blocking, mtsExecutionResult & executionResult,
                                              const std::string & serializedArgument);
    void ReceiveExecuteCommandReadSerialized(const mtsCommandIDType commandID,
                                             mtsExecutionResult & executionResult,
                                             std::string & serializedArgument);
    void ReceiveExecuteCommandQualifiedReadSerialized(const mtsCommandIDType commandID,
                                                      mtsExecutionResult & executionResult,
                                                      const std::string & serializedArgumentIn, std::string & serializedArgumentOut);
    void ReceiveExecuteCommandVoidReturnSerialized(const mtsCommandIDType commandID,
                                                   const mtsObjectIDType resultAddress,
                                                   mtsExecutionResult & executionResult,
                                                   std::string & serializedResult);
    void ReceiveExecuteCommandWriteReturnSerialized(const mtsCommandIDType commandID,
                                                    const std::string & serializedArgument,
                                                    const mtsObjectIDType resultAddress,
                                                    mtsExecutionResult & executionResult,
                                                    std::string & serializedResult);

public:
    /*! Constructor and destructor */
    mtsComponentInterfaceProxyClient(const std::string & serverEndpointInfo,
                                     const ConnectionIDType connectionID);
    ~mtsComponentInterfaceProxyClient();

    /*! Entry point to run a proxy. */
    bool StartProxy(mtsComponentProxy * proxyOwner);

    /*! Stop the proxy (clean up thread-related resources) */
    void StopProxy(void);

    //-------------------------------------------------------------------------
    //  Event Generators (Event Sender) : Client -> Server
    //-------------------------------------------------------------------------
    /*! Test method */
    void SendTestMessageFromClientToServer(const std::string & str) const;

    /*! Register per-command (de)serializer */
    bool AddPerEventSerializer(const mtsCommandIDType commandID, mtsProxySerializer * serializer);

    /*! Fetch pointers of event generator proxies from a provided interface
        proxy at server side */
    bool SendFetchEventGeneratorProxyPointers(
        const std::string & clientComponentName, const std::string & requiredInterfaceName,
        mtsComponentInterfaceProxy::EventGeneratorProxyPointerSet & eventGeneratorProxyPointers);

    /*! Request execution of events */
    bool SendExecuteEventVoid(const mtsCommandIDType commandID);
    bool SendExecuteEventWriteSerialized(const mtsCommandIDType commandID, const mtsGenericObject & argument);
    bool SendExecuteEventReturnSerialized(const mtsCommandIDType commandID, const mtsObjectIDType resultAddress, const std::string & result);

    //-------------------------------------------------------------------------
    //  Interface Implementation (refer to mtsComponentInterfaceProxy.ice)
    //-------------------------------------------------------------------------
protected:
    class ComponentInterfaceClientI :
        public mtsComponentInterfaceProxy::ComponentInterfaceClient, public IceUtil::Monitor<IceUtil::Mutex>
    {
    private:
        /*! Ice objects */
        Ice::CommunicatorPtr Communicator;
        SenderThread<ComponentInterfaceClientIPtr> * SenderThreadPtr;
        Ice::LoggerPtr IceLogger;

        /*! Network event handler */
        mtsComponentInterfaceProxyClient * ComponentInterfaceProxyClient;

        /*! Connected server proxy */
        mtsComponentInterfaceProxy::ComponentInterfaceServerPrx Server;

    public:
        ComponentInterfaceClientI(
            const Ice::CommunicatorPtr& communicator,
            const Ice::LoggerPtr& logger,
            const mtsComponentInterfaceProxy::ComponentInterfaceServerPrx& server,
            mtsComponentInterfaceProxyClient * componentInterfaceProxyClient);
        ~ComponentInterfaceClientI();

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

        /*! Fetch pointers of function proxies from a required interface proxy at
            server side */
        bool FetchFunctionProxyPointers(const std::string &, mtsComponentInterfaceProxy::FunctionProxyPointerSet &, const ::Ice::Current & current) const;

        /*! Execute commands */
        void ExecuteCommandVoid(::Ice::Long, bool, ::Ice::Byte&, const ::Ice::Current&);
        void ExecuteCommandWriteSerialized(::Ice::Long, const ::std::string&, bool, ::Ice::Byte&, const ::Ice::Current&);
        void ExecuteCommandReadSerialized(::Ice::Long, ::std::string&, ::Ice::Byte&, const ::Ice::Current&);
        void ExecuteCommandQualifiedReadSerialized(::Ice::Long, const ::std::string&, ::std::string&, ::Ice::Byte&, const ::Ice::Current&);
        void ExecuteCommandVoidReturnSerialized(::Ice::Long commandId,
                                                ::Ice::Long resultAddress, ::std::string & result,
                                                ::Ice::Byte & executionResult, const ::Ice::Current&);
        void ExecuteCommandWriteReturnSerialized(::Ice::Long commandId, const ::std::string & argument,
                                                 ::Ice::Long resultAddress, ::std::string & result,
                                                 ::Ice::Byte & executionResult, const ::Ice::Current&);
    };
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsComponentInterfaceProxyClient)

#endif // _mtsComponentInterfaceProxyClient_h
