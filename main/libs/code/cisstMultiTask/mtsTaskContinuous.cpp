/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsTaskContinuous.cpp 794 2009-09-01 21:43:56Z pkazanz1 $

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
            CMN_LOG_CLASS_INIT_VERBOSE << "RunInternal: " << this->GetName() << " Wait to start." << std::endl;
            WaitForWakeup();
        }
        if (TaskState == ACTIVE)
            DoRunInternal();
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "RunInternal: ending task " << this->GetName() << std::endl;
    CleanupInternal();
    return this->ReturnValue;
}

void mtsTaskContinuous::StartInternal(void)
{
    Thread.Wakeup();
}

/********************* Task constructor and destructor *****************/

mtsTaskContinuous::mtsTaskContinuous(const std::string & name, 
                                     unsigned int sizeStateTable, 
                                     bool newThread):
    mtsTask(name, sizeStateTable),
    NewThread(newThread),
    CaptureThread(false)
{
}


mtsTaskContinuous::~mtsTaskContinuous() {
    CMN_LOG_CLASS_INIT_VERBOSE << "Deleting task " << this->GetName() << ", current state = " << GetTaskStateName() << std::endl;
    //If the task was waiting on a queue, i.e. semaphore, mailbox,
    //etc, it is removed from such a queue and messaging tasks
    //pending on its message queue are unblocked with an error return.
    
    Kill();
    WaitToTerminate(1.0);   // Wait 1 second for it to terminate
    if (!IsTerminated()) {
        // If thread not dead, delete it.
        if (NewThread) {
            Thread.Delete();
            CMN_LOG_CLASS_INIT_VERBOSE << "mtsTaskContinuous destructor: Deleting thread for " << this->GetName() << std::endl;
        }
    }
}


/********************* Methods to change task state ******************/

void mtsTaskContinuous::Create(void *data)
{
    if (TaskState != CONSTRUCTED) {
        CMN_LOG_CLASS_INIT_ERROR << "Create: task " << this->GetName() << " cannot be created, state = "
                                 << GetTaskStateName() << std::endl;
        return;
    }
    if (NewThread) {
	    CMN_LOG_CLASS_INIT_VERBOSE << "Create: creating thread for task " << this->GetName() << std::endl;
        ChangeState(INITIALIZING);
	    Thread.Create<mtsTaskContinuous, void*>(this, &mtsTaskContinuous::RunInternal, data);
    }
    else {
	    CMN_LOG_CLASS_INIT_VERBOSE << "Create: using current thread for task " << this->GetName() << std::endl;
        Thread.CreateFromCurrentThread();
        CaptureThread = true;
        ChangeState(INITIALIZING);
        RunInternal(data);
    }
}


void mtsTaskContinuous::Start(void)
{
    if (TaskState == INITIALIZING) {
        WaitToStart(3.0);   // 3 seconds
    }
    if (TaskState == READY) {
        CMN_LOG_CLASS_INIT_VERBOSE << "Start: starting task " << this->GetName() << std::endl;
        ChangeState(ACTIVE);
        if (CaptureThread) {
            if (Thread.GetId() != osaGetCurrentThreadId()) {
                CMN_LOG_CLASS_INIT_ERROR << "Start: cannot start task " << this->GetName() << " (wrong thread)" << std::endl;
                return;
            }
            CaptureThread = false;
            CMN_LOG_CLASS_INIT_VERBOSE << "Start: started task " << this->GetName() << " with current thread" << std::endl;
            RunInternal(ThreadStartData);
        }
        StartInternal();
        CMN_LOG_CLASS_INIT_VERBOSE << "Start: started task " << this->GetName() << std::endl;
    }
    else
        CMN_LOG_CLASS_INIT_ERROR << "Start: could not start task " << this->GetName() << ", state = " << GetTaskStateName() << std::endl;
}

void mtsTaskContinuous::Suspend(void)
{
    if (TaskState == ACTIVE) {
        CMN_LOG_CLASS_RUN_VERBOSE << "Suspend: suspending task " << this->GetName() << std::endl;
        ChangeState(READY);
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

