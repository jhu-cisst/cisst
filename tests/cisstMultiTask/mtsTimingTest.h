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
		CPPUNIT_TEST(TestPeriodicPriorityVeryLowAffinity0RunDummy);
		CPPUNIT_TEST(TestPeriodicPriorityVeryLowAffinity0RunOsaSleep);
		CPPUNIT_TEST(TestPeriodicPriorityVeryLowAffinity0RunOsaThreadSleep);
		CPPUNIT_TEST(TestPeriodicPriorityVeryLowAffinity1RunDummy);
		CPPUNIT_TEST(TestPeriodicPriorityVeryLowAffinity1RunOsaSleep);
		CPPUNIT_TEST(TestPeriodicPriorityVeryLowAffinity1RunOsaThreadSleep);
		CPPUNIT_TEST(TestPeriodicPriorityLowAffinity0RunDummy);
		CPPUNIT_TEST(TestPeriodicPriorityLowAffinity0RunOsaSleep);
		CPPUNIT_TEST(TestPeriodicPriorityLowAffinity0RunOsaThreadSleep);
		CPPUNIT_TEST(TestPeriodicPriorityLowAffinity1RunDummy);
		CPPUNIT_TEST(TestPeriodicPriorityLowAffinity1RunOsaSleep);
		CPPUNIT_TEST(TestPeriodicPriorityLowAffinity1RunOsaThreadSleep);
		CPPUNIT_TEST(TestPeriodicPriorityNormalAffinity0RunDummy);
		CPPUNIT_TEST(TestPeriodicPriorityNormalAffinity0RunOsaSleep);
		CPPUNIT_TEST(TestPeriodicPriorityNormalAffinity0RunOsaThreadSleep);
		CPPUNIT_TEST(TestPeriodicPriorityNormalAffinity1RunDummy);
		CPPUNIT_TEST(TestPeriodicPriorityNormalAffinity1RunOsaSleep);
		CPPUNIT_TEST(TestPeriodicPriorityNormalAffinity1RunOsaThreadSleep);
		CPPUNIT_TEST(TestPeriodicPriorityHighAffinity0RunDummy);
		CPPUNIT_TEST(TestPeriodicPriorityHighAffinity0RunOsaSleep);
		CPPUNIT_TEST(TestPeriodicPriorityHighAffinity0RunOsaThreadSleep);
		CPPUNIT_TEST(TestPeriodicPriorityHighAffinity1RunDummy);
		CPPUNIT_TEST(TestPeriodicPriorityHighAffinity1RunOsaSleep);
		CPPUNIT_TEST(TestPeriodicPriorityHighAffinity1RunOsaThreadSleep);
		CPPUNIT_TEST(TestPeriodicPriorityVeryHighAffinity0RunDummy);
		CPPUNIT_TEST(TestPeriodicPriorityVeryHighAffinity0RunOsaSleep);
		CPPUNIT_TEST(TestPeriodicPriorityVeryHighAffinity0RunOsaThreadSleep);
		CPPUNIT_TEST(TestPeriodicPriorityVeryHighAffinity1RunDummy);
		CPPUNIT_TEST(TestPeriodicPriorityVeryHighAffinity1RunOsaSleep);
		CPPUNIT_TEST(TestPeriodicPriorityVeryHighAffinity1RunOsaThreadSleep);
    }
    CPPUNIT_TEST_SUITE_END();

    mtsManagerLocal * Manager;

public:
    mtsTimingTest(void);
    ~mtsTimingTest(void);

    template <class _componentType> void TestExecution(_componentType * component);

    
    void TestContinuous(const PriorityType & threadPriority,
                        const osaCPUMask & CPUAffinity);

    void TestPeriodic(double period,
                      double load,
                      const PriorityType & threadPriority,
                      const osaCPUMask & CPUAffinity,
                      const RunBehavior & runBehavior);


    void TestContinuousPriorityVeryLowAffinity0(const PriorityType & threadPriority,
                                                const osaCPUMask & CPUAffinity);
    void TestContinuousPriorityVeryLowAffinity1(const PriorityType & threadPriority,
                                                const osaCPUMask & CPUAffinity);
    void TestContinuousPriorityLowAffinity0(const PriorityType & threadPriority,
                                            const osaCPUMask & CPUAffinity);
    void TestContinuousPriorityLowAffinity1(const PriorityType & threadPriority,
                                            const osaCPUMask & CPUAffinity);
    void TestContinuousPriorityNormalAffinity0(const PriorityType & threadPriority,
                                               const osaCPUMask & CPUAffinity);
    void TestContinuousPriorityNormalAffinity1(const PriorityType & threadPriority,
                                               const osaCPUMask & CPUAffinity);
    void TestContinuousPriorityHighAffinity0(const PriorityType & threadPriority,
                                             const osaCPUMask & CPUAffinity);
    void TestContinuousPriorityHighAffinity1(const PriorityType & threadPriority,
                                             const osaCPUMask & CPUAffinity);
    void TestContinuousPriorityVeryHighAffinity0(const PriorityType & threadPriority,
                                                 const osaCPUMask & CPUAffinity);
    void TestContinuousPriorityVeryHighAffinity1(const PriorityType & threadPriority,
                                                 const osaCPUMask & CPUAffinity);

    void TestPeriodicPriorityVeryLowAffinity0RunDummy(double period,
                                                      double load,
                                                      const PriorityType & threadPriority,
                                                      const osaCPUMask & CPUAffinity,
                                                      const RunBehavior & runBehavior);
    void TestPeriodicPriorityVeryLowAffinity0RunOsaSleep(double period,
                                                         double load,
                                                         const PriorityType & threadPriority,
                                                         const osaCPUMask & CPUAffinity,
                                                         const RunBehavior & runBehavior);
    void TestPeriodicPriorityVeryLowAffinity0RunOsaThreadSleep(double period,
                                                               double load,
                                                               const PriorityType & threadPriority,
                                                               const osaCPUMask & CPUAffinity,
                                                               const RunBehavior & runBehavior);
    void TestPeriodicPriorityVeryLowAffinity1RunDummy(double period,
                                                      double load,
                                                      const PriorityType & threadPriority,
                                                      const osaCPUMask & CPUAffinity,
                                                      const RunBehavior & runBehavior);
    void TestPeriodicPriorityVeryLowAffinity1RunOsaSleep(double period,
                                                         double load,
                                                         const PriorityType & threadPriority,
                                                         const osaCPUMask & CPUAffinity,
                                                         const RunBehavior & runBehavior);
    void TestPeriodicPriorityVeryLowAffinity1RunOsaThreadSleep(double period,
                                                               double load,
                                                               const PriorityType & threadPriority,
                                                               const osaCPUMask & CPUAffinity,
                                                               const RunBehavior & runBehavior);
    void TestPeriodicPriorityLowAffinity0RunDummy(double period,
                                                  double load,
                                                  const PriorityType & threadPriority,
                                                  const osaCPUMask & CPUAffinity,
                                                  const RunBehavior & runBehavior);
    void TestPeriodicPriorityLowAffinity0RunOsaSleep(double period,
                                                     double load,
                                                     const PriorityType & threadPriority,
                                                     const osaCPUMask & CPUAffinity,
                                                     const RunBehavior & runBehavior);
    void TestPeriodicPriorityLowAffinity0RunOsaThreadSleep(double period,
                                                           double load,
                                                           const PriorityType & threadPriority,
                                                           const osaCPUMask & CPUAffinity,
                                                           const RunBehavior & runBehavior);
    void TestPeriodicPriorityLowAffinity1RunDummy(double period,
                                                  double load,
                                                  const PriorityType & threadPriority,
                                                  const osaCPUMask & CPUAffinity,
                                                  const RunBehavior & runBehavior);
    void TestPeriodicPriorityLowAffinity1RunOsaSleep(double period,
                                                     double load,
                                                     const PriorityType & threadPriority,
                                                     const osaCPUMask & CPUAffinity,
                                                     const RunBehavior & runBehavior);
    void TestPeriodicPriorityLowAffinity1RunOsaThreadSleep(double period,
                                                           double load,
                                                           const PriorityType & threadPriority,
                                                           const osaCPUMask & CPUAffinity,
                                                           const RunBehavior & runBehavior);
    void TestPeriodicPriorityNormalAffinity0RunDummy(double period,
                                                     double load,
                                                     const PriorityType & threadPriority,
                                                     const osaCPUMask & CPUAffinity,
                                                     const RunBehavior & runBehavior);
    void TestPeriodicPriorityNormalAffinity0RunOsaSleep(double period,
                                                        double load,
                                                        const PriorityType & threadPriority,
                                                        const osaCPUMask & CPUAffinity,
                                                        const RunBehavior & runBehavior);
    void TestPeriodicPriorityNormalAffinity0RunOsaThreadSleep(double period,
                                                              double load,
                                                              const PriorityType & threadPriority,
                                                              const osaCPUMask & CPUAffinity,
                                                              const RunBehavior & runBehavior);
    void TestPeriodicPriorityNormalAffinity1RunDummy(double period,
                                                     double load,
                                                     const PriorityType & threadPriority,
                                                     const osaCPUMask & CPUAffinity,
                                                     const RunBehavior & runBehavior);
    void TestPeriodicPriorityNormalAffinity1RunOsaSleep(double period,
                                                        double load,
                                                        const PriorityType & threadPriority,
                                                        const osaCPUMask & CPUAffinity,
                                                        const RunBehavior & runBehavior);
    void TestPeriodicPriorityNormalAffinity1RunOsaThreadSleep(double period,
                                                              double load,
                                                              const PriorityType & threadPriority,
                                                              const osaCPUMask & CPUAffinity,
                                                              const RunBehavior & runBehavior);
    void TestPeriodicPriorityHighAffinity0RunDummy(double period,
                                                   double load,
                                                   const PriorityType & threadPriority,
                                                   const osaCPUMask & CPUAffinity,
                                                   const RunBehavior & runBehavior);
    void TestPeriodicPriorityHighAffinity0RunOsaSleep(double period,
                                                      double load,
                                                      const PriorityType & threadPriority,
                                                      const osaCPUMask & CPUAffinity,
                                                      const RunBehavior & runBehavior);
    void TestPeriodicPriorityHighAffinity0RunOsaThreadSleep(double period,
                                                            double load,
                                                            const PriorityType & threadPriority,
                                                            const osaCPUMask & CPUAffinity,
                                                            const RunBehavior & runBehavior);
    void TestPeriodicPriorityHighAffinity1RunDummy(double period,
                                                   double load,
                                                   const PriorityType & threadPriority,
                                                   const osaCPUMask & CPUAffinity,
                                                   const RunBehavior & runBehavior);
    void TestPeriodicPriorityHighAffinity1RunOsaSleep(double period,
                                                      double load,
                                                      const PriorityType & threadPriority,
                                                      const osaCPUMask & CPUAffinity,
                                                      const RunBehavior & runBehavior);
    void TestPeriodicPriorityHighAffinity1RunOsaThreadSleep(double period,
                                                            double load,
                                                            const PriorityType & threadPriority,
                                                            const osaCPUMask & CPUAffinity,
                                                            const RunBehavior & runBehavior);
    void TestPeriodicPriorityVeryHighAffinity0RunDummy(double period,
                                                       double load,
                                                       const PriorityType & threadPriority,
                                                       const osaCPUMask & CPUAffinity,
                                                       const RunBehavior & runBehavior);
    void TestPeriodicPriorityVeryHighAffinity0RunOsaSleep(double period,
                                                          double load,
                                                          const PriorityType & threadPriority,
                                                          const osaCPUMask & CPUAffinity,
                                                          const RunBehavior & runBehavior);
    void TestPeriodicPriorityVeryHighAffinity0RunOsaThreadSleep(double period,
                                                                double load,
                                                                const PriorityType & threadPriority,
                                                                const osaCPUMask & CPUAffinity,
                                                                const RunBehavior & runBehavior);
    void TestPeriodicPriorityVeryHighAffinity1RunDummy(double period,
                                                       double load,
                                                       const PriorityType & threadPriority,
                                                       const osaCPUMask & CPUAffinity,
                                                       const RunBehavior & runBehavior);
    void TestPeriodicPriorityVeryHighAffinity1RunOsaSleep(double period,
                                                          double load,
                                                          const PriorityType & threadPriority,
                                                          const osaCPUMask & CPUAffinity,
                                                          const RunBehavior & runBehavior);
    void TestPeriodicPriorityVeryHighAffinity1RunOsaThreadSleep(double period,
                                                                double load,
                                                                const PriorityType & threadPriority,
                                                                const osaCPUMask & CPUAffinity,
                                                                const RunBehavior & runBehavior);
};

CPPUNIT_TEST_SUITE_REGISTRATION(mtsTimingTest);
