/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2014-08-18

  (C) Copyright 2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "vctDataFunctionsTransformationsJSONTest.h"

#include <cisstVector/vctTransformationTypes.h>
#include <cisstVector/vctDataFunctionsTransformationsJSON.h>
#include <cisstVector/vctRandomFixedSizeVector.h>
#include <cisstVector/vctRandomTransformations.h>

#include <json/json.h>

void vctDataFunctionsTransformationsJSONTest::TestFrm3(void)
{
    typedef vctFrm3 DataType;
    DataType source, destination;
    vctRandom(source.Rotation());
    vctRandom(source.Translation(), 5.0, 10.0);
    Json::Value jsonValue;
    cmnDataJSON<DataType>::SerializeText(source, jsonValue["object"]);
    cmnDataJSON<DataType>::DeSerializeText(destination, jsonValue["object"]);
    CPPUNIT_ASSERT(source.AlmostEquivalent(destination));
}

void vctDataFunctionsTransformationsJSONTest::TestFrm4x4(void)
{
    typedef vctFrm4x4 DataType;
    DataType source, destination;
    vctMatRot3 rotation;
    vct3 translation;
    vctRandom(rotation);
    vctRandom(translation, 5.0, 10.0);
    source.Translation().Assign(translation);
    source.Rotation().Assign(rotation);
    Json::Value jsonValue;
    cmnDataJSON<DataType>::SerializeText(source, jsonValue["object"]);
    cmnDataJSON<DataType>::DeSerializeText(destination, jsonValue["object"]);
    CPPUNIT_ASSERT(source.AlmostEquivalent(destination));
}

CPPUNIT_TEST_SUITE_REGISTRATION(vctDataFunctionsTransformationsJSONTest);
