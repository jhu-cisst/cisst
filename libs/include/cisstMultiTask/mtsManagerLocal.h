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
  \brief Definition of Local Component Manager
  \ingroup cisstMultiTask

  This class defines the local component manager (LCM) which replaces the
  previous task manager (mtsTaskManager) and is implemented as a singleton.

  Major differences between the two are:

  1) The LCM manages tasks and devices as a unified object, a component, which
  is of type mtsDevice. For this, task map and device map in the task manager
  has been consolidated into a single data structure, component map.

  2) The LCM does not keep the connection information; All connection information
  are now maintained and managed by the global component manager (GCM).

  \note Related classes: mtsManagerLocalInterface, mtsManagerGlobalInterface, mtsManagerGlobal
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

    /*! Flag for unit tests. Enabled only for unit tests and set as false by default */
    static bool UnitTestEnabled;

    /*! Flag to skip network-related processing such as network proxy creation,
        network proxy startup, remote connection, and so on. Set as false by default */
    static bool UnitTestNetworkProxyEnabled;

protected:
    /*! Typedef for component map: (component name, component object)
        component object is a pointer to mtsDevice object. */
    typedef cmnNamedMap<mtsDevice> ComponentMapType;
    ComponentMapType ComponentMap;

    /*! Time server used by all tasks. */
    osaTimeServer TimeServer;

    /*! Process name of this local component manager. Should be globally unique
        across a system. */
    std::string ProcessName;

    /*! IP address of the global component manager */
    const std::string GlobalComponentManagerIP;

    /*! IP address of this machine. Set internally by SetIPAddress(). */
    std::string ProcessIP;

    /*! A list of all IP addresses detected on this machine */
    std::vector<std::string> ProcessIPList;

    /*! Mutex to use ComponentMap safely */
    osaMutex ComponentMapChange;

    /*! A pointer to the global component manager.
        Depending on configurations, this points to two different objects:
        - In standalone mode, this is an instance of the GCM (of type
          mtsManagerGlobal) running in the same process.
        - In network mode, this is a pointer to a proxy object for the GCM
          (of type mtsManagerGlobalProxyClient) that links this LCM with the
          GCM. In this case, the GCM normally runs in a different process. */
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
    /*! Set an IP address of this machine */
    void SetIPAddress(void);
#endif

    /*! Connect two local interfaces. This method assumes two interfaces are in
        the same process.

        Returns: provided interface proxy instance id, if remote connection
                 zero, if local connection
                 -1,   if error occurs */
    int ConnectLocally(
        const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverComponentName, const std::string & serverProvidedInterfaceName);

    //-------------------------------------------------------------------------
    //  Methods required by mtsManagerLocalInterface
    //-------------------------------------------------------------------------
public:
#if CISST_MTS_HAS_ICE
    /*! Create a component proxy. This should be called before an interface
        proxy is created. */
    bool CreateComponentProxy(const std::string & componentProxyName, const std::string & listenerID = "");

    /*! Remove a component proxy. Note that all the interface proxies that the
        proxy manages is automatically removed when removing a component proxy. */
    bool RemoveComponentProxy(const std::string & componentProxyName, const std::string & listenerID = "");

    /*! Create a provided interface proxy using ProvidedInterfaceDescription */
    bool CreateProvidedInterfaceProxy(
        const std::string & serverComponentProxyName,
        const ProvidedInterfaceDescription & providedInterfaceDescription, const std::string & listenerID = "");

    /*! Create a required interface proxy using RequiredInterfaceDescription */
    bool CreateRequiredInterfaceProxy(
        const std::string & clientComponentProxyName,
        const RequiredInterfaceDescription & requiredInterfaceDescription, const std::string & listenerID = "");

    /*! Remove a provided interface proxy */
    bool RemoveProvidedInterfaceProxy(
        const std::string & clientComponentProxyName, const std::string & providedInterfaceProxyName, const std::string & listenerID = "");

    /*! Remove a required interface proxy */
    bool RemoveRequiredInterfaceProxy(
        const std::string & serverComponentProxyName, const std::string & requiredInterfaceProxyName, const std::string & listenerID = "");

    /*! Extract all the information on a provided interface such as command
        objects and events with arguments serialized */
    bool GetProvidedInterfaceDescription(
        const std::string & componentName,
        const std::string & providedInterfaceName,
        ProvidedInterfaceDescription & providedInterfaceDescription, const std::string & listenerID = "");

    /*! Extract all the information on a required interface such as function
        objects and event handlers with arguments serialized */
    bool GetRequiredInterfaceDescription(
        const std::string & componentName,
        const std::string & requiredInterfaceName,
        RequiredInterfaceDescription & requiredInterfaceDescription, const std::string & listenerID = "");

    /*! Returns a total number of interfaces that are running on a component */
    const int GetCurrentInterfaceCount(const std::string & componentName, const std::string & listenerID = "");

    /*! Connect interfaces at server side */
    bool ConnectServerSideInterface(const unsigned int providedInterfaceProxyInstanceID,
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName, const std::string & listenerID = "");

    /*! Connect interfaces at client side */
    bool ConnectClientSideInterface(const unsigned int connectionID,
        const std::string & clientProcessName, const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverProcessName, const std::string & serverComponentName, const std::string & serverProvidedInterfaceName, const std::string & listenerID = "");
#endif

#if !CISST_MTS_HAS_ICE
    /*! Get an instance of local component manager */
    static mtsManagerLocal * GetInstance(void);
#else
    /*! Get an instance of local component manager.
        If process ip is not specified, the first ip address detected is used.
        If process name is not given, the ip address is used instead. */
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

    /* Connect two interfaces */
    bool Connect(
        const std::string & clientComponentName, const std::string & clientRequiredInterfaceName,
        const std::string & serverComponentName, const std::string & serverProvidedInterfaceName);

#if CISST_MTS_HAS_ICE
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

