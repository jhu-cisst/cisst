/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2010-08-29

  (C) Copyright 2010-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

/*!
  \brief Declaration of Manager Component Client
  \ingroup cisstMultiTask

  This class defines the manager component client which is managed by all local
  component managers (LCMs).  An instance of this class is automatically created
  and gets connected to the manager component server which runs on LCM that runs
  with the global component manager (GCM).

  This component has two sets of interfaces, one for communication with the
  manager component server and the other one for command exchange between other
  manager component clients.

  \note Related classes: mtsManagerComponentBase, mtsManagerComponentServer
*/

#ifndef _mtsManagerComponentClient_h
#define _mtsManagerComponentClient_h

#include <cisstMultiTask/mtsManagerComponentBase.h>

class mtsManagerComponentClient : public mtsManagerComponentBase
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    friend class mtsManagerLocal;

protected:
    /*! Get a list of all processes running in the system */
    mtsFunctionRead GetNamesOfProcesses;

    /*! If MCS is ready */
    bool MCSReady;

    /*! Functions for InterfaceComponent's required interface.  Since one
        manager component client needs to be able to handle multiple user
        components, we keep a list of function objects using named map with
        (key = component name, value = function object set instance) */
    typedef struct {
        mtsFunctionVoid ComponentStop;
        mtsFunctionVoid ComponentResume;
        mtsFunctionRead ComponentGetState;
        mtsFunctionWriteReturn GetEndUserInterface;
        mtsFunctionWriteReturn AddObserverList;
        mtsFunctionWriteReturn RemoveEndUserInterface;
        mtsFunctionWriteReturn RemoveObserverList;
        mtsFunctionWriteReturn ComponentCreate;
        mtsFunctionWrite       ComponentStartOther;
    } InterfaceComponentFunctionType;

    typedef cmnNamedMap<InterfaceComponentFunctionType> InterfaceComponentFunctionMapType;
    InterfaceComponentFunctionMapType InterfaceComponentFunctionMap;

    /*! Functions for InterfaceLCM's required interface */
    typedef struct {
        // Dynamic component management
        mtsFunctionWriteReturn ComponentCreate;
        mtsFunctionWrite ComponentConfigure;
        mtsFunctionWrite ComponentConnect;
        mtsFunctionWriteReturn ComponentConnectNew;  // for CISST_MTS_NEW
        mtsFunctionWrite ComponentDisconnect;
        mtsFunctionWriteReturn ComponentDisconnectNew;  // for CISST_MTS_NEW
        mtsFunctionWrite ComponentStart;
        mtsFunctionWrite ComponentStop;
        mtsFunctionWrite ComponentResume;
        mtsFunctionQualifiedRead ComponentGetState;
        mtsFunctionQualifiedRead LoadLibrary;
        mtsFunctionWrite PrintLog;
        mtsFunctionWrite EnableLogForwarding;
        mtsFunctionWrite DisableLogForwarding;
        mtsFunctionQualifiedRead GetLogForwardingStates;
        // Getters
        mtsFunctionRead          GetNamesOfProcesses;
        mtsFunctionQualifiedRead GetNamesOfComponents; // in: process name, out: components' names
        mtsFunctionQualifiedRead GetNamesOfInterfaces; // in: process name, out: interfaces' names
        mtsFunctionRead          GetListOfConnections;
        mtsFunctionQualifiedRead GetListOfComponentClasses;
        mtsFunctionQualifiedRead GetInterfaceProvidedDescription;
        mtsFunctionQualifiedRead GetInterfaceRequiredDescription;
        mtsFunctionQualifiedRead GetAbsoluteTimeDiffs;
    } InterfaceLCMFunctionType;

    InterfaceLCMFunctionType InterfaceLCMFunction;

    // Event handlers for InterfaceLCM's required interface (handle events from MCS)
    void HandleAddComponentEvent(const mtsDescriptionComponent &component);
    void HandleChangeStateEvent(const mtsComponentStateChange &componentStateChange);
    void HandleAddConnectionEvent(const mtsDescriptionConnection &connection);
    void HandleRemoveConnectionEvent(const mtsDescriptionConnection &connection);
    void HandleMCSReadyEvent(void);

    // Event handlers for InterfaceComponent's required interface (handle events from Component)
    void HandleChangeStateFromComponent(const mtsComponentStateChange & componentStateChange);

    // General-purpose interface. These are used to allow a class method to be invoked from
    // any thread, but still allow that method to queue commands for execution by the MCC.
    // Because any thread can call these methods, thread-safety is obtained by using a mutex.
    struct GeneralInterfaceStruct {
        osaMutex Mutex;
        mtsFunctionWrite ComponentConnect;
        mtsFunctionWriteReturn ComponentConnectNew;  // for CISST_MTS_NEW
    } GeneralInterface;

    /*! \brief Connect two local interfaces.
        \param clientComponentName Name of client component
        \param clientInterfaceName Name of required/input interface
        \param serverComponentName Name of server component
        \param serverInterfaceName Name of provided/output interface
        \param clientProcessName Name of client process (ignored in standalone
               configuration, used in networked configuration)
        \return true if successful, false otherwise
        \note  It is assumed that the two components are in the same process. */
    bool ConnectLocally(const std::string & clientComponentName, const std::string & clientInterfaceName,
                        const std::string & serverComponentName, const std::string & serverInterfaceName,
                        const std::string & clientProcessName = "");

    bool DisconnectLocally(const std::string & clientComponentName, const std::string & clientInterfaceName,
                           const std::string & serverComponentName, const std::string & serverInterfaceName);

    // If connection between InterfaceComponent.required - InterfaceInternal.provided is
    // disconnected, required interface instance of InterfaceComponent that corresponds
    // to the connection should be removed.
    bool DisconnectCleanup(const std::string & componentName);

    bool AddInterfaceLCM(void);
    bool AddInterfaceComponent(void);

    /*! Create a new set of function objects, add a new instance of
        InterfaceComponent's required interface to this component, and connect
        it to InterfaceInternal's provided interface */
    bool AddNewClientComponent(const std::string & clientComponentName);

    /*! If MCC can forward log messages to MCS.  This returns false until
        MCC gets connected to MCS. */
    bool CanForwardLog(void) const;

    /*! Support for system-wide thread-safe logging. Forward logs to MCS. */
    bool ForwardLog(const mtsLogMessage & log) const;

    // Called from LCM
    bool Connect(const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
                 const std::string & serverComponentName, const std::string & serverInterfaceProvidedName);

    bool IsLocalProcess(const std::string &procName) const;

    /*! Commands for InterfaceLCM's provided interface */
    void InterfaceLCMCommands_ComponentCreate(const mtsDescriptionComponent & componentDescription, bool & result);
    void InterfaceLCMCommands_ComponentConfigure(const mtsDescriptionComponent & arg);
    void InterfaceLCMCommands_ComponentConnect(const mtsDescriptionConnection & connectionDescription /*, bool & result*/);
    void InterfaceLCMCommands_ComponentConnectNew(const mtsDescriptionConnection & connectionDescription, bool & result);
    void InterfaceLCMCommands_ComponentDisconnect(const mtsDescriptionConnection & arg);
    void InterfaceLCMCommands_ComponentDisconnectNew(const mtsDescriptionConnection & arg, bool & result);
    void InterfaceLCMCommands_ComponentStart(const mtsComponentStatusControl & arg);
    void InterfaceLCMCommands_ComponentStop(const mtsComponentStatusControl & arg);
    void InterfaceLCMCommands_ComponentResume(const mtsComponentStatusControl & arg);
    void InterfaceLCMCommands_ComponentGetState(const mtsDescriptionComponent &component,
                                                mtsComponentState &state) const;
    void InterfaceLCMCommands_GetInterfaceProvidedDescription(const mtsDescriptionInterface &intfc,
                                                              mtsInterfaceProvidedDescription & description) const;
    void InterfaceLCMCommands_GetInterfaceRequiredDescription(const mtsDescriptionInterface &intfc,
                                                              mtsInterfaceRequiredDescription & description) const;
    void InterfaceLCMCommands_LoadLibrary(const std::string &fileName, bool &result) const;
    void InterfaceLCMCommands_GetListOfComponentClasses(
                              std::vector<mtsDescriptionComponentClass> & listOfComponentClasses) const;
    void InterfaceLCMCommands_SetLogForwarding(const bool &state);
    void InterfaceLCMCommands_GetLogForwardingState(bool & state) const;
    void InterfaceLCMCommands_GetAbsoluteTimeInSeconds(double &time) const;

    /*! Event generators for InterfaceLCM's provided interface */
    mtsFunctionWrite InterfaceLCMEvents_ChangeState;

    /*! Commands for InterfaceComponent's provided interface */
    void InterfaceComponentCommands_ComponentCreate(const mtsDescriptionComponent & componentDescription, bool & result);
    void InterfaceComponentCommands_ComponentConfigure(const mtsDescriptionComponent & arg);
    void InterfaceComponentCommands_ComponentConnect(const mtsDescriptionConnection & connectionDescription /*, bool & result*/);
    void InterfaceComponentCommands_ComponentConnectNew(const mtsDescriptionConnection & connectionDescription, bool & result);
    void InterfaceComponentCommands_ComponentDisconnect(const mtsDescriptionConnection & arg);
    void InterfaceComponentCommands_ComponentDisconnectNew(const mtsDescriptionConnection & arg, bool & result);
    void InterfaceComponentCommands_ComponentStart(const mtsComponentStatusControl & arg);
    void InterfaceComponentCommands_ComponentStop(const mtsComponentStatusControl & arg);
    void InterfaceComponentCommands_ComponentResume(const mtsComponentStatusControl & arg);
    void InterfaceComponentCommands_ComponentGetState(const mtsDescriptionComponent &component,
                                                      mtsComponentState &state) const;

    void InterfaceComponentCommands_GetNamesOfProcesses(std::vector<std::string> & names) const;
    void InterfaceComponentCommands_GetNamesOfComponents(const std::string & processName,
                                                         std::vector<std::string> & names) const;
    void InterfaceComponentCommands_GetNamesOfInterfaces(const mtsDescriptionComponent & component, mtsDescriptionInterface & interfaces) const;
    void InterfaceComponentCommands_GetListOfConnections(std::vector <mtsDescriptionConnection> & listOfConnections) const;
    void InterfaceComponentCommands_GetListOfComponentClasses(const std::string &processName,
                                                              std::vector <mtsDescriptionComponentClass> & listOfComponentClasses) const;
    void InterfaceComponentCommands_GetInterfaceProvidedDescription(const mtsDescriptionInterface & intfc,
                                                                    mtsInterfaceProvidedDescription & description) const;
    void InterfaceComponentCommands_GetInterfaceRequiredDescription(const mtsDescriptionInterface & intfc,
                                                                    mtsInterfaceRequiredDescription & description) const;
    void InterfaceComponentCommands_LoadLibrary(const mtsDescriptionLoadLibrary &lib, bool &result) const;
    void InterfaceComponentCommands_EnableLogForwarding(const std::vector<std::string> &processNames);
    void InterfaceComponentCommands_DisableLogForwarding(const std::vector<std::string> &processNames);
    void InterfaceComponentCommands_GetLogForwardingStates(const stdStringVec & processNames, stdCharVec & states) const;
    void InterfaceComponentCommands_GetAbsoluteTimeDiffs(const std::vector<std::string> &processNames,
                                                         std::vector<double> &processTimes) const;

    /*! Event generators for InterfaceComponent's provided interface */
    mtsFunctionWrite InterfaceComponentEvents_AddComponent;
    mtsFunctionWrite InterfaceComponentEvents_ChangeState;
    mtsFunctionWrite InterfaceComponentEvents_AddConnection;
    mtsFunctionWrite InterfaceComponentEvents_RemoveConnection;

    void GetAbsoluteTimeInSeconds(mtsDouble &time) const;  // DEPRECATED


public:
    mtsManagerComponentClient(const std::string & componentName);
    ~mtsManagerComponentClient();

    void Startup(void);
    void Run(void);
    void Cleanup(void);

};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsManagerComponentClient);

#endif // _mtsManagerComponentClient_h
