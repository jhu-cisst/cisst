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
            cmnThrow("cmnDataSerializeBinary(type): error occured with std::ostream::write"); \
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
            cmnThrow("cmnDataDeSerializeBinary(type): error occured with std::istream::read"); \
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
            cmnThrow("cmnDataDeSerializeBinary(##_type##): error occured with std::istream::read"); \
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


/*! Macro to overload the function cmnDataDescription using the type
  name itself.  For example, for a double it will return the string
  "{double}". */
#define CMN_DATA_DESCRIPTION_USING_TYPE_NAME(_type)                     \
    inline std::string cmnDataDescription(const _type & CMN_UNUSED(data)) { \
        return "{"#_type"}";                                            \
    }


#define CMN_DATA_INSTANTIATE_ALL_NO_BYTE_SWAP(type)                     \
    CMN_DATA_COPY_USING_ASSIGN(type);                                   \
    CMN_DATA_SERIALIZE_BINARY_STREAM_USING_CAST_TO_CHAR(type)           \
    CMN_DATA_DE_SERIALIZE_BINARY_STREAM_USING_CAST_TO_CHAR_NO_BYTE_SWAP(type) \
    CMN_DATA_DESCRIPTION_USING_TYPE_NAME(type)

#define CMN_DATA_INSTANTIATE_ALL_BYTE_SWAP(type)                        \
    CMN_DATA_COPY_USING_ASSIGN(type);                                   \
    CMN_DATA_SERIALIZE_BINARY_STREAM_USING_CAST_TO_CHAR(type)           \
    CMN_DATA_DE_SERIALIZE_BINARY_STREAM_USING_CAST_TO_CHAR_AND_BYTE_SWAP(type) \
    CMN_DATA_DESCRIPTION_USING_TYPE_NAME(type)


CMN_DATA_INSTANTIATE_ALL_NO_BYTE_SWAP(bool);
CMN_DATA_INSTANTIATE_ALL_NO_BYTE_SWAP(char);
CMN_DATA_INSTANTIATE_ALL_NO_BYTE_SWAP(unsigned char);
CMN_DATA_INSTANTIATE_ALL_BYTE_SWAP(short);
CMN_DATA_INSTANTIATE_ALL_BYTE_SWAP(unsigned short);
CMN_DATA_INSTANTIATE_ALL_BYTE_SWAP(int);
CMN_DATA_INSTANTIATE_ALL_BYTE_SWAP(unsigned int);
CMN_DATA_INSTANTIATE_ALL_BYTE_SWAP(long long int);
CMN_DATA_INSTANTIATE_ALL_BYTE_SWAP(unsigned long long int);
CMN_DATA_INSTANTIATE_ALL_BYTE_SWAP(float);
CMN_DATA_INSTANTIATE_ALL_BYTE_SWAP(double);


// size_t specialization
CMN_DATA_COPY_USING_ASSIGN(size_t);
CMN_DATA_SERIALIZE_BINARY_STREAM_USING_CAST_TO_CHAR(size_t);
inline void cmnDataDeSerializeBinary(std::istream & inputStream,
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
        data = tmp;
        return;
    }
}


// std::string specialization
CMN_DATA_COPY_USING_ASSIGN(std::string);

inline void cmnDataSerializeBinary(std::ostream & outputStream,
                                   const std::string & data) throw (std::runtime_error)
{
    cmnDataSerializeBinary(outputStream, data.size());
    outputStream.write(data.data(), data.size());
    if (outputStream.fail()) {
        cmnThrow("cmnDataSerializeBinary(std::string): error occured with std::ostream::write");
    }
}

inline void cmnDataDeSerializeBinary(std::istream & inputStream,
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
