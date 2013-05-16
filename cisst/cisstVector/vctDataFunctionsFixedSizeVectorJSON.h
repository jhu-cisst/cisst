/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2012-07-09

  (C) Copyright 2012-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#pragma once

#ifndef _vctDataFunctionsFixedSizeVectorJSON_h
#define _vctDataFunctionsFixedSizeVectorJSON_h

#include <cisstCommon/cmnDataFunctionsJSON.h>
#include <cisstVector/vctFixedSizeVectorBase.h>

#if CISST_HAS_JSON
template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
void cmnDataToJSON(const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector,
                   Json::Value & jsonValue) {
    typedef vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> VectorType;
    typedef typename VectorType::const_iterator const_iterator;
    const const_iterator end = vector.end();
    const_iterator iter;
    int index = 0;
    for (iter = vector.begin();
         iter != end;
         ++index, ++iter) {
        cmnDataToJSON(*iter, jsonValue[index]);
    }
}

template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
inline void cmnDataFromJSON(vctFixedSizeVectorBase<_size, _stride, _elementType, _dataPtrType> & vector,
                            const Json::Value & jsonValue)
    throw (std::runtime_error)
{
    // make sure both vectors have the same size
    if (vector.size() != jsonValue.size()) {
        cmnThrow("cmnDataFromJSON: vector sizes don't match");
    }
    typedef vctFixedSizeVectorBase<_size, _stride, _elementType, _dataPtrType> VectorType;
    typedef typename VectorType::iterator iterator;
    const iterator end = vector.end();
    iterator iter;
    int index = 0;
    for (iter = vector.begin();
         iter != end;
         ++index, ++iter) {
        cmnDataFromJSON(*iter, jsonValue[index]);
    }
}

template <class _elementType, vct::size_type _size, vct::stride_type _stride>
inline void cmnDataFromJSON(vctFixedSizeVectorRef<_elementType, _size, _stride> vector,
                            const Json::Value & jsonValue)
    throw (std::runtime_error)
{
    // make sure both vectors have the same size
    if (vector.size() != jsonValue.size()) {
        cmnThrow("cmnDataFromJSON: vector sizes don't match");
    }
    typedef vctFixedSizeVectorRef<_elementType, _size, _stride> VectorType;
    typedef typename VectorType::iterator iterator;
    const iterator end = vector.end();
    iterator iter;
    int index = 0;
    for (iter = vector.begin();
         iter != end;
         ++index, ++iter) {
        cmnDataFromJSON(*iter, jsonValue[index]);
    }
}

#endif // CISST_HAS_JSON

#endif // _vctDataFunctionsFixedSizeVectorJSON_h
