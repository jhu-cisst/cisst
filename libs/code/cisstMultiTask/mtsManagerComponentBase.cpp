/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet, Min Yang Jung
  Created on: 2010-08-29

  (C) Copyright 2010-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsManagerComponentBase.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>

// Names of components
//const std::string mtsManagerComponentBase::ComponentNames::ManagerComponentServer = "MNGR-COMP-SERVER";
//const std::string mtsManagerComponentBase::ComponentNames::ManagerComponentClientSuffix = "_MNGR-COMP-CLIENT";
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
// Names of commands
const std::string mtsManagerComponentBase::CommandNames::ComponentCreate  = "ComponentCreate";
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
const std::string mtsManagerComponentBase::CommandNames::GetInterfaceProvidedDescription = "GetInterfaceProvidedDescription";
const std::string mtsManagerComponentBase::CommandNames::GetInterfaceRequiredDescription = "GetInterfaceRequiredDescription";
const std::string mtsManagerComponentBase::CommandNames::GetEndUserInterface = "GetEndUserInterface";
const std::string mtsManagerComponentBase::CommandNames::AddObserverList = "AddObserverList";
const std::string mtsManagerComponentBase::CommandNames::RemoveEndUserInterface = "RemoveEndUserInterface";
const std::string mtsManagerComponentBase::CommandNames::RemoveObserverList = "RemoveObserverList";

// Names of events
const std::string mtsManagerComponentBase::EventNames::AddComponent  = "AddComponentEvent";
const std::string mtsManagerComponentBase::EventNames::AddConnection = "AddConnectionEvent";
const std::string mtsManagerComponentBase::EventNames::RemoveConnection = "RemoveConnectionEvent";
const std::string mtsManagerComponentBase::EventNames::ChangeState   = "ChangeState";

CMN_IMPLEMENT_SERVICES(mtsManagerComponentBase);

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
   return (componentName == mtsManagerComponentBase::ComponentNames::ManagerComponentServer);
}

bool mtsManagerComponentBase::IsManagerComponentClient(const std::string & componentName)
{
    static const std::string suffix = mtsManagerComponentBase::ComponentNames::ManagerComponentClientSuffix;

    // MJ TEMP: special handling if componentName ends with "-MCC"
    return (std::string::npos != componentName.find(suffix, componentName.length() - suffix.size()));
}
