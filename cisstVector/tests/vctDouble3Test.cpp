/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2003-09-12
  
  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "vctDouble3Test.h"


void vctDouble3Test::TestXYZ(void) {

    vctDouble3 testVector(1.0, 4.0, 9.5);

    CPPUNIT_ASSERT(testVector[0] == 1.0);
    CPPUNIT_ASSERT(testVector[1] == 4.0);
    CPPUNIT_ASSERT(testVector[2] == 9.5);

    CPPUNIT_ASSERT(testVector.X() == testVector[0]);
    CPPUNIT_ASSERT(testVector.Y() == testVector[1]);
    CPPUNIT_ASSERT(testVector.Z() == testVector[2]);

    testVector.X() = 1.0;
    testVector.Y() = 1.0;
    testVector.Z() = 1.0;

    CPPUNIT_ASSERT(testVector == 1.0);
}


void vctDouble3Test::TestCrossProduct(void) {

    vctDouble3 testVector1(1.0, 4.0, 9.5);
    vctDouble3 testVector2(2.0, 5.0, 1.1);
    vctDouble3 testVector3;

    double tolerance = 1.0e-6;
    double scalar;

    testVector3.CrossProductOf(testVector1, testVector2);

    scalar = testVector1.DotProduct(testVector3);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, scalar, tolerance);
 
    scalar = testVector2.DotProduct(testVector3);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, scalar, tolerance);
}


CPPUNIT_TEST_SUITE_REGISTRATION(vctDouble3Test);

