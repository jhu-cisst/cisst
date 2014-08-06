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

#include <cisstMultiTask/mtsManagerComponentBase.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>

// MJ: If naming convention changes, all the check and getter methods should be updated accordingly.
// Names of components
const std::string mtsManagerComponentBase::ComponentNames::ManagerComponentServer = "MCS";
const std::string mtsManagerComponentBase::ComponentNames::ManagerComponentClientSuffix = "_MCC";
// Names of interfaces
const std::string mtsManagerComponentBase::InterfaceNames::InterfaceInternalProvided  = "InterfaceInternalProvided";
const std::string mtsManagerComponentBase::InterfaceNames::InterfaceInternalRequired  = "InterfaceInternalRequired";
const std::string mtsManagerComponentBase::InterfaceNames::InterfaceComponentProvided = "InterfaceComponentProvided";
const std::string mtsManagerComponentBase::InterfaceNames::InterfaceComponentRequired = "InterfaceComponentRequired";
const std::string mtsManagerComponentBase::InterfaceNames::InterfaceLCMProvided       = "InterfaceLCMProvided";
const std::string mtsManagerComponentBase::InterfaceNames::InterfaceLCMRequired       = "InterfaceLCMRequired";
const std::string mtsManagerComponentBase::InterfaceNames::InterfaceGCMProvided       = "InterfaceGCMProvided";
const std::string mtsManagerComponentBase::InterfaceNames::InterfaceGCMRequired       = "InterfaceGCMRequired";
const std::string mtsManagerComponentBase::InterfaceNames::InterfaceSystemLoggerProvided = "InterfaceSystemLoggerProvided";
const std::string mtsManagerComponentBase::InterfaceNames::InterfaceSystemLoggerRequired = "InterfaceSystemLoggerRequired";
const std::string mtsManagerComponentBase::InterfaceNames::InterfaceExecIn = "ExecIn";
const std::string mtsManagerComponentBase::InterfaceNames::InterfaceExecOut = "ExecOut";
// Names of commands
const std::string mtsManagerComponentBase::CommandNames::ComponentCreate  = "ComponentCreate";
const std::string mtsManagerComponentBase::CommandNames::ComponentConfigure  = "ComponentConfigure";
const std::string mtsManagerComponentBase::CommandNames::ComponentConnect = "Connect";
const std::string mtsManagerComponentBase::CommandNames::ComponentDisconnect = "Disconnect";
const std::string mtsManagerComponentBase::CommandNames::ComponentStart   = "ComponentStart";
const std::string mtsManagerComponentBase::CommandNames::ComponentStop    = "ComponentStop";
const std::string mtsManagerComponentBase::CommandNames::ComponentResume  = "ComponentResume";
const std::string mtsManagerComponentBase::CommandNames::ComponentGetState = "ComponentGetState";
const std::string mtsManagerComponentBase::CommandNames::GetNamesOfProcesses  = "GetNamesOfProcesses";
const std::string mtsManagerComponentBase::CommandNames::GetNamesOfComponents = "GetNamesOfComponents";
const std::string mtsManagerComponentBase::CommandNames::GetNamesOfInterfaces = "GetNamesOfInterfaces";
const std::string mtsManagerComponentBase::CommandNames::GetListOfConnections = "GetListOfConnections";
const std::string mtsManagerComponentBase::CommandNames::GetListOfComponentClasses = "GetListOfComponentClasses";
const std::string mtsManagerComponentBase::CommandNames::GetInterfaceProvidedDescription = "GetInterfaceProvidedDescription";
const std::string mtsManagerComponentBase::CommandNames::GetInterfaceRequiredDescription = "GetInterfaceRequiredDescription";
const std::string mtsManagerComponentBase::CommandNames::GetEndUserInterface = "GetEndUserInterface";
const std::string mtsManagerComponentBase::CommandNames::AddObserverList = "AddObserverList";
const std::string mtsManagerComponentBase::CommandNames::RemoveEndUserInterface = "RemoveEndUserInterface";
const std::string mtsManagerComponentBase::CommandNames::RemoveObserverList = "RemoveObserverList";
const std::string mtsManagerComponentBase::CommandNames::LoadLibrary = "LoadLibrary";
const std::string mtsManagerComponentBase::CommandNames::PrintLog = "PrintLog";
const std::string mtsManagerComponentBase::CommandNames::SetLogForwarding = "SetLogForwarding";
const std::string mtsManagerComponentBase::CommandNames::GetLogForwardingState = "GetLogForwardingState";
const std::string mtsManagerComponentBase::CommandNames::GetLogForwardingStates = "GetLogForwardingStates";
const std::string mtsManagerComponentBase::CommandNames::EnableLogForwarding = "EnableLogForwarding";
const std::string mtsManagerComponentBase::CommandNames::DisableLogForwarding = "DisableLogForwarding";
const std::string mtsManagerComponentBase::CommandNames::GetAbsoluteTimeInSeconds = "GetAbsoluteTimeInSeconds";
const std::string mtsManagerComponentBase::CommandNames::GetAbsoluteTimeDiffs = "GetAbsoluteTimeDiffs";

// Names of events
const std::string mtsManagerComponentBase::EventNames::AddComponent  = "AddComponentEvent";
const std::string mtsManagerComponentBase::EventNames::AddConnection = "AddConnectionEvent";
const std::string mtsManagerComponentBase::EventNames::RemoveConnection = "RemoveConnectionEvent";
const std::string mtsManagerComponentBase::EventNames::ChangeState   = "ChangeState";
const std::string mtsManagerComponentBase::EventNames::MCSReady      = "MCSReady";
const std::string mtsManagerComponentBase::EventNames::PrintLog      = "PrintLog";

CMN_IMPLEMENT_SERVICES_DERIVED(mtsManagerComponentBase, mtsTaskFromSignal);

mtsManagerComponentBase::mtsManagerComponentBase(const std::string & componentName)
    : mtsTaskFromSignal(componentName, 50)
{
}

mtsManagerComponentBase::~mtsManagerComponentBase()
{
}

void mtsManagerComponentBase::Run(void)
{
    ProcessQueuedCommands();
    ProcessQueuedEvents();
}

void mtsManagerComponentBase::Cleanup(void)
{
}

bool mtsManagerComponentBase::IsManagerComponentServer(const std::string & componentName)
{
   return (componentName == ComponentNames::ManagerComponentServer);
}

bool mtsManagerComponentBase::IsManagerComponentClient(const std::string & componentName)
{
    const std::string suffix = ComponentNames::ManagerComponentClientSuffix;

    // MJ TEMP: special handling if componentName ends with "-MCC"
    return (std::string::npos != componentName.find(suffix, componentName.length() - suffix.size()));
}

const std::string mtsManagerComponentBase::GetNameOfManagerComponentServer(void)
{
    return ComponentNames::ManagerComponentServer;
}

const std::string mtsManagerComponentBase::GetNameOfManagerComponentClientFor(const std::string & processName)
{
    std::string componentName(processName);
    componentName += ComponentNames::ManagerComponentClientSuffix;

    return componentName;
}

bool mtsManagerComponentBase::IsNameOfInterfaceGCMRequired(const std::string & interfaceName)
{
    const std::string prefix = GetNameOfInterfaceGCMRequiredFor("");

    return (interfaceName.substr(0, prefix.size()) == prefix);
}

bool mtsManagerComponentBase::IsNameOfInterfaceGCMProvided(const std::string & interfaceName)
{
    return (interfaceName.substr(0, mtsManagerComponentBase::InterfaceNames::InterfaceGCMProvided.size())
            == GetNameOfInterfaceGCMProvided());
}

bool mtsManagerComponentBase::IsNameOfInterfaceLCMRequired(const std::string & interfaceName)
{
    return (interfaceName == GetNameOfInterfaceLCMRequired());
}

bool mtsManagerComponentBase::IsNameOfInterfaceLCMProvided(const std::string & interfaceName)
{
    return (interfaceName.substr(0, mtsManagerComponentBase::InterfaceNames::InterfaceLCMProvided.size())
            == GetNameOfInterfaceLCMProvided());
}

bool mtsManagerComponentBase::IsNameOfInterfaceComponentRequired(const std::string & interfaceName)
{
    const std::string prefix = GetNameOfInterfaceComponentRequiredFor("");

    return (interfaceName.substr(0, prefix.size()) == prefix);
}

bool mtsManagerComponentBase::IsNameOfInterfaceComponentProvided(const std::string & interfaceName)
{
    return (interfaceName.substr(0, mtsManagerComponentBase::InterfaceNames::InterfaceComponentProvided.size())
            == GetNameOfInterfaceComponentProvided());
}

bool mtsManagerComponentBase::IsNameOfInterfaceInternalRequired(const std::string & interfaceName)
{
    return (interfaceName == GetNameOfInterfaceInternalRequired());
}

bool mtsManagerComponentBase::IsNameOfInterfaceInternalProvided(const std::string & interfaceName)
{
    return (interfaceName.substr(0, mtsManagerComponentBase::InterfaceNames::InterfaceInternalProvided.size())
            == GetNameOfInterfaceInternalProvided());
}

const std::string mtsManagerComponentBase::GetNameOfInterfaceGCMRequiredFor(const std::string & processName)
{
    std::string interfaceName = InterfaceNames::InterfaceGCMRequired;
    interfaceName += "For";
    interfaceName += processName;

    return interfaceName;
}

const std::string mtsManagerComponentBase::GetNameOfInterfaceGCMProvided(void)
{
    return InterfaceNames::InterfaceGCMProvided;
}

const std::string mtsManagerComponentBase::GetNameOfInterfaceLCMRequired(void)
{
    return InterfaceNames::InterfaceLCMRequired;
}

const std::string mtsManagerComponentBase::GetNameOfInterfaceLCMProvided(void)
{
    return InterfaceNames::InterfaceLCMProvided;
}

const std::string mtsManagerComponentBase::GetNameOfInterfaceComponentRequiredFor(const std::string & componentName)
{
    std::string interfaceName = InterfaceNames::InterfaceComponentRequired;
    interfaceName += "For";
    interfaceName += componentName;

    return interfaceName;
}

const std::string mtsManagerComponentBase::GetNameOfInterfaceComponentProvided(void)
{
    return InterfaceNames::InterfaceComponentProvided;
}

const std::string mtsManagerComponentBase::GetNameOfInterfaceInternalRequired(void)
{
    return InterfaceNames::InterfaceInternalRequired;
}

const std::string mtsManagerComponentBase::GetNameOfInterfaceInternalProvided(void)
{
    return InterfaceNames::InterfaceInternalProvided;
}

bool mtsManagerComponentBase::IsManagerComponent(const std::string & componentName)
{
    if (IsManagerComponentServer(componentName)) return true;
    if (IsManagerComponentClient(componentName)) return true;

    return false;
}

bool mtsManagerComponentBase::IsNameOfInternalInterface(const std::string & interfaceName)
{
    if (IsNameOfInterfaceGCMRequired(interfaceName)) return true;
    if (IsNameOfInterfaceGCMProvided(interfaceName)) return true;
    if (IsNameOfInterfaceLCMRequired(interfaceName)) return true;
    if (IsNameOfInterfaceLCMProvided(interfaceName)) return true;
    if (IsNameOfInterfaceComponentRequired(interfaceName)) return true;
    if (IsNameOfInterfaceComponentProvided(interfaceName)) return true;
    if (IsNameOfInterfaceInternalRequired(interfaceName)) return true;
    if (IsNameOfInterfaceInternalProvided(interfaceName)) return true;

    return false;
}
