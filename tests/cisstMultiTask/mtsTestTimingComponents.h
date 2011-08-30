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

#ifndef _mtsTestTimingComponents_h
#define _mtsTestTimingComponents_h

#include <cisstMultiTask/mtsTaskContinuous.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>

enum RunBehavior
{
    DUMMY_COMPUTATION,
    OSA_SLEEP,
    OSA_THREAD_SLEEP
};

class mtsTestTimingBase
{
    protected:
        std::string Name;
        bool DoneRunning;
        int Iterations;
        int RunCount;
        osaThread * CurrentThread;
        void Cleanup(void);

    public:
        mtsTestTimingBase(const std::string & name);
        ~mtsTestTimingBase(void);
        bool Done(void);
        std::string GetName(void);
        void SetIterations(int iterations);
        void SetThreadPriority(PriorityType threadPriority);
        void SetCPUAffinity(osaCPUMask CPUAffinity);
};

class mtsTestTimingContinuous: public mtsTestTimingBase, public mtsTaskContinuous
{
    public:
        mtsTestTimingContinuous(const std::string & name);
        void Run(void);
};

class mtsTestTimingPeriodic: public mtsTestTimingBase, public mtsTaskPeriodic
{
    private:
        int DummyOperations;
        double Load;
        RunBehavior RunType;

    public:
        mtsTestTimingPeriodic(const std::string & name, int period);

        double GetPeriod(void);
        void SetLoad(double load);
        void SetRunBehavior(RunBehavior runBehavior);
        void Run(void);
};

#endif // _mtsTestTimingComponents_h
