/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2010-08-29

  (C) Copyright 2010-2013 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

/*!
  \brief Declaration of Manager Component Server
  \ingroup cisstMultiTask

  This class defines the manager component server which is managed by the local
  component manager (LCM) that runs with the global component manager (GCM).
  Only one manager component server exists in the whole system and all the other
  manager components should be of type manager component client
  (mtsManagerComponentClient) which internally gets connected to the manager
  component server when they start.

  This component provides services for other manager component clients to allow
  dynamic component creation and connection request (disconnection and
  reconnection will be handled later).

  \note Related classes: mtsManagerComponentBase, mtsManagerComponentClient
*/

#ifndef _mtsManagerComponentServer_h
#define _mtsManagerComponentServer_h

#include <cisstMultiTask/mtsManagerComponentBase.h>

class mtsManagerGlobal;

class mtsManagerComponentServer : public mtsManagerComponentBase
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    friend class mtsManagerLocal;

protected:
    /*! Global component manager instance to directly use the services it
        provides */
    mtsManagerGlobal * GCM;

    /*! Add InterfaceGCM */
    bool AddInterfaceGCM(void);

    /*! Getters for GCM service provider */
    void GetNamesOfProcesses(std::vector<std::string> & processList) const;

    /*! Functions.  Since one manager component server needs to be able to
        handle multiple manager component clients, we keep a list of
        function objects using named map with (key = process name,
        value = function object set instance) */
    typedef struct {
        mtsFunctionWriteReturn ComponentCreate;
        mtsFunctionWrite ComponentConfigure;
        mtsFunctionWrite ComponentConnect;
        mtsFunctionWriteReturn ComponentConnectNew;   // for CISST_MTS_NEW
        mtsFunctionWrite ComponentDisconnect;
        mtsFunctionWriteReturn ComponentDisconnectNew;  // for CISST_MTS_NEW
        mtsFunctionWrite ComponentStart;
        mtsFunctionWrite ComponentStop;
        mtsFunctionWrite ComponentResume;
        mtsFunctionQualifiedRead ComponentGetState;
        mtsFunctionQualifiedRead GetInterfaceProvidedDescription;
        mtsFunctionQualifiedRead GetInterfaceRequiredDescription;
        mtsFunctionQualifiedRead LoadLibrary;
        mtsFunctionWrite SetLogForwarding;
        mtsFunctionRead GetLogForwardingState;
        mtsFunctionRead GetAbsoluteTimeInSeconds;
        mtsFunctionRead GetListOfComponentClasses;
    } InterfaceGCMFunctionType;

    typedef cmnNamedMap<InterfaceGCMFunctionType> InterfaceGCMFunctionMapType;
    InterfaceGCMFunctionMapType InterfaceGCMFunctionMap;

    /*! Event generator for system-wide thread-safe logging */
    mtsFunctionWrite EventPrintLog;

    /*! Commands */
    void InterfaceGCMCommands_ComponentCreate(const mtsDescriptionComponent & componentDescription, bool & result);
    void InterfaceGCMCommands_ComponentConfigure(const mtsDescriptionComponent & arg);
    void InterfaceGCMCommands_ComponentConnect(const mtsDescriptionConnection & connectionDescription /*, bool & result*/);
    void InterfaceGCMCommands_ComponentConnectNew(const mtsDescriptionConnection & connectionDescription, bool & result);
    void InterfaceGCMCommands_ComponentDisconnect(const mtsDescriptionConnection & arg);
    void InterfaceGCMCommands_ComponentDisconnectNew(const mtsDescriptionConnection & arg, bool & result);
    void InterfaceGCMCommands_ComponentStart(const mtsComponentStatusControl & arg);
    void InterfaceGCMCommands_ComponentStop(const mtsComponentStatusControl & arg);
    void InterfaceGCMCommands_ComponentResume(const mtsComponentStatusControl & arg);
    void InterfaceGCMCommands_ComponentGetState(const mtsDescriptionComponent &component, mtsComponentState &state) const;
    void InterfaceGCMCommands_GetNamesOfProcesses(std::vector<std::string> & names) const;
    void InterfaceGCMCommands_GetNamesOfComponents(const std::string & processName,
                                                   std::vector<std::string> & names) const;
    void InterfaceGCMCommands_GetNamesOfInterfaces(const mtsDescriptionComponent & component, mtsDescriptionInterface & interfaces) const;
    void InterfaceGCMCommands_GetListOfConnections(std::vector <mtsDescriptionConnection> & listOfConnections) const;
    void InterfaceGCMCommands_GetInterfaceProvidedDescription(const mtsDescriptionInterface & intfc, mtsInterfaceProvidedDescription & description) const;
    void InterfaceGCMCommands_GetInterfaceRequiredDescription(const mtsDescriptionInterface & intfc, mtsInterfaceRequiredDescription & description) const;
    void InterfaceGCMCommands_LoadLibrary(const mtsDescriptionLoadLibrary & lib, bool & result) const;
    void InterfaceGCMCommands_PrintLog(const mtsLogMessage & log);
    void InterfaceGCMCommands_SetLogForwarding(const std::vector<std::string> & processNames, bool state);  // internal
    void InterfaceGCMCommands_EnableLogForwarding(const std::vector<std::string> & processNames);
    void InterfaceGCMCommands_DisableLogForwarding(const std::vector<std::string> & processNames);
    void InterfaceGCMCommands_GetLogForwardingStates(const stdStringVec & processNames, stdCharVec & states) const;
    void InterfaceGCMCommands_GetAbsoluteTimeDiffs(const std::vector<std::string> & processNames,
                                                   std::vector<double> & processTimes) const;
    void InterfaceGCMCommands_GetListOfComponentClasses(const std::string & processName,
                                                        std::vector<mtsDescriptionComponentClass> & listOfComponentClasses) const;

    /*! Event generators */
    mtsFunctionWrite InterfaceGCMEvents_AddComponent;
    mtsFunctionWrite InterfaceGCMEvents_AddConnection;
    mtsFunctionWrite InterfaceGCMEvents_RemoveConnection;
    mtsFunctionWrite InterfaceGCMEvents_ChangeState;
    mtsFunctionVoid InterfaceGCMEvents_MCSReady;

    // Event handler of InterfaceGCM's required interface
    void HandleChangeStateEvent(const mtsComponentStateChange &stateChange);

    /*! Protected constructor to prevent users from createing this component */
    mtsManagerComponentServer(mtsManagerGlobal * gcm);
    ~mtsManagerComponentServer();

public:
    void Startup(void);
    void Run(void);
    void Cleanup(void);

    /*! Create a new set of function objects, add InterfaceGCM's required
        interface to this component, and connect it to InterfaceLCM's
        provided interface */
    bool AddNewClientProcess(const std::string & clientProcessName);

    /*! Methods for global component manager (GCM) */
    void AddComponentEvent(const mtsDescriptionComponent &component);
    void AddConnectionEvent(const mtsDescriptionConnection &connection);
    void RemoveConnectionEvent(const mtsDescriptionConnection &connection);
    // If connection between InterfaceGCM.required - InterfaceLCM.provided is
    // disconnected, required interface instance of InterfaceGCM that corresponds
    // to the connection should be removed.
    bool DisconnectCleanup(const std::string & processName);

    // Calls LCM::DisconnectLocally()
    bool ComponentDisconnect(const std::string & processName, const mtsDescriptionConnection & arg);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsManagerComponentServer);

#endif // _mtsManagerComponentServer_h
