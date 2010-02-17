/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsProxyBaseClient.h 142 2009-03-11 23:02:34Z mjung5 $

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

#include <cisstMultiTask/mtsProxyBaseCommon.h>

#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  This class inherits mtsProxyBaseCommon and implements the basic structure of
  ICE proxy object acting as a client. The actual processing routines should be
  implemented by a derived class.

  Compared to mtsProxyBaseServer, this base class allows only one connection,
  i.e., one server proxy because one required interface can connect to only one
  provided interface in the current cisstMultiTask design.
*/

template<class _proxyOwner>
class CISST_EXPORT mtsProxyBaseClient: public mtsProxyBaseCommon<_proxyOwner> {

public:
    typedef mtsProxyBaseCommon<_proxyOwner> BaseType;

protected:
    /*! Start proxy client */
    virtual bool Start(_proxyOwner * proxyOwner) = 0;

    /*! Called when server disconnection is detected */
    virtual bool OnServerDisconnect() = 0;

    /*! Terminate proxy */
    virtual void Stop(void)
    {
        IceCleanup();

        if (this->IceCommunicator) {
            try {                    
                this->IceCommunicator->destroy();
                this->IceCommunicator = NULL;
                this->IceLogger->trace("mtsProxyBaseClient", "Proxy client clean-up success.");
            } catch (const Ice::Exception& e) {
                this->IceLogger->trace("mtsProxyBaseClient", "Proxy client clean-up failure.");
                this->IceLogger->trace("mtsProxyBaseClient", e.what());
            } catch (const std::string& msg) {
                this->IceLogger->error("mtsProxyBaseClient: Proxy client clean-up failure.");
                this->IceLogger->trace("mtsProxyBaseClient", msg.c_str());
            } catch (const char* msg) {
                this->IceLogger->error("mtsProxyBaseClient: Proxy client clean-up failure.");
                this->IceLogger->trace("mtsProxyBaseClient", msg);
            }
        }
    }

    //-------------------------------------------------------------------------
    //  Networking: ICE
    //-------------------------------------------------------------------------
    /*! ICE Object */
    Ice::ObjectPrx ProxyObject;

    /*! Endpoint information to connect to proxy server. This information is
        feteched from  the global component manager. */
    const std::string EndpointInfo;

    /*! Create ICE proxy client object */
    virtual void CreateProxy() = 0;

    /*! Remove ICE proxy client object */
    virtual void RemoveProxy() = 0;

    /*! Initialize client proxy */
    void IceInitialize(void)
    {
        try {
            BaseType::IceInitialize();

            // Create a proxy object from stringfied proxy information
            std::string stringfiedProxy = EndpointInfo;
            ProxyObject = this->IceCommunicator->stringToProxy(stringfiedProxy);

            // If a proxy fails to be created, an exception is thrown.
            CreateProxy();

            this->InitSuccessFlag = true;
            
            ChangeProxyState(BaseType::PROXY_READY);

            this->IceLogger->trace("mtsProxyBaseClient", "Client proxy initialization success.");
        } catch (const ::Ice::ConnectionRefusedException & e) {
            if (this->IceLogger) {
                this->IceLogger->error("mtsProxyBaseClient: Connection refused. Check if server is running.");
                this->IceLogger->trace("mtsProxyBaseClient", e.what());
            } else {
                std::cout << "mtsProxyBaseClient: Connection refused. Check if server is running." << std::endl;
                std::cout << "mtsProxyBaseClient: " << e.what() << std::endl;
            }
        } catch (const ::Ice::Exception & e) {
            if (this->IceLogger) {
                this->IceLogger->error("mtsProxyBaseClient: Client proxy initialization error");
                this->IceLogger->trace("mtsProxyBaseClient", e.what());
            } else {
                std::cout << "mtsProxyBaseClient: Client proxy initialization error." << std::endl;
                std::cout << "mtsProxyBaseClient: " << e.what() << std::endl;
            }
        }

        if (!this->InitSuccessFlag) {
            try {
                this->IceCommunicator->destroy();
            } catch (const Ice::Exception& e) {
                if (this->IceLogger) {
                    this->IceLogger->error("mtsProxyBaseClient: Client proxy clean-up error");
                    this->IceLogger->trace("mtsProxyBaseClient", e.what());
                } else {
                    std::cerr << "mtsProxyBaseClient: Client proxy clean-up error." << std::endl;
                    std::cerr << e.what() << std::endl;
                }
            }
        }
    }

    /*! Clean up ICE related resources */
    virtual void IceCleanup(void)
    {
        this->InitSuccessFlag = false;

        RemoveProxy();

        BaseType::IceCleanup();
    }

    ///*! Shutdown the current session for graceful termination */
    //void ShutdownSession(const Ice::Current & current) {
    //    current.adapter->getCommunicator()->shutdown();
    //    BaseType::ShutdownSession();
    //}

public:
    /*! Constructor */
    mtsProxyBaseClient(const std::string & propertyFileName, const std::string & endpointInfo)
        : BaseType(propertyFileName, BaseType::PROXY_CLIENT), EndpointInfo(endpointInfo)
    {}

    /*! Destructor (do nothing) */
    virtual ~mtsProxyBaseClient() {}
};

#endif // _mtsProxyBaseClient_h

