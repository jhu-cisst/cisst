/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2012-07-09

  (C) Copyright 2012-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once

#ifndef _cmnDataFunctionsMatrixHelpers_h
#define _cmnDataFunctionsMatrixHelpers_h

template <class _matrixType>
void cmnDataMatrixCopy(_matrixType & data,
                       const _matrixType & source)
{
    // resize destination if needed
    data.resize(source.size());
    typedef _matrixType MatrixType;
    typedef typename MatrixType::iterator iterator;
    typedef typename MatrixType::const_iterator const_iterator;
    const const_iterator endSource = source.end();
    const_iterator iterSource = source.begin();
    iterator iterData = data.begin();
    for (;
         iterSource != endSource;
         ++iterSource, ++iterData) {
        cmnData<typename _matrixType::value_type>::Copy(*iterData, *iterSource);
    }
}

template <class _matrixType>
std::string cmnDataMatrixHumanReadable(const _matrixType & data)
{
    std::stringstream stringStream;
    const size_t rows = data.rows();
    const size_t cols = data.cols();
    stringStream << "[";
    for (size_t row = 0; row < rows; ++row) {
        if (row != 0) {
            stringStream << ", ";
        }
        stringStream << "[";
        for (size_t col = 0; col < cols; ++col) {
            if (col != 0) {
                stringStream << ", ";
            }
            stringStream << cmnData<typename _matrixType::value_type>::HumanReadable(data.at(row, col));
        }
        stringStream << "]";
    }
    stringStream << "]";
    return stringStream.str();
}

template <class _matrixType>
void cmnDataMatrixSerializeText(const _matrixType & data,
                                std::ostream & outputStream,
                                const char delimiter)
    CISST_THROW(std::runtime_error)
{
    typedef typename _matrixType::const_iterator const_iterator;
    const const_iterator begin = data.begin();
    const const_iterator end = data.end();
    for (const_iterator iter = begin; iter != end; ++iter) {
        if (iter != begin) {
            outputStream << delimiter;
        }
        cmnData<typename _matrixType::value_type>::SerializeText(*iter, outputStream, delimiter);
    }
}

template <class _matrixType>
void cmnDataMatrixDeSerializeText(_matrixType & data,
                                  std::istream & inputStream,
                                  const char delimiter)
{
    // get data
    typedef typename _matrixType::iterator iterator;
    const iterator begin = data.begin();
    const iterator end = data.end();
    for (iterator iter = begin; iter != end; ++iter) {
        if (iter != begin) {
            cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "std::matrix");
        }
        cmnData<typename _matrixType::value_type>::DeSerializeText(*iter, inputStream, delimiter);
    }
}

template <class _matrixType>
void cmnDataMatrixDeSerializeTextResize(_matrixType & data,
                                        std::istream & inputStream,
                                        const char delimiter)
    CISST_THROW(std::runtime_error)
{
    // deserialize size
    size_t rows, cols;
    cmnData<size_t>::DeSerializeText(rows, inputStream, delimiter);
    cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "vctFixedSizeMatrixBase");
    cmnData<size_t>::DeSerializeText(cols, inputStream, delimiter);
    data.resize(rows, cols);
    if (data.size() > 0) {
        cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "vctFixedSizeMatrixBase");
        cmnDataMatrixDeSerializeText(data, inputStream, delimiter);
    }
}

template <class _matrixType>
void cmnDataMatrixDeSerializeTextCheckSize(_matrixType & data,
                                           std::istream & inputStream,
                                           const char delimiter)
    CISST_THROW(std::runtime_error)
{
    // deserialize size
    size_t rows, cols;
    cmnData<size_t>::DeSerializeText(rows, inputStream, delimiter);
    cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "vctFixedSizeMatrixBase");
    cmnData<size_t>::DeSerializeText(cols, inputStream, delimiter);
    if ((data.rows() != rows) || (data.cols() != cols)) {
        cmnThrow("cmnDataDeSerializeText: matrix size doesn't match");
        return;
    }
    if (data.size() > 0) {
        cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "vctFixedSizeMatrixBase");
        cmnDataMatrixDeSerializeText(data, inputStream, delimiter);
    }
}

template <class _matrixType>
std::string cmnDataMatrixSerializeDescription(const _matrixType & data,
                                              const char delimiter,
                                              const std::string & userDescription,
                                              const bool serializeSize)
{
    std::string prefix = (userDescription == "") ? "m[" : (userDescription + "[");
    std::stringstream indexSuffix;
    std::stringstream description;
    const size_t rows = data.rows();
    const size_t cols = data.cols();
    if (serializeSize) {
        description << cmnData<size_t>::SerializeDescription(rows, delimiter, (userDescription == "") ? "v.rows" : (userDescription + ".rows"))
                    << delimiter
                    << cmnData<size_t>::SerializeDescription(cols, delimiter, (userDescription == "") ? "v.cols" : (userDescription + ".cols"));
    }
    size_t row, col;
    for (row = 0; row < rows; ++row) {
        for (col = 0; col < cols; ++col) {
            if ((row + col) != 0 || serializeSize) {
                description << delimiter;
            }
            indexSuffix.clear();
            indexSuffix.str(std::string());
            indexSuffix << row << "," << col << "]";
            description << cmnData<typename _matrixType::value_type>::SerializeDescription(data.Element(row, col), delimiter, prefix + indexSuffix.str());
        }
    }
    return description.str();
}

template <class _matrixType>
void cmnDataMatrixSerializeBinary(const _matrixType & data,
                                  std::ostream & outputStream)
    CISST_THROW(std::runtime_error)
{
    typedef typename _matrixType::const_iterator const_iterator;
    const const_iterator end = data.end();
    const_iterator iter = data.begin();
    for (; iter != end; ++iter) {
        cmnData<typename _matrixType::value_type>::SerializeBinary(*iter, outputStream);
    }
}

template <class _matrixType>
void cmnDataMatrixDeSerializeBinary(_matrixType & data,
                                    std::istream & inputStream,
                                    const cmnDataFormat & localFormat,
                                    const cmnDataFormat & remoteFormat)
    CISST_THROW(std::runtime_error)
{
    // deserialize data
    typedef typename _matrixType::iterator iterator;
    const iterator end = data.end();
    iterator iter = data.begin();
    for (; iter != end; ++iter) {
        cmnData<typename _matrixType::value_type>::DeSerializeBinary(*iter, inputStream, localFormat, remoteFormat);
    }
}

template <class _matrixType>
void cmnDataMatrixDeSerializeBinaryResize(_matrixType & data,
                                          std::istream & inputStream,
                                          const cmnDataFormat & localFormat,
                                          const cmnDataFormat & remoteFormat)
    CISST_THROW(std::runtime_error)
{
    // deserialize size and resize
    size_t rows, cols;
    cmnDataDeSerializeBinary_size_t(rows, inputStream, localFormat, remoteFormat);
    cmnDataDeSerializeBinary_size_t(cols, inputStream, localFormat, remoteFormat);
    data.resize(rows, cols);
    cmnDataMatrixDeSerializeBinary(data, inputStream, localFormat, remoteFormat);
}

template <class _matrixType>
void cmnDataMatrixDeSerializeBinaryCheckSize(_matrixType & data,
                                             std::istream & inputStream,
                                             const cmnDataFormat & localFormat,
                                             const cmnDataFormat & remoteFormat)
    CISST_THROW(std::runtime_error)
{
    // deserialize size and check
    size_t rows, cols;
    cmnDataDeSerializeBinary_size_t(rows, inputStream, localFormat, remoteFormat);
    cmnDataDeSerializeBinary_size_t(cols, inputStream, localFormat, remoteFormat);
    if ((data.rows() != rows)
        || (data.cols() != cols)) {
        cmnThrow("cmnDataDeSerializeBinary: matrix size doesn't match");
        return;
    }
    cmnDataMatrixDeSerializeBinary(data, inputStream, localFormat, remoteFormat);
}

template <class _matrixType>
size_t cmnDataMatrixScalarNumber(const _matrixType & data, const bool includeSize)
{
    size_t result = includeSize ? 2 : 0;
    typedef typename _matrixType::value_type value_type;
    if (cmnData<value_type>::ScalarNumberIsFixed(data.Element(0, 0))) {
        return result + data.size() * cmnData<value_type>::ScalarNumber(data.Element(0, 0));
    }
    typedef typename _matrixType::const_iterator const_iterator;
    const const_iterator end = data.end();
    const_iterator iter = data.begin();
    for (; iter != end; ++iter) {
        result += cmnData<value_type>::ScalarNumber(*iter);
    }
    return result;
}

template <class _matrixType>
bool cmnDataMatrixScalarFindInMatrixIndex(const _matrixType & data, const size_t index,
                                          size_t & elementRow, size_t & elementCol, size_t & inElementIndex)
{
    typedef typename _matrixType::value_type value_type;
    size_t elementIndex;
    const size_t scalarNumber = cmnDataMatrixScalarNumber(data, false);
    if (cmnData<value_type>::ScalarNumberIsFixed(data.Element(0, 0))) {
        const size_t scalarNumberPerElement = cmnData<value_type>::ScalarNumber(data.Element(0, 0));
        if (scalarNumberPerElement == 0) {
            return false;
        }
        if (index < scalarNumber) {
            elementIndex = index / scalarNumberPerElement;
            inElementIndex = index % scalarNumberPerElement;
            elementRow = elementIndex / data.cols();
            elementCol = elementIndex % data.cols();
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
        elementRow = elementIndex / data.cols();
        elementCol = elementIndex % data.cols(); ;
        numberOfScalarsInElement = cmnData<value_type>::ScalarNumber(data.Element(elementRow, elementCol));
        firstScalarInElement = scalarCounter;
        lastScalarInElement = scalarCounter + numberOfScalarsInElement - 1;
        scalarCounter = lastScalarInElement + 1;
        elementIndex++;
        indexFound = ((index >= firstScalarInElement) && (index <= lastScalarInElement));
    } while ((!indexFound)
             && (elementIndex < data.size()));
    if (indexFound) {
        elementIndex--;
        inElementIndex = index - firstScalarInElement;
        return true;
    }
    return false;
}

template <class _matrixType>
std::string cmnDataMatrixScalarDescription(const _matrixType & data,
                                           const size_t index,
                                           const std::string & userDescription,
                                           const bool includeSize)
    CISST_THROW(std::out_of_range)
{
    size_t elementRow, elementCol, inElementIndex;
    size_t realIndex = index;
    std::stringstream suffix;
    if (includeSize) {
        if (index == 0) {
            return cmnData<size_t>::ScalarDescription(data.rows(), 0, userDescription + ".rows");
        }
        if (index == 1) {
            return cmnData<size_t>::ScalarDescription(data.cols(), 0, userDescription + ".cols");
        }
        realIndex -= 2;
    }
    if (cmnDataMatrixScalarFindInMatrixIndex(data, realIndex, elementRow, elementCol, inElementIndex)) {
        suffix << "[" << elementRow << ',' << elementCol << "]";
        return cmnData<typename _matrixType::value_type>::ScalarDescription(data.Element(elementRow, elementCol), inElementIndex, userDescription + suffix.str());
    }
    cmnThrow(std::out_of_range("cmnDataScalarDescription: matrix index out of range"));
    return "";
}

template <class _matrixType>
double cmnDataMatrixScalar(const _matrixType & data, const size_t index, const bool includeSize)
    CISST_THROW(std::out_of_range)
{
    size_t elementRow, elementCol, inElementIndex;
    size_t realIndex = index;
    if (includeSize) {
        if (index == 0) {
            return static_cast<double>(data.rows());
        }
        if (index == 1) {
            return static_cast<double>(data.cols());
        }
        realIndex -= 2;
    }
    if (cmnDataMatrixScalarFindInMatrixIndex(data, realIndex, elementRow, elementCol, inElementIndex)) {
        return cmnData<typename _matrixType::value_type>::Scalar(data.Element(elementRow, elementCol), inElementIndex);
    } else {
        cmnThrow(std::out_of_range("cmnDataScalar: matrix index out of range"));
    }
    return 0.123456789; // unreachable, just to avoid compiler warnings
}

#endif // _cmnDataFunctionsMatrixHelpers_h
