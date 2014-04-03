/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2009-04-10

  (C) Copyright 2009-2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsProxyBaseCommon_h
#define _mtsProxyBaseCommon_h

#include <cisstCommon/cmnSerializer.h>
#include <cisstCommon/cmnDeSerializer.h>
#include <cisstCommon/cmnPath.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaMutex.h>

#include <cisstMultiTask/mtsConfig.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsManagerGlobal.h>

#include "mtsProxyConfig.h"

#include <cisstMultiTask/mtsExport.h>

#include <IceUtil/IceUtil.h>
#include <Ice/Ice.h>

/*!
  \ingroup cisstMultiTask

  This class implements basic features of ICE proxy such as proxy name, proxy
  state, and logging classes which are common in proxy server and proxy client
  object.
*/

/*! Typedef for command ID to handle both 32 and 64 bit pointers */
typedef cmnDeSerializer::TypeId mtsCommandIDType;
typedef cmnDeSerializer::TypeId mtsObjectIDType;
typedef ::Ice::Long mtsIceCommandIDType;

/*! Enable/disable detailed log. This affects all proxy objects. */
//#define ENABLE_DETAILED_MESSAGE_EXCHANGE_LOG

//-----------------------------------------------------------------------------
//  Common Base Class Definitions
//-----------------------------------------------------------------------------
template<class _proxyOwner>
class CISST_EXPORT mtsProxyBaseCommon {

public:
    /*! Typedef for proxy type. */
    enum ProxyTypes { PROXY_TYPE_UNDEFINED, PROXY_TYPE_SERVER, PROXY_TYPE_CLIENT };

    /*! The proxy status definition. This definition is adopted from mtsTask.h
        with slight modification.

        PROXY_STATE_CONSTRUCTED
            : Set by mtsProxyBaseCommon constructor.
              Initial state.

        PROXY_STATE_INITIALIZING
            : Set by mtsProxyBaseClient::IceInitialize() or
              mtsProxyBaseServer::IceInitialize(), both of which internally calls
              mtsProxyBaseCommon::IceInitialize().
              A proxy is created but has not been fully initialized yet.

        PROXY_STATE_READY
            : Set by either mtsProxyBaseServer::IceInitialize() or
              mtsProxyBaseClient::IceInitialize().
              This state represents that a proxy object is successfully
              initialized and is ready to run.

        PROXY_STATE_ACTIVE
            : Set by either mtsProxyBaseServer::SetAsActiveProxy() or
              mtsProxyBaseClient::SetAsActiveProxy().
              If a proxy is in this state, it is running and can process events.

        PROXY_STATE_FINISHING
            : Set by either mtsProxyBaseServer::Stop() or
              mtsProxyBaseClient::Stop() before trying to stop ICE proxy processing.

        PROXY_STATE_FINISHED
            : Set by either mtsProxyBaseServer::Stop() or
              mtsProxyBaseClient::Stop() after successful clean-up.
    */
    enum ProxyStateType {
        PROXY_STATE_CONSTRUCTED = 0,
        PROXY_STATE_INITIALIZING,
        PROXY_STATE_READY,
        PROXY_STATE_ACTIVE,
        PROXY_STATE_FINISHING,
        PROXY_STATE_FINISHED
    };

protected:
    /*! Proxy owner */
    _proxyOwner * ProxyOwner;

    /*! Proxy type: PROXY_TYPE_UNDEFINED, PROXY_TYPE_SERVER, PROXY_TYPE_CLIENT */
    ProxyTypes ProxyType;

    /*! Proxy Name. Internally set as the name of this proxy owner's */
    std::string ProxyName;

    void ProxyBaseInitialize(void);

    //-----------------------------------------------------
    //  Proxy State Management
    //-----------------------------------------------------
    /*! Proxy state */
    ProxyStateType ProxyState;

    /*! Helper function to change the proxy state in a thread-safe manner */
    void ChangeProxyState(const enum ProxyStateType newProxyState);

    //-----------------------------------------------------
    //  Thread Management
    //-----------------------------------------------------
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
        bool CanSend;

    public:
        SenderThread(const _SenderType& sender) : Sender(sender), CanSend(true) {}
        virtual void run() {
            if (CanSend) Sender->Run();
        }
        void StopSend(void) {
            CanSend = false;
        }
    };

    /*! Mutex to change proxy state */
    osaMutex StateChange;

    /*! The flag which is true if a proxy is properly initialized */
    bool InitSuccessFlag;

    /*! The worker thread that actually runs a proxy. */
    osaThread WorkerThread;

    /*! The arguments container used for thread creation */
    ProxyWorker<_proxyOwner> ProxyWorkerInfo;
    ThreadArguments<_proxyOwner> ThreadArgumentsInfo;

    //-----------------------------------------------------
    //  ICE Related
    //-----------------------------------------------------
    /*! The name of a property file that configures proxy connection settings. */
    //const std::string IcePropertyFileName;
	std::string IcePropertyFileName;

    /*! Ice initialization data */
    Ice::InitializationData IceInitData;

    /*! Ice proxy communicator -- the central object in Ice.
        See http://zeroc.com/doc/Ice-3.4.1-IceTouch/reference/Ice/Communicator.html
        for more details */
    Ice::CommunicatorPtr IceCommunicator;

    /*! Ice logger */
    Ice::LoggerPtr IceLogger;

    /*! Global unique id of this Ice object. */
    std::string IceGUID;

    /*! Initialize Ice module. */
    virtual void IceInitialize(void);

    /*! Ice module clean up. For Ice connection closure, see below:
        http://www.zeroc.com/doc/Ice-3.4.1-IceTouch/manual/Connections.37.6.html */
    virtual void IceCleanup(void);

    /*! Return the global unique id of this object. Currently, IceUtil::generateUUID()
        is used to set ID which guarantees the uniqueness of it across networks
        It is basically a mixture of IP address and/or MAC address, process id,
        and object id (or a pointer to this object). */
    std::string GetIceGUID() {
        if (IceGUID.empty()) {
            IceGUID = IceUtil::generateUUID();
        }
        return IceGUID;
    }

    //-----------------------------------------------------
    //  Auxiliary Class Definitions
    //-----------------------------------------------------
    /*! Logger class using the cisst's internal logging mechanism */
    class CisstLogger : public Ice::Logger
    {
    public:
        inline void print(const ::std::string & message) {
            CMN_LOG_RUN_VERBOSE << "ICE: " << message << std::endl;
        }
        inline void trace(const ::std::string & category, const ::std::string & message) {
            CMN_LOG_RUN_DEBUG << "ICE: " << category << " :: " << message << std::endl;
        }
        inline void warning(const ::std::string & message) {
            CMN_LOG_RUN_WARNING << "ICE: " << message << std::endl;
        }
        inline void error(const ::std::string & message) {
            CMN_LOG_RUN_ERROR << "ICE: " << message << std::endl;
        }
        // Support for Ice 3.4 or above (used by IceBox)
        // MJ: This does not get called in the current implementation and thus
        // can return null.  However, to be on the safe side, it returns a new
        // instance of itself.
        ::Ice::LoggerPtr cloneWithPrefix(const ::std::string&) {
            return new CisstLogger;
        }
    };

    /*! Logger class using OutputDebugString() on Windows. */
#if (CISST_OS == CISST_WINDOWS)
    class WindowLogger : public Ice::Logger
    {
    public:
        inline void print(const ::std::string & message) {
            Log(message);
        }
        inline void trace(const ::std::string & category, const ::std::string & message) {
            std::string s;
            s += category;
            s += ": ";
            s += message;
            Log(s);
        }
        inline void warning(const ::std::string & message) {
            Log("##### WARNING: " + message);
        }
        inline void error(const ::std::string & message) {
            Log("##### ERROR: " + message);
        }
        // Support for Ice 3.4 or above (used by IceBox)
        // MJ: This does not get called in the current implementation and thus
        // can return null.  However, to be on the safe side, it returns a new
        // instance of itself.
        ::Ice::LoggerPtr cloneWithPrefix(const ::std::string&) {
            return new WindowLogger;
        }

    protected:
        inline void Log(const std::string& log) {
            OutputDebugString(log.c_str());
        }
    };
#else
    typedef CisstLogger WindowLogger;
#endif

public:
    /*! Constructor and destructor */
    mtsProxyBaseCommon(const std::string& propertyFileName, const ProxyTypes& proxyType);
    virtual ~mtsProxyBaseCommon() {}

    /*! Initialize and start the proxy (returns immediately). */
    virtual bool StartProxy(_proxyOwner * proxyOwner) = 0;

    /*! Terminate the proxy. */
    virtual void StopProxy() = 0;

    /*! Set proxy owner and this proxy's name */
    virtual void SetProxyOwner(_proxyOwner * proxyOwner, const std::string & suffix = "");

    //-----------------------------------------------------
    //  Getters
    //-----------------------------------------------------
    inline const Ice::LoggerPtr GetLogger(void) const {
        return IceLogger;
    }

    /*! Getter for proxy state */
    inline bool IsActiveProxy(void) const {
        return (ProxyState == PROXY_STATE_ACTIVE);
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
        if (IceLogger) IceLogger->print(s);\
        else std::cout << s << std::endl; }

#define LogWarning(_className, _logStream) {\
        std::stringstream ss;\
        ss << #_className << ": ";\
        ss << _logStream;\
        std::string s = ss.str();\
        if (IceLogger) IceLogger->warning(s);\
        else std::cout << s << std::endl; }

#define LogError(_className, _logStream) {\
        std::stringstream ss;\
        ss << #_className << ": ";\
        ss << _logStream;\
        std::string s = ss.str();\
        if (IceLogger) IceLogger->error(s);\
        else std::cout << s << std::endl; }


template<class _proxyOwner>
mtsProxyBaseCommon<_proxyOwner>::mtsProxyBaseCommon(const std::string & propertyFileName,
                                                    const ProxyTypes & CMN_UNUSED(proxyType))
{
    ProxyBaseInitialize();

    cmnPath path;
#if CISST_HAS_IOS
    path.Add("./");
#else
    path.AddRelativeToCisstShare("cisstMultiTask/Ice");
    path.AddFromEnvironment("PATH", cmnPath::TAIL);
#endif
    IcePropertyFileName = path.Find(propertyFileName);
}

template<class _proxyOwner>
void mtsProxyBaseCommon<_proxyOwner>::ProxyBaseInitialize(void)
{
    ProxyOwner = 0;
    ProxyType = PROXY_TYPE_UNDEFINED;
    ProxyName = "ProxyNoName";
    ProxyState = PROXY_STATE_CONSTRUCTED;

    InitSuccessFlag = false;

    IcePropertyFileName = "";
    IceCommunicator = 0;
    IceLogger = 0;
    IceGUID = "";
}

template<class _proxyOwner>
void mtsProxyBaseCommon<_proxyOwner>::ChangeProxyState(const enum ProxyStateType newProxyState)
{
    StateChange.Lock();
    ProxyState = newProxyState;
    StateChange.Unlock();

#if 0
    switch (ProxyState) {
        case PROXY_STATE_CONSTRUCTED:  std::cout << "----------- Proxy state change: CONSTRUCTED" << std::endl;  break;
        case PROXY_STATE_INITIALIZING: std::cout << "----------- Proxy state change: INITIALIZING" << std::endl; break;
        case PROXY_STATE_READY:        std::cout << "----------- Proxy state change: READY" << std::endl;        break;
        case PROXY_STATE_ACTIVE:       std::cout << "----------- Proxy state change: ACTIVE" << std::endl;       break;
        case PROXY_STATE_FINISHING:    std::cout << "----------- Proxy state change: FINISHING" << std::endl;    break;
        case PROXY_STATE_FINISHED:     std::cout << "----------- Proxy state change: FINISHED" << std::endl;     break;
    }
#endif
}

template<class _proxyOwner>
void mtsProxyBaseCommon<_proxyOwner>::IceInitialize(void)
{
    ChangeProxyState(PROXY_STATE_INITIALIZING);

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

    // Initialize Ice communicator and Ice logger
    IceCommunicator = Ice::initialize(IceInitData);
    IceLogger = IceCommunicator->getLogger();
}

template<class _proxyOwner>
void mtsProxyBaseCommon<_proxyOwner>::IceCleanup(void)
{
    // Clean up Ice communicator and Ice logger
    IceLogger = 0;
    IceCommunicator = 0;

    ChangeProxyState(PROXY_STATE_FINISHED);
}

template<class _proxyOwner>
void mtsProxyBaseCommon<_proxyOwner>::SetProxyOwner(_proxyOwner * proxyOwner,
                                                            const std::string & suffix)
{
    ProxyOwner = proxyOwner;
    ProxyName = proxyOwner->GetName();
    ProxyName += suffix;
}

#endif // _mtsProxyBaseCommon_h
