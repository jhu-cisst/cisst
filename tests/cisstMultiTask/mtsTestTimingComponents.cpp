
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
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaCPUAffinity.h>
#include <math.h>
#include "mtsTestTimingComponents.h"

mtsTestTimingContinuous::mtsTestTimingContinuous(int iterations) :
    mtsTestTimingBase(); // call base constructor
    mtsTaskContinuous("mtsTestTimingContinuous"),
    DoneRunning(false),
    Iterations(iterations),
    {
    }

bool mtsTestTimingContinuous::Done(void)
{
    return DoneRunning;
}

void mtsTestTimingContinuous::SetIterations(int iterations)
{
    Iterations = iterations;
}

void mtsTestTimingContinuous::Run(void)
{
    if (RunCount >= Iterations) {
        DoneRunning = true;
        return;
    }
    RunCount++;
}

mtsTestTimingPeriodic::mtsTestTimingPeriodic() :
    mtsTaskPeriodic("mtsTestTimingPeriodic", period),
    DoneRunning(false),
    DummyOperations(1000)
{
    CurrentThread = new osaThread;
    CurrentThread.CreateFromCurrentThread();
    SetIterations(1000);
    SetPeriod(1.0);
    SetLoad(0.5);
    SetThreadPriority(0);
    SetCPUAffinity(0);
    SetRunBehavior(dummyComputation);
}

void mtsTestTimingPeridioc::Cleanup(void)
{
    delete CurrentThread;
}

bool mtsTestTimingPeriodic::Done(void) {
    return DoneRunning;
}

void mtsTestTimingPeriodic::SetIterations(int iterations)
{
    Iterations = iterations;
}

void mtsTestTimingPeriodic::SetPeriod(double period)
{
    Period = period;
}
void mtsTestTimingPeriodic::SetLoad(double load)
{
    Load = load;
}
void mtsTestTimingPeriodic::SetThreadPriority(PriorityType threadPriority)
{
    CurrentThread->setPriority(threadPriority);
}
void mtsTestTimingPeriodic::SetCPUAffinity(osaCPUMask CPUAffinity)
{
    osaCPUSetAffinity(CPUAffinity);
}
void mtsTestTimingPeriodic::SetRunBehavior(RunBehavior runBehavior)
{
    RunBehavior = runBehavior;
}

void mtsTestTimingPeriodic::Run(void) {
    if (RunCount >= Iterations) {
        DoneRunning = true;
        Cleanup();
        return;
    }
    RunCount++;

    switch (runBehavior) {
        case dummyComputation:
            const osaTimeServer & timeServer = mtsComponentManager::GetInstance()->GetTimeServer();
            int sum = 0;
            for (int i = 0; i < DummyOperations; i++) {
                sum += cos(i);
            }
            double timeElapsed = timeServer.getRelativeTime();
            // adjust operations to take load * period amount of time
            DummyOperations *= (Load * Period) / timeElapsed;
            break;
        case osaSleep:
            osaSleep(load * Period);
            break;
        case osaThreadSleep:
            CurrentThread.Sleep(load * Period);
            break;
    }
}
