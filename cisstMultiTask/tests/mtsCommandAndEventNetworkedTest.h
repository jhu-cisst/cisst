/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2010-10-20

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnUnits.h>
#include <cisstOSAbstraction/osaPipeExec.h>

/*
  Test that commands and events are executed for different types of
  components using a reasonable execution delay and blocking or not
  blocking commands.  These tests rely on the components defined in
  mtsTestComponents.h

  Name of tests are Test<_clientType><_serverType>{,Blocking}.
*/
class mtsCommandAndEventNetworkedTest: public CppUnit::TestFixture, public cmnGenericObject
{
private:
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    CPPUNIT_TEST_SUITE(mtsCommandAndEventNetworkedTest);
    {
        CPPUNIT_TEST(TestDeviceDevice);
#if 0
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
#endif
    }
    CPPUNIT_TEST_SUITE_END();

    // pipes used to communicate with different processes
    osaPipeExec PipeComponentManager;
    osaPipeExec PipeProcessServer;
    osaPipeExec PipeProcessClient;
    osaPipeExec PipeConfigurationManager;

public:
    mtsCommandAndEventNetworkedTest();

    void setUp(void);
    void tearDown(void);

    // send a message and retrieve result
    bool SendAndReceive(osaPipeExec & pipe,
                        const std::string & send,
                        std::string & received,
                        const double & timeOut = 20.0 * cmn_s);

    // send message and test for time out as well as expected answer
    void SendAndVerify(osaPipeExec & pipe,
                       const std::string & send,
                       const std::string & expected,
                       const double & timeOut = 20.0 * cmn_s);

    // start all components
    void StartAllComponents(void);

    // quit all components
    void StopAllComponents(void);

    // test that all components are alive
    void PingAllComponents(void);

    // test that all processes are registered on component manager
    void VerifyProcesses(void);

#if 0
    template <class _clientType, class _serverType>
    void TestExecution(_clientType * client, _serverType * server,
                       double clientExecutionDelay, double serverExecutionDelay,
                       double blockingDelay = 0.0);
#endif
    void TestDeviceDevice(void);
#if 0
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
#endif
};


CMN_DECLARE_SERVICES_INSTANTIATION(mtsCommandAndEventNetworkedTest);
