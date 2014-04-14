/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2011-06-27

  (C) Copyright 2011-2013 Johns Hopkins University (JHU), All Rights Reserved.

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

void CISST_EXPORT cmnDataToJSON(const double value, Json::Value & jsonValue);
void CISST_EXPORT cmnDataFromJSON(double & placeHolder, const Json::Value & jsonValue) throw (std::runtime_error);

void CISST_EXPORT cmnDataToJSON(const float value, Json::Value & jsonValue);
void CISST_EXPORT cmnDataFromJSON(float & placeHolder, const Json::Value & jsonValue) throw (std::runtime_error);

void CISST_EXPORT cmnDataToJSON(const int value, Json::Value & jsonValue);
void CISST_EXPORT cmnDataFromJSON(int & placeHolder, const Json::Value & jsonValue) throw (std::runtime_error);

void CISST_EXPORT cmnDataToJSON(const unsigned int value, Json::Value & jsonValue);
void CISST_EXPORT cmnDataFromJSON(unsigned int & placeHolder, const Json::Value & jsonValue) throw (std::runtime_error);

void CISST_EXPORT cmnDataToJSON(const bool value, Json::Value & jsonValue);
void CISST_EXPORT cmnDataFromJSON(bool & placeHolder, const Json::Value & jsonValue) throw (std::runtime_error);

void CISST_EXPORT cmnDataToJSON(const std::string value, Json::Value & jsonValue);
void CISST_EXPORT cmnDataFromJSON(std::string & placeHolder, const Json::Value & jsonValue) throw (std::runtime_error);

void CISST_EXPORT cmnDataToJSON_size_t(const size_t value, Json::Value & jsonValue);
void CISST_EXPORT cmnDataFromJSON_size_t(size_t & placeHolder, const Json::Value & jsonValue) throw (std::runtime_error);

template <class _elementType>
void cmnDataToJSON(const std::vector<_elementType> & vector, Json::Value & jsonValue) {
    typedef std::vector<_elementType> VectorType;
    typedef typename VectorType::const_iterator const_iterator;
    const const_iterator end = vector.end();
    int jsonIndex = 0;
    for (const_iterator iter = vector.begin();
         iter != end;
         ++iter, ++jsonIndex) {
        cmnDataToJSON(*iter, jsonValue[jsonIndex]);
    }
}

template <class _elementType>
void cmnDataFromJSON(std::vector<_elementType> & vector, const Json::Value & jsonValue) throw (std::runtime_error) {
    // get the vector size from JSON and resize
    vector.resize(jsonValue.size());
    typedef std::vector<_elementType> VectorType;
    typedef typename VectorType::iterator iterator;
    const iterator end = vector.end();
    int jsonIndex = 0;
    for (iterator iter = vector.begin();
         iter != end;
         ++iter, ++jsonIndex) {
        cmnDataFromJSON(*iter, jsonValue[jsonIndex]);
    }
}

#endif // CISST_HAS_JSON

#endif // _cmnDataFunctionsJSON_h
