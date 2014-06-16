/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2014-06-15

  (C) Copyright 2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "vctDataFunctionsFixedSizeMatrixJSONTest.h"

#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctDataFunctionsFixedSizeMatrixJSON.h>
#include <cisstVector/vctRandomFixedSizeMatrix.h>

#include <json/json.h>

void vctDataFunctionsFixedSizeMatrixJSONTest::TestMatrix(void)
{
    typedef vctFixedSizeMatrix<double, 7, 3> DataType;
    DataType source, destination;
    vctRandom(source, -1.0, 1.0);
    Json::Value jsonValue;
    cmnDataJSON<DataType>::SerializeText(source, jsonValue["object"]);
    cmnDataJSON<DataType>::DeSerializeText(destination, jsonValue["object"]);
    CPPUNIT_ASSERT(source.Equal(destination));
}

CPPUNIT_TEST_SUITE_REGISTRATION(vctDataFunctionsFixedSizeMatrixJSONTest);
