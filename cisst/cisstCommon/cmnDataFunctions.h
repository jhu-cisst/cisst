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


#pragma once
#ifndef _cmnDataFunctions_h
#define _cmnDataFunctions_h

#include <string.h> // for memcpy
#include <iostream>
#include <limits>
#include <cisstCommon/cmnThrow.h>

// always include last
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
    inline static void Execute(_elementType & data) throw (std::runtime_error) {
        cmnThrow("cmnDataByteSwap: a partial specialization should be called!");
    }
};

template <class _elementType>
class cmnDataByteSwapClass<_elementType, 1>
{
    // this method is private to make sure a compilation error will
    // happen if ones try to swap bytes on a one byte object
    inline static void Execute(_elementType & data) throw (std::runtime_error) {
    }
};

template <class _elementType>
class cmnDataByteSwapClass<_elementType, 2>
{
public:
    inline static void Execute(_elementType & data) throw (std::runtime_error) {
        *(unsigned short *)&(data) = ( ((*(unsigned short *)&(data) & 0xff) << 8) |
                                       (*(unsigned short *)&(data) >> 8) );
    }
};

template <class _elementType>
class cmnDataByteSwapClass<_elementType, 4>
{
public:
    inline static void Execute(_elementType & data) throw (std::runtime_error) {
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
    inline static void Execute(_elementType & data) throw (std::runtime_error) {
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


/* Native types to overload:
   bool
   char
   unsigned char
   short
   unsigned short
   int
   unsigned int
   long long int
   unsignef long long int
   float
   double

   Standard C++ library:
   std::string
*/

/*! Macro to overload the function cmnDataCopy using the assignement
  operator to copy from source to destination.  This is a
  appropriate for data types without any dynamic memory allocations.
  For types using dynamic memory allocations or large blocks of
  memory, users should overload the cmnDataCopy function using a
  more efficient approach (memcpy, ...). */
#define CMN_DATA_COPY_USING_ASSIGN(_type)                               \
    inline void cmnDataCopy(_type & destination,                        \
                            const _type & source) {                     \
        destination = source;                                           \
    }


/*! Macro to overload the function cmnDataSerializeBinary using a cast
  to char pointer and assuming that sizeof reports the real size of
  the object to be serialized.  Please note that this method will
  not work with data object relying on dynamic memory allocation
  (pointers) and might also fail if you are using a struct/class
  your compiler can pad. */
#define CMN_DATA_SERIALIZE_BINARY_STREAM_USING_CAST_TO_CHAR(_type)      \
    inline void cmnDataSerializeBinary(std::ostream & outputStream,     \
                                       const _type & data)              \
        throw (std::runtime_error)                                      \
    {                                                                   \
        outputStream.write(reinterpret_cast<const char *>(&data),       \
                           sizeof(_type));                              \
        if (outputStream.fail()) {                                      \
            cmnThrow("cmnDataSerializeBinary(" #_type "): error occured with std::ostream::write"); \
        }                                                               \
    }


#define CMN_DATA_DE_SERIALIZE_BINARY_STREAM_USING_CAST_TO_CHAR_NO_BYTE_SWAP(_type)   \
    inline void cmnDataDeSerializeBinary(std::istream & inputStream,    \
                                         _type & data,                  \
                                         const cmnDataFormat & CMN_UNUSED(remoteFormat), \
                                         const cmnDataFormat & CMN_UNUSED(localFormat)) \
        throw (std::runtime_error)                                      \
    {                                                                   \
        inputStream.read(reinterpret_cast<char *>(&data),               \
                         sizeof(_type));                                \
        if (inputStream.fail()) {                                       \
            cmnThrow("cmnDataDeSerializeBinary(" #_type "): error occured with std::istream::read"); \
        }                                                               \
    }


#define CMN_DATA_DE_SERIALIZE_BINARY_STREAM_USING_CAST_TO_CHAR_AND_BYTE_SWAP(_type) \
    inline void cmnDataDeSerializeBinary(std::istream & inputStream,    \
                                         _type & data,                  \
                                         const cmnDataFormat & remoteFormat, \
                                         const cmnDataFormat & localFormat) \
        throw (std::runtime_error)                                      \
    {                                                                   \
        inputStream.read(reinterpret_cast<char *>(&data),               \
                         sizeof(_type));                                \
        if (inputStream.fail()) {                                       \
            cmnThrow("cmnDataDeSerializeBinary(" #_type "): error occured with std::istream::read"); \
        }                                                               \
        if (remoteFormat.GetEndianness() != localFormat.GetEndianness()) { \
            cmnDataByteSwap(data);                                      \
        }                                                               \
    }


/*! Returns the number of bytes required to serialize the data
  object. */
inline size_t cmnDataSerializeBinaryByteSize(const double & CMN_UNUSED(data))
{
    return sizeof(double);
}

/*! Serialize to a char buffer.  The caller must provide the size of
  the buffer so the function can check if the buffer is large enough.
  If the buffer is large enough, the function modifies the buffer
  pointer to point to the next free char and returns the number of
  bytes written.  If the buffer is not large enough, the buffer
  pointer is not modified and the function returns 0. */
inline size_t cmnDataSerializeBinary(char * buffer, size_t bufferSize,
                                     const size_t & data) {
    if (bufferSize < sizeof(size_t)) {
        return 0;
    }
    *buffer = *(char *)&data;
    buffer += sizeof(size_t);
    return sizeof(size_t);
}


inline size_t cmnDataSerializeBinaryByteSize(const std::string & data) {
    return (sizeof(size_t) + data.size() * sizeof(char));
}


inline size_t cmnDataSerializeBinary(char * buffer, size_t bufferSize,
                                     const std::string & data) {
    const size_t dataSize = cmnDataSerializeBinaryByteSize(data);
    if (bufferSize < dataSize) {
        return 0;
    }
    // serialize length
    const size_t length = data.size();
    cmnDataSerializeBinary(buffer, bufferSize, length);
    // serialize string itself
    memcpy(buffer, data.data(), length);
    buffer += length;
    return length;
}


/*! Macro to overload the function cmnDataSerializeText using the C++
  stream out operator. */
#define CMN_DATA_SERIALIZE_TEXT_USING_STREAM_OUT(_type)                 \
    inline void cmnDataSerializeText(std::ostream & outputStream,       \
                                     const _type & data,                \
                                     const char CMN_UNUSED(delimiter))  \
        throw (std::runtime_error)                                      \
    {                                                                   \
        outputStream << data;                                           \
        if (outputStream.fail()) {                                      \
            cmnThrow("cmnDataSerializeText(" #_type "): error occured with std::ostream::write"); \
        }                                                               \
    }


/*! Macro to overload the function cmnDataSerializeTextDescription. */
#define CMN_DATA_SERIALIZE_TEXT_DESCRIPTION(_type, _description)    \
    inline std::string cmnDataSerializeTextDescription(const _type & CMN_UNUSED(data), \
                                                       const char CMN_UNUSED(delimiter), const std::string & userDescription = #_description) \
    {                                                                   \
        return userDescription;                                         \
    }


/*! Macro to overload the function cmnDataDeSerializeText using the C++
  stream in operator. */
#define CMN_DATA_DE_SERIALIZE_TEXT_USING_STREAM_IN(_type)               \
    inline void cmnDataDeSerializeText(std::istream & inputStream, \
                                       _type & data,                    \
                                       const char CMN_UNUSED(delimiter)) \
        throw (std::runtime_error)                                      \
    {                                                                   \
        inputStream >> data;                                            \
        if (inputStream.fail()) {                                       \
            cmnThrow("cmnDataDeSerializeText(" #_type "): error occured with std::istream::read"); \
        }                                                               \
    }


/*! Macro to overload the function cmnDataScalarDescription using the type
  name itself.  For example, for a double it will return the string
  "double". */
#define CMN_DATA_SCALAR_DESCRIPTION(_type, _description)                \
    inline std::string cmnDataScalarDescription(const _type & CMN_UNUSED(data), const size_t CMN_UNUSED(index), const std::string & userDescription = #_description) \
        throw (std::out_of_range) {                                     \
        return userDescription;                                         \
    }

/*! Macro to overload the function cmnDataScalar using a static_cast. */
#define CMN_DATA_SCALAR_USING_STATIC_CAST(_type)                        \
    inline double cmnDataScalar(const _type & data, const size_t CMN_UNUSED(index)) \
        throw (std::out_of_range) {                                     \
        return static_cast<double>(data);                               \
    }

/*! Macro to overload the function cmnDataScalarNumber, returns 1. */
#define CMN_DATA_SCALAR_NUMBER_IS_ONE(_type)                          \
    inline size_t cmnDataScalarNumber(const _type & CMN_UNUSED(data)) { \
        return 1;                                                     \
    }

#define CMN_DATA_SCALAR_NUMBER_IS_FIXED_TRUE(_type)                     \
    inline bool cmnDataScalarNumberIsFixed(const _type & CMN_UNUSED(data)) { \
        return true;                                                    \
    }

#define CMN_DATA_INSTANTIATE_ALL_NO_BYTE_SWAP(type, description)        \
    CMN_DATA_COPY_USING_ASSIGN(type);                                   \
    CMN_DATA_SERIALIZE_BINARY_STREAM_USING_CAST_TO_CHAR(type)           \
    CMN_DATA_SERIALIZE_TEXT_DESCRIPTION(type, description)              \
    CMN_DATA_DE_SERIALIZE_BINARY_STREAM_USING_CAST_TO_CHAR_NO_BYTE_SWAP(type) \
    CMN_DATA_SERIALIZE_TEXT_USING_STREAM_OUT(type)                      \
    CMN_DATA_DE_SERIALIZE_TEXT_USING_STREAM_IN(type)                    \
    CMN_DATA_SCALAR_DESCRIPTION(type, description)                      \
    CMN_DATA_SCALAR_USING_STATIC_CAST(type)                             \
    CMN_DATA_SCALAR_NUMBER_IS_ONE(type)                                 \
    CMN_DATA_SCALAR_NUMBER_IS_FIXED_TRUE(type)

#define CMN_DATA_INSTANTIATE_ALL_BYTE_SWAP(type, description)           \
    CMN_DATA_COPY_USING_ASSIGN(type);                                   \
    CMN_DATA_SERIALIZE_BINARY_STREAM_USING_CAST_TO_CHAR(type)           \
    CMN_DATA_SERIALIZE_TEXT_DESCRIPTION(type, description)              \
    CMN_DATA_DE_SERIALIZE_BINARY_STREAM_USING_CAST_TO_CHAR_AND_BYTE_SWAP(type) \
    CMN_DATA_SERIALIZE_TEXT_USING_STREAM_OUT(type)                      \
    CMN_DATA_DE_SERIALIZE_TEXT_USING_STREAM_IN(type)                    \
    CMN_DATA_SCALAR_DESCRIPTION(type, description)                      \
    CMN_DATA_SCALAR_USING_STATIC_CAST(type)                             \
    CMN_DATA_SCALAR_NUMBER_IS_ONE(type)                                 \
    CMN_DATA_SCALAR_NUMBER_IS_FIXED_TRUE(type)

CMN_DATA_INSTANTIATE_ALL_NO_BYTE_SWAP(bool, b);
CMN_DATA_INSTANTIATE_ALL_NO_BYTE_SWAP(char, c);
CMN_DATA_INSTANTIATE_ALL_NO_BYTE_SWAP(unsigned char, uc);

CMN_DATA_INSTANTIATE_ALL_BYTE_SWAP(short, s);
CMN_DATA_INSTANTIATE_ALL_BYTE_SWAP(unsigned short, us);
CMN_DATA_INSTANTIATE_ALL_BYTE_SWAP(int, i);
CMN_DATA_INSTANTIATE_ALL_BYTE_SWAP(unsigned int, ui);
CMN_DATA_INSTANTIATE_ALL_BYTE_SWAP(long long int, lli);
CMN_DATA_INSTANTIATE_ALL_BYTE_SWAP(unsigned long long int, ulli);
CMN_DATA_INSTANTIATE_ALL_BYTE_SWAP(float, f);
CMN_DATA_INSTANTIATE_ALL_BYTE_SWAP(double, d);


// size_t "specialization", we can't really specialize for size_t as
// the type is not always a built-in type.  For example the MS
// compiler uses typedef.  So we create a set of "parallel" functions
// with the suffix _size_t
std::string CISST_EXPORT cmnDataScalarDescription_size_t(const size_t & CMN_UNUSED(data),
                                                         const size_t CMN_UNUSED(index),
                                                         const std::string & userDescription = "s_t") throw (std::out_of_range);

double CISST_EXPORT cmnDataScalar_size_t(const size_t & data,
                                         const size_t CMN_UNUSED(index)) throw (std::out_of_range);

size_t CISST_EXPORT cmnDataScalarNumber_size_t(const size_t & CMN_UNUSED(data));

bool CISST_EXPORT cmnDataScalarNumberIsFixed_size_t(const size_t & CMN_UNUSED(data));

void CISST_EXPORT cmnDataSerializeBinary_size_t(std::ostream & outputStream,
                                                const size_t & data) throw (std::runtime_error);

void CISST_EXPORT cmnDataDeSerializeBinary_size_t(std::istream & inputStream,
                                                  size_t & data,
                                                  const cmnDataFormat & remoteFormat,
                                                  const cmnDataFormat & localFormat) throw (std::runtime_error);

void CISST_EXPORT cmnDataSerializeText_size_t(std::ostream & outputStream,
                                              const size_t & data,
                                              const char CMN_UNUSED(delimiter)) throw (std::runtime_error);

std::string CISST_EXPORT cmnDataSerializeTextDescription_size_t(const size_t & CMN_UNUSED(data),
                                                                const char CMN_UNUSED(delimiter),
                                                                const std::string & userDescription = "s_t");

void CISST_EXPORT cmnDataDeSerializeText_size_t(std::istream & inputStream,
                                                size_t & data,
                                                const char CMN_UNUSED(delimiter)) throw (std::runtime_error);

// std::string specialization
CMN_DATA_COPY_USING_ASSIGN(std::string);
void CISST_EXPORT cmnDataSerializeBinary(std::ostream & outputStream,
                                         const std::string & data) throw (std::runtime_error);

void CISST_EXPORT cmnDataDeSerializeBinary(std::istream & inputStream,
                                           std::string & data,
                                           const cmnDataFormat & remoteFormat,
                                           const cmnDataFormat & localFormat) throw (std::runtime_error);


// this should be define as a macro that can be used for all types without scalars
inline bool cmnDataScalarNumberIsFixed(const std::string & CMN_UNUSED(data)) {
    return true;
}

inline size_t cmnDataScalarNumber(const std::string & CMN_UNUSED(data)) {
    return 0;
}

inline std::string cmnDataScalarDescription(const std::string & CMN_UNUSED(data), const size_t & CMN_UNUSED(index),
                                            const std::string & CMN_UNUSED(userDescription) = "") throw (std::out_of_range) {
    cmnThrow(std::out_of_range("cmnDataScalarDescription: std::string has no scalar"));
    return "n/a";
}

#endif // _cmnDataFunctions_h
