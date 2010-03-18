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

#include <cisstMultiTask/mtsManagerLocalInterface.h>
#include <cisstMultiTask/mtsManagerGlobalInterface.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>

#include <cisstMultiTask/mtsExport.h>

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
    public:
        // Name of connect request process
        const std::string RequestProcessName;
        // This connection ID
        const unsigned int ConnectionID;
        // Connection status. False when waiting for a successful establishment,
        // True if successfully established.
        bool Connected;
        // Set of strings
        const std::string ClientProcessName;
        const std::string ClientComponentName;
        const std::string ClientRequiredInterfaceName;
        const std::string ServerProcessName;
        const std::string ServerComponentName;
        const std::string ServerProvidedInterfaceName;
#if CISST_MTS_HAS_ICE
        // Time when this object becomes timed out
        double TimeoutTime;
#endif

        ConnectionElement(const std::string & requestProcessName, const unsigned int connectionID,
            const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
            const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName)
            : RequestProcessName(requestProcessName), ConnectionID(connectionID), Connected(false),
              ClientProcessName(clientProcessName), ClientComponentName(clientComponentName), ClientRequiredInterfaceName(clientRequiredInterfaceName),
              ServerProcessName(serverProcessName), ServerComponentName(serverComponentName), ServerProvidedInterfaceName(serverProvidedInterfaceName)
        {
#if CISST_MTS_HAS_ICE
            //const double Timeout = (double) mtsManagerProxyServer::GetGCMConnectTimeout() / 1000.0;
            const double Timeout = 5.0;
            // Time when this object is timed out is set as current time plus timeout.
            TimeoutTime = osaGetTime() + Timeout;
#endif
        }

        /*! Set this connection as established */
        inline void SetConnected() { Connected = true; }

        /*! Return true if this connection is timed out */
#if CISST_MTS_HAS_ICE
        inline bool CheckTimeout() const {
            return (TimeoutTime - osaGetTime() <= 0);
        }
#endif
    };

    /*! Connection map: (connected interface name, connected interface information)
        Map name: a name of component that has these interfaces. */
    typedef cmnNamedMap<mtsManagerGlobal::ConnectedInterfaceInfo> ConnectionMapType;

    // Interface map consists of two pairs of containers:
    // containers for connection map (provided/required interface maps) and
    // containers for interface type flag map (provided/required interface maps)

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

    typedef struct {
        ConnectedInterfaceMapType ProvidedInterfaceMap;
        ConnectedInterfaceMapType RequiredInterfaceMap;
        InterfaceTypeMapType ProvidedInterfaceTypeMap;
        InterfaceTypeMapType RequiredInterfaceTypeMap;
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
        or network mode) */
    mtsManagerLocalInterface * LocalManagerConnected;

    /*! Mutex for ConnectionElementMap because several threads possibly access
        ConnectionElementMap. */
    osaMutex ConnectionElementMapChange;

    /*! Connection id to issue a new connection session ID */
    unsigned int ConnectionID;

#if CISST_MTS_HAS_ICE
    /*! Network proxy server */
    mtsManagerProxyServer * ProxyServer;
#endif

    //-------------------------------------------------------------------------
    //  Processing Methods
    //-------------------------------------------------------------------------
    /*! Clean up the internal variables */
    bool Cleanup(void);

    /*! Get a map containing connection information for a provided interface */
    ConnectionMapType * GetConnectionsOfProvidedInterface(
        const std::string & serverProcessName, const std::string & serverComponentName,
        const std::string & providedInterfaceName, InterfaceMapType ** interfaceMap);
    ConnectionMapType * GetConnectionsOfProvidedInterface(
        const std::string & serverProcessName, const std::string & serverComponentName,
        const std::string & providedInterfaceName) const;

    /*! Get a map containing connection information for a required interface */
    ConnectionMapType * GetConnectionsOfRequiredInterface(
        const std::string & clientProcessName, const std::string & clientComponentName,
        const std::string & requiredInterfaceName, InterfaceMapType ** interfaceMap);

    ConnectionMapType * GetConnectionsOfRequiredInterface(
        const std::string & clientProcessName, const std::string & clientComponentName,
        const std::string & requiredInterfaceName) const;

    /*! Add this interface to connectionMap as connected interface */
    bool AddConnectedInterface(ConnectionMapType * connectionMap,
        const std::string & processName, const std::string & componentName,
        const std::string & interfaceName, const bool isRemoteConnection = false);

    /*! Check if two interfaces are connected */
    bool IsAlreadyConnected(
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName);

public:
    /*! Constructor and destructor */
    mtsManagerGlobal();
    ~mtsManagerGlobal();

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
    bool AddProvidedInterface(
        const std::string & processName, const std::string & componentName, const std::string & interfaceName, const bool isProxyInterface = false);

    bool AddRequiredInterface(
        const std::string & processName, const std::string & componentName, const std::string & interfaceName, const bool isProxyInterface = false);

    bool FindProvidedInterface(
        const std::string & processName, const std::string & componentName, const std::string & interfaceName) const;

    bool FindRequiredInterface(
        const std::string & processName, const std::string & componentName, const std::string & interfaceName) const;

    bool RemoveProvidedInterface(
        const std::string & processName, const std::string & componentName, const std::string & interfaceName);

    bool RemoveRequiredInterface(
        const std::string & processName, const std::string & componentName, const std::string & interfaceName);

    //-------------------------------------------------------------------------
    //  Connection Management
    //-------------------------------------------------------------------------
    unsigned int Connect(const std::string & requestProcessName,
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName);

    bool ConnectConfirm(unsigned int connectionSessionID);

    bool Disconnect(
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName);

#if CISST_MTS_HAS_ICE
    bool InitiateConnect(const unsigned int connectionID,
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName);

    bool ConnectServerSideInterface(const unsigned int providedInterfaceProxyInstanceID,
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName);
#endif

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
    void GetNamesOfProcesses(std::vector<std::string>& namesOfProcesses);

    /*! Get names of all components in a process */
    void GetNamesOfComponents(const std::string & processName, 
                              std::vector<std::string>& namesOfComponents);

    /*! Get names of all provided interfaces in a component */
    void GetNamesOfProvidedInterfaces(const std::string & processName, 
                                      const std::string & componentName, 
                                      std::vector<std::string>& namesOfProvidedInterfaces);

    /*! Get names of all required interfaces in a component */
    void GetNamesOfRequiredInterfaces(const std::string & processName, 
                                      const std::string & componentName, 
                                      std::vector<std::string>& namesOfRequiredInterfaces);

#if CISST_MTS_HAS_ICE
    /*! Get names of all commands in a provided interface */
    void GetNamesOfCommands(const std::string & processName, 
                            const std::string & componentName, 
                            const std::string & providedInterfaceName, 
                            std::vector<std::string>& namesOfCommands);

    /*! Get names of all event generators in a provided interface */
    void GetNamesOfEventGenerators(const std::string & processName, 
                                   const std::string & componentName, 
                                   const std::string & providedInterfaceName, 
                                   std::vector<std::string>& namesOfEventGenerators);

    /*! Get names of all functions in a required interface */
    void GetNamesOfFunctions(const std::string & processName, 
                             const std::string & componentName, 
                             const std::string & requiredInterfaceName, 
                             std::vector<std::string>& namesOfFunctions);

    /*! Get names of all event handlers in a required interface */
    void GetNamesOfEventHandlers(const std::string & processName, 
                                 const std::string & componentName, 
                                 const std::string & requiredInterfaceName, 
                                 std::vector<std::string>& namesOfEventHandlers);

    /*! Get description of a command in a provided interface */
    void GetDescriptionOfCommand(const std::string & processName, 
                                 const std::string & componentName, 
                                 const std::string & providedInterfaceName, 
                                 const std::string & commandName,
                                 std::string & description);

    /*! Get description of a event generator in a provided interface */
    void GetDescriptionOfEventGenerator(const std::string & processName, 
                                        const std::string & componentName, 
                                        const std::string & providedInterfaceName, 
                                        const std::string & eventGeneratorName,
                                        std::string & description);

    /*! Get description of a function in a required interface */
    void GetDescriptionOfFunction(const std::string & processName, 
                                  const std::string & componentName, 
                                  const std::string & requiredInterfaceName, 
                                  const std::string & functionName,
                                  std::string & description);

    /*! Get description of a function in a required  interface */
    void GetDescriptionOfEventHandler(const std::string & processName, 
                                      const std::string & componentName, 
                                      const std::string & requiredInterfaceName, 
                                      const std::string & eventHandlerName,
                                      std::string & description);

    /*! Get parameter information (name, argument count, argument type) */
    void GetArgumentInformation(const std::string & processName, 
                                const std::string & componentName, 
                                const std::string & providedInterfaceName, 
                                const std::string & commandName,
                                std::string & argumentName,
                                std::vector<std::string> & argumentParameterNames);

    /*! Get current values of read commands */
    void GetValuesOfCommand(const std::string & processName, 
                            const std::string & componentName, 
                            const std::string & providedInterfaceName, 
                            const std::string & commandName,
                            mtsManagerLocalInterface::SetOfValues & values);
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

    bool SetProvidedInterfaceProxyAccessInfo(
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName,
        const std::string & endpointInfo);

    bool GetProvidedInterfaceProxyAccessInfo(
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName,
        std::string & endpointInfo);

    /*! Periodically check connection element map to cancel timed out connection
        elements. */
    void ConnectCheckTimeout();
#endif
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsManagerGlobal)

#endif // _mtsManagerGlobal_h
