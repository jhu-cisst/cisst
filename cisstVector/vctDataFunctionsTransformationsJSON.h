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

#ifndef _vctDataFunctionsTransformationsJSON_h
#define _vctDataFunctionsTransformationsJSON_h

#include <cisstVector/vctDataFunctionsFixedSizeVectorJSON.h>
#include <cisstVector/vctDataFunctionsFixedSizeMatrixJSON.h>

#if CISST_HAS_JSON
template <class _rotationType>
class cmnDataJSON<vctFrameBase<_rotationType> >
{
public:
    typedef vctFrameBase<_rotationType> DataType;
    typedef typename DataType::TranslationType TranslationType;
    typedef typename DataType::RotationType RotationType;

    static void SerializeText(const DataType & data, Json::Value & jsonValue)
    {
        cmnDataJSON<TranslationType>::SerializeText(data.Translation(), jsonValue["Translation"]);
        cmnDataJSON<RotationType>::SerializeText(data.Rotation(), jsonValue["Rotation"]);
    }

    static void DeSerializeText(DataType & data, const Json::Value & jsonValue)
        CISST_THROW(std::runtime_error)
    {
        cmnDataJSON<TranslationType>::DeSerializeText(data.Translation(), jsonValue["Translation"]);
        cmnDataJSON<RotationType>::DeSerializeText(data.Rotation(), jsonValue["Rotation"]);
        data.Rotation().NormalizedSelf();
    }
};

// pass through class for rotation matrix
template <class _elementType, bool _rowMajor>
class cmnDataJSON<vctMatrixRotation3<_elementType, _rowMajor> >
{
 public:
    typedef vctMatrixRotation3<_elementType, _rowMajor> DataType;
    typedef typename DataType::ContainerType ContainerType;

    static void SerializeText(const DataType & data, Json::Value & jsonValue)
    {
        cmnDataJSON<ContainerType>::SerializeText(data, jsonValue);
    }

    static void DeSerializeText(DataType & data, const Json::Value & jsonValue)
    {
        cmnDataJSON<ContainerType>::DeSerializeText(data, jsonValue);
        data.NormalizedSelf();
    }
};

// pass through class for frame4x4
template <class _elementType, bool _rowMajor>
class cmnDataJSON<vctFrame4x4<_elementType, _rowMajor> >
{
 public:
    typedef vctFrame4x4<_elementType, _rowMajor> DataType;
    typedef typename DataType::ContainerType ContainerType;

    static void SerializeText(const DataType & data, Json::Value & jsonValue)
    {
        cmnDataJSON<ContainerType>::SerializeText(data, jsonValue);
    }

    static void DeSerializeText(DataType & data, const Json::Value & jsonValue)
    {
        cmnDataJSON<ContainerType>::DeSerializeText(data, jsonValue);
        data.Rotation().NormalizedSelf();
    }
};

#endif // CISST_HAS_JSON

#endif // _vctDataFunctionsTransformationsJSON_h
