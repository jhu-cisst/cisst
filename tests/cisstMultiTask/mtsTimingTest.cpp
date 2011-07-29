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

#include <fstream>
#include <cisstOSAbstraction/osaSleep.h>
#include <cmnPath.h>
#include <mtsIntervalStatistics.h>
#include <mtsStateTable.h>
#include "mtsTimingTest.h"
#include "mtsTestTimingComponents.h"

mtsTimingTest::mtsTimingTest(void)
{
    Manager = mtsManagerLocal::GetInstance();
}

mtsTimingTest::~mtsTimingTest(void)
{
}

template <class _componentType>
void mtsTimingTest::TestExecution(_componentType * component)
{
    Manager->AddComponent(component);
    Manager->CreateAll();
    CPPUNIT_ASSERT(manager->WaitForStateAll(mtsComponentState::READY, StateTransitionMaximumDelay));
    Manager->StartAll();
    CPPUNIT_ASSERT(manager->WaitForStateAll(mtsComponentState::ACTIVE, StateTransitionMaximumDelay));
    while (!component->Done()) {
        osaSleep(15 * cmn_ms);
    }
    Manager->KillAll();
    CPPUNIT_ASSERT(manager->WaitForStateAll(mtsComponentState::FINISHED, StateTransitionMaximumDelay));
    Manager->Remove(component);
    Manager->Cleanup();

    // Save run results
    const mtsStateTable * stateTable = component.GetDefaultStateTable();
    mtsIntervalStatistics statistics = stateTable.periodStats();

    double average = statistics.getAvg();
    double stdDev = statistics.getStdDev();
    double min = statistics.getMin();
    double max = statistics.getMax();
    std::ofstream resultsFile("TimingTestResults.txt");
    resultsFile << cmnPrintf("Average: %.4f\n") << average;
    resultsFile << cmnPrintf("Stdev: %.4f\n") << stdDev;
    resultsFile << cmnPrintf("Min: %.4f\n") << min;
    resultsFile << cmnPrintf("Max: %.4f\n") << max;
    resultsFile.close();

    // Asserts for testing
    double period = component->GetPeriod();
#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    double stdDevThreshold = 5 * cmn_ms;
#elif (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
    double stdDevThreshold = 1 * cmn_ms;
#elif (CISST_OS == CISST_WINDOWS)
    double stdDevThreshold = 20 * cmn_ms;
#endif
    double rangeThreshold = 2 * stdDevThreshold;
    CPP_UNIT_ASSERT(stdDev <= stdDevThreshold);
    CPP_UNIT_ASSERT(max - min <= rangeThreshold);
}

void mtsTimingTest::TestContinuous(const PriorityType & threadPriority,
                                   const osaCPUMask & CPUAffinity)
{
    mtsTestTimingContinuous * task = new mtsTestTimingContinuous();

    task->SetIterations(1000);
    task->SetThreadPriority(threadPriority);
    task->SetCPUAffinity(CPUAffinity);

    TestExecution<mtsTestTimingContinuous>(task);
    delete task;
}

void mtsTimingTest::TestPeriodic(double period,
                                 double load,
                                 const PriorityType & threadPriority,
                                 const osaCPUMask & CPUAffinity,
                                 const RunBehavior & runBehavior)
{
    mtsTestTimingPeriodic * task = new mtsTestTimingPeriodic();

    task->SetIterations(1000);
    task->SetPeriod(period);
    task->SetLoad(load);
    task->SetThreadPriority(threadPriority);
    task->SetCPUAffinity(CPUAffinity);
    task->SetRunBehavior(runBehavior);

    TestExecution<mtsTestTimingPeriodic>(task);
    delete task;
}

void mtsTimingTest::TestContinuousPriorityVeryLowAffinity0(void)
{
     TestContinuous(PRIORITY_VERY_LOW, 0);
}
void mtsTimingTest::TestContinuousPriorityVeryLowAffinity1(void)
{
     TestContinuous(PRIORITY_VERY_LOW, 1);
}
void mtsTimingTest::TestContinuousPriorityLowAffinity0(void)
{
     TestContinuous(PRIORITY_LOW, 0);
}
void mtsTimingTest::TestContinuousPriorityLowAffinity1(void)
{
     TestContinuous(PRIORITY_LOW, 1);
}
void mtsTimingTest::TestContinuousPriorityNormalAffinity0(void)
{
     TestContinuous(PRIORITY_NORMAL, 0);
}
void mtsTimingTest::TestContinuousPriorityNormalAffinity1(void)
{
     TestContinuous(PRIORITY_NORMAL, 1);
}
void mtsTimingTest::TestContinuousPriorityHighAffinity0(void)
{
     TestContinuous(PRIORITY_HIGH, 0);
}
void mtsTimingTest::TestContinuousPriorityHighAffinity1(void)
{
     TestContinuous(PRIORITY_HIGH, 1);
}
void mtsTimingTest::TestContinuousPriorityVeryHighAffinity0(void)
{
     TestContinuous(PRIORITY_VERY_HIGH, 0);
}
void mtsTimingTest::TestContinuousPriorityVeryHighAffinity1(void)
{
     TestContinuous(PRIORITY_VERY_HIGH, 1);
}

void mtsTimingTest::TestPeriodicPriorityVeryLowAffinity0RundummyComputation(void)
{
    TestPeriodic(PRIORITY_VERY_LOW, 0, dummyComputation);
}
void mtsTimingTest::TestPeriodicPriorityVeryLowAffinity0RunosaSleep(void)
{
    TestPeriodic(PRIORITY_VERY_LOW, 0, osaSleep);
}
void mtsTimingTest::TestPeriodicPriorityVeryLowAffinity0RunosaThreadSleep(void)
{
    TestPeriodic(PRIORITY_VERY_LOW, 0, osaThreadSleep);
}
void mtsTimingTest::TestPeriodicPriorityVeryLowAffinity1RundummyComputation(void)
{
    TestPeriodic(PRIORITY_VERY_LOW, 1, dummyComputation);
}
void mtsTimingTest::TestPeriodicPriorityVeryLowAffinity1RunosaSleep(void)
{
    TestPeriodic(PRIORITY_VERY_LOW, 1, osaSleep);
}
void mtsTimingTest::TestPeriodicPriorityVeryLowAffinity1RunosaThreadSleep(void)
{
    TestPeriodic(PRIORITY_VERY_LOW, 1, osaThreadSleep);
}
void mtsTimingTest::TestPeriodicPriorityLowAffinity0RundummyComputation(void)
{
    TestPeriodic(PRIORITY_LOW, 0, dummyComputation);
}
void mtsTimingTest::TestPeriodicPriorityLowAffinity0RunosaSleep(void)
{
    TestPeriodic(PRIORITY_LOW, 0, osaSleep);
}
void mtsTimingTest::TestPeriodicPriorityLowAffinity0RunosaThreadSleep(void)
{
    TestPeriodic(PRIORITY_LOW, 0, osaThreadSleep);
}
void mtsTimingTest::TestPeriodicPriorityLowAffinity1RundummyComputation(void)
{
    TestPeriodic(PRIORITY_LOW, 1, dummyComputation);
}
void mtsTimingTest::TestPeriodicPriorityLowAffinity1RunosaSleep(void)
{
    TestPeriodic(PRIORITY_LOW, 1, osaSleep);
}
void mtsTimingTest::TestPeriodicPriorityLowAffinity1RunosaThreadSleep(void)
{
    TestPeriodic(PRIORITY_LOW, 1, osaThreadSleep);
}
void mtsTimingTest::TestPeriodicPriorityNormalAffinity0RundummyComputation(void)
{
    TestPeriodic(PRIORITY_NORMAL, 0, dummyComputation);
}
void mtsTimingTest::TestPeriodicPriorityNormalAffinity0RunosaSleep(void)
{
    TestPeriodic(PRIORITY_NORMAL, 0, osaSleep);
}
void mtsTimingTest::TestPeriodicPriorityNormalAffinity0RunosaThreadSleep(void)
{
    TestPeriodic(PRIORITY_NORMAL, 0, osaThreadSleep);
}
void mtsTimingTest::TestPeriodicPriorityNormalAffinity1RundummyComputation(void)
{
    TestPeriodic(PRIORITY_NORMAL, 1, dummyComputation);
}
void mtsTimingTest::TestPeriodicPriorityNormalAffinity1RunosaSleep(void)
{
    TestPeriodic(PRIORITY_NORMAL, 1, osaSleep);
}
void mtsTimingTest::TestPeriodicPriorityNormalAffinity1RunosaThreadSleep(void)
{
    TestPeriodic(PRIORITY_NORMAL, 1, osaThreadSleep);
}
void mtsTimingTest::TestPeriodicPriorityHighAffinity0RundummyComputation(void)
{
    TestPeriodic(PRIORITY_HIGH, 0, dummyComputation);
}
void mtsTimingTest::TestPeriodicPriorityHighAffinity0RunosaSleep(void)
{
    TestPeriodic(PRIORITY_HIGH, 0, osaSleep);
}
void mtsTimingTest::TestPeriodicPriorityHighAffinity0RunosaThreadSleep(void)
{
    TestPeriodic(PRIORITY_HIGH, 0, osaThreadSleep);
}
void mtsTimingTest::TestPeriodicPriorityHighAffinity1RundummyComputation(void)
{
    TestPeriodic(PRIORITY_HIGH, 1, dummyComputation);
}
void mtsTimingTest::TestPeriodicPriorityHighAffinity1RunosaSleep(void)
{
    TestPeriodic(PRIORITY_HIGH, 1, osaSleep);
}
void mtsTimingTest::TestPeriodicPriorityHighAffinity1RunosaThreadSleep(void)
{
    TestPeriodic(PRIORITY_HIGH, 1, osaThreadSleep);
}
void mtsTimingTest::TestPeriodicPriorityVeryHighAffinity0RundummyComputation(void)
{
    TestPeriodic(PRIORITY_VERY_HIGH, 0, dummyComputation);
}
void mtsTimingTest::TestPeriodicPriorityVeryHighAffinity0RunosaSleep(void)
{
    TestPeriodic(PRIORITY_VERY_HIGH, 0, osaSleep);
}
void mtsTimingTest::TestPeriodicPriorityVeryHighAffinity0RunosaThreadSleep(void)
{
    TestPeriodic(PRIORITY_VERY_HIGH, 0, osaThreadSleep);
}
void mtsTimingTest::TestPeriodicPriorityVeryHighAffinity1RundummyComputation(void)
{
    TestPeriodic(PRIORITY_VERY_HIGH, 1, dummyComputation);
}
void mtsTimingTest::TestPeriodicPriorityVeryHighAffinity1RunosaSleep(void)
{
    TestPeriodic(PRIORITY_VERY_HIGH, 1, osaSleep);
}
void mtsTimingTest::TestPeriodicPriorityVeryHighAffinity1RunosaThreadSleep(void)
{
    TestPeriodic(PRIORITY_VERY_HIGH, 1, osaThreadSleep);
}
