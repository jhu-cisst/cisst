/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Paul Wilkening
  Created on: 2014

  (C) Copyright 2014 Johns Hopkins University (JHU), All Rights Reserved.

 --- begin cisst license - do not edit ---

 This software is provided "as is" under an open source license, with
 no warranty.  The complete license can be found in license.txt and
 http://www.cisst.org/cisst/license.txt.

 --- end cisst license ---
 */

#ifndef _nmrConstraintOptimizerTest_h
#define _nmrConstraintOptimizerTest_h

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cisstNumerical/nmrConstraintOptimizer.h>

class nmrConstraintOptimizerTest: public CppUnit::TestCase
{
 public:

    CPPUNIT_TEST_SUITE(nmrConstraintOptimizerTest);
    {
        CPPUNIT_TEST(TestConstructor);
        CPPUNIT_TEST(TestAllocate);
        CPPUNIT_TEST(TestSetRefs);
        CPPUNIT_TEST(TestSolve);
    }
    CPPUNIT_TEST_SUITE_END();

 public:

    void setUp()
    {}

    void tearDown()
    {}

    /*! Test constructor. */
    void TestConstructor(void);

    /*! Test allocate commands */
    void TestAllocate(void);

    /*! Test SetRefs command */
    void TestSetRefs(void);

    /*! Test Solve */
    void TestSolve(void);
};

CPPUNIT_TEST_SUITE_REGISTRATION(nmrConstraintOptimizerTest);

#endif // _nmrConstraintOptimizerTest_h
