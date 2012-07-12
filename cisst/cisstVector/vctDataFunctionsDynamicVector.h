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

#ifndef _vctDataFunctionsDynamicVector_h
#define _vctDataFunctionsDynamicVector_h

#include <cisstCommon/cmnDataFunctions.h>
#include <cisstVector/vctDataFunctionsVector.h>
#include <cisstVector/vctDynamicVectorBase.h>


// there are two different specialization for vectors, dynamic vectors can be resized while references can't
template <typename _elementType, class _vectorOwnerTypeSource>
void cmnDataCopy(vctDynamicVector<_elementType> & destination,
                 const vctDynamicConstVectorBase<_vectorOwnerTypeSource, _elementType> & source)
{
    // potentially resizes the destination
    destination.ForceAssign(source);
}

template <typename _elementType, class _vectorOwnerTypeSource>
void cmnDataCopy(vctDynamicVectorRef<_elementType> & destination,
                 const vctDynamicConstVectorBase<_vectorOwnerTypeSource, _elementType> & source)
{
    // this might fail if the destination is not properly sized
    destination.Assign(source);
}


// there is only one specialization since we only read and there is no size issue
template <typename _elementType, class _vectorOwnerType>
void cmnDataSerializeBinary(std::ostream & outputStream,
                            const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & data)
    throw (std::runtime_error)
{
    vct::size_type index;
    const vct::size_type mySize = data.size();

    cmnDataSerializeBinary(outputStream, mySize);
    for (index = 0; index < mySize; ++index) {
        cmnDataSerializeBinary(outputStream, data.Element(index));
    }
}


// as for the cmnDataCopy, two different specializations
template <typename _elementType>
void cmnDataDeSerializeBinary(std::istream & inputStream,
                              vctDynamicVector<_elementType> & data,
                              const cmnDataFormat & remoteFormat,
                              const cmnDataFormat & localFormat)
    throw (std::runtime_error)
{
    // for vectors that own memory, we resize the destination based on deserialized "size"
    vct::size_type mySize = 0;
    cmnDataDeSerializeBinary(inputStream, mySize, remoteFormat, localFormat);
    data.SetSize(mySize);

    // get data
    vct::size_type index;
    for (index = 0; index < mySize; ++index) {
        cmnDataDeSerializeBinary(inputStream, data.Element(index), remoteFormat, localFormat);
    }
}

template <typename _elementType>
void cmnDataDeSerializeBinary(std::istream & inputStream,
                              vctDynamicVectorRef<_elementType> & data,
                              const cmnDataFormat & remoteFormat,
                              const cmnDataFormat & localFormat)
    throw (std::runtime_error)
{
    // get and set size
    vct::size_type mySize;
    cmnDataDeSerializeBinary(inputStream, mySize, remoteFormat, localFormat);

    if (mySize != data.size()) {
        cmnThrow(std::runtime_error("cmnDataDeSerializeBinary: vctDynamicVectorRef, size of vectors don't match"));
    }

    // get data
    vct::size_type index;
    for (index = 0; index < mySize; ++index) {
        cmnDataDeSerializeBinary(inputStream, data.Element(index), remoteFormat, localFormat);
    }
}


template <class _vectorOwnerType, typename _elementType>
bool cmnDataScalarNumberIsFixed(const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & data)
{
    return false;
}


template <class _vectorOwnerType, typename _elementType>
size_t cmnDataScalarNumber(const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & data)
{
    if (cmnDataScalarNumberIsFixed(data)) {
        return data.size() * cmnDataScalarNumber(data.Element(0));
    }
    size_t result = 0;
    typedef typename vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::const_iterator const_iterator;
    const const_iterator end = data.end();
    const_iterator iter = data.begin();
    for (; iter != end; ++iter) {
        result += cmnDataScalarNumber(*iter);
    }
    return result;
}


template <class _vectorOwnerType, typename _elementType>
std::string
cmnDataScalarDescription(const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & data,
                         const size_t & index)
    throw (std::out_of_range)
{
    size_t elementIndex, inElementIndex;
    std::stringstream result;
    if (vctDataFindInVectorScalarIndex(data, index, elementIndex, inElementIndex)) {
        result << "v[" << elementIndex << "]{" << cmnDataScalarDescription(data.Element(elementIndex), inElementIndex) << "}";
    } else {
        cmnThrow(std::out_of_range("cmnDataScalarDescription: vctDynamicVector index out of range"));
    }
    return result.str(); // unreachable, just to avoid compiler warnings
}


template <class _vectorOwnerType, typename _elementType>
double
cmnDataScalar(const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & data,
              const size_t & index)
    throw (std::out_of_range)
{
    size_t elementIndex, inElementIndex;
    if (vctDataFindInVectorScalarIndex(data, index, elementIndex, inElementIndex)) {
        return cmnDataScalar(data.Element(elementIndex), inElementIndex);
    } else {
        cmnThrow(std::out_of_range("cmnDataScalar: vctDynamicVector index out of range"));
    }
    return 0.123456789; // unreachable, just to avoid compiler warnings
}


#endif // _vctDataFunctionsDynamicVector_h
