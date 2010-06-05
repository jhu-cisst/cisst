/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung
  Created on: 2009-12-07

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
  \brief Declaration of Local Component Manager
  \ingroup cisstMultiTask

  This class defines the local component manager (LCM) that manages local
  components and is unique in a process.  Since only one instance of LCM should
  exist in a process, this class is implemented as singleton.  Thus, to get an 
  instance of LCM, mtsManagerLocal::GetInstance() is to be called (instead of
  constructor).

  The LCM replaces the previous task manager (mtsTaskManager) which was similar
  to the LCM without networking support.  Major differences between the two are:

  1) The LCM manages tasks and devices as a unified object--component--which
  is of type mtsDevice and is stored in a single data structure--component map.
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
#include <cisstOSAbstraction/osaThreadBuddy.h>
#include <cisstOSAbstraction/osaTimeServer.h>
#include <cisstOSAbstraction/osaMutex.h>
#include <cisstMultiTask/mtsManagerLocalInterface.h>
#include <cisstMultiTask/mtsManagerGlobalInterface.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>

#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsManagerLocal: public mtsManagerLocalInterface
{
    friend class mtsManagerLocalTest;
    friend class mtsManagerGlobalTest;
    friend class mtsManagerGlobal;

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

private:
    /*! Singleton object */
    static mtsManagerLocal * Instance;

    /*! Flag for unit tests. Enabled only for unit tests (false by default) */
    static bool UnitTestEnabled;

    /*! Flag that allows unit tests to skip network-related processings such as 
        network proxy creation/setup or remote connection (false by default) */
    static bool UnitTestNetworkProxyEnabled;

protected:
    /*! Typedef for component map: key is component name, value is component
        object */
    typedef cmnNamedMap<mtsDevice> ComponentMapType;
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

    /*! Protected constructor (singleton) */
    mtsManagerLocal(void);

#if CISST_MTS_HAS_ICE
    mtsManagerLocal(const std::string & globalComponentManagerIP,
                    const std::string & thisProcessName,
                    const std::string & thisProcessIP);
#endif

    /*! Destructor. Includes OS-specific cleanup. */
    virtual ~mtsManagerLocal();

    /*! Initialization */
    void Initialize(void);

#if CISST_MTS_HAS_ICE
    /*! \brief Set IP address of this machine */
    void SetIPAddress(void);

    /*! \brief Create proxy objects and enable network support
        \return True if success, false otherwise */    
    bool CreateProxy(void);
#endif

    /*! \brief Register all interfaces that a component owns to the global 
               component manager.  The GCM uses this information to connect
               interfaces that are in different processes.
        \param component Component object instance
        \param componentName Name of component */
    bool RegisterInterfaces(mtsDevice * component);
    bool RegisterInterfaces(const std::string & componentName);

    /*! \brief Connect two local interfaces. 
        \param clientComponentName Name of client component
        \param clientRequiredInterfaceName Name of required interface
        \param serverComponentName Name of server component
        \param serverProvidedInterfaceName Name of provided interface
        \param userId User id allocated for this connection. Valid only in the
               networked configuration. Zero by default.
        \return zero if successful, -1 if error occurs.
        \note  It is assumed that two components are in the same process. Thus,
               this does not work with components in different processes. */
    int ConnectLocally(
        const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverComponentName, const std::string & serverProvidedInterfaceName,
        const int userId = 0);

    //-------------------------------------------------------------------------
    //  Methods required by mtsManagerLocalInterface
    //
    //  See mtsManagerLocalInterface.h for detailed documentation.
    //-------------------------------------------------------------------------
public:
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
        \param serverComponentProxyName Name of server component proxy that 
               has provided interface to be duplicated
        \param providedInterfaceDescription Full description of provided
               interface to be duplicated
        \param listenerID Not used */
    bool CreateProvidedInterfaceProxy(
        const std::string & serverComponentProxyName,
        const ProvidedInterfaceDescription & providedInterfaceDescription, const std::string & listenerID = "");

    /* AAAAAAAAAAAAAAAAAAAAAAA */
    /*! \brief Create required interface proxy 
        \param clientComponentProxyName Name of component proxy that has */
    bool CreateRequiredInterfaceProxy(
        const std::string & clientComponentProxyName,
        const RequiredInterfaceDescription & requiredInterfaceDescription, const std::string & listenerID = "");

    /*! Remove a provided interface proxy */
    bool RemoveProvidedInterfaceProxy(
        const std::string & clientComponentProxyName, const std::string & providedInterfaceProxyName, const std::string & listenerID = "");

    /*! Remove a required interface proxy */
    bool RemoveRequiredInterfaceProxy(
        const std::string & serverComponentProxyName, const std::string & requiredInterfaceProxyName, const std::string & listenerID = "");

    /*! Get information about provided interface */
    bool GetProvidedInterfaceDescription(
        const unsigned int userId,
        const std::string & serverComponentName,
        const std::string & providedInterfaceName,
        ProvidedInterfaceDescription & providedInterfaceDescription, const std::string & listenerID = "");

    /*! Extract all the information on a required interface such as function
        objects and event handlers with arguments serialized */
    bool GetRequiredInterfaceDescription(
        const std::string & componentName,
        const std::string & requiredInterfaceName,
        RequiredInterfaceDescription & requiredInterfaceDescription, const std::string & listenerID = "");

    /*! Returns a total number of interfaces that are running on a component */
    int GetCurrentInterfaceCount(const std::string & componentName, const std::string & listenerID = "");

    /*! Connect interfaces at server side */
    bool ConnectServerSideInterface(
        const int userId, const unsigned int providedInterfaceProxyInstanceID,
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName,
        const std::string & listenerID = "");

    /*! Connect two local interfaces at client side. */
    bool ConnectClientSideInterface(const unsigned int connectionID,
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName,
        const std::string & listenerID = "");

    /*! Pre-allocate provided interface's resources */
    int PreAllocateResources(const std::string & userName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName, 
        const std::string & listenerID = "");
#endif

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

#endif

    //-------------------------------------------------------------------------
    //  Component Management
    //-------------------------------------------------------------------------
    /*! Add a component to this local component manager. */
    bool AddComponent(mtsDevice * component);
    bool /*CISST_DEPRECATED*/ AddTask(mtsTask * component); // For backward compatibility
    bool /*CISST_DEPRECATED*/ AddDevice(mtsDevice * component); // For backward compatibility

    /*! Remove a component from this local component manager. */
    bool RemoveComponent(mtsDevice * component);
    bool RemoveComponent(const std::string & componentName);

    /*! Retrieve a component by name. */
    mtsDevice * GetComponent(const std::string & componentName) const;
    mtsTask * GetComponentAsTask(const std::string & componentName) const;

    mtsDevice /*CISST_DEPRECATED*/ * GetDevice(const std::string & deviceName); // For backward compatibility
    mtsTask /*CISST_DEPRECATED*/ * GetTask(const std::string & taskName); // For backward compatibility

    /*! Check if a component exists by its name */
    bool FindComponent(const std::string & componentName) const;

    /* \brief Connect two local interfaces
       \param clientComponentName Name of client component
       \param clientRequiredInterfaceName Name of required interface
       \param serverComponentName Name of server component
       \param serverProvidedInterfaceName Name of provided interface
       \return True if success, false otherwise
       \note If connection is established successfully, this information is
             reported to the global component manager (the local component 
             manager does not keep any connection information). */
    bool Connect(
        const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverComponentName, const std::string & serverProvidedInterfaceName);

#if CISST_MTS_HAS_ICE
    /* \brief Connect two remote interfaces
       \param clientProcessName Name of client process
       \param clientComponentName Name of client component
       \param clientRequiredInterfaceName Name of required interface
       \param serverProcessName Name of server process
       \param serverComponentName Name of server component
       \param serverProvidedInterfaceName Name of provided interface
       \return True if success, false otherwise
       \note If connection is established successfully, this information is
             reported to the global component manager. Since connection between
             two interfaces should be established twice--once in the client 
             process and once in the server process--there are two internal 
             connection management methods: ConnectClientSideInterface() and
             ConnectServerSideInterface().  ConnectClientSideInterface() is 
             always executed first followed by ConnectServerSideInterface().
             Connection request can be made at any process--server or client 
             process.  That is, whichever process initiate connection request,
             the result is the same. 
             If this method is called against two local interfaces, the other 
             Connect() method is internally called instead. */
    bool Connect(
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName);
#endif

    /*! Disconnect two interfaces */
    bool Disconnect(
        const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverComponentName, const std::string & serverProvidedInterfaceName);

#if CISST_MTS_HAS_ICE
    bool Disconnect(
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName);
#endif

    /*! Create all components. If a component is of type mtsTask, mtsTask::Create()
        is called internally. */
    void CreateAll(void);

    /*! Start all components. If a component is of type mtsTask, mtsTask::Start()
        is called internally. */
    void StartAll(void);

    /*! Stop all components. If a component is of type mtsTask, mtsTask::Kill()
        is called internally. */
    void KillAll(void);

    /*! Cleanup.  Since a local component manager is a singleton, the
      destructor will be called when the program exits but a library user
      is not capable of handling the timing. Thus, for safe termination, this
      method should be called before an application quits. */
    void Cleanup(void);

    //-------------------------------------------------------------------------
    //  Getters and Utilities
    //-------------------------------------------------------------------------
    /*! Enumerate all the names of components added */
    std::vector<std::string> GetNamesOfComponents(void) const;
    std::vector<std::string> /*CISST_DEPRECATED*/ GetNamesOfDevices(void) const;  // For backward compatibility
    std::vector<std::string> /*CISST_DEPRECATED*/ GetNamesOfTasks(void) const;  // For backward compatibility

    void GetNamesOfComponents(std::vector<std::string>& namesOfComponents) const;
    void /*CISST_DEPRECATED*/ GetNamesOfDevices(std::vector<std::string>& namesOfDevices) const; // For backward compatibility
    void /*CISST_DEPRECATED*/ GetNamesOfTasks(std::vector<std::string>& namesOfTasks) const; // For backward compatibility

    /*! Return a reference to the time server. */
    inline const osaTimeServer & GetTimeServer(void) {
        return TimeServer;
    }

    /*! Returns name of this local component manager */
    inline const std::string GetProcessName(const std::string & CMN_UNUSED(listenerID) = "") {
        return ProcessName;
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
    inline std::string GetIPAddress() const { return ProcessIP; }

    /*! Return a list of all IP addresses detected on this machine. */
    static std::vector<std::string> GetIPAddressList(void);
    static void GetIPAddressList(std::vector<std::string> & ipAddresses);

    /*! Returns name of this local component manager (for mtsProxyBaseCommon.h) */
    inline const std::string GetName() {
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
    /*! Check if a component is a proxy object based on its name */
    static bool IsProxyComponent(const std::string & componentName) {
        const std::string proxyStr = "Proxy";
        size_t found = componentName.find(proxyStr);
        return found != std::string::npos;
    }

    /*! Set endpoint access information */
    bool SetProvidedInterfaceProxyAccessInfo(
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName,
        const std::string & endpointInfo);

    /*! For testing purposes */
    void DisconnectGCM();
    void ReconnectGCM();
#endif

    //-------------------------------------------------------------------------
    // Deprecated APIs
    //-------------------------------------------------------------------------
    void CISST_DEPRECATED SetServerTaskIP(const std::string) {}
    void CISST_DEPRECATED SetGlobalTaskManagerIP(const std::string) {}

    typedef enum { TASK_MANAGER_CLIENT, TASK_MANAGER_SERVER } ManagerType;
    void CISST_DEPRECATED SetTaskManagerType(ManagerType CMN_UNUSED(type)) {}
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsManagerLocal)

#endif // _mtsManagerLocal_h

