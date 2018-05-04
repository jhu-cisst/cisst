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

#pragma once
#ifndef _cmnDataFunctionsString_h
#define _cmnDataFunctionsString_h

#include <cisstCommon/cmnDataFunctions.h>
#include <string>

// always include last
#include <cisstCommon/cmnExport.h>

size_t CISST_EXPORT cmnDataSerializeBinary(const std::string & data, char * buffer, size_t bufferSize);

size_t CISST_EXPORT cmnDataDeSerializeBinary(std::string & data, const char * buffer, size_t bufferSize,
                                             const cmnDataFormat & localFormat,
                                             const cmnDataFormat & remoteFormat);

void CISST_EXPORT cmnDataSerializeBinary(const std::string & data,
                                         std::ostream & outputStream) CISST_THROW(std::runtime_error);

void CISST_EXPORT cmnDataDeSerializeBinary(std::string & data,
                                           std::istream & inputStream,
                                           const cmnDataFormat & localFormat,
                                           const cmnDataFormat & remoteFormat) CISST_THROW(std::runtime_error);

void CISST_EXPORT cmnDataSerializeText(const std::string & data,
                                       std::ostream & outputStream,
                                       const char delimiter = ',') CISST_THROW(std::runtime_error);

void CISST_EXPORT cmnDataDeSerializeText(std::string & data,
                                         std::istream & inputStream,
                                         const char delimiter = ',') CISST_THROW(std::runtime_error);


template <>
class cmnData<std::string>
{
public:
    typedef std::string DataType;

    CMN_DATA_IS_SPECIALIZED_TRUE(std::string)
    CMN_DATA_COPY_USING_ASSIGN(std::string);
    CMN_DATA_SERIALIZE_DESCRIPTION(std::string, str);

    static std::string HumanReadable(const std::string & data) {
        return data;
    }

    static size_t SerializeBinaryByteSize(const DataType & data) {
        return (cmnData<size_t>::SerializeBinaryByteSize(data.size()) + data.size() * sizeof(std::string::value_type));
    }

    static size_t SerializeBinary(const DataType & data,
                                  char * buffer, size_t bufferSize) {
        return cmnDataSerializeBinary(data, buffer, bufferSize);
    }

    static size_t DeSerializeBinary(DataType & data, const char * buffer, size_t bufferSize,
                                    const cmnDataFormat & localFormat,
                                    const cmnDataFormat & remoteFormat)
    {
        return cmnDataDeSerializeBinary(data, buffer, bufferSize, localFormat, remoteFormat);
    }

    static void SerializeBinary(const DataType & data,
                                std::ostream & outputStream)
        CISST_THROW(std::runtime_error)
    {
        cmnDataSerializeBinary(data, outputStream);
    }

    static void DeSerializeBinary(DataType & data,
                                  std::istream & inputStream,
                                  const cmnDataFormat & localFormat,
                                  const cmnDataFormat & remoteFormat)
        CISST_THROW(std::runtime_error)
    {
        cmnDataDeSerializeBinary(data, inputStream, localFormat, remoteFormat);
    }

    static void SerializeText(const DataType & data,
                              std::ostream & outputStream,
                              const char delimiter = ',')
        CISST_THROW(std::runtime_error)
    {
        cmnDataSerializeText(data, outputStream, delimiter);
    }

    static void DeSerializeText(DataType & data,
                                std::istream & inputStream,
                                const char delimiter = ',')
        CISST_THROW(std::runtime_error)
    {
        cmnDataDeSerializeText(data, inputStream, delimiter);
    }

    static bool ScalarNumberIsFixed(const std::string & CMN_UNUSED(data)) {
        return true;
    }

    static size_t ScalarNumber(const std::string & CMN_UNUSED(data)) {
        return 0;
    }

    static std::string ScalarDescription(const std::string & CMN_UNUSED(data), const size_t CMN_UNUSED(index),
                                         const std::string & CMN_UNUSED(userDescription)) CISST_THROW(std::out_of_range) {
        cmnThrow(std::out_of_range("cmnDataScalarDescription: std::string has no scalar"));
        return "n/a";
    }

    static double Scalar(const std::string & CMN_UNUSED(data), const size_t CMN_UNUSED(index)) CISST_THROW(std::out_of_range) {
        cmnThrow(std::out_of_range("cmnDataScalar: std::string has no scalar"));
        return 1.234;
    }
};

#endif // _cmnDataFunctionsString_h
