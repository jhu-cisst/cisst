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

#ifndef _vctDataFunctionsFixedSizeVectorJSON_h
#define _vctDataFunctionsFixedSizeVectorJSON_h

#include <cisstCommon/cmnDataFunctionsJSON.h>
#include <cisstVector/vctFixedSizeVectorBase.h>

#if CISST_HAS_JSON
template <class _elementType, vct::size_type _size>
class cmnDataJSON<vctFixedSizeVector<_elementType, _size> >
{
public:
    typedef vctFixedSizeVector<_elementType, _size> DataType;

    static void SerializeText(const DataType & vector, Json::Value & jsonValue)
    {
        typedef typename DataType::const_iterator const_iterator;
        const const_iterator end = vector.end();
        const_iterator iter;
        int index = 0;
        for (iter = vector.begin();
             iter != end;
             ++index, ++iter) {
            cmnDataJSON<_elementType>::SerializeText(*iter, jsonValue[index]);
        }
    }

    static void DeSerializeText(DataType & vector, const Json::Value & jsonValue)
        CISST_THROW(std::runtime_error)
    {
        // make sure both vectors have the same size
        if (vector.size() != jsonValue.size()) {
            cmnThrow("cmnDataJSON<vctFixedSizeVector>::DeSerializeText: vector sizes don't match");
        }
        typedef typename DataType::iterator iterator;
        const iterator end = vector.end();
        iterator iter;
        int index = 0;
        for (iter = vector.begin();
             iter != end;
             ++index, ++iter) {
            cmnDataJSON<_elementType>::DeSerializeText(*iter, jsonValue[index]);
        }
    }
};

#endif // CISST_HAS_JSON

#endif // _vctDataFunctionsFixedSizeVectorJSON_h
