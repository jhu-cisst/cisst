/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ankur Kapoor, Peter Kazanzides
  Created on: 2004-04-30

  (C) Copyright 2004-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstCommon/cmnUnits.h>
#include <cisstOSAbstraction/osaThreadBuddy.h>
#include <cisstOSAbstraction/osaSleep.h>


void * mtsTaskPeriodic::RunInternal(void *data)
{
    if (ExecIn && ExecIn->GetConnectedInterface()) {
        CMN_LOG_CLASS_RUN_ERROR << "RunInternal for " << this->GetName()
                                << " called, even though task receives thread from "
                                << ExecIn->GetConnectedInterface()->GetComponent()->GetName() << std::endl;
        return 0;
    }

    if (this->State == mtsComponentState::INITIALIZING) {
        SaveThreadStartData(data);
        this->StartupInternal();
        if (CaptureThread) {
            return 0;
        }
    }

    // Use a local variable, currentState, because otherwise we can have a problem when this->State is
    // changed by another thread. Specifically, if the state is changed from READY to ACTIVE in between
    // these conditions, then both will evaluate to false.
    mtsComponentState currentState = this->State;
    while ((currentState == mtsComponentState::ACTIVE) || (currentState == mtsComponentState::READY)) {
        if (currentState == mtsComponentState::ACTIVE) {
            DoRunInternal();
            if (StateTable.GetToc() - StateTable.GetTic() > Period) {
                OverranPeriod = true;
            }
        }
        // Wait for remaining period also handles thread suspension
        ThreadBuddy.WaitForRemainingPeriod();
        currentState = this->State;
    }

    CMN_LOG_CLASS_RUN_VERBOSE << "RunInternal: End of task " << Name << std::endl;
    CleanupInternal();
    return this->ReturnValue;
}

void mtsTaskPeriodic::StartupInternal(void) {
    CMN_LOG_CLASS_INIT_VERBOSE << "Starting StartupInternal (periodic) for " << Name << std::endl;
    // user defined initialization, find commands from associated resource interfaces
    ThreadBuddy.Create(GetName().c_str(), AbsoluteTimePeriod); // convert to nano seconds

    // Call base class StartupInternal, which also calls user-supplied Startup.
    // If all goes well, this changes the state to READY.
    BaseType::StartupInternal();

    // allow no more stack allocation. allowing this will
    // break realtime performance.
    // lock all pages too
    //ThreadBuddy.LockStack();
    if (IsHardRealTime) {
        // Might as well wait for loop to start before going hard-real-time
        while ((this->State == mtsComponentState::READY) && !CaptureThread) {
            ThreadBuddy.WaitForRemainingPeriod();
        }
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
                                 bool isHardRealTime, unsigned int sizeStateTable,
                                 bool newThread):
    mtsTaskContinuous(name, sizeStateTable, newThread),
    ThreadBuddy(),
    Period(periodicityInSeconds),
    IsHardRealTime(isHardRealTime)
{
    AbsoluteTimePeriod.FromSeconds(periodicityInSeconds);
    CMN_ASSERT(GetPeriodicity() > 0);
}

mtsTaskPeriodic::mtsTaskPeriodic( const std::string & name,
                                  const osaAbsoluteTime & period,
                                  bool isHardRealTime,
                                  unsigned int sizeStateTable,
                                  bool newThread ):
    mtsTaskContinuous(name, sizeStateTable, newThread),
    ThreadBuddy(),
    Period(period.ToSeconds()),
    AbsoluteTimePeriod(period),
    IsHardRealTime(isHardRealTime)
{
    CMN_ASSERT(GetPeriodicity() > 0);
}

mtsTaskPeriodic::mtsTaskPeriodic(const mtsTaskPeriodicConstructorArg &arg):
    mtsTaskContinuous(arg.Name, arg.StateTableSize, true),
    ThreadBuddy(),
    Period(arg.Period),
    IsHardRealTime(arg.IsHardRealTime)
{
    AbsoluteTimePeriod.FromSeconds(arg.Period);
    CMN_ASSERT(GetPeriodicity() > 0);
}

mtsTaskPeriodic::~mtsTaskPeriodic() {
    CMN_LOG_CLASS_RUN_DEBUG << "mtsTaskPeriodic destructor: deleting task " << Name << std::endl;
    //If the task was waiting on a queue, i.e. semaphore, mailbox,
    //etc, it is removed from such a queue and messaging tasks
    //pending on its message queue are unblocked with an error return.

    Kill();
    // adeguet1, is this sleep still necessary?
    // Now, wait for 2 periods to see if it was killed
    // osaSleep(2.0 * this->PeriodInSeconds); // all expressed in seconds
}


/********************* Methods to change task state ******************/

void mtsTaskPeriodic::Suspend(void)
{
    if (this->State == mtsComponentState::ACTIVE) {
        BaseType::Suspend();
        ThreadBuddy.Suspend();
        CMN_LOG_CLASS_RUN_DEBUG << "Suspended task " << Name << std::endl;
    }
}


double mtsTaskPeriodic::GetPeriodicity(void) const
{
    return AbsoluteTimePeriod.ToSeconds();
}

/*! Return true if thread is periodic.  Currently, returns true if
  the thread was created with a period > 0. */
bool mtsTaskPeriodic::IsPeriodic(void) const
{
    return Period > 0.0;
}
