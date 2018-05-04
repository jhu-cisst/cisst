/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2011-06-27

  (C) Copyright 2011-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#pragma once
#ifndef _cmnDataFunctionsJSON_h
#define _cmnDataFunctionsJSON_h

#include <string.h> // for memcpy
#include <iostream>
#include <limits>
#include <vector>
#include <cisstConfig.h> // for CISST_HAS_JSON
#include <cisstCommon/cmnThrow.h>

// always include last
#include <cisstCommon/cmnExport.h>

#if CISST_HAS_JSON

#include <json/json.h>

template <typename _elementType>
class cmnDataJSON
{
public:
    typedef _elementType DataType;
    static void SerializeText(const DataType & data, Json::Value & jsonValue);
    static void DeSerializeText(DataType & data, const Json::Value & jsonValue) CISST_THROW(std::runtime_error);
};

template <>
void CISST_EXPORT cmnDataJSON<double>::SerializeText(const DataType & data, Json::Value & jsonValue);
template <>
void CISST_EXPORT cmnDataJSON<double>::DeSerializeText(DataType & data, const Json::Value & jsonValue) CISST_THROW(std::runtime_error);

template <>
void CISST_EXPORT cmnDataJSON<float>::SerializeText(const DataType & data, Json::Value & jsonValue);
template <>
void CISST_EXPORT cmnDataJSON<float>::DeSerializeText(DataType & data, const Json::Value & jsonValue) CISST_THROW(std::runtime_error);

template <>
void CISST_EXPORT cmnDataJSON<char>::SerializeText(const DataType & data, Json::Value & jsonValue);
template <>
void CISST_EXPORT cmnDataJSON<char>::DeSerializeText(DataType & data, const Json::Value & jsonValue) CISST_THROW(std::runtime_error);

template <>
void CISST_EXPORT cmnDataJSON<int>::SerializeText(const DataType & data, Json::Value & jsonValue);
template <>
void CISST_EXPORT cmnDataJSON<int>::DeSerializeText(DataType & data, const Json::Value & jsonValue) CISST_THROW(std::runtime_error);

template <>
void CISST_EXPORT cmnDataJSON<unsigned int>::SerializeText(const DataType & data, Json::Value & jsonValue);
template <>
void CISST_EXPORT cmnDataJSON<unsigned int>::DeSerializeText(DataType & data, const Json::Value & jsonValue) CISST_THROW(std::runtime_error);

template <>
void CISST_EXPORT cmnDataJSON<unsigned long int>::SerializeText(const DataType & data, Json::Value & jsonValue);
template <>
void CISST_EXPORT cmnDataJSON<unsigned long int>::DeSerializeText(DataType & data, const Json::Value & jsonValue) CISST_THROW(std::runtime_error);

template <>
void CISST_EXPORT cmnDataJSON<unsigned long long int>::SerializeText(const DataType & data, Json::Value & jsonValue);
template <>
void CISST_EXPORT cmnDataJSON<unsigned long long int>::DeSerializeText(DataType & data, const Json::Value & jsonValue) CISST_THROW(std::runtime_error);

template <>
void CISST_EXPORT cmnDataJSON<bool>::SerializeText(const DataType & data, Json::Value & jsonValue);
template <>
void CISST_EXPORT cmnDataJSON<bool>::DeSerializeText(DataType & data, const Json::Value & jsonValue) CISST_THROW(std::runtime_error);

template <>
void CISST_EXPORT cmnDataJSON<std::string>::SerializeText(const DataType & data, Json::Value & jsonValue);
template <>
void CISST_EXPORT cmnDataJSON<std::string>::DeSerializeText(DataType & data, const Json::Value & jsonValue) CISST_THROW(std::runtime_error);


template <class _elementType>
class cmnDataJSON<std::vector<_elementType> >
{
public:
    typedef std::vector<_elementType> DataType;

    static void SerializeText(const DataType & data, Json::Value & jsonValue)
    {
        typedef typename DataType::const_iterator const_iterator;
        const const_iterator end = data.end();
        int jsonIndex = 0;
        for (const_iterator iter = data.begin();
             iter != end;
             ++iter, ++jsonIndex) {
            cmnDataJSON<_elementType>::SerializeText(*iter, jsonValue[jsonIndex]);
        }
    }

    static void DeSerializeText(std::vector<_elementType> & data, const Json::Value & jsonValue) CISST_THROW(std::runtime_error)
    {
        // get the vector size from JSON and resize
        data.resize(jsonValue.size());
        typedef typename DataType::iterator iterator;
        const iterator end = data.end();
        int jsonIndex = 0;
        for (iterator iter = data.begin();
             iter != end;
             ++iter, ++jsonIndex) {
            cmnDataJSON<_elementType>::DeSerializeText(*iter, jsonValue[jsonIndex]);
        }
    }
};


template <class _elementType, int _size>
class cmnDataJSON<_elementType[_size] >
{
public:
    typedef _elementType * pointer;
    typedef const _elementType * const_pointer;

    static void SerializeText(const_pointer data, Json::Value & jsonValue)
    {
        const_pointer ptr = data;
        for (int index = 0; index < _size; ++index, ++ptr) {
            cmnDataJSON<_elementType>::SerializeText(*ptr, jsonValue[index]);
        }
    }

    static void DeSerializeText(pointer data, const Json::Value & jsonValue) CISST_THROW(std::runtime_error)
    {
        if (jsonValue.size() != _size) {
            cmnThrow("cmnDataJSON<c-array>::DeSerializeText: vector sizes don't match");
        }
        pointer ptr = data;
        for (int index = 0; index < _size; ++index, ++ptr) {
            cmnDataJSON<_elementType>::DeSerializeText(*ptr, jsonValue[index]);
        }
    }
};

#endif // CISST_HAS_JSON

#endif // _cmnDataFunctionsJSON_h
