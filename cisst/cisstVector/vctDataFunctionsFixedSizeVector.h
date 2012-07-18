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

#ifndef _vctDataFunctionsFixedSizeVector_h
#define _vctDataFunctionsFixedSizeVector_h

#include <cisstCommon/cmnDataFunctions.h>
#include <cisstVector/vctDataFunctionsVector.h>
#include <cisstVector/vctFixedSizeVectorBase.h>


template <vct::size_type _size, class _elementType,
          vct::stride_type _strideDestination, class _dataPtrTypeDestination,
          vct::stride_type _strideSource, class _dataPtrTypeSource>
void cmnDataCopy(vctFixedSizeVectorBase<_size, _strideDestination, _elementType, _dataPtrTypeDestination> & destination,
                 const vctFixedSizeConstVectorBase<_size, _strideSource, _elementType, _dataPtrTypeSource> & source)
{
    destination.Assign(source);
}


template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
void cmnDataSerializeBinary(std::ostream & outputStream,
                            const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & data)
    throw (std::runtime_error)
{
    vct::size_type index;
    const vct::size_type mySize = data.size();
    for (index = 0; index < mySize; ++index) {
        cmnDataSerializeBinary(outputStream, data.Element(index));
    }
}


template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
void cmnDataDeSerializeBinary(std::istream & inputStream,
                              vctFixedSizeVectorBase<_size, _stride, _elementType, _dataPtrType> & data,
                              const cmnDataFormat & remoteFormat,
                              const cmnDataFormat & localFormat)
    throw (std::runtime_error)
{
    vct::size_type index;
    const vct::size_type mySize = data.size();
    for (index = 0; index < mySize; ++index) {
        cmnDataDeSerializeBinary(inputStream, data.Element(index), remoteFormat, localFormat);
    }
}


template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
bool cmnDataScalarNumberIsFixed(const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & data)
{
    return cmnDataScalarNumberIsFixed(data.Element(0));
}


template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
size_t cmnDataScalarNumber(const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & data)
{
    if (cmnDataScalarNumberIsFixed(data.Element(0))) {
        return data.size() * cmnDataScalarNumber(data.Element(0));
    }
    size_t result = 0;
    typedef typename vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType>::const_iterator const_iterator;
    const const_iterator end = data.end();
    const_iterator iter = data.begin();
    for (; iter != end; ++iter) {
        result += cmnDataScalarNumber(*iter);
    }
    return result;
}


template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
std::string
cmnDataScalarDescription(const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & data,
                         const size_t & index,
                         const char * userDescription = "v")
    throw (std::out_of_range)
{
    size_t elementIndex, inElementIndex;
    std::stringstream result;
    if (vctDataFindInVectorScalarIndex(data, index, elementIndex, inElementIndex)) {
        result << userDescription << "[" << elementIndex << "]{" << cmnDataScalarDescription(data.Element(elementIndex), inElementIndex) << "}";
    } else {
        cmnThrow(std::out_of_range("cmnDataScalarDescription: vctFixedSizeVector index out of range"));
    }
    return result.str(); // unreachable, just to avoid compiler warnings
}


template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
double
cmnDataScalar(const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & data,
              const size_t & index)
    throw (std::out_of_range)
{
    size_t elementIndex, inElementIndex;
    if (vctDataFindInVectorScalarIndex(data, index, elementIndex, inElementIndex)) {
        return cmnDataScalar(data.Element(elementIndex), inElementIndex);
    } else {
        cmnThrow(std::out_of_range("cmnDataScalar: vctFixedSizeVector index out of range"));
    }
    return 0.123456789; // unreachable, just to avoid compiler warnings
}


#endif // _vctDataFunctionsFixedSizeVector_h
