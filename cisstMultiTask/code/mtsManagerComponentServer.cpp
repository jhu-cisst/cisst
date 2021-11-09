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

#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsInterfaceCommon.h>
#include <cisstMultiTask/mtsManagerComponentServer.h>
#include <cisstMultiTask/mtsManagerComponentClient.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsManagerGlobal.h>

CMN_IMPLEMENT_SERVICES_DERIVED(mtsManagerComponentServer, mtsManagerComponentBase);

mtsManagerComponentServer::mtsManagerComponentServer(mtsManagerGlobal * gcm)
    : mtsManagerComponentBase(mtsManagerComponentBase::GetNameOfManagerComponentServer()),
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

    // For system-wide thread-safe logging
    mtsInterfaceProvided * provided = AddInterfaceProvided(
        mtsManagerComponentBase::InterfaceNames::InterfaceSystemLoggerProvided);
    if (provided) {
        provided->AddEventWrite(this->EventPrintLog,
                                mtsManagerComponentBase::EventNames::PrintLog,
                                mtsLogMessage());
    }
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
    const std::string interfaceName = mtsManagerComponentBase::GetNameOfInterfaceGCMProvided();
    mtsInterfaceProvided * provided = AddInterfaceProvided(interfaceName);
    if (!provided) {
        CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceGCM: failed to add \"GCM\" provided interface: " << interfaceName << std::endl;
        return false;
    }

    provided->AddCommandWriteReturn(&mtsManagerComponentServer::InterfaceGCMCommands_ComponentCreate,
                                    this, mtsManagerComponentBase::CommandNames::ComponentCreate);
    provided->AddCommandWrite(&mtsManagerComponentServer::InterfaceGCMCommands_ComponentConfigure,
                              this, mtsManagerComponentBase::CommandNames::ComponentConfigure);
    provided->AddCommandWriteReturn(&mtsManagerComponentServer::InterfaceGCMCommands_ComponentConnectNew,
                                    this, mtsManagerComponentBase::CommandNames::ComponentConnect);
    provided->AddCommandWriteReturn(&mtsManagerComponentServer::InterfaceGCMCommands_ComponentDisconnectNew,
                                    this, mtsManagerComponentBase::CommandNames::ComponentDisconnect);
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
    provided->AddCommandQualifiedRead(&mtsManagerComponentServer::InterfaceGCMCommands_GetListOfComponentClasses,
                              this, mtsManagerComponentBase::CommandNames::GetListOfComponentClasses);
    provided->AddCommandQualifiedRead(&mtsManagerComponentServer::InterfaceGCMCommands_GetInterfaceProvidedDescription,
                              this, mtsManagerComponentBase::CommandNames::GetInterfaceProvidedDescription);
    provided->AddCommandQualifiedRead(&mtsManagerComponentServer::InterfaceGCMCommands_GetInterfaceRequiredDescription,
                              this, mtsManagerComponentBase::CommandNames::GetInterfaceRequiredDescription);
    provided->AddCommandQualifiedRead(&mtsManagerComponentServer::InterfaceGCMCommands_LoadLibrary,
                              this, mtsManagerComponentBase::CommandNames::LoadLibrary);
    provided->AddCommandWrite(&mtsManagerComponentServer::InterfaceGCMCommands_PrintLog,
                              this, mtsManagerComponentBase::CommandNames::PrintLog, MTS_COMMAND_NOT_QUEUED);
    provided->AddCommandWrite(&mtsManagerComponentServer::InterfaceGCMCommands_EnableLogForwarding,
                              this, mtsManagerComponentBase::CommandNames::EnableLogForwarding);
    provided->AddCommandWrite(&mtsManagerComponentServer::InterfaceGCMCommands_DisableLogForwarding,
                              this, mtsManagerComponentBase::CommandNames::DisableLogForwarding);
    provided->AddCommandQualifiedRead(&mtsManagerComponentServer::InterfaceGCMCommands_GetLogForwardingStates,
                              this, mtsManagerComponentBase::CommandNames::GetLogForwardingStates);
    provided->AddCommandQualifiedRead(&mtsManagerComponentServer::InterfaceGCMCommands_GetAbsoluteTimeDiffs,
                                      this, mtsManagerComponentBase::CommandNames::GetAbsoluteTimeDiffs);

    provided->AddEventWrite(this->InterfaceGCMEvents_AddComponent,
                            mtsManagerComponentBase::EventNames::AddComponent, mtsDescriptionComponent());
    // MJ TODO: We may need RemoveComponent command as well
    //provided->AddEventWrite(this->InterfaceGCMEvents_AddComponent,
    //                        mtsManagerComponentBase::EventNames::AddComponent, mtsDescriptionComponent());
    provided->AddEventWrite(this->InterfaceGCMEvents_AddConnection,
                            mtsManagerComponentBase::EventNames::AddConnection, mtsDescriptionConnection());
    provided->AddEventWrite(this->InterfaceGCMEvents_RemoveConnection,
                            mtsManagerComponentBase::EventNames::RemoveConnection, mtsDescriptionConnection());
    provided->AddEventWrite(this->InterfaceGCMEvents_ChangeState,
                            mtsManagerComponentBase::EventNames::ChangeState, mtsComponentStateChange());
    provided->AddEventVoid(this->InterfaceGCMEvents_MCSReady,
                           mtsManagerComponentBase::EventNames::MCSReady);

    CMN_LOG_CLASS_INIT_VERBOSE << "AddInterfaceGCM: successfully added \"GCM\" interfaces" << std::endl;

    return true;
}

bool mtsManagerComponentServer::AddNewClientProcess(const std::string & clientProcessName)
{
    if (InterfaceGCMFunctionMap.FindItem(clientProcessName)) {
        CMN_LOG_CLASS_INIT_VERBOSE << "AddNewClientProcess: process is already known" << std::endl;
        return true;
    }

    const std::string interfaceName = mtsManagerComponentBase::GetNameOfInterfaceGCMRequiredFor(clientProcessName);
    mtsInterfaceRequired * required = AddInterfaceRequired(interfaceName);
    if (!required) {
        CMN_LOG_CLASS_INIT_ERROR << "AddNewClientProcess: failed to create \"GCM\" required interface: " << interfaceName << std::endl;
        return false;
    }

    // Create a new set of function objects
    InterfaceGCMFunctionType * newFunctionSet = new InterfaceGCMFunctionType;

    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentCreate,
                          newFunctionSet->ComponentCreate);
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentConfigure,
                          newFunctionSet->ComponentConfigure);
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentConnect,
                          newFunctionSet->ComponentConnectNew);
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentDisconnect,
                          newFunctionSet->ComponentDisconnectNew);
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
    required->AddFunction(mtsManagerComponentBase::CommandNames::GetInterfaceProvidedDescription,
                          newFunctionSet->GetInterfaceProvidedDescription);
    required->AddFunction(mtsManagerComponentBase::CommandNames::GetInterfaceRequiredDescription,
                          newFunctionSet->GetInterfaceRequiredDescription);
    required->AddFunction(mtsManagerComponentBase::CommandNames::LoadLibrary,
                          newFunctionSet->LoadLibrary);
    required->AddFunction(mtsManagerComponentBase::CommandNames::SetLogForwarding,
                          newFunctionSet->SetLogForwarding);
    required->AddFunction(mtsManagerComponentBase::CommandNames::GetLogForwardingState,
                          newFunctionSet->GetLogForwardingState);
    required->AddFunction(mtsManagerComponentBase::CommandNames::GetAbsoluteTimeInSeconds,
                          newFunctionSet->GetAbsoluteTimeInSeconds);
    required->AddFunction(mtsManagerComponentBase::CommandNames::GetListOfComponentClasses,
                          newFunctionSet->GetListOfComponentClasses);

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
    const std::string serverComponentName = mtsManagerComponentBase::GetNameOfManagerComponentClientFor(clientProcessName);
    const std::string serverInterfaceName = mtsManagerComponentBase::GetNameOfInterfaceLCMProvided();
    if (!LCM->Connect(this->GetName(), interfaceName, serverComponentName, serverInterfaceName)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddNewClientProcess: failed to connect: "
            << this->GetName() << ":" << interfaceName
            << " - "
            << serverComponentName << ":" << serverInterfaceName
            << std::endl;
        return false;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "AddNewClientProcess: creation and connection success" << std::endl;

    return true;
}


void mtsManagerComponentServer::InterfaceGCMCommands_ComponentCreate(const mtsDescriptionComponent & componentDescription, bool & result)
{
    // Check if a new component with the name specified can be created
    if (GCM->FindComponent(componentDescription.ProcessName,
                           componentDescription.ComponentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentCreate: failed to create component: " << componentDescription << std::endl
                                << "InterfaceGCMCommands_ComponentCreate: component already exists" << std::endl;
        result = false;
        return;
    }

    // Get a set of function objects that are bound to the InterfaceLCM's provided
    // interface.
    InterfaceGCMFunctionType * functionSet = InterfaceGCMFunctionMap.GetItem(componentDescription.ProcessName);
    if (!functionSet) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentCreate: failed to execute \"Component Create\": " << componentDescription << std::endl;
        result = false;
        return;
    }

    mtsExecutionResult executionResult = functionSet->ComponentCreate(componentDescription, result);
    if (!executionResult.IsOK()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentCreate: failed to execute \"ComponentCreate\": " << componentDescription << std::endl
                                << " error \"" << executionResult << "\"" << std::endl;
        result = false;
    }
}


void mtsManagerComponentServer::InterfaceGCMCommands_ComponentConfigure(const mtsDescriptionComponent & arg)
{
    // Check if component with the name specified can be found
    if (!GCM->FindComponent(arg.ProcessName, arg.ComponentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentConfigure - no component found: " << arg << std::endl;
        return;
    }

    // Get a set of function objects that are bound to the InterfaceLCM's provided
    // interface.
    InterfaceGCMFunctionType * functionSet = InterfaceGCMFunctionMap.GetItem(arg.ProcessName);
    if (!functionSet) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentConfigure: failed to execute \"Component Configure\": " << arg << std::endl;
        return;
    }

    if (!functionSet->ComponentConfigure.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentConfigure: function not bound to command" << std::endl;
        return;
    }

    //functionSet->ComponentConfigure.ExecuteBlocking(arg);
    functionSet->ComponentConfigure(arg);
}


void mtsManagerComponentServer::InterfaceGCMCommands_ComponentConnect(const mtsDescriptionConnection & connectionDescription /*, bool & result*/)
{
    // We don't check argument validity with the GCM at this stage and rely on
    // the current normal connection procedure (GCM allows connection at the
    // request of LCM) because the GCM guarantees that arguments are valid.
    // The Connect request is then passed to the manager component client which
    // calls local component manager's Connect() method.

    // Get a set of function objects that are bound to the InterfaceLCM's provided
    // interface.
    InterfaceGCMFunctionType * functionSet = InterfaceGCMFunctionMap.GetItem(connectionDescription.Client.ProcessName);
    if (!functionSet) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentConnect: failed to execute \"Component Connect\": " << connectionDescription << std::endl;
        // result = false;
        return;
    }
    mtsExecutionResult executionResult =  functionSet->ComponentConnect(connectionDescription /*, result*/);
    if (!executionResult.IsOK()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentConnect: failed to execute \"ComponentConnect\": " << connectionDescription << std::endl
                                << " error \"" << executionResult << "\"" << std::endl;
        // result = false;
    }
}

void mtsManagerComponentServer::InterfaceGCMCommands_ComponentConnectNew(const mtsDescriptionConnection & connectionDescription, bool & result)
{
    // We don't check argument validity with the GCM at this stage and rely on
    // the current normal connection procedure (GCM allows connection at the
    // request of LCM) because the GCM guarantees that arguments are valid.
    // The Connect request is then passed to the manager component client which
    // calls local component manager's Connect() method.

    // Get a set of function objects that are bound to the InterfaceLCM's provided
    // interface.
    InterfaceGCMFunctionType * functionSet = InterfaceGCMFunctionMap.GetItem(connectionDescription.Client.ProcessName);
    if (!functionSet) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentConnect: failed to execute \"Component Connect\": " << connectionDescription << std::endl;
        result = false;
        return;
    }
    mtsExecutionResult executionResult =  functionSet->ComponentConnectNew(connectionDescription, result);
    if (!executionResult.IsOK()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentConnect: failed to execute \"ComponentConnect\": " << connectionDescription << std::endl
                                << " error \"" << executionResult << "\"" << std::endl;
        result = false;
    }
}

// MJ: Another method that does the same thing but accepts a single parameter
// as connection id should be added.
void mtsManagerComponentServer::InterfaceGCMCommands_ComponentDisconnect(const mtsDescriptionConnection & arg)
{
    if (!GCM->Disconnect(arg)) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentDisconnect: failed to execute \"Component Disconnect\" for: " << arg << std::endl;
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

void mtsManagerComponentServer::InterfaceGCMCommands_ComponentDisconnectNew(const mtsDescriptionConnection & arg, bool & result)
{
    // PK: the GCM Disconnect method queue the disconnect request, so we cannot really know if it
    //     has succeeded.
    result = GCM->Disconnect(arg);
    if (!result) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_ComponentDisconnectNew: failed to execute \"Component Disconnect\" for: " << arg << std::endl;
        return;
    }
}

bool mtsManagerComponentServer::ComponentDisconnect(const std::string & processName, const mtsDescriptionConnection & arg)
{
    // Get a set of function objects that are bound to the InterfaceLCM's provided interface.
    InterfaceGCMFunctionType * functionSet = InterfaceGCMFunctionMap.GetItem(processName);
    if (!functionSet) {
        CMN_LOG_CLASS_RUN_ERROR << "ComponentDisconnect: failed to get function set for process \""
            << processName << "\": " << arg << std::endl;
        return false;
    }

    bool result = true;
#if CISST_MTS_NEW
    //functionSet->ComponentDisconnectNew(arg, result);
    result = false;   // PK HACK: actually works better if we don't call ComponentDisconnect or ComponentDisconnectNew
#else
    //functionSet->ComponentDisconnect.ExecuteBlocking(arg);
    functionSet->ComponentDisconnect(arg);
#endif
    return result;
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
    InterfaceGCMFunctionType * functionSet = InterfaceGCMFunctionMap.GetItem(component.ProcessName, CMN_LOG_LEVEL_NONE);
    if (!functionSet) {
        state = mtsComponentState::INITIALIZING;
        CMN_LOG_CLASS_RUN_WARNING << "InterfaceGCMCommands_ComponentGetState: failed to get function set: " << component << std::endl;
        return;
    }
    if (!functionSet->ComponentGetState.IsValid()) {
        state = mtsComponentState::INITIALIZING;
        CMN_LOG_CLASS_RUN_WARNING << "InterfaceGCMCommands_ComponentGetState: failed to execute \"Component GetState\"" << std::endl;
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

void mtsManagerComponentServer::InterfaceGCMCommands_GetInterfaceProvidedDescription(const mtsDescriptionInterface & intfc,
                                                                                     mtsInterfaceProvidedDescription & description) const
{
    if (!GCM->FindComponent(intfc.ProcessName, intfc.ComponentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_GetInterfaceProvidedDescription: failed to get interface description - no component found: "
                                << intfc.ProcessName << ":" << intfc.ComponentName << std::endl;
        return;
    }

    // Get a set of function objects that are bound to the InterfaceLCM's provided interface.
    InterfaceGCMFunctionType * functionSet = InterfaceGCMFunctionMap.GetItem(intfc.ProcessName);
    if (!functionSet) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_GetInterfaceProvidedDescription: failed to get function set for "
                                << intfc.ProcessName << std::endl;
        return;
    }
    if (!functionSet->GetInterfaceProvidedDescription.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_GetInterfaceProvidedDescription: function not bound to command" << std::endl;
        return;
    }

    functionSet->GetInterfaceProvidedDescription(intfc, description);
}

void mtsManagerComponentServer::InterfaceGCMCommands_GetInterfaceRequiredDescription(const mtsDescriptionInterface & intfc,
                                                                                     mtsInterfaceRequiredDescription & description) const
{
    if (!GCM->FindComponent(intfc.ProcessName, intfc.ComponentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_GetInterfaceRequiredDescription: failed to get interface description - no component found: "
                                << intfc.ProcessName << ":" << intfc.ComponentName << std::endl;
        return;
    }

    // Get a set of function objects that are bound to the InterfaceLCM's provided interface.
    InterfaceGCMFunctionType * functionSet = InterfaceGCMFunctionMap.GetItem(intfc.ProcessName);
    if (!functionSet) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_GetInterfaceRequiredDescription: failed to get function set for "
                                << intfc.ProcessName << std::endl;
        return;
    }
    if (!functionSet->GetInterfaceRequiredDescription.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_GetInterfaceRequiredDescription: function not bound to command" << std::endl;
        return;
    }

    functionSet->GetInterfaceRequiredDescription(intfc, description);
}

void mtsManagerComponentServer::InterfaceGCMCommands_LoadLibrary(const mtsDescriptionLoadLibrary &lib,
                                                                       bool &result) const
{
    // Get a set of function objects that are bound to the InterfaceLCM's provided interface.
    InterfaceGCMFunctionType * functionSet = InterfaceGCMFunctionMap.GetItem(lib.ProcessName);
    if (!functionSet) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_LoadLibrary: failed to get function set: " << lib << std::endl;
        result = false;
        return;
    }
    if (!functionSet->LoadLibrary.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_LoadLibrary: function not bound to command" << std::endl;
        result = false;
        return;
    }

    functionSet->LoadLibrary(lib.LibraryName, result);
}

void mtsManagerComponentServer::InterfaceGCMCommands_PrintLog(const mtsLogMessage & log)
{
    static osaTimeServer timeServer = mtsManagerLocal::GetInstance()->GetTimeServer();

#if 0  // NOT USED
    // Get absolute timestamp
    double timestamp = log.Timestamp();
    // Convert absolute to relative timestamp
    struct osaAbsoluteTime s;
    s.FromSeconds(timestamp);
    timeServer.AbsoluteToRelative(s);
#endif

    std::string now;
    osaGetDateTimeString(now, ':');

    std::string msg(log.Message, log.Length);
    std::stringstream ss;
    ss << "|" << now << " " << log.ProcessName << "| " << msg;

    mtsLogMessage _log(ss.str().c_str(), ss.str().size());
    _log.ProcessName = log.ProcessName;

    // Generate system-wide thread-safe logging event
    EventPrintLog(_log);
}

// Internal command
void mtsManagerComponentServer::InterfaceGCMCommands_SetLogForwarding(const std::vector<std::string> & processNames, bool state)
{
    for (unsigned int i = 0; i < processNames.size(); i++) {
        InterfaceGCMFunctionType * functionSet = InterfaceGCMFunctionMap.GetItem(processNames[i], CMN_LOG_LEVEL_NONE);
        if (functionSet)
            functionSet->SetLogForwarding(state);
        else
            CMN_LOG_CLASS_RUN_ERROR << "SetLogForwarding: could not find functions for process " << processNames[i] << std::endl;
    }
}

void mtsManagerComponentServer::InterfaceGCMCommands_GetLogForwardingStates(const stdStringVec & processNames, stdCharVec & states) const
{
    states.resize(processNames.size());

    bool state;
    for (unsigned int i = 0; i < processNames.size(); i++) {
        InterfaceGCMFunctionType * functionSet = InterfaceGCMFunctionMap.GetItem(processNames[i], CMN_LOG_LEVEL_NONE);
        if (functionSet) {
            functionSet->GetLogForwardingState(state);
            states[i] = (state ? 1 : 0);
        }
        else
            CMN_LOG_CLASS_RUN_ERROR << "GetLogForwardingStates: could not find functions for process " << processNames[i] << std::endl;
    }
}

void mtsManagerComponentServer::InterfaceGCMCommands_EnableLogForwarding(const std::vector<std::string> & processNames)
{
    InterfaceGCMCommands_SetLogForwarding(processNames, true);
}

void mtsManagerComponentServer::InterfaceGCMCommands_DisableLogForwarding(const std::vector<std::string> & processNames)
{
    InterfaceGCMCommands_SetLogForwarding(processNames, false);
}

void mtsManagerComponentServer::InterfaceGCMCommands_GetAbsoluteTimeDiffs(const std::vector<std::string> & processNames,
                                                                          std::vector<double> & processTimes) const
{
    double sleepTime = 0.1;
    unsigned int numOfTrials = 10;

    vctDynamicVector<double> trialTimes(numOfTrials);

    double time;
    mtsExecutionResult result;

    // Make sure return vector has correct size
    processTimes.resize(processNames.size());

    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    const std::string thisProcess = LCM->GetProcessName();

    for (unsigned int i = 0; i < processNames.size(); i++) {

        processTimes[i] = 0.0;
        if (processNames[i] == thisProcess)
            continue;

        InterfaceGCMFunctionType * functionSet = InterfaceGCMFunctionMap.GetItem(processNames[i], CMN_LOG_LEVEL_NONE);
        if (!functionSet) {
            CMN_LOG_CLASS_RUN_ERROR << "GetAbsoluteTimeDiffs: could not find functions for process " << processNames[i] << std::endl;
            continue;
        }

        trialTimes.Zeros();
        for (unsigned int t = 0; t < numOfTrials; t++) {
            double tic = LCM->GetTimeServer().GetAbsoluteTimeInSeconds();
            result = functionSet->GetAbsoluteTimeInSeconds(time);
            double roundTripTime =  LCM->GetTimeServer().GetAbsoluteTimeInSeconds() - tic;
            if (result.IsOK()) {
                trialTimes[i] = (tic + roundTripTime/2.0) - time;
            }
            else {
                CMN_LOG_CLASS_RUN_ERROR << "GetAbsoluteTimeDiffs: could not execute command for process " << processNames[i]
                                        << ", result = " << result << std::endl;
                trialTimes.Zeros();
                break;
            }
            osaSleep(sleepTime);
        }
        processTimes[i] = trialTimes.SumOfElements()/numOfTrials;
    }
}

void mtsManagerComponentServer::InterfaceGCMCommands_GetListOfComponentClasses(const std::string & processName,
                                std::vector<mtsDescriptionComponentClass> & listOfComponentClasses) const
{
    // Get a set of function objects that are bound to the InterfaceLCM's provided interface.
    InterfaceGCMFunctionType * functionSet = InterfaceGCMFunctionMap.GetItem(processName);
    if (!functionSet) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_GetListOfComponentClasses: failed to get function set for "
                                << processName << std::endl;
        return;
    }
    if (!functionSet->GetListOfComponentClasses.IsValid()) {
        CMN_LOG_CLASS_RUN_ERROR << "InterfaceGCMCommands_GetListOfComponentClasses: function not bound to command" << std::endl;
        return;
    }

    functionSet->GetListOfComponentClasses(listOfComponentClasses);
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

    return true;
}
