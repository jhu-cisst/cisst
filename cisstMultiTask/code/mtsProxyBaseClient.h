/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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

#ifndef _mtsProxyBaseClient_h
#define _mtsProxyBaseClient_h

#include "mtsProxyBaseCommon.h"

#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  This class is derived from mtsProxyBaseCommon and implements the basic
  structure and functions for ICE proxy client.  The actual processing routine
  should be implemented by derived classes.

  Compared to mtsProxyBaseServer, this base class for Ice client proxy supports
  a single connection to a server proxy because the current cisstMultiTask
  design allows a required interface to connect to only one provided interface.
*/

template<class _proxyOwner>
class CISST_EXPORT mtsProxyBaseClient: public mtsProxyBaseCommon<_proxyOwner>
{
protected:
    typedef mtsProxyBaseCommon<_proxyOwner> BaseType;

    /*! ICE Object */
    Ice::ObjectPrx ProxyObject;

    /*! Endpoint information used to connect to server. This information is
        feteched from the global component manager. */
    const std::string EndpointInfo;

    /*! Start proxy client. Entry point to initialize Ice proxy client objects.
        Gets called by user (application) */
    virtual bool StartProxy(_proxyOwner * proxyOwner) = 0;

    /*! Initialize Ice proxy client.  Called by StartProxy(). */
    virtual void IceInitialize(void);

    /*! Create ICE proxy client object */
    virtual void CreateProxy(void) = 0;

    /*! Called whenever server disconnection is detected */
    virtual void OnServerDisconnect(const Ice::Exception & ex) = 0;

    /*! Remove ICE proxy client object */
    virtual void RemoveProxy(void) = 0;

    /*! Clean up ICE related resources */
    virtual void IceCleanup(void);

    /*! Stop and clean up proxy client */
    virtual void StopProxy(void);

    ///*! Shutdown the current session for graceful termination */
    //void ShutdownSession(const Ice::Current & current) {
    //    current.adapter->getCommunicator()->shutdown();
    //    BaseType::ShutdownSession();
    //}

public:
    /*! Constructor and destructor */
    mtsProxyBaseClient(const std::string & propertyFileName, const std::string & endpointInfo)
        : BaseType(propertyFileName, BaseType::PROXY_TYPE_CLIENT), EndpointInfo(endpointInfo)
    {}

    virtual ~mtsProxyBaseClient() {}
};

template<class _proxyOwner>
void mtsProxyBaseClient<_proxyOwner>::IceInitialize(void)
{
    try {
        BaseType::IceInitialize();

        // Create an Ice proxy using stringfied proxy information
        ProxyObject = this->IceCommunicator->stringToProxy(EndpointInfo);

        // If a proxy fails to be created, an exception is thrown.
        CreateProxy();

        this->InitSuccessFlag = true;
        this->ChangeProxyState(BaseType::PROXY_STATE_READY);

        this->IceLogger->trace("mtsProxyBaseClient", "ICE init - Client proxy initialization success.");
    } catch (const Ice::ConnectionRefusedException & e) {
        if (this->IceLogger) {
            this->IceLogger->error("mtsProxyBaseClient: ICE init - Connection refused. Check if server is running.");
            this->IceLogger->trace("mtsProxyBaseClient", e.what());
        } else {
            CMN_LOG_RUN_ERROR << "mtsProxyBaseClient: ICE init - Connection refused. Check if server is running." << std::endl;
            CMN_LOG_RUN_ERROR << "mtsProxyBaseClient: " << e.what() << std::endl;
        }
    } catch (const Ice::Exception & e) {
        if (this->IceLogger) {
            this->IceLogger->error("mtsProxyBaseClient: ICE init - Client proxy initialization error");
            this->IceLogger->trace("mtsProxyBaseClient", e.what());
        } else {
            CMN_LOG_RUN_ERROR << "mtsProxyBaseClient: ICE init - Client proxy initialization error." << std::endl;
            CMN_LOG_RUN_ERROR << "mtsProxyBaseClient: " << e.what() << std::endl;
        }
    } catch (...) {
        if (this->IceLogger) {
            std::string msg("mtsProxyBaseClient: ICE init - exception: Endpoint = ");
            msg += "\"";
            msg += EndpointInfo;
            msg += "\"";
            this->IceLogger->error(msg);
        } else {
            CMN_LOG_RUN_ERROR << "mtsProxyBaseClient: ICE init - exception: Endpoint = " << EndpointInfo << std::endl;
        }
    }

    if (!this->InitSuccessFlag) {
        try {
            this->IceCommunicator->destroy();
        } catch (const Ice::Exception & e) {
            if (this->IceLogger) {
                std::string msg("mtsProxyBaseClient: ICE init - Client proxy clean-up error: Endpoint = ");
                msg += "\"";
                msg += EndpointInfo;
                msg += "\", ";
                msg += e.what();
                this->IceLogger->error(msg);
                this->IceLogger->trace("mtsProxyBaseClient", msg);
            } else {
                CMN_LOG_RUN_ERROR << "mtsProxyBaseClient: ICE init - Client proxy clean-up error: \"" << EndpointInfo << "\"" << std::endl;
                CMN_LOG_RUN_ERROR << "mtsProxyBaseClient: " << e.what() << std::endl;
            }
        }
    }
}

template<class _proxyOwner>
void mtsProxyBaseClient<_proxyOwner>::IceCleanup(void)
{
    this->ChangeProxyState(BaseType::PROXY_STATE_FINISHING);
    this->InitSuccessFlag = false;

    RemoveProxy();
}

template<class _proxyOwner>
void mtsProxyBaseClient<_proxyOwner>::StopProxy(void)
{
    IceCleanup();

    if (this->IceCommunicator) {
        try {
            this->IceCommunicator->destroy();
            this->IceCommunicator = 0;
            this->IceLogger->trace("mtsProxyBaseClient", "Proxy client clean-up success");
        } catch (const Ice::Exception& e) {
            this->IceLogger->trace("mtsProxyBaseClient", "Proxy client clean-up failure");
            this->IceLogger->trace("mtsProxyBaseClient", e.what());
        } catch (...) {
            this->IceLogger->error("mtsProxyBaseClient: Proxy client clean-up failure");
        }
    }

    BaseType::IceCleanup();
}


#endif // _mtsProxyBaseClient_h
