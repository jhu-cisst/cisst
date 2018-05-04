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

#include <cisstCommon/cmnDataFunctions.h>
#include <cisstCommon/cmnAssert.h>
#include <limits>

// size_t specializations
void cmnDataCopy_size_t(size_t & data, const size_t & source) {
    data = source;
}

std::string cmnDataSerializeDescription_size_t(const size_t & CMN_UNUSED(data),
                                               const char CMN_UNUSED(delimiter),
                                               const std::string & userDescription)
{
    return (userDescription == "" ? "{s_t}" : (userDescription + ":{s_t}"));
}

size_t cmnDataSerializeBinaryByteSize_size_t(const size_t & CMN_UNUSED(data)) {
    return sizeof(size_t);
}

size_t cmnDataSerializeBinary_size_t(const size_t & data,
                                     char * buffer, size_t bufferSize) {
    if (bufferSize < sizeof(size_t)) {
        return 0;
    }
    *(reinterpret_cast<size_t *>(buffer)) = data;
    return sizeof(size_t);
}

size_t cmnDataDeSerializeBinary_size_t(size_t & data, const char * buffer, size_t bufferSize,
                                       const cmnDataFormat & localFormat,
                                       const cmnDataFormat & remoteFormat)
{
    // first case, both use same size
    if (remoteFormat.GetSizeTSize() == localFormat.GetSizeTSize()) {
        const size_t sizeOfData = sizeof(size_t);
        if (bufferSize < sizeOfData) {
            return 0;
        }
        data = *(reinterpret_cast<const size_t *>(buffer));
        bufferSize -= sizeOfData;
        return sizeOfData;
    }
    // different sizes
    // local is 64, hence remote is 32
    if (localFormat.GetSizeTSize() == cmnDataFormat::CMN_DATA_SIZE_T_SIZE_64) {
        unsigned int tmp;
        const size_t sizeOfData = sizeof(unsigned int);
        if (bufferSize < sizeOfData) {
            return 0;
        }
        tmp = *(reinterpret_cast<const unsigned int *>(buffer));
        bufferSize -= sizeOfData;
        data = tmp;
        return sizeOfData;
    }
    // local is 32, hence remote is 64
    if (localFormat.GetSizeTSize() == cmnDataFormat::CMN_DATA_SIZE_T_SIZE_32) {
        unsigned long long int tmp;
        const size_t sizeOfData = sizeof(unsigned long long int);
        if (bufferSize < sizeOfData) {
            return 0;
        }
        tmp = *(reinterpret_cast<const unsigned long long int *>(buffer));
        bufferSize -= sizeOfData;
        if (tmp > std::numeric_limits<size_t>::max()) {
            cmnThrow("cmnDataDeSerializeBinary(size_t): received a size_t larger than what can be handled on 32 bits");
        }
        data = static_cast<size_t>(tmp); // this should be safe now
        return sizeOfData;
    }
    return 0;
}

void cmnDataSerializeBinary_size_t(const size_t & data,
                                   std::ostream & outputStream)
    CISST_THROW(std::runtime_error)
{
    outputStream.write(reinterpret_cast<const char *>(&data),
                       sizeof(size_t));
    if (outputStream.fail()) {
        cmnThrow("cmnDataSerializeBinary(type): error occured with std::ostream::write");
    }
}

void cmnDataDeSerializeBinary_size_t(size_t & data,
                                     std::istream & inputStream,
                                     const cmnDataFormat & localFormat,
                                     const cmnDataFormat & remoteFormat)
    CISST_THROW(std::runtime_error)
{
    // first case, both use same size
    if (remoteFormat.GetSizeTSize() == localFormat.GetSizeTSize()) {
        inputStream.read(reinterpret_cast<char *>(&data),
                         sizeof(size_t));
        if (inputStream.fail()) {
            cmnThrow("cmnDataDeSerializeBinary(size_t): error occured with std::istream::read");
        }
        return;
    }
    // different sizes
    // local is 64, hence remote is 32
    if (localFormat.GetSizeTSize() == cmnDataFormat::CMN_DATA_SIZE_T_SIZE_64) {
        unsigned int tmp;
        inputStream.read(reinterpret_cast<char *>(&tmp),
                         sizeof(tmp));
        if (inputStream.fail()) {
            cmnThrow("cmnDataDeSerializeBinary(size_t): error occured with std::istream::read");
        }
        data = tmp;
        return;
    }
    // local is 32, hence remote is 64
    if (localFormat.GetSizeTSize() == cmnDataFormat::CMN_DATA_SIZE_T_SIZE_32) {
        unsigned long long int tmp;
        inputStream.read(reinterpret_cast<char *>(&tmp),
                         sizeof(tmp));
        if (inputStream.fail()) {
            cmnThrow("cmnDataDeSerializeBinary(size_t): error occured with std::istream::read");
        }
        if (tmp > std::numeric_limits<size_t>::max()) {
            cmnThrow("cmnDataDeSerializeBinary(size_t): received a size_t larger than what can be handled on 32 bits");
        }
        data = static_cast<size_t>(tmp); // this should be safe now
        return;
    }
}

void cmnDataSerializeText_size_t(const size_t & data,
                                 std::ostream & outputStream,
                                 const char CMN_UNUSED(delimiter))
    CISST_THROW(std::runtime_error)
{
    outputStream << data;
    if (outputStream.fail()) {
        cmnThrow("cmnDataSerializeText_size_t: error occured with std::ostream::write");
    }
}

void cmnDataDeSerializeText_size_t(size_t & data,
                                   std::istream & inputStream,
                                   const char CMN_UNUSED(delimiter)) CISST_THROW(std::runtime_error)
{
    inputStream >> data;
    if (inputStream.fail()) {
        cmnThrow("cmnDataDeSerializeText_size_t: error occured with std::istream::read");
    }
}

std::string cmnDataHumanReadable_size_t(const size_t & data)
{
    std::stringstream stringStream;
    stringStream << data;
    return stringStream.str();
}

std::string cmnDataScalarDescription_size_t(const size_t & CMN_UNUSED(data), const size_t CMN_UNUSED(index), const std::string & userDescription)
    CISST_THROW(std::out_of_range)
{
    return (userDescription == "" ? "{s_t}" : (userDescription + ":{s_t}"));
}

double cmnDataScalar_size_t(const size_t & data, const size_t CMN_UNUSED(index))
    CISST_THROW(std::out_of_range)
{
    return static_cast<double>(data);
}

size_t cmnDataScalarNumber_size_t(const size_t & CMN_UNUSED(data))
{
    return 1;
}

bool cmnDataScalarNumberIsFixed_size_t(const size_t & CMN_UNUSED(data))
{
    return true;
}

void cmnDataDeSerializeTextDelimiter(std::istream & inputStream, const char delimiter, const char * className)
    CISST_THROW(std::runtime_error)
{
    char delimiterRead;
    // look for the delimiter
    if (!isspace(delimiter)) {
        inputStream >> delimiterRead;
        if (inputStream.fail()) {
            std::string message("cmnDataDeSerializeTextDelimiter: ");
            message.append(className);
            message.append(", error occured with std::istream::read");
            cmnThrow(message);
        }
        if (delimiterRead != delimiter) {
            std::string message("cmnDataDeSerializeTextDelimiter: ");
            message.append(className);
            message.append(", expected delimiter '");
            message.push_back(delimiter);
            message.append("', found '");
            message.push_back(delimiterRead);
            message.append("'");
            cmnThrow(message);
        }
    }
}
