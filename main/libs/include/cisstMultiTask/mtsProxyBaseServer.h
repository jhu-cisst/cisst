/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsProxyBaseServer.h 142 2009-03-11 23:02:34Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2009-03-17

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsProxyBaseServer_h
#define _mtsProxyBaseServer_h

#include <cisstMultiTask/mtsProxyBaseCommon.h>
#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  This class inherits from mtsProxyBaseCommon and implements the basic structure of
  the ICE server which includes an independent logger, an adapter, and a servant.
  The actual processing routines are implemented by a class inherited from this
  class.

  TODO: Read an ICE property configuration file and set the properties of proxy
  connection based on it.
*/
template<class _argumentType>
class CISST_EXPORT mtsProxyBaseServer : public mtsProxyBaseCommon<_argumentType> {
public:
    typedef mtsProxyBaseCommon<_argumentType> BaseType;
    
protected:
    Ice::ObjectAdapterPtr IceAdapter;
    Ice::ObjectPtr Servant;
    const std::string AdapterName;
    const std::string EndpointInfo;
    const std::string CommunicatorID;

    virtual Ice::ObjectPtr CreateServant() = 0;

    void IceInitialize(void)
    {
        try {
            ChangeProxyState(BaseType::PROXY_INITIALIZING);

            Ice::InitializationData initData;
            initData.logger = new typename BaseType::ProxyLogger();
            initData.properties = Ice::createProperties();
            // There are two different modes of using implicit context: 
            // shared vs. PerThread.
            // (see http://www.zeroc.com/doc/Ice-3.3.1/manual/Adv_server.33.12.html)
            initData.properties->setProperty("Ice.ImplicitContext", "Shared");
            //initData.properties->load(IcePropertyFileName);           
            this->IceCommunicator = Ice::initialize(initData);
            
            // Create Logger
            this->IceLogger = this->IceCommunicator->getLogger();

            // Create an adapter (server-side only)
            IceAdapter = this->IceCommunicator->
                createObjectAdapterWithEndpoints(AdapterName, EndpointInfo);

            // Create a servant
            Servant = CreateServant();

            // Inform the object adapter of the presence of a new servant
            IceAdapter->add(Servant, this->IceCommunicator->stringToIdentity(CommunicatorID));

            // Activate the adapter. The adapter is initially created in a 
            // holding state. The server starts to process incoming requests
            // from clients as soon as the adapter is activated.
            IceAdapter->activate();

            this->InitSuccessFlag = true;
            this->Runnable = true;
            
            ChangeProxyState(BaseType::PROXY_READY);

            this->IceLogger->trace("mtsProxyBaseServer", "Server proxy initialization success.");
        } catch (const Ice::Exception& e) {
            if (this->IceLogger) {
                this->IceLogger->trace("mtsProxyBaseServer", "Server proxy initialization error");
                this->IceLogger->trace("mtsProxyBaseServer", e.what());
            } else {
                std::cerr << "mtsProxyBaseServer: Server proxy initialization error." << std::endl;
                std::cerr << "mtsProxyBaseServer: " << e.what() << std::endl;
            }
        } catch (const char * msg) {
            if (this->IceLogger) {
                this->IceLogger->trace("mtsProxyBaseServer", "Server proxy initialization error");
                this->IceLogger->trace("mtsProxyBaseServer", msg);
            } else {
                std::cerr << "mtsProxyBaseServer: Server proxy initialization error." << std::endl;
                std::cerr << "mtsProxyBaseServer: " << msg << std::endl;
            }
        }

        if (!this->InitSuccessFlag) {
            try {
                this->IceCommunicator->destroy();
            } catch (const Ice::Exception & e) {
                if (this->IceLogger) {
                    this->IceLogger->trace("mtsProxyBaseServer", "Server proxy clean-up error");
                    this->IceLogger->trace("mtsProxyBaseServer", e.what());
                } else {
                    std::cerr << "mtsProxyBaseServer: Server proxy clean-up error." << std::endl;
                    std::cerr << e.what() << std::endl;
                }
            }
        }
    }

public:
    mtsProxyBaseServer(const std::string & adapterName,
                       const std::string & endpointInfo,
                       const std::string & communicatorID):
        BaseType("", "", BaseType::PROXY_SERVER),
        AdapterName(adapterName),
        EndpointInfo(endpointInfo),
        CommunicatorID(communicatorID)        
    {}
    virtual ~mtsProxyBaseServer() {}
    
    virtual void Start(_argumentType * callingClass) = 0;

    virtual void SetAsActiveProxy() = 0;

    /* Return true if the current proxy state is active. */
    virtual void OnEnd()
    {
        if (this->ProxyState != BaseType::PROXY_ACTIVE) {
            return;
        }

        if (this->ProxyState == BaseType::PROXY_ACTIVE) {
            ChangeProxyState(BaseType::PROXY_FINISHING);

            if (this->IceCommunicator) {                
                try {
                    this->IceCommunicator->destroy();

                    this->ChangeProxyState(BaseType::PROXY_FINISHED);
                    this->IceLogger->trace("mtsProxyBaseServer", "Server proxy clean-up success.");
                } catch (const Ice::Exception & e) {
                    this->IceLogger->trace("mtsProxyBaseServer", "Server proxy clean-up failure.");
                    this->IceLogger->trace("mtsProxyBaseServer", e.what());
                }
            }
        }
    }
};

#endif // _mtsProxyBaseServer_h

