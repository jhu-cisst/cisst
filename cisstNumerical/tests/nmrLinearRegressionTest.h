/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Peter Kazanzides
  Created on: 2016-04-29
  
  (C) Copyright 2016 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _nmrLinearRegressionTest_h
#define _nmrLinearRegressionTest_h

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstNumerical/nmrLinearRegression.h>

class nmrLinearRegressionTest : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(nmrLinearRegressionTest);

    CPPUNIT_TEST(TestLinearRegression_vctDoubleVec);
    CPPUNIT_TEST(TestLinearRegression_vctIntVec);
    CPPUNIT_TEST(TestLinearRegression_StdVecDouble);
    CPPUNIT_TEST(TestLinearRegression_StdVecInt);

    CPPUNIT_TEST_SUITE_END();

public:

    void setUp()
    {}
    
    void tearDown()
    {}

    template <class _vectorType>
    void TestLinearRegression(void);

    template <class _vectorType>
    void TestMovingWindow(nmrLinearRegressionWindowSolver<typename _vectorType::value_type> *solver, _vectorType x, _vectorType y);

    void TestLinearRegression_vctDoubleVec(void);
    void TestLinearRegression_vctIntVec(void);
    void TestLinearRegression_StdVecDouble(void);
    void TestLinearRegression_StdVecInt(void);
};

#endif // _nmrLinearRegressionTest_h
