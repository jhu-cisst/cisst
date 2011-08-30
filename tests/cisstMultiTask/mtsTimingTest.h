/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Martin Kelly
  Created on: 2011-05-13

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
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstOSAbstraction/osaCPUAffinity.h>
#include "mtsTestTimingComponents.h"

const double StateTransitionMaximumDelay = 5.0 * cmn_s;

class mtsTimingTest: public CppUnit::TestFixture
{
private:
    CPPUNIT_TEST_SUITE(mtsTimingTest);
    {
		// Test continuous
		CPPUNIT_TEST(TestContinuousPriorityVeryLowAffinity0);
		CPPUNIT_TEST(TestContinuousPriorityVeryLowAffinity1);
		CPPUNIT_TEST(TestContinuousPriorityLowAffinity0);
		CPPUNIT_TEST(TestContinuousPriorityLowAffinity1);
		CPPUNIT_TEST(TestContinuousPriorityNormalAffinity0);
		CPPUNIT_TEST(TestContinuousPriorityNormalAffinity1);
		CPPUNIT_TEST(TestContinuousPriorityHighAffinity0);
		CPPUNIT_TEST(TestContinuousPriorityHighAffinity1);
		CPPUNIT_TEST(TestContinuousPriorityVeryHighAffinity0);
		CPPUNIT_TEST(TestContinuousPriorityVeryHighAffinity1);

		// Test periodic
		CPPUNIT_TEST(TestPeriodicPriorityVeryLowAffinity0RunDummyComputation);
		CPPUNIT_TEST(TestPeriodicPriorityVeryLowAffinity0RunOsaSleep);
		CPPUNIT_TEST(TestPeriodicPriorityVeryLowAffinity0RunOsaThreadSleep);
		CPPUNIT_TEST(TestPeriodicPriorityVeryLowAffinity1RunDummyComputation);
		CPPUNIT_TEST(TestPeriodicPriorityVeryLowAffinity1RunOsaSleep);
		CPPUNIT_TEST(TestPeriodicPriorityVeryLowAffinity1RunOsaThreadSleep);
		CPPUNIT_TEST(TestPeriodicPriorityLowAffinity0RunDummyComputation);
		CPPUNIT_TEST(TestPeriodicPriorityLowAffinity0RunOsaSleep);
		CPPUNIT_TEST(TestPeriodicPriorityLowAffinity0RunOsaThreadSleep);
		CPPUNIT_TEST(TestPeriodicPriorityLowAffinity1RunDummyComputation);
		CPPUNIT_TEST(TestPeriodicPriorityLowAffinity1RunOsaSleep);
		CPPUNIT_TEST(TestPeriodicPriorityLowAffinity1RunOsaThreadSleep);
		CPPUNIT_TEST(TestPeriodicPriorityNormalAffinity0RunDummyComputation);
		CPPUNIT_TEST(TestPeriodicPriorityNormalAffinity0RunOsaSleep);
		CPPUNIT_TEST(TestPeriodicPriorityNormalAffinity0RunOsaThreadSleep);
		CPPUNIT_TEST(TestPeriodicPriorityNormalAffinity1RunDummyComputation);
		CPPUNIT_TEST(TestPeriodicPriorityNormalAffinity1RunOsaSleep);
		CPPUNIT_TEST(TestPeriodicPriorityNormalAffinity1RunOsaThreadSleep);
		CPPUNIT_TEST(TestPeriodicPriorityHighAffinity0RunDummyComputation);
		CPPUNIT_TEST(TestPeriodicPriorityHighAffinity0RunOsaSleep);
		CPPUNIT_TEST(TestPeriodicPriorityHighAffinity0RunOsaThreadSleep);
		CPPUNIT_TEST(TestPeriodicPriorityHighAffinity1RunDummyComputation);
		CPPUNIT_TEST(TestPeriodicPriorityHighAffinity1RunOsaSleep);
		CPPUNIT_TEST(TestPeriodicPriorityHighAffinity1RunOsaThreadSleep);
		CPPUNIT_TEST(TestPeriodicPriorityVeryHighAffinity0RunDummyComputation);
		CPPUNIT_TEST(TestPeriodicPriorityVeryHighAffinity0RunOsaSleep);
		CPPUNIT_TEST(TestPeriodicPriorityVeryHighAffinity0RunOsaThreadSleep);
		CPPUNIT_TEST(TestPeriodicPriorityVeryHighAffinity1RunDummyComputation);
		CPPUNIT_TEST(TestPeriodicPriorityVeryHighAffinity1RunOsaSleep);
		CPPUNIT_TEST(TestPeriodicPriorityVeryHighAffinity1RunOsaThreadSleep);
    }
    CPPUNIT_TEST_SUITE_END();

    mtsManagerLocal * Manager;

public:
    mtsTimingTest(void);

    template <class _componentType> void TestExecution(_componentType * component);
    
    void TestContinuous(const std::string & name,
                        PriorityType threadPriority,
                        osaCPUMask CPUAffinity);

    void TestPeriodic(const std::string & name,
                      PriorityType threadPriority,
                      osaCPUMask CPUAffinity,
                      RunBehavior runBehavior,
                      double period = 1.0,
                      double load = 0.5);

    void TestContinuousPriorityVeryLowAffinity0(void);
    void TestContinuousPriorityVeryLowAffinity1(void);
    void TestContinuousPriorityLowAffinity0(void);
    void TestContinuousPriorityLowAffinity1(void);
    void TestContinuousPriorityNormalAffinity0(void);
    void TestContinuousPriorityNormalAffinity1(void);
    void TestContinuousPriorityHighAffinity0(void);
    void TestContinuousPriorityHighAffinity1(void);
    void TestContinuousPriorityVeryHighAffinity0(void);
    void TestContinuousPriorityVeryHighAffinity1(void);

    void TestPeriodicPriorityVeryLowAffinity0RunDummyComputation(void);
    void TestPeriodicPriorityVeryLowAffinity0RunOsaSleep(void);
    void TestPeriodicPriorityVeryLowAffinity0RunOsaThreadSleep(void);
    void TestPeriodicPriorityVeryLowAffinity1RunDummyComputation(void);
    void TestPeriodicPriorityVeryLowAffinity1RunOsaSleep(void);
    void TestPeriodicPriorityVeryLowAffinity1RunOsaThreadSleep(void);
    void TestPeriodicPriorityLowAffinity0RunDummyComputation(void);
    void TestPeriodicPriorityLowAffinity0RunOsaSleep(void);
    void TestPeriodicPriorityLowAffinity0RunOsaThreadSleep(void);
    void TestPeriodicPriorityLowAffinity1RunDummyComputation(void);
    void TestPeriodicPriorityLowAffinity1RunOsaSleep(void);
    void TestPeriodicPriorityLowAffinity1RunOsaThreadSleep(void);
    void TestPeriodicPriorityNormalAffinity0RunDummyComputation(void);
    void TestPeriodicPriorityNormalAffinity0RunOsaSleep(void);
    void TestPeriodicPriorityNormalAffinity0RunOsaThreadSleep(void);
    void TestPeriodicPriorityNormalAffinity1RunDummyComputation(void);
    void TestPeriodicPriorityNormalAffinity1RunOsaSleep(void);
    void TestPeriodicPriorityNormalAffinity1RunOsaThreadSleep(void);
    void TestPeriodicPriorityHighAffinity0RunDummyComputation(void);
    void TestPeriodicPriorityHighAffinity0RunOsaSleep(void);
    void TestPeriodicPriorityHighAffinity0RunOsaThreadSleep(void);
    void TestPeriodicPriorityHighAffinity1RunDummyComputation(void);
    void TestPeriodicPriorityHighAffinity1RunOsaSleep(void);
    void TestPeriodicPriorityHighAffinity1RunOsaThreadSleep(void);
    void TestPeriodicPriorityVeryHighAffinity0RunDummyComputation(void);
    void TestPeriodicPriorityVeryHighAffinity0RunOsaSleep(void);
    void TestPeriodicPriorityVeryHighAffinity0RunOsaThreadSleep(void);
    void TestPeriodicPriorityVeryHighAffinity1RunDummyComputation(void);
    void TestPeriodicPriorityVeryHighAffinity1RunOsaSleep(void);
    void TestPeriodicPriorityVeryHighAffinity1RunOsaThreadSleep(void);
};

CPPUNIT_TEST_SUITE_REGISTRATION(mtsTimingTest);
