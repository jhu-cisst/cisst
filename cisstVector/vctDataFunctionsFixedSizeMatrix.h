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

#ifndef _vctDataFunctionsFixedSizeMatrix_h
#define _vctDataFunctionsFixedSizeMatrix_h

#include <cisstCommon/cmnDataFunctions.h>
#include <cisstVector/vctFixedSizeMatrixBase.h>
#include <cisstCommon/cmnDataFunctionsMatrixHelpers.h>

#if CISST_HAS_JSON
#include <cisstVector/vctDataFunctionsFixedSizeMatrixJSON.h>
#endif // CISST_HAS_JSON

template <class _elementType, vct::size_type _rows, vct::size_type _cols, bool _rowMajor>
class cmnData<vctFixedSizeMatrix<_elementType, _rows, _cols, _rowMajor> >
{
public:
    enum {IS_SPECIALIZED = 1};

    typedef vctFixedSizeMatrix<_elementType, _rows, _cols, _rowMajor> DataType;

    static void Copy(DataType & data, const DataType & source)
    {
        data.Assign(source);
    }

    static std::string HumanReadable(const DataType & data)
    {
        return cmnDataMatrixHumanReadable(data);
    }

    static void SerializeBinary(const DataType & data, std::ostream & outputStream)
        CISST_THROW(std::runtime_error)
    {
        cmnDataMatrixSerializeBinary(data, outputStream);
    }

    static void DeSerializeBinary(DataType & data,
                                  std::istream & inputStream,
                                  const cmnDataFormat & localFormat,
                                  const cmnDataFormat & remoteFormat)
        CISST_THROW(std::runtime_error)
    {
        cmnDataMatrixDeSerializeBinary(data, inputStream, localFormat, remoteFormat);
    }

    static void SerializeText(const DataType & data,
                              std::ostream & outputStream,
                              const char delimiter = ',')
        CISST_THROW(std::runtime_error)
    {
        cmnDataMatrixSerializeText(data, outputStream, delimiter);
    }

    static std::string SerializeDescription(const DataType & data,
                                            const char delimiter = ',',
                                            const std::string & userDescription = "m")
    {
        return cmnDataMatrixSerializeDescription(data, delimiter, userDescription, false /* no need to serialize size */);
    }

    static void DeSerializeText(DataType & data,
                                std::istream & inputStream,
                                const char delimiter = ',')
        CISST_THROW(std::runtime_error)
    {
        cmnDataMatrixDeSerializeText(data, inputStream, delimiter);
    }

    static bool ScalarNumberIsFixed(const DataType & data)
    {
        return ((data.size() == 0) || cmnData<_elementType>::ScalarNumberIsFixed(data.Element(0, 0)));
    }

    static size_t ScalarNumber(const DataType & data)
    {
        return cmnDataMatrixScalarNumber(data, false /* do not treat size as a scalar*/);
    }

    static std::string ScalarDescription(const DataType & data,
                                         const size_t & index,
                                         const std::string & userDescription = "m")
        CISST_THROW(std::out_of_range)
    {
        return cmnDataMatrixScalarDescription(data, index, userDescription, false /* do not treat size as a scalar */);
    }

    static double Scalar(const DataType & data, const size_t & index)
        CISST_THROW(std::out_of_range)
    {
        return cmnDataMatrixScalar(data, index, false /* do not treat size as a scalar */);
    }
};

// ---------------------- older functions, to be deprecated
template <typename _elementType, vct::size_type _rows, vct::size_type _cols>
inline void cmnDeSerializeRaw(std::istream & inputStream,
                              vctFixedSizeMatrix<_elementType, _rows, _cols> & matrix)
    CISST_THROW(std::runtime_error)
{
    matrix.DeSerializeRaw(inputStream);
}

template <typename _elementType, vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride>
inline void cmnDeSerializeRaw(std::istream & inputStream,
                              vctFixedSizeMatrixRef<_elementType, _rows, _cols, _rowStride, _colStride> & matrix)
    CISST_THROW(std::runtime_error)
{
    matrix.DeSerializeRaw(inputStream);
}

template <typename _elementType, vct::size_type _rows, vct::size_type _cols>
inline void cmnSerializeRaw(std::ostream & outputStream,
                            const vctFixedSizeMatrix<_elementType, _rows, _cols> & matrix)
    CISST_THROW(std::runtime_error)
{
    matrix.SerializeRaw(outputStream);
}

template <typename _elementType, vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride>
inline void cmnSerializeRaw(std::ostream & outputStream,
                            const vctFixedSizeMatrixRef<_elementType, _rows, _cols, _rowStride, _colStride> & matrix)
    CISST_THROW(std::runtime_error)
{
    matrix.SerializeRaw(outputStream);
}

template <typename _elementType, vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride>
inline void cmnSerializeRaw(std::ostream & outputStream,
                            const vctFixedSizeConstMatrixRef<_elementType, _rows, _cols, _rowStride, _colStride> & matrix)
    CISST_THROW(std::runtime_error)
{
    matrix.SerializeRaw(outputStream);
}

// ----------------------- end of older functions

#endif // _vctDataFunctionsFixedSizeMatrix_h
