/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung
  Created on: 2009-04-10

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights
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
#include <cisstCommon/cmnPath.h>

#include <cisstMultiTask/mtsConfig.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsManagerGlobal.h>

#include <cisstMultiTask/mtsExport.h>

#include <IceUtil/IceUtil.h>
#include <Ice/Ice.h>

/*!
  \ingroup cisstMultiTask

  This class implements basic features of ICE proxy such as proxy name, proxy
  state, and logging classes which are common in proxy server and proxy client
  object.
*/

/*! Typedef for command ID. These typedef cover both 32 and 64 bit pointers. */
typedef cmnDeSerializer::TypeId CommandIDType;
typedef ::Ice::Long IceCommandIDType;

/*! Enable/disable detailed log. This affects all proxy objects. */
//#define ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG

/*! Path to Ice property files */
#define ICE_PROPERTY_FILE_ROOT CISST_SOURCE_ROOT"/libs/etc/cisstMultiTask/Ice/"

//-----------------------------------------------------------------------------
//  Common Base Class Definitions
//-----------------------------------------------------------------------------
template<class _proxyOwner>
class CISST_EXPORT mtsProxyBaseCommon {

public:
    /*! Typedef for proxy type. */
    enum ProxyType { PROXY_SERVER, PROXY_CLIENT };

    /*! The proxy status definition. This definition is adopted from mtsTask.h
        with slight modification.

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
        PROXY_FINISHING    -- Set by either mtsProxyBaseServer::Stop() or
                              mtsProxyBaseClient::Stop() before trying to stop ICE
                              proxy processing.
        PROXY_FINISHED     -- Set by either mtsProxyBaseServer::Stop() or
                              mtsProxyBaseClient::Stop() after successful clean-up.
    */
    enum ProxyStateType {
        PROXY_CONSTRUCTED,
        PROXY_INITIALIZING,
        PROXY_READY,
        PROXY_ACTIVE,
        PROXY_FINISHING,
        PROXY_FINISHED
    };

protected:
    /*! Proxy owner */
    _proxyOwner * ProxyOwner;

    /*! Proxy type: PROXY_SERVER or PROXY_CLIENT */
    ProxyType ProxyTypeMember;

    /*! Proxy Name. Internally set as the name of this proxy owner's */
    std::string ProxyName;

    //-----------------------------------------------------
    //  Proxy State Management
    //-----------------------------------------------------
    /*! Proxy state */
    ProxyStateType ProxyState;

    /*! Change the proxy state as active. */
    void SetAsActiveProxy(void) {
        ChangeProxyState(PROXY_ACTIVE);
    }

    //-----------------------------------------------------
    // Auxiliary Class Definitions
    //-----------------------------------------------------
    /*! Logger class using the cisst's internal logging mechanism */
    class CisstLogger : public Ice::Logger
    {
    public:
        void print(const ::std::string & message) {
            CMN_LOG_RUN_VERBOSE << "ICE: " << message << std::endl;
        }
        void trace(const ::std::string & category, const ::std::string & message) {
            CMN_LOG_RUN_DEBUG << "ICE: " << category << " :: " << message << std::endl;
        }
        void warning(const ::std::string & message) {
            CMN_LOG_RUN_WARNING << "ICE: " << message << std::endl;
        }
        void error(const ::std::string & message) {
            CMN_LOG_RUN_ERROR << "ICE: " << message << std::endl;
        }
        ::Ice::LoggerPtr cloneWithPrefix(const ::std::string&) {
            // TODO: Ice-3.4 support
            return NULL;
        }
    };

    /*! Logger class using OutputDebugString() on Windows. */
#if (CISST_OS == CISST_WINDOWS)
    class WindowLogger : public Ice::Logger
    {
    public:
        void print(const ::std::string & message) {
            Log(message);
        }
        void trace(const ::std::string & category, const ::std::string & message) {
            std::string s;
            s += category;
            s += ": ";
            s += message;
            Log(s);
        }
        void warning(const ::std::string & message) {
            Log("##### WARNING: " + message);
        }
        void error(const ::std::string & message) {
            Log("##### ERROR: " + message);
        }
        ::Ice::LoggerPtr cloneWithPrefix(const ::std::string&) {
            // TODO: Ice-3.4 support
            return NULL;
        }

    protected:
        void Log(const std::string& log)
        {
            OutputDebugString(log.c_str());
        }
    };
#else
    typedef CisstLogger WindowLogger;
#endif

    /* Internal class for thread arguments */
    template<class __proxyOwner>
    class ThreadArguments {
    public:
        mtsProxyBaseCommon * Proxy;
        void (*Runner)(ThreadArguments<__proxyOwner> *);
    };

    /* Internal class for proxy worker */
    template<class __proxyOwner>
    class ProxyWorker {
    public:
        ProxyWorker(void) {}
        virtual ~ProxyWorker(void) {}

        void * Run(ThreadArguments<__proxyOwner> * arguments) {
            arguments->Runner(arguments);
            return 0;
        }
    };

    /* Internal class for send thread */
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
    /*! Mutex to change proxy state */
    osaMutex StateChange;

    /*! The flag which is true if a proxy is properly initialized */
    bool InitSuccessFlag;

    /*! The worker thread that actually runs a proxy. */
    osaThread WorkerThread;

    /*! The arguments container used for thread creation */
    ProxyWorker<_proxyOwner> ProxyWorkerInfo;
    ThreadArguments<_proxyOwner> ThreadArgumentsInfo;

    /*! Helper function to change the proxy state in a thread-safe manner */
    void ChangeProxyState(const enum ProxyStateType newProxyState) {
        StateChange.Lock();
        ProxyState = newProxyState;
        StateChange.Unlock();
    }

    //-----------------------------------------------------
    //  ICE Related
    //-----------------------------------------------------
    /*! The name of a property file that configures proxy connection settings. */
    //const std::string IcePropertyFileName;
	std::string IcePropertyFileName;

    /*! Ice initialization data */
    Ice::InitializationData IceInitData;

    /*! The proxy communicator. */
    Ice::CommunicatorPtr IceCommunicator;

    /*! The Ice default logger. */
    Ice::LoggerPtr IceLogger;

    /*! The global unique id of this Ice object. */
    std::string IceGUID;

    /*! Initialize Ice module. */
    virtual void IceInitialize(void) {
        ChangeProxyState(PROXY_INITIALIZING);

        // Load configuration file to set the properties of ICE proxy server
        IceInitData.properties = Ice::createProperties();
        IceInitData.properties->load(IcePropertyFileName);

        // Create a logger
        const std::string loggerProperty = IceInitData.properties->getProperty("Logger");
        if (loggerProperty == "Windows") {
            IceInitData.logger = new WindowLogger();
        } else {
            IceInitData.logger = new CisstLogger();
        }

        IceCommunicator = Ice::initialize(IceInitData);
        IceLogger = IceCommunicator->getLogger();
    }

    /*! Ice module clean up */
    virtual void IceCleanup(void) {
        IceGUID = "";
        InitSuccessFlag = false;

        ChangeProxyState(PROXY_FINISHED);
    }

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

public:
    /*! Constructor */
    mtsProxyBaseCommon(const std::string& propertyFileName, const ProxyType& proxyType) :
        ProxyTypeMember(proxyType),
        ProxyState(PROXY_CONSTRUCTED),
        InitSuccessFlag(false),
        IceCommunicator(NULL),
        IceGUID("")
    {
        cmnPath path;
        path.Add(ICE_PROPERTY_FILE_ROOT);
        path.AddFromEnvironment("PATH", cmnPath::TAIL);
        IcePropertyFileName = path.Find(propertyFileName);
        //IceUtil::CtrlCHandler ctrCHandler(onCtrlC);
    }

    virtual ~mtsProxyBaseCommon() {}

    /*! Initialize and start the proxy (returns immediately). */
    virtual bool Start(_proxyOwner * proxyOwner) = 0;

    /*! Deactivate this proxy */
    void Deactivate(void) {
        ProxyState = PROXY_FINISHING;
    }

    /*! Terminate the proxy. */
    virtual void Stop() = 0;

    /*! Set proxy owner and this proxy's name */
    virtual void SetProxyOwner(_proxyOwner * proxyOwner, const std::string & suffix = "") {
        ProxyOwner = proxyOwner;
        ProxyName = proxyOwner->GetName();
        ProxyName += suffix;
    }

    //-----------------------------------------------------
    //  Getters
    //-----------------------------------------------------
    inline const Ice::LoggerPtr GetLogger(void) const {
        return IceLogger;
    }

    /*! Check if this proxy is active */
    bool IsActiveProxy(void) const {
        return (ProxyState == PROXY_ACTIVE);
    }

    /*! Get the name of this proxy */
    inline std::string GetProxyName() const {
        return ProxyName;
    }
};

#define LogPrint(_className, _logStream) {\
        std::stringstream ss;\
        ss << #_className << ": ";\
        ss << _logStream;\
        std::string s = ss.str();\
        IceLogger->print(s); }

#define LogWarning(_className, _logStream) {\
        std::stringstream ss;\
        ss << #_className << ": ";\
        ss << _logStream;\
        std::string s = ss.str();\
        IceLogger->warning(s); }

#define LogError(_className, _logStream) {\
        std::stringstream ss;\
        ss << #_className << ": ";\
        ss << _logStream;\
        std::string s = ss.str();\
        IceLogger->error(s); }

#endif // _mtsProxyBaseCommon_h

/* TODO MEMO

   Connection closure: http://www.zeroc.com/doc/Ice-3.3.1/manual/Connections.38.6.html
*/
