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
#ifndef _cmnDataFunctionsMacros_h
#define _cmnDataFunctionsMacros_h


/*! Macro to overload the function cmnDataCopy using the assignement
  operator to copy from source to data.  This is a appropriate for
  data types without any dynamic memory allocations.  For types using
  dynamic memory allocations or large blocks of memory, users should
  overload the cmnDataCopy function using a more efficient approach
  (memcpy, ...). */
#define CMN_DATA_COPY_USING_ASSIGN(_type)                               \
    static void Copy(_type & data,                                      \
                     const _type & source) {                            \
        data = source;                                                  \
    }


/*! Macro to overload the function cmnDataSerializeDescription. */
#define CMN_DATA_SERIALIZE_DESCRIPTION(_type, _description)             \
    static std::string SerializeDescription(const _type & CMN_UNUSED(data), \
                                            const char CMN_UNUSED(delimiter) = ',', \
                                            const std::string & userDescription = "") \
    {                                                                   \
        return (userDescription == "" ? "{"#_description"}" : (userDescription + ":{"#_description"}")); \
    }


#define CMN_DATA_SERIALIZE_BINARY_BYTE_SIZE_USING_SIZEOF(_type)         \
    static size_t SerializeBinaryByteSize(const _type & CMN_UNUSED(data)) \
    {                                                                   \
        return sizeof(_type);                                           \
    }

/*! Macro to overload the function cmnDataSerializeBinary using a cast
  to char pointer and assuming that sizeof reports the real size of
  the object to be serialized.  Please note that this method will
  not work with data object relying on dynamic memory allocation
  (pointers) and might also fail if you are using a struct/class
  your compiler can pad. */
#define CMN_DATA_SERIALIZE_BINARY_BUFFER_USING_CAST_TO_CHAR(_type)      \
    static size_t SerializeBinary(const _type & data,                   \
                                  char * buffer, size_t bufferSize)     \
    {                                                                   \
        const size_t sizeOfData = cmnData<_type>::SerializeBinaryByteSize(data); \
        if (bufferSize < sizeOfData) {                                  \
            return 0;                                                   \
        }                                                               \
        *(reinterpret_cast<_type *>(buffer)) = data;                    \
        bufferSize -= sizeOfData;                                       \
        return sizeOfData;                                              \
    }

#define CMN_DATA_DE_SERIALIZE_BINARY_BUFFER_USING_CAST_TO_CHAR_NO_BYTE_SWAP(_type) \
    static size_t DeSerializeBinary(_type & data, const char * buffer, size_t bufferSize, \
                                    const cmnDataFormat & CMN_UNUSED(localFormat), \
                                    const cmnDataFormat & CMN_UNUSED(remoteFormat)) \
    {                                                                   \
        const size_t sizeOfData = cmnData<_type>::SerializeBinaryByteSize(data); \
        if (bufferSize < sizeOfData) {                                  \
            return 0;                                                   \
        }                                                               \
        data = *(reinterpret_cast<const _type *>(buffer));              \
        bufferSize -= sizeOfData;                                       \
        return sizeOfData;                                              \
    }

#define CMN_DATA_DE_SERIALIZE_BINARY_BUFFER_USING_CAST_TO_CHAR_AND_BYTE_SWAP(_type) \
    static size_t DeSerializeBinary(_type & data, const char * buffer, size_t bufferSize, \
                                    const cmnDataFormat & localFormat,  \
                                    const cmnDataFormat & remoteFormat) \
    {                                                                   \
        const size_t sizeOfData = cmnData<_type>::SerializeBinaryByteSize(data); \
        if (bufferSize < sizeOfData) {                                  \
            return 0;                                                   \
        }                                                               \
        data = *(reinterpret_cast<const _type *>(buffer));              \
        if (remoteFormat.GetEndianness() != localFormat.GetEndianness()) { \
            cmnDataByteSwap(data);                                      \
        }                                                               \
        bufferSize -= sizeOfData;                                       \
        return sizeOfData;                                              \
    }

/*! Macro to overload the function cmnDataSerializeBinary using a cast
  to char pointer and assuming that sizeof reports the real size of
  the object to be serialized.  Please note that this method will
  not work with data object relying on dynamic memory allocation
  (pointers) and might also fail if you are using a struct/class
  your compiler can pad. */
#define CMN_DATA_SERIALIZE_BINARY_STREAM_USING_CAST_TO_CHAR(_type)      \
    static void SerializeBinary(const _type & data,                     \
                                std::ostream & outputStream)            \
        CISST_THROW(std::runtime_error)                                 \
    {                                                                   \
        outputStream.write(reinterpret_cast<const char *>(&data),       \
                           sizeof(_type));                              \
        if (outputStream.fail()) {                                      \
            cmnThrow("cmnData::SerializeBinary(" #_type "): error occured with std::ostream::write"); \
        }                                                               \
    }


#define CMN_DATA_DE_SERIALIZE_BINARY_STREAM_USING_CAST_TO_CHAR_NO_BYTE_SWAP(_type)   \
    static void DeSerializeBinary(_type & data,                         \
                                  std::istream & inputStream,           \
                                  const cmnDataFormat & CMN_UNUSED(localFormat), \
                                  const cmnDataFormat & CMN_UNUSED(remoteFormat)) \
        CISST_THROW(std::runtime_error)                                 \
    {                                                                   \
        inputStream.read(reinterpret_cast<char *>(&data),               \
                         sizeof(_type));                                \
        if (inputStream.fail()) {                                       \
            cmnThrow("cmnData::DeSerializeBinary(" #_type "): error occured with std::istream::read"); \
        }                                                               \
    }


#define CMN_DATA_DE_SERIALIZE_BINARY_STREAM_USING_CAST_TO_CHAR_AND_BYTE_SWAP(_type) \
    static void DeSerializeBinary(_type & data,                         \
                                  std::istream & inputStream,           \
                                  const cmnDataFormat & localFormat,    \
                                  const cmnDataFormat & remoteFormat)   \
        CISST_THROW(std::runtime_error)                                 \
    {                                                                   \
        inputStream.read(reinterpret_cast<char *>(&data),               \
                         sizeof(_type));                                \
        if (inputStream.fail()) {                                       \
            cmnThrow("cmnData::DeSerializeBinary(" #_type "): error occured with std::istream::read"); \
        }                                                               \
        if (remoteFormat.GetEndianness() != localFormat.GetEndianness()) { \
            cmnDataByteSwap(data);                                      \
        }                                                               \
    }


/*! Macro to overload the function cmnDataSerializeText using the C++
  stream out operator. */
#define CMN_DATA_SERIALIZE_TEXT_USING_STREAM_OUT(_type)                 \
    static void SerializeText(const _type & data,                       \
                              std::ostream & outputStream,              \
                              const char CMN_UNUSED(delimiter) = ',')   \
        CISST_THROW(std::runtime_error)                                 \
    {                                                                   \
        outputStream << data;                                           \
        if (outputStream.fail()) {                                      \
            cmnThrow("cmnData::SerializeText(" #_type "): error occured with std::ostream::write"); \
        }                                                               \
    }


/*! Macro to overload the function cmnDataDeSerializeText using the C++
  stream in operator. */
#define CMN_DATA_DE_SERIALIZE_TEXT_USING_STREAM_IN(_type)               \
    static void DeSerializeText(_type & data,                           \
                                std::istream & inputStream,             \
                                const char CMN_UNUSED(delimiter) = ',') \
        CISST_THROW(std::runtime_error)                                 \
    {                                                                   \
        inputStream >> data;                                            \
        if (inputStream.fail()) {                                       \
            cmnThrow("cmnData::DeSerializeText(" #_type "): error occured with std::istream::read"); \
        }                                                               \
    }


/*! Macro to overload the function cmnDataHumanReadble using the C++
  stream out operator. */
#define CMN_DATA_HUMAN_READABLE_USING_STREAM_OUT(_type)                 \
    static std::string HumanReadable(const _type & data)                \
    {                                                                   \
        std::stringstream stringStream;                                 \
        stringStream << data;                                           \
        return stringStream.str();                                      \
    }


/*! Macro to overload the function cmnDataScalarDescription using the type
  name itself.  For example, for a double it will return the string
  "double". */
#define CMN_DATA_SCALAR_DESCRIPTION(_type, _description)                \
    static std::string ScalarDescription(const _type & CMN_UNUSED(data), \
                                         const size_t CMN_UNUSED(index), \
                                         const std::string & userDescription = "") \
        CISST_THROW(std::out_of_range) {                                \
        return (userDescription == "" ? "{"#_description"}" : (userDescription + ":{"#_description"}")); \
    }

/*! Macro to overload the function cmnDataScalar using a static_cast. */
#define CMN_DATA_SCALAR_USING_STATIC_CAST(_type)                        \
    static double Scalar(const _type & data, const size_t CMN_UNUSED(index)) \
        CISST_THROW(std::out_of_range) {                                \
        return static_cast<double>(data);                               \
    }

/*! Macro to overload the function cmnDataScalarNumber, returns 1. */
#define CMN_DATA_SCALAR_NUMBER_IS_ONE(_type)                            \
    static size_t ScalarNumber(const _type & CMN_UNUSED(data)) {        \
        return 1;                                                       \
    }

#define CMN_DATA_SCALAR_NUMBER_IS_FIXED_TRUE(_type)                     \
    static bool ScalarNumberIsFixed(const _type & CMN_UNUSED(data)) {   \
        return true;                                                    \
    }

#define CMN_DATA_IS_SPECIALIZED_TRUE(_type)     \
    enum {IS_SPECIALIZED = 1};

#define CMN_DATA_SPECIALIZE_ALL_NO_BYTE_SWAP(type, description)         \
template <>                                                             \
class cmnData<type> {                                                   \
public:                                                                 \
    CMN_DATA_IS_SPECIALIZED_TRUE(type)                                  \
    CMN_DATA_COPY_USING_ASSIGN(type)                                    \
    CMN_DATA_SERIALIZE_DESCRIPTION(type, description)                   \
    CMN_DATA_SERIALIZE_BINARY_BYTE_SIZE_USING_SIZEOF(type)              \
    CMN_DATA_SERIALIZE_BINARY_BUFFER_USING_CAST_TO_CHAR(type)           \
    CMN_DATA_DE_SERIALIZE_BINARY_BUFFER_USING_CAST_TO_CHAR_NO_BYTE_SWAP(type) \
    CMN_DATA_SERIALIZE_BINARY_STREAM_USING_CAST_TO_CHAR(type)           \
    CMN_DATA_DE_SERIALIZE_BINARY_STREAM_USING_CAST_TO_CHAR_NO_BYTE_SWAP(type) \
    CMN_DATA_SERIALIZE_TEXT_USING_STREAM_OUT(type)                      \
    CMN_DATA_DE_SERIALIZE_TEXT_USING_STREAM_IN(type)                    \
    CMN_DATA_HUMAN_READABLE_USING_STREAM_OUT(type)                      \
    CMN_DATA_SCALAR_DESCRIPTION(type, description)                      \
    CMN_DATA_SCALAR_USING_STATIC_CAST(type)                             \
    CMN_DATA_SCALAR_NUMBER_IS_ONE(type)                                 \
    CMN_DATA_SCALAR_NUMBER_IS_FIXED_TRUE(type)                          \
    };

#define CMN_DATA_SPECIALIZE_ALL_BYTE_SWAP(type, description)            \
template <>                                                             \
class cmnData<type> {                                                   \
public:                                                                 \
    CMN_DATA_IS_SPECIALIZED_TRUE(type)                                  \
    CMN_DATA_COPY_USING_ASSIGN(type)                                    \
    CMN_DATA_SERIALIZE_DESCRIPTION(type, description)                   \
    CMN_DATA_SERIALIZE_BINARY_BYTE_SIZE_USING_SIZEOF(type)              \
    CMN_DATA_SERIALIZE_BINARY_BUFFER_USING_CAST_TO_CHAR(type)           \
    CMN_DATA_DE_SERIALIZE_BINARY_BUFFER_USING_CAST_TO_CHAR_AND_BYTE_SWAP(type) \
    CMN_DATA_SERIALIZE_BINARY_STREAM_USING_CAST_TO_CHAR(type)           \
    CMN_DATA_DE_SERIALIZE_BINARY_STREAM_USING_CAST_TO_CHAR_AND_BYTE_SWAP(type) \
    CMN_DATA_SERIALIZE_TEXT_USING_STREAM_OUT(type)                      \
    CMN_DATA_DE_SERIALIZE_TEXT_USING_STREAM_IN(type)                    \
    CMN_DATA_HUMAN_READABLE_USING_STREAM_OUT(type)                      \
    CMN_DATA_SCALAR_DESCRIPTION(type, description)                      \
    CMN_DATA_SCALAR_USING_STATIC_CAST(type)                             \
    CMN_DATA_SCALAR_NUMBER_IS_ONE(type)                                 \
    CMN_DATA_SCALAR_NUMBER_IS_FIXED_TRUE(type)                          \
    };

#endif // _cmnDataFunctionsMacros_h
