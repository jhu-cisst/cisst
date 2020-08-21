/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2012-07-09

  (C) Copyright 2012-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once

#ifndef _cmnDataFunctionsVectorHelpers_h
#define _cmnDataFunctionsVectorHelpers_h

template <class _vectorType>
void cmnDataVectorCopy(_vectorType & data,
                       const _vectorType & source)
{
    // resize destination if needed
    data.resize(source.size());
    typedef _vectorType VectorType;
    typedef typename VectorType::iterator iterator;
    typedef typename VectorType::const_iterator const_iterator;
    const const_iterator endSource = source.end();
    const_iterator iterSource = source.begin();
    iterator iterData = data.begin();
    for (;
         iterSource != endSource;
         ++iterSource, ++iterData) {
        cmnData<typename _vectorType::value_type>::Copy(*iterData, *iterSource);
    }
}

template <class _vectorType>
std::string cmnDataVectorHumanReadable(const _vectorType & data)
{
    std::stringstream stringStream;
    stringStream << "[";
    typedef _vectorType VectorType;
    typedef typename VectorType::const_iterator const_iterator;
    const const_iterator end = data.end();
    const const_iterator begin = data.begin();
    for (const_iterator iter = begin; iter != end; ++iter) {
        if (iter != begin) {
            stringStream << ", ";
        }
        stringStream << cmnData<typename _vectorType::value_type>::HumanReadable(*iter);
    }
    stringStream << "]";
    return stringStream.str();
}

template <class _vectorType>
void cmnDataVectorSerializeText(const _vectorType & data,
                                std::ostream & outputStream,
                                const char delimiter)
    CISST_THROW(std::runtime_error)
{
    typedef typename _vectorType::const_iterator const_iterator;
    const const_iterator begin = data.begin();
    const const_iterator end = data.end();
    for (const_iterator iter = begin; iter != end; ++iter) {
        if (iter != begin) {
            outputStream << delimiter;
        }
        cmnData<typename _vectorType::value_type>::SerializeText(*iter, outputStream, delimiter);
    }
}

template <class _vectorType>
void cmnDataVectorDeSerializeText(_vectorType & data,
                                  std::istream & inputStream,
                                  const char delimiter)
{
    // get data
    typedef typename _vectorType::iterator iterator;
    const iterator begin = data.begin();
    const iterator end = data.end();
    for (iterator iter = begin; iter != end; ++iter) {
        if (iter != begin) {
            cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "std::vector");
        }
        cmnData<typename _vectorType::value_type>::DeSerializeText(*iter, inputStream, delimiter);
    }
}

template <class _vectorType>
void cmnDataVectorDeSerializeTextResize(_vectorType & data,
                                        std::istream & inputStream,
                                        const char delimiter)
    CISST_THROW(std::runtime_error)
{
    // deserialize size
    size_t size;
    cmnData<size_t>::DeSerializeText(size, inputStream, delimiter);
    data.resize(size);
    if (size > 0) {
        cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "vctFixedSizeVectorBase");
        cmnDataVectorDeSerializeText(data, inputStream, delimiter);
    }
}

template <class _vectorType>
void cmnDataVectorDeSerializeTextCheckSize(_vectorType & data,
                                           std::istream & inputStream,
                                           const char delimiter)
    CISST_THROW(std::runtime_error)
{
    // deserialize size
    size_t size;
    cmnData<size_t>::DeSerializeText(size, inputStream, delimiter);
    if (data.size() != size) {
        cmnThrow("cmnDataDeSerializeText: vector size doesn't match");
        return;
    }
    if (size > 0) {
        cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "vctFixedSizeVectorBase");
        cmnDataVectorDeSerializeText(data, inputStream, delimiter);
    }
}

template <class _vectorType>
std::string cmnDataVectorSerializeDescription(const _vectorType & data,
                                              const char delimiter,
                                              const std::string & userDescription,
                                              const bool serializeSize)
{
    std::string prefix = (userDescription == "") ? "v[" : (userDescription + "[");
    std::stringstream indexSuffix;
    std::stringstream description;
    const size_t size = data.size();
    if (serializeSize) {
        description << cmnData<size_t>::SerializeDescription(size, delimiter, (userDescription == "") ? "v.size" : (userDescription + ".size"));
    }
    size_t index;
    for (index = 0; index < size; ++index) {
        if (index != 0 || serializeSize) {
            description << delimiter;
        }
        indexSuffix.clear();
        indexSuffix.str(std::string());
        indexSuffix << index << "]";
        description << cmnData<typename _vectorType::value_type>::SerializeDescription(data[index], delimiter, prefix + indexSuffix.str());
    }
    return description.str();
}

template <class _vectorType>
void cmnDataVectorSerializeBinary(const _vectorType & data,
                                  std::ostream & outputStream)
    CISST_THROW(std::runtime_error)
{
    typedef typename _vectorType::const_iterator const_iterator;
    const const_iterator end = data.end();
    const_iterator iter = data.begin();
    for (; iter != end; ++iter) {
        cmnData<typename _vectorType::value_type>::SerializeBinary(*iter, outputStream);
    }
}

template <class _vectorType>
void cmnDataVectorDeSerializeBinary(_vectorType & data,
                                    std::istream & inputStream,
                                    const cmnDataFormat & localFormat,
                                    const cmnDataFormat & remoteFormat)
    CISST_THROW(std::runtime_error)
{
    // deserialize data
    typedef typename _vectorType::iterator iterator;
    const iterator end = data.end();
    iterator iter = data.begin();
    for (; iter != end; ++iter) {
        cmnData<typename _vectorType::value_type>::DeSerializeBinary(*iter, inputStream, localFormat, remoteFormat);
    }
}

template <class _vectorType>
void cmnDataVectorDeSerializeBinaryResize(_vectorType & data,
                                          std::istream & inputStream,
                                          const cmnDataFormat & localFormat,
                                          const cmnDataFormat & remoteFormat)
    CISST_THROW(std::runtime_error)
{
    // deserialize size and resize
    size_t size;
    cmnDataDeSerializeBinary_size_t(size, inputStream, localFormat, remoteFormat);
    data.resize(size);
    cmnDataVectorDeSerializeBinary(data, inputStream, localFormat, remoteFormat);
}

template <class _vectorType>
void cmnDataVectorDeSerializeBinaryCheckSize(_vectorType & data,
                                             std::istream & inputStream,
                                             const cmnDataFormat & localFormat,
                                             const cmnDataFormat & remoteFormat)
    CISST_THROW(std::runtime_error)
{
    // deserialize size and check
    size_t size;
    cmnDataDeSerializeBinary_size_t(size, inputStream, localFormat, remoteFormat);
    if (data.size() != size) {
        cmnThrow("cmnDataDeSerializeBinary: vector size doesn't match");
        return;
    }
    cmnDataVectorDeSerializeBinary(data, inputStream, localFormat, remoteFormat);
}

template <class _vectorType>
size_t cmnDataVectorScalarNumber(const _vectorType & data)
{
    if (data.size() == 0) {
        return 0;
    }
    typedef typename _vectorType::value_type value_type;
    if (cmnData<value_type>::ScalarNumberIsFixed(data[0])) {
        return data.size() * cmnData<value_type>::ScalarNumber(data[0]);
    }
    size_t result = 0;
    typedef typename _vectorType::const_iterator const_iterator;
    const const_iterator end = data.end();
    const_iterator iter = data.begin();
    for (; iter != end; ++iter) {
        result += cmnData<value_type>::ScalarNumber(*iter);
    }
    return result;
}

template <class _vectorType, class _elementType>
bool cmnDataVectorScalarFindInVectorIndex(_vectorType data, const size_t size, const size_t scalarNumber,
                                          const size_t index,
                                          size_t & elementIndex, size_t & inElementIndex)
{
    if (cmnData<_elementType>::ScalarNumberIsFixed(data[0])) {
        const size_t scalarNumberPerElement = cmnData<_elementType>::ScalarNumber(data[0]);
        if (scalarNumberPerElement == 0) {
            return false;
        }
        if (index < scalarNumber) {
            elementIndex = index / scalarNumberPerElement;
            inElementIndex = index % scalarNumberPerElement;
            return true;
        }
        return false;
    }
    bool indexFound = false;
    size_t scalarCounter = 0;
    size_t lastScalarInElement = 0;
    size_t firstScalarInElement = 0;
    size_t numberOfScalarsInElement = 0;
    elementIndex = 0;
    do {
        numberOfScalarsInElement = cmnData<_elementType>::ScalarNumber(data[elementIndex]);
        firstScalarInElement = scalarCounter;
        lastScalarInElement = scalarCounter + numberOfScalarsInElement - 1;
        scalarCounter = lastScalarInElement + 1;
        elementIndex++;
        indexFound = ((index >= firstScalarInElement) && (index <= lastScalarInElement));
    } while ((!indexFound)
             && (elementIndex < size));
    if (indexFound) {
        elementIndex--;
        inElementIndex = index - firstScalarInElement;
        return true;
    }
    return false;
}

template <class _vectorType>
std::string cmnDataVectorScalarDescription(const _vectorType & data,
                                           const size_t index,
                                           const std::string & userDescription)
    CISST_THROW(std::out_of_range)
{
    size_t elementIndex, inElementIndex;
    std::stringstream suffix;
    const size_t scalarNumber = cmnData<_vectorType>::ScalarNumber(data);
    if (cmnDataVectorScalarFindInVectorIndex<const _vectorType &, typename _vectorType::value_type>(data, data.size(), scalarNumber,
                                                                                                    index, elementIndex, inElementIndex)) {
        suffix << "[" << index << "]";
        return cmnData<typename _vectorType::value_type>::ScalarDescription(data[elementIndex], inElementIndex, userDescription + suffix.str());
    }
    cmnThrow(std::out_of_range("cmnDataScalarDescription: vector index out of range"));
    return "";
}

template <class _vectorType>
double cmnDataVectorScalar(const _vectorType & data, const size_t index)
    CISST_THROW(std::out_of_range)
{
    size_t elementIndex, inElementIndex;
    const size_t scalarNumber = cmnData<_vectorType>::ScalarNumber(data);
    if (cmnDataVectorScalarFindInVectorIndex<const _vectorType &, typename _vectorType::value_type>(data, data.size(), scalarNumber,
                                                                                                    index, elementIndex, inElementIndex)) {
        return cmnData<typename _vectorType::value_type>::Scalar(data[elementIndex], inElementIndex);
    } else {
        cmnThrow(std::out_of_range("cmnDataScalar: vector index out of range"));
    }
    return 0.123456789; // unreachable, just to avoid compiler warnings
}

#endif // _cmnDataFunctionsVectorHelpers_h
