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


