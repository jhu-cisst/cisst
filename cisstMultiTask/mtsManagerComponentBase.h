/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet, Min Yang Jung, Peter Kazanzides
  Created on: 2010-08-29

  (C) Copyright 2010-2026 Johns Hopkins University (JHU), All Rights Reserved.

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

  The cisstMultiTask's command pattern is based on a pair of interfaces that
  are connected to each other.  The following diagram shows how user and internal
  interfaces are defined and how such interfaces are connected to each other.

  (INTFC = one provided interface + one required interface)

                    LCM - MCS (of type mtsManagerComponent)
                           |
                         INTFC ("InterfaceComponent")

                           :
                           :

                         INTFC ("InterfaceInternal")
                           |
                     User Component
                with internal interfaces

  The cisstMultiTask has two different set of internal connections between
  components.

  1) InterfaceInternal.Required - InterfaceComponent.Provided
     : Established whenever mtsManagerComponent::ComponentAdd() gets called

  2) InterfaceComponent.Required - InterfaceInternal.Provided
     : When user component with internal interfaces connects to MCS
       (See mtsManagerComponent::AddNewClientComponent())
*/

class CISST_EXPORT mtsManagerComponentBase : public mtsTaskFromSignal
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    /*! Component name definitions */
    class CISST_EXPORT ComponentNames {
    public:
        /*! Name of manager component.  Should be globally unique */
        const static std::string ManagerComponent;
    };

    /*! Interface name definitions */
    class CISST_EXPORT InterfaceNames {
    public:
        // Internal interfaces for dynamic component services
        const static std::string InterfaceInternalProvided;
        const static std::string InterfaceInternalRequired;
        const static std::string InterfaceComponentProvided;
        const static std::string InterfaceComponentRequired;
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
        const static std::string ComponentAdd;
        const static std::string ComponentRemove;
        const static std::string ComponentGet;
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
        // Getters
        const static std::string GetNamesOfProcesses;
        const static std::string GetNamesOfComponents;
        const static std::string GetDescriptionsOfComponents;
        const static std::string GetNamesOfInterfaces;
        const static std::string GetDescriptionsOfInterfaces;
        const static std::string GetListOfConnections;
        const static std::string GetListOfComponentClasses;
        const static std::string GetInterfaceProvidedDescription;
        const static std::string GetInterfaceRequiredDescription;
        // Get absolute time for each process
        const static std::string GetAbsoluteTimeInSeconds;
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
        const static std::string PrintLog;
    };

    mtsManagerComponentBase(const std::string & componentName);
    virtual ~mtsManagerComponentBase();

    static bool IsManagerComponent(const std::string & componentName);
    static bool IsNameOfInternalInterface(const std::string & interfaceName);

    static bool IsNameOfInterfaceComponentRequired(const std::string & interfaceName);
    static bool IsNameOfInterfaceComponentProvided(const std::string & interfaceName);
    static bool IsNameOfInterfaceInternalRequired(const std::string & interfaceName);
    static bool IsNameOfInterfaceInternalProvided(const std::string & interfaceName);

    static const std::string GetNameOfManagerComponent(void);
    static const std::string GetNameOfInterfaceComponentRequiredFor(const std::string & componentName);
    static const std::string GetNameOfInterfaceComponentProvided(void);
    static const std::string GetNameOfInterfaceInternalRequired(void);
    static const std::string GetNameOfInterfaceInternalProvided(void);

    // mtsTask implementation
    void Run(void) override;
    void Cleanup(void) override;
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsManagerComponentBase);

#endif // _mtsManagerComponentBase_h
