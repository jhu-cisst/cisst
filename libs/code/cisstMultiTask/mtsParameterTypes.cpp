/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet, Min Yang Jung
  Created on: 2010-09-01

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsParameterTypes.h>
#include <cisstMultiTask/mtsManagerGlobal.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

// Utility functions
void mtsParameterTypes::ConvertVectorStringType(const mtsStdStringVec & mtsVec, std::vector<std::string> & stdVec)
{
    // MJ: is there better way to do this?
    for (size_t i = 0; i < mtsVec.size(); ++i) {
        stdVec.push_back(mtsVec(i));
    }
}

void mtsParameterTypes::ConvertVectorStringType(const std::vector<std::string> & stdVec, mtsStdStringVec & mtsVec)
{
    // MJ: is there better way to do this?
    mtsVec.SetSize(stdVec.size());
    for (size_t i = 0; i < stdVec.size(); ++i) {
        mtsVec(i) = stdVec[i];
    }
}

//-----------------------------------------------------------------------------
//  Component Description
//
CMN_IMPLEMENT_SERVICES(mtsDescriptionComponent);

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
//  Interface Description
//
CMN_IMPLEMENT_SERVICES(mtsDescriptionInterface);

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
    InterfaceRequiredNames.SerializeRaw(outputStream);
    InterfaceProvidedNames.SerializeRaw(outputStream);
}

void mtsDescriptionInterface::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream, this->ProcessName);
    cmnDeSerializeRaw(inputStream, this->ComponentName);
    InterfaceRequiredNames.DeSerializeRaw(inputStream);
    InterfaceProvidedNames.DeSerializeRaw(inputStream);
}


//-----------------------------------------------------------------------------
//  Connection Description
//
CMN_IMPLEMENT_SERVICES(mtsDescriptionConnection);

mtsDescriptionConnection::mtsDescriptionConnection(
    const std::string & clientProcessName,
    const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
    const std::string & serverProcessName, 
    const std::string & serverComponentName, const std::string & serverInterfaceProvidedName,
    const int connectionId)
{
    Client.ProcessName   = clientProcessName;
    Client.ComponentName = clientComponentName;
    Client.InterfaceName = clientInterfaceRequiredName;
    Server.ProcessName   = serverProcessName;
    Server.ComponentName = serverComponentName;
    Server.InterfaceName = serverInterfaceProvidedName;
    ConnectionID         = connectionId;
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

CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDescriptionConnectionVec);

//-----------------------------------------------------------------------------
//  Component Status Control
//
CMN_IMPLEMENT_SERVICES(mtsComponentStatusControl);

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
//  Component Status Change Event
//
CMN_IMPLEMENT_SERVICES(mtsComponentStateChange);

void mtsComponentStateChange::ToStream(std::ostream & outputStream) const
{
    mtsGenericObject::ToStream(outputStream);
    outputStream << ", Process: " << this->ProcessName
                 << ", component: " << this->ComponentName
                 << ", state: " << mtsComponentState::ToString(this->NewState);
}

void mtsComponentStateChange::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream, ProcessName);
    cmnSerializeRaw(outputStream, ComponentName);
    cmnSerializeRaw(outputStream, static_cast<int>(NewState));
}

void mtsComponentStateChange::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream, ProcessName);
    cmnDeSerializeRaw(inputStream, ComponentName);
    int newState;
    cmnDeSerializeRaw(inputStream, newState);
    NewState = static_cast<mtsComponentState::Enum>(newState);
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
