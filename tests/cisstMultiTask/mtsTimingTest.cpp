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

template <class _componentType>
void mtsTimingTest::TestExecution(_componentType * component)
{
    Manager->AddComponent(component);
    Manager->CreateAll();
    std::cerr << "creating..." << std::endl;
    CPPUNIT_ASSERT(Manager->WaitForStateAll(mtsComponentState::READY, StateTransitionMaximumDelay));
    std::cerr << "created" << std::endl;
    Manager->StartAll();
    std::cerr << "starting..." << std::endl;
    CPPUNIT_ASSERT(Manager->WaitForStateAll(mtsComponentState::ACTIVE, StateTransitionMaximumDelay));
    std::cerr << "started" << std::endl;
    while (!component->Done()) {
        osaSleep(15 * cmn_ms);
    }
    Manager->KillAll();
    std::cerr << "done, killing..." << std::endl;
    CPPUNIT_ASSERT(Manager->WaitForStateAll(mtsComponentState::FINISHED, StateTransitionMaximumDelay));
    std::cerr << "killed" << std::endl;
    Manager->Cleanup();

    // Save run results
    const mtsStateTable * stateTable = component->GetDefaultStateTable();
    const mtsIntervalStatistics & statistics = stateTable->PeriodStats;

    double average = statistics.GetAvg();
    double stdDev = statistics.GetStdDev();
    double min = statistics.GetMin();
    double max = statistics.GetMax();
    std::ofstream resultsFile("TimingTestResults.txt");
    resultsFile << cmnPrintf("Test: %s\n") << component->mtsTestTimingBase::GetName();
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

void mtsTimingTest::TestContinuous(const std::string & name,
                                   PriorityType threadPriority,
                                   osaCPUMask CPUAffinity)
{
    mtsTestTimingContinuous * task = new mtsTestTimingContinuous(name);

    task->SetIterations(1000);
    task->SetThreadPriority(threadPriority);
    task->SetCPUAffinity(CPUAffinity);

    TestExecution<mtsTestTimingContinuous>(task);
    delete task;
}

void mtsTimingTest::TestPeriodic(const std::string & name,
                                 PriorityType threadPriority,
                                 osaCPUMask CPUAffinity,
                                 RunBehavior runBehavior,
                                 double period,
                                 double load)
{
    mtsTestTimingPeriodic * task = new mtsTestTimingPeriodic(name, period);

    task->SetIterations(1000);
    task->SetLoad(load);
    task->SetThreadPriority(threadPriority);
    task->SetCPUAffinity(CPUAffinity);
    task->SetRunBehavior(runBehavior);

    TestExecution<mtsTestTimingPeriodic>(task);
    delete task;
}

void mtsTimingTest::TestContinuousPriorityVeryLowAffinity0(void)
{
     TestContinuous("ContinuousPriorityVeryLowAffinity0", PRIORITY_VERY_LOW, 0);
}
void mtsTimingTest::TestContinuousPriorityVeryLowAffinity1(void)
{
     TestContinuous("ContinuousPriorityVeryLowAffinity1", PRIORITY_VERY_LOW, 1);
}
void mtsTimingTest::TestContinuousPriorityLowAffinity0(void)
{
     TestContinuous("ContinuousPriorityLowAffinity0", PRIORITY_LOW, 0);
}
void mtsTimingTest::TestContinuousPriorityLowAffinity1(void)
{
     TestContinuous("ContinuousPriorityLowAffinity1", PRIORITY_LOW, 1);
}
void mtsTimingTest::TestContinuousPriorityNormalAffinity0(void)
{
     TestContinuous("ContinuousPriorityNormalAffinity0", PRIORITY_NORMAL, 0);
}
void mtsTimingTest::TestContinuousPriorityNormalAffinity1(void)
{
     TestContinuous("ContinuousPriorityNormalAffinity1", PRIORITY_NORMAL, 1);
}
void mtsTimingTest::TestContinuousPriorityHighAffinity0(void)
{
     TestContinuous("ContinuousPriorityHighAffinity0", PRIORITY_HIGH, 0);
}
void mtsTimingTest::TestContinuousPriorityHighAffinity1(void)
{
     TestContinuous("ContinuousPriorityHighAffinity1", PRIORITY_HIGH, 1);
}
void mtsTimingTest::TestContinuousPriorityVeryHighAffinity0(void)
{
     TestContinuous("ContinuousPriorityVeryHighAffinity0", PRIORITY_VERY_HIGH, 0);
}
void mtsTimingTest::TestContinuousPriorityVeryHighAffinity1(void)
{
     TestContinuous("ContinuousPriorityVeryHighAffinity1", PRIORITY_VERY_HIGH, 1);
}

void mtsTimingTest::TestPeriodicPriorityVeryLowAffinity0RunDummyComputation(void)
{
    TestPeriodic("PeriodicPriorityVeryLowAffinity0RunDummyComputation", PRIORITY_VERY_LOW, 0, DUMMY_COMPUTATION);
}
void mtsTimingTest::TestPeriodicPriorityVeryLowAffinity0RunOsaSleep(void)
{
    TestPeriodic("PeriodicPriorityVeryLowAffinity0RunOsaSleep", PRIORITY_VERY_LOW, 0, OSA_SLEEP);
}
void mtsTimingTest::TestPeriodicPriorityVeryLowAffinity0RunOsaThreadSleep(void)
{
    TestPeriodic("PeriodicPriorityVeryLowAffinity0RunOsaThreadSleep", PRIORITY_VERY_LOW, 0, OSA_THREAD_SLEEP);
}
void mtsTimingTest::TestPeriodicPriorityVeryLowAffinity1RunDummyComputation(void)
{
    TestPeriodic("PeriodicPriorityVeryLowAffinity1RunDummyComputation", PRIORITY_VERY_LOW, 1, DUMMY_COMPUTATION);
}
void mtsTimingTest::TestPeriodicPriorityVeryLowAffinity1RunOsaSleep(void)
{
    TestPeriodic("PeriodicPriorityVeryLowAffinity1RunOsaSleep", PRIORITY_VERY_LOW, 1, OSA_SLEEP);
}
void mtsTimingTest::TestPeriodicPriorityVeryLowAffinity1RunOsaThreadSleep(void)
{
    TestPeriodic("PeriodicPriorityVeryLowAffinity1RunOsaThreadSleep", PRIORITY_VERY_LOW, 1, OSA_THREAD_SLEEP);
}
void mtsTimingTest::TestPeriodicPriorityLowAffinity0RunDummyComputation(void)
{
    TestPeriodic("PeriodicPriorityLowAffinity0RunDummyComputation", PRIORITY_LOW, 0, DUMMY_COMPUTATION);
}
void mtsTimingTest::TestPeriodicPriorityLowAffinity0RunOsaSleep(void)
{
    TestPeriodic("PeriodicPriorityLowAffinity0RunOsaSleep", PRIORITY_LOW, 0, OSA_SLEEP);
}
void mtsTimingTest::TestPeriodicPriorityLowAffinity0RunOsaThreadSleep(void)
{
    TestPeriodic("PeriodicPriorityLowAffinity0RunOsaThreadSleep", PRIORITY_LOW, 0, OSA_THREAD_SLEEP);
}
void mtsTimingTest::TestPeriodicPriorityLowAffinity1RunDummyComputation(void)
{
    TestPeriodic("PeriodicPriorityLowAffinity1RunDummyComputation", PRIORITY_LOW, 1, DUMMY_COMPUTATION);
}
void mtsTimingTest::TestPeriodicPriorityLowAffinity1RunOsaSleep(void)
{
    TestPeriodic("PeriodicPriorityLowAffinity1RunOsaSleep", PRIORITY_LOW, 1, OSA_SLEEP);
}
void mtsTimingTest::TestPeriodicPriorityLowAffinity1RunOsaThreadSleep(void)
{
    TestPeriodic("PeriodicPriorityLowAffinity1RunOsaThreadSleep", PRIORITY_LOW, 1, OSA_THREAD_SLEEP);
}
void mtsTimingTest::TestPeriodicPriorityNormalAffinity0RunDummyComputation(void)
{
    TestPeriodic("PeriodicPriorityNormalAffinity0RunDummyComputation", PRIORITY_NORMAL, 0, DUMMY_COMPUTATION);
}
void mtsTimingTest::TestPeriodicPriorityNormalAffinity0RunOsaSleep(void)
{
    TestPeriodic("PeriodicPriorityNormalAffinity0RunOsaSleep", PRIORITY_NORMAL, 0, OSA_SLEEP);
}
void mtsTimingTest::TestPeriodicPriorityNormalAffinity0RunOsaThreadSleep(void)
{
    TestPeriodic("PeriodicPriorityNormalAffinity0RunOsaThreadSleep", PRIORITY_NORMAL, 0, OSA_THREAD_SLEEP);
}
void mtsTimingTest::TestPeriodicPriorityNormalAffinity1RunDummyComputation(void)
{
    TestPeriodic("PeriodicPriorityNormalAffinity1RunDummyComputation", PRIORITY_NORMAL, 1, DUMMY_COMPUTATION);
}
void mtsTimingTest::TestPeriodicPriorityNormalAffinity1RunOsaSleep(void)
{
    TestPeriodic("PeriodicPriorityNormalAffinity1RunOsaSleep", PRIORITY_NORMAL, 1, OSA_SLEEP);
}
void mtsTimingTest::TestPeriodicPriorityNormalAffinity1RunOsaThreadSleep(void)
{
    TestPeriodic("PeriodicPriorityNormalAffinity1RunOsaThreadSleep", PRIORITY_NORMAL, 1, OSA_THREAD_SLEEP);
}
void mtsTimingTest::TestPeriodicPriorityHighAffinity0RunDummyComputation(void)
{
    TestPeriodic("PeriodicPriorityHighAffinity0RunDummyComputation", PRIORITY_HIGH, 0, DUMMY_COMPUTATION);
}
void mtsTimingTest::TestPeriodicPriorityHighAffinity0RunOsaSleep(void)
{
    TestPeriodic("PeriodicPriorityHighAffinity0RunOsaSleep", PRIORITY_HIGH, 0, OSA_SLEEP);
}
void mtsTimingTest::TestPeriodicPriorityHighAffinity0RunOsaThreadSleep(void)
{
    TestPeriodic("PeriodicPriorityHighAffinity0RunOsaThreadSleep", PRIORITY_HIGH, 0, OSA_THREAD_SLEEP);
}
void mtsTimingTest::TestPeriodicPriorityHighAffinity1RunDummyComputation(void)
{
    TestPeriodic("PeriodicPriorityHighAffinity1RunDummyComputation", PRIORITY_HIGH, 1, DUMMY_COMPUTATION);
}
void mtsTimingTest::TestPeriodicPriorityHighAffinity1RunOsaSleep(void)
{
    TestPeriodic("PeriodicPriorityHighAffinity1RunOsaSleep", PRIORITY_HIGH, 1, OSA_SLEEP);
}
void mtsTimingTest::TestPeriodicPriorityHighAffinity1RunOsaThreadSleep(void)
{
    TestPeriodic("PeriodicPriorityHighAffinity1RunOsaThreadSleep", PRIORITY_HIGH, 1, OSA_THREAD_SLEEP);
}
void mtsTimingTest::TestPeriodicPriorityVeryHighAffinity0RunDummyComputation(void)
{
    TestPeriodic("PeriodicPriorityVeryHighAffinity0RunDummyComputation", PRIORITY_VERY_HIGH, 0, DUMMY_COMPUTATION);
}
void mtsTimingTest::TestPeriodicPriorityVeryHighAffinity0RunOsaSleep(void)
{
    TestPeriodic("PeriodicPriorityVeryHighAffinity0RunOsaSleep", PRIORITY_VERY_HIGH, 0, OSA_SLEEP);
}
void mtsTimingTest::TestPeriodicPriorityVeryHighAffinity0RunOsaThreadSleep(void)
{
    TestPeriodic("PeriodicPriorityVeryHighAffinity0RunOsaThreadSleep", PRIORITY_VERY_HIGH, 0, OSA_THREAD_SLEEP);
}
void mtsTimingTest::TestPeriodicPriorityVeryHighAffinity1RunDummyComputation(void)
{
    TestPeriodic("PeriodicPriorityVeryHighAffinity1RunDummyComputation", PRIORITY_VERY_HIGH, 1, DUMMY_COMPUTATION);
}
void mtsTimingTest::TestPeriodicPriorityVeryHighAffinity1RunOsaSleep(void)
{
    TestPeriodic("PeriodicPriorityVeryHighAffinity1RunOsaSleep", PRIORITY_VERY_HIGH, 1, OSA_SLEEP);
}
void mtsTimingTest::TestPeriodicPriorityVeryHighAffinity1RunOsaThreadSleep(void)
{
    TestPeriodic("PeriodicPriorityVeryHighAffinity1RunOsaThreadSleep", PRIORITY_VERY_HIGH, 1, OSA_THREAD_SLEEP);
}
