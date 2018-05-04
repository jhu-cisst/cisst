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

#ifndef _vctDataFunctionsDynamicVectorJSON_h
#define _vctDataFunctionsDynamicVectorJSON_h

#include <cisstCommon/cmnDataFunctionsJSON.h>
#include <cisstVector/vctDynamicVectorBase.h>

#if CISST_HAS_JSON
template <class _elementType>
class cmnDataJSON<vctDynamicVector<_elementType> >
{
public:
    typedef vctDynamicVector<_elementType> DataType;

    static void SerializeText(const DataType & vector,
                              Json::Value & jsonValue) {
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

    static void DeSerializeText(DataType & vector,
                                const Json::Value & jsonValue)
        CISST_THROW(std::runtime_error)
    {
        // get the vector size from JSON and resize
        vector.SetSize(jsonValue.size());
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

#endif // _vctDataFunctionsDynamicVectorJSON_h
