/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2009-04-13

  (C) Copyright 2009-2013 Johns Hopkins University (JHU), All Rights
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
    throw (std::out_of_range)
{
    return cmnDataScalar(*this, index);
}


std::string mtsGenericObject::ScalarDescription(const size_t index, const std::string & userDescription) const
    throw (std::out_of_range)
{
    return cmnDataScalarDescription(*this, index, userDescription);
}


void cmnDataCopy(mtsGenericObject & destination, const mtsGenericObject & source)
{
    destination.Timestamp() = source.Timestamp();
    destination.AutomaticTimestamp() = source.AutomaticTimestamp();
    destination.Valid() = source.Valid();
}


void cmnDataSerializeBinary(std::ostream & outputStream, const mtsGenericObject & data)
    throw (std::runtime_error)
{
    cmnDataSerializeBinary(outputStream, data.Timestamp());
    cmnDataSerializeBinary(outputStream, data.AutomaticTimestamp());
    cmnDataSerializeBinary(outputStream, data.Valid());
}


void cmnDataDeSerializeBinary(std::istream & inputStream, mtsGenericObject & data,
                              const cmnDataFormat & remoteFormat, const cmnDataFormat & localFormat)
    throw (std::runtime_error)
{
    cmnDataDeSerializeBinary(inputStream, data.Timestamp(), remoteFormat, localFormat);
    cmnDataDeSerializeBinary(inputStream, data.AutomaticTimestamp(), remoteFormat, localFormat);
    cmnDataDeSerializeBinary(inputStream, data.Valid(), remoteFormat, localFormat);
}


void cmnDataSerializeText(std::ostream & outputStream, const mtsGenericObject & data, const char delimiter)
    throw (std::runtime_error)
{
    cmnDataSerializeText(outputStream, data.Timestamp(), delimiter);
    outputStream << delimiter;
    cmnDataSerializeText(outputStream, data.AutomaticTimestamp(), delimiter);
    outputStream << delimiter;
    cmnDataSerializeText(outputStream, data.Valid(), delimiter);
}


std::string cmnDataSerializeTextDescription(const mtsGenericObject & data, const char delimiter, const std::string & userDescription)
{
    const std::string prefix = (userDescription == "") ? "" : (userDescription + ".");
    std::stringstream description;
    description << cmnDataSerializeTextDescription(data.Timestamp(), delimiter, prefix + "Timestamp")
                << delimiter
                << cmnDataSerializeTextDescription(data.AutomaticTimestamp(), delimiter, prefix + "AutomaticTimestamp")
                << delimiter
                << cmnDataSerializeTextDescription(data.Valid(), delimiter, prefix + "Valid");
    return description.str();
}


void cmnDataDeSerializeText(std::istream & inputStream, mtsGenericObject & data, const char delimiter)
    throw (std::runtime_error)
{
    cmnDataDeSerializeText(inputStream, data.Timestamp(), delimiter);
    cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "mtsGenericObject");
    cmnDataDeSerializeText(inputStream, data.AutomaticTimestamp(), delimiter);
    cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "mtsGenericObject");
    cmnDataDeSerializeText(inputStream, data.Valid(), delimiter);
}


bool cmnDataScalarNumberIsFixed(const mtsGenericObject & CMN_UNUSED(data))
{
    return true;
}


size_t cmnDataScalarNumber(const mtsGenericObject & CMN_UNUSED(data))
{
    return 3;
}


double cmnDataScalar(const mtsGenericObject & data, const size_t index)
    throw (std::out_of_range)
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


std::string cmnDataScalarDescription(const mtsGenericObject & data, const size_t index, const std::string & userDescription)
    throw (std::out_of_range)
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
