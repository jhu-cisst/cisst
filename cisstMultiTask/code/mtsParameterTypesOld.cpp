/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet, Min Yang Jung
  Created on: 2010-09-01

  (C) Copyright 2010-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsParameterTypes.h>
#include <cisstMultiTask/mtsManagerGlobal.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

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

//-----------------------------------------------------------------------------
// AddObserverList
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
//  System-wide Thread-safe Logging
//
mtsLogMessage::mtsLogMessage()
    : mtsGenericObject(),
      Length(0), ProcessName("")
{
    memset(this->Message, 0, MAX_LOG_SIZE);
}

mtsLogMessage::mtsLogMessage(const mtsLogMessage & other)
    : mtsGenericObject(other),
      Length(other.Length), ProcessName(other.ProcessName)
{
    memset(this->Message, 0, MAX_LOG_SIZE);
    memcpy(this->Message, other.Message, this->Length);
}

mtsLogMessage & mtsLogMessage::operator = (const mtsLogMessage & other)
{
    mtsGenericObject::operator = (other);
    Length = other.Length;
    ProcessName = other.ProcessName;
    memset(this->Message, 0, MAX_LOG_SIZE);
    memcpy(this->Message, other.Message, this->Length);
    return *this;
}

mtsLogMessage::mtsLogMessage(const char * log, size_t len)
    : mtsGenericObject(),
      Length(len), ProcessName("")
{
    memset(this->Message, 0, MAX_LOG_SIZE);
    memcpy(this->Message, log, this->Length);
}

void mtsLogMessage::ToStream(std::ostream & outputStream) const
{
    mtsGenericObject::ToStream(outputStream);
    outputStream << ", Message: \"" << std::string(this->Message, this->Length)
                 << "\", Length: " << Length
                 << ", Process: \"" << ProcessName << "\"";
}

void mtsLogMessage::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream, this->Length);
    cmnSerializeRaw(outputStream, this->Message);
    cmnSerializeRaw(outputStream, this->ProcessName);
}

void mtsLogMessage::DeSerializeRaw(std::istream & inputStream)
{
    memset(this->Message, 0, MAX_LOG_SIZE);

    mtsGenericObject::DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream, this->Length);
    cmnDeSerializeRaw(inputStream, this->Message);
    cmnDeSerializeRaw(inputStream, this->ProcessName);
}
