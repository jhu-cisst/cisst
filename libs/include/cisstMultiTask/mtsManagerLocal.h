/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung
  Created on: 2009-12-07

  (C) Copyright 2009-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Declaration of Local Component Manager
  \ingroup cisstMultiTask

  This class defines the local component manager (LCM) that manages local
  components and is unique in a process.  Since only one instance of LCM should
  exist in a process, this class is implemented as singleton.  To get an
  instance of LCM, therefore, mtsManagerLocal::GetInstance() should be used
  (instead of constructor).

  The LCM replaces the previous task manager (mtsTaskManager) which was similar
  to the LCM without networking support.  Major differences between the two are:

  1) The LCM manages tasks and components as a unified object--component--which
  is of type mtsComponent and is stored in a single data structure--component map.
  That is, task map and device map in the previous task manager has been
  consolidated into the component map.

  2) While the previous task manager keeps all information about tasks (devices)
  and connections, the LCM only keeps information about local components; it does
  not keep any connection information.  All connection information are now
  managed by the global component manager (GCM) and the LCM requests and retrieves
  such information as needed.

  Note that this class implements mtsManagerLocalInterface class which defines
  common APIs to communicate with the GCM and is declared as pure virtual. See
  mtsManagerProxyServer class as another example that implements the interface.

  \note Related classes: mtsManagerLocalInterface, mtsManagerGlobalInterface,
  mtsManagerGlobal, mtsManagerProxyServer
*/

#ifndef _mtsManagerLocal_h
#define _mtsManagerLocal_h

#include <cisstCommon/cmnNamedMap.h>
#include <cisstCommon/cmnUnits.h>
#include <cisstOSAbstraction/osaThreadBuddy.h>
#include <cisstOSAbstraction/osaTimeServer.h>
#include <cisstOSAbstraction/osaMutex.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsComponentState.h>
#include <cisstMultiTask/mtsManagerLocalInterface.h>
#include <cisstMultiTask/mtsManagerGlobalInterface.h>

#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsManagerLocal: public mtsManagerLocalInterface
{
    // for unit-testing
    friend class mtsManagerLocalTest;
    friend class mtsManagerGlobalTest;
    // for internal access to manage proxy objects
    friend class mtsManagerGlobal;
    friend class mtsManagerProxyClient;
    // for dynamic creation of a component
    friend class mtsManagerComponentClient;
    // for reconfiguration
    friend class mtsComponentProxy;

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    /*! Typedef for local component manager's configuration */
    enum ConfigurationType {
        // Standalone mode: supports only local components/connections
        LCM_CONFIG_STANDALONE
#if CISST_MTS_HAS_ICE
        // Networked mode: supports both local and remote components/connections
        , LCM_CONFIG_NETWORKED
        // Networked mode with global component manager: basically identical to
        // LCM_CONFIG_NETWORKED configuration except that LCM runs with the
        // global component manager on the same process.
        , LCM_CONFIG_NETWORKED_WITH_GCM
#endif
    };


private:
    /*! Singleton object */
    static mtsManagerLocal * Instance;

    /*! Flag for unit tests. Enabled only for unit tests (false by default) */
    static bool UnitTestEnabled;

    /*! Flag that allows unit tests to skip network-related processings such as
        network proxy creation/setup or remote connection (false by default) */
    static bool UnitTestNetworkProxyEnabled;

    ConfigurationType Configuration;

    /*! Temporary singleton object for reconfiguration.  Should not be used
        except reconfiguration */
    static mtsManagerLocal * InstanceReconfiguration;
    static mtsManagerLocal * GetSafeInstance(void);

protected:
    /*! Typedef for component map: key is component name, value is component
        object */
    typedef cmnNamedMap<mtsComponent> ComponentMapType;
    ComponentMapType ComponentMap;

    /*! Time server used by all tasks. */
    osaTimeServer TimeServer;

    /*! Process name of this local component manager. Should be globally unique
        across the whole system. */
    std::string ProcessName;

    /*! IP address of the global component manager that this LCM connects to */
    const std::string GlobalComponentManagerIP;

    /*! IP address of this machine. Internally set by SetIPAddress(). */
    std::string ProcessIP;

    /*! List of all IP addresses detected on this machine */
    std::vector<std::string> ProcessIPList;

    /*! Mutex to use ComponentMap safely */
    osaMutex ComponentMapChange;

    /*! Mutex for thread-safe transition of configuration from standalone mode to
        networked mode */
    static osaMutex ConfigurationChange;

    /*! Pointer to the global component manager.
        Depending on configuration, this can be of two different type of object:
        - In standalone mode: an instance of the GCM (of type
          mtsManagerGlobal) which runs in the same process.
        - In networked mode: a network proxy object for the GCM
          (of type mtsManagerGlobalProxyClient) that possibly runs in a
          different process or a different host. */
    mtsManagerGlobalInterface * ManagerGlobal;

    /*! Manager component instances (for direct access) */
    struct {
        mtsManagerComponentClient * Client;
        mtsManagerComponentServer * Server;
    } ManagerComponent;

    /*! If connection to GCM is active */
    bool GCMConnected;

    /*! Protected constructor (singleton) */
    mtsManagerLocal(void);

#if CISST_MTS_HAS_ICE
    mtsManagerLocal(const std::string & globalComponentManagerIP,
                    const std::string & thisProcessName,
                    const std::string & thisProcessIP);
    mtsManagerLocal(mtsManagerGlobal & globalComponentManager);
#endif

    /*! Destructor. Includes OS-specific cleanup. */
    virtual ~mtsManagerLocal();

    /*! Initialization */
    void Initialize(void);

    /*! \brief Create internal manager components automatically when LCM is
               initialized.  */
    bool CreateManagerComponents(void);

    /*! \brief Add an internal manager component
        \param processName Name of this process (or this LCM)
        \param isServer True to create manager component server, false to create
               manager component client.  Note that this argument should be true
               only when LCM runs with GCM in the same process. */
    bool AddManagerComponent(const std::string & processName, const bool isServer = false);

    /*! \brief Connect manager component client to manager component server
               (connect InterfaceLCM.Required - InterfaceGCM.Provided)
               This will make the manager component server dynamically create
               a required interface which connects to InterfaceLCM's provided
               interface. */
    bool ConnectManagerComponentClientToServer(void);

    /*! \brief Connect a local component which has internal interfaces to the
               manager component client (connect InterfaceInternal.Required -
               InterfaceComponent.Provided) */
    bool ConnectToManagerComponentClient(const std::string & componentName);

#if CISST_MTS_HAS_ICE
    /*! \brief Set IP address of this machine */
    void SetIPAddress(void);

    /*! \brief Create Ice proxy for this LCM and connects to the GCM
        \return True if success, false otherwise */
    bool ConnectToGlobalComponentManager(void);
#endif

    /*! \brief Register all interfaces that a component owns to the global
               component manager.  The GCM uses this information to connect
               interfaces that are in different processes.
        \param component Component object instance
        \param componentName Name of component */
    bool RegisterInterfaces(mtsComponent * component);
    bool RegisterInterfaces(const std::string & componentName);

    // PK: following two methods were part of Connect method
    ConnectionIDType ConnectSetup(const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
                                  const std::string & serverComponentName, const std::string & serverInterfaceProvidedName);

    bool ConnectNotify(ConnectionIDType connectionId,
                       const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
                       const std::string & serverComponentName, const std::string & serverInterfaceProvidedName);

    /*! Remove component from this local component manager */
    bool RemoveComponent(mtsComponent * component, const bool notifyGCM);
    bool RemoveComponent(const std::string & componentName, const bool notifyGCM);

    /*! Remove provided interface */
    // MJ: Current implemention should be reviwed -- interfaces have to be removed in a thread-safe way
    bool RemoveInterfaceProvided(const std::string & componentName, const std::string & interfaceProvidedName);

    /*! Remove required interface */
    // MJ: Current implemention should be reviwed -- interfaces have to be removed in a thread-safe way
    bool RemoveInterfaceRequired(const std::string & componentName, const std::string & interfaceRequiredName);

    //-------------------------------------------------------------------------
    //  Methods required by mtsManagerLocalInterface
    //
    //  See mtsManagerLocalInterface.h for detailed documentation.
    //-------------------------------------------------------------------------
#if CISST_MTS_HAS_ICE
    /*! \brief Create component proxy
        \param componentProxyName Name of component proxy
        \param listenerID Not used
        \note This should be called before an interface proxy is created. */
    bool CreateComponentProxy(const std::string & componentProxyName, const std::string & listenerID = "");

    /*! \brief Remove component proxy
        \param componentProxyName Name of component proxy
        \param listenerID Not used
        \note Note that all the interface proxies that the proxy manages is
              automatically removed when removing a component proxy. */
    bool RemoveComponentProxy(const std::string & componentProxyName, const std::string & listenerID = "");

    /*! \brief Create provided interface proxy
        \param serverComponentProxyName Name of server component proxy
        \param providedInterfaceDescription Description of provided interface */
    bool CreateInterfaceProvidedProxy(
        const std::string & serverComponentProxyName,
        const InterfaceProvidedDescription & providedInterfaceDescription, const std::string & listenerID = "");

    /*! \brief Create required interface proxy
        \param clientComponentProxyName Name of component proxy that has */
    bool CreateInterfaceRequiredProxy(
        const std::string & clientComponentProxyName,
        const InterfaceRequiredDescription & requiredInterfaceDescription, const std::string & listenerID = "");

    /*! Remove provided interface proxy */
    bool RemoveInterfaceProvidedProxy(
        const std::string & componentProxyName, const std::string & providedInterfaceProxyName, const std::string & listenerID = "");

    /*! Remove required interface proxy */
    bool RemoveInterfaceRequiredProxy(
        const std::string & componentProxyName, const std::string & requiredInterfaceProxyName, const std::string & listenerID = "");

    /*! Connect two local interfaces at the server side */
    bool ConnectServerSideInterface(const mtsDescriptionConnection & description, const std::string & listenerID = "");

    /*! \brief Connect two local interfaces at the client side */
    bool ConnectClientSideInterface(const mtsDescriptionConnection & description, const std::string & listenerID = "");
#endif

    /*! Get information about provided interface */
    bool GetInterfaceProvidedDescription(
        const std::string & serverComponentName,
        const std::string & providedInterfaceName,
        InterfaceProvidedDescription & providedInterfaceDescription, const std::string & listenerID = "");

    /*! Extract all the information on a required interface such as function
        objects and event handlers with arguments serialized */
    bool GetInterfaceRequiredDescription(
        const std::string & componentName,
        const std::string & requiredInterfaceName,
        InterfaceRequiredDescription & requiredInterfaceDescription, const std::string & listenerID = "");

public:
    //-------------------------------------------------------------------------
    //  Component Management
    //-------------------------------------------------------------------------
    /*! \brief Create a component.  Does not add it to the local component manager. */
    mtsComponent * CreateComponentDynamically(const std::string & className, const std::string & componentName,
                                              const std::string & constructorArgSerialized);

    /*! \brief Add a component to this local component manager.
        \param component Component instance to be added */
    bool AddComponent(mtsComponent * component);
    bool CISST_DEPRECATED AddTask(mtsTask * component); // For backward compatibility
    bool CISST_DEPRECATED AddDevice(mtsComponent * component); // For backward compatibility

    /*! \brief Remove component from this local component manager. */
    bool RemoveComponent(mtsComponent * component);
    bool RemoveComponent(const std::string & componentName);
    
    /*! \brief Retrieve a component by name. */
    mtsComponent * GetComponent(const std::string & componentName) const;
    mtsTask * GetComponentAsTask(const std::string & componentName) const;

    mtsComponent CISST_DEPRECATED * GetDevice(const std::string & deviceName); // For backward compatibility
    mtsTask CISST_DEPRECATED * GetTask(const std::string & taskName); // For backward compatibility

    /*! \brief Check if a component exists by its name */
    bool FindComponent(const std::string & componentName) const;

    /*! Wait until all components reach a certain state.  If all
      components have reach the given state within the time alloted,
      the method returns true. */
    bool WaitForStateAll(mtsComponentState desiredState, double timeout = 3.0 * cmn_minute) const;

    /*! \brief Create all components. If a component is of type mtsTask,
      mtsTask::Create() is called internally. */
    void CreateAll(void);

    /*! \brief Start all components. If a component is of type mtsTask,
      mtsTask::Start() is called internally. */
    void StartAll(void);

    /*! \brief Stop all components. If a component is of type mtsTask,
      mtsTask::Kill() is called internally. */
    void KillAll(void);

    /*! \brief Cleanup.  Since a local component manager is a singleton, the
               destructor will be called when the program exits but a library
               user is not capable of handling the timing. Thus, for safe
               termination, this method should be called before an application
               quits. */
    void Cleanup(void);

    //-------------------------------------------------------------------------
    //  Connection Management
    //-------------------------------------------------------------------------
    /*! \brief Connect two local interfaces
        \param clientComponentName Name of client component
        \param clientInterfaceRequiredName Name of required interface
        \param serverComponentName Name of server component
        \param serverInterfaceProvidedName Name of provided interface
        \return True if success, false otherwise
        \note If connection is established successfully, this information is
              reported to the global component manager (the local component
              manager does not keep any connection information). */
    bool Connect(const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
                 const std::string & serverComponentName, const std::string & serverInterfaceProvidedName);

#if CISST_MTS_HAS_ICE
    /*! \brief Connect two remote interfaces
        \param clientProcessName Name of client process
        \param clientComponentName Name of client component
        \param clientInterfaceRequiredName Name of required interface
        \param serverProcessName Name of server process
        \param serverComponentName Name of server component
        \param serverInterfaceProvidedName Name of provided interface
        \param retryCount Number of times this connection is retried (default: 10)
        \return True if success, false otherwise
        \note If connection is established successfully, this information is
              reported to the global component manager. Since connection between
              two interfaces should be established twice--once in the client
              process and once in the server process--there are two internal
              connection management methods: ConnectClientSideInterface() and
              ConnectServerSideInterface().  ConnectClientSideInterface() is
              always executed first and calls ConnectServerSideInterface()
              internally in a blocking way (i.e., it waits for
              ConnectServerSideInterface() to finish).
              Connection request can be made by any process -- server process,
              client process, or even third process -- and the result should
              be the same regardless the request process.
              If this method is called against two local interfaces, the other
              Connect() method is internally called instead. */
    bool Connect(const std::string & clientProcessName, const std::string & clientComponentName,
                 const std::string & clientInterfaceRequiredName,
                 const std::string & serverProcessName, const std::string & serverComponentName,
                 const std::string & serverInterfaceProvidedName,
                 const unsigned int retryCount = 10);
#endif

    /*! Disconnect two interfaces */
    bool Disconnect(const ConnectionIDType connectionID);

    bool Disconnect(const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
                    const std::string & serverComponentName, const std::string & serverInterfaceProvidedName);

#if CISST_MTS_HAS_ICE
    bool Disconnect(const std::string & clientProcessName, const std::string & clientComponentName,
                    const std::string & clientInterfaceRequiredName,
                    const std::string & serverProcessName, const std::string & serverComponentName,
                    const std::string & serverInterfaceProvidedName);
#endif

    //-------------------------------------------------------------------------
    //  Getters and Utilities
    //-------------------------------------------------------------------------
    /*! Default name of local component manager */
    static std::string ProcessNameOfLCMDefault;

    /*! Name of local component manager running with the global component manager */
    static std::string ProcessNameOfLCMWithGCM;

#if !CISST_MTS_HAS_ICE
    /*! Get a singleton object of local component manager */
    static mtsManagerLocal * GetInstance(void);
#else
    /*! \brief Return singleton object of local component manager.
        \param globalComponentManagerIP Ip address of global component manager
               that this local component manager connects to
        \param thisProcessName Name of this process. If not specified, set as
               ip address of this host by default
        \param thisProcessIP IP address of this process. If not specified, set
               as the first ip address detected
        \return Pointer to singleton object
        \note  If no argument is specified, local component manager is
               configured as the standalone mode. If this method is called again
               with proper arguments, the singleton object is reconfigured as
               the networked mode to support inter-process communication. During
               this reconfiguration process, a caller thread is blocked for
               thread-safe transition of all internal data. */
    static mtsManagerLocal * GetInstance(const std::string & globalComponentManagerIP = "",
                                         const std::string & thisProcessName = "",
                                         const std::string & thisProcessIP = "");

    static mtsManagerLocal * GetInstance(mtsManagerGlobal & globalComponentManager);

#endif

    /*! Enumerate all the names of components added */
    std::vector<std::string> GetNamesOfComponents(void) const;
    std::vector<std::string> CISST_DEPRECATED GetNamesOfDevices(void) const;  // For backward compatibility
    std::vector<std::string> CISST_DEPRECATED GetNamesOfTasks(void) const;  // For backward compatibility

    void GetNamesOfComponents(std::vector<std::string>& namesOfComponents) const;
    void CISST_DEPRECATED GetNamesOfDevices(std::vector<std::string>& namesOfDevices) const; // For backward compatibility
    void CISST_DEPRECATED GetNamesOfTasks(std::vector<std::string>& namesOfTasks) const; // For backward compatibility

    /*! Return a reference to the time server. */
    inline const osaTimeServer & GetTimeServer(void) const {
        return TimeServer;
    }

    /*! Returns name of this local component manager */
    inline const std::string GetProcessName(const std::string & CMN_UNUSED(listenerID) = "") const {
        return ProcessName;
    }

    /*! Returns the current configuration of this local component manager */
    ConfigurationType GetConfiguration(void) const {
        return Configuration;
    }

    /*! Check if connection to GCM is active */
    inline bool IsGCMActive(void) const {
        return GCMConnected;
    }

    inline void SetGCMConnected(const bool connected) {
        GCMConnected = connected;
    }

#if CISST_MTS_HAS_ICE
    /*! Get names of all commands in a provided interface */
    void GetNamesOfCommands(std::vector<std::string>& namesOfCommands,
                            const std::string & componentName,
                            const std::string & providedInterfaceName,
                            const std::string & CMN_UNUSED(listenerID) = "");

    /*! Get names of all event generators in a provided interface */
    void GetNamesOfEventGenerators(std::vector<std::string>& namesOfEventGenerators,
                                   const std::string & componentName,
                                   const std::string & providedInterfaceName,
                                   const std::string & CMN_UNUSED(listenerID) = "");

    /*! Get names of all functions in a required interface */
    void GetNamesOfFunctions(std::vector<std::string>& namesOfFunctions,
                             const std::string & componentName,
                             const std::string & requiredInterfaceName,
                             const std::string & CMN_UNUSED(listenerID) = "");

    /*! Get names of all event handlers in a required interface */
    void GetNamesOfEventHandlers(std::vector<std::string>& namesOfEventHandlers,
                                 const std::string & componentName,
                                 const std::string & requiredInterfaceName,
                                 const std::string & CMN_UNUSED(listenerID) = "");

    /*! Get description of a command in a provided interface */
    void GetDescriptionOfCommand(std::string & description,
                                 const std::string & componentName,
                                 const std::string & providedInterfaceName,
                                 const std::string & commandName,
                                 const std::string & CMN_UNUSED(listenerID) = "");

    /*! Get description of a event generator in a provided interface */
    void GetDescriptionOfEventGenerator(std::string & description,
                                        const std::string & componentName,
                                        const std::string & providedInterfaceName,
                                        const std::string & eventGeneratorName,
                                        const std::string & CMN_UNUSED(listenerID) = "");

    /*! Get description of a function in a required interface */
    void GetDescriptionOfFunction(std::string & description,
                                  const std::string & componentName,
                                  const std::string & requiredInterfaceName,
                                  const std::string & functionName,
                                  const std::string & CMN_UNUSED(listenerID) = "");

    /*! Get description of a function in a required  interface */
    void GetDescriptionOfEventHandler(std::string & description,
                                      const std::string & componentName,
                                      const std::string & requiredInterfaceName,
                                      const std::string & eventHandlerName,
                                      const std::string & CMN_UNUSED(listenerID) = "");

    /*! Get parameter information (name, argument count, argument type) */
    void GetArgumentInformation(std::string & argumentName,
                                std::vector<std::string> & signalNames,
                                const std::string & componentName,
                                const std::string & providedInterfaceName,
                                const std::string & commandName,
                                const std::string & CMN_UNUSED(listenerID) = "");

    /*! Get a set of current values with timestamp for data visualization */
    void GetValuesOfCommand(SetOfValues & values,
                            const std::string & componentName,
                            const std::string & providedInterfaceName,
                            const std::string & commandName,
                            const int scalarIndex,
                            const std::string & CMN_UNUSED(listenerID) = "");

    /*! Return IP address of this process */
    inline const std::string & GetIPAddress(void) const { return ProcessIP; }

    /*! Return a list of all IP addresses detected on this machine. */
    static std::vector<std::string> GetIPAddressList(void);
    static void GetIPAddressList(std::vector<std::string> & ipAddresses);

    /*! Returns name of this local component manager (for mtsProxyBaseCommon.h) */
    inline const std::string GetName(void) const {
        return GetProcessName();
    }
#endif

    /*! For debugging. Dumps to stream the maps maintained by the manager. */
    void /*CISST_DEPRECATED*/ ToStream(std::ostream & outputStream) const;

    /*! Create a dot file to be used by graphviz to generate a nice
      graph of connections between tasks/interfaces. */
    void /*CISST_DEPRECATED*/ ToStreamDot(std::ostream & outputStream) const;

    //-------------------------------------------------------------------------
    //  Networking
    //-------------------------------------------------------------------------
#if CISST_MTS_HAS_ICE
public:
    /*! Set endpoint access information */
    bool SetInterfaceProvidedProxyAccessInfo(const ConnectionIDType connectionID, const std::string & endpointInfo);
#endif
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsManagerLocal)

#endif // _mtsManagerLocal_h

