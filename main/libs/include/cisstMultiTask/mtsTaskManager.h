/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsTaskManager.h 794 2009-09-01 21:43:56Z pkazanz1 $

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Define the task manager
*/


#ifndef _mtsTaskManager_h
#define _mtsTaskManager_h

#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassRegister.h>
#include <cisstCommon/cmnAssert.h>
#include <cisstCommon/cmnNamedMap.h>

#include <cisstOSAbstraction/osaThreadBuddy.h>
#include <cisstOSAbstraction/osaTimeServer.h>
#include <cisstOSAbstraction/osaSocket.h>

#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsConfig.h>

#if CISST_MTS_HAS_ICE
#include <cisstMultiTask/mtsProxyBaseCommon.h>
#include <cisstMultiTask/mtsDeviceInterfaceProxy.h>
#endif // CISST_MTS_HAS_ICE

#include <set>

#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  The Task Manager is used to manage the connections between tasks
  and devices.  It is a Singleton object.
*/
class CISST_EXPORT mtsTaskManager: public cmnGenericObject {

    friend class mtsTaskManagerTest;

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

    /*! Typedef for task name and pointer map. */
    typedef cmnNamedMap<mtsTask> TaskMapType;

    /*! Typedef for device name and pointer map. */
    typedef cmnNamedMap<mtsDevice> DeviceMapType;

    /*! Typedef for user task, composed of task name and "output port"
        name. */
    typedef std::pair<std::string, std::string> UserType;
    /*! Typedef for resource device or task, composed of device or
        task name and interface name. */ 
    typedef std::pair<std::string, std::string> ResourceType;
    /*! Typedef for associations between users and resources. */
    typedef std::pair<UserType, ResourceType> AssociationType;
    typedef std::set<AssociationType> AssociationSetType;

public:

    // Default mailbox size -- perhaps this should be specified elsewhere
    enum { MAILBOX_DEFAULT_SIZE = 16 };

#ifdef CISST_MTS_HAS_ICE
    /*! Typedef for task manager type. */
    typedef enum {
        TASK_MANAGER_LOCAL,
        TASK_MANAGER_SERVER, // global task manager
        TASK_MANAGER_CLIENT  // general task manager
    } TaskManagerType;
#endif

protected:

    /*! Mapping of task name (key) and pointer to mtsTask object. */
    TaskMapType TaskMap;

    /*! Mapping of interface name (key) and pointer to mtsInterface
      object. */
    DeviceMapType DeviceMap;

    /*! Mapping of task name (key) and associated interface name. */
    AssociationSetType AssociationSet;

    /*! Time server used by all tasks. */
    osaTimeServer TimeServer;

    osaSocket JGraphSocket;
    bool JGraphSocketConnected;

    /*! Constructor.  Protected because this is a singleton.
        Does OS-specific initialization to start real-time operations. */
    mtsTaskManager(void);
    
    /*! Destructor.  Does OS-specific cleanup. */
    virtual ~mtsTaskManager();

 public:
    /*! Create the static instance of this class. */
    static mtsTaskManager * GetInstance(void) ;

    /*! Return a reference to the time server. */
    inline const osaTimeServer & GetTimeServer(void) {
        return TimeServer;
    }

    /*! Put a task under the control of the Manager. */
    bool AddTask(mtsTask * task);

    /*! Pull out a task from the Manager. */
    bool RemoveTask(mtsTask * task);

    /*! Put a device under the control of the Manager. */
    bool AddDevice(mtsDevice * device);

    /*! Put a task or device under the control of the Manager
      (calls AddTask or AddDevice based on dynamic type of parameter). */
    bool Add(mtsDevice * device);

    /*! List all devices already added */
    std::vector<std::string> GetNamesOfDevices(void) const;

    /*! List all tasks already added */
    std::vector<std::string> GetNamesOfTasks(void) const;
    
    /*! Fetch all tasks already added. (overloaded) */
    void GetNamesOfTasks(std::vector<std::string>& taskNameContainer) const;

    /*! Retrieve a device by name.  Return 0 if the device is not
        known. */
    mtsDevice * GetDevice(const std::string & deviceName);

    /*! Retrieve a task by name.  Return 0 if the task is not
        known. */
    mtsTask * GetTask(const std::string & taskName);

    /*! Connect the required interface of a user task to the provided
      interface of a resource task (or device).
    */
    bool Connect(const std::string & userTaskName, const std::string & requiredInterfaceName,
                 const std::string & resourceTaskName, const std::string & providedInterfaceName);

    /*! Disconnect the required interface of a user task to the provided
      interface of a resource task (or device).
    */
    bool Disconnect(const std::string & userTaskName, const std::string & requiredInterfaceName,
                    const std::string & resourceTaskName, const std::string & providedInterfaceName);
    
    /*! Create all tasks, i.e. create the threads for each already
        added task.  This method will call the mtsTask::Create method
        for each task. */
    void CreateAll(void);

    /*! Start all tasks.  This method will call the mtsTask::Start method for each task.
        If a task will use the current thread, it is called last because its Start method
        will not return until the task is terminated. There should be no more than one task
        using the current thread. */
    void StartAll(void);

    /*! Stop all tasks.  This method will call the mtsTask::Kill method for each task. */
    void KillAll(void);

    /*! Cleanup.  Since the task manager is a singleton, the
      destructor will be called when the program exits but the
      user/programmer will not be able to control when exactly.  If
      the cleanup requires some objects to still be instantiated (log
      files, ...), this might lead to crashes.  To avoid this, the
      Cleanup method should be called before the program quits. */
    void Cleanup(void);

    /*! For debugging. Dumps to stream the maps maintained by the manager. */
    void ToStream(std::ostream & outputStream) const;

    /*! Create a dot file to be used by graphviz to generate a nice
      graph of connections between tasks/interfaces. */
    void ToStreamDot(std::ostream & outputStream) const;
    
    inline void Kill(void) {
        __os_exit();
    }

#if CISST_MTS_HAS_ICE
    //-------------------------------------------------------------------------
    //  Proxy-related
    //-------------------------------------------------------------------------
protected:
    /*! Task manager type. */
    TaskManagerType TaskManagerTypeMember;

    /*! Task manager communicator ID. Used as one of ICE proxy object properties. */
    const std::string TaskManagerCommunicatorID;

    /*! Task manager proxy objects. Both are initialized as null at first and 
      will be assigned later. Either one of the objects should be null and the 
      other has to be valid.
      ProxyServer is valid iff this is the global task manager.
      ProxyClient is valid iff this is a general task manager.
    */
    mtsTaskManagerProxyServer * ProxyGlobalTaskManager;
    mtsTaskManagerProxyClient * ProxyTaskManagerClient;

    /*! IP address information. */
    std::string GlobalTaskManagerIP;
    std::string ServerTaskIP;

    /*! Start two kinds of proxies.
      Task Manager Layer: Start either GlobalTaskManagerProxy of TaskManagerClientProxy
      according to the type of this task manager.
      Task Layer: While iterating all tasks, create and start all provided interface 
      proxies (see mtsTask::RunProvidedInterfaceProxy()).
    */
    void StartProxies();

public:
    /*! Set the type of task manager-global task manager (server) or conventional
      task manager (client)-and start an appropriate task manager proxy.
      Also start a task interface proxy. */
    void SetTaskManagerType(const TaskManagerType taskManagerType) {
        TaskManagerTypeMember = taskManagerType;
        StartProxies();
    }

    /*! Getter */
    inline TaskManagerType GetTaskManagerType() { return TaskManagerTypeMember; }

    inline mtsTaskManagerProxyServer * GetProxyGlobalTaskManager() const {
        return ProxyGlobalTaskManager;
    }

    inline mtsTaskManagerProxyClient * GetProxyTaskManagerClient() const {
        return ProxyTaskManagerClient;
    }

    /*! Setter */
    inline void SetGlobalTaskManagerIP(const std::string & globalTaskManagerIP) {
        GlobalTaskManagerIP = globalTaskManagerIP;
    }

    inline void SetServerTaskIP(const std::string & serverTaskIP) {
        ServerTaskIP = serverTaskIP;
    }
#endif // CISST_MTS_HAS_ICE
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsTaskManager)

#endif // _mtsTaskManager_h

