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
#ifndef _cmnDataFormat_h
#define _cmnDataFormat_h

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnThrow.h>

// Always include last
#include <cisstCommon/cmnExport.h>

class CISST_EXPORT cmnDataFormat
{
    cmnDataFormat(const cmnDataFormat & other);
    friend class cmnDataTest;

public:
    typedef enum {CMN_DATA_32_BITS, CMN_DATA_64_BITS} WordSize;
    typedef enum {CMN_DATA_LITTLE_ENDIAN, CMN_DATA_BIG_ENDIAN} Endianness;
    typedef enum {CMN_DATA_SIZE_T_SIZE_32, CMN_DATA_SIZE_T_SIZE_64} SizeTSize;

    /*! Constructor.  By default the constructor will determine the
      word size, endian-ness and size of size_t for the current
      binary. */
    cmnDataFormat(void);

    inline const WordSize & GetWordSize(void) const {
        return this->WordSizeMember;
    }

    inline const Endianness & GetEndianness(void) const {
        return this->EndiannessMember;
    }

    inline const SizeTSize & GetSizeTSize(void) const {
        return this->SizeTSizeMember;
    }

private:
    WordSize WordSizeMember;
    Endianness EndiannessMember;
    SizeTSize SizeTSizeMember;
};


template <class _elementType, size_t _sizeInBytes>
class cmnDataByteSwapClass
{
    // this method is private to make sure a compilation error will
    // happen if ones try to swap bytes on unsupported sizes
    inline static void Execute(_elementType & CMN_UNUSED(data)) CISST_THROW(std::runtime_error) {
        cmnThrow("cmnDataByteSwap: a partial specialization should be called!");
    }
};

template <class _elementType>
class cmnDataByteSwapClass<_elementType, 1>
{
    // this method is private to make sure a compilation error will
    // happen if ones try to swap bytes on a one byte object
    inline static void Execute(_elementType & CMN_UNUSED(data)) CISST_THROW(std::runtime_error) {
    }
};

template <class _elementType>
class cmnDataByteSwapClass<_elementType, 2>
{
public:
    inline static void Execute(_elementType & data) CISST_THROW(std::runtime_error) {
        *(unsigned short *)&(data) = ( ((*(unsigned short *)&(data) & 0xff) << 8) |
                                       (*(unsigned short *)&(data) >> 8) );
    }
};

template <class _elementType>
class cmnDataByteSwapClass<_elementType, 4>
{
public:
    inline static void Execute(_elementType & data) CISST_THROW(std::runtime_error) {
        *(unsigned int *)&(data) = ( ((*(unsigned int *)&(data) & 0xff000000) >> 24) |
                                     ((*(unsigned int *)&(data) & 0x00ff0000) >>  8) |
                                     ((*(unsigned int *)&(data) & 0x0000ff00) <<  8) |
                                     ((*(unsigned int *)&(data) & 0x000000ff) << 24) );
    }
};

template <class _elementType>
class cmnDataByteSwapClass<_elementType, 8>
{
public:
    inline static void Execute(_elementType & data) CISST_THROW(std::runtime_error) {
        *(unsigned long long int *)&(data) = ( ((*(unsigned long long int *)&(data) & 0xff00000000000000ULL) >> 56) |
                                               ((*(unsigned long long int *)&(data) & 0x00ff000000000000ULL) >> 40) |
                                               ((*(unsigned long long int *)&(data) & 0x0000ff0000000000ULL) >> 24) |
                                               ((*(unsigned long long int *)&(data) & 0x000000ff00000000ULL) >> 8) |
                                               ((*(unsigned long long int *)&(data) & 0x00000000ff000000ULL) << 8) |
                                               ((*(unsigned long long int *)&(data) & 0x0000000000ff0000ULL) << 24) |
                                               ((*(unsigned long long int *)&(data) & 0x000000000000ff00ULL) << 40) |
                                               ((*(unsigned long long int *)&(data) & 0x00000000000000ffULL) << 56) );
    }
};

template <class _elementType>
void cmnDataByteSwap(_elementType & data) {
    cmnDataByteSwapClass<_elementType, sizeof(_elementType)>::Execute(data);
}

#endif // _cmnDataFormat_h
