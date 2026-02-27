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

#include "nmrSavitzkyGolayTest.h"
#include "cisstNumerical/nmrSavitzkyGolay.h"
#include <cppunit/TestAssert.h>

void nmrSavitzkyGolayTest::TestCausalSmoothing()
{
    Eigen::VectorXd coef_2_4_0 = nmrSavitzkyGolay(2, 0, 4, 0);
    Eigen::VectorXd correct_2_4_0(5);
    correct_2_4_0 << 0.0857, -0.1429, -0.0857, 0.2571, 0.8857;
    CPPUNIT_ASSERT_EQUAL((Eigen::Index)5, coef_2_4_0.size());

    for (size_t idx = 0; idx < (size_t)coef_2_4_0.size(); idx++) {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(correct_2_4_0[idx], coef_2_4_0[idx], 0.0001);
    }

    Eigen::VectorXd coef_4_6_0 = nmrSavitzkyGolay(4, 0, 6, 0);
    Eigen::VectorXd correct_4_6_0(7);
    correct_4_6_0 << 0.0108, -0.0411, 0.0433, 0.0216, -0.0758, 0.0541, 0.9870;
    CPPUNIT_ASSERT_EQUAL((Eigen::Index)7, coef_4_6_0.size());

    for (size_t idx = 0; idx < (size_t)coef_4_6_0.size(); idx++) {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(correct_4_6_0[idx], coef_4_6_0[idx], 0.0001);
    }
}

void nmrSavitzkyGolayTest::TestNonCausalSmoothing()
{
    Eigen::VectorXd coef_2_2_2 = nmrSavitzkyGolay(2, 0, 2, 2);
    Eigen::VectorXd correct_2_2_2(5);
    correct_2_2_2 << -0.0857, 0.3429, 0.4857, 0.3429, -0.0857;
    CPPUNIT_ASSERT_EQUAL((Eigen::Index)5, coef_2_2_2.size());

    for (size_t idx = 0; idx < (size_t)coef_2_2_2.size(); idx++) {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(correct_2_2_2[idx], coef_2_2_2[idx], 0.0001);
    }

    Eigen::VectorXd coef_3_2_2 = nmrSavitzkyGolay(3, 0, 2, 2);
    Eigen::VectorXd correct_3_2_2(5);
    correct_3_2_2 << -0.0857, 0.3429, 0.4857, 0.3429, -0.0857;
    CPPUNIT_ASSERT_EQUAL((Eigen::Index)5, coef_3_2_2.size());

    for (size_t idx = 0; idx < (size_t)correct_3_2_2.size(); idx++) {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(correct_3_2_2[idx], coef_3_2_2[idx], 0.0001);
    }

    Eigen::VectorXd coef_3_3_3 = nmrSavitzkyGolay(3, 0, 3, 3);
    Eigen::VectorXd correct_3_3_3(7);
    correct_3_3_3 << -0.0952, 0.1429, 0.2857, 0.3333, 0.2857, 0.1429, -0.0952;

    CPPUNIT_ASSERT_EQUAL((Eigen::Index)7, coef_3_3_3.size());

    for (size_t idx = 0; idx < (size_t)coef_3_3_3.size(); idx++) {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(correct_3_3_3[idx], coef_3_3_3[idx], 0.0001);
    }
}

void nmrSavitzkyGolayTest::TestFirstDerivativeEstimation()
{
    Eigen::VectorXd derivative_2_2_2 = nmrSavitzkyGolay(2, 1, 2, 2);
    Eigen::VectorXd correct_2_2_2(5);
    correct_2_2_2 << -0.2000, -0.1000, 0.0000, 0.1000, 0.2000;

    CPPUNIT_ASSERT_EQUAL((Eigen::Index)5, derivative_2_2_2.size());

    for (size_t idx = 0; idx < (size_t)derivative_2_2_2.size(); idx++) {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(correct_2_2_2[idx], derivative_2_2_2[idx], 0.0001);
    }

    Eigen::VectorXd derivative_3_2_2 = nmrSavitzkyGolay(3, 1, 2, 2);
    Eigen::VectorXd correct_3_2_2(5);
    correct_3_2_2 << 0.0833, -0.6667, 0.0000, 0.6667, -0.0833;

    CPPUNIT_ASSERT_EQUAL((Eigen::Index)5, derivative_3_2_2.size());

    for (size_t idx = 0; idx < (size_t)derivative_3_2_2.size(); idx++) {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(correct_3_2_2[idx], derivative_3_2_2[idx], 0.0001);
    }
}

void nmrSavitzkyGolayTest::TestThirdDerivativeEstimation()
{
    Eigen::VectorXd derivative_2_2_2 = nmrSavitzkyGolay(2, 3, 2, 2);
    Eigen::VectorXd correct_2_2_2(5);
    correct_2_2_2 << 0.0, 0.0, 0.0, 0.0, 0.0;

    CPPUNIT_ASSERT_EQUAL((Eigen::Index)5, derivative_2_2_2.size());

    for (size_t idx = 0; idx < (size_t)derivative_2_2_2.size(); idx++) {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(correct_2_2_2[idx], derivative_2_2_2[idx], 0.0001);
    }

    Eigen::VectorXd derivative_3_2_2 = nmrSavitzkyGolay(3, 3, 2, 2);
    Eigen::VectorXd correct_3_2_2(5);
    correct_3_2_2 << -0.5000, 1.0000, 0.0000, -1.0000, 0.5000;

    CPPUNIT_ASSERT_EQUAL((Eigen::Index)5, derivative_3_2_2.size());

    for (size_t idx = 0; idx < (size_t)derivative_3_2_2.size(); idx++) {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(correct_3_2_2[idx], derivative_3_2_2[idx], 0.0001);
    }

    Eigen::VectorXd derivative_4_3_3 = nmrSavitzkyGolay(4, 3, 3, 3);
    Eigen::VectorXd correct_4_3_3 (7);
    correct_4_3_3  << -0.1667, 0.1667, 0.1667, -0.0000, -0.1667, -0.1667, 0.1667;

    CPPUNIT_ASSERT_EQUAL((Eigen::Index)7, derivative_4_3_3 .size());

    for (size_t idx = 0; idx < (size_t)derivative_4_3_3 .size(); idx++) {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(correct_4_3_3[idx], derivative_4_3_3[idx], 0.0001);
    }
}

CPPUNIT_TEST_SUITE_REGISTRATION(nmrSavitzkyGolayTest);
