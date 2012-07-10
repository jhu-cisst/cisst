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



// std::string specialization
void cmnDataSerializeBinary(std::ostream & outputStream,
                            const std::string & data) throw (std::runtime_error)
{
    cmnDataSerializeBinary(outputStream, data.size());
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
    cmnDataDeSerializeBinary(inputStream, size, remoteFormat, localFormat);
    data.resize(size);
    // this const_cast is a bit alarming, lets be verbose until we are sure this is safe
    std::cerr << CMN_LOG_DETAILS << " - not really sure about the following const cast" << std::endl;
    inputStream.read(const_cast<char *>(data.data()), size);
    if (inputStream.fail()) {
        cmnThrow("cmnDataDeSerializeBinary(std::string): error occured with std::istream::read");
    }
}
