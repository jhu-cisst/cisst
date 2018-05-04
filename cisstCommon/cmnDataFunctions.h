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
#ifndef _cmnDataFunctions_h
#define _cmnDataFunctions_h

#include <string.h> // for memcpy
#include <iostream>
#include <sstream>
#include <cisstCommon/cmnThrow.h>
#include <cisstCommon/cmnDataFormat.h>
#include <cisstCommon/cmnDataFunctionsMacros.h>

// always include last
#include <cisstCommon/cmnExport.h>

/*!  cmnDataFunction.  Data functions used to handle objects across
  the cisst libraries.  All data function names use the prefix cmnData
  and the first parameter is the object currently manipulated.

  The main principle is to rely on the compiler to find the proper
  class overload for each object type.
  <code>
  void cmnData<int>::HumanReadable(const int & data);
  void cmnData<std::string>::HumanReadable(const std::string & data);
  ...
  </code>
  This approach allows to manipulate external data types by
  overloading the required cmnData class.

  Error handling is based on standard exceptions using std::runtime_error.
*/
template <typename _elementType>
class cmnData
{
public:
    enum {IS_SPECIALIZED = 0};

    typedef _elementType DataType;

    /*! Returns a human readable string describing a specific instance
      of an object. */
    static
    std::string
    HumanReadable(const DataType & data);

    /*! Copy the source to the data object.  This allows to define
      mechanisms that might be more efficient than copy constructors
      and avoid the ambiguities of the assignment operator.  For
      example, to copy a dynamic vector of simple object, it can be
      more efficient to use a memcpy than to iterate over each element
      and assign one by one.
    */
    static
    void
    Copy(DataType & data, const DataType & source);

    /*! Provides a human readble string describing each element of the
      data object.  This description is for all instances of the same
      "size".  For example, if the data object is a dynamic vector of
      intergers of size 4, the delimiter is a comma and the user
      description is "V", the returned string would look like
      "V.size,v[0],v[1],v[2],v[3]".
    */
    static
    std::string
    SerializeDescription(const DataType & data, const char delimiter,
                         const std::string & userDescription = "");

    /*! Serializes each element of the data object in text format,
      separated by the user provided delimiter.  Using the previous
      example, this method would stream the size followed by the 4
      value: "4,11,22,33,44".  I/O stream errors can lead to
      exceptions.
    */
    static
    void
    SerializeText(const DataType & data, std::ostream & outputStream,
                  const char delimiter = ',') CISST_THROW(std::runtime_error);

    /*! Set the value of the data object by deserializing each element
      from the input stream.  The function will look for the user
      provided delimiter between the elements to deserialize.  Using
      the previous example, the overloaded function for a dynamic
      vector will look for the size (4), attempt to resize the data
      vector to fit 4 elements, skip one delimiter (comma) and then
      attempt to deserialize 4 comma separated values from the input
      stream.
    */
    static
    void
    DeSerializeText(DataType & data, std::istream & inputStream,
                    const char delimiter = ',') CISST_THROW(std::runtime_error);

    /*! Serializes each element of the data object in binary format.
     */
    static
    void
    SerializeBinary(const DataType & data, std::ostream & outputStream)
        CISST_THROW(std::runtime_error);

    /*!
    */
    static
    void
    DeSerializeBinary(DataType & data, std::istream & inputStream,
                      const cmnDataFormat & localFormat,
                      const cmnDataFormat & remoteFormat) CISST_THROW(std::runtime_error);

    /*! Returns the numbers of bytes required to stored the content of
      the data object.  This function can be used to allocate a char
      buffer used to perform the serialization.
    */
    static
    size_t
    SerializeBinaryByteSize(const DataType & data);

    /*! Attempt to serialize the data object using the user provided
      buffer.  The buffer size must be provided by the caller.  The
      function first checks that the buffer is large enough and
      returns 0 otherwise.  The function then serializes (binary
      format) the data object, modify the parameter bufferSize to
      reflect how much is left and return the number of bytes used.
    */
    static
    size_t
    SerializeBinary(const DataType & data, char * buffer, size_t bufferSize);

    static
    size_t
    DeSerializeBinary(DataType & data, const char * buffer, size_t bufferSize,
                      const cmnDataFormat & CMN_UNUSED(localFormat),
                      const cmnDataFormat & CMN_UNUSED(remoteFormat));

    static
    std::string
    ScalarDescription(const DataType & data, const size_t index, const std::string & userDescription = "")
        CISST_THROW(std::out_of_range);

    static
    double
    Scalar(const DataType & data, const size_t index)
        CISST_THROW(std::out_of_range);

    static
    size_t ScalarNumber(const DataType & data);

    static
    bool ScalarNumberIsFixed(const DataType & data);
};

/* Native types to overload:
   bool
   char
   unsigned char
   short
   unsigned short
   int
   unsigned int
   long long int
   unsigned long long int
   float
   double
*/
CMN_DATA_SPECIALIZE_ALL_NO_BYTE_SWAP(bool, b);
CMN_DATA_SPECIALIZE_ALL_NO_BYTE_SWAP(char, c);
CMN_DATA_SPECIALIZE_ALL_NO_BYTE_SWAP(unsigned char, uc);

CMN_DATA_SPECIALIZE_ALL_BYTE_SWAP(short, s);
CMN_DATA_SPECIALIZE_ALL_BYTE_SWAP(unsigned short, us);
CMN_DATA_SPECIALIZE_ALL_BYTE_SWAP(int, i);
CMN_DATA_SPECIALIZE_ALL_BYTE_SWAP(unsigned int, ui);
CMN_DATA_SPECIALIZE_ALL_BYTE_SWAP(long int, li);
CMN_DATA_SPECIALIZE_ALL_BYTE_SWAP(unsigned long int, uli);
CMN_DATA_SPECIALIZE_ALL_BYTE_SWAP(long long int, lli);
CMN_DATA_SPECIALIZE_ALL_BYTE_SWAP(unsigned long long int, ulli);
CMN_DATA_SPECIALIZE_ALL_BYTE_SWAP(float, f);
CMN_DATA_SPECIALIZE_ALL_BYTE_SWAP(double, d);

/*! Special case for size_t, binary de-serialization needs to handle
  differences between 32 and 64 bits. */
//@{
size_t CISST_EXPORT cmnDataDeSerializeBinary_size_t(size_t & data, const char * buffer, size_t bufferSize,
                                                    const cmnDataFormat & localFormat,
                                                    const cmnDataFormat & remoteFormat);

void CISST_EXPORT cmnDataDeSerializeBinary_size_t(size_t & data,
                                                  std::istream & inputStream,
                                                  const cmnDataFormat & localFormat,
                                                  const cmnDataFormat & remoteFormat) CISST_THROW(std::runtime_error);
//@}

/*! Utility used to extract delimiter for functions cmnDataDeSerializeText */
void CISST_EXPORT cmnDataDeSerializeTextDelimiter(std::istream & inputStream, const char delimiter, const char * className)
    CISST_THROW(std::runtime_error);

#endif // _cmnDataFunctions_h
