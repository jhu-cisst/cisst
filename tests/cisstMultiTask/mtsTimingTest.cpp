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

#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsIntervalStatistics.h>
#include <cisstMultiTask/mtsStateTable.h>
#include "mtsTimingTest.h"
#include "mtsTestComponents.h"


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
    while (!component->TestTiming->IsDone()) {
        osaSleep(15 * cmn_ms);
    }
    Manager->KillAll();
    CPPUNIT_ASSERT(Manager->WaitForStateAll(mtsComponentState::FINISHED, StateTransitionMaximumDelay));
    Manager->RemoveComponent(component);
    // Manager->Cleanup();
    std::cerr << "Finished test" << std::endl;

    // collect results, do some asserts
    const mtsStateTable * stateTable = component->GetDefaultStateTable();
    const mtsIntervalStatistics * statistics = &(stateTable->PeriodStats);
    std::cerr << "Component type: " << component->ClassServices()->GetName() << std::endl;
    std::cerr << "Average: " << statistics->GetAvg() << std::endl;
    std::cerr << "Stdev: " << statistics->GetStdDev() << std::endl;
    std::cerr << "Max: " << statistics->GetMax() << std::endl;
    std::cerr << "Min: " << statistics->GetMin() << std::endl;

    std::cerr << "temporary hack " << CMN_LOG_DETAILS << std::endl;
    Manager->RemoveComponent("LCM_MCC");
    Manager->RemoveComponent("MCS");
}


void mtsTimingTest::TestContinuous(void)
{
    mtsTestContinuous1<int> * task = new mtsTestContinuous1<int>();
    task->AddTestTiming();
    task->TestTiming->SetTotalNumberOfIterations(1000);
    task->TestTiming->SetPeriod(1.0 * cmn_ms);
    task->TestTiming->SetLoadRatio(0.5);
    task->TestTiming->SetRunComputation(mtsTestTiming::FunctionSleep);
    TestExecution(task);
    // delete task;
}


void mtsTimingTest::TestPeriodic(void)
{
    mtsTestPeriodic1<int> * task = new mtsTestPeriodic1<int>(1.0 * cmn_ms);
    task->AddTestTiming();
    task->TestTiming->SetTotalNumberOfIterations(1000);
    task->TestTiming->SetPeriod(1.0 * cmn_ms);
    task->TestTiming->SetLoadRatio(0.5);
    task->TestTiming->SetRunComputation(mtsTestTiming::FunctionSleep);
    // task->SetThreadPriority(0);
    // task->SetCPUAffinity(0);
    TestExecution(task);
    // delete task;
}
