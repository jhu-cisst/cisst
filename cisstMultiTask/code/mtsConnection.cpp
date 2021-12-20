/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Min Yang Jung
  Created on: 2010-12-27

  (C) Copyright 2010-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsConnection.h>
#include <cisstOSAbstraction/osaGetTime.h>

CMN_IMPLEMENT_SERVICES(mtsConnection);

mtsConnection::mtsConnection(const mtsDescriptionConnection & description, const std::string & requestProcessName)
    : ConnectionDescription(description), RequestProcessName(requestProcessName), Connected(false)
{
}

mtsDescriptionConnection mtsConnection::GetDescriptionConnection(void) const
{
    return ConnectionDescription;
}

void mtsConnection::GetDescriptionConnection(mtsDescriptionConnection & description) const
{
    description = ConnectionDescription;
}

ConnectionIDType mtsConnection::GetConnectionID(void) const {
    return ConnectionDescription.ConnectionID;
}

std::string mtsConnection::GetClientProcessName(void) const {
    return ConnectionDescription.Client.ProcessName;
}

std::string mtsConnection::GetClientComponentName(void) const {
    return ConnectionDescription.Client.ComponentName;
}

std::string mtsConnection::GetClientInterfaceName(void) const {
    return ConnectionDescription.Client.InterfaceName;
}

std::string mtsConnection::GetServerProcessName(void) const {
    return ConnectionDescription.Server.ProcessName;
}

std::string mtsConnection::GetServerComponentName(void) const {
    return ConnectionDescription.Server.ComponentName;
}

std::string mtsConnection::GetServerInterfaceName(void) const {
    return ConnectionDescription.Server.InterfaceName;
}

bool mtsConnection::IsRemoteConnection(void) const {
    return (ConnectionDescription.Client.ProcessName != ConnectionDescription.Server.ProcessName);
}

void mtsConnection::ToStream(std::ostream & outputStream) const
{
    mtsGenericObject::ToStream(outputStream);
    outputStream << ", " << ConnectionDescription
                 << ", Connected: " << (Connected ? "YES" : "NO")
                 ;
}

void mtsConnection::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);
    ConnectionDescription.SerializeRaw(outputStream);
    cmnSerializeRaw(outputStream, Connected);
}

void mtsConnection::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);
    ConnectionDescription.DeSerializeRaw(inputStream);
    cmnDeSerializeRaw(inputStream, Connected);
}
