/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Min Yang Jung
  Created on: 2009-12-07

  (C) Copyright 2009-2020 Johns Hopkins University (JHU), All Rights Reserved.

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
#include <cisstCommon/cmnPath.h>
#include <cisstOSAbstraction/osaThreadBuddy.h>
#include <cisstOSAbstraction/osaMutex.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsComponentState.h>
#include <cisstMultiTask/mtsManagerLocalInterface.h>
#include <cisstMultiTask/mtsManagerGlobalInterface.h>

#include <stack>

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
        LCM_CONFIG_STANDALONE,
        // Networked mode: supports both local and remote components/connections
        LCM_CONFIG_NETWORKED,
        // Networked mode with global component manager: basically identical to
        // LCM_CONFIG_NETWORKED configuration except that LCM runs with the
        // global component manager on the same process.
        LCM_CONFIG_NETWORKED_WITH_GCM
    };


private:
    /*! Singleton object */
    static mtsManagerLocal * Instance;

    /*! Thread ID of main thread */
    osaThreadId MainThreadId;

    /*! List of main tasks (in chronological order) */
    std::stack<std::string> MainTaskNames;

    /*! Pointer to task that currently has main thread (set when that task is started) */
    mtsTaskContinuous * CurrentMainTask;

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

    /*! Typedef for component map: key is component name, value is component
        object */
    typedef cmnNamedMap<mtsComponent> ComponentMapType;
    ComponentMapType ComponentMap;

    /*! Time server used by all tasks. */
    // MJ: Move this to mtsManagerLocal.cpp (for system-wide logging)
    //osaTimeServer TimeServer;

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

    /*! Internal thread to buffer log messages */
    osaThread       LogThead;
    osaThreadSignal LogTheadFinished;
    bool            LogThreadFinishWaiting;
    void*           LogDispatchThread(void * arg);

    /*! Set up system logger that allows collecting system-wide logs across network */
    void SetupSystemLogger(void);

    /*! If Manager Component Client (MCC) is ready to forward logs to
        Manager Component Server (MCS) */
    bool MCCReadyForLogForwarding(void) const;

public:
    /*! Callback function for system-wide thread-safe logging */
    static void LogDispatcher(const char * str, int len);

    /*! Enable or disable system-wide thread-safe logging */
    static void SetLogForwarding(bool activate);

    /*! Get whether system-wide logging is enabled or not */
    static void GetLogForwardingState(bool & state);
    static bool GetLogForwardingState(void);

    /*! Is system-wide thread-safe logging enabled? */
    static bool IsLogForwardingEnabled(void);

    /*! Check if further logs are allowed */
    static bool IsLogAllowed(void);

protected:
    /*! Protected constructor (singleton) */
    mtsManagerLocal(void);

    mtsManagerLocal(const std::string & globalComponentManagerIP,
                    const std::string & thisProcessName,
                    const std::string & thisProcessIP);
    mtsManagerLocal(mtsManagerGlobal & globalComponentManager);

    /*! Destructor. Includes OS-specific cleanup. */
    virtual ~mtsManagerLocal();

    /*! Initialization */
    void Initialize(void);
    void InitializeLocal(void);

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

    /*! \brief Set IP address of this machine */
    void SetIPAddress(void);

    /*! \brief Create Ice proxy for this LCM and connects to the GCM
        \return True if success, false otherwise */
    bool ConnectToGlobalComponentManager(void);

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

    /*! Get information about provided interface */
    bool GetInterfaceProvidedDescription(
        const std::string & serverComponentName,
        const std::string & providedInterfaceName,
        mtsInterfaceProvidedDescription & providedInterfaceDescription, const std::string & listenerID = "");

    /*! Extract all the information on a required interface such as function
        objects and event handlers with arguments serialized */
    bool GetInterfaceRequiredDescription(
        const std::string & componentName,
        const std::string & requiredInterfaceName,
        mtsInterfaceRequiredDescription & requiredInterfaceDescription, const std::string & listenerID = "");

    /*! Change GCM connection state */
    inline void SetGCMConnected(const bool connected) {
        GCMConnected = connected;
    }

public:
    //-------------------------------------------------------------------------
    //  Component Management
    //-------------------------------------------------------------------------
    /*! \brief Create a component.  Does not add it to the local component manager. */
    mtsComponent * CreateComponentDynamically(const std::string & className, const std::string & componentName,
                                              const std::string & constructorArgSerialized);

#if CISST_HAS_JSON
    /*! Configure using a JSON file.  This method will automatically
      generate a search path using the current working directory and
      the directory containing the configuration file (filename).  The
      search path will be used to locate files that might be used to
      configure the components dynamically created. */
    bool ConfigureJSON(const std::string & filename);
    bool ConfigureJSON(const std::list<std::string> & filenames);

    /*! Configure using a Json::Value.  This method will look for the
      arrays "components" and "connections" and will then call the
      methods ConfigureComponentJSON and ConfigureConnectionJSON for
      each element found.  The path is used to locate extra
      configuration files potentially used by Configure methods for
      newly created components. */
    bool ConfigureJSON(const Json::Value & configuration, const cmnPath & configPath);

    /*! Create, configure and add component based on Json::Value.
      Fields used are "shared-library", "class-name",
      "constructor-arg" (see cdg file for each type of constructor
      arg) and "configure-parameter".  The method will test if the
      configure-parameter corresponds to a file in the configPath.  If
      it is, it will try to configure using the full path name. */
    bool ConfigureComponentJSON(const Json::Value & componentConfiguration, const cmnPath & configPath);

    /*! Connect two components based on Json::Value.  Fields used are
      "required": { "component", interface" } and "provided": {
      "component", "interface"}. */
    bool ConfigureConnectionJSON(const Json::Value & connectionConfiguration);

    /*! Create with a constructor argument serialized in JSON.  If
      sharedLibrary is an empty string, the method will not attempt to
      load the dynamic library.  sharedLibrary can be a full file name
      or just the library name (without OS prefixes (e.g. on Linux,
      you should provide "MyLib" to load libMyLib.so).  If the library
      is not found, the method will also search using
      LD_LIBRARY_PATH. */
    mtsComponent * CreateComponentDynamicallyJSON(const std::string & sharedLibrary,
                                                  const std::string & className,
                                                  const std::string & constructorArgSerialized);
#endif

    /*! \brief Add a component to this local component manager.
        \param component Component instance to be added */
    bool AddComponent(mtsComponent * component);
    bool CISST_DEPRECATED AddTask(mtsTask * component); // For backward compatibility
    bool CISST_DEPRECATED AddDevice(mtsComponent * component); // For backward compatibility

    /*! \brief Remove component from this local component manager. */
    bool RemoveComponent(mtsComponent * component);
    bool RemoveComponent(const std::string & componentName);

    /*! \brief Remove all user components from this local component manager.
        \returns the number of user components removed. */
    size_t RemoveAllUserComponents(void);

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

    /*! Call CreateAll method followed by WaitForStateAll. */
    bool CreateAllAndWait(double timeoutInSeconds);

    /*! \brief Start all components. If a component is of type mtsTask,
      mtsTask::Start() is called internally. */
    void StartAll(void);

    /*! Call StartAll method followed by WaitForStateAll. */
    bool StartAllAndWait(double timeoutInSeconds);

    /*! \brief Stop all components. If a component is of type mtsTask,
      mtsTask::Kill() is called internally. */
    void KillAll(void);

    /*! Call KillAll method followed by WaitForStateAll. */
    bool KillAllAndWait(double timeoutInSeconds);

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

    /*! Disconnect two interfaces */
    bool Disconnect(const ConnectionIDType connectionID);

    bool Disconnect(const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
                    const std::string & serverComponentName, const std::string & serverInterfaceProvidedName);

    bool Disconnect(const std::string & clientProcessName, const std::string & clientComponentName,
                    const std::string & clientInterfaceRequiredName,
                    const std::string & serverProcessName, const std::string & serverComponentName,
                    const std::string & serverInterfaceProvidedName);

    //-------------------------------------------------------------------------
    //  Getters and Utilities
    //-------------------------------------------------------------------------
    /*! Default name of local component manager */
    static const std::string ProcessNameOfLCMDefault;

    /*! Name of local component manager running with the global component manager */
    static const std::string ProcessNameOfLCMWithGCM;

    /*! Get a singleton object of local component manager.
        \note  If this is called first, the local component manager is
               configured in standalone mode. If one of the other GetInstance
               methods (with arguments) is later called, the singleton object is
               reconfigured in networked mode if (CISST_MTS_HAS_ICE is defined)
               to support inter-process communication. During
               this reconfiguration process, a caller thread is blocked for
               thread-safe transition of all internal data.
    */
    static mtsManagerLocal * GetInstance(void);

    /*! \brief Return singleton object of local component manager (networked mode)
        \param globalComponentManagerIP Ip address of global component manager
               that this local component manager connects to
        \param thisProcessName Name of this process. If not specified, set as
               ip address of this host by default
        \param thisProcessIP IP address of this process. If not specified, set
               as the first ip address detected
        \return Pointer to singleton object
    */
    static mtsManagerLocal * GetInstance(const std::string & globalComponentManagerIP,
                                         const std::string & thisProcessName = "",
                                         const std::string & thisProcessIP = "");

    /*! \brief Return singleton object of local component manager (networked mode, as GCM)
        \param globalComponentManager reference to global component manager (GCM)
    */
    static mtsManagerLocal * GetInstance(mtsManagerGlobal & globalComponentManager);

    /*! Enumerate all the names of components added */
    std::vector<std::string> GetNamesOfComponents(void) const;
    void GetNamesOfComponents(std::vector<std::string>& namesOfComponents) const;

    /*! Return a reference to the time server. */
#if 0
    inline const osaTimeServer & GetTimeServer(void) const {
        return TimeServer;
    }
#endif
    const osaTimeServer & GetTimeServer(void) const;

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

    /*! Set main thread id based on the current thread. In most situations, it is not
        necessary to call this function because the main thread id is initialized
        in GetInstance. */
    void SetMainThreadId(void) { MainThreadId = osaGetCurrentThreadId(); }

    /*! Set main thread id based on the passed parameter. In most situations, it is not
        necessary to call this function because the main thread id is initialized
        in GetInstance. */
    void SetMainThreadId(const osaThreadId &threadId) { MainThreadId = threadId; }

    /*! Return main thread id. */
    osaThreadId GetMainThreadId(void) const { return MainThreadId; }

    /*! Set active task that has main thread (called by mtsTaskContinuous::Start) */
    void PushCurrentMainTask(mtsTaskContinuous *cur);

    /*! Restore previous active task that has main thread (called when task is exiting) */
    mtsTaskContinuous *PopCurrentMainTask();

    /*! Get pointer to active task that has main thread (if none, returns 0) */
    mtsTaskContinuous *GetCurrentMainTask(void) const { return CurrentMainTask; }

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

    /*! Return IP address of this process */
    inline const std::string & GetIPAddress(void) const { return ProcessIP; }

    /*! Return a list of all IP addresses detected on this machine. */
    static std::vector<std::string> GetIPAddressList(void);
    static void GetIPAddressList(std::vector<std::string> & ipAddresses);

    /*! Returns name of this local component manager (for mtsProxyBaseCommon.h) */
    inline const std::string GetName(void) const {
        return GetProcessName();
    }

    /*! Set endpoint access information */
    bool SetInterfaceProvidedProxyAccessInfo(const ConnectionIDType connectionID, const std::string & endpointInfo);

    //returns the list or processes in the system and their absolute time differences relative to GCM
    bool GetGCMProcTimeSyncInfo(std::vector<std::string> &processNames, std::vector<double> &timeOffsets);

    /*! For debugging. Dumps to stream the maps maintained by the manager. */
    void /*CISST_DEPRECATED*/ ToStream(std::ostream & outputStream) const;

    /*! Create a dot file to be used by graphviz to generate a nice
      graph of connections between tasks/interfaces. */
    void /*CISST_DEPRECATED*/ ToStreamDot(std::ostream & outputStream) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsManagerLocal)

#endif // _mtsManagerLocal_h
