/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2011-06-27

  (C) Copyright 2011-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstCommon/cmnDataFunctionsJSON.h>
#include <cisstCommon/cmnAssert.h>

#if CISST_HAS_JSON

template <>
void cmnDataJSON<double>::SerializeText(const DataType & data, Json::Value & jsonValue) {
    jsonValue = data;
}
template <>
void cmnDataJSON<double>::DeSerializeText(DataType & data, const Json::Value & jsonValue) CISST_THROW(std::runtime_error) {
    if (jsonValue.empty()) {
        cmnThrow("cmnDataJSON<double>::DeSerializeText: empty JSON value");
    }
    data = jsonValue.asDouble();
}

template <>
void cmnDataJSON<float>::SerializeText(const DataType & data, Json::Value & jsonValue) {
    jsonValue = data;
}
template <>
void cmnDataJSON<float>::DeSerializeText(DataType & data, const Json::Value & jsonValue) CISST_THROW(std::runtime_error) {
    if (jsonValue.empty()) {
        cmnThrow("cmnDataJSON<float>::DeSerializeText: empty JSON value");
    }
    data = jsonValue.asFloat();
}

template <>
void cmnDataJSON<char>::SerializeText(const DataType & data, Json::Value & jsonValue) {
    jsonValue = data;
}
template <>
void cmnDataJSON<char>::DeSerializeText(DataType & data, const Json::Value & jsonValue) CISST_THROW(std::runtime_error) {
    if (jsonValue.empty()) {
        cmnThrow("cmnDataJSON<char>::DeSerializeText: empty JSON value");
    }
    std::string temp = jsonValue.asString();
    if (temp.size() != 1) {
        cmnThrow("cmnDataJSON<char>::DeSerializeText: string with more than one character");
    }
    data = temp[0];
}

template <>
void cmnDataJSON<int>::SerializeText(const DataType & data, Json::Value & jsonValue) {
    jsonValue = data;
}
template <>
void cmnDataJSON<int>::DeSerializeText(DataType & data, const Json::Value & jsonValue) CISST_THROW(std::runtime_error) {
    if (jsonValue.empty()) {
        cmnThrow("cmnDataJSON<int>::DeSerializeText: empty JSON value");
    }
    data = jsonValue.asInt();
}

template <>
void cmnDataJSON<unsigned int>::SerializeText(const DataType & data, Json::Value & jsonValue) {
    jsonValue = data;
}
template <>
void cmnDataJSON<unsigned int>::DeSerializeText(DataType & data, const Json::Value & jsonValue) CISST_THROW(std::runtime_error) {
    if (jsonValue.empty()) {
        cmnThrow("cmnDataJSON<unsigned int>::DeSerializeText: empty JSON value");
    }
    data = jsonValue.asUInt();
}

template <>
void cmnDataJSON<unsigned long int>::SerializeText(const DataType & data, Json::Value & jsonValue) {
    jsonValue = Json::UInt64(data);
}
template <>
void cmnDataJSON<unsigned long int>::DeSerializeText(DataType & data, const Json::Value & jsonValue) CISST_THROW(std::runtime_error) {
    if (jsonValue.empty()) {
        cmnThrow("cmnDataJSON<unsigned long int>::DeSerializeText: empty JSON value");
    }
    data = jsonValue.asUInt64();
}

template <>
void cmnDataJSON<unsigned long long int>::SerializeText(const DataType & data, Json::Value & jsonValue) {
    jsonValue = Json::UInt64(data);
}
template <>
void cmnDataJSON<unsigned long long int>::DeSerializeText(DataType & data, const Json::Value & jsonValue) CISST_THROW(std::runtime_error) {
    if (jsonValue.empty()) {
        cmnThrow("cmnDataJSON<unsigned long long int>::DeSerializeText: empty JSON value");
    }
    data = jsonValue.asUInt64();
}

template <>
void cmnDataJSON<bool>::SerializeText(const DataType & data, Json::Value & jsonValue) {
    jsonValue = data;
}
template <>
void cmnDataJSON<bool>::DeSerializeText(DataType & data, const Json::Value & jsonValue) CISST_THROW(std::runtime_error) {
    if (jsonValue.empty()) {
        cmnThrow("cmnDataJSON<bool>::DeSerializeText: empty JSON value");
    }
    data = jsonValue.asBool();
}

template <>
void cmnDataJSON<std::string>::SerializeText(const DataType & data, Json::Value & jsonValue) {
    jsonValue = data;
}
template <>
void cmnDataJSON<std::string>::DeSerializeText(DataType & data, const Json::Value & jsonValue) CISST_THROW(std::runtime_error) {
    if (jsonValue.empty()) {
        cmnThrow("cmnDataJSON<std::string>::DeSerializeText: empty JSON value");
    }
    data = jsonValue.asString();
}

#endif // CISST_HAS_JSON
