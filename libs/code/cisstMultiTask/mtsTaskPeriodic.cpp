/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides
  Created on: 2004-04-30

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstCommon/cmnUnits.h>
#include <cisstOSAbstraction/osaThreadBuddy.h>
#include <cisstOSAbstraction/osaSleep.h>


CMN_IMPLEMENT_SERVICES(mtsTaskPeriodic)

/********************* Methods that call user methods *****************/

void * mtsTaskPeriodic::RunInternal(void *data)
{
    if (TaskState == INITIALIZING) {
        SaveThreadStartData(data);
        this->StartupInternal();
        if (CaptureThread) {
            return 0;
        }
    }

	while ((TaskState == ACTIVE) || (TaskState == READY)) {
        if (TaskState == ACTIVE) {
            DoRunInternal();
            if (StateTable.GetToc() - StateTable.GetTic() > Period) OverranPeriod = true;
        }
        // Wait for remaining period also handles thread suspension
        ThreadBuddy.WaitForRemainingPeriod();
	}

	CMN_LOG_CLASS(7) << "End of task " << Name << std::endl;
	CleanupInternal();
	return this->retValue;
}

void mtsTaskPeriodic::StartupInternal(void) {
    CMN_LOG_CLASS(3) << "Starting StartupInternal (periodic) for " << Name << std::endl;
    // user defined initialization, find commands from associated resource interfaces 
    ThreadBuddy.Create(GetName().c_str(), cmnInternalTo_ns(Period)); // convert to nano seconds

    // Call base class StartupInternal, which also calls user-supplied Startup.
    BaseType::StartupInternal();

	// allow no more stack allocation. allowing this will
	// break realtime performance.
	// lock all pages too
	//ThreadBuddy.LockStack();
	if (IsHardRealTime) {
        // Might as well wait for loop to start before going hard-real-time
        while ((TaskState == READY) && !CaptureThread)
            ThreadBuddy.WaitForRemainingPeriod();
		ThreadBuddy.MakeHardRealTime();
	}
}


void mtsTaskPeriodic::CleanupInternal() {

	if (IsHardRealTime) {
		ThreadBuddy.MakeSoftRealTime();
	}
	ThreadBuddy.UnlockStack();

    //If the task was waiting on a queue, i.e. semaphore, mailbox,
    //etc, it is removed from such a queue and messaging tasks
    //pending on its message queue are unblocked with an error return.
	ThreadBuddy.Delete();

    BaseType::CleanupInternal();
}

void mtsTaskPeriodic::StartInternal(void)
{
    ThreadBuddy.Resume();
}

/********************* Task constructor and destructor *****************/

mtsTaskPeriodic::mtsTaskPeriodic(const std::string & name, double periodicityInSeconds,
                                 bool isHardRealTime, unsigned int sizeStateTable, bool newThread):
    mtsTaskContinuous(name, sizeStateTable, newThread),
    ThreadBuddy(),
	Period(periodicityInSeconds),
    IsHardRealTime(isHardRealTime)
{
    CMN_ASSERT(Period > 0);
}


mtsTaskPeriodic::~mtsTaskPeriodic() {
    CMN_LOG_CLASS(5) << "mtsTaskPeriodic destructor: deleting task " << Name << std::endl;
    //If the task was waiting on a queue, i.e. semaphore, mailbox,
    //etc, it is removed from such a queue and messaging tasks
    //pending on its message queue are unblocked with an error return.
    
    Kill();
    // Now, wait for 2 periods to see if it was killed
    osaSleep(2.0 * this->Period); // all expressed in seconds
}


/********************* Methods to change task state ******************/

void mtsTaskPeriodic::Suspend(void)
{
    if (TaskState == ACTIVE) {
        BaseType::Suspend();
        ThreadBuddy.Suspend();
        CMN_LOG_CLASS(5) << "Suspended task " << Name << std::endl;
    }
}

