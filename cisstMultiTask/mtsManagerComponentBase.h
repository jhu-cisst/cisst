/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet, Min Yang Jung
  Created on: 2010-08-29

  (C) Copyright 2010-2013 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsManagerComponentBase_h
#define _mtsManagerComponentBase_h

#include <cisstMultiTask/mtsTaskFromSignal.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsParameterTypes.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

/*!
  \file mtsManagerComponentBase.h
  \brief Declaration of Base Class for Manager Components
  \ingroup cisstMultiTask

  In the networked configuration, the communication between the global component
  manager (GCM) and local component managers (LCMs) is currently done by SLICE.
  If the cisstMultiTask's command pattern is used instead, users can have more
  flexible way to interact with the system, such as creating and starting
  components dynamically.  To replace SLICE with the cisstMultiTask's command
  pattern, we introduce a special type of component, manager component.

  There are two different typs of it, manager component server and client. The
  manager component server (MCS) runs in LCM that runs with GCM in the same
  process and only one instance of MCS exists in the whole system.  On the
  contrary, each LCM has manager component client (MCC) that connects to MCS
  and thus more than one MCC can exist in a system.  However, LCM can have only
  one MCC, i.e., one MCC per LCM.

  The cisstMultiTask's command pattern is based on a pair of interfaces that
  are connected to each other.  The following diagram shows how user and internal
  interfaces are defined and how such interfaces are connected to each other.

  (INTFC = one provided interface + one required interface)

              GCM - LCM - MCS (of type mtsManagerComponentServer)
                           |
                         INTFC ("InterfaceGCM")

                           :
                           :

                         INTFC ("InterfaceLCM")
                           |
                    LCM - MCC (of type mtsManagerComponentClient)
                           |
                         INTFC ("InterfaceComponent")

                           :
                           :

                         INTFC ("InterfaceInternal")
                           |
                     User Component
                with internal interfaces

  The cisstMultiTask has four different set of internal connections between
  components.

  1) InterfaceInternal.Required - InterfaceComponent.Provided
     : Established whenever mtsManagerLocal::AddComponent() gets called

  2) InterfaceLCM.Required - InterfaceGCM.Provided
     : Established when a singleton of mtsManagerLocal gets instantiated
       (See mtsManagerLocal's constructor)

  3) InterfaceGCM.Required - InterfaceLCM.Provided
     : When MCC connects to MCS
       (See mtsManagerComponentServer::AddNewClientProcess())

  4) InterfaceComponent.Required - InterfaceInternal.Provided
     : When user component with internal interfaces connects to MCC
       (See mtsManagerComponentClient::AddNewClientComponent())

  \note Related classes: mtsManagerLocalInterface, mtsManagerGlobalInterface,
  mtsManagerGlobal, mtsManagerProxyServer
*/

class CISST_EXPORT mtsManagerComponentBase : public mtsTaskFromSignal
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    /*! Component name definitions */
    class CISST_EXPORT ComponentNames {
    public:
        /*! Name of manager component server.  Should be globally unique */
        const static std::string ManagerComponentServer;
        const static std::string ManagerComponentClientSuffix;
    };

    /*! Interface name definitions */
    class CISST_EXPORT InterfaceNames {
    public:
        // Internal interfaces for dynamic component services
        const static std::string InterfaceInternalProvided;
        const static std::string InterfaceInternalRequired;
        const static std::string InterfaceComponentProvided;
        const static std::string InterfaceComponentRequired;
        const static std::string InterfaceLCMProvided;
        const static std::string InterfaceLCMRequired;
        const static std::string InterfaceGCMProvided;
        const static std::string InterfaceGCMRequired;
        // Interface for system-wide thread-safe logging
        const static std::string InterfaceSystemLoggerProvided;
        const static std::string InterfaceSystemLoggerRequired;
        // Interfaces for sharing execution threads between tasks
        const static std::string InterfaceExecIn;
        const static std::string InterfaceExecOut;
    };

    /*! Command name definitions */
    class CISST_EXPORT CommandNames {
    public:
        // Dynamic component management
        const static std::string ComponentCreate;
        const static std::string ComponentConfigure;
        const static std::string ComponentConnect;
        const static std::string ComponentDisconnect;
        const static std::string ComponentStart;
        const static std::string ComponentStop;
        const static std::string ComponentResume;
        const static std::string ComponentGetState;
        const static std::string LoadLibrary;  // dynamic loading
        // Logging
        const static std::string PrintLog;
        const static std::string SetLogForwarding;
        const static std::string GetLogForwardingStateInternal;
        const static std::string EnableLogForwarding;
        const static std::string DisableLogForwarding;
        const static std::string GetLogForwardingState;
        const static std::string GetLogForwardingStates;
        // Getters
        const static std::string GetNamesOfProcesses;
        const static std::string GetNamesOfComponents;
        const static std::string GetNamesOfInterfaces;
        const static std::string GetListOfConnections;
        const static std::string GetListOfComponentClasses;
        const static std::string GetInterfaceProvidedDescription;
        const static std::string GetInterfaceRequiredDescription;
        // Get absolute time for each process
        const static std::string GetAbsoluteTimeInSeconds;
        // Get absolute time differences between each process and GCM
        const static std::string GetAbsoluteTimeDiffs;
        // Establishing connections
        const static std::string GetEndUserInterface;
        const static std::string AddObserverList;
        const static std::string RemoveEndUserInterface;
        const static std::string RemoveObserverList;
    };

    /*! Event name definitions */
    class CISST_EXPORT EventNames {
    public:
        // Events
        const static std::string AddComponent;
        const static std::string AddConnection;
        const static std::string RemoveConnection;
        const static std::string ChangeState;
        const static std::string MCSReady;
        const static std::string PrintLog;
    };

    mtsManagerComponentBase(const std::string & componentName);
    virtual ~mtsManagerComponentBase();

    static bool IsManagerComponent(const std::string & componentName);
    static bool IsNameOfInternalInterface(const std::string & interfaceName);

    static bool IsManagerComponentServer(const std::string & componentName);
    static bool IsManagerComponentClient(const std::string & componentName);

    static bool IsNameOfInterfaceGCMRequired(const std::string & interfaceName);
    static bool IsNameOfInterfaceGCMProvided(const std::string & interfaceName);
    static bool IsNameOfInterfaceLCMRequired(const std::string & interfaceName);
    static bool IsNameOfInterfaceLCMProvided(const std::string & interfaceName);
    static bool IsNameOfInterfaceComponentRequired(const std::string & interfaceName);
    static bool IsNameOfInterfaceComponentProvided(const std::string & interfaceName);
    static bool IsNameOfInterfaceInternalRequired(const std::string & interfaceName);
    static bool IsNameOfInterfaceInternalProvided(const std::string & interfaceName);

    static const std::string GetNameOfManagerComponentServer(void);
    static const std::string GetNameOfManagerComponentClientFor(const std::string & processName);
    static const std::string GetNameOfInterfaceGCMRequiredFor(const std::string & processName);
    static const std::string GetNameOfInterfaceGCMProvided(void);
    static const std::string GetNameOfInterfaceLCMRequired(void);
    static const std::string GetNameOfInterfaceLCMProvided(void);
    static const std::string GetNameOfInterfaceComponentRequiredFor(const std::string & componentName);
    static const std::string GetNameOfInterfaceComponentProvided(void);
    static const std::string GetNameOfInterfaceInternalRequired(void);
    static const std::string GetNameOfInterfaceInternalProvided(void);

    // mtsTask implementation
    virtual void Startup(void) = 0;
    virtual void Run(void);
    virtual void Cleanup(void);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsManagerComponentBase);

#endif // _mtsManagerComponentBase_h
