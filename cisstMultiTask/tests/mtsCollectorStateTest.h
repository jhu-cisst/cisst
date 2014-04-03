/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2011-01-06

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cisstMultiTask/mtsConfig.h>


class mtsCollectorStateTest: public CppUnit::TestFixture
{
private:
    CPPUNIT_TEST_SUITE(mtsCollectorStateTest);
    {
        CPPUNIT_TEST(TestDevice_mtsInt);
        CPPUNIT_TEST(TestDevice_int);
        CPPUNIT_TEST(TestPeriodic_mtsInt);
        CPPUNIT_TEST(TestPeriodic_int);
        CPPUNIT_TEST(TestContinuous_mtsInt);
        CPPUNIT_TEST(TestContinuous_int);
        CPPUNIT_TEST(TestFromCallback_mtsInt);
        CPPUNIT_TEST(TestFromCallback_int);
        CPPUNIT_TEST(TestFromSignal_mtsInt);
        CPPUNIT_TEST(TestFromSignal_int);
    }
    CPPUNIT_TEST_SUITE_END();

public:
    mtsCollectorStateTest();

    void setUp(void);
    void tearDown(void);

    template <class _serverType>
    void TestExecution(_serverType * server, double serverExecutionDelay);

    template <class _elementType> void TestDevice(void);
    void TestDevice_mtsInt(void);
    void TestDevice_int(void);

    template <class _elementType> void TestPeriodic(void);
    void TestPeriodic_mtsInt(void);
    void TestPeriodic_int(void);

    template <class _elementType> void TestContinuous(void);
    void TestContinuous_mtsInt(void);
    void TestContinuous_int(void);

    template <class _elementType> void TestFromCallback(void);
    void TestFromCallback_mtsInt(void);
    void TestFromCallback_int(void);

    template <class _elementType> void TestFromSignal(void);
    void TestFromSignal_mtsInt(void);
    void TestFromSignal_int(void);
};
