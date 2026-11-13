/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Brendan Burkhart
  Created on: 2026-11-13

  (C) Copyright 2026 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _cmnDataFunctionsPath_h
#define _cmnDataFunctionsPath_h

#include <filesystem>

#include <cisstCommon/cmnSearchPath.h>
#include <cisstCommon/cmnDataFunctions.h>
#include <cisstCommon/cmnDataFunctionsString.h>

// always include last
#include <cisstCommon/cmnExport.h>

template <>
class cmnData<std::filesystem::path, void>
{
public:
    typedef std::filesystem::path DataType;

    CMN_DATA_IS_SPECIALIZED_TRUE(std::filesystem::path)
    CMN_DATA_COPY_USING_ASSIGN(std::filesystem::path);
    CMN_DATA_SERIALIZE_DESCRIPTION(std::filesystem::path, str);

    static std::string HumanReadable(const DataType& data) {
        return cmnData<std::string>::HumanReadable(pathToUTF8(data));
    }

    static size_t SerializeBinaryByteSize(const DataType& data) {
        return cmnData<std::string>::SerializeBinaryByteSize(pathToUTF8(data));
    }

    static size_t SerializeBinary(const DataType& data,
                                  char * buffer, size_t bufferSize) {
        return cmnDataSerializeBinary(pathToUTF8(data), buffer, bufferSize);
    }

    static size_t DeSerializeBinary(DataType& data, const char * buffer, size_t bufferSize,
                                    const cmnDataFormat & localFormat,
                                    const cmnDataFormat & remoteFormat)
    {
        std::string output;
        size_t result = cmnDataDeSerializeBinary(output, buffer, bufferSize, localFormat, remoteFormat);
        data = pathFromUTF8(output);
        return result;
    }

    static void SerializeBinary(const DataType& data,
                                std::ostream & outputStream)
        CISST_THROW(std::runtime_error)
    {
        cmnDataSerializeBinary(pathToUTF8(data), outputStream);
    }

    static void DeSerializeBinary(DataType& data,
                                  std::istream & inputStream,
                                  const cmnDataFormat & localFormat,
                                  const cmnDataFormat & remoteFormat)
        CISST_THROW(std::runtime_error)
    {
        std::string output;
        cmnDataDeSerializeBinary(output, inputStream, localFormat, remoteFormat);
        data = pathFromUTF8(output);
    }

    static void SerializeText(const DataType& data,
                              std::ostream & outputStream,
                              const char delimiter = ',')
        CISST_THROW(std::runtime_error)
    {
        cmnDataSerializeText(pathToUTF8(data), outputStream, delimiter);
    }

    static void DeSerializeText(DataType& data,
                                std::istream & inputStream,
                                const char delimiter = ',')
        CISST_THROW(std::runtime_error)
    {
        std::string output;
        cmnDataDeSerializeText(output, inputStream, delimiter);
        data = pathFromUTF8(output);
    }

    static bool ScalarNumberIsFixed([[maybe_unused]] const std::filesystem::path& data) {
        return true;
    }

    static size_t ScalarNumber([[maybe_unused]] const std::filesystem::path& data) {
        return 0;
    }

    static std::string ScalarDescription(
        [[maybe_unused]] const std::filesystem::path& data,
        [[maybe_unused]] const size_t index,
        [[maybe_unused]] const std::string& userDescription
    ) CISST_THROW(std::out_of_range) {
        cmnThrow(std::out_of_range("cmnDataScalarDescription: std::filesystem has no scalar"));
        return "n/a";
    }

    static double Scalar(
        [[maybe_unused]] const std::filesystem::path& data,
        [[maybe_unused]] const size_t index
    ) CISST_THROW(std::out_of_range) {
        cmnThrow(std::out_of_range("cmnDataScalar: std::filesystem has no scalar"));
        return 1.234;
    }
};

#endif // _cmnDataFunctionsPath_h
