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
#ifndef _mtsDataFunctionsChrono_h
#define _mtsDataFunctionsChrono_h

#include <chrono>

#include <cisstCommon/cmnDataFunctions.h>
#include <cisstOSAbstraction/osaTime.h>

// always include last
#include <cisstMultiTask/mtsExport.h>

using Clock_t = std::chrono::steady_clock;
using time_point_t = std::chrono::time_point<Clock_t>;
using duration_t = Clock_t::duration;

template <>
class cmnData<duration_t, void>
{
public:
    typedef duration_t DataType;

    CMN_DATA_IS_SPECIALIZED_TRUE(duration_t)
    CMN_DATA_COPY_USING_ASSIGN(duration_t);
    CMN_DATA_SERIALIZE_DESCRIPTION(duration_t, str);

    static std::string HumanReadable(const DataType& data) {
        return std::to_string(durationToSeconds(data));
    }

    static size_t SerializeBinaryByteSize(const DataType& data) {
        return cmnData<Clock_t::rep>::SerializeBinaryByteSize(data.count());
    }

    static size_t SerializeBinary(const DataType& data,
                                  char * buffer, size_t bufferSize) {
        return cmnData<Clock_t::rep>::SerializeBinary(
            data.count(),
            buffer,
            bufferSize
        );
    }

    static size_t DeSerializeBinary(DataType& data, const char * buffer, size_t bufferSize,
                                    const cmnDataFormat & localFormat,
                                    const cmnDataFormat & remoteFormat)
    {
        Clock_t::rep ticks;
        size_t ret = cmnData<Clock_t::rep>::DeSerializeBinary(
            ticks,
            buffer,
            bufferSize,
            localFormat,
            remoteFormat
        );
        data = duration_t{Clock_t::duration{ticks}};
        return ret;
    }

    static void SerializeBinary(const DataType& data,
                                std::ostream & outputStream)
        CISST_THROW(std::runtime_error)
    {
        cmnData<Clock_t::rep>::SerializeBinary(data.count(), outputStream);
    }

    static void DeSerializeBinary(DataType& data,
                                  std::istream & inputStream,
                                  const cmnDataFormat & localFormat,
                                  const cmnDataFormat & remoteFormat)
        CISST_THROW(std::runtime_error)
    {
        Clock_t::rep value;
        cmnData<Clock_t::rep>::DeSerializeBinary(value, inputStream, localFormat, remoteFormat);
        data = Clock_t::duration{value};
    }

    static void SerializeText(const DataType& data,
                              std::ostream & outputStream,
                              const char delimiter = ',')
        CISST_THROW(std::runtime_error)
    {
        cmnData<Clock_t::rep>::SerializeText(data.count(), outputStream, delimiter);
    }

    static void DeSerializeText(DataType& data,
                                std::istream & inputStream,
                                const char delimiter = ',')
        CISST_THROW(std::runtime_error)
    {
        Clock_t::rep value;
        cmnData<Clock_t::rep>::DeSerializeText(value, inputStream, delimiter);
        data = Clock_t::duration{value};
    }

    static bool ScalarNumberIsFixed([[maybe_unused]] const DataType& data) {
        return true;
    }

    static size_t ScalarNumber([[maybe_unused]] const DataType& data) {
        return 0;
    }

    static std::string ScalarDescription(
        [[maybe_unused]] const DataType& data,
        [[maybe_unused]] const size_t index,
        [[maybe_unused]] const std::string& userDescription
    ) CISST_THROW(std::out_of_range) {
        cmnThrow(std::out_of_range("cmnDataScalarDescription: duration has no scalar"));
        return "n/a";
    }

    static double Scalar(
        [[maybe_unused]] const DataType& data,
        [[maybe_unused]] const size_t index
    ) CISST_THROW(std::out_of_range) {
        cmnThrow(std::out_of_range("cmnDataScalar: duration has no scalar"));
        return 1.234;
    }
};

template <>
class cmnData<time_point_t, void>
{
public:
    typedef time_point_t DataType;

    CMN_DATA_IS_SPECIALIZED_TRUE(time_point_t)
    CMN_DATA_COPY_USING_ASSIGN(time_point_t);
    CMN_DATA_SERIALIZE_DESCRIPTION(time_point_t, str);

    static std::string HumanReadable(const DataType& data) {
        return std::to_string(durationToSeconds(data.time_since_epoch()));
    }

    static size_t SerializeBinaryByteSize(const DataType& data) {
        return cmnData<duration_t>::SerializeBinaryByteSize(data.time_since_epoch());
    }

    static size_t SerializeBinary(const DataType& data,
                                  char * buffer, size_t bufferSize) {
        return cmnData<duration_t>::SerializeBinary(
            data.time_since_epoch(),
            buffer,
            bufferSize
        );
    }

    static size_t DeSerializeBinary(DataType& data, const char * buffer, size_t bufferSize,
                                    const cmnDataFormat & localFormat,
                                    const cmnDataFormat & remoteFormat)
    {
        duration_t duration;
        size_t ret = cmnData<duration_t>::DeSerializeBinary(
            duration,
            buffer,
            bufferSize,
            localFormat,
            remoteFormat
        );
        data = time_point_t{duration};
        return ret;
    }

    static void SerializeBinary(const DataType& data,
                                std::ostream & outputStream)
        CISST_THROW(std::runtime_error)
    {
        cmnData<duration_t>::SerializeBinary(data.time_since_epoch(), outputStream);
    }

    static void DeSerializeBinary(DataType& data,
                                  std::istream & inputStream,
                                  const cmnDataFormat & localFormat,
                                  const cmnDataFormat & remoteFormat)
        CISST_THROW(std::runtime_error)
    {
        duration_t value;
        cmnData<duration_t>::DeSerializeBinary(value, inputStream, localFormat, remoteFormat);
        data = Clock_t::time_point{value};
    }

    static void SerializeText(const DataType& data,
                              std::ostream & outputStream,
                              const char delimiter = ',')
        CISST_THROW(std::runtime_error)
    {
        cmnData<duration_t>::SerializeText(data.time_since_epoch(), outputStream, delimiter);
    }

    static void DeSerializeText(DataType& data,
                                std::istream & inputStream,
                                const char delimiter = ',')
        CISST_THROW(std::runtime_error)
    {
        duration_t value;
        cmnData<duration_t>::DeSerializeText(value, inputStream, delimiter);
        data = Clock_t::time_point{value};
    }

    static bool ScalarNumberIsFixed([[maybe_unused]] const DataType& data) {
        return true;
    }

    static size_t ScalarNumber([[maybe_unused]] const DataType& data) {
        return 0;
    }

    static std::string ScalarDescription(
        [[maybe_unused]] const DataType& data,
        [[maybe_unused]] const size_t index,
        [[maybe_unused]] const std::string& userDescription
    ) CISST_THROW(std::out_of_range) {
        cmnThrow(std::out_of_range("cmnDataScalarDescription: time_point has no scalar"));
        return "n/a";
    }

    static double Scalar(
        [[maybe_unused]] const DataType& data,
        [[maybe_unused]] const size_t index
    ) CISST_THROW(std::out_of_range) {
        cmnThrow(std::out_of_range("cmnDataScalar: time_point has no scalar"));
        return 1.234;
    }
};

#endif // _mtsDataFunctionsChrono_h
