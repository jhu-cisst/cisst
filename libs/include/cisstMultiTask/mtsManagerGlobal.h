/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung
  Created on: 2009-11-12

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

/*!
  \file
  \brief Definition of the global manager
  \ingroup cisstMultiTask

  Please see mtsManagerGlobalInterface.h for detailed comments on methods
  defined as pure virtual in mtsManagerGlobalInterface.
*/

#ifndef _mtsManagerGlobal_h
#define _mtsManagerGlobal_h

#include <cisstCommon/cmnClassRegister.h>
#include <cisstCommon/cmnNamedMap.h>

#include <cisstOSAbstraction/osaMutex.h>
#include <cisstOSAbstraction/osaGetTime.h>

#include <cisstMultiTask/mtsProxyConfig.h>
#include <cisstMultiTask/mtsManagerLocalInterface.h>
#include <cisstMultiTask/mtsManagerGlobalInterface.h>
#include <cisstMultiTask/mtsParameterTypes.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>

#include <cisstMultiTask/mtsExport.h>

// Forward declaration
class mtsManagerComponentServer;

class CISST_EXPORT mtsManagerGlobal : public mtsManagerGlobalInterface
{
    friend class mtsManagerGlobalTest;
    friend class mtsManagerLocalTest;

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

protected:
    //-------------------------------------------------------------------------
    //  Data Structure of Process Map
    //-------------------------------------------------------------------------
    /*
        P1 - C1 - p1 - r1   Process Map: (P, ComponentMap)
        |    |    |    |    Component Map: (C, InterfaceMap)
        |    |    |    r2   Interface Map: { (PI, ConnectionMap) where PI is Provided Interface
        |    |    |                          (RI, ConnectionMap) where RI is Required Interface }
        |    |    p2 - r3   Connection Map: (name of connected interface, ConnectedInterfaceInfo)
        |    |    |
        |    |    r1 - p1
        |    |
        |    C2 - r1 - p2
        |
        P2 - C1
             |
             C2
    */

    /*! Data structure to keep information about a connected interface */
    class ConnectedInterfaceInfo {
    protected:
        // Names (IDs)
        const std::string ProcessName;
        const std::string ComponentName;
        const std::string InterfaceName;
        // True if this interface is remote
        const bool RemoteConnection;
#if CISST_MTS_HAS_ICE
        // Server proxy access information (sent to client proxy as requested)
        std::string EndpointInfo;
#endif

        ConnectedInterfaceInfo() : ProcessName(""), ComponentName(""), InterfaceName(""), RemoteConnection(false)
#if CISST_MTS_HAS_ICE
            , EndpointInfo("")
#endif
        {}

    public:
        ConnectedInterfaceInfo(const std::string & processName, const std::string & componentName,
                               const std::string & interfaceName, const bool isRemoteConnection)
            : ProcessName(processName), ComponentName(componentName), InterfaceName(interfaceName), RemoteConnection(isRemoteConnection)
        {}

        // Getters
        const std::string GetProcessName() const   { return ProcessName; }
        const std::string GetComponentName() const { return ComponentName; }
        const std::string GetInterfaceName() const { return InterfaceName; }
        bool IsRemoteConnection() const            { return RemoteConnection; }
#if CISST_MTS_HAS_ICE
        std::string GetEndpointInfo() const        { return EndpointInfo; }

        // Setters
        void SetProxyAccessInfo(const std::string & endpointInfo) {
            EndpointInfo = endpointInfo;
        }
#endif
    };

    /*! Data structure to keep information about a connection */
    class ConnectionElement {
        // For unit-test
        friend class mtsManagerGlobalTest;

    protected:
        // This connection ID
        const unsigned int ConnectionID;
        // Connection status. False when waiting for a successful establishment,
        // True if successfully established.
        bool Connected;

    public:
        // Name of connect request process
        const std::string RequestProcessName;
        // Set of strings
        const std::string ClientProcessName;
        const std::string ClientComponentName;
        const std::string ClientInterfaceRequiredName;
        const std::string ServerProcessName;
        const std::string ServerComponentName;
        const std::string ServerInterfaceProvidedName;
#if CISST_MTS_HAS_ICE
        // Time when this object becomes timed out
        double TimeoutTime;
#endif

        ConnectionElement(const std::string & requestProcessName, const unsigned int connectionID,
            const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
            const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName)
            : ConnectionID(connectionID), Connected(false), RequestProcessName(requestProcessName),
              ClientProcessName(clientProcessName), ClientComponentName(clientComponentName), ClientInterfaceRequiredName(clientInterfaceRequiredName),
              ServerProcessName(serverProcessName), ServerComponentName(serverComponentName), ServerInterfaceProvidedName(serverInterfaceProvidedName)
        {
#if CISST_MTS_HAS_ICE
            // When current time plus Timeout expires, this connection elements
            // gets invalidated by the GCM.
            TimeoutTime = osaGetTime() + mtsProxyConfig::ConnectConfirmTimeOut;
#endif
        }

        /*! Get connection id */
        unsigned int GetConnectionID(void) const {
            return ConnectionID;
        }

        mtsDescriptionConnection GetDescriptionConnection(void) const {
            mtsDescriptionConnection conn;
            conn.Client.ProcessName = ClientProcessName;
            conn.Client.ComponentName = ClientComponentName;
            conn.Client.InterfaceName = ClientInterfaceRequiredName;
            conn.Server.ProcessName = ServerProcessName;
            conn.Server.ComponentName = ServerComponentName;
            conn.Server.InterfaceName = ServerInterfaceProvidedName;
            conn.ConnectionID = ConnectionID;
            return conn;
        }

        inline bool IsConnected(void) const {
            return Connected;
        }

        /*! Set this connection as established */
        inline void SetConnected(void) {
            Connected = true;
        }

        /*! Return true if this connection is timed out */
#if CISST_MTS_HAS_ICE
        inline bool CheckTimeout(void) const {
            return (TimeoutTime - osaGetTime() <= 0);
        }
#endif
    };

    /*! Connection map: (connected interface name, connected interface information)
        Map name: a name of component that has these interfaces. */
    typedef cmnNamedMap<mtsManagerGlobal::ConnectedInterfaceInfo> ConnectionMapType;

    /*! Interface map: a map of registered interfaces in a component
        key=(interface name), value=(connection map)
        value can be null if an interface does not have any connection. */
    typedef cmnNamedMap<ConnectionMapType> ConnectedInterfaceMapType;

    /*! Interface type flag map: a map of registered interfaces in a component
        key=(interface name), value=(bool)
        value is false if an interface is an original interface
                 true  if an interface is a proxy interface
        This information is used to determine if an interface should be removed
        (cleaned up) when a connection is disconnected. See
        mtsManagerGlobal::Disconnect() for more details. */
    typedef std::map<std::string, bool> InterfaceTypeMapType;

    /*! Interface map has two kinds of containers:
        - containers for connection map
        - containers for interface type flag map */
    typedef struct {
        ConnectedInterfaceMapType InterfaceProvidedOrOutputMap;
        ConnectedInterfaceMapType InterfaceRequiredOrInputMap;
        InterfaceTypeMapType InterfaceProvidedOrOutputTypeMap;
        InterfaceTypeMapType InterfaceRequiredOrInputTypeMap;
    } InterfaceMapType;

    /*! Component map: a map of registered components in a process
        key=(component name), value=(interface map)
        value can be null if a component does not have any interface. */
    typedef cmnNamedMap<InterfaceMapType> ComponentMapType;

    /*! Process map: a map of registered processes (i.e., local component managers)
        key=(process name), value=(component map)
        value can be null if a process does not have any component. */
    typedef cmnNamedMap<ComponentMapType> ProcessMapType;
    ProcessMapType ProcessMap;

    /*! Connection element map: a map of strings that defines a connection
        key=(connection id), value=(an instance of ConnectionElement)
        When a local component manager requests estbalishing a connection, an
        element is created and added. If a connection is not established before
        timeout, the element is removed. When a local component manager notifies
        that a connection is successfully established, the element is marked
        as connected. */
    typedef std::map<unsigned int, mtsManagerGlobal::ConnectionElement*> ConnectionElementMapType;
    ConnectionElementMapType ConnectionElementMap;

    /*! Instance of connected local component manager. Note that the global
        component manager communicates with the only one instance of
        mtsManagerLocalInterface regardless of connection type (standalone
        or network mode) 
        
        MJ: (8/20/10) To support a local connection between the global component
        manager and a local component manager on the same process, this 
        assumption is slightly modified such that the GCM can have two different 
        type of connections -- local and remote connections.  When the GCM 
        executes commands, it checks if it has a local connection to the LCM on 
        the same process and, if yes, it sends the command to the local LCM.  
        If not, it delivers the command to a remote LCM as before. */
    mtsManagerLocal * LocalManager;
    mtsManagerLocalInterface * LocalManagerConnected;

    /*! Mutex for thread-safe connection processing (there could be possibly multiple
        threads that try to establish a connection */
    osaMutex ConnectionChange;

    /*! Counter to issue a new connection ID */
    unsigned int ConnectionID;

    /*! Typedef to get user id using connection id.  User id is set by provided
        interface's AllocatedResources()
        (see mtsManagerLocal::GetInterfaceProvidedDescription() for details). */
    //typedef std::map<unsigned int, int> UserIDMapType;
    //UserIDMapType UserIDMap;

#if CISST_MTS_HAS_ICE
    /*! Network proxy server */
    mtsManagerProxyServer * ProxyServer;
#endif

    mtsManagerComponentServer *ManagerComponentServer;

    //-------------------------------------------------------------------------
    //  Processing Methods
    //-------------------------------------------------------------------------
    /*! Clean up the internal variables */
    bool Cleanup(void);

    /*! Get a map containing connection information for a provided interface */
    ConnectionMapType * GetConnectionsOfInterfaceProvidedOrOutput(const std::string & serverProcessName, const std::string & serverComponentName,
                                                                  const std::string & providedInterfaceName, InterfaceMapType ** interfaceMap);
    ConnectionMapType * GetConnectionsOfInterfaceProvidedOrOutput(const std::string & serverProcessName, const std::string & serverComponentName,
                                                                  const std::string & providedInterfaceName) const;

    /*! Get a map containing connection information for a required interface */
    ConnectionMapType * GetConnectionsOfInterfaceRequiredOrInput(const std::string & clientProcessName, const std::string & clientComponentName,
                                                                 const std::string & requiredInterfaceName, InterfaceMapType ** interfaceMap);

    ConnectionMapType * GetConnectionsOfInterfaceRequiredOrInput(const std::string & clientProcessName, const std::string & clientComponentName,
                                                                 const std::string & requiredInterfaceName) const;

    /*! Add this interface to connectionMap as connected interface */
    bool AddConnectedInterface(ConnectionMapType * connectionMap,
                               const std::string & processName, const std::string & componentName,
                               const std::string & interfaceName, const bool isRemoteConnection = false);

    /*! Check if two interfaces are connected */
    bool IsAlreadyConnected(const std::string & clientProcessName, const std::string & clientComponentName,
                            const std::string & clientInterfaceRequiredName,
                            const std::string & serverProcessName, const std::string & serverComponentName,
                            const std::string & serverInterfaceProvidedName);

public:
    /*! Constructor and destructor */
    mtsManagerGlobal();
    ~mtsManagerGlobal();

    void SetMCS(mtsManagerComponentServer *mcs) { ManagerComponentServer = mcs; }

    //-------------------------------------------------------------------------
    //  Process Management
    //-------------------------------------------------------------------------
    bool AddProcess(const std::string & processName);

    bool AddProcessObject(mtsManagerLocalInterface * localManagerObject, const bool isManagerProxyServer = false);

    bool FindProcess(const std::string & processName) const;

    bool RemoveProcess(const std::string & processName);

    //-------------------------------------------------------------------------
    //  Component Management
    //-------------------------------------------------------------------------
    bool AddComponent(const std::string & processName, const std::string & componentName);

    bool FindComponent(const std::string & processName, const std::string & componentName) const;

    bool RemoveComponent(const std::string & processName, const std::string & componentName);

    //-------------------------------------------------------------------------
    //  Interface Management
    //-------------------------------------------------------------------------
    bool AddInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName,
                                      const std::string & interfaceName, const bool isProxyInterface = false);

    bool AddInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName,
                                     const std::string & interfaceName, const bool isProxyInterface = false);

    bool FindInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName,
                                       const std::string & interfaceName) const;

    bool FindInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName,
                                      const std::string & interfaceName) const;

    bool RemoveInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName,
                                         const std::string & interfaceName);

    bool RemoveInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName,
                                        const std::string & interfaceName);

    //-------------------------------------------------------------------------
    //  Connection Management
    //-------------------------------------------------------------------------
    int Connect(const std::string & requestProcessName,
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName);

    bool ConnectConfirm(unsigned int connectionSessionID);

    bool Disconnect(
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName);

#if CISST_MTS_HAS_ICE
    bool InitiateConnect(const unsigned int connectionID,
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName);

    bool ConnectServerSideInterfaceRequest(const unsigned int connectionID,
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName);
#endif

    void GetListOfConnections(std::vector<mtsDescriptionConnection> & list) const;

    //-------------------------------------------------------------------------
    //  Getters
    //-------------------------------------------------------------------------
    /*! Return the name of the global component manager (for mtsProxyBaseCommon.h) */
    inline static std::string GetName(void) {
        return "GlobalComponentManager";
    }

    /*! Return IP address of this machine. */
    std::vector<std::string> GetIPAddress(void) const;
    void GetIPAddress(std::vector<std::string> & ipAddresses) const;

    /*! Get names of all processes */
    void GetNamesOfProcesses(std::vector<std::string>& namesOfProcesses) const;

    /*! Get names of all components in a process */
    void GetNamesOfComponents(const std::string & processName,
                              std::vector<std::string>& namesOfComponents) const;

    /*! Get names of all provided interfaces in a component */
    void GetNamesOfInterfacesProvidedOrOutput(const std::string & processName,
                                              const std::string & componentName,
                                              std::vector<std::string> & namesOfInterfacesProvided) const;

    /*! Get names of all required interfaces in a component */
    void GetNamesOfInterfacesRequiredOrInput(const std::string & processName,
                                             const std::string & componentName,
                                             std::vector<std::string> & namesOfInterfacesRequired) const;

#if CISST_MTS_HAS_ICE
    /*! Get names of all commands in a provided interface */
    void GetNamesOfCommands(const std::string & processName,
                            const std::string & componentName,
                            const std::string & providedInterfaceName,
                            std::vector<std::string>& namesOfCommands) const;

    /*! Get names of all event generators in a provided interface */
    void GetNamesOfEventGenerators(const std::string & processName,
                                   const std::string & componentName,
                                   const std::string & providedInterfaceName,
                                   std::vector<std::string>& namesOfEventGenerators) const;

    /*! Get names of all functions in a required interface */
    void GetNamesOfFunctions(const std::string & processName,
                             const std::string & componentName,
                             const std::string & requiredInterfaceName,
                             std::vector<std::string>& namesOfFunctions) const;

    /*! Get names of all event handlers in a required interface */
    void GetNamesOfEventHandlers(const std::string & processName,
                                 const std::string & componentName,
                                 const std::string & requiredInterfaceName,
                                 std::vector<std::string>& namesOfEventHandlers) const;

    /*! Get description of a command in a provided interface */
    void GetDescriptionOfCommand(const std::string & processName,
                                 const std::string & componentName,
                                 const std::string & providedInterfaceName,
                                 const std::string & commandName,
                                 std::string & description) const;

    /*! Get description of an event generator in a provided interface */
    void GetDescriptionOfEventGenerator(const std::string & processName,
                                        const std::string & componentName,
                                        const std::string & providedInterfaceName,
                                        const std::string & eventGeneratorName,
                                        std::string & description) const;

    /*! Get description of a function in a required interface */
    void GetDescriptionOfFunction(const std::string & processName,
                                  const std::string & componentName,
                                  const std::string & requiredInterfaceName,
                                  const std::string & functionName,
                                  std::string & description) const;

    /*! Get description of a function in a required  interface */
    void GetDescriptionOfEventHandler(const std::string & processName,
                                      const std::string & componentName,
                                      const std::string & requiredInterfaceName,
                                      const std::string & eventHandlerName,
                                      std::string & description) const;

    /*! Get parameter information (name, argument count, argument type) */
    void GetArgumentInformation(const std::string & processName,
                                const std::string & componentName,
                                const std::string & providedInterfaceName,
                                const std::string & commandName,
                                std::string & argumentName,
                                std::vector<std::string> & argumentParameterNames) const;

    /*! Get current values of read commands */
    void GetValuesOfCommand(const std::string & processName,
                            const std::string & componentName,
                            const std::string & providedInterfaceName,
                            const std::string & commandName,
                            const int scalarIndex,
                            mtsManagerLocalInterface::SetOfValues & values) const;
#endif

    /*! Get a process object (local component manager object) */
    mtsManagerLocalInterface * GetProcessObject(const std::string & processName);

    /*! Generate unique id of an interface as string */
    inline static const std::string GetInterfaceUID(
        const std::string & processName, const std::string & componentName, const std::string & interfaceName)
    {
        return processName + ":" + componentName + ":" + interfaceName;
    }

    /*! Generate unique name of a proxy component */
    inline static const std::string GetComponentProxyName(const std::string & processName, const std::string & componentName) {
        return processName + ":" + componentName + "Proxy";
    }

    //-------------------------------------------------------------------------
    //  Networking
    //-------------------------------------------------------------------------
#if CISST_MTS_HAS_ICE
    /*! Start network proxy server. The server's listening port number is
    fetched from config.server file. (default port number: 10705) */
    bool StartServer();

    /*! Stop network proxy server */
    bool StopServer();

    bool SetInterfaceProvidedProxyAccessInfo(
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName,
        const std::string & endpointInfo);

    /*! \brief Fetch information to access (connect to) network proxy server
        \param clientProcessName Name of client process
        \param clientComponentName Name of client component
        \param clientInterfaceRequiredName Name of required interface
        \param serverProcessName Name of server process
        \param serverComponentName Name of server component
        \param serverInterfaceProvidedName Name of provided interface
        \param [out] endpointInfo Access information
        \return True if success, false otherwise */
    bool GetInterfaceProvidedProxyAccessInfo(
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceProvidedName,
        std::string & endpointInfo);

    /*! Periodically check connection element map to cancel timed out connection
        elements. */
    void ConnectCheckTimeout();
#endif
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsManagerGlobal)

#endif // _mtsManagerGlobal_h
