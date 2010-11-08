/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsManagerComponentBase.h 1726 2010-08-30 05:07:54Z mjung5 $

  Author(s):  Anton Deguet, Min Yang Jung
  Created on: 2010-08-29

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

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

class mtsManagerComponentBase : public mtsTaskFromSignal
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    /*! Component name definitions */
    class ComponentNames {
    public:
        /*! Name of manager component server.  Should be globally unique */
        const static std::string ManagerComponentServer;
        const static std::string ManagerComponentClientSuffix;
    };

    /*! Interface name definitions */
    class InterfaceNames {
    public:
        const static std::string InterfaceInternalProvided;
        const static std::string InterfaceInternalRequired;
        const static std::string InterfaceComponentProvided;
        const static std::string InterfaceComponentRequired;
        const static std::string InterfaceLCMProvided;
        const static std::string InterfaceLCMRequired;
        const static std::string InterfaceGCMProvided;
        const static std::string InterfaceGCMRequired;
    };

    /*! Command name definitions */
    class CommandNames {
    public:
        // Dynamic component management
        const static std::string ComponentCreate;
        const static std::string ComponentConnect;
        const static std::string ComponentStart;
        const static std::string ComponentStop;
        const static std::string ComponentResume;
        // Getters
        const static std::string GetNamesOfProcesses;
        const static std::string GetNamesOfComponents;
        const static std::string GetNamesOfInterfaces;
        const static std::string GetListOfConnections;
    };

    /*! Event name definitions */
    class EventNames {
    public:
        // Events
        const static std::string AddComponent;
        const static std::string AddConnection;
        const static std::string ChangeState;
    };

    mtsManagerComponentBase(const std::string & componentName);
    virtual ~mtsManagerComponentBase();

    static bool IsManagerComponentServer(const std::string & componentName);
    static bool IsManagerComponentClient(const std::string & componentName);

    // mtsTask implementation
    virtual void Startup(void) = 0;
    virtual void Run(void);
    virtual void Cleanup(void);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsManagerComponentBase);

#endif // _mtsManagerComponentBase_h
