/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2011-06-27

  (C) Copyright 2011-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstCommon/cmnDataFunctionsString.h>

#if 0
template <>
class cmnData<std::string> {
public:
    typedef std::string DataType;

    CMN_DATA_IS_DEFINED_TRUE(std::string)
    CMN_DATA_COPY_USING_ASSIGN(std::string);
    CMN_DATA_SERIALIZE_DESCRIPTION(std::string, str);

    static std::string HumanReadable(const std::string & data) {
        return data;
    }

    static size_t SerializeBinaryByteSize(const DataType & data) {
        return (cmnData<size_t>::SerializeBinaryByteSize(data.size()) + data.size() * sizeof(std::string::value_type));
    }

    static size_t SerializeBinary(const DataType & data,
                                  char * buffer, size_t bufferSize) {
        // check that buffer is large enough to serialize
        const size_t dataSize = cmnData<std::string>::SerializeBinaryByteSize(data);
        if (bufferSize < dataSize) {
            return 0;
        }
        // serialize length as a size_T object
        const size_t length = data.size();
        buffer += cmnData<size_t>::SerializeBinary(length, buffer, cmnData<size_t>::SerializeBinaryByteSize(0));
        // serialize string itself
        const size_t numberOfBytes = length * sizeof(std::string::value_type);
        memcpy(buffer, data.data(), length * numberOfBytes);
        bufferSize -= numberOfBytes;
        return dataSize;
    }

    static size_t DeSerializeBinary(DataType & data, const char * buffer, size_t bufferSize,
                                    const cmnDataFormat & localFormat,
                                    const cmnDataFormat & remoteFormat)
    {
        size_t stringSize;
        size_t byteRead = cmnData<size_t>::DeSerializeBinary(stringSize, buffer, bufferSize,
                                                             localFormat, remoteFormat);
        // make sure we can at least get a string size
        if (byteRead == 0) {
            return 0;
        }
        // if the string itself was empty, return
        if (stringSize == 0) {
            data.clear();
            return byteRead;
        }
        // make sure the buffer has the whole string
        const size_t sizeOfData = stringSize * sizeof(std::string::value_type);
        if (bufferSize < sizeOfData) {
            return 0;
        }
        // get the string characters
        data.resize(stringSize);
        data.assign(buffer + byteRead, stringSize);
        bufferSize -= sizeOfData;
        return byteRead + sizeOfData;
    }

    static void SerializeBinary(const DataType & data,
                                std::ostream & outputStream)
        throw (std::runtime_error)
    {
        cmnData<size_t>::SerializeBinary(data.size(), outputStream);
        outputStream.write(data.data(), data.size());
        if (outputStream.fail()) {
            cmnThrow("cmnDataSerializeBinary(std::string): error occured with std::ostream::write");
        }
    }

    static void DeSerializeBinary(DataType & data,
                                  std::istream & inputStream,
                                  const cmnDataFormat & localFormat,
                                  const cmnDataFormat & remoteFormat)
        throw (std::runtime_error)
    {
        size_t size;
        // retrieve size of string
        cmnData<size_t>::DeSerializeBinary(size, inputStream, localFormat, remoteFormat);
        data.resize(size);
        inputStream.read(const_cast<char *>(data.data()), size);
        if (inputStream.fail()) {
            cmnThrow("cmnDataDeSerializeBinary(std::string): error occured with std::istream::read");
        }
    }

    static void SerializeText(const DataType & data,
                              std::ostream & outputStream,
                              const char delimiter)
        throw (std::runtime_error)
    {
        const std::string::const_iterator end = data.end();
        std::string::const_iterator iter = data.begin();
        for (; iter != end; ++iter) {
            if ((*iter == delimiter)
                || (*iter == '\\')) {
                outputStream << '\\';
            }
            outputStream << *iter;
        }
    }

    static void DeSerializeText(DataType & data,
                                std::istream & inputStream,
                                const char delimiter)
        throw (std::runtime_error)
    {
        // reset string content
        data = "";
        bool lastCharWasEscape = false;
        char newChar;

        // seek around to figure how many characters are left in input stream
        std::streampos currentPosition = inputStream.tellg();
        inputStream.seekg(0, inputStream.end);
        std::streamoff charactersLeft = inputStream.tellg() - currentPosition;
        inputStream.seekg(currentPosition);

        // keep reading as long as we don't run into comma
        while (charactersLeft > 0                     // there is still something to read
               && ((inputStream.peek() != delimiter)  // we are not finding the delimiter
                   || lastCharWasEscape)              // unless the delimiter has been "escaped"
               ) {
            inputStream.get(newChar);
            --charactersLeft;
            if ((newChar == '\\')
                && !lastCharWasEscape) {
                lastCharWasEscape = true;
            } else {
                lastCharWasEscape = false;
                data.append(1, newChar);
            }
        }
    }

    static bool ScalarNumberIsFixed(const std::string & CMN_UNUSED(data)) {
        return true;
    }

    static size_t ScalarNumber(const std::string & CMN_UNUSED(data)) {
        return 0;
    }

    static std::string ScalarDescription(const std::string & CMN_UNUSED(data), const size_t CMN_UNUSED(index),
                                         const std::string & CMN_UNUSED(userDescription)) throw (std::out_of_range) {
        cmnThrow(std::out_of_range("cmnDataScalarDescription: std::string has no scalar"));
        return "n/a";
    }

    static double Scalar(const std::string & CMN_UNUSED(data), const size_t CMN_UNUSED(index)) throw (std::out_of_range) {
        cmnThrow(std::out_of_range("cmnDataScalar: std::string has no scalar"));
        return 1.234;
    }
};
#endif
