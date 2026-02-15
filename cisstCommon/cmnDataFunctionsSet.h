/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2026-02-13

  (C) Copyright 2026 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _cmnDataFunctionsSet_h
#define _cmnDataFunctionsSet_h

#include <cisstCommon/cmnDataFunctions.h>
#include <set>
#include <cisstCommon/cmnDataFunctionsVectorHelpers.h>
#include <cisstCommon/cmnDataFunctionsJSON.h>

// always include last
#include <cisstCommon/cmnExport.h>

template <class _elementType>
class cmnData<std::set<_elementType> >
{
public:
    enum {IS_SPECIALIZED = 1};

    typedef std::set<_elementType> DataType;

    static void Copy(DataType & data, const DataType & source)
    {
        data = source;
    }

    static std::string HumanReadable(const DataType & data)
    {
        return cmnDataVectorHumanReadable(data);
    }

    static void SerializeText(const DataType & data,
                          std::ostream & outputStream,
                          const char delimiter) CISST_THROW(std::runtime_error)
    {
        const size_t size = data.size();
        cmnData<size_t>::SerializeText(size, outputStream, delimiter);
        if (size > 0) {
            outputStream << delimiter;
            cmnDataVectorSerializeText(data, outputStream, delimiter);
        }
    }

    static void DeSerializeText(DataType & data,
                                std::istream & inputStream,
                                const char delimiter) CISST_THROW(std::runtime_error)
    {
        data.clear();
        size_t size;
        cmnData<size_t>::DeSerializeText(size, inputStream, delimiter);
        for (size_t i = 0; i < size; ++i) {
            cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "std::set");
            _elementType element;
            cmnData<_elementType>::DeSerializeText(element, inputStream, delimiter);
            data.insert(element);
        }
    }

    static std::string SerializeDescription(const DataType & data,
                                            const char delimiter,
                                            const std::string & userDescription = "v")
    {
        std::string prefix = (userDescription == "") ? "v[" : (userDescription + "[");
        std::stringstream indexSuffix;
        std::stringstream description;
        const size_t size = data.size();
        description << cmnData<size_t>::SerializeDescription(size, delimiter, (userDescription == "") ? "v.size" : (userDescription + ".size"));
        size_t index = 0;
        typename DataType::const_iterator iter = data.begin();
        typename DataType::const_iterator end = data.end();
        for (; iter != end; ++iter, ++index) {
            description << delimiter;
            indexSuffix.clear();
            indexSuffix.str(std::string());
            indexSuffix << index << "]";
            description << cmnData<_elementType>::SerializeDescription(*iter, delimiter, prefix + indexSuffix.str());
        }
        return description.str();
    }

    static void SerializeBinary(const DataType & data, std::ostream & outputStream)
        CISST_THROW(std::runtime_error)
    {
        cmnData<size_t>::SerializeBinary(data.size(), outputStream);
        cmnDataVectorSerializeBinary(data, outputStream);
    }

    static void DeSerializeBinary(DataType & data,
                                  std::istream & inputStream,
                                  const cmnDataFormat & localFormat,
                                  const cmnDataFormat & remoteFormat)
        CISST_THROW(std::runtime_error)
    {
        data.clear();
        size_t size;
        cmnDataDeSerializeBinary_size_t(size, inputStream, localFormat, remoteFormat);
        for (size_t i = 0; i < size; ++i) {
            _elementType element;
            cmnData<_elementType>::DeSerializeBinary(element, inputStream, localFormat, remoteFormat);
            data.insert(element);
        }
    }

    static bool ScalarNumberIsFixed(const DataType & CMN_UNUSED(data))
    {
        return false;
    }

    static size_t ScalarNumber(const DataType & data)
    {
        size_t result = 1; // for the size
        typename DataType::const_iterator iter = data.begin();
        typename DataType::const_iterator end = data.end();
        for (; iter != end; ++iter) {
            result += cmnData<_elementType>::ScalarNumber(*iter);
        }
        return result;
    }

    static std::string ScalarDescription(const DataType & data, const size_t index,
                                         const std::string & userDescription = "v")
        CISST_THROW(std::out_of_range)
    {
        if (index == 0) {
            return cmnData<size_t>::ScalarDescription(data.size(), 0, userDescription + ".size");
        }
        size_t currentIndex = 1;
        typename DataType::const_iterator iter = data.begin();
        typename DataType::const_iterator end = data.end();
        size_t elementIndex = 0;
        for (; iter != end; ++iter, ++elementIndex) {
            const size_t elementScalarNumber = cmnData<_elementType>::ScalarNumber(*iter);
            if (index < currentIndex + elementScalarNumber) {
                std::stringstream ss;
                ss << userDescription << "[" << elementIndex << "]";
                return cmnData<_elementType>::ScalarDescription(*iter, index - currentIndex, ss.str());
            }
            currentIndex += elementScalarNumber;
        }
        cmnThrow(std::out_of_range("cmnDataScalarDescription: set index out of range"));
        return "";
    }

    static double Scalar(const DataType & data, const size_t index)
        CISST_THROW(std::out_of_range)
    {
        if (index == 0) {
            return static_cast<double>(data.size());
        }
        size_t currentIndex = 1;
        typename DataType::const_iterator iter = data.begin();
        typename DataType::const_iterator end = data.end();
        for (; iter != end; ++iter) {
            const size_t elementScalarNumber = cmnData<_elementType>::ScalarNumber(*iter);
            if (index < currentIndex + elementScalarNumber) {
                return cmnData<_elementType>::Scalar( *iter, index - currentIndex);
            }
            currentIndex += elementScalarNumber;
        }
        cmnThrow(std::out_of_range("cmnDataScalar: set index out of range"));
        return 0.0;
    }
};

#if CISST_HAS_JSON
template <class _elementType>
class cmnDataJSON<std::set<_elementType> >
{
public:
    typedef std::set<_elementType> DataType;

    static void SerializeText(const DataType & data, Json::Value & jsonValue)
    {
        typename DataType::const_iterator iter = data.begin();
        typename DataType::const_iterator end = data.end();
        int jsonIndex = 0;
        for (; iter != end; ++iter, ++jsonIndex) {
            cmnDataJSON<_elementType>::SerializeText(*iter, jsonValue[jsonIndex]);
        }
    }

    static void DeSerializeText(DataType & data, const Json::Value & jsonValue) CISST_THROW(std::runtime_error)
    {
        data.clear();
        const int size = jsonValue.size();
        for (int jsonIndex = 0; jsonIndex < size; ++jsonIndex) {
            _elementType element;
            cmnDataJSON<_elementType>::DeSerializeText(element, jsonValue[jsonIndex]);
            data.insert(element);
        }
    }
};
#endif

#endif // _cmnDataFunctionsSet_h
