/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsManagerComponentServer.h 1726 2010-08-30 05:07:54Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2010-08-29

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

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

    /*! Getters for GCM service provider */
    void GetNamesOfProcesses(mtsStdStringVec & stdStringVec) const;

    /*! Functions.  Since one manager component server needs to be able to
        handle multiple manager component clients, we keep a list of 
        function objects using named map with (key = process name, 
        value = function object set instance) */
    typedef struct {
        mtsFunctionWrite ComponentCreate;
        mtsFunctionWrite ComponentConnect;
        mtsFunctionWrite ComponentStart;
        mtsFunctionWrite ComponentStop;
        mtsFunctionWrite ComponentResume;
    } InterfaceGCMFunctionType;

    typedef cmnNamedMap<InterfaceGCMFunctionType> InterfaceGCMFunctionMapType;
    InterfaceGCMFunctionMapType InterfaceGCMFunctionMap;

    /*! Protected constructor to disallow users to create this component */
    mtsManagerComponentServer(mtsManagerGlobal * gcm);
    ~mtsManagerComponentServer();

public:
    void Startup(void);
    void Run(void);
    void Cleanup(void);

    /*! Add InterfaceGCM */
    bool AddInterfaceGCM(void);

    /*! Create a new set of function objects, add InterfaceGCM's required
        interface to this component, and connect it to InterfaceLCM's
        provided interface */
    bool AddNewClientProcess(const std::string & clientProcessName);

    /*! Commands */
    void InterfaceGCMCommands_ComponentCreate(const mtsDescriptionComponent & arg);
    void InterfaceGCMCommands_ComponentConnect(const mtsDescriptionConnection & arg);
    void InterfaceGCMCommands_ComponentStart(const mtsComponentStatusControl & arg);
    void InterfaceGCMCommands_ComponentStop(const mtsComponentStatusControl & arg);
    void InterfaceGCMCommands_ComponentResume(const mtsComponentStatusControl & arg);
    void InterfaceGCMCommands_GetNamesOfProcesses(mtsStdStringVec & names) const;
    void InterfaceGCMCommands_GetNamesOfComponents(const mtsStdString & processName, mtsStdStringVec & names) const;
    void InterfaceGCMCommands_GetNamesOfInterfaces(const mtsDescriptionComponent & component, mtsDescriptionInterface & interfaces) const;
    void InterfaceGCMCommands_GetListOfConnections(std::vector <mtsDescriptionConnection> & listOfConnections) const;

    /*! Event generators */
    mtsFunctionWrite InterfaceGCMEvents_AddComponent;
    mtsFunctionWrite InterfaceGCMEvents_AddConnection;

    // Methods for use by mtsManagerGlobal (Global Component Manager, GCM).
    // These just cause an event to be generated on the Manager Component provided interface.
    void AddComponentEvent(const mtsDescriptionComponent &component);
    void AddConnectionEvent(const mtsDescriptionConnection &connection);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsManagerComponentServer);

#endif // _mtsManagerComponentServer_h
