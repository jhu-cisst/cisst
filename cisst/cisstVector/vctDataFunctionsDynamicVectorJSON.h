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

#ifndef _vctDataFunctionsDynamicVectorJSON_h
#define _vctDataFunctionsDynamicVectorJSON_h

#include <cisstCommon/cmnDataFunctionsJSON.h>
#include <cisstVector/vctDynamicVectorBase.h>

#if CISST_HAS_JSON
template <typename _elementType, typename _vectorOwnerType>
void cmnDataToJSON(const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & vector,
                   Json::Value & jsonValue) {
    typedef vctDynamicConstVectorBase<_vectorOwnerType, _elementType> VectorType;
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

template <typename _elementType>
void cmnDataFromJSON(vctDynamicVector<_elementType> & vector,
                     const Json::Value & jsonValue) throw (std::runtime_error) {
    // get the vector size from JSON and resize
    vector.SetSize(jsonValue.size());
    typedef vctDynamicVector<_elementType> VectorType;
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

template <typename _elementType>
void cmnDataFromJSON(vctDynamicVectorRef<_elementType> vector,
                     const Json::Value & jsonValue) throw (std::runtime_error) {
    // make sure both vectors have the same size
    if (vector.size() != jsonValue.size()) {
        cmnThrow("cmnDataFromJSON: vector sizes don't match");
    }
    typedef vctDynamicVectorRef<_elementType> VectorType;
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

#endif // _vctDataFunctionsDynamicVectorJSON_h
