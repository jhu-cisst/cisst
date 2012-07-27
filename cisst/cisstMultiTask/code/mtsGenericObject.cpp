/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2009-04-13

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstMultiTask/mtsGenericObject.h>

#include <iostream>
#include <iomanip>

bool mtsGenericObject::SetTimestampIfAutomatic(double timestamp) {
    if (this->AutomaticTimestampMember) {
        this->TimestampMember = timestamp;
        return true;
    }
    return false;
}


void mtsGenericObject::ToStream(std::ostream & outputStream) const {
    outputStream << "Timestamp (";
    if (this->AutomaticTimestamp()) {
        outputStream << "auto";
    } else {
        outputStream << "manual";
    }
    outputStream << "): ";

    std::ios_base::fmtflags flags = outputStream.flags();       // Save old flags
    outputStream.setf(std::ios::fixed | std::ios::showpoint);
    outputStream.precision(5);

    outputStream  << this->Timestamp();
    outputStream.flags(flags);                             // Restore old flags

    if (this->Valid()) {
        outputStream << " (valid)";
    } else {
        outputStream << " (invalid)";
    }
}


void mtsGenericObject::ToStreamRaw(std::ostream & outputStream, const char delimiter,
                                   bool headerOnly, const std::string & headerPrefix) const {
    if (headerOnly) {
        outputStream << headerPrefix << "-timestamp" << delimiter
                     << headerPrefix << "-automatic-timestamp" << delimiter
                     << headerPrefix << "-valid";
    } else {
        outputStream << this->Timestamp() << delimiter
                     << this->AutomaticTimestamp() << delimiter
                     << this->Valid();
    }
}


bool mtsGenericObject::FromStreamRaw(std::istream & inputStream, const char delimiter)
{
    // For now, only deal with space delimiters
    if (!isspace(delimiter)) {
        inputStream.setstate(std::ios::failbit | std::ios::badbit);
        return false;
    }
    inputStream >> TimestampMember;
    if (inputStream.fail())
        return false;
    inputStream >> AutomaticTimestampMember;
    if (inputStream.fail())
        return false;
    inputStream >> ValidMember;
    if (inputStream.fail())
        return false;
    return (typeid(this) == typeid(mtsGenericObject));
}

void mtsGenericObject::SerializeRaw(std::ostream & outputStream) const {
    cmnSerializeRaw(outputStream, this->Timestamp());
    cmnSerializeRaw(outputStream, this->AutomaticTimestamp());
    cmnSerializeRaw(outputStream, this->Valid());
}


void mtsGenericObject::DeSerializeRaw(std::istream & inputStream) {
    cmnDeSerializeRaw(inputStream, this->Timestamp());
    cmnDeSerializeRaw(inputStream, this->AutomaticTimestamp());
    cmnDeSerializeRaw(inputStream, this->Valid());
}


size_t mtsGenericObject::ScalarNumber(void) const
{
    return cmnDataScalarNumber(*this);
}


bool mtsGenericObject::ScalarNumberIsFixed(void) const
{
    return cmnDataScalarNumberIsFixed(*this);
}


double mtsGenericObject::Scalar(const size_t index) const
{
    return cmnDataScalar(*this, index);
}


std::string mtsGenericObject::ScalarDescription(const size_t index, const char * userDescription) const
{
    return cmnDataScalarDescription(*this, index, userDescription);
}


void cmnDataSerializeBinary(std::ostream & outputStream, const mtsGenericObject & data)
{
    std::cerr << CMN_LOG_DETAILS << "mtsGenericObject serialize!" << std::endl;
    cmnDataSerializeBinary(outputStream, data.Timestamp());
    cmnDataSerializeBinary(outputStream, data.AutomaticTimestamp());
    cmnDataSerializeBinary(outputStream, data.Valid());
}


void cmnDataDeSerializeBinary(std::istream & inputStream, mtsGenericObject & data,
                              const cmnDataFormat & remoteFormat, const cmnDataFormat & localFormat)
{
    std::cerr << CMN_LOG_DETAILS << "mtsGenericObject de-serialize!" << std::endl;
    cmnDataDeSerializeBinary(inputStream, data.Timestamp(), remoteFormat, localFormat);
    cmnDataDeSerializeBinary(inputStream, data.AutomaticTimestamp(), remoteFormat, localFormat);
    cmnDataDeSerializeBinary(inputStream, data.Valid(), remoteFormat, localFormat);
}


bool cmnDataScalarNumberIsFixed(const mtsGenericObject & CMN_UNUSED(data))
{
    return true;
}


size_t cmnDataScalarNumber(const mtsGenericObject & data)
{
    return 3;
}


double cmnDataScalar(const mtsGenericObject & data, const size_t index)
{
    if (index >= data.ScalarNumber()) {
        return 0.0;
    }
    if (index == 0) {
        return data.Timestamp();
    }
    if (index == 1) {
        return static_cast<double>(data.AutomaticTimestamp());
    }
    // last case
    return static_cast<double>(data.Valid());
}


std::string cmnDataScalarDescription(const mtsGenericObject & data, const size_t index, const char * userDescription)
{
    if (index >= data.ScalarNumber()) {
        return "index out of range";
    }
    if (index == 0) {
        return std::string(userDescription) + "Timestamp";
    }
    if (index == 1) {
        return std::string(userDescription) + "AutomaticTimestamp";
    }
    // last case
    return std::string(userDescription) + "Valid";
}
