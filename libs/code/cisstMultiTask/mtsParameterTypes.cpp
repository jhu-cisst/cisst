/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet, Min Yang Jung
  Created on: 2010-09-01

  (C) Copyright 2010-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsParameterTypes.h>
#include <cisstMultiTask/mtsManagerGlobal.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

//-----------------------------------------------------------------------------
//  Component state (mtsComponentState)
//
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsComponentStateProxy);

//-----------------------------------------------------------------------------
//  InterfaceProvidedDescription (defined in mtsInterfaceCommon.h)
//  InterfaceRequiredDescription (defined in mtsInterfaceCommon.h)
//
CMN_IMPLEMENT_SERVICES_TEMPLATED(InterfaceProvidedDescriptionProxy);
CMN_IMPLEMENT_SERVICES_TEMPLATED(InterfaceRequiredDescriptionProxy);

//-----------------------------------------------------------------------------
//  Component Description
//
CMN_IMPLEMENT_SERVICES(mtsDescriptionComponent);

mtsDescriptionComponent::mtsDescriptionComponent(const mtsDescriptionComponent & other):
    mtsGenericObject(other)
{
    this->ProcessName = other.ProcessName;
    this->ComponentName = other.ComponentName;
    this->ClassName = other.ClassName;
}

mtsDescriptionComponent::mtsDescriptionComponent(const std::string & processName, const std::string & componentName):
    mtsGenericObject(),
    ProcessName(processName),
    ComponentName(componentName)
{
}

void mtsDescriptionComponent::ToStream(std::ostream & outputStream) const
{
    mtsGenericObject::ToStream(outputStream);
    outputStream << ", Process: " << this->ProcessName
                 << ", Class: " << this->ClassName
                 << ", Name: " << this->ComponentName;
}

void mtsDescriptionComponent::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream, this->ProcessName);
    cmnSerializeRaw(outputStream, this->ClassName);
    cmnSerializeRaw(outputStream, this->ComponentName);
}

void mtsDescriptionComponent::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream, this->ProcessName);
    cmnDeSerializeRaw(inputStream, this->ClassName);
    cmnDeSerializeRaw(inputStream, this->ComponentName);
}


//-----------------------------------------------------------------------------
//  Component Class Description
//
CMN_IMPLEMENT_SERVICES(mtsDescriptionComponentClass);

mtsDescriptionComponentClass::mtsDescriptionComponentClass(const mtsDescriptionComponentClass & other):
    mtsGenericObject(other)
{
    this->ClassName = other.ClassName;
    this->ArgType = other.ArgType;
    this->ArgTypeId = other.ArgTypeId;
}

void mtsDescriptionComponentClass::ToStream(std::ostream & outputStream) const
{
    mtsGenericObject::ToStream(outputStream);
    outputStream << ", Class: " << this->ClassName
                 << ", ArgType: " << this->ArgType
                 << ", ArgTypeId: " << this->ArgTypeId;
}

void mtsDescriptionComponentClass::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream, this->ClassName);
    cmnSerializeRaw(outputStream, this->ArgType);
    cmnSerializeRaw(outputStream, this->ArgTypeId);
}

void mtsDescriptionComponentClass::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream, this->ClassName);
    cmnDeSerializeRaw(inputStream, this->ArgType);
    cmnDeSerializeRaw(inputStream, this->ArgTypeId);
}

CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDescriptionComponentClassVecProxy);

//-----------------------------------------------------------------------------
//  Interface Description
//
CMN_IMPLEMENT_SERVICES(mtsDescriptionInterface);

mtsDescriptionInterface::mtsDescriptionInterface(const mtsDescriptionInterface & other):
    mtsGenericObject(other)
{
    this->ProcessName = other.ProcessName;
    this->ComponentName = other.ComponentName;
    this->InterfaceRequiredNames = other.InterfaceRequiredNames;
    this->InterfaceProvidedNames = other.InterfaceProvidedNames;
}

void mtsDescriptionInterface::ToStream(std::ostream & outputStream) const
{
    mtsGenericObject::ToStream(outputStream);
    outputStream << ", Process: " << this->ProcessName
                 << ", Component: " << this->ComponentName << ", ";

    outputStream << "Required interfaces: ";
    for (size_t i = 0; i < InterfaceRequiredNames.size(); ++i) {
        outputStream << InterfaceRequiredNames[i];
        outputStream << ", ";
    }
    outputStream << "Provided interfaces: ";
    for (size_t i = 0; i < InterfaceProvidedNames.size(); ++i) {
        outputStream << InterfaceProvidedNames[i];
        outputStream << ", ";
    }
}

void mtsDescriptionInterface::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream, this->ProcessName);
    cmnSerializeRaw(outputStream, this->ComponentName);
    cmnSerializeRaw(outputStream, this->InterfaceRequiredNames);
    cmnSerializeRaw(outputStream, this->InterfaceProvidedNames);
}

void mtsDescriptionInterface::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream, this->ProcessName);
    cmnDeSerializeRaw(inputStream, this->ComponentName);
    cmnDeSerializeRaw(inputStream, this->InterfaceRequiredNames);
    cmnDeSerializeRaw(inputStream, this->InterfaceProvidedNames);
}


//-----------------------------------------------------------------------------
//  Connection Description
//
CMN_IMPLEMENT_SERVICES(mtsDescriptionConnection);

mtsDescriptionConnection::mtsDescriptionConnection(const mtsDescriptionConnection & other):
    mtsGenericObject(other)
{
    Client.ProcessName   = other.Client.ProcessName;
    Client.ComponentName = other.Client.ComponentName;
    Client.InterfaceName = other.Client.InterfaceName;
    Server.ProcessName   = other.Server.ProcessName;
    Server.ComponentName = other.Server.ComponentName;
    Server.InterfaceName = other.Server.InterfaceName;
    ConnectionID         = other.ConnectionID;
}

mtsDescriptionConnection::mtsDescriptionConnection(
    const std::string & clientProcessName,
    const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
    const std::string & serverProcessName,
    const std::string & serverComponentName, const std::string & serverInterfaceProvidedName,
    const ConnectionIDType connectionId)
{
    Client.ProcessName   = clientProcessName;
    Client.ComponentName = clientComponentName;
    Client.InterfaceName = clientInterfaceRequiredName;
    Server.ProcessName   = serverProcessName;
    Server.ComponentName = serverComponentName;
    Server.InterfaceName = serverInterfaceProvidedName;
    ConnectionID         = connectionId;
}

void mtsDescriptionConnection::Init(void)
{
    Client.ProcessName.clear();
    Client.ComponentName.clear();
    Client.InterfaceName.clear();
    Server.ProcessName.clear();
    Server.ComponentName.clear();
    Server.InterfaceName.clear();
    ConnectionID = InvalidConnectionID;
}

void mtsDescriptionConnection::ToStream(std::ostream & outputStream) const
{
    mtsGenericObject::ToStream(outputStream);
    outputStream << ", (" << ConnectionID << ") "
                 << mtsManagerGlobal::GetInterfaceUID(Client.ProcessName, Client.ComponentName, Client.InterfaceName)
                 << " - "
                 << mtsManagerGlobal::GetInterfaceUID(Server.ProcessName, Server.ComponentName, Server.InterfaceName);
}

void mtsDescriptionConnection::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream, this->Client.ProcessName);
    cmnSerializeRaw(outputStream, this->Client.ComponentName);
    cmnSerializeRaw(outputStream, this->Client.InterfaceName);
    cmnSerializeRaw(outputStream, this->Server.ProcessName);
    cmnSerializeRaw(outputStream, this->Server.ComponentName);
    cmnSerializeRaw(outputStream, this->Server.InterfaceName);
    cmnSerializeRaw(outputStream, this->ConnectionID);
}

void mtsDescriptionConnection::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream, this->Client.ProcessName);
    cmnDeSerializeRaw(inputStream, this->Client.ComponentName);
    cmnDeSerializeRaw(inputStream, this->Client.InterfaceName);
    cmnDeSerializeRaw(inputStream, this->Server.ProcessName);
    cmnDeSerializeRaw(inputStream, this->Server.ComponentName);
    cmnDeSerializeRaw(inputStream, this->Server.InterfaceName);
    cmnDeSerializeRaw(inputStream, this->ConnectionID);
}

CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDescriptionConnectionVecProxy);

//-----------------------------------------------------------------------------
//  Component Status Control
//
CMN_IMPLEMENT_SERVICES(mtsComponentStatusControl);

mtsComponentStatusControl::mtsComponentStatusControl(const mtsComponentStatusControl & other):
    mtsGenericObject(other)
{
    ProcessName   = other.ProcessName;
    ComponentName = other.ComponentName;
    DelayInSecond = other.DelayInSecond;
    Command       = other.Command;
}

void mtsComponentStatusControl::ToStream(std::ostream & outputStream) const
{
    mtsGenericObject::ToStream(outputStream);
    outputStream << ", Process: " << this->ProcessName
                 << ", component: " << this->ComponentName
                 << ", delay: " << this->DelayInSecond
                 << ", command: ";
     switch (Command) {
         case COMPONENT_START:  outputStream << "START" << std::endl; break;
         case COMPONENT_STOP:   outputStream << "STOP" << std::endl; break;
         case COMPONENT_RESUME: outputStream << "RESUME" << std::endl; break;
         default:
             outputStream << "(INVALID)" << std::endl;
     }
}

void mtsComponentStatusControl::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream, ProcessName);
    cmnSerializeRaw(outputStream, ComponentName);
    cmnSerializeRaw(outputStream, DelayInSecond);
    cmnSerializeRaw(outputStream, static_cast<int>(Command));
}

void mtsComponentStatusControl::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream, ProcessName);
    cmnDeSerializeRaw(inputStream, ComponentName);
    cmnDeSerializeRaw(inputStream, DelayInSecond);
    int command;
    cmnDeSerializeRaw(inputStream, command);
    Command = static_cast<ComponentStatusCommand>(command);
}

//-----------------------------------------------------------------------------
//  Component State Change Event
//
CMN_IMPLEMENT_SERVICES(mtsComponentStateChange);

mtsComponentStateChange::mtsComponentStateChange(const mtsComponentStateChange & other):
    mtsGenericObject(other)
{
    ProcessName = other.ProcessName;
    ComponentName = other.ComponentName;
    NewState = other.NewState;
}

void mtsComponentStateChange::ToStream(std::ostream & outputStream) const
{
    mtsGenericObject::ToStream(outputStream);
    outputStream << ", Process: " << this->ProcessName
                 << ", component: " << this->ComponentName
                 << ", state: " << mtsComponentState::ToString(this->NewState.GetState());
}

void mtsComponentStateChange::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream, ProcessName);
    cmnSerializeRaw(outputStream, ComponentName);
    cmnSerializeRaw(outputStream, NewState);
}

void mtsComponentStateChange::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream, ProcessName);
    cmnDeSerializeRaw(inputStream, ComponentName);
    cmnDeSerializeRaw(inputStream, NewState);
}

//-----------------------------------------------------------------------------
// GetEndUserInterface (provided interface)
//

CMN_IMPLEMENT_SERVICES(mtsEndUserInterfaceArg);

void mtsEndUserInterfaceArg::ToStream(std::ostream & outputStream) const
{
    outputStream << "EndUserInterface to "
                 << (OriginalInterface ? OriginalInterface->GetName() : "???")
                 << (EndUserInterface ? " valid" : " invalid") << " for client"
                 << UserName << std::endl;
}

void mtsEndUserInterfaceArg::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream, UserName);
    cmnSerializeRaw(outputStream, OriginalInterface);
    cmnSerializeRaw(outputStream, EndUserInterface);
}

void mtsEndUserInterfaceArg::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream, UserName);
    cmnDeSerializeRaw(inputStream, OriginalInterface);
    cmnDeSerializeRaw(inputStream, EndUserInterface);
}

//-----------------------------------------------------------------------------
// AddObserverList
//

CMN_IMPLEMENT_SERVICES(mtsEventHandlerList);

// Shouldn't need to serialize or deserialize this class, or even print it out

void mtsEventHandlerList::ToStream(std::ostream & outputStream) const
{
    outputStream << "EventHandlerList (ToStream not implemented)" << std::endl;
}

void mtsEventHandlerList::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream, Provided);
    CMN_LOG_CLASS_RUN_WARNING << "SerializeRaw not implemented" << std::endl;
}

void mtsEventHandlerList::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream, Provided);
    CMN_LOG_CLASS_RUN_WARNING << "DeSerializeRaw not implemented" << std::endl;
}

//-----------------------------------------------------------------------------
//  LoadLibrary
//
CMN_IMPLEMENT_SERVICES(mtsDescriptionLoadLibrary);

mtsDescriptionLoadLibrary::mtsDescriptionLoadLibrary(const mtsDescriptionLoadLibrary & other):
    mtsGenericObject(other)
{
    this->ProcessName = other.ProcessName;
    this->LibraryName = other.LibraryName;
}

mtsDescriptionLoadLibrary::mtsDescriptionLoadLibrary(const std::string & processName, const std::string & libraryName):
    mtsGenericObject(),
    ProcessName(processName),
    LibraryName(libraryName)
{
}

void mtsDescriptionLoadLibrary::ToStream(std::ostream & outputStream) const
{
    mtsGenericObject::ToStream(outputStream);
    outputStream << ", Process: " << this->ProcessName
                 << ", library: " << this->LibraryName;
}

void mtsDescriptionLoadLibrary::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream, this->ProcessName);
    cmnSerializeRaw(outputStream, this->LibraryName);
}

void mtsDescriptionLoadLibrary::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream, this->ProcessName);
    cmnDeSerializeRaw(inputStream, this->LibraryName);
}
