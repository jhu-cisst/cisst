/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2009-11-12

  (C) Copyright 2009-2019 Johns Hopkins University (JHU), All Rights Reserved.

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
#include <cisstOSAbstraction/osaThread.h>

#include <cisstMultiTask/mtsManagerLocalInterface.h>
#include <cisstMultiTask/mtsManagerGlobalInterface.h>
#include <cisstMultiTask/mtsParameterTypes.h>
#include <cisstMultiTask/mtsConnection.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>

#include <cisstMultiTask/mtsExport.h>

// Forward declaration
class mtsManagerComponentServer;

class CISST_EXPORT mtsManagerGlobal : public mtsManagerGlobalInterface
{
    friend class mtsManagerGlobalTest;
    friend class mtsManagerLocalTest;

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

protected:
    //-------------------------------------------------------------------------
    //  Data Structure of Process Map
    //-------------------------------------------------------------------------
    /*
        P1 - C1 - p1 - r1   Process Map: (P, ComponentMap)
        |    |    |    |    Component Map: (C, InterfaceMap)
        |    |    |    r2   Interface Map: { (PI, ConnectionMap) where PI is Provided Interface
        |    |    |                          (RI, ConnectionMap) where RI is Required Interface }
        |    |    p2 - r3   Connection Map: (name of connected interface, list of connection id(s))
        |    |    |
        |    |    r1 - p1
        |    |
        |    C2 - r1 - p2
        |
        P2 - C1
             |
             C2
    */

    /*! Typedef for connection map:
            key=(connected interface name), value=(list of connection ids)
            map name=(name of component that owns these interfaces. */
    //typedef cmnNamedMap<mtsManagerGlobal::ConnectedInterfaceInfo> ConnectionMapType;
    typedef std::vector<ConnectionIDType> ConnectionIDListType;

    /*! Typedef for interface map element:
            key=(interface name), value=(connection id list type) */
    //typedef cmnNamedMap<ConnectionIDListType> ConnectedInterfaceMapType;
    typedef cmnNamedMap<ConnectionIDListType> InterfaceMapElementType;

    /*! Typedef for interface map */
    typedef struct {
        //ConnectedInterfaceMapType InterfaceProvidedOrOutputMap;
        //ConnectedInterfaceMapType InterfaceRequiredOrInputMap;
        InterfaceMapElementType InterfaceProvidedOrOutputMap;
        InterfaceMapElementType InterfaceRequiredOrInputMap;
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
        key=(connection id), value=(an instance of Connection)
        When a local component manager requests estbalishing a connection, an
        element is created and added. If a connection is not established before
        timeout, the element is removed. When a local component manager notifies
        that a connection is successfully established, the element is marked
        as connected. */
    typedef std::map<ConnectionIDType, mtsConnection> ConnectionMapType;
    ConnectionMapType ConnectionMap;

    /*! Instance of connected local component manager. Note that the global
        component manager communicates with the only one instance of
        mtsManagerLocalInterface regardless of connection type (standalone
        or network mode)

        MJ: (8/20/10) To support a local connection between the global component
        manager and a local component manager on the same process, this
        assumption is slightly modified such that the GCM can have two different
        types of connection with LCM -- local and remote.  When the GCM
        executes commands, it checks if it has a local connection to the LCM on
        the same process and, if yes, it sends the command to the local LCM.
        If not, it delivers the command to a remote LCM same as before. */
    mtsManagerLocal * LocalManager;
    mtsManagerLocalInterface * LocalManagerConnected;

    /*! Mutex for thread-safe processing */
    osaMutex ProcessMapChange;    // for thread-safe ProcessMap update
    osaMutex ConnectionMapChange; // for thread-safe ConnectionMap update
    osaMutex ConnectionChange;    // to process Connect() request one-by-one
    osaMutex DisconnectedProcessCleanupMapChange; // for thread-safe DisconnectedProcessCleanupMap update

    /*! Counter to issue a new connection ID */
    ConnectionIDType ConnectionID;

    /*! Network proxy server */
    mtsManagerProxyServer * ProxyServer;

    /*! For dynamic component composition feature */
    mtsManagerComponentServer * ManagerComponentServer;

    /*! Queues, mutexes, and internal thread for thread-safe disconnection */
    typedef std::map<ConnectionIDType, ConnectionIDType> DisconnectQueueType;
    DisconnectQueueType QueueDisconnectWaiting;
    DisconnectQueueType QueueDisconnected;

    osaMutex QueueDisconnectWaitingChange;
    osaMutex QueueDisconnectedChange;

    osaThread ThreadDisconnect;
    osaThreadSignal ThreadDisconnectFinished;
    bool ThreadDisconnectRunning;
    void * ThreadDisconnectProcess(void * arg);

    /*! To clean up disconnected process */
    typedef struct CleanupElementType {
        std::string ProcessName;
        std::string ComponentProxyName;
    } CleanupElementType;
    typedef std::list<CleanupElementType> CleanupElementListType;
    typedef cmnNamedMap<CleanupElementListType> DisconnectedProcessCleanupMapType;
    DisconnectedProcessCleanupMapType DisconnectedProcessCleanupMap;

    /*! Prints out ProcessMap in human readable format */
    void ShowInternalStructure(void);

    //-------------------------------------------------------------------------
    //  Processing Methods
    //-------------------------------------------------------------------------
    /*! Reset internal data structures */
    void Cleanup(void);

    /*! Get connection information about provided interface specified */
    ConnectionIDListType * GetConnectionsOfInterfaceProvidedOrOutput(
        const std::string & serverProcessName, const std::string & serverComponentName,
        const std::string & interfaceName) const;

    ConnectionIDListType * GetConnectionsOfInterfaceProvidedOrOutput(
        const std::string & serverProcessName, const std::string & serverComponentName,
        const std::string & interfaceName, InterfaceMapType ** interfaceMap) const;

    /*! Get connection information about required interface specified */
    ConnectionIDListType * GetConnectionsOfInterfaceRequiredOrInput(
        const std::string & clientProcessName, const std::string & clientComponentName,
        const std::string & interfaceName) const;

    ConnectionIDListType * GetConnectionsOfInterfaceRequiredOrInput(
        const std::string & clientProcessName, const std::string & clientComponentName,
        const std::string & interfaceName, InterfaceMapType ** interfaceMap) const;

    /*! Add new connection id to specified interface */
    bool AddConnectionToInterfaceProvidedOrOutput(
        const std::string & serverProcessName, const std::string & serverComponentName,
        const std::string & interfaceName, const ConnectionIDType connectionID);

    bool AddConnectionToInterfaceRequiredOrInput(
        const std::string & clientProcessName, const std::string & clientComponentName,
        const std::string & interfaceName, const ConnectionIDType connectionID);

    /*! Remove connection id from specified interface */
    bool RemoveConnectionOfInterfaceProvidedOrOutput(
        const std::string & serverProcessName, const std::string & serverComponentName,
        const std::string & interfaceName, const ConnectionIDType connectionID);

    bool RemoveConnectionOfInterfaceRequiredOrInput(
        const std::string & clientProcessName, const std::string & clientComponentName,
        const std::string & interfaceName, const ConnectionIDType connectionID);

    /*! Remove component proxies and internal interfaces (InterfaceComponentRequiredForXXX)
        that were created because of the disconnected process */
    bool CleanupDisconnectedProcess(const std::string & nameOfProcessDisconnected);

    /*! Check if two interfaces are connected */
    bool IsAlreadyConnected(const mtsDescriptionConnection & description) const;

    /*! Get total number of interfaces a component manages */
    int GetNumberOfInterfaces(const std::string & processName, const std::string & componentName, const bool includeInternalInterface = true) const;

    /*! Get connection id that the required interface specified involves in */
    ConnectionIDType GetConnectionID(const std::string & clientProcessName,
        const std::string & clientComponentName, const std::string & interfaceName) const;

    /*! Get connection information using connection id */
    mtsConnection * GetConnectionInformation(const ConnectionIDType connectionID);

    /*! Process disconnect waiting queue.  This is periodically called by the
        internal processing thread.  The connection id that is disconnected
        is dequeued from disconnect waiting queue and enqueued to disconnected queue. */
    void DisconnectInternal(void);

    /*! Maintains information to clean up disconnected processes */
    void AddToDisconnectedProcessCleanup(const std::string & sourceProcessName,
        const std::string & targetProcessName, const std::string & targetComponentProxyName);

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

    bool RemoveProcess(const std::string & processName, const bool networkDisconnect = false);

    //-------------------------------------------------------------------------
    //  Component Management
    //-------------------------------------------------------------------------
    bool AddComponent(const std::string & processName, const std::string & componentName);

    bool FindComponent(const std::string & processName, const std::string & componentName) const;

    bool RemoveComponent(const std::string & processName, const std::string & componentName, const bool lock = true);

    //-------------------------------------------------------------------------
    //  Interface Management
    //-------------------------------------------------------------------------
    bool AddInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName, const std::string & interfaceName);

    bool AddInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName, const std::string & interfaceName);

    bool FindInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName, const std::string & interfaceName) const;

    bool FindInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName, const std::string & interfaceName) const;

    bool RemoveInterfaceProvidedOrOutput(const std::string & processName, const std::string & componentName, const std::string & interfaceName, const bool lock = true);

    bool RemoveInterfaceRequiredOrInput(const std::string & processName, const std::string & componentName, const std::string & interfaceName, const bool lock = true);

    //-------------------------------------------------------------------------
    //  Connection Management
    //-------------------------------------------------------------------------
    ConnectionIDType Connect(const std::string & requestProcessName,
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceName);

    bool ConnectConfirm(const ConnectionIDType connectionID);

    bool Disconnect(const ConnectionIDType connectionID);

    bool Disconnect(const mtsDescriptionConnection & connection);

    bool Disconnect(
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverInterfaceName);

    bool InitiateConnect(const ConnectionIDType connectionID);

    bool ConnectServerSideInterfaceRequest(const ConnectionIDType connectionID);

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

    /*! Check if component is proxy based on its name */
    static bool IsProxyComponent(const std::string & componentName);

    /*! Generate unique name of a proxy component */
    static const std::string GetComponentProxyName(const std::string & processName, const std::string & componentName);

    /*! Get a process object (local component manager object) */
    mtsManagerLocalInterface * GetProcessObject(const std::string & processName) const;

    /*! Generate unique representation of interface as string */
    static const std::string GetInterfaceUID(
        const std::string & processName, const std::string & componentName, const std::string & interfaceName);

    //-------------------------------------------------------------------------
    //  Networking
    //-------------------------------------------------------------------------
    /*! Start network proxy server. The server's listening port number is
        fetched from config.server file (default port: 10705) */
    bool StartServer(void);

    /*! Stop network proxy server */
    bool StopServer(void);

    /*! Set access information of interface proxy server */
    bool SetInterfaceProvidedProxyAccessInfo(const ConnectionIDType connectionID, const std::string & endpointInfo);

    /*! Get access information of interface proxy server */
    bool GetInterfaceProvidedProxyAccessInfo(const ConnectionIDType connectionID, std::string & endpointInfo);

    bool GetInterfaceProvidedProxyAccessInfo(const std::string & clientProcessName,
        const std::string & serverProcessName, const std::string & serverComponentName,
        const std::string & serverInterfaceName, std::string & endpointInfo);

    /*! Check if there is any pending connection.  All new connections should be
        confirmed by the LCM within timeout after the GCM issues a new connection
        id.  Otherwise, the GCM actively disconnects the pending connection. */
    void CheckConnectConfirmTimeout(void);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsManagerGlobal)

#endif // _mtsManagerGlobal_h
