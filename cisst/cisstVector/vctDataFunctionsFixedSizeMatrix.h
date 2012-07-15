/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2012-07-09

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights
  Reserved.

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
#include <cisstVector/vctDataFunctionsMatrix.h>
#include <cisstVector/vctFixedSizeMatrixBase.h>


template <vct::size_type _rows, vct::size_type _cols, class _elementType,
          vct::stride_type _rowStrideDestination, vct::stride_type _colStrideDestination, class _dataPtrTypeDestination,
          vct::stride_type _rowStrideSource, vct::stride_type _colStrideSource, class _dataPtrTypeSource>
void cmnDataCopy(vctFixedSizeMatrixBase<_rows, _cols, _rowStrideDestination, _colStrideDestination, _elementType, _dataPtrTypeDestination> & destination,
                 const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStrideSource, _colStrideSource, _elementType, _dataPtrTypeSource> & source)
{
    destination.Assign(source);
}


template <vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride,
          class _elementType, class _dataPtrType>
void cmnDataSerializeBinary(std::ostream & outputStream,
                            const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & data)
    throw (std::runtime_error)
{
    const vct::size_type myRows = data.rows();
    const vct::size_type myCols = data.cols();
    vct::size_type indexRow, indexCol;
    for (indexRow = 0; indexRow < myRows; ++indexRow) {
        for (indexCol = 0; indexCol < myCols; ++indexCol) {
            cmnDataSerializeBinary(outputStream, data.Element(indexRow, indexCol));
        }
    }
}


template <vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride,
          class _elementType, class _dataPtrType>
void cmnDataDeSerializeBinary(std::istream & inputStream,
                              vctFixedSizeMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & data,
                              const cmnDataFormat & remoteFormat,
                              const cmnDataFormat & localFormat)
    throw (std::runtime_error)
{
    const vct::size_type myRows = data.rows();
    const vct::size_type myCols = data.cols();
    vct:: size_type indexRow, indexCol;

    for (indexRow = 0; indexRow < myRows; ++indexRow) {
        for (indexCol = 0; indexCol < myCols; ++indexCol) {
            cmnDataDeSerializeBinary(inputStream, data.Element(indexRow, indexCol), remoteFormat, localFormat);
        }
    }
}


template <vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride,
          class _elementType, class _dataPtrType>
bool cmnDataScalarNumberIsFixed(const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & data)
{
    return cmnDataScalarNumberIsFixed(data.Element(0, 0));
}


template <vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride,
          class _elementType, class _dataPtrType>
size_t cmnDataScalarNumber(const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & data)
{
    if (cmnDataScalarNumberIsFixed(data.Element(0, 0))) {
        return data.size() * cmnDataScalarNumber(data.Element(0, 0));
    }
    size_t result = 0;
    typedef typename vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType>::const_iterator const_iterator;
    const const_iterator end = data.end();
    const_iterator iter = data.begin();
    for (; iter != end; ++iter) {
        result += cmnDataScalarNumber(*iter);
    }
    return result;
}


template <vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride,
          class _elementType, class _dataPtrType>
std::string
cmnDataScalarDescription(const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & data,
                         const size_t & index)
    throw (std::out_of_range)
{
    size_t elementRow, elementCol, inElementIndex;
    std::stringstream result;
    if (vctDataFindInMatrixScalarIndex(data, index, elementRow, elementCol, inElementIndex)) {
        result << "v[" << elementRow << "," << elementCol << "]{" << cmnDataScalarDescription(data.Element(elementRow, elementCol), inElementIndex) << "}";
    } else {
        cmnThrow(std::out_of_range("cmnDataScalarDescription: vctFixedSizeMatrix index out of range"));
    }
    return result.str(); // unreachable, just to avoid compiler warnings
}


template <vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride,
          class _elementType, class _dataPtrType>
double
cmnDataScalar(const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & data,
              const size_t & index)
    throw (std::out_of_range)
{
    size_t elementRow, elementCol, inElementIndex;
    if (vctDataFindInMatrixScalarIndex(data, index, elementRow, elementCol, inElementIndex)) {
        return cmnDataScalar(data.Element(elementRow, elementCol), inElementIndex);
    } else {
        cmnThrow(std::out_of_range("cmnDataScalar: vctFixedSizeMatrix index out of range"));
    }
    return 0.123456789; // unreachable, just to avoid compiler warnings
}


#endif // _vctDataFunctionsFixedSizeMatrix_h
