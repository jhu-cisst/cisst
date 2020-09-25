/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2019-12-02

  (C) Copyright 2019-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstVector/vctTransformationTypes.h>

class vctEulerRotationTest: public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(vctEulerRotationTest);
    CPPUNIT_TEST(TestOrderZYX);
    CPPUNIT_TEST(TestOrderZXZ);
    CPPUNIT_TEST(TestOrderYZX);
    CPPUNIT_TEST(TestSingularitiesZXZ);
    CPPUNIT_TEST_SUITE_END();

 public:
    void setUp(void) {
    }

    void tearDown(void) {
    }

    /*! Test that rotation matrix created matches Rz * Ry * Rx */
    void TestOrderZYX(void);

    /*! Test that rotation matrix created matches Rz * Rx * Rz */
    void TestOrderZXZ(void);

    /*! Test that rotation matrix created matches Ry * Rz * Rx */
    void TestOrderYZX(void);

    /*! Test singularities */
    void TestSingularitiesZXZ(void);
};
