/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2009-04-13

  (C) Copyright 2009-2018 Johns Hopkins University (JHU), All Rights Reserved.

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
        this->SetTimestamp(timestamp);
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
    return cmnData<mtsGenericObject>::ScalarNumber(*this);
}


bool mtsGenericObject::ScalarNumberIsFixed(void) const
{
    return cmnData<mtsGenericObject>::ScalarNumberIsFixed(*this);
}


double mtsGenericObject::Scalar(const size_t index) const
    CISST_THROW(std::out_of_range)
{
    return cmnData<mtsGenericObject>::Scalar(*this, index);
}


std::string mtsGenericObject::ScalarDescription(const size_t index, const std::string & userDescription) const
    CISST_THROW(std::out_of_range)
{
    return cmnData<mtsGenericObject>::ScalarDescription(*this, index, userDescription);
}


template <>
void cmnData<mtsGenericObject>::Copy(mtsGenericObject & data, const mtsGenericObject & source)
{
    data.Timestamp() = source.Timestamp();
    data.AutomaticTimestamp() = source.AutomaticTimestamp();
    data.Valid() = source.Valid();
}


template <>
void cmnData<mtsGenericObject>::SerializeBinary(const mtsGenericObject & data, std::ostream & outputStream)
    CISST_THROW(std::runtime_error)
{
    cmnData<double>::SerializeBinary(data.Timestamp(), outputStream);
    cmnData<bool>::SerializeBinary(data.AutomaticTimestamp(), outputStream);
    cmnData<bool>::SerializeBinary(data.Valid(), outputStream);
}


template <>
void cmnData<mtsGenericObject>::DeSerializeBinary(mtsGenericObject & data,
                                                  std::istream & inputStream,
                                                  const cmnDataFormat & localFormat,
                                                  const cmnDataFormat & remoteFormat)
    CISST_THROW(std::runtime_error)
{
    cmnData<double>::DeSerializeBinary(data.Timestamp(), inputStream, localFormat, remoteFormat);
    cmnData<bool>::DeSerializeBinary(data.AutomaticTimestamp(), inputStream, localFormat, remoteFormat);
    cmnData<bool>::DeSerializeBinary(data.Valid(), inputStream, localFormat, remoteFormat);
}


template <>
void cmnData<mtsGenericObject>::SerializeText(const mtsGenericObject & data, std::ostream & outputStream, const char delimiter)
    CISST_THROW(std::runtime_error)
{
    cmnData<double>::SerializeText(data.Timestamp(), outputStream, delimiter);
    outputStream << delimiter;
    cmnData<bool>::SerializeText(data.AutomaticTimestamp(), outputStream, delimiter);
    outputStream << delimiter;
    cmnData<bool>::SerializeText(data.Valid(), outputStream, delimiter);
}


template <>
std::string cmnData<mtsGenericObject>::SerializeDescription(const mtsGenericObject & data, const char delimiter, const std::string & userDescription)
{
    const std::string prefix = (userDescription == "") ? "" : (userDescription + ".");
    std::stringstream description;
    description << cmnData<double>::SerializeDescription(data.Timestamp(), delimiter, prefix + "Timestamp")
                << delimiter
                << cmnData<bool>::SerializeDescription(data.AutomaticTimestamp(), delimiter, prefix + "AutomaticTimestamp")
                << delimiter
                << cmnData<bool>::SerializeDescription(data.Valid(), delimiter, prefix + "Valid");
    return description.str();
}


template <>
void cmnData<mtsGenericObject>::DeSerializeText(mtsGenericObject & data, std::istream & inputStream, const char delimiter)
    CISST_THROW(std::runtime_error)
{
    cmnData<double>::DeSerializeText(data.Timestamp(), inputStream, delimiter);
    cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "mtsGenericObject");
    cmnData<bool>::DeSerializeText(data.AutomaticTimestamp(), inputStream, delimiter);
    cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "mtsGenericObject");
    cmnData<bool>::DeSerializeText(data.Valid(), inputStream, delimiter);
}


template <>
std::string cmnData<mtsGenericObject>::HumanReadable(const mtsGenericObject & data)
{
    std::stringstream output;
    output << "Timestamp (";
    if (data.AutomaticTimestamp()) {
        output << "auto";
    } else {
        output << "manual";
    }
    output << "): ";

    std::ios_base::fmtflags flags = output.flags();       // Save old flags
    output.setf(std::ios::fixed | std::ios::showpoint);
    output.precision(5);
    output  << data.Timestamp();
    output.flags(flags);                             // Restore old flags

    if (data.Valid()) {
        output << " (valid)";
    } else {
        output << " (invalid)";
    }
    return output.str();
}


template <>
bool cmnData<mtsGenericObject>::ScalarNumberIsFixed(const mtsGenericObject & CMN_UNUSED(data))
{
    return true;
}


template <>
size_t cmnData<mtsGenericObject>::ScalarNumber(const mtsGenericObject & CMN_UNUSED(data))
{
    return 3;
}


template <>
double cmnData<mtsGenericObject>::Scalar(const mtsGenericObject & data, const size_t index)
    CISST_THROW(std::out_of_range)
{
    if (index >= data.ScalarNumber()) {
        cmnThrow(std::out_of_range("cmnDataScalar: mtsGenericObject index out of range"));
        return 1.2345;
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


template <>
std::string cmnData<mtsGenericObject>::ScalarDescription(const mtsGenericObject & data, const size_t index, const std::string & userDescription)
    CISST_THROW(std::out_of_range)
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

#if CISST_HAS_JSON
template <>
void cmnDataJSON<mtsGenericObject>::SerializeText(const mtsGenericObject & data, Json::Value & jsonValue) {
    cmnDataJSON<double>::SerializeText(data.Timestamp(), jsonValue["Timestamp"]);
    cmnDataJSON<bool>::SerializeText(data.AutomaticTimestamp(), jsonValue["AutomaticTimestamp"]);
    cmnDataJSON<bool>::SerializeText(data.Valid(), jsonValue["Valid"]);
}

template <>
void cmnDataJSON<mtsGenericObject>::DeSerializeText(mtsGenericObject & data, const Json::Value & jsonValue) CISST_THROW(std::runtime_error) {
    Json::Value field;
    field = jsonValue["Timestamp"];
    if (!field.empty()) {
        cmnDataJSON<double>::DeSerializeText(data.Timestamp(), field);
    }
    field = jsonValue["AutomaticTimestamp"];
    if (!field.empty()) {
        cmnDataJSON<bool>::DeSerializeText(data.AutomaticTimestamp(), field);
    }
    field = jsonValue["Valid"];
    if (!field.empty()) {
        cmnDataJSON<bool>::DeSerializeText(data.Valid(), field);
    }
}

#endif // CISST_HAS_JSON
