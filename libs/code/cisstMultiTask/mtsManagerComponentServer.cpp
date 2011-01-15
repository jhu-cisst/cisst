/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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

#include <cisstMultiTask/mtsManagerComponentServer.h>
#include <cisstMultiTask/mtsManagerComponentClient.h>
#include <cisstMultiTask/mtsManagerGlobal.h>

CMN_IMPLEMENT_SERVICES(mtsManagerComponentServer);

mtsManagerComponentServer::mtsManagerComponentServer(mtsManagerGlobal * gcm)
    : mtsManagerComponentBase(mtsManagerComponentBase::ComponentNames::ManagerComponentServer),
      GCM(gcm),
      InterfaceGCMFunctionMap("InterfaceGCMFunctionMap")
{
    // Prevent this component from being created more than once
    // MJ: singleton can be implemented instead.
    static int instanceCount = 0;
    if (instanceCount != 0) {
        cmnThrow(std::runtime_error("Error in creating manager component server: it's already created"));
    }
    gcm->SetMCS(this);
    InterfaceGCMFunctionMap.SetOwner(*this);
}

mtsManagerComponentServer::~mtsManagerComponentServer()
{
    InterfaceGCMFunctionMapType::iterator it = InterfaceGCMFunctionMap.begin();
    const InterfaceGCMFunctionMapType::iterator itEnd = InterfaceGCMFunctionMap.end();
    for (; it != itEnd; ++it) {
        delete it->second;
    }
}

void mtsManagerComponentServer::Startup(void)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Manager component SERVER starts" << std::endl;
}

void mtsManagerComponentServer::Run(void)
{
    mtsManagerComponentBase::Run();
}

void mtsManagerComponentServer::Cleanup(void)
{
}

std::string mtsManagerComponentServer::GetNameOfInterfaceGCMRequiredFor(const std::string & processName)
{
    std::string interfaceName = mtsManagerComponentBase::InterfaceNames::InterfaceGCMRequired;
    interfaceName += "For";
    interfaceName += processName;

    return interfaceName;
}

void mtsManagerComponentServer::GetNamesOfProcesses(std::vector<std::string> & processList) const
{
    GCM->GetNamesOfProcesses(processList);
}

bool mtsManagerComponentServer::AddInterfaceGCM(void)
{
    // InterfaceGCM's required interface is not created here but is created
    // when a manager component client connects to the manager component
    // server.  
    // See mtsManagerComponentServer::AddNewClientProcess()
    // for the creation of required interfaces.

    // Add provided interface to which InterfaceLCM's required interface connects.
    const std::string interfaceName = mtsManagerComponentBase::InterfaceNames::InterfaceGCMProvided;
    mtsInterfaceProvided * provided = AddInterfaceProvided(interfaceName);
    if (!provided) {
        CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceGCM: failed to add \"GCM\" provided interface: " << interfaceName << std::endl;
        return false;
    }

    provided->AddCommandWrite(&mtsManagerComponentServer::InterfaceGCMCommands_ComponentCreate,
                              this, mtsManagerComponentBase::CommandNames::ComponentCreate);
    provided->AddCommandWrite(&mtsManagerComponentServer::InterfaceGCMCommands_ComponentConnect,
                              this, mtsManagerComponentBase::CommandNames::ComponentConnect);
    provided->AddCommandWrite(&mtsManagerComponentServer::InterfaceGCMCommands_ComponentDisconnect,
                              this, mtsManagerComponentBase::CommandNames::ComponentDisconnect);
    provided->AddCommandWrite(&mtsManagerComponentServer::InterfaceGCMCommands_ComponentStart,
                              this, mtsManagerComponentBase::CommandNames::ComponentStart);
    provided->AddCommandWrite(&mtsManagerComponentServer::InterfaceGCMCommands_ComponentStop,
                              this, mtsManagerComponentBase::CommandNames::ComponentStop);
    provided->AddCommandWrite(&mtsManagerComponentServer::InterfaceGCMCommands_ComponentResume,
                              this, mtsManagerComponentBase::CommandNames::ComponentResume);
    provided->AddCommandQualifiedRead(&mtsManagerComponentServer::InterfaceGCMCommands_ComponentGetState,
                              this, mtsManagerComponentBase::CommandNames::ComponentGetState);
    provided->AddCommandRead(&mtsManagerComponentServer::InterfaceGCMCommands_GetNamesOfProcesses,
                              this, mtsManagerComponentBase::CommandNames::GetNamesOfProcesses);
    provided->AddCommandQualifiedRead(&mtsManagerComponentServer::InterfaceGCMCommands_GetNamesOfComponents,
                              this, mtsManagerComponentBase::CommandNames::GetNamesOfComponents);
    provided->AddCommandQualifiedRead(&mtsManagerComponentServer::InterfaceGCMCommands_GetNamesOfInterfaces,
                              this, mtsManagerComponentBase::CommandNames::GetNamesOfInterfaces);
    provided->AddCommandRead(&mtsManagerComponentServer::InterfaceGCMCommands_GetListOfConnections,
                              this, mtsManagerComponentBase::CommandNames::GetListOfConnections);

    provided->AddEventWrite(this->InterfaceGCMEvents_AddComponent,
                            mtsManagerComponentBase::EventNames::AddComponent, mtsDescriptionComponent());
    provided->AddEventWrite(this->InterfaceGCMEvents_AddConnection,
                            mtsManagerComponentBase::EventNames::AddConnection, mtsDescriptionConnection());
    provided->AddEventWrite(this->InterfaceGCMEvents_RemoveConnection,
                            mtsManagerComponentBase::EventNames::RemoveConnection, mtsDescriptionConnection());
    provided->AddEventWrite(this->InterfaceGCMEvents_ChangeState,
                            mtsManagerComponentBase::EventNames::ChangeState, mtsComponentStateChange());

    CMN_LOG_CLASS_INIT_VERBOSE << "AddInterfaceGCM: successfully added \"GCM\" interfaces" << std::endl;

    return true;
}

bool mtsManagerComponentServer::AddNewClientProcess(const std::string & clientProcessName)
{
    if (InterfaceGCMFunctionMap.FindItem(clientProcessName)) {
        CMN_LOG_CLASS_INIT_VERBOSE << "AddNewClientProcess: process is already known" << std::endl;
        return true;
    }

    // Create a new set of function objects
    InterfaceGCMFunctionType * newFunctionSet = new InterfaceGCMFunctionType;

    const std::string interfaceName = mtsManagerComponentServer::GetNameOfInterfaceGCMRequiredFor(clientProcessName);
    mtsInterfaceRequired * required = AddInterfaceRequired(interfaceName);
    if (!required) {
        CMN_LOG_CLASS_INIT_ERROR << "AddNewClientProcess: failed to create \"GCM\" required interface: " << interfaceName << std::endl;
        return false;
    }
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentCreate,
                          newFunctionSet->ComponentCreate);
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentConnect,
                          newFunctionSet->ComponentConnect);
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentDisconnect,
                          newFunctionSet->ComponentDisconnect);
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentStart,
                          newFunctionSet->ComponentStart);
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentStop,
                          newFunctionSet->ComponentStop);
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentResume,
                          newFunctionSet->ComponentResume);
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentGetState,
                          newFunctionSet->ComponentGetState);
    required->AddEventHandlerWrite(&mtsManagerComponentServer::HandleChangeStateEvent, this, 
                                   mtsManagerComponentBase::EventNames::ChangeState);

    // Remember a required interface (InterfaceGCM's required interface) to 
    // connect it to the provided interface (InterfaceLCM's provided interface).
    if (!InterfaceGCMFunctionMap.AddItem(clientProcessName, newFunctionSet)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddNewClientProcess: failed to add \"GCM\" required interface: " 
            << "\"" << clientProcessName << "\", " << interfaceName << std::endl;
        return false;
    }

    // Connect InterfaceGCM's required interface to InterfaceLCM's provided interface
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
#if CISST_MTS_HAS_ICE
    if (!LCM->Connect(LCM->GetProcessName(), this->GetName(), interfaceName,
                      clientProcessName, 
                      mtsManagerComponentClient::GetNameOfManagerComponentClient(clientProcessName),
                      mtsManagerComponentBase::InterfaceNames::InterfaceLCMProvided))
    {
        CMN_LOG_CLASS_INIT_ERROR << "AddNewClientProcess: failed to connect: " 
            << mtsManagerGlobal::GetInterfaceUID(LCM->GetProcessName(), this->GetName(), interfaceName)
            << " - "
            << mtsManagerGlobal::GetInterfaceUID(clientProcessName,
                                                 mtsManagerComponentClient::GetNameOfManagerComponentClient(clientProcessName),
                                                 mtsManagerComponentBase::InterfaceNames::InterfaceLCMProvided)
            << std::endl;
        return false;
    }
#else
    if (!LCM->Connect(this->GetName(), interfaceName,
                      mtsManagerComponentClient::GetNameOfManagerComponentClient(clientProcessName),
                      mtsManagerComponentBase::InterfaceNames::InterfaceLCMProvided))
    {
        CMN_LOG_CLASS_INIT_ERROR << "AddNewClientProcess: failed to connect: " 
            << this->GetName() << ":" << interfaceName
            << " - "
            << mtsManagerComponentClient::GetNameOfManagerComponentClient(clientProcessName) << ":"
            << mtsManagerComponentBase::InterfaceNames::InterfaceLCMProvided
            << std::endl;
        return false;
    }
#endif

    CMN_LOG_CLASS_INIT_VERBOSE << "AddNewClientProcess: creation and connection success" << std::endl;

    return true;
}

void mtsManagerComponentServer::InterfaceGCMCommands_ComponentCreate(const mtsDescriptionComponent & arg)
{
    // Check if a new component with the name specified can be created
    if (GCM->FindComponent(arg.ProcessName, arg.ComponentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentCreate: failed to create component: " << arg << std::endl
                                << "InterfaceGCMCommands_ComponentCreate: component already exists" << std::endl;
        return;
    }

    // Get a set of function objects that are bound to the InterfaceLCM's provided
    // interface.
    InterfaceGCMFunctionType * functionSet = InterfaceGCMFunctionMap.GetItem(arg.ProcessName);
    if (!functionSet) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentCreate: failed to execute \"Component Create\": " << arg << std::endl;
        return;
    }

    //functionSet->ComponentCreate.ExecuteBlocking(arg);
    functionSet->ComponentCreate(arg);
}

void mtsManagerComponentServer::InterfaceGCMCommands_ComponentConnect(const mtsDescriptionConnection & arg)
{
    // We don't check argument validity with the GCM at this stage and rely on 
    // the current normal connection procedure (GCM allows connection at the 
    // request of LCM) because the GCM guarantees that arguments are valid.
    // The Connect request is then passed to the manager component client which
    // calls local component manager's Connect() method.

    // Get a set of function objects that are bound to the InterfaceLCM's provided
    // interface.
    InterfaceGCMFunctionType * functionSet = InterfaceGCMFunctionMap.GetItem(arg.Client.ProcessName);
    if (!functionSet) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentConnect: failed to execute \"Component Connect\": " << arg << std::endl;
        return;
    }

    //functionSet->ComponentConnect.ExecuteBlocking(arg);
    functionSet->ComponentConnect(arg);
}

// MJ: Another method that does the same thing but accepts a single parameter 
// as connection id should be added.
void mtsManagerComponentServer::InterfaceGCMCommands_ComponentDisconnect(const mtsDescriptionConnection & arg)
{
    if (!GCM->Disconnect(arg.ConnectionID)) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentDisconnect: failed to execute \"Component Disconnect\" for " 
            << "connection id [ " << arg.ConnectionID << " ]" << std::endl;
        return;
    }

#if 0
    InterfaceGCMFunctionType *functionSet;
    if (arg.Client.ProcessName != arg.Server.ProcessName) {
        // PK TEMP fix for network disconnect
        mtsDescriptionConnection arg2;
        // Step 1: Disconnect Client from ServerProxy  
        CMN_LOG_CLASS_RUN_WARNING << "Network disconnect for " << arg << ", step 1 (client side disconnect)" << std::endl;
        arg2 = arg;
        arg2.Server.ProcessName = arg.Client.ProcessName;
        arg2.Server.ComponentName = mtsManagerGlobal::GetComponentProxyName(arg.Server.ProcessName, arg.Server.ComponentName);
        functionSet = InterfaceGCMFunctionMap.GetItem(arg2.Client.ProcessName);
        if (!functionSet) {
            CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentDisconnect: failed to get function set for " << arg2.Client.ProcessName << std::endl;
            return;
        }
        functionSet->ComponentDisconnect(arg2);
        // Step 2: Disconnect ClientProxy from Server
        CMN_LOG_CLASS_RUN_WARNING << "Network disconnect for " << arg << ", step 2 (server side disconnect)" << std::endl;
        arg2 = arg;
        arg2.Client.ProcessName = arg.Server.ProcessName;
        arg2.Client.ComponentName = mtsManagerGlobal::GetComponentProxyName(arg.Client.ProcessName, arg.Client.ComponentName);
        functionSet = InterfaceGCMFunctionMap.GetItem(arg2.Server.ProcessName);
        if (!functionSet) {
            CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentDisconnect: failed to get function set for " << arg2.Server.ProcessName << std::endl;
            return;
        }
        functionSet->ComponentDisconnect(arg2);
        // Step 3: Update the GCM database (send arg unchanged to any MCC)
        // For now, we just fall through to the statement below
        CMN_LOG_CLASS_RUN_WARNING << "Network disconnect for " << arg << ", step 3 (update GCM)" << std::endl;
    }

    // Get a set of function objects that are bound to the InterfaceLCM's provided
    // interface.
    functionSet = InterfaceGCMFunctionMap.GetItem(arg.Client.ProcessName);
    if (!functionSet) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentDisconnect: failed to execute \"Component Disconnect\": " << arg << std::endl;
        return;
    }

    //functionSet->ComponentDisconnect.ExecuteBlocking(arg);
    functionSet->ComponentDisconnect(arg);
#endif
}

void mtsManagerComponentServer::ComponentDisconnect(const std::string & processName, const mtsDescriptionConnection & arg)
{
    // Get a set of function objects that are bound to the InterfaceLCM's provided interface.
    InterfaceGCMFunctionType * functionSet = InterfaceGCMFunctionMap.GetItem(processName);
    if (!functionSet) {
        CMN_LOG_CLASS_RUN_ERROR << "ComponentDisconnect: failed to get function set for process \"" 
            << processName << "\": " << arg << std::endl;
        return;
    }

    //functionSet->ComponentDisconnect.ExecuteBlocking(arg);
    functionSet->ComponentDisconnect(arg);
}

void mtsManagerComponentServer::InterfaceGCMCommands_ComponentStart(const mtsComponentStatusControl & arg)
{
    // Check if command is for this component (MCS)
    if (arg.ComponentName == this->GetName()) {
        CMN_LOG_CLASS_RUN_WARNING << "ComponentStart for " << arg.ComponentName << " ignored." << std::endl;
        return;
    }
    // Check if a new component with the name specified can be created
    if (!GCM->FindComponent(arg.ProcessName, arg.ComponentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentStart: failed to start component - no component found: " << arg << std::endl;
        return;;
    }

    // Get a set of function objects that are bound to the InterfaceLCM's provided
    // interface.
    InterfaceGCMFunctionType * functionSet = InterfaceGCMFunctionMap.GetItem(arg.ProcessName);
    if (!functionSet) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentStart: failed to get function set: " << arg << std::endl;
        return;
    }
    if (!functionSet->ComponentStart.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentStart: failed to execute \"Component Start\": " << arg << std::endl;
        return;
    }

    //functionSet->ComponentStart.ExecuteBlocking(arg);
    functionSet->ComponentStart(arg);
}

void mtsManagerComponentServer::InterfaceGCMCommands_ComponentStop(const mtsComponentStatusControl & arg)
{
    // Check if command is for this component (MCS)
    if (arg.ComponentName == this->GetName()) {
        CMN_LOG_CLASS_RUN_WARNING << "ComponentStop for " << arg.ComponentName << " ignored." << std::endl;
        return;
    }
    // Check if a new component with the name specified can be created
    if (!GCM->FindComponent(arg.ProcessName, arg.ComponentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentStop: failed to Stop component - no component found: " << arg << std::endl;
        return;
    }

    // Get a set of function objects that are bound to the InterfaceLCM's provided
    // interface.
    InterfaceGCMFunctionType * functionSet = InterfaceGCMFunctionMap.GetItem(arg.ProcessName);
    if (!functionSet) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentStop: failed to get function set: " << arg << std::endl;
        return;
    }
    if (!functionSet->ComponentStop.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentStop: failed to execute \"Component Stop\"" << std::endl;
        return;
    }

    //functionSet->ComponentStop.ExecuteBlocking(arg);
    functionSet->ComponentStop(arg);
}

void mtsManagerComponentServer::InterfaceGCMCommands_ComponentResume(const mtsComponentStatusControl & arg)
{
    // Check if command is for this component (MCS)
    if (arg.ComponentName == this->GetName()) {
        CMN_LOG_CLASS_RUN_WARNING << "ComponentResume for " << arg.ComponentName << " ignored." << std::endl;
        return;
    }
    // Check if a new component with the name specified can be created
    if (!GCM->FindComponent(arg.ProcessName, arg.ComponentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentResume: failed to Resume component - no component found: " << arg << std::endl;
        return;
    }

    // Get a set of function objects that are bound to the InterfaceLCM's provided
    // interface.
    InterfaceGCMFunctionType * functionSet = InterfaceGCMFunctionMap.GetItem(arg.ProcessName);
    if (!functionSet) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentResume: failed to get function set: " << arg << std::endl;
        return;
    }
    if (!functionSet->ComponentResume.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentResume: failed to execute \"Component Resume\"" << std::endl;
        return;
    }

    //functionSet->ComponentResume.ExecuteBlocking(arg);
    functionSet->ComponentResume(arg);
}

void mtsManagerComponentServer::InterfaceGCMCommands_ComponentGetState(const mtsDescriptionComponent &component,
                                                                       mtsComponentState &state) const
{
    // Check if command is for this component (MCS)
    if (component.ComponentName == this->GetName()) {
        //GetState(state);
        //For now, always return active for MCC
        state = mtsComponentState::ACTIVE;
        return;
    }
    if (!GCM->FindComponent(component.ProcessName, component.ComponentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentGetState: failed to get component state - no component found: "
                                << component << std::endl;
        return;
    }

    // Get a set of function objects that are bound to the InterfaceLCM's provided
    // interface.
    InterfaceGCMFunctionType * functionSet = InterfaceGCMFunctionMap.GetItem(component.ProcessName);
    if (!functionSet) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentGetState: failed to get function set: " << component << std::endl;
        return;
    }
    if (!functionSet->ComponentGetState.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentGetState: failed to execute \"Component GetState\"" << std::endl;
        return;
    }

    functionSet->ComponentGetState(component, state);
}

void mtsManagerComponentServer::InterfaceGCMCommands_GetNamesOfProcesses(std::vector<std::string> & names) const
{
    GCM->GetNamesOfProcesses(names);
}

void mtsManagerComponentServer::InterfaceGCMCommands_GetNamesOfComponents(const std::string & processName,
                                                                          std::vector<std::string> & names) const
{
    GCM->GetNamesOfComponents(processName, names);
}

void mtsManagerComponentServer::InterfaceGCMCommands_GetNamesOfInterfaces(const mtsDescriptionComponent & component, mtsDescriptionInterface & interfaces) const
{
    // Get a list of required interfaces
    GCM->GetNamesOfInterfacesRequiredOrInput(component.ProcessName, component.ComponentName, interfaces.InterfaceRequiredNames);

    // Get a list of provided interfaces
    GCM->GetNamesOfInterfacesProvidedOrOutput(component.ProcessName, component.ComponentName, interfaces.InterfaceProvidedNames);
}

void mtsManagerComponentServer::InterfaceGCMCommands_GetListOfConnections(std::vector <mtsDescriptionConnection> & listOfConnections) const
{
    GCM->GetListOfConnections(listOfConnections);
}

void mtsManagerComponentServer::AddComponentEvent(const mtsDescriptionComponent &component)
{
    //InterfaceGCMEvents_AddComponent.ExecuteBlocking(component);
    InterfaceGCMEvents_AddComponent(component);
}

void mtsManagerComponentServer::AddConnectionEvent(const mtsDescriptionConnection &connection)
{
    //InterfaceGCMEvents_AddConnection.ExecuteBlocking(connection);
    InterfaceGCMEvents_AddConnection(connection);
}

void mtsManagerComponentServer::RemoveConnectionEvent(const mtsDescriptionConnection &connection)
{
    //InterfaceGCMEvents_RemoveConnection.ExecuteBlocking(connection);
    InterfaceGCMEvents_RemoveConnection(connection);
}

void mtsManagerComponentServer::HandleChangeStateEvent(const mtsComponentStateChange &stateChange)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "MCS got ChangeState event for " << stateChange.ComponentName << std::endl;
    InterfaceGCMEvents_ChangeState(stateChange);
}

bool mtsManagerComponentServer::DisconnectCleanup(const std::string & processName)
{
    // Get instance of InterfaceGCM's required interface that corresponds to
    // "processName"
    InterfaceGCMFunctionType * functionSet = InterfaceGCMFunctionMap.GetItem(processName);
    if (!functionSet) {
        CMN_LOG_CLASS_RUN_ERROR << "DisconnectCleanup: failed to get function set for process \"" << processName << "\"" << std::endl;
        return false;
    }

    // MJ: This might need to be protected as mutex
    InterfaceGCMFunctionMap.RemoveItem(processName);
    delete functionSet;
}