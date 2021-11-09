/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2011-06-27

  (C) Copyright 2011-2021 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _cmnDataFunctionsEnumMacros_h
#define _cmnDataFunctionsEnumMacros_h

#include <cisstCommon/cmnDataFunctions.h>


#define CMN_DATA_SPECIALIZATION_FOR_ENUM_USER_HUMAN_READABLE(_enum, _promotedType, _humanReadableFunction) \
template <>                                                             \
class cmnData<_enum> {                                                  \
public:                                                                 \
 enum {IS_SPECIALIZED = 1};                                             \
 typedef _enum DataType;                                                \
 static void Copy(DataType & data, const _enum & source) {              \
     data = source;                                                     \
 }                                                                      \
 static std::string HumanReadable(const DataType & data) {              \
     return _humanReadableFunction(data);                               \
 }                                                                      \
 static void SerializeBinary(const DataType & data,                     \
                             std::ostream & outputStream) CISST_THROW(std::runtime_error) { \
     CMN_ASSERT(sizeof(_promotedType) >= sizeof(DataType));             \
     const _promotedType dataPromoted = static_cast<_promotedType>(data); \
     cmnData<_promotedType>::SerializeBinary(dataPromoted, outputStream); \
 }                                                                      \
 static void DeSerializeBinary(DataType & data, std::istream & inputStream, \
                               const cmnDataFormat & localFormat,       \
                               const cmnDataFormat & remoteFormat) CISST_THROW(std::runtime_error) { \
     CMN_ASSERT(sizeof(_promotedType) >= sizeof(DataType));             \
     _promotedType dataPromoted;                                        \
     cmnData<_promotedType>::DeSerializeBinary(dataPromoted, inputStream, localFormat, remoteFormat); \
     data = static_cast<DataType>(dataPromoted);                        \
 }                                                                      \
 static void SerializeText(const DataType & data, std::ostream & outputStream, \
                           const char CMN_UNUSED(delimiter) = ',') CISST_THROW(std::runtime_error) { \
     CMN_ASSERT(sizeof(_promotedType) >= sizeof(DataType));             \
     outputStream << static_cast<_promotedType>(data);                  \
 }                                                                      \
 static std::string SerializeDescription(const DataType & CMN_UNUSED(data), \
                                         const char CMN_UNUSED(delimiter) = ',', \
                                         const std::string & userDescription = "") { \
     return (userDescription == "") ? #_enum : (userDescription + #_enum); \
 }                                                                      \
 static void DeSerializeText(DataType & data, std::istream & inputStream, \
                             const char CMN_UNUSED(delimiter) = ',') CISST_THROW(std::runtime_error) { \
     CMN_ASSERT(sizeof(_promotedType) >= sizeof(DataType));             \
     _promotedType dataPromoted;                                        \
     inputStream >> dataPromoted;                                       \
     data = static_cast<DataType>(dataPromoted);                        \
 }                                                                      \
 static bool ScalarNumberIsFixed(const DataType & CMN_UNUSED(data)) {   \
     return true;                                                       \
 }                                                                      \
 static size_t ScalarNumber(const DataType & CMN_UNUSED(data)) {        \
     return 1;                                                          \
 }                                                                      \
 static std::string ScalarDescription(const DataType & CMN_UNUSED(data), const size_t index, \
                                      const std::string & userDescription) CISST_THROW(std::out_of_range) { \
     if (index == 0) {                                                  \
         return (userDescription == "") ? #_enum : (userDescription + #_enum); \
     }                                                                  \
     cmnThrow(std::out_of_range("cmnDataScalarDescription: " #_enum " index out of range")); \
     return "";                                                         \
 }                                                                      \
 static double Scalar(const DataType & data, const size_t index) CISST_THROW(std::out_of_range) { \
     if (index == 0) {                                                  \
         return static_cast<double>(data);                              \
     }                                                                  \
     cmnThrow(std::out_of_range("cmnDataScalar: " #_enum " index out of range")); \
     return 1.2345;                                                     \
 }                                                                      \
};


#define CMN_DATA_SPECIALIZATION_FOR_ENUM(_enum, _promotedType) \
    CMN_DATA_SPECIALIZATION_FOR_ENUM_USER_HUMAN_READABLE(_enum, _promotedType, cmnData<int>::HumanReadable)


#define CMN_DECLARE_DATA_FUNCTIONS_JSON_FOR_ENUM(_enum) \
    template <> void cmnDataJSON<_enum>::SerializeText(const _enum & data, Json::Value & jsonValue); \
    template <> void cmnDataJSON<_enum>::DeSerializeText(_enum & data, const Json::Value & jsonValue) CISST_THROW(std::runtime_error);

#define CMN_DECLARE_DATA_FUNCTIONS_JSON_FOR_ENUM_EXPORT(_enum) \
    template <> void CISST_EXPORT cmnDataJSON<_enum>::SerializeText(const _enum & data, Json::Value & jsonValue); \
    template <> void CISST_EXPORT cmnDataJSON<_enum>::DeSerializeText(_enum & data, const Json::Value & jsonValue) CISST_THROW(std::runtime_error);

#define CMN_IMPLEMENT_DATA_FUNCTIONS_JSON_FOR_ENUM(_enum, _promotedType) \
    template <> void cmnDataJSON<_enum>::SerializeText(const _enum & data, Json::Value & jsonValue) { \
        CMN_ASSERT(sizeof(_promotedType) >= sizeof(_enum));             \
        const _promotedType dataPromoted = static_cast<_promotedType>(data); \
        cmnDataJSON<_promotedType>::SerializeText(dataPromoted, jsonValue); \
    }                                                                   \
    template <> void cmnDataJSON<_enum>::DeSerializeText(_enum & data, const Json::Value & jsonValue) CISST_THROW(std::runtime_error) { \
        CMN_ASSERT(sizeof(_promotedType) >= sizeof(_enum));             \
        _promotedType dataPromoted;                                     \
        cmnDataJSON<_promotedType>::DeSerializeText(dataPromoted, jsonValue); \
        data = static_cast<_enum>(dataPromoted);                        \
    }

#endif // _cmnDataFunctionsEnumMacros_h
