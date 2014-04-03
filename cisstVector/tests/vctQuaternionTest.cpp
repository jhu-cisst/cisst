/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2004-01-09
  
  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "vctQuaternionTest.h"
#include <cisstVector/vctFixedSizeVector.h>

template<class _elementType>
void vctQuaternionTest::TestConstructors(void) {

    vctQuaternion<_elementType> testQuaternion(_elementType(1.0),
                                               _elementType(4.0),
                                               _elementType(9.5),
                                               _elementType(-1.3));
    
    CPPUNIT_ASSERT(testQuaternion[0] == _elementType(1.0));
    CPPUNIT_ASSERT(testQuaternion[1] == _elementType(4.0));
    CPPUNIT_ASSERT(testQuaternion[2] == _elementType(9.5));
    CPPUNIT_ASSERT(testQuaternion[3] == _elementType(-1.3));
    
    CPPUNIT_ASSERT(testQuaternion.X() == testQuaternion[0]);
    CPPUNIT_ASSERT(testQuaternion.Y() == testQuaternion[1]);
    CPPUNIT_ASSERT(testQuaternion.Z() == testQuaternion[2]);
    CPPUNIT_ASSERT(testQuaternion.R() == testQuaternion[3]);

    testQuaternion.X() = _elementType(1.0);
    testQuaternion.Y() = _elementType(1.0);
    testQuaternion.Z() = _elementType(1.0);
    testQuaternion.R() = _elementType(1.0);

    CPPUNIT_ASSERT(testQuaternion == _elementType(1.0));
}

void vctQuaternionTest::TestConstructorsDouble(void) {
    TestConstructors<double>();
}

void vctQuaternionTest::TestConstructorsFloat(void) {
    TestConstructors<float>();
}



CPPUNIT_TEST_SUITE_REGISTRATION(vctQuaternionTest);

