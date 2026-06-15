/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Peter Kazanzides, Min Yang Jung, Anton Deguet
  Created on: 2010-08-29

  (C) Copyright 2010-2026 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsConfig.h>
#include <cisstMultiTask/mtsManagerComponent.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsInterfaceOutput.h>
#include <cisstMultiTask/mtsInterfaceInput.h>
#include <cisstMultiTask/mtsManagerComponentServices.h>
#include <cisstOSAbstraction/osaGetTime.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstOSAbstraction/osaDynamicLoader.h>
#include <cisstCommon/cmnUnits.h>

CMN_IMPLEMENT_SERVICES_DERIVED(mtsManagerComponent, mtsManagerComponentBase);

mtsManagerComponent::mtsManagerComponent()
    : mtsManagerComponentBase(mtsManagerComponentBase::ComponentNames::ManagerComponent),
      ComponentMap("ComponentMap"),
      InterfaceComponentFunctionMap("InterfaceComponentFunctionMap")
{
    this->mTags.clear();
    this->AddTag("System");
    ComponentMap.SetOwner(*this);
    InterfaceComponentFunctionMap.SetOwner(*this);

    // Add this component to the map
    ComponentMap.AddItem(mtsManagerComponentBase::ComponentNames::ManagerComponent, this);

    // Add provided interface for dynamic component management
    AddInterfaceComponent();

    // Add provided interface for system-wide thread-safe logging
    mtsInterfaceProvided * provided = AddInterfaceProvided(
        mtsManagerComponentBase::InterfaceNames::InterfaceSystemLoggerProvided);
    if (provided) {
        provided->AddCommandWrite(&mtsManagerComponent::LogInterface::PrintLog,
                                  &(this->Logger), mtsManagerComponentBase::CommandNames::PrintLog);
        provided->AddEventWrite(Logger.EventPrintLog,
                                mtsManagerComponentBase::EventNames::PrintLog,
                                mtsLogMessage());
    }
}

mtsManagerComponent::~mtsManagerComponent()
{
}

void mtsManagerComponent::Startup(void)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Manager Component starts" << std::endl;
}

void mtsManagerComponent::Run(void)
{
    mtsManagerComponentBase::Run();
}

void mtsManagerComponent::Cleanup(void)
{
}

std::string mtsManagerComponent::GetInterfaceUID(
      const std::string & processName, const std::string & componentName, const std::string & interfaceName)
{
    return (processName.empty()) ? (componentName + ":" + interfaceName)
                                 : processName + ":" + componentName + ":" + interfaceName;
}
//***************************** Dynamic Component Management (provided interface) ***********************************

bool mtsManagerComponent::AddInterfaceComponent(void)
{
    // InterfaceComponent's required interface is not created here but is created
    // when a user component with internal interfaces connects to the manager
    // component.
    // See mtsManagerComponent::AddNewClientComponent() for the dynamic
    // creation of required interfaces.

    // Add provided interface to which InterfaceInternal's required interface connects.
    std::string interfaceName = mtsManagerComponentBase::GetNameOfInterfaceComponentProvided();
    // Return if provided interface already exists
    if (GetInterfaceProvided(interfaceName)) {
        return true;
    }
    mtsInterfaceProvided * provided = AddInterfaceProvided(interfaceName);
    if (!provided) {
        CMN_LOG_CLASS_INIT_ERROR << "AddInterfaceComponent: failed to add \"Component\" provided interface: "
                                 << interfaceName << std::endl;
        return false;
    }

    //***************************** Component creation *********************************************
    provided->AddCommandWriteReturn(&mtsManagerComponent::ComponentCreate,
                                    this, mtsManagerComponentBase::CommandNames::ComponentCreate);
    provided->AddCommandWriteReturn(&mtsManagerComponent::ComponentAdd,
                                    this, mtsManagerComponentBase::CommandNames::ComponentAdd);
    provided->AddCommandWriteReturn(&mtsManagerComponent::ComponentRemove,
                                    this, mtsManagerComponentBase::CommandNames::ComponentRemove);
    provided->AddCommandQualifiedRead(&mtsManagerComponent::ComponentGet,
                              this, mtsManagerComponentBase::CommandNames::ComponentGet);
    provided->AddCommandWrite(&mtsManagerComponent::ComponentConfigure,
                              this, mtsManagerComponentBase::CommandNames::ComponentConfigure);
    //***************************** Component connection management *********************************
    provided->AddCommandWriteReturn(&mtsManagerComponent::ComponentConnect,
                                    this, mtsManagerComponentBase::CommandNames::ComponentConnect);
    provided->AddCommandWriteReturn(&mtsManagerComponent::ComponentDisconnect,
                                    this, mtsManagerComponentBase::CommandNames::ComponentDisconnect);
    //***************************** Component state management **************************************
    provided->AddCommandWrite(&mtsManagerComponent::ComponentStart,
                              this, mtsManagerComponentBase::CommandNames::ComponentStart);
    provided->AddCommandWrite(&mtsManagerComponent::ComponentStop,
                              this, mtsManagerComponentBase::CommandNames::ComponentStop);
    provided->AddCommandWrite(&mtsManagerComponent::ComponentResume,
                              this, mtsManagerComponentBase::CommandNames::ComponentResume);
    provided->AddCommandQualifiedRead(&mtsManagerComponent::ComponentGetState,
                                      this, mtsManagerComponentBase::CommandNames::ComponentGetState);
    //************************************* Dynamic loading *****************************************
    provided->AddCommandQualifiedRead(&mtsManagerComponent::LoadLibrary,
                                      this, mtsManagerComponentBase::CommandNames::LoadLibrary);
    //****************************** Getters ********************************************************
    provided->AddCommandRead(&mtsManagerComponent::GetNamesOfProcesses,
                              this, mtsManagerComponentBase::CommandNames::GetNamesOfProcesses);
    provided->AddCommandQualifiedRead(&mtsManagerComponent::GetNamesOfComponents,
                              this, mtsManagerComponentBase::CommandNames::GetNamesOfComponents);
    provided->AddCommandQualifiedRead(&mtsManagerComponent::GetDescriptionsOfComponents,
                              this, mtsManagerComponentBase::CommandNames::GetDescriptionsOfComponents);
    provided->AddCommandQualifiedRead(&mtsManagerComponent::GetNamesOfInterfaces,
                              this, mtsManagerComponentBase::CommandNames::GetNamesOfInterfaces);
    provided->AddCommandQualifiedRead(&mtsManagerComponent::GetDescriptionsOfInterfaces,
                              this, mtsManagerComponentBase::CommandNames::GetDescriptionsOfInterfaces);
    provided->AddCommandRead(&mtsManagerComponent::GetListOfConnections,
                              this, mtsManagerComponentBase::CommandNames::GetListOfConnections);
    provided->AddCommandQualifiedRead(&mtsManagerComponent::GetListOfComponentClasses,
                              this, mtsManagerComponentBase::CommandNames::GetListOfComponentClasses);
    provided->AddCommandQualifiedRead(&mtsManagerComponent::GetInterfaceProvidedDescription,
                                      this, mtsManagerComponentBase::CommandNames::GetInterfaceProvidedDescription);
    provided->AddCommandQualifiedRead(&mtsManagerComponent::GetInterfaceRequiredDescription,
                                      this, mtsManagerComponentBase::CommandNames::GetInterfaceRequiredDescription);

    provided->AddEventWrite(this->InterfaceComponentEvents_AddComponent,
                            mtsManagerComponentBase::EventNames::AddComponent, mtsDescriptionComponent());
    provided->AddEventWrite(this->InterfaceComponentEvents_ChangeState,
                            mtsManagerComponentBase::EventNames::ChangeState, mtsComponentStateChange());
    provided->AddEventWrite(this->InterfaceComponentEvents_AddConnection,
                            mtsManagerComponentBase::EventNames::AddConnection, mtsDescriptionConnection());
    provided->AddEventWrite(this->InterfaceComponentEvents_RemoveConnection,
                            mtsManagerComponentBase::EventNames::RemoveConnection, mtsDescriptionConnection());

    CMN_LOG_CLASS_INIT_VERBOSE << "AddInterfaceComponent: successfully added \"Component\" interfaces" << std::endl;

    return true;
}


// Note that this dynamically creates a new component.  Do not confuse with mtsComponent::Create().
void mtsManagerComponent::ComponentCreate(const mtsDescriptionComponent & componentDescription, bool & result)
{
    result = false;
    // CreateComponentDynamically is in LCM so that it can be run in the caller's thread when needed.
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    mtsComponent * newComponent = LCM->CreateComponentDynamically(componentDescription.ClassName,
                                                                  componentDescription.ComponentName,
                                                                  componentDescription.ConstructorArgSerialized);
    if (newComponent) {
        ComponentAdd(newComponent, result);
        if (result) {
            CMN_LOG_CLASS_RUN_VERBOSE << GetName() << ": successfully created and added component: "
                                      << "\"" << componentDescription.ComponentName << "\" of type \""
                                      << componentDescription.ClassName << "\"" << std::endl;
            result = true;
        }
        else {
            CMN_LOG_CLASS_RUN_ERROR << GetName() << ": failed to add component: "
                                    << "\"" << componentDescription.ComponentName << "\" of type \""
                                    << componentDescription.ClassName << "\"" << std::endl;
        }
    }
    else {
        CMN_LOG_CLASS_RUN_ERROR << GetName() << ": failed to create component: "
                                << "\"" << componentDescription.ComponentName << "\" of type \""
                                << componentDescription.ClassName << "\"" << std::endl;
    }
}

void mtsManagerComponent::ComponentAdd(const mtsComponentPointer & componentPtr, bool & result)
{
    result = false;
    mtsComponent * component = componentPtr.GetPointer();
    std::string componentName = component->GetName();

    // If component does not yet have a valid name, assign one now, based on the class
    // name and the pointer value (to ensure that name is unique).
    if (componentName.empty()) {
        componentName.assign(component->Services()->GetName());
        char buf[20];
        sprintf(buf, "_%p", component);
        componentName.append(buf);
        CMN_LOG_CLASS_INIT_DEBUG << "AddComponent: assigning name \"" << componentName << "\"" << std::endl;
        component->SetName(componentName);
    }

    // If dynamic component management is enabled
    if (component->GetInterfaceRequired(mtsManagerComponentBase::GetNameOfInterfaceInternalRequired())) {
        // Add internal provided and required interface for dynamic component management service
        if (!component->AddInterfaceInternal(true)) {
            CMN_LOG_CLASS_INIT_ERROR << "AddComponent: failed to add \"Internal\" provided and required interfaces: "
                                     << componentName << std::endl;
            return;
        }
    }
    // If dynamic component management is not enabled
    else {
        // Add an internal provided interface.  This interface is connected to the
        // manager component and is used to inform it of the change of
        // the running state of this component (more features can be added later).
        if (!component->AddInterfaceInternal()) {
            CMN_LOG_CLASS_INIT_ERROR << "AddComponent: failed to add \"Internal\" provided interfaces: "
                                     << componentName << std::endl;
            return;
        }
    }

    if (!ComponentMap.AddItem(componentName, component)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddComponent: failed to add component to local component manager: "
                                 << componentName << std::endl;
        return;
    }

    // Connect user component's internal interface to the manager component.
    // That is, connect InterfaceInternal.Required to InterfaceComponent.Provided.
    // This enables user components to use dynamic component composition services
    // through cisstMultiTask's thread-safe command pattern.

    // Create InterfaceComponent's required interface which will be connected
    // to connect user component's InterfaceInternal's provided interface.
    if (!AddNewClientComponent(componentName)) {
        CMN_LOG_CLASS_INIT_ERROR << "ComponentAdd: "
                                 << "failed to add InterfaceComponent's required interface to MCS: "
                                 << "\"" << componentName << "\"" << std::endl;
        return;
    }

    // Connect user component to the manager component.  If a component
    // has InterfaceInternal's required interface which provides dynamic
    // component control services, the required interface gets connected to
    // InterfaceComponent's provided interface.
    if (!ConnectToManagerComponent(componentName)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddComponent: failed to connect component \"" << componentName << "\" "
                                 << "to MCS" << std::endl;
        return;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "ComponentAdd: connected user component "
                               << "\"" << componentName << "\" to manager component "
                               << "\"" << GetName() << "\""
                               << std::endl;

    CMN_LOG_CLASS_INIT_VERBOSE << "ComponentAdd: successfully added component: " << componentName << std::endl;
    mtsDescriptionComponent componentDesc("", componentName, component->GetTags());
    InterfaceComponentEvents_AddComponent(componentDesc);
    result = true;
}

void mtsManagerComponent::ComponentRemove(const std::string & componentName, bool & result)
{
    result = false;
    if (DisconnectFromManagerComponent(componentName))
        result = ComponentMap.RemoveItem(componentName);
}

void mtsManagerComponent::ComponentGet(const std::string & componentName, mtsComponentPointer & componentPtr) const
{
    componentPtr.SetPointer(ComponentMap.GetItem(componentName));
}

void mtsManagerComponent::ComponentConfigure(const mtsDescriptionComponent & arg)
{
    mtsComponent * component = GetComponent(arg.ComponentName);
    if (!component) {
        CMN_LOG_CLASS_RUN_ERROR << "ComponentConfigure - no component found: "
                                << arg.ComponentName << std::endl;
        return;
    }

    // For now, using ConstructorArgSerialized field.
    component->Configure(arg.ConstructorArgSerialized);
}


void mtsManagerComponent::ComponentConnect(const mtsDescriptionConnection & arg, bool & result)
{
    result = ConnectInternal(arg.Client.ComponentName, arg.Client.InterfaceName,
                             arg.Server.ComponentName, arg.Server.InterfaceName);
}


void mtsManagerComponent::ComponentDisconnect(const mtsDescriptionConnection & arg, bool & result)
{
    result = true;
    if (!GetComponent(arg.Client.ComponentName)) {
        result = false;
        CMN_LOG_CLASS_RUN_WARNING << "ComponentDisconnect: did not find client component: "
                                  << arg.Client.ComponentName << std::endl;
    }
    if (!GetComponent(arg.Server.ComponentName)) {
        result = false;
        CMN_LOG_CLASS_RUN_WARNING << "ComponentDisconnect: did not find server component: "
                                  << arg.Server.ComponentName << std::endl;
    }
    if (result) {
        if (DisconnectInternal(arg.Client.ComponentName, arg.Client.InterfaceName,
                               arg.Server.ComponentName, arg.Server.InterfaceName))
        {
            CMN_LOG_CLASS_RUN_VERBOSE << "ComponentDisconnect: successfully disconnected: " << arg << std::endl;
        }
        else {
            result = false;
            CMN_LOG_CLASS_RUN_WARNING << "ComponentDisconnect: failed to disconnect: " << arg << std::endl;
        }
    }
}

void mtsManagerComponent::ComponentStart(const mtsComponentStatusControl & arg)
{
    // Check if command is for this component
    if (arg.ComponentName == this->GetName()) {
        CMN_LOG_CLASS_RUN_WARNING << "ComponentStart for " << arg.ComponentName << " ignored." << std::endl;
        return;
    }
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    if (LCM->GetCurrentMainTask() && (LCM->GetCurrentMainTask() != this)) {
        // If there is a main task, we call Start from there. This is really only necessary if we need
        // to start another task that captures the main thread.
        std::string mainTaskName = LCM->GetCurrentMainTask()->GetName();
        CMN_LOG_CLASS_RUN_VERBOSE << "ComponentStart: planning to call main task " << mainTaskName
                                  << " to start component " << arg.ComponentName << std::endl;
        InterfaceComponentFunctionType *functionSetMain = InterfaceComponentFunctionMap.GetItem(mainTaskName);
        if (functionSetMain) {
            if (functionSetMain->ComponentStartOther.IsValid()) {
                functionSetMain->ComponentStartOther(arg);
            }
            else {
                CMN_LOG_CLASS_RUN_ERROR << "ComponentStart: failed to find valid function for main task "
                                        << mainTaskName << ", trying to start component " << arg.ComponentName << std::endl;
            }
        }
        else {
            CMN_LOG_CLASS_RUN_ERROR << "ComponentStart: failed to find function set for main task "
                                    << mainTaskName << std::endl;
        }
    }
    else {
        CMN_LOG_CLASS_RUN_VERBOSE << "ComponentStart: starting component " << arg.ComponentName << " from MCS" << std::endl;
        mtsComponent * component = GetComponent(arg.ComponentName);
        if (component) {
            component->Start();
        }
        else {
            CMN_LOG_CLASS_RUN_ERROR << "ComponentStart: could find component \"" << arg.ComponentName << "\"" << std::endl;
        }
    }
}

void mtsManagerComponent::ComponentStop(const mtsComponentStatusControl & arg)
{
    // Check if command is for this component (MCS)
    if (arg.ComponentName == this->GetName()) {
        CMN_LOG_CLASS_RUN_WARNING << "ComponentStop for " << arg.ComponentName << " ignored." << std::endl;
        return;
    }
    mtsComponent * component = GetComponent(arg.ComponentName);
    if (component) {
        component->Suspend();
    }
    else {
        CMN_LOG_CLASS_RUN_ERROR << "ComponentStop: could find component \"" << arg.ComponentName << "\"" << std::endl;
    }
}

void mtsManagerComponent::ComponentResume(const mtsComponentStatusControl & arg)
{
    mtsComponent * component = GetComponent(arg.ComponentName);
    if (component) {
        // Wait if desired
        osaSleep(arg.DelayInSecond);

        // Resume (Start) the component
        component->Start();
    }
    else {
        CMN_LOG_CLASS_RUN_ERROR << "ComponentResume: did not find component \""
                                << arg.ComponentName << "\"" << std::endl;
    }
}

void mtsManagerComponent::ComponentGetState(const mtsDescriptionComponent &arg,
                                            mtsComponentState &state) const
{
    mtsComponent * component = GetComponent(arg.ComponentName);
    if (component) {
        component->GetState(state);
    }
    else {
        CMN_LOG_CLASS_RUN_ERROR << "ComponentGetState: did not find component \""
                                << arg.ComponentName << "\"" << std::endl;
    }
}

void mtsManagerComponent::LoadLibrary(const mtsDescriptionLoadLibrary &lib, bool &result) const
{
    osaDynamicLoader dl;
    result = dl.Load(lib.LibraryName.c_str());
}

void mtsManagerComponent::GetNamesOfProcesses(std::vector<std::string> & names) const
{
    names.clear();
    names.push_back("");
}
void mtsManagerComponent::GetNamesOfComponents(const std::string & processName,
                                               std::vector<std::string> & names) const
{
    // PK TODO: could check processName
    ComponentMap.GetNames(names);
}

void mtsManagerComponent::GetDescriptionsOfComponents(const std::string & processName,
                                                      std::vector<mtsDescriptionComponent> & descriptions) const
{
    descriptions.clear();
    ComponentMapType::MapType::const_iterator it = ComponentMap.begin();
    const ComponentMapType::MapType::const_iterator itEnd = ComponentMap.end();
    for (; it != itEnd; ++it) {
        mtsDescriptionComponent desc;
        desc.ProcessName = "";
        desc.ComponentName = it->first;
        mtsComponent * component = it->second;
        if (component) {
            desc.ClassName = component->Services()->GetName();
            desc.Tags = component->mTags;
        }
        descriptions.push_back(desc);
    }
}

void mtsManagerComponent::GetNamesOfInterfaces(
    const mtsDescriptionComponent & arg, mtsDescriptionInterface & interfaces) const
{
    mtsComponent * component = GetComponent(arg.ComponentName);
    if (component) {
        interfaces.ProcessName = arg.ProcessName;
        interfaces.ComponentName = arg.ComponentName;
        // Get a list of required interfaces
        interfaces.InterfaceRequiredNames = component->GetNamesOfInterfacesRequiredOrInput();
        // Get a list of provided interfaces
        interfaces.InterfaceProvidedNames =  component->GetNamesOfInterfacesProvidedOrOutput();
    }
    else {
        CMN_LOG_CLASS_RUN_ERROR << "GetNamesOfInterfaces: did not find component \""
                                << arg.ComponentName << "\"" << std::endl;
    }
}

void mtsManagerComponent::GetDescriptionsOfInterfaces(
    const mtsDescriptionComponent & arg, mtsDescriptionInterfaceFull & interfaces) const
{
    mtsComponent * component = GetComponent(arg.ComponentName);
    if (component) {
        std::vector<std::string>::const_iterator it;
        // Get a list of required interfaces
        std::vector<std::string> interfaceRequiredNames = component->GetNamesOfInterfacesRequiredOrInput();
        for (it = interfaceRequiredNames.begin(); it != interfaceRequiredNames.end(); ++it) {
            mtsInterfaceRequired * required = component->GetInterfaceRequired(*it);
            if (required) {
                mtsDescriptionInterfaceFullName desc;
                desc.ProcessName = arg.ProcessName;
                desc.ComponentName = arg.ComponentName;
                desc.InterfaceName = *it;
                desc.Tags = required->GetTags();
                interfaces.Required.push_back(desc);
            }
        }
        // Get a list of provided interfaces
        std::vector<std::string> interfaceProvidedNames = component->GetNamesOfInterfacesProvidedOrOutput();
        for (it = interfaceProvidedNames.begin(); it != interfaceProvidedNames.end(); ++it) {
            mtsInterfaceProvided * provided = component->GetInterfaceProvided(*it);
            if (provided) {
                mtsDescriptionInterfaceFullName desc;
                desc.ProcessName = arg.ProcessName;
                desc.ComponentName = arg.ComponentName;
                desc.InterfaceName = *it;
                desc.Tags = provided->GetTags();
                interfaces.Provided.push_back(desc);
            }
        }
    }
    else {
        CMN_LOG_CLASS_RUN_ERROR << "GetDescriptionsOfInterfaces: did not find component \""
                                << arg.ComponentName << "\"" << std::endl;
    }
}

void mtsManagerComponent::GetListOfConnections(std::vector <mtsDescriptionConnection> & listOfConnections) const
{
    mtsDescriptionConnection connection;

    ConnectionMapType::const_iterator it = ConnectionMap.begin();
    const ConnectionMapType::const_iterator itEnd = ConnectionMap.end();

    for (; it != itEnd; ++it) {
        // Check if this connection has been successfully established
        if (it->second.IsConnected())
            listOfConnections.push_back(it->second.GetDescriptionConnection());
    }
}

void mtsManagerComponent::GetListOfComponentClasses(const std::string &processName,
                          std::vector <mtsDescriptionComponentClass> & listOfComponentClasses) const
{
    // PK TODO: check processName

    // Loop through the class register, looking for components that can be created with one argument
    // or derived from mtsComponent (and have dynamic creation enabled).
    cmnClassRegister::const_iterator it = cmnClassRegister::begin();
    while (it != cmnClassRegister::end()) {
        if (it->second->OneArgConstructorAvailable()) {
            // CMN_DYNAMIC_CREATION_ONEARG or CMN_DYNAMIC_CREATION_SETNAME
            mtsDescriptionComponentClass classInfo;
            classInfo.ClassName = it->first;
            const cmnClassServicesBase *argServices = it->second->GetConstructorArgServices();
            if (argServices) {
                classInfo.ArgType = argServices->GetName();
                classInfo.ArgTypeId = argServices->TypeInfoPointer()->name();
            }
            listOfComponentClasses.push_back(classInfo);
        }
        else if (it->second->HasDynamicCreation() && it->second->IsDerivedFrom<mtsComponent>()) {
            // Backward compatibility (CMN_DYNAMIC_CREATION)
            mtsDescriptionComponentClass classInfo;
            classInfo.ClassName = it->first;
            classInfo.ArgType = "std::string";
            classInfo.ArgTypeId = typeid(std::string).name();
            listOfComponentClasses.push_back(classInfo);
        }
        it++;
    }
}

void mtsManagerComponent::GetInterfaceProvidedDescription(const mtsDescriptionInterface & intfc,
                                                          mtsInterfaceProvidedDescription & description) const
{
    if (intfc.InterfaceProvidedNames.size() < 1) {
        CMN_LOG_CLASS_RUN_ERROR << "GetInterfaceProvidedDescription: provided interface name not specified for component \""
                                << intfc.ComponentName << "\"" << std::endl;
        return;
    }

    // Get component specified
    mtsComponent * component = GetComponent(intfc.ComponentName);
    if (!component) {
        CMN_LOG_CLASS_RUN_ERROR << "GetInterfaceProvidedDescription: did not find component \""
                                << intfc.ComponentName << "\"" << std::endl;
        return;
    }

    // Get provided interface specified
    std::string interfaceName(intfc.InterfaceProvidedNames[0]);
    mtsInterfaceProvided * interfaceProvided = component->GetInterfaceProvided(interfaceName);
    if (!interfaceProvided) {
        CMN_LOG_CLASS_RUN_ERROR << "GetInterfaceProvidedDescription: no provided interface \""
                                << interfaceName << "\" found in component \"" << intfc.ComponentName << "\"" << std::endl;
        return;
    }

    // Extract complete information about all commands and event generators in
    // the provided interface specified. Argument prototypes are serialized.
    description.InterfaceName = interfaceName;
    if (!interfaceProvided->GetDescription(description)) {
        CMN_LOG_CLASS_RUN_ERROR << "GetInterfaceProvidedDescription: failed to get complete information of \""
                                << interfaceName << "\" found in component \"" << intfc.ComponentName << "\"" << std::endl;
        description.InterfaceName.clear();
    }
}

void mtsManagerComponent::GetInterfaceRequiredDescription(const mtsDescriptionInterface & intfc,
                                                          mtsInterfaceRequiredDescription & description) const
{
    if (intfc.InterfaceRequiredNames.size() < 1) {
        CMN_LOG_CLASS_RUN_ERROR << "GetInterfaceRequiredDescription: required interface name not specified for component \""
                                << intfc.ComponentName << "\"" << std::endl;
        return;
    }

    // Get the component instance specified
    mtsComponent * component = GetComponent(intfc.ComponentName);
    if (!component) {
        CMN_LOG_CLASS_RUN_ERROR << "GetInterfaceRequiredDescription: did not find component \""
                                << intfc.ComponentName << "\"" << std::endl;
        return;
    }

    // Get the required interface specified
    std::string interfaceName(intfc.InterfaceRequiredNames[0]);
    mtsInterfaceRequired * requiredInterface = component->GetInterfaceRequired(interfaceName);
    if (!requiredInterface) {
        CMN_LOG_CLASS_RUN_ERROR << "GetInterfaceRequiredDescription: no required interface \""
                                << interfaceName << "\" found in component \"" << intfc.ComponentName << "\"" << std::endl;
        return;
    }

    // Extract complete information about all functions and event handlers in
    // a required interface. Argument prototypes are fetched with serialization.
    description.InterfaceName = interfaceName;
    requiredInterface->GetDescription(description);
}

mtsComponent * mtsManagerComponent::GetComponent(const std::string &componentName) const
{
    return ComponentMap.GetItem(componentName);
}
 
//***************************************** System-wide Logging ********************************************

void mtsManagerComponent::LogInterface::PrintLog(const mtsLogMessage & log)
{
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

//********************************** Connect/Disconnect Internal *************************************************

ConnectionIDType mtsManagerComponent::GetConnectionID(const std::string & clientProcessName,
        const std::string & clientComponentName, const std::string & interfaceName) const
{
    ConnectionMapType::const_iterator it = ConnectionMap.begin();
    const ConnectionMapType::const_iterator itEnd = ConnectionMap.end();

    mtsDescriptionConnection description;
    for (; it != itEnd; ++it) {
        it->second.GetDescriptionConnection(description);
        if ((description.Client.ComponentName == clientComponentName) &&
            (description.Client.InterfaceName == interfaceName))
        {
            return description.ConnectionID;
        }
    }

    return InvalidConnectionID;
}

mtsConnection * mtsManagerComponent::GetConnectionInformation(const ConnectionIDType connectionID)
{
    const ConnectionMapType::iterator it = ConnectionMap.find(connectionID);
    if (it == ConnectionMap.end()) {
        return 0;
    } else {
        return &it->second;
    }
}

bool mtsManagerComponent::ConnectInternal(const std::string & clientComponentName, const std::string & clientInterfaceName,
                                          const std::string & serverComponentName, const std::string & serverInterfaceName)
{
    std::vector<std::string> options;
    std::stringstream allOptions;
    std::ostream_iterator< std::string > output(allOptions, ", ");

    mtsComponent * clientComponent = GetComponent(clientComponentName);
    if (!clientComponent) {
        CMN_LOG_CLASS_INIT_ERROR << "ComponentConnect: failed to get client component: \""
                                 << clientComponentName << "\"" << std::endl;
        return false;
    }

    mtsComponent * serverComponent = GetComponent(serverComponentName);
    if (!serverComponent) {
        CMN_LOG_CLASS_INIT_ERROR << "ComponentConnect: failed to get server component: \""
                                 << serverComponentName << "\"" << std::endl;
        return false;
    }

    // First, try to figure out if the interface is either provided or output
    mtsInterfaceProvided * serverInterfaceProvided = serverComponent->GetInterfaceProvided(serverInterfaceName);
    mtsInterfaceOutput * serverInterfaceOutput = serverComponent->GetInterfaceOutput(serverInterfaceName);
    if (!serverInterfaceProvided && !serverInterfaceOutput) {
        // test for swapped interfaces
        CMN_LOG_CLASS_INIT_DEBUG << "ComponentConnect: looking for provided/output interface in first component as well" << std::endl;
        serverInterfaceProvided = clientComponent->GetInterfaceProvided(clientInterfaceName);
        serverInterfaceOutput = clientComponent->GetInterfaceOutput(clientInterfaceName);
        if (!serverInterfaceProvided && !serverInterfaceOutput) {
            options = serverComponent->GetNamesOfInterfacesProvidedOrOutput();
            if (options.size() == 0) {
                allOptions << "no provided/output interface available for this component";
            } else {
                allOptions << "available provided/output interface(s): ";
                std::copy(options.begin(), options.end(), output);
            }
            return false;
        } else {
            CMN_LOG_CLASS_INIT_DEBUG << "ComponentConnect: Swapping client/server" << std::endl;
            return ConnectInternal(serverComponentName, serverInterfaceName,
                   clientComponentName, clientInterfaceName);
        }
    }

    // Now, handle the connections.  First, we look for connection between required/provided interfaces.  Then, we look
    // for connection between input/output interfaces.
    if (serverInterfaceProvided) {
        mtsInterfaceRequired * clientInterfaceRequired = clientComponent->GetInterfaceRequired(clientInterfaceName);
        if (!clientInterfaceRequired) {
            options = clientComponent->GetNamesOfInterfacesRequiredOrInput();
            if (options.size() == 0) {
                allOptions << "no required/input interface available for this component";
            } else {
                allOptions << "available required/input interface(s): ";
                std::copy(options.begin(), options.end(), output);
            }
            return false;
        }

        // Check if the required interface is already connected
        const mtsInterfaceProvided * interfaceConnected = clientInterfaceRequired->GetConnectedInterface();
        if (interfaceConnected) {
            interfaceConnected = interfaceConnected->GetOriginalInterface();
            if (interfaceConnected == serverInterfaceProvided) {
                CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to connect - already connected interfaces: \""
                                         << GetInterfaceUID("", clientComponentName, clientInterfaceName)
                                         << "\" - \""
                                         << GetInterfaceUID("", serverComponentName, serverInterfaceName)
                                         << "\"" << std::endl;
            }
            else {
                CMN_LOG_CLASS_INIT_ERROR << "Connect: failed to connect: \""
                                         << GetInterfaceUID("", clientComponentName, clientInterfaceName)
                                         << "\" - \""
                                         << GetInterfaceUID("", serverComponentName, serverInterfaceName)
                                         << " -- required interface already connected to: \""
                                         << GetInterfaceUID("", interfaceConnected->GetComponentName(), interfaceConnected->GetName())
                                         << "\"" << std::endl;
            }
            return false;
        }

        bool success = false;
        // If the server is this component (ManagerComponent), or if the server component is not active,
        // we can use the previous implementation (mtsInterfaceRequired::ConnectTo), which directly calls the methods.
        if ((serverComponentName == GetName()) ||
            !serverComponent->IsRunning()) {
            success = clientInterfaceRequired->ConnectTo(serverInterfaceProvided);
        }
        else {
            InterfaceComponentFunctionType * serverFunctionSet = InterfaceComponentFunctionMap.GetItem(serverComponentName);
            if (!serverFunctionSet) {
                CMN_LOG_CLASS_INIT_ERROR << "ComponentConnect: failed to connect interfaces: "
                                         << clientComponentName << ":" << clientInterfaceName << " - "
                                         << serverComponentName << ":" << serverInterfaceName
                                         << ", failed to get function set for " << serverComponentName << std::endl;
                return false;
            }
            if (!serverFunctionSet->GetEndUserInterface.IsValid()) {
                CMN_LOG_CLASS_RUN_ERROR << "ComponentConnect: GetEndUserInterface invalid for " << serverComponentName << std::endl;
                return false;
            }
            if (!serverFunctionSet->AddObserverList.IsValid()) {
                CMN_LOG_CLASS_RUN_ERROR << "ComponentConnect: AddObserverList invalid for " << serverComponentName << std::endl;
                return false;
            }
            mtsEndUserInterfaceArg endUserInterfaceArg(reinterpret_cast<size_t>(serverInterfaceProvided), clientInterfaceName, 0);

#if (CISST_OS == CISST_LINUX_XENOMAI && CISST_MTS_64BIT)
            {
                // See void mtsComponent::InterfaceInternalCommands_GetEndUserInterface()
                // 11/20/13: Patch for cisstDataGenerator
                //endUserInterfaceArg.EndUserInterface = endUserInterfaceArg.OriginalInterface->GetEndUserInterface(clientInterfaceName);
                mtsInterfaceProvided * originalInterface = reinterpret_cast<mtsInterfaceProvided*>(endUserInterfaceArg.OriginalInterface);
                CMN_ASSERT(originalInterface);
                endUserInterfaceArg.EndUserInterface =
                    reinterpret_cast<size_t>(originalInterface->GetEndUserInterface(clientInterfaceName));
            }
#else
            serverFunctionSet->GetEndUserInterface(endUserInterfaceArg, endUserInterfaceArg);
#endif

            mtsInterfaceProvided * endUserInterface = reinterpret_cast<mtsInterfaceProvided*>(endUserInterfaceArg.EndUserInterface);
            if (!endUserInterface) {
                CMN_LOG_CLASS_RUN_ERROR << "ComponentConnect: failed to get end-user interface for " << serverComponentName << std::endl;
                return false;
            }
            success = clientInterfaceRequired->BindCommands(endUserInterface);
            mtsEventHandlerList eventList(endUserInterface);
            clientInterfaceRequired->GetEventList(eventList);

#if (CISST_OS == CISST_LINUX_XENOMAI && CISST_MTS_64BIT)
            {
                // From void mtsInterfaceProvided::AddObserverList(const mtsEventHandlerList & argin, mtsEventHandlerList & argout)
                size_t i;
                for (i = 0; i < eventList.VoidEvents.size(); i++) {
                    eventList.VoidEvents[i].Result = eventList.Provided->AddObserver(eventList.VoidEvents[i].EventName, eventList.VoidEvents[i].HandlerPointer);
                }
                for (i = 0; i < eventList.WriteEvents.size(); i++) {
                    eventList.WriteEvents[i].Result = eventList.Provided->AddObserver(eventList.WriteEvents[i].EventName, eventList.WriteEvents[i].HandlerPointer);
                }
            }
#else
            serverFunctionSet->AddObserverList(eventList, eventList);
#endif
            if (!clientInterfaceRequired->CheckEventList(eventList))
                success = false;
        }
        if (success) {
            CMN_LOG_CLASS_INIT_VERBOSE << "ComponentConnect: successfully connected required/provided: "
                                       << clientComponentName << ":" << clientInterfaceName << " - "
                                       << serverComponentName << ":" << serverInterfaceName << std::endl;
        } else {
            CMN_LOG_CLASS_INIT_ERROR << "ComponentConnect: failed to connect required/provided: "
                                     << clientComponentName << ":" << clientInterfaceName << " - "
                                     << serverComponentName << ":" << serverInterfaceName << std::endl;
        }
    }
    else {  // Input/Output connection
        if (!serverInterfaceOutput) {
            CMN_LOG_CLASS_INIT_ERROR << "ComponentConnect: failed to connect interfaces: "
                                     << clientComponentName << ":" << clientInterfaceName << " - "
                                     << serverComponentName << ":" << serverInterfaceName
                                     << ", server does not have provided or output interface." << std::endl;
            return false;
        }
        mtsInterfaceInput * clientInterfaceInput = clientComponent->GetInterfaceInput(clientInterfaceName);
        if (!clientInterfaceInput) {
            CMN_LOG_CLASS_INIT_ERROR << "ComponentConnect: failed to connect interfaces: "
                                     << clientComponentName << ":" << clientInterfaceName << " - "
                                     << serverComponentName << ":" << serverInterfaceName
                                     << ", client does not have input interface." << std::endl;
            return false;
        }
        if (clientInterfaceInput->ConnectTo(serverInterfaceOutput)) {
            CMN_LOG_CLASS_INIT_VERBOSE << "ComponentConnect: component \""
                                       << this->GetName()
                                       << "\" input interface \"" << clientInterfaceName
                                       << "\" successfully connected to output interface \""
                                       << serverInterfaceName << "\"" << std::endl;
        } else {
            CMN_LOG_CLASS_INIT_ERROR << "ComponentConnect: component \""
                                     << this->GetName()
                                     << "\" input interface \"" << clientInterfaceName
                                     << "\" failed to connect to output interface \""
                                     << serverInterfaceName << "\"" << std::endl;
            return false;
        }
    }

    // Successful connection, now update data structures

    // Assign new connection id
    ConnectionIDType thisConnectionID = ConnectionID;

    mtsDescriptionConnection description(
        "", clientComponentName, clientInterfaceName,
        "", serverComponentName, serverInterfaceName,
        thisConnectionID);
    mtsConnection connection(description);

    ConnectionMap.insert(std::make_pair(thisConnectionID, connection));

    // Increase connection id
    if (ConnectionID + 1 == InvalidConnectionID) {
        CMN_LOG_CLASS_INIT_ERROR << "Connect: connection id approached its upper limit: " << InvalidConnectionID << std::endl;
    } else {
        ++ConnectionID;
    }
    CMN_LOG_CLASS_RUN_VERBOSE << "ComponentConnect: successfully connected: " << description << std::endl;
    // Generate the event
    InterfaceComponentEvents_AddConnection(description);
    return true;
}

bool mtsManagerComponent::DisconnectInternal(const std::string & clientComponentName, const std::string & clientInterfaceName,
                                             const std::string & serverComponentName, const std::string & serverInterfaceName)
{
    mtsComponent * clientComponent = GetComponent(clientComponentName);
    if (!clientComponent) {
        CMN_LOG_CLASS_INIT_ERROR << "ComponentDisconnect: failed to get client component: \"" << clientComponentName << "\"" << std::endl;
        return false;
    }

    mtsComponent * serverComponent = GetComponent(serverComponentName);
    if (!serverComponent) {
        CMN_LOG_CLASS_INIT_ERROR << "ComponentDisconnect: failed to get server component: \"" << serverComponentName << "\"" << std::endl;
        return false;
    }

    mtsInterfaceProvided * serverInterfaceProvided = serverComponent->GetInterfaceProvided(serverInterfaceName);
    mtsInterfaceOutput * serverInterfaceOutput = serverComponent->GetInterfaceOutput(serverInterfaceName);
    if (!serverInterfaceProvided && !serverInterfaceOutput) {
        // test for swapped interfaces
        CMN_LOG_CLASS_INIT_DEBUG << "ComponentDisconnect: looking for provided/output interface in first component as well" << std::endl;
        serverInterfaceProvided = clientComponent->GetInterfaceProvided(clientInterfaceName);
        serverInterfaceOutput = clientComponent->GetInterfaceOutput(clientInterfaceName);
        if (!serverInterfaceProvided && !serverInterfaceOutput) {
            CMN_LOG_CLASS_INIT_ERROR << "ComponentDisconnect: failed to get provided/output interface \"" << serverInterfaceName << "\""
                                     << " in component \"" << serverComponentName << "\"" << std::endl;
            return false;
        } else {
            CMN_LOG_CLASS_INIT_DEBUG << "ComponentDisconnect: Swapping client/server" << std::endl;
            return DisconnectInternal(serverComponentName, serverInterfaceName, clientComponentName, clientInterfaceName);
        }
    }

    // Now, handle the disconnection.  First, we look for connection between required/provided interfaces.  Then, we look
    // for connection between input/output interfaces.
    if (serverInterfaceProvided) {
        mtsInterfaceRequired *clientInterfaceRequired = clientComponent->GetInterfaceRequired(clientInterfaceName);
        if (!clientInterfaceRequired) {
            CMN_LOG_CLASS_RUN_ERROR << "ComponentDisconnect: failed to disconnect interfaces: "
                                     << clientComponentName << ":" << clientInterfaceName << " - "
                                     << serverComponentName << ":" << serverInterfaceName
                                     << ", client does not have required interface." << std::endl;
            return false;
        }
        // Following check should not be necessary -- serverInterfaceProvided should always be the original interface
        // because we obtained it via a string query.
        if (serverInterfaceProvided->GetOriginalInterface()) {
            CMN_LOG_CLASS_RUN_ERROR << "ComponentDisconnect: failed to disconnect interfaces: "
                                     << clientComponentName << ":" << clientInterfaceName << " - "
                                     << serverComponentName << ":" << serverInterfaceName
                                     << ", did not get original interface." << std::endl;
            return false;
        }
        // Now, get the end-user interface for this client
        mtsInterfaceProvided *endUserInterface = serverInterfaceProvided->FindEndUserInterfaceByName(clientInterfaceName);
        if (!endUserInterface) {
            CMN_LOG_CLASS_RUN_ERROR << "ComponentDisconnect: failed to disconnect interfaces: "
                                     << clientComponentName << ":" << clientInterfaceName << " - "
                                     << serverComponentName << ":" << serverInterfaceName
                                     << ", could not find end-user interface." << std::endl;
            return false;
        }
        bool success = false;
        // If the server is this component (ManagerComponent), or if this component is not active,
        // we can directly call the methods. Note that we could use the StateChange mutex to make sure that the state
        // does not change during execution of this method, but that is unlikely.
        if ((serverComponentName == GetName()) || !IsRunning()) {
            mtsEventHandlerList eventList(serverInterfaceProvided);
            clientInterfaceRequired->GetEventList(eventList);
            serverInterfaceProvided->RemoveObserverList(eventList, eventList);
            success = clientInterfaceRequired->CheckEventList(eventList);
            // Now, pause/stop the client component.  In the future, the component could be left
            // running if the required interface is MTS_OPTIONAL.
            clientComponent->Suspend();
            clientInterfaceRequired->DetachCommands();
            if (serverInterfaceProvided->RemoveEndUserInterface(endUserInterface, clientInterfaceName) != 0)
                success = false;
        }
        else {
            InterfaceComponentFunctionType * serverFunctionSet = InterfaceComponentFunctionMap.GetItem(serverComponentName);
            if (!serverFunctionSet) {
                CMN_LOG_CLASS_RUN_ERROR << "ComponentDisconnect: failed to get function set for " << serverComponentName << std::endl;
                return false;
            }
            if (!serverFunctionSet->RemoveEndUserInterface.IsValid()) {
                CMN_LOG_CLASS_RUN_ERROR << "ComponentDisconnect: RemoveEndUserInterface invalid for " << serverComponentName << std::endl;
                return false;
            }
            if (!serverFunctionSet->RemoveObserverList.IsValid()) {
                CMN_LOG_CLASS_RUN_ERROR << "ComponentDisconnect: RemoveObserverList invalid for " << serverComponentName << std::endl;
                return false;
            }
#if 0
            if (!serverFunctionSet->ComponentStop.IsValid()) {
                CMN_LOG_CLASS_RUN_ERROR << "ComponentDisconnect: ComponentStop invalid for " << serverComponentName << std::endl;
                return false;
            }
#endif
            mtsEventHandlerList eventList(serverInterfaceProvided);
            clientInterfaceRequired->GetEventList(eventList);
            serverFunctionSet->RemoveObserverList(eventList, eventList);
            success = clientInterfaceRequired->CheckEventList(eventList);
            // Now, pause/stop the client component.  In the future, the component could be left
            // running if the required interface is MTS_OPTIONAL.
            clientComponent->Suspend(); // Could instead use serverFunctionSet->ComponentStop
            clientInterfaceRequired->DetachCommands();
            mtsEndUserInterfaceArg endUserInterfaceArg(reinterpret_cast<size_t>(serverInterfaceProvided),
                                                       clientInterfaceName,
                                                       reinterpret_cast<size_t>(endUserInterface));
            serverFunctionSet->RemoveEndUserInterface(endUserInterfaceArg, endUserInterfaceArg);
            if (endUserInterfaceArg.EndUserInterface != 0) {
                CMN_LOG_CLASS_RUN_WARNING << "ComponentDisconnect: failed to remove end-user interface for " << serverComponentName << std::endl;
                success = false;
            }
        }
        if (success) {
            CMN_LOG_CLASS_INIT_VERBOSE << "ComponentDisconnect: successfully disconnected required/provided: "
                                       << clientComponentName << ":" << clientInterfaceName << " - "
                                       << serverComponentName << ":" << serverInterfaceName << std::endl;
        } else {
            CMN_LOG_CLASS_INIT_ERROR << "ComponentDisconnect: failed to disconnect required/provided: "
                                     << clientComponentName << ":" << clientInterfaceName << " - "
                                     << serverComponentName << ":" << serverInterfaceName << std::endl;
            return false;
        }
    }
    else {  // Input/Output connection
        if (!serverInterfaceOutput) {
            CMN_LOG_CLASS_INIT_ERROR << "ComponentDisconnect: failed to disconnect interfaces: "
                                     << clientComponentName << ":" << clientInterfaceName << " - "
                                     << serverComponentName << ":" << serverInterfaceName
                                     << ", server does not have provided or output interface." << std::endl;
            return false;
        }
        mtsInterfaceInput *clientInterfaceInput = clientComponent->GetInterfaceInput(clientInterfaceName);
        if (!clientInterfaceInput) {
            CMN_LOG_CLASS_INIT_ERROR << "ComponentDisconnect: failed to disconnect interfaces: "
                                     << clientComponentName << ":" << clientInterfaceName << " - "
                                     << serverComponentName << ":" << serverInterfaceName
                                     << ", client does not have input interface." << std::endl;
            return false;
        }
        if (clientInterfaceInput->Disconnect()) {
            CMN_LOG_CLASS_INIT_VERBOSE << "ComponentDisconnect: component \""
                                       << this->GetName()
                                       << "\" input interface \"" << clientInterfaceName
                                       << "\" successfully disconnected from output interface \""
                                       << serverInterfaceName << "\"" << std::endl;
        } else {
            CMN_LOG_CLASS_INIT_ERROR << "ComponentDisconnect: component \""
                                     << this->GetName()
                                     << "\" input interface \"" << clientInterfaceName
                                     << "\" failed to disconnect from output interface \""
                                     << serverInterfaceName << "\"" << std::endl;
            return false;
        }
    }

    // Finally, remove from ConnectionMap
    ConnectionIDType id = GetConnectionID("", clientComponentName, clientInterfaceName);
    if (id == InvalidConnectionID) {
        CMN_LOG_CLASS_INIT_ERROR << "Disconnect: no connection id found for "
            << "\"" << GetInterfaceUID("", clientComponentName, clientInterfaceName) << "\" - "
            << "\"" << GetInterfaceUID("", serverComponentName, serverInterfaceName) << std::endl;
        return false;
    }
    ConnectionMapType::iterator itConnectionMap = ConnectionMap.find(id);
    mtsDescriptionConnection description = itConnectionMap->second.GetDescriptionConnection();
    ConnectionMap.erase(itConnectionMap);

    // Generate the event
    InterfaceComponentEvents_RemoveConnection(description);
    return true;
}

//**************************************************************************************************************************

bool mtsManagerComponent::AddNewClientComponent(const std::string & clientComponentName)
{
    // Create a new set of function objects
    InterfaceComponentFunctionType * newFunctionSet = new InterfaceComponentFunctionType;

    const std::string interfaceName = mtsManagerComponentBase::GetNameOfInterfaceComponentRequiredFor(clientComponentName);
    mtsInterfaceRequired * required = AddInterfaceRequired(interfaceName);
    if (!required) {
        CMN_LOG_CLASS_INIT_ERROR << "AddNewClientComponent: failed to create \"Component\" required interface: " << interfaceName << std::endl;
        return false;
    }
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentStop,
                          newFunctionSet->ComponentStop);
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentResume,
                          newFunctionSet->ComponentResume);
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentGetState,
                          newFunctionSet->ComponentGetState);
    required->AddFunction(mtsManagerComponentBase::CommandNames::GetEndUserInterface,
                          newFunctionSet->GetEndUserInterface);
    required->AddFunction(mtsManagerComponentBase::CommandNames::AddObserverList,
                          newFunctionSet->AddObserverList);
    required->AddFunction(mtsManagerComponentBase::CommandNames::RemoveEndUserInterface,
                          newFunctionSet->RemoveEndUserInterface);
    required->AddFunction(mtsManagerComponentBase::CommandNames::RemoveObserverList,
                          newFunctionSet->RemoveObserverList);
    required->AddFunction(mtsManagerComponentBase::CommandNames::ComponentStart,
                          newFunctionSet->ComponentStartOther);
    required->AddEventHandlerWrite(&mtsManagerComponent::HandleChangeStateFromComponent, this,
                                   mtsManagerComponentBase::EventNames::ChangeState);

    // Remember a required interface (InterfaceComponent's required interface)
    // to connect it to the provided interface (InterfaceInternals's provided
    // interface).
    if (!InterfaceComponentFunctionMap.AddItem(clientComponentName, newFunctionSet)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddNewClientComponent: failed to add \"Component\" required interface: "
            << "\"" << clientComponentName << "\", " << interfaceName << std::endl;
        return false;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "AddNewClientComponent: successfully added new client component: "
        << clientComponentName << std::endl;

    return true;
}

void mtsManagerComponent::HandleChangeStateFromComponent(const mtsComponentStateChange &stateChange)
{
    CMN_LOG_CLASS_RUN_VERBOSE << "MCS got ChangeState event for " << stateChange.ComponentName << std::endl;
    InterfaceComponentEvents_ChangeState(stateChange);
}

bool mtsManagerComponent::ConnectToManagerComponent(const std::string & componentName)
{
    mtsComponent * component = GetComponent(componentName);
    if (!component) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectToManagerComponent: no component found with name of "
            << "\"" << componentName << "\"" << std::endl;
        return false;
    }

    // Connect InterfaceComponent's required interface to InterfaceInternal's
    // provided interface of the connecting component.
    const std::string nameOfInterfaceComponentRequired
        = mtsManagerComponentBase::GetNameOfInterfaceComponentRequiredFor(componentName);
    if (!ConnectInternal(GetName(), nameOfInterfaceComponentRequired,
                         componentName, mtsManagerComponentBase::GetNameOfInterfaceInternalProvided())) {
        CMN_LOG_CLASS_INIT_ERROR << "ConnectToManagerComponent: failed to connect: "
                                 << GetName() << ":" << nameOfInterfaceComponentRequired
                                 << " - "
                                 << componentName << ":" << mtsManagerComponentBase::GetNameOfInterfaceInternalProvided()
                                 << std::endl;
        return false;
    }

    // If a component has support for the dynamic component control services,
    // connect InterfaceInternal's required interface to InterfaceComponent's
    // provided interface.
    if (component->GetInterfaceRequired(mtsManagerComponentBase::GetNameOfInterfaceInternalRequired())) {
        if (!ConnectInternal(component->GetName(), mtsManagerComponentBase::GetNameOfInterfaceInternalRequired(),
                             GetName(), mtsManagerComponentBase::GetNameOfInterfaceComponentProvided())) {
            CMN_LOG_CLASS_INIT_ERROR << "ConnectToManagerComponent: failed to connect: "
                                     << component->GetName() << ":" << mtsManagerComponentBase::GetNameOfInterfaceInternalRequired()
                                     << " - "
                                     << GetName() << ":" << mtsManagerComponentBase::GetNameOfInterfaceComponentProvided()
                                     << std::endl;
            return false;
        }
    }

    return true;
}

bool mtsManagerComponent::DisconnectFromManagerComponent(const std::string & componentName)
{
    mtsComponent * component = ComponentMap.GetItem(componentName);
    if (!component) {
        CMN_LOG_CLASS_RUN_WARNING << "DisconnectFromManagerComponent: no component found with name of "
            << "\"" << componentName << "\"" << std::endl;
        return false;
    }

    // First remove internal connection between MCS and component
    const std::string nameOfInterfaceComponentRequired
                      = mtsManagerComponentBase::GetNameOfInterfaceComponentRequiredFor(componentName);
    if (!DisconnectInternal(GetName(), nameOfInterfaceComponentRequired,
                            componentName, mtsManagerComponentBase::GetNameOfInterfaceInternalProvided())) {
        CMN_LOG_CLASS_RUN_WARNING << "DisconnectFromManagerComponent: failed to disconnect: "
                                  << GetName() << ":" << nameOfInterfaceComponentRequired
                                  << " - "
                                  << componentName << ":" << mtsManagerComponentBase::GetNameOfInterfaceInternalProvided()
                                  << std::endl;
    }
    // Next, remove the internal required interface
    RemoveInterfaceRequired(nameOfInterfaceComponentRequired);

    // Next, remove the function set
    InterfaceComponentFunctionType * functionSet = InterfaceComponentFunctionMap.GetItem(componentName);
    if (functionSet) {
        InterfaceComponentFunctionMap.RemoveItem(componentName);
        delete functionSet;
    }
    else {
        CMN_LOG_CLASS_RUN_WARNING << "DisconnectFromManagerComponent: failed to get function set for component \""
                                  << componentName << "\"" << std::endl;
    }

    // If the component has support for dynamic component control services,
    // disconnect InterfaceInternal's required interface from InterfaceComponent's
    // provided interface.
    if (component->GetInterfaceRequired(mtsManagerComponentBase::GetNameOfInterfaceInternalRequired())) {
        if (!DisconnectInternal(component->GetName(), mtsManagerComponentBase::GetNameOfInterfaceInternalRequired(),
                                GetName(), mtsManagerComponentBase::GetNameOfInterfaceComponentProvided())) {
            CMN_LOG_CLASS_RUN_WARNING << "DisconnectFromManagerComponent: failed to connect: "
                                      << component->GetName() << ":" << mtsManagerComponentBase::GetNameOfInterfaceInternalRequired()
                                      << " - "
                                      << GetName() << ":" << mtsManagerComponentBase::GetNameOfInterfaceComponentProvided()
                                      << std::endl;
        }
    }
    // For now, return true because failure to disconnect an interface is not critical
    return true;
}
