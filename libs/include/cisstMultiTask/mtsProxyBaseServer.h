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

    // smmy: check if I really need this
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
    osaMutex ClientIDMapChange;
    osaMutex ConnectionIDMapChange;

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

    // smmy: todo: Implement this
    /*! Close all the connected clients */
    //void CloseAllClients(void);

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
        IceAdapter = IceCommunicator->createObjectAdapterWithEndpoints(AdapterName, endpoint);

        // Get endpoint information as string (ice_getEndpoints() can be used as well)
        EndpointInfo = IceAdapter->createProxy(IceCommunicator->stringToIdentity(CommunicatorID))->ice_toString();

        // Create a servant
        Servant = CreateServant();

        // Inform the object adapter of the presence of a new servant
        IceAdapter->add(Servant, IceCommunicator->stringToIdentity(CommunicatorID));

        // Activate the adapter. The adapter is initially created in a
        // holding state. The server starts to process incoming requests
        // from clients as soon as the adapter is activated.
        IceAdapter->activate();

        InitSuccessFlag = true;

        ChangeProxyState(BaseType::PROXY_STATE_READY);

        IceLogger->trace("mtsProxyBaseServer", "ICE init - Server proxy initialization success.");
    } catch (const Ice::Exception& e) {
        if (IceLogger) {
            IceLogger->error("mtsProxyBaseServer: ICE init - Server proxy initialization error");
            IceLogger->trace("mtsProxyBaseServer", e.what());
        } else {
            CMN_LOG_RUN_ERROR << "mtsProxyBaseServer: ICE init - Server proxy initialization error." << std::endl;
            CMN_LOG_RUN_ERROR << "mtsProxyBaseServer: " << e.what() << std::endl;
        }
    } catch (...) {
        if (IceLogger) {
            IceLogger->error("mtsProxyBaseServer: ICE init - exception");
        } else {
            CMN_LOG_RUN_ERROR << "mtsProxyBaseServer: ICE init - exception" << std::endl;
        }
    }

    if (!InitSuccessFlag) {
        try {
            IceCommunicator->destroy();
        } catch (const Ice::Exception & e) {
            if (IceLogger) {
                IceLogger->error("mtsProxyBaseServer: ICE init - Server proxy clean-up error");
                IceLogger->trace("mtsProxyBaseServer", e.what());
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
    ChangeProxyState(BaseType::PROXY_STATE_FINISHING);

    InitSuccessFlag = false;

    // smmy: two different clean-up process
    // 1) when an individual client is disconnected - only Ice proxy associated
    //    with the client should be cleaned up
    // 2) when server itself terminates - all clients via 1) + Ice server proxy
    //    needs to be cleaned up

    // TODO: smmy: remove client selectively!!!
    //RemoveServant();
}

template<class _proxyOwner, class _clientProxyType, class _clientIDType>
void mtsProxyBaseServerType::StopProxy(void) 
{
    IceCleanup();

    if (IceCommunicator) {
        try {
            IceCommunicator->destroy();
            IceCommunicator = 0;
            IceLogger->trace("mtsProxyBaseServer", "Proxy server clean-up success");
        } catch (const Ice::Exception & e) {
            IceLogger->error("mtsProxyBaseServer: Proxy server clean-up failure");
            IceLogger->trace("mtsProxyBaseServer", e.what());
        } catch (...) {
            IceLogger->error("mtsProxyBaseServer: Proxy server clean-up failure");
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

    ClientIDMapChange.Lock();
    ClientIDMap.insert(std::make_pair(clientID, client));
    ClientIDMapChange.Unlock();

    ConnectionIDMapChange.Lock();
    IceConnectionIDMap.insert(std::make_pair(iceConnectionID, client));
    ConnectionIDMapChange.Unlock();

    return ((FindClientByClientID(clientID) && FindClientByConnectionID(iceConnectionID)));
}

template<class _proxyOwner, class _clientProxyType, class _clientIDType>
bool mtsProxyBaseServerType::RemoveClientByConnectionID(const IceConnectionIDType & iceConnectionID)
{
    typename IceConnectionIDMapType::iterator it1 = IceConnectionIDMap.find(iceConnectionID);
    if (it1 == IceConnectionIDMap.end()) {
        return false;
    }
    typename ClientIDMapType::iterator it2 = ClientIDMap.find(it1->second.ClientID);
    if (it2 == ClientIDMap.end()) {
        return false;
    }

    ConnectionIDMapChange.Lock();
    IceConnectionIDMap.erase(it1);
    ConnectionIDMapChange.Unlock();

    ClientIDMapChange.Lock();
    ClientIDMap.erase(it2);
    ClientIDMapChange.Unlock();

    return true;
}

template<class _proxyOwner, class _clientProxyType, class _clientIDType>
bool mtsProxyBaseServerType::RemoveClientByClientID(const ClientIDType & clientID) 
{
    typename ClientIDMapType::iterator it1 = ClientIDMap.find(clientID);
    if (it1 == ClientIDMap.end()) {
        return false;
    }
    typename IceConnectionIDMapType::iterator it2 = IceConnectionIDMap.find(it1->second.ConnectionID);
    if (it2 == IceConnectionIDMap.end()) {
        return false;
    }

    ClientIDMapChange.Lock();
    ClientIDMap.erase(it1);
    ClientIDMapChange.Unlock();

    ConnectionIDMapChange.Lock();
    IceConnectionIDMap.erase(it2);
    ConnectionIDMapChange.Unlock();

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
        IceLogger->warning(s);
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
    if (!IsActiveProxy()) return;

    typename IceConnectionIDMapType::iterator it = IceConnectionIDMap.begin();
    while (it != IceConnectionIDMap.end()) {
        try {
            it->second.ClientProxy->ice_ping();
            ++it;
        } catch (const Ice::Exception & ex) {
            std::stringstream ss;
            ss << "Proxy \"" << ProxyName << "\" detected CLIENT DISCONNECTION: client id=\"" << it->second.ClientID << "\", "
                << "connection id=\"" << it->second.ConnectionID << "\"" << std::endl << ex;
            std::string s = ss.str();
            IceLogger->warning(s);

            OnClientDisconnect(it->second.ClientID);

            // Reset iterator (OnClientDisconnect() may invalidated it)
            it = IceConnectionIDMap.begin();
        }
    }
}

#endif // _mtsProxyBaseServer_h
