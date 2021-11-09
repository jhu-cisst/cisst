/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Min Yang Jung, Peter Kazanzides
  Created on: 2010-08-29

  (C) Copyright 2010-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsManagerComponentServices_h
#define _mtsManagerComponentServices_h

#include <cisstMultiTask/mtsManagerComponentBase.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsParameterTypes.h>
#include <cisstMultiTask/mtsFunctionRead.h>
#include <cisstMultiTask/mtsFunctionQualifiedRead.h>
#include <cisstMultiTask/mtsFunctionWrite.h>
#include <cisstMultiTask/mtsEventReceiver.h>

class CISST_EXPORT mtsManagerComponentServices : public cmnGenericObject {

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT)

protected:
    mtsInterfaceRequired * InternalInterfaceRequired;

    /*! Internal functions to use services provided by manager component client */
    // Dynamic component management
    struct ManagementStruct {
        mtsFunctionWriteReturn Create;
        mtsFunctionWrite Configure;
        mtsFunctionWrite Connect;
        mtsFunctionWriteReturn ConnectNew;   // used for CISST_MTS_NEW
        mtsFunctionWrite Disconnect;
        mtsFunctionWriteReturn DisconnectNew;   // used for CISST_MTS_NEW
        mtsFunctionWrite Start;
        mtsFunctionWrite Stop;
        mtsFunctionWrite Resume;
        mtsFunctionQualifiedRead GetState;     // in: process, component, out: state
        mtsFunctionQualifiedRead LoadLibrary;  // in: process, library name, out: result (bool)
    } ServiceComponentManagement;

    struct LogStruct {
        mtsFunctionWrite EnableLogForwarding;
        mtsFunctionWrite DisableLogForwarding;
        mtsFunctionQualifiedRead GetLogForwardingStates;
    } ServiceLogManagement;

    // Getters
    struct GetterStruct {
        mtsFunctionRead          GetNamesOfProcesses;
        mtsFunctionQualifiedRead GetNamesOfComponents; // in: process name, out: components' names
        mtsFunctionQualifiedRead GetNamesOfInterfaces; // in: process name, out: interfaces' names
        mtsFunctionRead          GetListOfConnections;
        mtsFunctionQualifiedRead GetListOfComponentClasses;  // in: process name, out: list of classes
        mtsFunctionQualifiedRead GetInterfaceProvidedDescription;
        mtsFunctionQualifiedRead GetInterfaceRequiredDescription;
        mtsFunctionQualifiedRead GetAbsoluteTimeDiffs;
    } ServiceGetters;

    // Event receivers
    struct EventStruct {
        mtsEventReceiverWrite    AddComponent;
        mtsEventReceiverWrite    AddConnection;
        mtsEventReceiverWrite    RemoveConnection;
        mtsEventReceiverWrite    ChangeState;
    } EventReceivers;

    // private function used by WaitFor methods.
    bool CheckAndWait(const std::vector<std::string> &list, const std::string &key, double &timeoutInSec,
                      mtsEventReceiverWrite &eventReceiver);
public:
    mtsManagerComponentServices(mtsInterfaceRequired * internalInterfaceRequired);
    ~mtsManagerComponentServices() {}

    bool InitializeInterfaceInternalRequired(void);

    bool IsConnected(void) const
    { return (InternalInterfaceRequired->GetConnectedInterface() != 0); }

    /*! Add event handlers to the internal required interface */
    //@{
    template <class __classType>
    bool AddComponentEventHandler(void (__classType::*method)(const mtsDescriptionComponent &),
                                  __classType * classInstantiation,
                                  mtsEventQueueingPolicy queueingPolicy = MTS_INTERFACE_EVENT_POLICY)
    {
        return (EventReceivers.AddComponent.SetHandler(method, classInstantiation, queueingPolicy) != 0);
    }

    template <class __classType>
    bool AddConnectionEventHandler(void (__classType::*method)(const mtsDescriptionConnection &),
                                   __classType * classInstantiation,
                                   mtsEventQueueingPolicy queueingPolicy = MTS_INTERFACE_EVENT_POLICY)
    {
        return (EventReceivers.AddConnection.SetHandler(method, classInstantiation, queueingPolicy) != 0);
    }

    template <class __classType>
    bool RemoveConnectionEventHandler(void (__classType::*method)(const mtsDescriptionConnection &),
                                      __classType * classInstantiation,
                                      mtsEventQueueingPolicy queueingPolicy = MTS_INTERFACE_EVENT_POLICY)
    {
        return (EventReceivers.RemoveConnection.SetHandler(method, classInstantiation, queueingPolicy) != 0);
    }

    template <class __classType>
    bool ChangeStateEventHandler(void (__classType::*method)(const mtsComponentStateChange &),
                                 __classType * classInstantiation,
                                 mtsEventQueueingPolicy queueingPolicy = MTS_INTERFACE_EVENT_POLICY)
    {
        return (EventReceivers.ChangeState.SetHandler(method, classInstantiation, queueingPolicy) != 0);
    }
    //@}

    /*! Wrappers for internal function objects */
    //@{
    bool ComponentCreate(const std::string & className, const std::string & componentName) const;
    bool ComponentCreate(const std::string & processName, const std::string & className,
                         const std::string & componentName) const;

    bool ComponentCreate(const std::string & className, const mtsGenericObject & constructorArg) const;
    bool ComponentCreate(
        const std::string& processName, const std::string & className, const mtsGenericObject & constructorArg) const;

    bool ComponentConfigure(const std::string & componentName, const std::string & configString) const;
    bool ComponentConfigure(
        const std::string& processName, const std::string & componentName, const std::string & configString) const;

    bool Connect(const std::string & clientComponentName, const std::string & clientInterfaceName,
                 const std::string & serverComponentName, const std::string & serverInterfaceName) const;
    bool Connect(const std::string & clientProcessName,
                 const std::string & clientComponentName, const std::string & clientInterfaceName,
                 const std::string & serverProcessName,
                 const std::string & serverComponentName, const std::string & serverInterfaceName) const;
    bool Connect(const mtsDescriptionConnection & connection) const;

    bool Disconnect(
        const std::string & clientComponentName, const std::string & clientInterfaceName,
        const std::string & serverComponentName, const std::string & serverInterfaceName) const;
    bool Disconnect(
        const std::string & clientProcessName,
        const std::string & clientComponentName, const std::string & clientInterfaceName,
        const std::string & serverProcessName,
        const std::string & serverComponentName, const std::string & serverInterfaceName) const;
    bool Disconnect(const mtsDescriptionConnection & connection) const;
    bool Disconnect(ConnectionIDType connectionID) const;

    bool ComponentStart(const std::string & componentName, const double delayInSecond) const;
    bool ComponentStart(const std::string & componentName) const
         { return ComponentStart(componentName, 0.0); }
    bool ComponentStart(const std::string& processName, const std::string & componentName,
                        const double delayInSecond) const;
    bool ComponentStart(const std::string& processName, const std::string & componentName) const
         { return ComponentStart(processName, componentName, 0.0); }


    bool ComponentStop(const std::string & componentName, const double delayInSecond) const;
    bool ComponentStop(const std::string & componentName) const
         { return ComponentStop(componentName, 0.0); }
    bool ComponentStop(const std::string& processName, const std::string & componentName,
                       const double delayInSecond) const;
    bool ComponentStop(const std::string& processName, const std::string & componentName) const
         { return ComponentStop(processName, componentName, 0.0); }

    bool ComponentResume(const std::string & componentName, const double delayInSecond = 0.0) const;
    bool ComponentResume(const std::string& processName, const std::string & componentName,
                         const double delayInSecond = 0.0) const;

    mtsComponentState ComponentGetState(const mtsDescriptionComponent &component) const;
    std::string ComponentGetState(const std::string componentName) const;
    std::string ComponentGetState(const std::string & processName, const std::string componentName) const;

    std::vector<std::string> GetNamesOfProcesses(void) const;
    std::vector<std::string> GetNamesOfComponents(const std::string & processName) const;
    bool GetNamesOfInterfaces(const std::string & processName,
                              const std::string & componentName,
                              std::vector<std::string> & namesOfInterfacesRequired,
                              std::vector<std::string> & namesOfInterfacesProvided) const;

    std::vector<mtsDescriptionConnection> GetListOfConnections(void) const;

    std::vector<mtsDescriptionComponentClass> GetListOfComponentClasses(void) const;
    std::vector<mtsDescriptionComponentClass> GetListOfComponentClasses(const std::string & processName) const;

    mtsInterfaceProvidedDescription
        GetInterfaceProvidedDescription(const std::string & processName,
                                        const std::string & componentName, const std::string & interfaceName) const;
    inline mtsInterfaceProvidedDescription
        GetInterfaceProvidedDescription(const mtsDescriptionInterfaceFullName & interfaceDescription) const {
        return GetInterfaceProvidedDescription(interfaceDescription.ProcessName,
                                               interfaceDescription.ComponentName,
                                               interfaceDescription.InterfaceName);
    }

    mtsInterfaceRequiredDescription
        GetInterfaceRequiredDescription(const std::string & processName,
                                        const std::string & componentName, const std::string & interfaceName) const;
    inline mtsInterfaceRequiredDescription
        GetInterfaceRequiredDescription(const mtsDescriptionInterfaceFullName & interfaceDescription) const {
        return GetInterfaceRequiredDescription(interfaceDescription.ProcessName,
                                               interfaceDescription.ComponentName,
                                               interfaceDescription.InterfaceName);
    }

    // Dynamically load the file (fileName) into the current process
    bool Load(const std::string & fileName) const;
    // Dynamically load the file (fileName) into the process processName
    bool Load(const std::string & processName, const std::string & fileName) const;

    // Enable/disable log forwarding
    void EnableLogForwarding(void);  // for all processes
    void EnableLogForwarding(const std::vector<std::string> &processNames);
    void DisableLogForwarding(void);  // for all processes
    void DisableLogForwarding(const std::vector<std::string> &processNames);
    void GetLogForwardingStates(stdCharVec & states) const;  // for all processes
    void GetLogForwardingStates(const stdStringVec & processNames, stdCharVec & states) const;

    // Get absolute time differences of specified processes with respect to GCM
    std::vector<double> GetAbsoluteTimeDiffs(const std::vector<std::string> &processNames) const;

    // Wait for component state change. This can be used to wait for a process to be created, or for a component
    // to be created (in those cases, any state is acceptable). A negative timeout is used to wait indefinitely.
    // Returns false if timeout occurred.
    bool WaitFor(const std::string & processName, double timeoutInSec = -1.0);
    bool WaitFor(const std::string & processName, const std::string & componentName, double timeoutInSec = -1.0);
    bool WaitFor(const std::string & processName, const std::string & componentName, const std::string & state,
                 double timeoutInSec = -1.0);
    //@}

};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsManagerComponentServices)

#endif // _mtsManagerComponentServices_h
