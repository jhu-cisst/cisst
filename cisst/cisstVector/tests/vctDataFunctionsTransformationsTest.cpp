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

#include "vctDataFunctionsTransformationsTest.h"

#include <cisstVector/vctTransformationTypes.h>
#include <cisstVector/vctDataFunctionsTransformations.h>
#include <cisstVector/vctRandomFixedSizeVector.h>
#include <cisstVector/vctRandomTransformations.h>

void vctDataFunctionsTransformationsTest::TestFrm3DataCopy(void)
{
    vctFrm3 source, destination;
    vctRandom(source.Rotation());
    vctRandom(source.Translation(), -10.0, 10.0);
    cmnDataCopy(destination, source);
    CPPUNIT_ASSERT(source.Equal(destination));
}


void vctDataFunctionsTransformationsTest::TestFrm3BinarySerializationStream(void)
{
    cmnDataFormat local, remote;
    std::stringstream stream;
    vctFrm3 f1, f2, fReference;
    vctRandom(fReference.Rotation());
    vctRandom(fReference.Translation(), -10.0, 10.0);
    f1 = fReference;
    cmnDataSerializeBinary(stream, f1);
    f1.From(f1.Identity());
    cmnDataDeSerializeBinary(stream, f2, remote, local);
    CPPUNIT_ASSERT_EQUAL(fReference, f2);
}


void vctDataFunctionsTransformationsTest::TestFrm3Scalar(void)
{
    vctFrm3 f;
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), cmnDataScalarNumber(f));
    CPPUNIT_ASSERT_EQUAL(true, cmnDataScalarNumberIsFixed(f));

    size_t index;
    bool exceptionReceived = false;
    std::string description;

    // exception expected if index too high
    try {
        description = cmnDataScalarDescription(f, cmnDataScalarNumber(f) + 1);
    } catch (std::out_of_range) {
        exceptionReceived = true;
    }
    CPPUNIT_ASSERT(exceptionReceived);

    // get scalar
    for (index = 0; index < cmnDataScalarNumber(f); ++index) {
        // all should be null, except for rotation diagonal, index = 3, 7 and 11
        if ((index == 3) || (index == 7) || (index == 11)) {
            CPPUNIT_ASSERT_EQUAL(1.0, cmnDataScalar(f, index));
        } else {
            CPPUNIT_ASSERT_EQUAL(0.0, cmnDataScalar(f, index));
        }
    }
}


CPPUNIT_TEST_SUITE_REGISTRATION(vctDataFunctionsTransformationsTest);
