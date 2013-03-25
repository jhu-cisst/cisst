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

#include "vctDataFunctionsDynamicVectorJSONTest.h"

#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctDynamicMatrix.h>
#include <cisstVector/vctDataFunctionsDynamicVectorJSON.h>
#include <cisstVector/vctRandomDynamicVector.h>
#include <cisstVector/vctRandomDynamicMatrix.h>

#include <json/json.h>

void vctDataFunctionsDynamicVectorJSONTest::TestVector(void)
{
    vctDynamicVector<double> source, destination;
    source.SetSize(10);
    destination.SetSize(10);
    vctRandom(source, -1.0, 1.0);
    Json::Value jsonValue;
    cmnDataToJSON(source, jsonValue["object"]);
    cmnDataFromJSON(destination, jsonValue["object"]);
    CPPUNIT_ASSERT(source.Equal(destination));
}

void vctDataFunctionsDynamicVectorJSONTest::TestVectorRef(void)
{
    vctDynamicMatrix<int> source, destination;
    source.SetSize(10, 10);
    destination.SetSize(10, 10);
    source.SetAll(0);
    destination.SetAll(0);
    vctRandom(source, -10, 10);
    Json::Value jsonValue;
    cmnDataToJSON(source.Row(1), jsonValue["object"]);
    cmnDataFromJSON(destination.Column(1), jsonValue["object"]);
    CPPUNIT_ASSERT(source.Row(1).Equal(destination.Column(1)));
    destination.Column(1).SetAll(0);
    CPPUNIT_ASSERT(destination.Equal(0));
}

CPPUNIT_TEST_SUITE_REGISTRATION(vctDataFunctionsDynamicVectorJSONTest);
