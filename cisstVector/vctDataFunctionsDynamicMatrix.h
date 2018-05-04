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

#ifndef _vctDataFunctionsDynamicMatrix_h
#define _vctDataFunctionsDynamicMatrix_h

#include <cisstCommon/cmnDataFunctions.h>
#include <cisstVector/vctDynamicMatrixBase.h>
#include <cisstCommon/cmnDataFunctionsMatrixHelpers.h>

#if CISST_HAS_JSON
#include <cisstVector/vctDataFunctionsDynamicMatrixJSON.h>
#endif // CISST_HAS_JSON

template <typename _elementType>
class cmnData<vctDynamicMatrix<_elementType> >
{
public:
    enum {IS_SPECIALIZED = 1};

    typedef vctDynamicMatrix<_elementType> DataType;

    static void Copy(DataType & data, const DataType & source)
    {
        // potentially resizes the destination
        data.ForceAssign(source);
    }

    static std::string HumanReadable(const DataType & data)
    {
        return cmnDataMatrixHumanReadable(data);
    }

    static void SerializeBinary(const DataType & data, std::ostream & outputStream)
        CISST_THROW(std::runtime_error)
    {
        const vct::size_type myRows = data.rows();
        const vct::size_type myCols = data.cols();
        cmnData<size_t>::SerializeBinary(myRows, outputStream);
        cmnData<size_t>::SerializeBinary(myCols, outputStream);
        cmnDataMatrixSerializeBinary(data, outputStream);
    }

    static void DeSerializeBinary(DataType & data, std::istream & inputStream,
                                  const cmnDataFormat & localFormat, const cmnDataFormat & remoteFormat)
        CISST_THROW(std::runtime_error)
    {
        cmnDataMatrixDeSerializeBinaryResize(data, inputStream, localFormat, remoteFormat);
    }

    static void SerializeText(const DataType & data, std::ostream & outputStream, const char delimiter = ',')
        CISST_THROW(std::runtime_error)
    {
        const vct::size_type myRows = data.rows();
        const vct::size_type myCols = data.cols();
        cmnData<size_t>::SerializeText(myRows, outputStream, delimiter);
        outputStream << delimiter;
        cmnData<size_t>::SerializeText(myCols, outputStream, delimiter);
        if (data.size() > 0) {
            outputStream << delimiter;
            cmnDataMatrixSerializeText(data, outputStream, delimiter);
        }
    }

    static std::string SerializeDescription(const DataType & data, const char delimiter = ',',
                                            const std::string & userDescription = "m")
    {
        return cmnDataMatrixSerializeDescription(data, delimiter, userDescription, true /* need to serialize size */);
    }

    static void DeSerializeText(DataType & data, std::istream & inputStream, const char delimiter = ',')
        CISST_THROW(std::runtime_error)
    {
        cmnDataMatrixDeSerializeTextResize(data, inputStream, delimiter);
    }

    static bool ScalarNumberIsFixed(const DataType & CMN_UNUSED(data))
    {
        return false;
    }

    static size_t ScalarNumber(const DataType & data)
    {
        return cmnDataMatrixScalarNumber(data, true /* treat size as a scalar */);
    }

    static std::string ScalarDescription(const DataType & data, const size_t index, const std::string & userDescription = "m")
        CISST_THROW(std::out_of_range)
    {
        return cmnDataMatrixScalarDescription(data, index, userDescription, true /* treat size as a scalar */);
    }

    static double Scalar(const DataType & data, const size_t index)
        CISST_THROW(std::out_of_range)
    {
        return cmnDataMatrixScalar(data, index, true /* treat size as a scalar */);
    }
};

// ---------------------- older functions, to be deprecated
template <typename _elementType>
inline void cmnDeSerializeRaw(std::istream & inputStream,
                              vctDynamicMatrix<_elementType> & matrix)
    CISST_THROW(std::runtime_error)
{
    matrix.DeSerializeRaw(inputStream);
}

template <typename _elementType>
inline void cmnDeSerializeRaw(std::istream & inputStream,
                              vctDynamicMatrixRef<_elementType> & matrix)
    CISST_THROW(std::runtime_error)
{
    matrix.DeSerializeRaw(inputStream);
}

template <typename _elementType>
inline void cmnSerializeRaw(std::ostream & outputStream,
                            const vctDynamicMatrix<_elementType> & matrix)
    CISST_THROW(std::runtime_error)
{
    matrix.SerializeRaw(outputStream);
}

template <typename _elementType>
inline void cmnSerializeRaw(std::ostream & outputStream,
                            const vctDynamicMatrixRef<_elementType> & matrix)
    CISST_THROW(std::runtime_error)
{
    matrix.SerializeRaw(outputStream);
}

template <typename _elementType>
inline void cmnSerializeRaw(std::ostream & outputStream,
                            const vctDynamicConstMatrixRef<_elementType> & matrix)
    CISST_THROW(std::runtime_error)
{
    matrix.SerializeRaw(outputStream);
}

#endif // _vctDataFunctionsDynamicMatrix_h
