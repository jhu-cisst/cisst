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

#include <cisstCommon/cmnPrintf.h>
#include <cisstMultiTask/mtsIntervalStatistics.h>
#include <cisstMultiTask/mtsStateTable.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <fstream>
#include "mtsTimingTest.h"

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
    CPPUNIT_ASSERT(Manager->WaitForStateAll(mtsComponentState::READY, StateTransitionMaximumDelay));
    Manager->StartAll();
    CPPUNIT_ASSERT(Manager->WaitForStateAll(mtsComponentState::ACTIVE, StateTransitionMaximumDelay));
    while (!component->Done()) {
        osaSleep(15 * cmn_ms);
    }
    Manager->KillAll();
    CPPUNIT_ASSERT(Manager->WaitForStateAll(mtsComponentState::FINISHED, StateTransitionMaximumDelay));
    Manager->Cleanup();

    // Save run results
    const mtsStateTable * stateTable = component->GetDefaultStateTable();
    const mtsIntervalStatistics & statistics = stateTable->PeriodStats;

    double average = statistics.GetAvg();
    double stdDev = statistics.GetStdDev();
    double min = statistics.GetMin();
    double max = statistics.GetMax();
    std::ofstream resultsFile("TimingTestResults.txt");
    resultsFile << cmnPrintf("Average: %.4f\n") << average;
    resultsFile << cmnPrintf("Stdev: %.4f\n") << stdDev;
    resultsFile << cmnPrintf("Min: %.4f\n") << min;
    resultsFile << cmnPrintf("Max: %.4f\n") << max;
    resultsFile.close();

    // Asserts for testing
#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    double stdDevThreshold = 5 * cmn_ms;
#elif (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
    double stdDevThreshold = 1 * cmn_ms;
#elif (CISST_OS == CISST_WINDOWS)
    double stdDevThreshold = 20 * cmn_ms;
#endif
    double rangeThreshold = 2 * stdDevThreshold;
    CPPUNIT_ASSERT(stdDev <= stdDevThreshold);
    CPPUNIT_ASSERT(max - min <= rangeThreshold);
}

void mtsTimingTest::TestContinuous(PriorityType threadPriority,
                                   osaCPUMask CPUAffinity)
{
    mtsTestTimingContinuous * task = new mtsTestTimingContinuous();

    task->SetIterations(1000);
    task->SetThreadPriority(threadPriority);
    task->SetCPUAffinity(CPUAffinity);

    TestExecution<mtsTestTimingContinuous>(task);
    delete task;
}

void mtsTimingTest::TestPeriodic(PriorityType threadPriority,
                                 osaCPUMask CPUAffinity,
                                 RunBehavior runBehavior,
                                 double period,
                                 double load)
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

void mtsTimingTest::TestPeriodicPriorityVeryLowAffinity0RunDummyComputation(void)
{
    TestPeriodic(PRIORITY_VERY_LOW, 0, dummyComputationBehavior);
}
void mtsTimingTest::TestPeriodicPriorityVeryLowAffinity0RunOsaSleep(void)
{
    TestPeriodic(PRIORITY_VERY_LOW, 0, osaSleepBehavior);
}
void mtsTimingTest::TestPeriodicPriorityVeryLowAffinity0RunOsaThreadSleep(void)
{
    TestPeriodic(PRIORITY_VERY_LOW, 0, osaThreadSleepBehavior);
}
void mtsTimingTest::TestPeriodicPriorityVeryLowAffinity1RunDummyComputation(void)
{
    TestPeriodic(PRIORITY_VERY_LOW, 1, dummyComputationBehavior);
}
void mtsTimingTest::TestPeriodicPriorityVeryLowAffinity1RunOsaSleep(void)
{
    TestPeriodic(PRIORITY_VERY_LOW, 1, osaSleepBehavior);
}
void mtsTimingTest::TestPeriodicPriorityVeryLowAffinity1RunOsaThreadSleep(void)
{
    TestPeriodic(PRIORITY_VERY_LOW, 1, osaThreadSleepBehavior);
}
void mtsTimingTest::TestPeriodicPriorityLowAffinity0RunDummyComputation(void)
{
    TestPeriodic(PRIORITY_LOW, 0, dummyComputationBehavior);
}
void mtsTimingTest::TestPeriodicPriorityLowAffinity0RunOsaSleep(void)
{
    TestPeriodic(PRIORITY_LOW, 0, osaSleepBehavior);
}
void mtsTimingTest::TestPeriodicPriorityLowAffinity0RunOsaThreadSleep(void)
{
    TestPeriodic(PRIORITY_LOW, 0, osaThreadSleepBehavior);
}
void mtsTimingTest::TestPeriodicPriorityLowAffinity1RunDummyComputation(void)
{
    TestPeriodic(PRIORITY_LOW, 1, dummyComputationBehavior);
}
void mtsTimingTest::TestPeriodicPriorityLowAffinity1RunOsaSleep(void)
{
    TestPeriodic(PRIORITY_LOW, 1, osaSleepBehavior);
}
void mtsTimingTest::TestPeriodicPriorityLowAffinity1RunOsaThreadSleep(void)
{
    TestPeriodic(PRIORITY_LOW, 1, osaThreadSleepBehavior);
}
void mtsTimingTest::TestPeriodicPriorityNormalAffinity0RunDummyComputation(void)
{
    TestPeriodic(PRIORITY_NORMAL, 0, dummyComputationBehavior);
}
void mtsTimingTest::TestPeriodicPriorityNormalAffinity0RunOsaSleep(void)
{
    TestPeriodic(PRIORITY_NORMAL, 0, osaSleepBehavior);
}
void mtsTimingTest::TestPeriodicPriorityNormalAffinity0RunOsaThreadSleep(void)
{
    TestPeriodic(PRIORITY_NORMAL, 0, osaThreadSleepBehavior);
}
void mtsTimingTest::TestPeriodicPriorityNormalAffinity1RunDummyComputation(void)
{
    TestPeriodic(PRIORITY_NORMAL, 1, dummyComputationBehavior);
}
void mtsTimingTest::TestPeriodicPriorityNormalAffinity1RunOsaSleep(void)
{
    TestPeriodic(PRIORITY_NORMAL, 1, osaSleepBehavior);
}
void mtsTimingTest::TestPeriodicPriorityNormalAffinity1RunOsaThreadSleep(void)
{
    TestPeriodic(PRIORITY_NORMAL, 1, osaThreadSleepBehavior);
}
void mtsTimingTest::TestPeriodicPriorityHighAffinity0RunDummyComputation(void)
{
    TestPeriodic(PRIORITY_HIGH, 0, dummyComputationBehavior);
}
void mtsTimingTest::TestPeriodicPriorityHighAffinity0RunOsaSleep(void)
{
    TestPeriodic(PRIORITY_HIGH, 0, osaSleepBehavior);
}
void mtsTimingTest::TestPeriodicPriorityHighAffinity0RunOsaThreadSleep(void)
{
    TestPeriodic(PRIORITY_HIGH, 0, osaThreadSleepBehavior);
}
void mtsTimingTest::TestPeriodicPriorityHighAffinity1RunDummyComputation(void)
{
    TestPeriodic(PRIORITY_HIGH, 1, dummyComputationBehavior);
}
void mtsTimingTest::TestPeriodicPriorityHighAffinity1RunOsaSleep(void)
{
    TestPeriodic(PRIORITY_HIGH, 1, osaSleepBehavior);
}
void mtsTimingTest::TestPeriodicPriorityHighAffinity1RunOsaThreadSleep(void)
{
    TestPeriodic(PRIORITY_HIGH, 1, osaThreadSleepBehavior);
}
void mtsTimingTest::TestPeriodicPriorityVeryHighAffinity0RunDummyComputation(void)
{
    TestPeriodic(PRIORITY_VERY_HIGH, 0, dummyComputationBehavior);
}
void mtsTimingTest::TestPeriodicPriorityVeryHighAffinity0RunOsaSleep(void)
{
    TestPeriodic(PRIORITY_VERY_HIGH, 0, osaSleepBehavior);
}
void mtsTimingTest::TestPeriodicPriorityVeryHighAffinity0RunOsaThreadSleep(void)
{
    TestPeriodic(PRIORITY_VERY_HIGH, 0, osaThreadSleepBehavior);
}
void mtsTimingTest::TestPeriodicPriorityVeryHighAffinity1RunDummyComputation(void)
{
    TestPeriodic(PRIORITY_VERY_HIGH, 1, dummyComputationBehavior);
}
void mtsTimingTest::TestPeriodicPriorityVeryHighAffinity1RunOsaSleep(void)
{
    TestPeriodic(PRIORITY_VERY_HIGH, 1, osaSleepBehavior);
}
void mtsTimingTest::TestPeriodicPriorityVeryHighAffinity1RunOsaThreadSleep(void)
{
    TestPeriodic(PRIORITY_VERY_HIGH, 1, osaThreadSleepBehavior);
}
