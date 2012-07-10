/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2012-07-09

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "vctDataFunctionsFixedSizeVectorTest.h"
#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctRandomFixedSizeVector.h>

void vctDataFunctionsFixedSizeVectorTest::TestDataCopy(void)
{
    vctFixedSizeVector<double, 7> source, destination;
    vctRandom(source, -1.0, 1.0);
    cmnDataCopy(destination, source);
    CPPUNIT_ASSERT(source.Equal(destination));
}


void vctDataFunctionsFixedSizeVectorTest::TestBinarySerializationStream(void)
{
    cmnDataFormat local, remote;
    std::stringstream stream;
    vctFixedSizeVector<double, 12> v1, v2, vReference;
    vctRandom(vReference, -10.0, 10.0);
    v1 = vReference;
    cmnDataSerializeBinary(stream, v1);
    v1.SetAll(0);
    cmnDataDeSerializeBinary(stream, v2, remote, local);
    CPPUNIT_ASSERT_EQUAL(vReference, v2);
}


void vctDataFunctionsFixedSizeVectorTest::TestScalar(void)
{
    vctFixedSizeVector<int, 6> vInt;
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), cmnDataScalarNumber(vInt));
    CPPUNIT_ASSERT_EQUAL(true, cmnDataScalarNumberIsFixed(vInt));

    vctFixedSizeVector<vctFixedSizeVector<double, 12>, 4> vvDouble;
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(48), cmnDataScalarNumber(vvDouble));
    CPPUNIT_ASSERT_EQUAL(true, cmnDataScalarNumberIsFixed(vvDouble));

    vctFixedSizeVector<std::string, 3> vString;
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), cmnDataScalarNumber(vString));
    CPPUNIT_ASSERT_EQUAL(true, cmnDataScalarNumberIsFixed(vString));

    size_t index;
    for (index = 0; index < cmnDataScalarNumber(vvDouble); ++index) {
        std::cerr << cmnDataScalarDescription(vvDouble, index) << std::endl;
    }
    // throws an exception
    std::cerr << cmnDataScalarDescription(vString, 3) << std::endl;
}


CPPUNIT_TEST_SUITE_REGISTRATION(vctDataFunctionsFixedSizeVectorTest);
