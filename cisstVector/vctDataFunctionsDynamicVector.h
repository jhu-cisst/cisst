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

#ifndef _vctDataFunctionsDynamicVector_h
#define _vctDataFunctionsDynamicVector_h

#include <cisstCommon/cmnDataFunctions.h>
#include <cisstVector/vctDynamicVectorBase.h>
#include <cisstCommon/cmnDataFunctionsVectorHelpers.h>

#if CISST_HAS_JSON
#include <cisstVector/vctDataFunctionsDynamicVectorJSON.h>
#endif // CISST_HAS_JSON

template <typename _elementType>
class cmnData<vctDynamicVector<_elementType> >
{
public:
    enum {IS_SPECIALIZED = 1};

    typedef vctDynamicVector<_elementType> DataType;

    static void Copy(DataType & data, const DataType & source)
    {
        // potentially resizes the destination
        data.ForceAssign(source);
    }

    static std::string HumanReadable(const DataType & data)
    {
        return cmnDataVectorHumanReadable(data);
    }

    static void SerializeBinary(const DataType & data,
                                       std::ostream & outputStream)
        CISST_THROW(std::runtime_error)
    {
        const vct::size_type mySize = data.size();
        cmnData<size_t>::SerializeBinary(mySize, outputStream);
        cmnDataVectorSerializeBinary(data, outputStream);
    }

    static void DeSerializeBinary(DataType & data,
                                  std::istream & inputStream,
                                  const cmnDataFormat & localFormat,
                                  const cmnDataFormat & remoteFormat)
        CISST_THROW(std::runtime_error)
    {
        cmnDataVectorDeSerializeBinaryResize(data, inputStream, localFormat, remoteFormat);
    }

    static void SerializeText(const DataType & data,
                              std::ostream & outputStream,
                              const char delimiter = ',')
        CISST_THROW(std::runtime_error)
    {
        const size_t size = data.size();
        cmnData<size_t>::SerializeText(size, outputStream, delimiter);
        if (size > 0) {
            outputStream << delimiter;
            cmnDataVectorSerializeText(data, outputStream, delimiter);
        }
    }

    static std::string SerializeDescription(const DataType & data,
                                            const char delimiter = ',',
                                            const std::string & userDescription = "v")
    {
        return cmnDataVectorSerializeDescription(data, delimiter, userDescription, true /* need to serialize size */);
    }

    static void DeSerializeText(DataType & data,
                                std::istream & inputStream,
                                const char delimiter = ',')
        CISST_THROW(std::runtime_error)
    {
        cmnDataVectorDeSerializeTextResize(data, inputStream, delimiter);
    }

    static bool ScalarNumberIsFixed(const DataType & CMN_UNUSED(data))
    {
        return false;
    }

    static size_t ScalarNumber(const DataType & data)
    {
        return cmnDataVectorScalarNumber(data) + 1; /* treat vector size as a scalar */
    }

    static std::string ScalarDescription(const DataType & data, const size_t & index, const std::string & userDescription = "v")
        CISST_THROW(std::out_of_range)
    {
        /* treat vector size as a scalar */
        if (index == 0) {
            return cmnData<size_t>::ScalarDescription(data.size(), 0, userDescription + ".size");
        }
        return cmnDataVectorScalarDescription(data, index - 1, userDescription);
    }

    static double Scalar(const DataType & data, const size_t & index) CISST_THROW(std::out_of_range)
    {
        /* treat vector size as a scalar */
        if (index == 0) {
            return static_cast<double>(data.size());
        }
        return cmnDataVectorScalar(data, index - 1);
    }
};

// ---------------------- older functions, to be deprecated
template <typename _elementType>
inline void cmnDeSerializeRaw(std::istream & inputStream,
                              vctDynamicVector<_elementType> & vector)
    CISST_THROW(std::runtime_error)
{
    vector.DeSerializeRaw(inputStream);
}

template <typename _elementType>
inline void cmnDeSerializeRaw(std::istream & inputStream,
                              vctDynamicVectorRef<_elementType> & vector)
    CISST_THROW(std::runtime_error)
{
    vector.DeSerializeRaw(inputStream);
}

template <typename _elementType>
inline void cmnSerializeRaw(std::ostream & outputStream,
                            const vctDynamicVector<_elementType> & vector)
    CISST_THROW(std::runtime_error)
{
    vector.SerializeRaw(outputStream);
}

template <typename _elementType>
inline void cmnSerializeRaw(std::ostream & outputStream,
                            const vctDynamicVectorRef<_elementType> & vector)
    CISST_THROW(std::runtime_error)
{
    vector.SerializeRaw(outputStream);
}

template <typename _elementType>
inline void cmnSerializeRaw(std::ostream & outputStream,
                            const vctDynamicConstVectorRef<_elementType> & vector)
    CISST_THROW(std::runtime_error)
{
    vector.SerializeRaw(outputStream);
}

#endif // _vctDataFunctionsDynamicVector_h
