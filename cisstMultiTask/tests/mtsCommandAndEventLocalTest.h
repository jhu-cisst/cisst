/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung, Anton Deguet
  Created on: 2009-11-17

  (C) Copyright 2009-2011 Johns Hopkins University (JHU), All Rights
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

/*
  Test that commands and events are executed for different types of
  components using a reasonable execution delay and blocking or not
  blocking commands.  These tests rely on the components defined in
  mtsTestComponents.h

  Name of tests are Test<_clientType><_serverType>{,Blocking}.
*/
class mtsCommandAndEventLocalTest: public CppUnit::TestFixture
{
private:
    CPPUNIT_TEST_SUITE(mtsCommandAndEventLocalTest);
    {
        CPPUNIT_TEST(TestDeviceDevice_mtsInt);
        CPPUNIT_TEST(TestDeviceDevice_int);
        CPPUNIT_TEST(TestPeriodicPeriodic_mtsInt);
        CPPUNIT_TEST(TestPeriodicPeriodic_int);
        CPPUNIT_TEST(TestContinuousContinuous_mtsInt);
        CPPUNIT_TEST(TestContinuousContinuous_int);
        CPPUNIT_TEST(TestFromCallbackFromCallback_mtsInt);
        CPPUNIT_TEST(TestFromCallbackFromCallback_int);
        CPPUNIT_TEST(TestFromSignalFromSignal_mtsInt);
        CPPUNIT_TEST(TestFromSignalFromSignal_int);

        CPPUNIT_TEST(TestPeriodicPeriodicBlocking_mtsInt);
        CPPUNIT_TEST(TestPeriodicPeriodicBlocking_int);
        CPPUNIT_TEST(TestContinuousContinuousBlocking_mtsInt);
        CPPUNIT_TEST(TestContinuousContinuousBlocking_int);
        CPPUNIT_TEST(TestFromCallbackFromCallbackBlocking_mtsInt);
        CPPUNIT_TEST(TestFromCallbackFromCallbackBlocking_int);
        CPPUNIT_TEST(TestFromSignalFromSignalBlocking_mtsInt);
        CPPUNIT_TEST(TestFromSignalFromSignalBlocking_int);

        CPPUNIT_TEST(TestArgumentPrototypes_mtsInt);
        CPPUNIT_TEST(TestArgumentPrototypes_int);
    }
    CPPUNIT_TEST_SUITE_END();

public:
    mtsCommandAndEventLocalTest();

    void setUp(void);
    void tearDown(void);

    template <class _clientType, class _serverType>
    void TestExecution(_clientType * client, _serverType * server,
                       double clientExecutionDelay, double serverExecutionDelay,
                       double blockingDelay = 0.0);

    template <class _elementType> void TestDeviceDevice(void);
    void TestDeviceDevice_mtsInt(void);
    void TestDeviceDevice_int(void);

    template <class _elementType> void TestPeriodicPeriodic(void);
    void TestPeriodicPeriodic_mtsInt(void);
    void TestPeriodicPeriodic_int(void);

    template <class _elementType> void TestContinuousContinuous(void);
    void TestContinuousContinuous_mtsInt(void);
    void TestContinuousContinuous_int(void);

    template <class _elementType> void TestFromCallbackFromCallback(void);
    void TestFromCallbackFromCallback_mtsInt(void);
    void TestFromCallbackFromCallback_int(void);

    template <class _elementType> void TestFromSignalFromSignal(void);
    void TestFromSignalFromSignal_mtsInt(void);
    void TestFromSignalFromSignal_int(void);

    template <class _elementType> void TestPeriodicPeriodicBlocking(void);
    void TestPeriodicPeriodicBlocking_mtsInt(void);
    void TestPeriodicPeriodicBlocking_int(void);

    template <class _elementType> void TestContinuousContinuousBlocking(void);
    void TestContinuousContinuousBlocking_mtsInt(void);
    void TestContinuousContinuousBlocking_int(void);

    template <class _elementType> void TestFromCallbackFromCallbackBlocking(void);
    void TestFromCallbackFromCallbackBlocking_mtsInt(void);
    void TestFromCallbackFromCallbackBlocking_int(void);

    template <class _elementType> void TestFromSignalFromSignalBlocking(void);
    void TestFromSignalFromSignalBlocking_mtsInt(void);
    void TestFromSignalFromSignalBlocking_int(void);

    template <class _elementType> void TestArgumentPrototypes(void);
    void TestArgumentPrototypes_mtsInt(void);
    void TestArgumentPrototypes_int(void);
};
