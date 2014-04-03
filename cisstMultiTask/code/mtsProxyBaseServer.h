/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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
#include "mtsProxyBaseCommon.h"

#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  This class is derived from mtsProxyBaseCommon and implements the basic
  structure and functions for ICE proxy server.  The actual processing routine
  should be implemented by derived classes.

  Besides proxy server setup, server proxy needs to handle multiple clients and
  connections.  To support a general type of client proxy, this class is
  templated.
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
    typedef std::string IceConnectionIDType;

    /*! Typedef for client id */
    typedef _clientIDType ClientIDType;

protected:
    /*! ICE objects */
    Ice::ObjectAdapterPtr IceAdapter;
    Ice::ObjectPtr Servant;

    /*! Endpoint information that clients uses to connect to this server */
    const std::string AdapterName;
    const std::string CommunicatorID;
    std::string EndpointInfo;

    /*! Set as true when using dynamic port allocation (true by default) */
    bool DynamicPortAllocation;

    /*! Start proxy server. Entry point to initialize Ice proxy server objects.
        Gets called by user (application) */
    virtual bool StartProxy(_proxyOwner * proxyOwner) = 0;

    /*! Initialize Ice proxy server. Called by StartProxy(). */
    void IceInitialize(void);

    /*! Create ICE servant object */
    virtual Ice::ObjectPtr CreateServant(void) = 0;

    /*! Called when client disconnection is detected */
    virtual bool OnClientDisconnect(const ClientIDType clientID) = 0;

    /*! Remove ICE servant object */
    virtual void RemoveServant(void) = 0;

    /*! Clean up ICE related resources */
    virtual void IceCleanup(void);

    /*! Stop and clean up proxy server.  This cleans up all client proxies
        connected */
    virtual void StopProxy(void);

    /*! Shutdown the current session for graceful termination */
    //void ShutdownSession(const Ice::Current & current);

    //-------------------------------------------------------------------------
    //  Connection and Client Proxy Management
    //-------------------------------------------------------------------------
    /*! Client information */
    typedef struct {
        std::string      ClientName;
        ClientIDType     ClientID;
        IceConnectionIDType ConnectionID;
        ClientProxyType  ClientProxy;
    } ClientInformation;

    /*! Lookup table to get client information with ClientID */
    typedef std::map<ClientIDType, ClientInformation> ClientIDMapType;
    ClientIDMapType ClientIDMap;

    /*! Lookup table to get client information with ConnectionID */
    typedef std::map<IceConnectionIDType, ClientInformation> IceConnectionIDMapType;
    IceConnectionIDMapType IceConnectionIDMap;

    /*! Mutex */
    osaMutex ClientMapChange;

    /*! Add proxy client connecting to this proxy server (key: connection id) */
    bool AddProxyClient(const std::string & clientName, const ClientIDType & clientID,
        const IceConnectionIDType & iceConnectionID, ClientProxyType & clientProxy);

    /*! Remove ICE proxy object using connection id */
    bool RemoveClientByConnectionID(const IceConnectionIDType & iceConnectionID);

    /*! Remove ICE proxy object using client id */
    bool RemoveClientByClientID(const ClientIDType & clientID);

    /*! \brief Close ICE connection with connected client using client id
        \param clientID client id (set as IceUtil::generateUUID())
        \param force false for graceful closure (default), true for forceful closure */
    bool CloseClient(const ClientIDType & clientID, const bool force = false);

    /*! Monitor active connection by heart beat. If a client proxy disconnects or is
        disconnected, the close event is detected here. */
    virtual void Monitor(void);

    //-------------------------------------------------------------------------
    //  Getters
    //-------------------------------------------------------------------------
    /*! Return ClientIDType */
    ClientIDType GetClientID(const IceConnectionIDType & iceConnectionID) const {
        typename IceConnectionIDMapType::const_iterator it = IceConnectionIDMap.find(iceConnectionID);
        if (it == IceConnectionIDMap.end()) {
            return 0;
        }
        return it->second.ClientID;
    }

    /*! Get ICE proxy object using connection id */
    ClientProxyType * GetClientByConnectionID(const IceConnectionIDType & iceConnectionID) const {
        typename IceConnectionIDMapType::const_iterator it = IceConnectionIDMap.find(iceConnectionID);
        if (it == IceConnectionIDMap.end()) {
            return NULL;
        }
        return &(it->second.ClientProxy);
    }

    /*! Get ICE proxy object using client id */
    ClientProxyType * GetClientByClientID(const ClientIDType & clientID) {
        typename ClientIDMapType::iterator it = ClientIDMap.find(clientID);
        if (it == ClientIDMap.end()) {
            return NULL;
        }
        return &(it->second.ClientProxy);
    }

    /*! Look for ICE proxy client using connection id */
    inline bool FindClientByConnectionID(const IceConnectionIDType & iceConnectionID) const {
        return (IceConnectionIDMap.find(iceConnectionID) != IceConnectionIDMap.end());
    }

    /*! Look for ICE proxy object using client id */
    inline bool FindClientByClientID(const ClientIDType & clientID) const {
        return (ClientIDMap.find(clientID) != ClientIDMap.end());
    }


public:
    /*! Constructor. If useDynamicPortAllocation is false, a port number is
        fetched from config.server. Only mtsManagerProxyServer sets it as false.
        It is true by default and a port number is randomly chosen by OS. This
        guarantees no overlap of port number across multiple instances of Ice
        proxy server. */
    mtsProxyBaseServer(const std::string & propertyFileName,
                       const std::string & adapterName,
                       const std::string & communicatorID,
                       const bool useDynamicPortAllocation = true)
                       : BaseType(propertyFileName, BaseType::PROXY_TYPE_SERVER),
                         AdapterName(adapterName),
                         CommunicatorID(communicatorID),
                         DynamicPortAllocation(useDynamicPortAllocation)
    {}

    /*! Destructor */
    virtual ~mtsProxyBaseServer() {}

    /*! Getter for this server's endpoint information */
    inline std::string GetEndpointInfo() const { return EndpointInfo; }
};

#define mtsProxyBaseServerType mtsProxyBaseServer<_proxyOwner, _clientProxyType, _clientIDType>

template<class _proxyOwner, class _clientProxyType, class _clientIDType>
void mtsProxyBaseServerType::IceInitialize(void)
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
        IceAdapter = this->IceCommunicator->createObjectAdapterWithEndpoints(AdapterName, endpoint);

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
        this->ChangeProxyState(BaseType::PROXY_STATE_READY);

        this->IceLogger->trace("mtsProxyBaseServer", "ICE init - Server proxy initialization success.");
    } catch (const Ice::Exception& e) {
        if (this->IceLogger) {
            this->IceLogger->error("mtsProxyBaseServer: ICE init - Server proxy initialization error");
            this->IceLogger->trace("mtsProxyBaseServer", e.what());
        } else {
            CMN_LOG_RUN_ERROR << "mtsProxyBaseServer: ICE init - Server proxy initialization error." << std::endl;
            CMN_LOG_RUN_ERROR << "mtsProxyBaseServer: " << e.what() << std::endl;
        }
    } catch (...) {
        if (this->IceLogger) {
            this->IceLogger->error("mtsProxyBaseServer: ICE init - exception");
        } else {
            CMN_LOG_RUN_ERROR << "mtsProxyBaseServer: ICE init - exception" << std::endl;
        }
    }

    if (!this->InitSuccessFlag) {
        try {
            this->IceCommunicator->destroy();
        } catch (const Ice::Exception & e) {
            if (this->IceLogger) {
                this->IceLogger->error("mtsProxyBaseServer: ICE init - Server proxy clean-up error");
                this->IceLogger->trace("mtsProxyBaseServer", e.what());
            } else {
                CMN_LOG_RUN_ERROR << "mtsProxyBaseServer: ICE init - Server proxy clean-up error." << std::endl;
                CMN_LOG_RUN_ERROR << e.what() << std::endl;
            }
        }
    }
}

template<class _proxyOwner, class _clientProxyType, class _clientIDType>
void mtsProxyBaseServerType::IceCleanup(void)
{
    this->ChangeProxyState(BaseType::PROXY_STATE_FINISHING);
    this->InitSuccessFlag = false;
}

template<class _proxyOwner, class _clientProxyType, class _clientIDType>
void mtsProxyBaseServerType::StopProxy(void)
{
    IceCleanup();

    if (this->IceCommunicator) {
        try {
            this->IceCommunicator->destroy();
            this->IceCommunicator = 0;
            this->IceLogger->trace("mtsProxyBaseServer", "Proxy server clean-up success");
        } catch (const Ice::Exception & e) {
            this->IceLogger->error("mtsProxyBaseServer: Proxy server clean-up failure");
            this->IceLogger->trace("mtsProxyBaseServer", e.what());
        } catch (...) {
            this->IceLogger->error("mtsProxyBaseServer: Proxy server clean-up failure");
        }
    }

    BaseType::IceCleanup();
}

//template<class _proxyOwner, class _clientProxyType, class _clientIDType>
//void mtsProxyBaseServerType::ShutdownSession(
//    const Ice::Current & current)
//{
//    current.adapter->getCommunicator()->shutdown();
//    BaseType::ShutdownSession();
//}

template<class _proxyOwner, class _clientProxyType, class _clientIDType>
bool mtsProxyBaseServerType::AddProxyClient(const std::string & clientName, const ClientIDType & clientID,
                                            const IceConnectionIDType & iceConnectionID, ClientProxyType & clientProxy)
{
    // Check the uniqueness of clientID
    if (FindClientByClientID(clientID)) {
        std::stringstream ss;
        ss << "AddProxyClient: duplicate client id: " << clientID;
        std::string s = ss.str();
        this->IceLogger->error(s);
        return false;
    }

    // Check the uniqueness of iceConnectionID
    if (FindClientByConnectionID(iceConnectionID)) {
        std::stringstream ss;
        ss << "AddProxyClient: duplicate connection id: " << iceConnectionID;
        std::string s = ss.str();
        this->IceLogger->error(s);
        return false;
    }

    ClientInformation client;
    client.ClientName = clientName;
    client.ClientID = clientID;
    client.ConnectionID = iceConnectionID;
    client.ClientProxy = clientProxy;

    ClientMapChange.Lock();
        ClientIDMap.insert(std::make_pair(clientID, client));
        IceConnectionIDMap.insert(std::make_pair(iceConnectionID, client));
    ClientMapChange.Unlock();

    return ((FindClientByClientID(clientID) && FindClientByConnectionID(iceConnectionID)));
}

template<class _proxyOwner, class _clientProxyType, class _clientIDType>
bool mtsProxyBaseServerType::RemoveClientByConnectionID(const IceConnectionIDType & iceConnectionID)
{
    ClientMapChange.Lock();

    typename IceConnectionIDMapType::iterator it1 = IceConnectionIDMap.find(iceConnectionID);
    if (it1 == IceConnectionIDMap.end()) {
        ClientMapChange.Unlock();
        return false;
    }
    typename ClientIDMapType::iterator it2 = ClientIDMap.find(it1->second.ClientID);
    if (it2 == ClientIDMap.end()) {
        ClientMapChange.Unlock();
        return false;
    }

    ClientIDMap.erase(it2);
    IceConnectionIDMap.erase(it1);

    ClientMapChange.Unlock();

    return true;
}

template<class _proxyOwner, class _clientProxyType, class _clientIDType>
bool mtsProxyBaseServerType::RemoveClientByClientID(const ClientIDType & clientID)
{
    ClientMapChange.Lock();

    typename ClientIDMapType::iterator it1 = ClientIDMap.find(clientID);
    if (it1 == ClientIDMap.end()) {
        ClientMapChange.Unlock();
        return false;
    }
    typename IceConnectionIDMapType::iterator it2 = IceConnectionIDMap.find(it1->second.ConnectionID);
    if (it2 == IceConnectionIDMap.end()) {
        ClientMapChange.Unlock();
        return false;
    }

    ClientIDMap.erase(it1);
    IceConnectionIDMap.erase(it2);

    ClientMapChange.Unlock();

    return true;
}

template<class _proxyOwner, class _clientProxyType, class _clientIDType>
bool mtsProxyBaseServerType::CloseClient(const ClientIDType & clientID, const bool force)
{
    ClientProxyType * clientProxy = GetClientByClientID(clientID);
    if (!clientProxy) {
        std::stringstream ss;
        ss << "CloseClient: cannot find client with client id: " << clientID;
        std::string s = ss.str();
        this->IceLogger->warning(s);
        return false;
    }

    // Close Ice connection
    Ice::ConnectionPtr conn = ClientIDMap.begin()->second.ClientProxy->ice_getConnection();
    conn->close(force);

    return true;
}

template<class _proxyOwner, class _clientProxyType, class _clientIDType>
void mtsProxyBaseServerType::Monitor(void)
{
    if (mtsProxyConfig::DisableConnectionMonitoring) return;
    if (IceConnectionIDMap.size() == 0) return;
    if (!this->IsActiveProxy()) return;

    ClientMapChange.Lock();

    typename IceConnectionIDMapType::iterator it = IceConnectionIDMap.begin();
    while (it != IceConnectionIDMap.end()) {
        try {
            it->second.ClientProxy->ice_ping();
            ++it;
        } catch (const Ice::Exception & ex) {
            std::stringstream ss;
            ss << "Proxy \"" << this->ProxyName << "\" detected CLIENT DISCONNECTION: client id [ " << it->second.ClientID << " ], "
                << "Ice connection id=\"" << it->second.ConnectionID << "\"" << std::endl << ex;
            std::string s = ss.str();
            this->IceLogger->warning(s);

            ClientIDType clientID = it->second.ClientID;

            ClientMapChange.Unlock();
                OnClientDisconnect(clientID);
            ClientMapChange.Lock();

            // Reset iterator (iterator may get invalidated by OnClientDisconnect())
            it = IceConnectionIDMap.begin();
        }
    }

    ClientMapChange.Unlock();
}

#endif // _mtsProxyBaseServer_h
