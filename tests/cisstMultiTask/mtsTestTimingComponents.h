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

#include <cisstMultiTask/mtsTaskContinuous.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>

enum RunBehavior {
    dummyComputation,
    osaSleep,
    osaThreadSleep
};

class mtsTestTimingBase {
    bool Done(void);
    void SetIterations(int iterations);
    void Run(void);
}

class mtsTestTimingContinuous: public mtsTestTimingBase, public mtsTaskContinuous
{
private:
    bool DoneRunning;
    int RunCount;

public:
    mtsTestTimingContinuous(int iterations,
                            RunBehavior runBehavior);

    bool Done(void);
    void SetIterations(int iterations);
    void Run(void);
};

class mtsTestTimingPeriodic: public mtsTestTimingBase, public mtsTaskPeriodic
{
private:
    bool DoneRunning;
    int DummyOperations;
    int Iterations;
    double Period;
    double Load;
    RunBehavior RunBehavior;
    osaThread * currentThread;
    int RunCount;

    void Cleanup(void);

public:
    mtsTestTimingPeriodic(int iterations,
                          double period,
                          double load,
                          RunBehavior runBehavior);
    bool Done(void);
    void SetIterations(int iterations);
    void SetPeriod(double period);
    void SetLoad(double load);
    void SetThreadPriority(PriorityType threadPriority);
    void SetCPUAffinity(osaCPUMask CPUAffinity);
    void SetRunBehavior(RunBehavior runBehavior);
    void Run(void);
};
