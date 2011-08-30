
/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Martin Kelly
  Created on: 2011-07-02

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsTaskContinuous.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaCPUAffinity.h>
#include <math.h>
#include "mtsTestTimingComponents.h"

void mtsTestTimingBase::Cleanup(void)
{
    delete CurrentThread;
}

mtsTestTimingBase::mtsTestTimingBase(const std::string & name) :
    Name(name),
    DoneRunning(false),
    RunCount(0),
    CurrentThread(new osaThread)
{
}

mtsTestTimingBase::~mtsTestTimingBase(void)
{
    mtsTestTimingBase::Cleanup();
}

bool mtsTestTimingBase::Done(void)
{
    return DoneRunning;
}

std::string mtsTestTimingBase::GetName(void)
{
    return Name;
}

void mtsTestTimingBase::SetIterations(int iterations)
{
    Iterations = iterations;
}

void mtsTestTimingBase::SetThreadPriority(PriorityType threadPriority)
{
    CurrentThread->SetPriority(threadPriority);
}

void mtsTestTimingBase::SetCPUAffinity(osaCPUMask CPUAffinity)
{
    osaCPUSetAffinity(CPUAffinity);
}

mtsTestTimingContinuous::mtsTestTimingContinuous(const std::string & name) :
    mtsTestTimingBase(name), // call base constructor
    mtsTaskContinuous("mtsTestTimingContinuous")
{
    SetIterations(1000);
    SetThreadPriority(PRIORITY_NORMAL);
    SetCPUAffinity(0);
}

void mtsTestTimingContinuous::Run(void)
{
    if (RunCount == Iterations) {
        DoneRunning = true;
        mtsTestTimingBase::Cleanup();
        return;
    }
    RunCount++;
}

mtsTestTimingPeriodic::mtsTestTimingPeriodic(const std::string & name, int period) :
    mtsTestTimingBase(name),
    mtsTaskPeriodic("mtsTestTimingPeriodic", period),
    DummyOperations(1000)
{
    CurrentThread->CreateFromCurrentThread();
    SetIterations(1000);
    SetLoad(0.5);
    SetThreadPriority(PRIORITY_NORMAL);
    SetCPUAffinity(0);
    SetRunBehavior(DUMMY_COMPUTATION);
}

double mtsTestTimingPeriodic::GetPeriod(void)
{
    return GetPeriodicity();
}

void mtsTestTimingPeriodic::SetLoad(double load)
{
    Load = load;
}

void mtsTestTimingPeriodic::SetRunBehavior(RunBehavior runBehavior)
{
    RunType = runBehavior;
}

void mtsTestTimingPeriodic::Run(void) {
    if (RunCount == Iterations) {
        DoneRunning = true;
        mtsTestTimingBase::Cleanup();
        return;
    }
    RunCount++;

    double timeElapsed;
    int sum;
    switch (RunType) {
        case DUMMY_COMPUTATION:
            sum = 0;
            for (int i = 0; i < DummyOperations; i++) {
                sum += cos(i);
            }
            timeElapsed = mtsComponentManager::GetInstance()->GetTimeServer().GetRelativeTime();
            // adjust operations to take load * period amount of time
            DummyOperations *= (Load * Period) / timeElapsed;
            break;
        case OSA_SLEEP:
            osaSleep(Load * Period);
            break;
        case OSA_THREAD_SLEEP:
            CurrentThread->Sleep(Load * Period);
            break;
    }
}
