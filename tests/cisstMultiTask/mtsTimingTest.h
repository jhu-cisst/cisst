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

class mtsTimingTest: public CppUnit::TestFixture
{
private:
    CPPUNIT_TEST_SUITE(mtsTimingTest);
    {
		// Test continuous
		CPPUNIT_TEST(TestContinuousPriorityLowAffinity0);
		CPPUNIT_TEST(TestContinuousPriorityLowAffinity1);
		CPPUNIT_TEST(TestContinuousPriorityMedAffinity0);
		CPPUNIT_TEST(TestContinuousPriorityMedAffinity1);
		CPPUNIT_TEST(TestContinuousPriorityHighAffinity0);
		CPPUNIT_TEST(TestContinuousPriorityHighAffinity1);

		// Test periodic
		CPPUNIT_TEST(TestPeriodicPriorityLowAffinity0RunDummy);
		CPPUNIT_TEST(TestPeriodicPriorityLowAffinity0RunOsaSleep);
		CPPUNIT_TEST(TestPeriodicPriorityLowAffinity0RunOsaThreadSleep);
		CPPUNIT_TEST(TestPeriodicPriorityLowAffinity1RunDummy);
		CPPUNIT_TEST(TestPeriodicPriorityLowAffinity1RunOsaSleep);
		CPPUNIT_TEST(TestPeriodicPriorityLowAffinity1RunOsaThreadSleep);
		CPPUNIT_TEST(TestPeriodicPriorityMedAffinity0RunDummy);
		CPPUNIT_TEST(TestPeriodicPriorityMedAffinity0RunOsaSleep);
		CPPUNIT_TEST(TestPeriodicPriorityMedAffinity0RunOsaThreadSleep);
		CPPUNIT_TEST(TestPeriodicPriorityMedAffinity1RunDummy);
		CPPUNIT_TEST(TestPeriodicPriorityMedAffinity1RunOsaSleep);
		CPPUNIT_TEST(TestPeriodicPriorityMedAffinity1RunOsaThreadSleep);
		CPPUNIT_TEST(TestPeriodicPriorityHighAffinity0RunDummy);
		CPPUNIT_TEST(TestPeriodicPriorityHighAffinity0RunOsaSleep);
		CPPUNIT_TEST(TestPeriodicPriorityHighAffinity0RunOsaThreadSleep);
		CPPUNIT_TEST(TestPeriodicPriorityHighAffinity1RunDummy);
		CPPUNIT_TEST(TestPeriodicPriorityHighAffinity1RunOsaSleep);
		CPPUNIT_TEST(TestPeriodicPriorityHighAffinity1RunOsaThreadSleep);
    }
    CPPUNIT_TEST_SUITE_END();

    mtsManagerLocal * Manager;

public:
    mtsTimingTest(void);
    ~mtsTimingTest(void);

    template <class _componentType> void TestExecution(_componentType * component);
    void TestContinuous(void);
    void TestPeriodic(void);
};

CPPUNIT_TEST_SUITE_REGISTRATION(mtsTimingTest);
