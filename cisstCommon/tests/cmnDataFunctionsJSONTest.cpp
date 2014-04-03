/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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


#include "cmnDataFunctionsJSONTest.h"

#include <cisstCommon/cmnDataFunctionsJSON.h>
#include <json/json.h>

void cmnDataFunctionsJSONTest::TestNativeTypes(void)
{
    Json::Value jsonValue;
    bool boolTrue, boolFalse, boolRead;
    boolTrue = true;
    boolFalse = false;
    int intZero, intNegative, intPositive, intRead;
    intZero = 0;
    intNegative = -123456;
    intPositive = 654321;
    double doubleZero, doubleNegative, doublePositive, doubleRead;
    doubleZero = 0.0;
    doubleNegative = -123.456;
    doublePositive = 654.321;
    std::string stringHello, stringRead;
    stringHello = "hello";
    cmnDataJSON<bool>::SerializeText(boolTrue, jsonValue["boolTrue"]);
    cmnDataJSON<bool>::SerializeText(boolFalse, jsonValue["boolFalse"]);
    cmnDataJSON<int>::SerializeText(intZero, jsonValue["intZero"]);
    cmnDataJSON<int>::SerializeText(intNegative, jsonValue["intNegative"]);
    cmnDataJSON<int>::SerializeText(intPositive, jsonValue["intPositive"]);
    cmnDataJSON<double>::SerializeText(doubleZero, jsonValue["doubleZero"]);
    cmnDataJSON<double>::SerializeText(doubleNegative, jsonValue["doubleNegative"]);
    cmnDataJSON<double>::SerializeText(doublePositive, jsonValue["doublePositive"]);
    cmnDataJSON<std::string>::SerializeText(stringHello, jsonValue["stringHello"]);
    boolRead = false;
    cmnDataJSON<bool>::DeSerializeText(boolRead, jsonValue["boolTrue"]);
    CPPUNIT_ASSERT_EQUAL(boolTrue, boolRead);
    cmnDataJSON<bool>::DeSerializeText(boolRead, jsonValue["boolFalse"]);
    CPPUNIT_ASSERT_EQUAL(boolFalse, boolRead);
    intRead = 1347893291;
    cmnDataJSON<int>::DeSerializeText(intRead, jsonValue["intZero"]);
    CPPUNIT_ASSERT_EQUAL(intZero, intRead);
    cmnDataJSON<int>::DeSerializeText(intRead, jsonValue["intNegative"]);
    CPPUNIT_ASSERT_EQUAL(intNegative, intRead);
    cmnDataJSON<int>::DeSerializeText(intRead, jsonValue["intPositive"]);
    CPPUNIT_ASSERT_EQUAL(intPositive, intRead);
    doubleRead = 1347893291.123;
    cmnDataJSON<double>::DeSerializeText(doubleRead, jsonValue["doubleZero"]);
    CPPUNIT_ASSERT_EQUAL(doubleZero, doubleRead);
    cmnDataJSON<double>::DeSerializeText(doubleRead, jsonValue["doubleNegative"]);
    CPPUNIT_ASSERT_EQUAL(doubleNegative, doubleRead);
    cmnDataJSON<double>::DeSerializeText(doubleRead, jsonValue["doublePositive"]);
    CPPUNIT_ASSERT_EQUAL(doublePositive, doubleRead);
    stringRead = "bye";
    cmnDataJSON<std::string>::DeSerializeText(stringRead, jsonValue["stringHello"]);
    CPPUNIT_ASSERT_EQUAL(stringHello, stringRead);
}


void cmnDataFunctionsJSONTest::TestStdVector(void)
{
    std::vector<std::string> vectorString, vectorRead;
    vectorString.push_back("hello");
    vectorString.push_back("world");
    vectorString.push_back("bonjour");
    vectorString.push_back("monde");
    Json::Value jsonValue;
    cmnDataJSON<std::vector<std::string> >::SerializeText(vectorString, jsonValue["strings"]);
    cmnDataJSON<std::vector<std::string> >::DeSerializeText(vectorRead, jsonValue["strings"]);
    CPPUNIT_ASSERT_EQUAL(vectorRead.size(), vectorString.size());
    for (size_t index = 0;
         index < vectorRead.size();
         ++index) {
        CPPUNIT_ASSERT_EQUAL(vectorRead[index], vectorString[index]);
    }
}
