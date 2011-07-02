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
    std::cerr << "Finished test" << std::endl;

    // collect results, do some asserts
    const mtsStateTable * stateTable = component.GetDefaultStateTable();
    mtsIntervalStatistics statistics = stateTable.periodStats();
    std::cerr << "Average: " << statistics.GetAvg() << std::endl;
    std::cerr << "Stdev: " << statistics.GetStdDev() << std::endl;
    std::cerr << "Max: " << statistics.GetMax() << std::endl;
    std::cerr << "Min: " << statistics.GetMin() << std::endl;
}

void mtsTimingTest::TestContinuous(void)
{
    mtsTestTimingContinuous * task = new mtsTestTimingContinuous();

    task->SetIterations(1000);

    TestExecution<mtsTestTimingContinuous>(task);
    delete task;
}

void mtsTimingTest::TestPeriodic(void)
{
    mtsTestTimingPeriodic * task = new mtsTestTimingPeriodic();

    SetIterations(1000);
    SetPeriod(1.0);
    SetLoad(0.5);
    SetThreadPriority(0);
    SetCPUAffinity(0);
    SetRunBehavior(dummyComputation);

    TestExecution<mtsTestTimingPeriodic>(task);
    delete task;
}
