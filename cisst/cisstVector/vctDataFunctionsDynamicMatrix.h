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

#ifndef _vctDataFunctionsDynamicMatrix_h
#define _vctDataFunctionsDynamicMatrix_h

#include <cisstCommon/cmnDataFunctions.h>
#include <cisstVector/vctDataFunctionsMatrix.h>
#include <cisstVector/vctDynamicMatrixBase.h>


// there are two different specialization for matrixs, dynamic matrixs can be resized while references can't
template <typename _elementType, class _matrixOwnerTypeSource>
void cmnDataCopy(vctDynamicMatrix<_elementType> & destination,
                 const vctDynamicConstMatrixBase<_matrixOwnerTypeSource, _elementType> & source)
{
    // potentially resizes the destination
    destination.ForceAssign(source);
}

template <typename _elementType, class _matrixOwnerTypeSource>
void cmnDataCopy(vctDynamicMatrixRef<_elementType> & destination,
                 const vctDynamicConstMatrixBase<_matrixOwnerTypeSource, _elementType> & source)
{
    // this might fail if the destination is not properly sized
    destination.Assign(source);
}


// there is only one specialization since we only read and there is no size issue
template <typename _elementType, class _matrixOwnerType>
void cmnDataSerializeBinary(std::ostream & outputStream,
                            const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & data)
    throw (std::runtime_error)
{
    vct::size_type indexRow, indexCol;
    const vct::size_type myRows = data.rows();
    const vct::size_type myCols = data.cols();

    cmnSerializeSizeRaw(outputStream, myRows);
    cmnSerializeSizeRaw(outputStream, myCols);
    for (indexRow = 0; indexRow < myRows; ++indexRow) {
        for (indexCol = 0; indexCol < myCols; ++indexCol) {
            cmnDataSerializeBinary(outputStream, data.Element(indexRow, indexCol));
        }
    }
}


// as for the cmnDataCopy, two different specializations
template <typename _elementType>
void cmnDataDeSerializeBinary(std::istream & inputStream,
                              vctDynamicMatrix<_elementType> & data,
                              const cmnDataFormat & remoteFormat,
                              const cmnDataFormat & localFormat)
    throw (std::runtime_error)
{
    // for matrices that own memory, we resize the destination based on deserialized "size"
    vct::size_type myRows = 0;
    vct::size_type myCols = 0;
    cmnDataDeSerializeBinary(inputStream, myRows, remoteFormat, localFormat);
    cmnDataDeSerializeBinary(inputStream, myCols, remoteFormat, localFormat);
    data.SetSize(myRows, myCols);

    // get data
    vct::size_type indexRow, indexCol;
    for (indexRow = 0; indexRow < myRows; ++indexRow) {
        for (indexCol = 0; indexCol < myCols; ++indexCol) {
            cmnDataDeSerializeBinary(inputStream, data.Element(indexRow, indexCol), remoteFormat, localFormat);
        }
    }
}

template <typename _elementType>
void cmnDataDeSerializeBinary(std::istream & inputStream,
                              vctDynamicMatrixRef<_elementType> & data,
                              const cmnDataFormat & remoteFormat,
                              const cmnDataFormat & localFormat)
    throw (std::runtime_error)
{
    // get and check size
    vct::size_type myRows = 0;
    vct::size_type myCols = 0;
    cmnDataDeSerializeBinary(inputStream, myRows, remoteFormat, localFormat);
    cmnDataDeSerializeBinary(inputStream, myCols, remoteFormat, localFormat);

    if ((myRows != data.rows())
        || (myCols != data.cols())) {
        cmnThrow(std::runtime_error("cmnDataDeSerializeBinary: vctDynamicMatrixRef, size of matrices don't match"));
    }

    // get data
    vct::size_type indexRow, indexCol;
    for (indexRow = 0; indexRow < myRows; ++indexRow) {
        for (indexCol = 0; indexCol < myCols; ++indexCol) {
            cmnDataDeSerializeBinary(inputStream, data.Element(indexRow, indexCol), remoteFormat, localFormat);
        }
    }
}


template <class _matrixOwnerType, typename _elementType>
bool cmnDataScalarNumberIsFixed(const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & data)
{
    return false;
}


template <class _matrixOwnerType, typename _elementType>
size_t cmnDataScalarNumber(const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & data)
{
    if (cmnDataScalarNumberIsFixed(data.Element(0,0))) {
        return data.size() * cmnDataScalarNumber(data.Element(0, 0));
    }
    size_t result = 0;
    typedef typename vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::const_iterator const_iterator;
    const const_iterator end = data.end();
    const_iterator iter = data.begin();
    for (; iter != end; ++iter) {
        result += cmnDataScalarNumber(*iter);
    }
    return result;
}


template <class _matrixOwnerType, typename _elementType>
std::string
cmnDataScalarDescription(const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & data,
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


template <class _matrixOwnerType, typename _elementType>
double
cmnDataScalar(const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & data,
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


#endif // _vctDataFunctionsDynamicMatrix_h
