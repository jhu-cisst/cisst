/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung, Anton Deguet
  Created on: 2009-11-17

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights
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

  Name of tests are Test{Local,Remote}<_clientType><_serverType>{,Blocking}.
*/
class mtsCommandAndEventTest: public CppUnit::TestFixture
{
private:
    CPPUNIT_TEST_SUITE(mtsCommandAndEventTest);
    {
        CPPUNIT_TEST(TestLocalDeviceDevice_mtsInt);
        CPPUNIT_TEST(TestLocalDeviceDevice_int);
        CPPUNIT_TEST(TestLocalPeriodicPeriodic_mtsInt);
        CPPUNIT_TEST(TestLocalPeriodicPeriodic_int);
        CPPUNIT_TEST(TestLocalContinuousContinuous_mtsInt);
        CPPUNIT_TEST(TestLocalContinuousContinuous_int);
        CPPUNIT_TEST(TestLocalFromCallbackFromCallback_mtsInt);
        CPPUNIT_TEST(TestLocalFromCallbackFromCallback_int);
        CPPUNIT_TEST(TestLocalFromSignalFromSignal_mtsInt);
        CPPUNIT_TEST(TestLocalFromSignalFromSignal_int);

        CPPUNIT_TEST(TestLocalPeriodicPeriodicBlocking_mtsInt);
        CPPUNIT_TEST(TestLocalPeriodicPeriodicBlocking_int);
        CPPUNIT_TEST(TestLocalContinuousContinuousBlocking_mtsInt);
        CPPUNIT_TEST(TestLocalContinuousContinuousBlocking_int);
        CPPUNIT_TEST(TestLocalFromCallbackFromCallbackBlocking_mtsInt);
        CPPUNIT_TEST(TestLocalFromCallbackFromCallbackBlocking_int);
        CPPUNIT_TEST(TestLocalFromSignalFromSignalBlocking_mtsInt);
        CPPUNIT_TEST(TestLocalFromSignalFromSignalBlocking_int);

        CPPUNIT_TEST(TestArgumentPrototypes_mtsInt);
        CPPUNIT_TEST(TestArgumentPrototypes_int);

#if CISST_MTS_HAS_ICE
        //        CPPUNIT_TEST(TestRemoteDeviceDevice);
#endif
    }
    CPPUNIT_TEST_SUITE_END();

public:
    mtsCommandAndEventTest();

    void setUp(void);
    void tearDown(void);

    template <class _clientType, class _serverType>
    void TestExecution(_clientType * client, _serverType * server,
                       double clientExecutionDelay, double serverExecutionDelay,
                       double blockingDelay = 0.0);

    template <class _elementType> void TestLocalDeviceDevice(void);
    void TestLocalDeviceDevice_mtsInt(void);
    void TestLocalDeviceDevice_int(void);

    template <class _elementType> void TestLocalPeriodicPeriodic(void);
    void TestLocalPeriodicPeriodic_mtsInt(void);
    void TestLocalPeriodicPeriodic_int(void);

    template <class _elementType> void TestLocalContinuousContinuous(void);
    void TestLocalContinuousContinuous_mtsInt(void);
    void TestLocalContinuousContinuous_int(void);

    template <class _elementType> void TestLocalFromCallbackFromCallback(void);
    void TestLocalFromCallbackFromCallback_mtsInt(void);
    void TestLocalFromCallbackFromCallback_int(void);

    template <class _elementType> void TestLocalFromSignalFromSignal(void);
    void TestLocalFromSignalFromSignal_mtsInt(void);
    void TestLocalFromSignalFromSignal_int(void);

    template <class _elementType> void TestLocalPeriodicPeriodicBlocking(void);
    void TestLocalPeriodicPeriodicBlocking_mtsInt(void);
    void TestLocalPeriodicPeriodicBlocking_int(void);

    template <class _elementType> void TestLocalContinuousContinuousBlocking(void);
    void TestLocalContinuousContinuousBlocking_mtsInt(void);
    void TestLocalContinuousContinuousBlocking_int(void);

    template <class _elementType> void TestLocalFromCallbackFromCallbackBlocking(void);
    void TestLocalFromCallbackFromCallbackBlocking_mtsInt(void);
    void TestLocalFromCallbackFromCallbackBlocking_int(void);

    template <class _elementType> void TestLocalFromSignalFromSignalBlocking(void);
    void TestLocalFromSignalFromSignalBlocking_mtsInt(void);
    void TestLocalFromSignalFromSignalBlocking_int(void);

    template <class _elementType> void TestArgumentPrototypes(void);
    void TestArgumentPrototypes_mtsInt(void);
    void TestArgumentPrototypes_int(void);

#if CISST_MTS_HAS_ICE
    void TestRemoteDeviceDevice(void);
#endif
};
