/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung
  Created on: 2010-01-13

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
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
#include <cisstMultiTask/mtsProxyBaseClient.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>

#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsComponentInterfaceProxyClient : public mtsProxyBaseClient<mtsComponentProxy>
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

    /*! Typedef for base type. */
    typedef mtsProxyBaseClient<mtsComponentProxy> BaseClientType;

    /*! Typedef for connected server proxy. */
    typedef mtsComponentInterfaceProxy::ComponentInterfaceServerPrx ComponentInterfaceServerProxyType;
    ComponentInterfaceServerProxyType ComponentInterfaceServerProxy;

protected:
    /*! Definitions for send thread */
    class ComponentInterfaceClientI;
    typedef IceUtil::Handle<ComponentInterfaceClientI> ComponentInterfaceClientIPtr;
    ComponentInterfaceClientIPtr Sender;

    /*! Typedef for per-event argument serializer */
    typedef std::map<CommandIDType, mtsProxySerializer *> PerEventSerializerMapType;
    PerEventSerializerMapType PerEventSerializerMap;

    /*! ID of this proxy. mtsComponentInterfaceProxyServer handles multiple
        network proxy clients (as a provided interface can be connected to
        multiple required interfaces) and it uses this ID as unique client ID. */
    unsigned int ProvidedInterfaceProxyInstanceID;

    /*! Instance counter used to set a short name of this thread */
    static unsigned int InstanceCounter;

    //-------------------------------------------------------------------------
    //  Proxy Implementation
    //-------------------------------------------------------------------------
    /*! Create a proxy object and a send thread. */
    void CreateProxy() {
        ComponentInterfaceServerProxy =
            mtsComponentInterfaceProxy::ComponentInterfaceServerPrx::checkedCast(ProxyObject);
        if (!ComponentInterfaceServerProxy) {
            throw "mtsComponentInterfaceProxyClient: Invalid proxy";
        }

        Sender = new ComponentInterfaceClientI(IceCommunicator, IceLogger, ComponentInterfaceServerProxy, this);
    }

    /*! Remove a proxy object */
    void RemoveProxy() {
        Sender->Stop();
    }

    /*! Start a send thread and wait for shutdown (blocking call). */
    void StartClient();

    /*! Called when server disconnection is detected */
    bool OnServerDisconnect();

    /*! Thread runner */
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
    void ReceiveExecuteCommandVoid(const CommandIDType commandID);
    void ReceiveExecuteCommandWriteSerialized(const CommandIDType commandID, const std::string & serializedArgument);
    void ReceiveExecuteCommandReadSerialized(const CommandIDType commandID, std::string & serializedArgument);
    void ReceiveExecuteCommandQualifiedReadSerialized(const CommandIDType commandID, const std::string & serializedArgumentIn, std::string & serializedArgumentOut);

public:
    /*! Constructor and destructor */
    mtsComponentInterfaceProxyClient(const std::string & serverEndpointInfo,
                                     const unsigned int providedInterfaceProxyInstanceID);
    ~mtsComponentInterfaceProxyClient();

    /*! Entry point to run a proxy. */
    bool Start(mtsComponentProxy * proxyOwner);

    /*! Stop the proxy (clean up thread-related resources) */
    void Stop(void);

    //-------------------------------------------------------------------------
    //  Event Generators (Event Sender) : Client -> Server
    //-------------------------------------------------------------------------
    /*! Test method */
    void SendTestMessageFromClientToServer(const std::string & str) const;

    /*! Register per-command (de)serializer */
    bool AddPerEventSerializer(const CommandIDType commandID, mtsProxySerializer * serializer);

    /*! Fetch pointers of event generator proxies from a provided interface
        proxy at server side */
    bool SendFetchEventGeneratorProxyPointers(
        const std::string & requiredInterfaceName, const std::string & providedInterfaceName,
        mtsComponentInterfaceProxy::EventGeneratorProxyPointerSet & eventGeneratorProxyPointers);

    /*! Request execution of events */
    bool SendExecuteEventVoid(const CommandIDType commandID);
    bool SendExecuteEventWriteSerialized(const CommandIDType commandID, const mtsGenericObject & argument);

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
        IceUtil::ThreadPtr SenderThreadPtr;
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
            mtsComponentInterfaceProxyClient * ComponentInterfaceClient);
        ~ComponentInterfaceClientI();

        /*! Proxy management */
        void Start();
        void Run();
        void Stop();

        /*! Getter */
        bool IsActiveProxy() const {
            if (ComponentInterfaceProxyClient) {
                return ComponentInterfaceProxyClient->IsActiveProxy();
            } else {
                return false;
            }
        }

        //-------------------------------------------------
        //  Event handlers (Server -> Client)
        //-------------------------------------------------
        /*! Test method */
        void TestMessageFromServerToClient(const std::string & str, const ::Ice::Current & current);

        /*! Fetch pointers of function proxies from a required interface proxy at
            server side */
        bool FetchFunctionProxyPointers(const std::string &, mtsComponentInterfaceProxy::FunctionProxyPointerSet &, const ::Ice::Current & current) const;

        /*! Execute commands */
        void ExecuteCommandVoid(::Ice::Long, const ::Ice::Current&);
        void ExecuteCommandWriteSerialized(::Ice::Long, const ::std::string&, const ::Ice::Current&);
        void ExecuteCommandReadSerialized(::Ice::Long, ::std::string&, const ::Ice::Current&);
        void ExecuteCommandQualifiedReadSerialized(::Ice::Long, const ::std::string&, ::std::string&, const ::Ice::Current&);
    };
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsComponentInterfaceProxyClient)

#endif // _mtsComponentInterfaceProxyClient_h
