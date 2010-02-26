/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung
  Created on: 2009-03-17

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsProxyBaseServer_h
#define _mtsProxyBaseServer_h

#include <cisstOSAbstraction/osaMutex.h>
#include <cisstMultiTask/mtsProxyBaseCommon.h>

#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  This class inherits mtsProxyBaseCommon and implements the basic structure and
  common functionalities of ICE proxy server. They include proxy server setup,
  proxy initialization, multiple client manage, and connection management.
  Note that this proxy server manages multiple clients regardless of its type
  since the type is templated.
*/
template<class _proxyOwner, class _clientProxyType, class _clientIDType>
class CISST_EXPORT mtsProxyBaseServer : public mtsProxyBaseCommon<_proxyOwner>
{
public:
    typedef mtsProxyBaseCommon<_proxyOwner> BaseType;
    typedef _clientProxyType ClientProxyType;
    typedef mtsProxyBaseServer<_proxyOwner, _clientProxyType, _clientProxyType> ThisType;

    /*! Typedef for proxy connection id (defined by ICE). Set as Ice::Identity
        which can be transformed to std::string by identityToString().
        See http://www.zeroc.com/doc/Ice-3.3.1/reference/Ice/Identity.html */
    typedef std::string ConnectionIDType;

    /*! Typedef for client id */
    typedef _clientIDType ClientIDType;

    /*! Start proxy server */
    virtual bool Start(_proxyOwner * proxyOwner) = 0;

    /*! Terminate proxy */
    virtual void Stop(void)
    {
        if (this->ProxyState != BaseType::PROXY_ACTIVE) {
            return;
        }

        ChangeProxyState(BaseType::PROXY_FINISHING);

        if (this->IceCommunicator) {
            try {
                this->IceCommunicator->destroy();
                this->ChangeProxyState(BaseType::PROXY_FINISHED);
                this->IceLogger->trace("mtsProxyBaseServer", "Proxy server clean-up success.");
            } catch (const Ice::Exception & e) {
                this->IceLogger->error("mtsProxyBaseServer: Proxy server clean-up failure.");
                this->IceLogger->trace("mtsProxyBaseServer", e.what());
            } catch (const std::string& msg) {
                this->IceLogger->error("mtsProxyBaseServer: Proxy server clean-up failure.");
                this->IceLogger->trace("mtsProxyBaseServer", msg.c_str());
            } catch (const char* msg) {
                this->IceLogger->error("mtsProxyBaseServer: Proxy server clean-up failure.");
                this->IceLogger->trace("mtsProxyBaseServer", msg);
            }
        }
    }

    /*! Called when a client disconnection is detected */
    virtual bool OnClientDisconnect(const ClientIDType clientID) = 0;

    //-------------------------------------------------------------------------
    //  Networking: ICE
    //-------------------------------------------------------------------------
protected:
    /*! ICE objects */
    Ice::ObjectAdapterPtr IceAdapter;
    Ice::ObjectPtr Servant;

    /*! Set as true when using dynamic port allocation (true by default) */
    bool DynamicPortAllocation;

    /*! Endpoint information that clients uses to connect to this server */
    const std::string AdapterName;
    const std::string CommunicatorID;
    std::string EndpointInfo;

    /*! Create ICE servant object */
    virtual Ice::ObjectPtr CreateServant() = 0;

    /*! Initialize server proxy */
    void IceInitialize(void)
    {
        try {
            BaseType::IceInitialize();

            // Determine a port number and generate an endpoint string
            std::string endpoint(":default");
            if (!DynamicPortAllocation) {
                // Fetch a port number from ice property file. Currently, only
                // the GCM uses this feature.
                const std::string portNumber = this->IceInitData.properties->getProperty("GCM.Port");
                endpoint += " -p ";
                endpoint += portNumber;
            }

            // Create an adapter (server-side only)
            // (http://www.zeroc.com/doc/Ice-3.3.1/reference/Ice/ObjectAdapter.html)
            IceAdapter = this->IceCommunicator->
                createObjectAdapterWithEndpoints(AdapterName, endpoint);

            // Get endpoint information as string (ice_getEndpoints() can be used as well)
            EndpointInfo = IceAdapter->createProxy(this->IceCommunicator->stringToIdentity(CommunicatorID))->ice_toString();

            // Create a servant
            Servant = CreateServant();

            // Inform the object adapter of the presence of a new servant
            IceAdapter->add(Servant, this->IceCommunicator->stringToIdentity(CommunicatorID));

            // Activate the adapter. The adapter is initially created in a
            // holding state. The server starts to process incoming requests
            // from clients as soon as the adapter is activated.
            IceAdapter->activate();

            this->InitSuccessFlag = true;

            ChangeProxyState(BaseType::PROXY_READY);

            this->IceLogger->trace("mtsProxyBaseServer", "Server proxy initialization success.");
        } catch (const Ice::Exception& e) {
            if (this->IceLogger) {
                this->IceLogger->error("mtsProxyBaseServer: Server proxy initialization error");
                this->IceLogger->trace("mtsProxyBaseServer", e.what());
            } else {
                CMN_LOG_RUN_ERROR << "mtsProxyBaseServer: Server proxy initialization error." << std::endl;
                CMN_LOG_RUN_ERROR << "mtsProxyBaseServer: " << e.what() << std::endl;
            }
        } catch (const char * msg) {
            if (this->IceLogger) {
                this->IceLogger->error("mtsProxyBaseServer: Server proxy initialization error");
                this->IceLogger->trace("mtsProxyBaseServer", msg);
            } else {
                CMN_LOG_RUN_ERROR << "mtsProxyBaseServer: Server proxy initialization error." << std::endl;
                CMN_LOG_RUN_ERROR << "mtsProxyBaseServer: " << msg << std::endl;
            }
        }

        if (!this->InitSuccessFlag) {
            try {
                this->IceCommunicator->destroy();
            } catch (const Ice::Exception & e) {
                if (this->IceLogger) {
                    this->IceLogger->error("mtsProxyBaseServer: Server proxy clean-up error");
                    this->IceLogger->trace("mtsProxyBaseServer", e.what());
                } else {
                    CMN_LOG_RUN_ERROR << "mtsProxyBaseServer: Server proxy clean-up error." << std::endl;
                    CMN_LOG_RUN_ERROR << e.what() << std::endl;
                }
            }
        }
    }

    /*! Shutdown the current session for graceful termination */
    void ShutdownSession(const Ice::Current & current) {
        current.adapter->getCommunicator()->shutdown();
        BaseType::ShutdownSession();
    }

    //-------------------------------------------------------------------------
    //  Connection Management and Client Proxy Management
    //-------------------------------------------------------------------------
    /*! Client information */
    typedef struct {
        std::string ClientName;
        ClientIDType ClientID;
        ConnectionIDType ConnectionID;
        ClientProxyType ClientProxy;
    } ClientInformation;

    /*! Lookup table to fetch client information with ClientID */
    typedef std::map<ClientIDType, ClientInformation> ClientIDMapType;
    ClientIDMapType ClientIDMap;

    /*! Lookup table to fetch client information with ConnectionID */
    typedef std::map<ConnectionIDType, ClientInformation> ConnectionIDMapType;
    ConnectionIDMapType ConnectionIDMap;

    /*! Mutex */
    osaMutex ClientIDMapChange;
    osaMutex ConnectionIDMapChange;

    /*! When a client proxy is connected to this server proxy, add it to client
        proxy map with a key of connection id */
    bool AddProxyClient(const std::string & clientName, const ClientIDType & clientID,
        const ConnectionIDType & connectionID, ClientProxyType & clientProxy)
    {
        // Check the uniqueness of clientID
        if (FindClientByClientID(clientID)) {
            std::stringstream ss;
            ss << "AddProxyClient: duplicate client id: " << clientID;
            std::string s = ss.str();
            this->IceLogger->error(s);
            return false;
        }

        // Check the uniqueness of connectionID
        if (FindClientByConnectionID(connectionID)) {
            std::stringstream ss;
            ss << "AddProxyClient: duplicate connection id: " << connectionID;
            std::string s = ss.str();
            this->IceLogger->error(s);
            return false;
        }

        ClientInformation client;
        client.ClientName = clientName;
        client.ClientID = clientID;
        client.ConnectionID = connectionID;
        client.ClientProxy = clientProxy;

        ClientIDMapChange.Lock();
        ClientIDMap.insert(std::make_pair(clientID, client));
        ClientIDMapChange.Unlock();

        ConnectionIDMapChange.Lock();
        ConnectionIDMap.insert(std::make_pair(connectionID, client));
        ConnectionIDMapChange.Unlock();

        return (FindClientByClientID(clientID) && FindClientByConnectionID(connectionID));
    }

    /*! Return ClientIDType */
    ClientIDType GetClientID(const ConnectionIDType & connectionID) {
        typename ConnectionIDMapType::iterator it = ConnectionIDMap.find(connectionID);
        if (it == ConnectionIDMap.end()) {
            return 0;
        }
        return it->second.ClientID;
    }

    /*! Get an ICE proxy object using connection id to send a message to a client */
    ClientProxyType * GetClientByConnectionID(const ConnectionIDType & connectionID) {
        typename ConnectionIDMapType::iterator it = ConnectionIDMap.find(connectionID);
        if (it == ConnectionIDMap.end()) {
            return NULL;
        }
        return &(it->second.ClientProxy);
    }

    /*! Get an ICE proxy object using client id to send a message to a client */
    ClientProxyType * GetClientByClientID(const ClientIDType & clientID) {
        typename ClientIDMapType::iterator it = ClientIDMap.find(clientID);
        if (it == ClientIDMap.end()) {
            return NULL;
        }
        return &(it->second.ClientProxy);
    }

    /*! Check if there is an ICE proxy object using connection id */
    bool FindClientByConnectionID(const ConnectionIDType & connectionID) const {
        return (ConnectionIDMap.find(connectionID) != ConnectionIDMap.end());
    }

    /*! Check if there is an ICE proxy object using client id */
    bool FindClientByClientID(const ClientIDType & clientID) const {
        return (ClientIDMap.find(clientID) != ClientIDMap.end());
    }

    /*! Remove an ICE proxy object using connection id */
    bool RemoveClientByConnectionID(const ConnectionIDType & connectionID) {
        typename ConnectionIDMapType::iterator it1 = ConnectionIDMap.find(connectionID);
        if (it1 == ConnectionIDMap.end()) {
            return false;
        }
        typename ClientIDMapType::iterator it2 = ClientIDMap.find(it1->second.ClientID);
        if (it2 == ClientIDMap.end()) {
            return false;
        }

        ConnectionIDMapChange.Lock();
        ConnectionIDMap.erase(it1);
        ConnectionIDMapChange.Unlock();

        ClientIDMapChange.Lock();
        ClientIDMap.erase(it2);
        ClientIDMapChange.Unlock();

        return true;
    }

    /*! Remove an ICE proxy object using client id */
    bool RemoveClientByClientID(const ClientIDType & clientID) {
        typename ClientIDMapType::iterator it1 = ClientIDMap.find(clientID);
        if (it1 == ClientIDMap.end()) {
            return false;
        }
        typename ConnectionIDMapType::iterator it2 = ConnectionIDMap.find(it1->second.ConnectionID);
        if (it2 == ConnectionIDMap.end()) {
            return false;
        }

        ClientIDMapChange.Lock();
        ClientIDMap.erase(it1);
        ClientIDMapChange.Unlock();

        ConnectionIDMapChange.Lock();
        ConnectionIDMap.erase(it2);
        ConnectionIDMapChange.Unlock();

        return true;
    }

    /*! Close a connection with a specific client */
    void CloseClient(const ConnectionIDType & connectionID) {
        ClientProxyType * clientProxy = GetClientByConnectionID(connectionID);
        if (!clientProxy) {
            std::stringstream ss;
            ss << "CloseClient: cannot find client with connection id: " << connectionID;
            std::string s = ss.str();
            this->IceLogger->warning(s);
            return;
        }

        // Close a connection explicitly (graceful closure)
        Ice::ConnectionPtr conn = ClientIDMap.begin()->second.ClientProxy->ice_getConnection();
        conn->close(false);
    }

    /*! Monitor active connection by heart beat. If a client proxy disconnects or is
        disconnected, the close event is detected here. */
    virtual void Monitor(void) {
        typename ConnectionIDMapType::iterator it = ConnectionIDMap.begin();
        while (it != ConnectionIDMap.end()) {
            try {
                it->second.ClientProxy->ice_ping();
                ++it;
            } catch (const Ice::Exception & ex) {
                std::stringstream ss;
                ss << "ProxyBaseServer Monitor: remove disconnected client: client id=\"" << it->second.ClientID << "\", "
                   << "connection id=\"" << it->second.ConnectionID << "\"\n" << ex;
                std::string s = ss.str();
                this->IceLogger->warning(s);

                if (!this->OnClientDisconnect(it->second.ClientID)) {
                    std::stringstream ss;
                    ss << "ProxyBaseServer Monitor: failed to remove disconnected client: client id=\"" << it->second.ClientID << "\", "
                       << "connection id=\"" << it->second.ConnectionID << "\"\n" << ex;
                    std::string s = ss.str();
                    this->IceLogger->error(s);
                    break; // prevents infinite loop
                }

                it = ConnectionIDMap.begin();
            }
        }
    }

    /*! Close all the connections with all the clients */
    // TODO

public:
    /*! Constructor. If useDynamicPortAllocation is false, a port number is
        fetched from config.server. Only mtsManagerProxyServer sets it as false.
        If it is true by default, a port number is randomly chosen by OS. This
        guarantees there is no overlap among multiple server instances' ports. */
    mtsProxyBaseServer(const std::string & propertyFileName,
                       const std::string & adapterName,
                       const std::string & communicatorID,
                       const bool useDynamicPortAllocation = true)
                       : BaseType(propertyFileName, BaseType::PROXY_SERVER),
                         AdapterName(adapterName),
                         CommunicatorID(communicatorID),
                         DynamicPortAllocation(useDynamicPortAllocation)
    {}

    /*! Destructor (do nothing) */
    virtual ~mtsProxyBaseServer() {}

    /*! Getter for this server's endpoint information */
    inline std::string GetEndpointInfo() const { return EndpointInfo; }
};

#endif // _mtsProxyBaseServer_h

