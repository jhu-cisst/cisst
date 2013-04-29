/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2011-06-27

  (C) Copyright 2011-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstCommon/cmnDataFunctionsJSON.h>
#include <cisstCommon/cmnAssert.h>

#if CISST_HAS_JSON

void cmnDataToJSON(const double value, Json::Value & jsonValue) {
    jsonValue = value;
}
void cmnDataFromJSON(double & placeHolder, const Json::Value & jsonValue) throw (std::runtime_error) {
    placeHolder = jsonValue.asDouble();
}

void cmnDataToJSON(const float value, Json::Value & jsonValue) {
    jsonValue = value;
}
void cmnDataFromJSON(float & placeHolder, const Json::Value & jsonValue) throw (std::runtime_error) {
    placeHolder = jsonValue.asFloat();
}

void cmnDataToJSON(const int value, Json::Value & jsonValue) {
    jsonValue = value;
}
void cmnDataFromJSON(int & placeHolder, const Json::Value & jsonValue) throw (std::runtime_error) {
    placeHolder = jsonValue.asInt();
}

void cmnDataToJSON(const unsigned int value, Json::Value & jsonValue) {
    jsonValue = value;
}
void cmnDataFromJSON(unsigned int & placeHolder, const Json::Value & jsonValue) throw (std::runtime_error) {
    placeHolder = jsonValue.asUInt();
}

void cmnDataToJSON(const bool value, Json::Value & jsonValue) {
    jsonValue = value;
}
void cmnDataFromJSON(bool & placeHolder, const Json::Value & jsonValue) throw (std::runtime_error) {
    placeHolder = jsonValue.asBool();
}

void cmnDataToJSON(const std::string value, Json::Value & jsonValue) {
    jsonValue = value;
}
void cmnDataFromJSON(std::string & placeHolder, const Json::Value & jsonValue) throw (std::runtime_error) {
    placeHolder = jsonValue.asString();
}

void cmnDataToJSON_size_t(const size_t value, Json::Value & jsonValue) {
    jsonValue = static_cast<Json::UInt64>(value);
}
void cmnDataFromJSON_size_t(size_t & placeHolder, const Json::Value & jsonValue) throw (std::runtime_error) {
    placeHolder = jsonValue.asUInt64();
}

#endif // CISST_HAS_JSON
