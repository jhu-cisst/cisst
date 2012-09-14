/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2011-06-27

  (C) Copyright 2011-2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstCommon/cmnDataFunctions.h>
#include <cisstCommon/cmnAssert.h>

cmnDataFormat::cmnDataFormat(void)
{
    // determine size of pointer
    if (sizeof(void *) == 4) {
        this->WordSizeMember = CMN_DATA_32_BITS;
    } else if (sizeof(void *)) {
        this->WordSizeMember = CMN_DATA_64_BITS;
    } else {
        CMN_ASSERT(false);
    }
    // determine endianness
    int one = 1;
    char * part = reinterpret_cast<char *>(&one);
    if (part[0] == 1) {
        this->EndiannessMember = CMN_DATA_LITTLE_ENDIAN;
    } else {
        this->EndiannessMember = CMN_DATA_BIG_ENDIAN;
    }
    // determine size of size_t
    if (sizeof(size_t) == 4) {\
        this->SizeTSizeMember = CMN_DATA_SIZE_T_SIZE_32;
    } else if (sizeof(size_t) == 8) {
        this->SizeTSizeMember = CMN_DATA_SIZE_T_SIZE_64;
    } else {
        CMN_ASSERT(false);
    }
}



// size_t specializations
std::string cmnDataScalarDescription_size_t(const size_t & CMN_UNUSED(data), const size_t CMN_UNUSED(index), const std::string & userDescription)
    throw (std::out_of_range) {
    return userDescription;
}

double cmnDataScalar_size_t(const size_t & data, const size_t CMN_UNUSED(index))
    throw (std::out_of_range) {
    return static_cast<double>(data);
}

size_t cmnDataScalarNumber_size_t(const size_t & CMN_UNUSED(data)) {
    return 1;
}

bool cmnDataScalarNumberIsFixed_size_t(const size_t & CMN_UNUSED(data)) {
    return true;
}

void cmnDataSerializeBinary_size_t(std::ostream & outputStream,
                                   const size_t & data)
    throw (std::runtime_error)
{
    outputStream.write(reinterpret_cast<const char *>(&data),
                       sizeof(size_t));
    if (outputStream.fail()) {
        cmnThrow("cmnDataSerializeBinary(type): error occured with std::ostream::write");
    }
}

void cmnDataDeSerializeBinary_size_t(std::istream & inputStream,
                                     size_t & data,
                                     const cmnDataFormat & remoteFormat,
                                     const cmnDataFormat & localFormat) throw (std::runtime_error)
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

void cmnDataSerializeText_size_t(std::ostream & outputStream,
                                 const size_t & data,
                                 const char CMN_UNUSED(delimiter))
    throw (std::runtime_error)
{
    outputStream << data;
    if (outputStream.fail()) {
            cmnThrow("cmnDataSerializeText_size_t: error occured with std::ostream::write");
        }
}

std::string cmnDataSerializeTextDescription_size_t(const size_t & CMN_UNUSED(data),
                                                   const char CMN_UNUSED(delimiter),
                                                   const std::string & userDescription)
{
    return userDescription;
}

void cmnDataDeSerializeText_size_t(std::istream & inputStream,
                                   size_t & data,
                                   const char CMN_UNUSED(delimiter)) throw (std::runtime_error)
{
    inputStream >> data;
    if (inputStream.fail()) {
        cmnThrow("cmnDataDeSerializeText_size_t: error occured with std::istream::read");
    }
}



// std::string specialization
void cmnDataSerializeBinary(std::ostream & outputStream,
                            const std::string & data) throw (std::runtime_error)
{
    cmnDataSerializeBinary_size_t(outputStream, data.size());
    outputStream.write(data.data(), data.size());
    if (outputStream.fail()) {
        cmnThrow("cmnDataSerializeBinary(std::string): error occured with std::ostream::write");
    }
}

void cmnDataDeSerializeBinary(std::istream & inputStream,
                              std::string & data,
                              const cmnDataFormat & remoteFormat,
                              const cmnDataFormat & localFormat) throw (std::runtime_error)
{
    size_t size;
    // retrieve size of string
    cmnDataDeSerializeBinary_size_t(inputStream, size, remoteFormat, localFormat);
    data.resize(size);
    // this const_cast is a bit alarming, lets be verbose until we are sure this is safe
    std::cerr << CMN_LOG_DETAILS << " - not really sure about the following const cast" << std::endl;
    inputStream.read(const_cast<char *>(data.data()), size);
    if (inputStream.fail()) {
        cmnThrow("cmnDataDeSerializeBinary(std::string): error occured with std::istream::read");
    }
}
