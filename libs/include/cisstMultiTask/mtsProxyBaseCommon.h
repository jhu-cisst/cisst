/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsProxyBaseCommon.h 142 2009-03-11 23:02:34Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2009-04-10

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsProxyBaseCommon_h
#define _mtsProxyBaseCommon_h

#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaMutex.h>
#include <cisstCommon/cmnSerializer.h>
#include <cisstCommon/cmnDeSerializer.h>

#include <cisstMultiTask/mtsConfig.h>
#include <cisstMultiTask/mtsExport.h>

#include <IceUtil/IceUtil.h>
#include <Ice/Ice.h>


/*!
  \ingroup cisstMultiTask

  This class defines core functions and data members that are common in both 
  clients and servers are required to be implemented in order to use ICE.

  If a new server or client proxy class is to be defined using ICE, they should
  be derived from either mtsProxyBaseServer or mtsProxyBaseClient, respectively,
  rather than inheriting from this class.

  Term definitions for proxy objects:

     Proxy server: a proxy that WORKS AS a server and runs at the server side
     Proxy client: a proxy that WORKS AS a client and runs at the client side
     Server proxy: a virtual server that runs at the client side
     Client proxy: a virtual client that runs at the server side
*/

//-----------------------------------------------------------------------------
//  Definitions for constant string values
//-----------------------------------------------------------------------------
/*! Implicit per-proxy context to set connection id. */
#define CONNECTION_ID "ConnectionID"

/*! The base port number is assigned as follows, which is not registered yet to 
IANA (Internet Assigned Numbers Authority) as of June 25th, 2009. 
See http://www.iana.org/assignments/port-numbers for more details.

Current port number assignment for proxies
-----------------------------------------------------------
Task manager layer  : 10705 (e.g. the global task manager)
Task layer          : 11705 (e.g. the server task)
*/
#define BASE_PORT_NUMBER_TASK_MANAGER_LAYER 10705
#define BASE_PORT_NUMBER_TASK_LAYER         11705

/*! Typedef for Command ID */
#define CommandIDType unsigned long long
#define IceCommandIDType Long

//-----------------------------------------------------------------------------
//  Common Base Class Definitions
//-----------------------------------------------------------------------------
template<class _ArgumentType>
class CISST_EXPORT mtsProxyBaseCommon {
public:
    /*! Typedef for proxy type. */
    enum ProxyType { PROXY_SERVER, PROXY_CLIENT };

    /*! The proxy status definition. This is adopted from mtsTask.h with slight
        modification.

        PROXY_CONSTRUCTED  -- Set by mtsProxyBaseCommon constructor. 
                              Initial state.
        PROXY_INITIALIZING -- Set by either mtsProxyBaseServer::IceInitialize() or
                              mtsProxyBaseClient::IceInitialize().
                              This state means a proxy object is created but not 
                              yet successfully initialized.
        PROXY_READY        -- Set by either mtsProxyBaseServer::IceInitialize() or
                              mtsProxyBaseClient::IceInitialize().
                              This state represents that a proxy object is 
                              successfully initialized and is ready to run.
        PROXY_ACTIVE       -- Set by either mtsProxyBaseServer::SetAsActiveProxy() 
                              or mtsProxyBaseClient::SetAsActiveProxy().
                              If a proxy is in this state, it is running and can 
                              process events.
        PROXY_FINISHING    -- Set by either mtsProxyBaseServer::OnEnd() or
                              mtsProxyBaseClient::OnEnd() before trying to stop ICE 
                              proxy processing.
        PROXY_FINISHED     -- Set by either mtsProxyBaseServer::OnEnd() or
                              mtsProxyBaseClient::OnEnd() after successful clean-up.
    */
    enum ProxyStateType { 
        PROXY_CONSTRUCTED, 
        PROXY_INITIALIZING, 
        PROXY_READY,
        PROXY_ACTIVE, 
        PROXY_FINISHING, 
        PROXY_FINISHED 
    };

    class ProxyLogger : public Ice::Logger
    {
    public:
        void print(const ::std::string & message) {
            CMN_LOG_RUN_VERBOSE << "ICE: " << message << std::endl;
            // Log(log);
        }
        void trace(const ::std::string & category, const ::std::string & message) {
            CMN_LOG_RUN_DEBUG << "ICE: " << category << " :: " << message << std::endl;
            // Log(log1, log2);
        }
        void warning(const ::std::string & message) {
            CMN_LOG_RUN_WARNING << "ICE: " << message << std::endl;
            // Log("##### WARNING: " + log);
        }
        void error(const ::std::string & message) {
            CMN_LOG_RUN_ERROR << "ICE: " << message << std::endl;
            // Log("##### ERROR: " + log);
        }

        void Log(const std::string & className, const std::string & description) {
            std::string log = className + ": ";
            log += description;

            Log(log);
        }

    protected:
        void Log(const std::string& log)
        {
        //
        // TODO: need to diversify the output?
        //
#if (CISST_OS == CISST_WINDOWS)
            OutputDebugString(log.c_str());
#else
            std::cout << log.c_str() << std::endl;
#endif
        }        
    };

protected:
    ProxyType ProxyTypeMember;
    ProxyStateType ProxyState;

    //-----------------------------------------------------
    // Auxiliary Class Definition
    //-----------------------------------------------------
    template<class __ArgumentType>
    class ThreadArguments {
    public:
        _ArgumentType * argument;
        mtsProxyBaseCommon * proxy;
        void (*Runner)(ThreadArguments<__ArgumentType> *);
    };

    template<class __ArgumentType>
    class ProxyWorker {
    public:
        ProxyWorker(void) {}
        virtual ~ProxyWorker(void) {}

        void * Run(ThreadArguments<__ArgumentType> * arguments) {
            arguments->Runner(arguments);
            return 0;
        }
    };

    template<class _SenderType>
    class SenderThread : public IceUtil::Thread
    {
    private:
        const _SenderType Sender;

    public:
        SenderThread(const _SenderType& sender) : Sender(sender) {}          
        virtual void run() { Sender->Run(); }
    };

    //-----------------------------------------------------
    //  Thread Management
    //-----------------------------------------------------
    /*! Mutex to change the proxy state. */
    osaMutex StateChange;

    /*! The flag which is true only if all initiliazation process succeeded. */
    bool InitSuccessFlag;

    /*! The flag which is true only if this proxy is runnable. */
    bool Runnable;

    /*! Set as true when a session is to be closed.
        For a client, this is set when a client notifies a server of disconnection.
        For a server, this is set when a client calls Shutdown() which allows safe
        and clean termination. */
    bool IsValidSession;

    /*! The worker thread that actually runs a proxy. */
    osaThread WorkerThread;

    /*! The arguments container used for thread creation */
    ProxyWorker<_ArgumentType> ProxyWorkerInfo;
    ThreadArguments<_ArgumentType> ThreadArgumentsInfo;

    /*! Helper function to change the proxy state. */
    void ChangeProxyState(const enum ProxyStateType newProxyState) {
        StateChange.Lock();
        ProxyState = newProxyState;
        StateChange.Unlock();
    }

    //-----------------------------------------------------
    //  ICE Related
    //-----------------------------------------------------
    /*! The name of a property file that configures proxy connection settings. */
    const std::string IcePropertyFileName;

    /*! The identity of an Ice object which can also be set through an Ice property file
        (not supported yet). */
    const std::string IceObjectIdentity;

    /*! The proxy communicator. */
    Ice::CommunicatorPtr IceCommunicator;

    /*! The Ice default logger. */
    Ice::LoggerPtr IceLogger;

    /*! The global unique id of this Ice object. */
    std::string IceGUID;

    /*! Initialize Ice module. */
    virtual void IceInitialize(void) = 0;

    /*! Return the global unique id of this object. Currently, IceUtil::generateUUID()
        is used to set the id which is guaranteed to be unique across networks by ICE.
        We can also use a combination of IP address (or MAC address), process id,
        and object id (or a pointer to this object) as the GUID. */
    std::string GetGUID() {
        if (IceGUID.empty()) {
            IceGUID = IceUtil::generateUUID();
        }
        return IceGUID;
    }

    //-----------------------------------------------------
    //  Serialization and Deserialization
    //-----------------------------------------------------
    /*! Buffers for serialization and deserialization. */
    std::stringstream SerializationBuffer;
    std::stringstream DeSerializationBuffer;

    /*! Per-proxy Serializer and DeSerializer. */
    cmnSerializer * Serializer;
    cmnDeSerializer * DeSerializer;

    void Serialize(const mtsGenericObject & argument, std::string & serializedData)
    {
        SerializationBuffer.str("");    
        Serializer->Serialize(argument);

        serializedData = SerializationBuffer.str();
    }

public:
    mtsProxyBaseCommon(const std::string& propertyFileName,
                       const std::string& objectIdentity,
                       const ProxyType proxyType) :
        ProxyTypeMember(proxyType),
        ProxyState(PROXY_CONSTRUCTED),
        InitSuccessFlag(false),
        Runnable(false),
        IsValidSession(true),
        IcePropertyFileName(propertyFileName),
        IceObjectIdentity(objectIdentity),
        IceCommunicator(NULL),
        IceGUID("")
    {
        //IceUtil::CtrlCHandler ctrCHandler(onCtrlC);

        Serializer = new cmnSerializer(SerializationBuffer);
        DeSerializer = new cmnDeSerializer(DeSerializationBuffer);
    }

    virtual ~mtsProxyBaseCommon() 
    {
        delete Serializer;
        delete DeSerializer;
    }

    /*! Initialize and start the proxy (returns immediately). */
    virtual void Start(_ArgumentType * callingClass) = 0;
    
    /*! Terminate the proxy. */
    virtual void Stop() = 0;

    /*! Close a session. */
    virtual void ShutdownSession()
    {
        IsValidSession = false;
    }

    /*! Called when the worker thread terminates. */
    virtual void OnEnd(void) = 0;

    //-----------------------------------------------------
    //  Getters
    //-----------------------------------------------------
    inline bool IsInitalized(void) const  { return InitSuccessFlag; }
    
    inline const Ice::LoggerPtr GetLogger() const { return IceLogger; }

    inline Ice::CommunicatorPtr GetIceCommunicator() const { return IceCommunicator; }
};

#endif // _mtsProxyBaseCommon_h
