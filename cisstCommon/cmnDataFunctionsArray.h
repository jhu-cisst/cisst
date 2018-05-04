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
#ifndef _cmnDataFunctionsArray_h
#define _cmnDataFunctionsArray_h

#include <cisstCommon/cmnDataFunctions.h>
#include <cisstCommon/cmnDataFunctionsVectorHelpers.h>

// always include last
#include <cisstCommon/cmnExport.h>

template <class _elementType, size_t _size>
class cmnData<_elementType[_size]>
{
public:
    enum {IS_SPECIALIZED = 1};
    typedef _elementType * pointer;
    typedef const _elementType * const_pointer;

    static void Copy(pointer data, const_pointer source)
    {
        pointer dataPtr = data;
        const_pointer sourcePtr = source;
        for (size_t index = 0; index < _size; ++index, ++dataPtr, ++sourcePtr) {
            cmnData<_elementType>::Copy(*dataPtr, *sourcePtr);
        }
    }

    static std::string HumanReadable(const_pointer data)
    {
        const_pointer ptr = data;
        std::stringstream stringStream;
        stringStream << "[";
        for (size_t index = 0; index < _size; ++index, ++ptr) {
            if (index != 0) {
                stringStream << ", ";
            }
            stringStream << cmnData<_elementType>::HumanReadable(*ptr);
        }
        stringStream << "]";
        return stringStream.str();
    }

    static void SerializeText(const_pointer data,
                              std::ostream & outputStream,
                              const char delimiter) CISST_THROW(std::runtime_error)
    {
        const_pointer ptr = data;
        for (size_t index = 0; index < _size; ++index, ++ptr) {
            if (index != 0) {
                outputStream << delimiter;
            }
            cmnData<_elementType>::SerializeText(*ptr, outputStream, delimiter);
        }
    }

    static void DeSerializeText(pointer data,
                                std::istream & inputStream,
                                const char delimiter) CISST_THROW(std::runtime_error)
    {
        pointer ptr = data;
        for (size_t index = 0; index < _size; ++index, ++ptr) {
            if (index != 0) {
                cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "C Array");
            }
            cmnData<_elementType>::DeSerializeText(*ptr, inputStream, delimiter);
        }
    }

    static std::string SerializeDescription(const_pointer data,
                                            const char delimiter,
                                            const std::string & userDescription = "v")
    {
        std::string prefix = (userDescription == "") ? "v[" : (userDescription + "[");
        std::stringstream indexSuffix;
        std::stringstream description;
        const_pointer ptr = data;
        for (size_t index = 0; index < _size; ++index, ++ptr) {
            if (index != 0) {
                description << delimiter;
            }
            indexSuffix.clear();
            indexSuffix.str(std::string());
            indexSuffix << index << "]";
            description << cmnData<_elementType>::SerializeDescription(*ptr, delimiter, prefix + indexSuffix.str());
        }
        return description.str();
    }

    static void SerializeBinary(const_pointer data, std::ostream & outputStream)
        CISST_THROW(std::runtime_error)
    {
        const_pointer ptr = data;
        for (size_t index = 0; index < _size; ++index, ++ptr) {
            cmnData<_elementType>::SerializeBinary(*ptr, outputStream);
        }
    }

    static void DeSerializeBinary(pointer data,
                                  std::istream & inputStream,
                                  const cmnDataFormat & localFormat,
                                  const cmnDataFormat & remoteFormat)
        CISST_THROW(std::runtime_error)
    {
        pointer ptr = data;
        for (size_t index = 0; index < _size; ++index, ++ptr) {
            cmnData<_elementType>::DeSerializeBinary(*ptr, inputStream, localFormat, remoteFormat);
        }
    }

    static bool ScalarNumberIsFixed(const_pointer data)
    {
        return cmnData<_elementType>::ScalarNumberIsFixed(data[0]);
    }

    static size_t ScalarNumber(const_pointer data)
    {
        if (cmnData<_elementType>::ScalarNumberIsFixed(data[0])) {
            return _size * cmnData<_elementType>::ScalarNumber(data[0]);
        }
        size_t result = 0;
        const_pointer ptr = data;
        for (size_t index = 0; index < _size; ++index, ++ptr) {
            result += cmnData<_elementType>::ScalarNumber(*ptr);
        }
        return result;
    }

    static std::string ScalarDescription(const_pointer data, const size_t index,
                                         const std::string & userDescription = "v")
        CISST_THROW(std::out_of_range)
    {
        size_t elementIndex, inElementIndex;
        std::stringstream suffix;
        const size_t scalarNumber = cmnData<_elementType[_size]>::ScalarNumber(data);
        if (cmnDataVectorScalarFindInVectorIndex<const _elementType *, _elementType>(data, _size, scalarNumber,
                                                                                     index, elementIndex, inElementIndex)) {
            suffix << "[" << index << "]";
            return cmnData<_elementType>::ScalarDescription(data[elementIndex], inElementIndex, userDescription + suffix.str());
        }
        cmnThrow(std::out_of_range("cmnDataScalarDescription: C array index out of range"));
        return "";
    }

    static double Scalar(const_pointer data, const size_t index)
        CISST_THROW(std::out_of_range)
    {
        size_t elementIndex, inElementIndex;
        const size_t scalarNumber = cmnData<_elementType[_size]>::ScalarNumber(data);
        if (cmnDataVectorScalarFindInVectorIndex<const _elementType *, _elementType>(data, _size, scalarNumber,
                                                                                     index, elementIndex, inElementIndex)) {
            return cmnData<_elementType>::Scalar(data[elementIndex], inElementIndex);
        } else {
            cmnThrow(std::out_of_range("cmnDataScalar: C array index out of range"));
        }
        return 0.123456789; // unreachable, just to avoid compiler warnings
    }
};

#endif // _cmnDataFunctionsArray_h
