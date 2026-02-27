/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Brendan Burkhart
  Created on: 2026-02-26

  (C) Copyright 2026 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _nmrSavitzkyGolayTest_h
#define _nmrSavitzkyGolayTest_h

#include <cisstNumerical/nmrSavitzkyGolay.h>

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>


class nmrSavitzkyGolayTest: public CppUnit::TestFixture
{
public:
    CPPUNIT_TEST_SUITE(nmrSavitzkyGolayTest);
    {
        CPPUNIT_TEST(TestCausalSmoothing);
        CPPUNIT_TEST(TestNonCausalSmoothing);
        CPPUNIT_TEST(TestFirstDerivativeEstimation);
        CPPUNIT_TEST(TestThirdDerivativeEstimation);
    }
    CPPUNIT_TEST_SUITE_END();

public:
    inline void setUp() {};
    inline void tearDown() {};

    void TestCausalSmoothing();
    void TestNonCausalSmoothing();

    void TestFirstDerivativeEstimation();
    void TestThirdDerivativeEstimation();
};


#endif  // _nmrSavitzkyGolayTest_h

