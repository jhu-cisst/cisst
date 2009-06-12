/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Peter Kazanzides
  Created on: 2008-09-23

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsTaskContinuous.h>


CMN_IMPLEMENT_SERVICES(mtsTaskContinuous)

/********************* Methods that call user methods *****************/

void * mtsTaskContinuous::RunInternal(void *data)
{
    if (TaskState == INITIALIZING) {
        SaveThreadStartData(data);
        this->StartupInternal();
        if (CaptureThread)
            return 0;
    }

	while ((TaskState == ACTIVE) || (TaskState == READY)) {
        while (TaskState == READY) {
            // Suspend the task until there is a call to Start().
            CMN_LOG_CLASS_INIT_VERBOSE << Name << ": Wait to start." << std::endl;
            WaitForWakeup();
        }
        if (TaskState == ACTIVE)
            DoRunInternal();
	}

	CMN_LOG_CLASS_RUN_WARNING << "End of task " << Name << std::endl;
	CleanupInternal();
	return this->ReturnValue;
}

void mtsTaskContinuous::StartInternal(void)
{
    Thread.Wakeup();
}

/********************* Task constructor and destructor *****************/

mtsTaskContinuous::mtsTaskContinuous(const std::string & name, unsigned int sizeStateTable, bool newThread):
    mtsTask(name, sizeStateTable),
    NewThread(newThread),
    CaptureThread(false)
{
}


mtsTaskContinuous::~mtsTaskContinuous() {
    CMN_LOG_CLASS_RUN_WARNING << "Deleting task " << Name << ", current state = " << GetTaskStateName() << std::endl;
    //If the task was waiting on a queue, i.e. semaphore, mailbox,
    //etc, it is removed from such a queue and messaging tasks
    //pending on its message queue are unblocked with an error return.
    
    Kill();
    WaitToTerminate(1.0);   // Wait 1 second for it to terminate
    if (!IsTerminated()) {
        // If thread not dead, delete it.
        if (NewThread) {
            Thread.Delete();
            CMN_LOG_CLASS_RUN_ERROR << "mtsTaskContinuous destructor: Deleting thread" << std::endl;
        }
    }
}


/********************* Methods to change task state ******************/

void mtsTaskContinuous::Create(void *data)
{
    if (TaskState != CONSTRUCTED) {
        CMN_LOG_CLASS_INIT_ERROR << "task " << Name << " cannot be created, state = "
                                 << GetTaskStateName() << std::endl;
        return;
    }
    if (NewThread) {
	    CMN_LOG_CLASS_INIT_VERBOSE << "Creating thread for task " << Name << std::endl;
        // Lock the StateChange mutex and unlock it when the thread starts running (in RunInternal)
        StateChange.Lock();
        TaskState = INITIALIZING;
	    Thread.Create<mtsTaskContinuous, void*>(this, &mtsTaskContinuous::RunInternal, data);
    }
    else {
	    CMN_LOG_CLASS_INIT_VERBOSE << "Using current thread for task " << Name << std::endl;
        Thread.CreateFromCurrentThread();
        CaptureThread = true;
        StateChange.Lock();
        TaskState = INITIALIZING;
        RunInternal(data);
    }
}


void mtsTaskContinuous::Start(void)
{
    if (TaskState == INITIALIZING) {
        WaitToStart(3.0);   // 3 seconds
    }
    if (TaskState == READY) {
        CMN_LOG_CLASS_RUN_ERROR << "Starting task " << Name << std::endl;
        StateChange.Lock();
        TaskState = ACTIVE;
        StateChange.Unlock();
        if (CaptureThread) {
            if (Thread.GetId() != osaGetCurrentThreadId()) {
                CMN_LOG_CLASS_INIT_ERROR << "Cannot start task " << Name << " (wrong thread)" << std::endl;
                return;
            }
            CaptureThread = false;
            CMN_LOG_CLASS_RUN_ERROR << "Started task " << Name << " with current thread" << std::endl;
            RunInternal(ThreadStartData);
        }
        StartInternal();
        CMN_LOG_CLASS_RUN_ERROR << "Started task " << Name << std::endl;
    }
    else
        CMN_LOG_CLASS_INIT_ERROR << "Could not start task " << Name << ", state = " << GetTaskStateName() << std::endl;
}

void mtsTaskContinuous::Suspend(void)
{
    if (TaskState == ACTIVE) {
        CMN_LOG_CLASS_RUN_ERROR << "Suspending task " << Name << std::endl;
        StateChange.Lock();
        TaskState = READY;
        StateChange.Unlock();
    }
}

void mtsTaskContinuous::Kill(void)
{
    TaskStateType oldState = TaskState;
    BaseType::Kill();
    // If task is suspended, we need to restart it so that it can respond to
    // the Kill request.
    if (oldState == READY)
        StartInternal();
}

