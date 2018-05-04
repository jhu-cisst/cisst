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

#include <cisstCommon/cmnDataFunctionsString.h>

size_t cmnDataSerializeBinary(const std::string & data, char * buffer, size_t bufferSize)
{
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

size_t cmnDataDeSerializeBinary(std::string & data, const char * buffer, size_t bufferSize,
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

void cmnDataSerializeBinary(const std::string & data,
                            std::ostream & outputStream)
    CISST_THROW(std::runtime_error)
{
    cmnData<size_t>::SerializeBinary(data.size(), outputStream);
    outputStream.write(data.data(), data.size());
    if (outputStream.fail()) {
        cmnThrow("cmnDataSerializeBinary(std::string): error occured with std::ostream::write");
    }
}

void cmnDataDeSerializeBinary(std::string & data,
                              std::istream & inputStream,
                              const cmnDataFormat & localFormat,
                              const cmnDataFormat & remoteFormat)
    CISST_THROW(std::runtime_error)
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

void cmnDataSerializeText(const std::string & data,
                          std::ostream & outputStream,
                          const char delimiter)
    CISST_THROW(std::runtime_error)
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

void cmnDataDeSerializeText(std::string & data,
                            std::istream & inputStream,
                            const char delimiter)
    CISST_THROW(std::runtime_error)
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
