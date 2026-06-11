/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Min Yang Jung
  Created on: 2009-12-07

  (C) Copyright 2009-2026 Johns Hopkins University (JHU), All Rights Reserved.

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
  exist in a process, this class is implemented as a singleton.  To get an
  instance of LCM, therefore, mtsManagerLocal::GetInstance() should be used
  (instead of constructor).
*/

#ifndef _mtsManagerLocal_h
#define _mtsManagerLocal_h

#include <string>
#include <set>
#include <stack>

#include <cisstCommon/cmnUnits.h>
#include <cisstCommon/cmnPath.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsComponentState.h>
#include <cisstMultiTask/mtsComponent.h>

#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsManagerLocal: public cmnGenericObject
{
    // for unit-testing
    friend class mtsManagerLocalTest;

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

private:
    /*! Valid component tags */
    std::set<std::string> ValidComponentTags;
    /*! Valid interface tags */
    std::set<std::string> ValidInterfaceTags;

    /*! Singleton object */
    static mtsManagerLocal * Instance;

    /*! Thread ID of thread that called GetInstance(). In the future,
        we will support multiple threads. */
    osaThreadId MainThreadId;

    // PK TODO:  Review MainTaskNames and CurrentMainTask

    /*! List of main tasks (in chronological order) */
    std::stack<std::string> MainTaskNames;

    /*! Pointer to task that currently has main thread (set when that task is started) */
    mtsTaskContinuous * CurrentMainTask;

    /*! Process name (for a multi-process system).
     */
    std::string ProcessName;

    /*! Name of this object
     */
    std::string Name;

    /*! Manager component instance (for direct access) */
    mtsManagerComponent *ManagerComponent;

    /*! Internal thread to buffer log messages */
    osaThread       LogThread;
    osaThreadSignal LogThreadFinished;
    bool            LogThreadFinishWaiting;
    void*           LogDispatchThread(void * arg);

protected:
    /*! Protected constructor (singleton) */
    mtsManagerLocal(void);

    /*! Destructor. Includes OS-specific cleanup. */
    virtual ~mtsManagerLocal();

    /*! Initialization */
    void Initialize(void);

    /*! \brief Create internal manager component. */
    bool CreateManagerComponent(void);

    /* Local component */
    mtsComponentWithManagement * LocalComponent;

    /*! Return the ManagerComponentServices.
        This is a method so that we can later add thread-safety.
     */
    mtsManagerComponentServices * GetManagerServices() const;

    struct LogInterface {
        mtsFunctionWrite PrintLog;
    };
    LogInterface Logger;

    /*! Return the LoggerServices.
        This is a method so that we can later add thread-safety.
     */
    const LogInterface * GetLoggerServices() const;

    /*! Get information about provided interface */
    void GetInterfaceProvidedDescription(
         const std::string & componentName,
         const std::string & interfaceName,
         mtsInterfaceProvidedDescription & interfaceProvidedDescription);

    /*! Extract all the information on a required interface such as function
        objects and event handlers with arguments serialized */
    void GetInterfaceRequiredDescription(
         const std::string & componentName,
         const std::string & interfaceName,
         mtsInterfaceRequiredDescription & interfaceRequiredDescription);

    /*! Set up system logger that allows collecting system-wide logs across network */
    void SetupSystemLogger(void);

public:

    /*! Get a singleton object of local component manager.
    */
    static mtsManagerLocal * GetInstance(void);

    /*! \brief Cleanup. Left in the public API for backwards compatibility.
               Client code should not call this method directly, and should
               instead call DeleteInstance prior to quitting. DeleteInstance
               calls this method before deleting the singleton. */
    void Cleanup(void);

    /*! \brief DeleteInstance. Since a local component manager is a singleton,
               and demand-created by the first caller of GetInstance, the
               destructor will never be called unless an application calls
               this method just prior to quitting. GetInstance should NOT
               be used again after this method is called. This method calls
               Cleanup from its implementation, so a separate call to Cleanup
               is unnecessary. */
    static void DeleteInstance(void);

    /*! Returns name of this local component manager (for mtsProxyBaseCommon.h) */
    inline const std::string CISST_DEPRECATED GetName(void) const {
        return Name;
    }
    //-------------------------------------------------------------------------
    //  Component Management
    //-------------------------------------------------------------------------
    /*! \brief Create a component.  Does not add it to the local component manager.
        In this method, componentName is only used if constructorArgSerialized is an empty string. */
    mtsComponent * CreateComponentDynamically(const std::string & className, const std::string & componentName,
                                              const std::string & constructorArgSerialized);

    /*! \brief Create a component.  Does not add it to the local component manager.
        In this method, the component name is obtained from constructorArg. */
    mtsComponent * CreateComponentDynamically(const std::string & className,
                                              const mtsGenericObject & constructorArg);

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

    /*! \brief Add a component to the component manager.
        \param component Component instance to be added */
    bool AddComponent(mtsComponent * component);
    bool CISST_DEPRECATED AddTask(mtsTask * component); // For backward compatibility
    bool CISST_DEPRECATED AddDevice(mtsComponent * component); // For backward compatibility

    /*! \brief Remove component from component manager. */
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
                 const std::string & serverInterfaceProvidedName);

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

    /*! Returns name of this local component manager */
    inline const std::string GetProcessName(void) const {
        return ProcessName;
    }

    /*! Enumerate all the names of components added */
    std::vector<std::string> GetNamesOfComponents(void) const;
    void GetNamesOfComponents(std::vector<std::string>& namesOfComponents) const;

    /*! Return a reference to the time server. */
    const osaTimeServer & GetTimeServer(void) const;

#ifndef PK_TODO  // Review following methods

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
    mtsTaskContinuous CISST_DEPRECATED *PopCurrentMainTask();

    /*! Get pointer to active task that has main thread (if none, returns 0) */
    mtsTaskContinuous CISST_DEPRECATED *GetCurrentMainTask(void) const { return CurrentMainTask; }
#endif

    /*! Get names of all commands in a provided interface */
    void GetNamesOfCommands(std::vector<std::string>& namesOfCommands,
                            const std::string & componentName,
                            const std::string & providedInterfaceName);

    /*! Get names of all event generators in a provided interface */
    void GetNamesOfEventGenerators(std::vector<std::string>& namesOfEventGenerators,
                                   const std::string & componentName,
                                   const std::string & providedInterfaceName);

    /*! Get names of all functions in a required interface */
    void GetNamesOfFunctions(std::vector<std::string>& namesOfFunctions,
                             const std::string & componentName,
                             const std::string & requiredInterfaceName);

    /*! Get names of all event handlers in a required interface */
    void GetNamesOfEventHandlers(std::vector<std::string>& namesOfEventHandlers,
                                 const std::string & componentName,
                                 const std::string & requiredInterfaceName);

    /*! Get description of a command in a provided interface */
    void GetDescriptionOfCommand(std::string & description,
                                 const std::string & componentName,
                                 const std::string & providedInterfaceName,
                                 const std::string & commandName);

    /*! Get description of a event generator in a provided interface */
    void GetDescriptionOfEventGenerator(std::string & description,
                                        const std::string & componentName,
                                        const std::string & providedInterfaceName,
                                        const std::string & eventGeneratorName);

    /*! Get description of a function in a required interface */
    void GetDescriptionOfFunction(std::string & description,
                                  const std::string & componentName,
                                  const std::string & requiredInterfaceName,
                                  const std::string & functionName);

    /*! Get description of a function in a required  interface */
    void GetDescriptionOfEventHandler(std::string & description,
                                      const std::string & componentName,
                                      const std::string & requiredInterfaceName,
                                      const std::string & eventHandlerName);

    /*! Return IP address of this process */
    inline std::string CISST_DEPRECATED GetIPAddress(void) const { return ""; }

    /*! Return a list of all IP addresses detected on this machine. */
    static std::vector<std::string> GetIPAddressList(void);
    static void GetIPAddressList(std::vector<std::string> & ipAddresses);

    /*! Check if further logs are allowed (used in mtsTask); this could be
        moved to cmnLogger. */
    static bool IsLogAllowed(void);

    /*! Is system-wide thread-safe logging enabled? */
    static bool IsLogForwardingEnabled(void);

    /*! Enable or disable system-wide thread-safe logging */
    static void SetLogForwarding(bool activate);

    /*! Get whether system-wide logging is enabled or not */
    static void GetLogForwardingState(bool & state);
    static bool GetLogForwardingState(void);

    /*! Callback function for system-wide thread-safe logging */
    static void LogDispatcher(const char * str, int len);

    // TODO: should the following be moved somewhere else?
    bool IsValidComponentTag(const std::string & tag) const;
    bool IsValidInterfaceTag(const std::string & tag) const;
    void AddValidComponentTag(const std::string & tag);
    void AddValidInterfaceTag(const std::string & tag);
    const std::set<std::string> & GetValidComponentTags(void) const;
    const std::set<std::string> & GetValidInterfaceTags(void) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsManagerLocal)

#endif // _mtsManagerLocal_h
