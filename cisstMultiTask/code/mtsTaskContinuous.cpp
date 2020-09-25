/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Peter Kazanzides
  Created on: 2008-09-23

  (C) Copyright 2008-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsTaskContinuous.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstCommon/cmnUnits.h>


void * mtsTaskContinuous::RunInternal(void *data)
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
        if (CaptureThread)
            return 0;
    }

    // Use a local variable, currentState, because otherwise we can have a problem when this->State is
    // changed by another thread. Specifically, if the state is changed from READY to ACTIVE in between
    // these conditions, then both will evaluate to false.
    mtsComponentState currentState = this->State;
    while ((currentState == mtsComponentState::ACTIVE) || (currentState == mtsComponentState::READY)) {
        while (this->State == mtsComponentState::READY) {
            // Suspend the task until there is a call to Start().
            CMN_LOG_CLASS_INIT_VERBOSE << "RunInternal: " << this->GetName() << " Wait to start." << std::endl;
            WaitForWakeup();
        }
        if (this->State == mtsComponentState::ACTIVE)
            DoRunInternal();
        currentState = this->State;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "RunInternal: ending task " << this->GetName() << std::endl;
    mtsManagerLocal *LCM = mtsManagerLocal::GetInstance();
    if (this == LCM->GetCurrentMainTask())
        LCM->PopCurrentMainTask();

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

mtsTaskContinuous::mtsTaskContinuous(const mtsTaskContinuousConstructorArg &arg) :
    mtsTask(arg.Name, arg.StateTableSize),
    NewThread(arg.NewThread),
    CaptureThread(false)
{
}

mtsTaskContinuous::~mtsTaskContinuous() {
    CMN_LOG_CLASS_INIT_VERBOSE << "Deleting task " << this->GetName() << ", current state = " << this->State << std::endl;
    //If the task was waiting on a queue, i.e. semaphore, mailbox,
    //etc, it is removed from such a queue and messaging tasks
    //pending on its message queue are unblocked with an error return.

    if (!this->IsTerminated()) {
        Kill();
        WaitToTerminate(1.0 * cmn_s);   // Wait 1 second for it to terminate
        if (!IsTerminated()) {
            // If thread not dead, delete it.
            if (NewThread) {
                Thread.Delete();
                CMN_LOG_CLASS_INIT_VERBOSE << "mtsTaskContinuous destructor: Deleting thread for " << this->GetName() << std::endl;
            }
        }
    }
}


/********************* Methods to change task state ******************/

void mtsTaskContinuous::Create(void *data)
{
    if (this->State != mtsComponentState::CONSTRUCTED) {
        CMN_LOG_CLASS_INIT_VERBOSE << "Create: task " << this->GetName() << " cannot be created, state = "
                                   << this->State << std::endl;
        return;
    }
    if (ExecIn && ExecIn->GetConnectedInterface()) {
        CMN_LOG_CLASS_INIT_VERBOSE << "Create: getting thread from component "
                                   << ExecIn->GetConnectedInterface()->GetComponent()->GetName() << std::endl;
        ChangeState(mtsComponentState::INITIALIZING);
        // Special case handling: if Create was called from the source task, then we call StartupInternal now.
        // This case occurs when the source task uses the main thread.
        const mtsTask *srcTask = dynamic_cast<const mtsTask *>(ExecIn->GetConnectedInterface()->GetComponent());
        if (srcTask && srcTask->CheckForOwnThread()) {
            CMN_LOG_CLASS_INIT_VERBOSE << "Create: special case initialization from "
                                       << srcTask->GetName() << std::endl;
            Thread.CreateFromCurrentThread();
            StartupInternal();
        }
    }
    else {
        // NOTE: still need to update GCM
        RemoveInterfaceRequired("ExecIn", true);
        ExecIn = 0;
        if (NewThread) {
            CMN_LOG_CLASS_INIT_VERBOSE << "Create: creating thread for task " << this->GetName() << std::endl;
            ChangeState(mtsComponentState::INITIALIZING);
            Thread.Create<mtsTaskContinuous, void*>(this, &mtsTaskContinuous::RunInternal, data);
        }
        else {
            CMN_LOG_CLASS_INIT_VERBOSE << "Create: using current thread for task " << this->GetName() << std::endl;
            Thread.CreateFromCurrentThread();
            CaptureThread = true;
            ChangeState(mtsComponentState::INITIALIZING);
            RunInternal(data);
        }
    }
}


void mtsTaskContinuous::Start(void)
{
    if (this->State == mtsComponentState::INITIALIZING) {
        WaitToStart(this->InitializationDelay);
    }

    if (this->State == mtsComponentState::READY) {
        CMN_LOG_CLASS_INIT_VERBOSE << "Start: starting task " << this->GetName() << std::endl;
        ChangeState(mtsComponentState::ACTIVE);
        if (CaptureThread) {
            if (Thread.GetId() != osaGetCurrentThreadId()) {
                CMN_LOG_CLASS_INIT_ERROR << "Start: cannot start task " << this->GetName() << " (wrong thread)" << std::endl;
                return;
            }
            mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
            if (Thread.GetId() == LCM->GetMainThreadId())
                LCM->PushCurrentMainTask(this);
            CaptureThread = false;
            CMN_LOG_CLASS_INIT_VERBOSE << "Start: started task " << this->GetName() << " with current thread" << std::endl;
            RunInternal(ThreadStartData);
        }
        StartInternal();
        CMN_LOG_CLASS_INIT_VERBOSE << "Start: started task " << this->GetName() << std::endl;
    } else if (this->State == mtsComponentState::ACTIVE) {
        // NOP if task is already running
        return;
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "Start: could not start task " << this->GetName()
                                 << ", state = " << this->State
                                 << "(" << CMN_LOG_DETAILS << ")" << std::endl;
    }
}

void mtsTaskContinuous::Suspend(void)
{
    if (this->State == mtsComponentState::ACTIVE) {
        CMN_LOG_CLASS_RUN_VERBOSE << "Suspend: suspending task " << this->GetName() << std::endl;
        ChangeState(mtsComponentState::READY);
    }
}

void mtsTaskContinuous::Kill(void)
{
    mtsComponentState oldState = this->State;
    BaseType::Kill();
    // If task is suspended, we need to restart it so that it can respond to
    // the Kill request.
    if (oldState == mtsComponentState::READY) {
        StartInternal();
    }
}
