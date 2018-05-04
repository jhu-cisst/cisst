/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2012-07-09

  (C) Copyright 2012-201 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once

#ifndef _vctDataFunctionsFixedSizeVector_h
#define _vctDataFunctionsFixedSizeVector_h

#include <cisstCommon/cmnDataFunctions.h>
#include <cisstVector/vctFixedSizeVectorBase.h>
#include <cisstCommon/cmnDataFunctionsVectorHelpers.h>

#if CISST_HAS_JSON
#include <cisstVector/vctDataFunctionsFixedSizeVectorJSON.h>
#endif // CISST_HAS_JSON

template <class _elementType, vct::size_type _size>
class cmnData<vctFixedSizeVector<_elementType, _size> >
{
public:
    enum {IS_SPECIALIZED = 1};

    typedef vctFixedSizeVector<_elementType, _size> DataType;

    static void Copy(DataType & data, const DataType & source)
    {
        data.Assign(source);
    }

    static std::string HumanReadable(const DataType & data)
    {
        return cmnDataVectorHumanReadable(data);
    }

    static void SerializeBinary(const DataType & data, std::ostream & outputStream)
        CISST_THROW(std::runtime_error)
    {
        cmnDataVectorSerializeBinary(data, outputStream);
    }

    static void DeSerializeBinary(DataType & data, std::istream & inputStream,
                                  const cmnDataFormat & localFormat,
                                  const cmnDataFormat & remoteFormat)
        CISST_THROW(std::runtime_error)
    {
        cmnDataVectorDeSerializeBinary(data, inputStream, localFormat, remoteFormat);
    }

    static void SerializeText(const DataType & data, std::ostream & outputStream,
                              const char delimiter = ',')
        CISST_THROW(std::runtime_error)
    {
        cmnDataVectorSerializeText(data, outputStream, delimiter);
    }

    static std::string SerializeDescription(const DataType & data, const char delimiter = ',',
                                            const std::string & userDescription = "v")
    {
        return cmnDataVectorSerializeDescription(data, delimiter, userDescription, false /* no need to serialize size */);
    }

    static void DeSerializeText(DataType & data, std::istream & inputStream,
                                const char delimiter = ',')
        CISST_THROW(std::runtime_error)
    {
        cmnDataVectorDeSerializeText(data, inputStream, delimiter);
    }

    static bool ScalarNumberIsFixed(const DataType & data)
    {
        return cmnData<typename DataType::value_type>::ScalarNumberIsFixed(data.Element(0));
    }

    static size_t ScalarNumber(const DataType & data)
    {
        return cmnDataVectorScalarNumber(data);
    }

    static std::string ScalarDescription(const DataType & data, const size_t & index,
                                         const std::string & userDescription = "v")
        CISST_THROW(std::out_of_range)
    {
        return cmnDataVectorScalarDescription(data, index, userDescription);
    }

    static double Scalar(const DataType & data, const size_t & index)
        CISST_THROW(std::out_of_range)
    {
        return cmnDataVectorScalar(data, index);
    }
};

// ---------------------- older functions, to be deprecated
template <typename _elementType, vct::size_type _size>
inline void cmnDeSerializeRaw(std::istream & inputStream,
                              vctFixedSizeVector<_elementType, _size> & vector)
    CISST_THROW(std::runtime_error)
{
    vector.DeSerializeRaw(inputStream);
}

template <typename _elementType, vct::size_type _size, vct::stride_type _stride>
inline void cmnDeSerializeRaw(std::istream & inputStream,
                              vctFixedSizeVectorRef<_elementType, _size, _stride> & vector)
    CISST_THROW(std::runtime_error)
{
    vector.DeSerializeRaw(inputStream);
}

template <typename _elementType, vct::size_type _size>
inline void cmnSerializeRaw(std::ostream & outputStream,
                            const vctFixedSizeVector<_elementType, _size> & vector)
    CISST_THROW(std::runtime_error)
{
    vector.SerializeRaw(outputStream);
}

template <typename _elementType, vct::size_type _size, vct::stride_type _stride>
inline void cmnSerializeRaw(std::ostream & outputStream,
                            const vctFixedSizeVectorRef<_elementType, _size, _stride> & vector)
    CISST_THROW(std::runtime_error)
{
    vector.SerializeRaw(outputStream);
}

template <typename _elementType, vct::size_type _size, vct::stride_type _stride>
inline void cmnSerializeRaw(std::ostream & outputStream,
                            const vctFixedSizeConstVectorRef<_elementType, _size, _stride> & vector)
    CISST_THROW(std::runtime_error)
{
    vector.SerializeRaw(outputStream);
}
// ----------------------- end of older functions

#endif // _vctDataFunctionsFixedSizeVector_h
