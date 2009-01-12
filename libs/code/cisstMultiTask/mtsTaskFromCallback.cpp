/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsTaskFromCallback.cpp,v 1.3 2008/10/21 20:38:22 anton Exp $

  Author(s):  Peter Kazanzides
  Created on: 2008-09-18

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnPortability.h>
#include <cisstMultiTask/mtsTaskFromCallback.h>

CMN_IMPLEMENT_SERVICES(mtsTaskFromCallback)
CMN_IMPLEMENT_SERVICES(mtsTaskFromCallbackAdapter)

/********************* Methods that call user methods *****************/

// We assume that RunInternal can be called multiple times (re-entrant);
// this is possible for callbacks. Thus, we use the inRunInternal flag
// to make sure that re-entrancy is handled.

void * mtsTaskFromCallback::RunInternal(void *data) {
    if (inRunInternal) {
        if (TaskState == ACTIVE)
           this->OverranPeriod = true;
        return 0;
    }
    inRunInternal = true;
    if (TaskState == INITIALIZING)
        this->StartupInternal();

    if (TaskState == ACTIVE)
        DoRunInternal();

    if (TaskState == FINISHING) {
    	CMN_LOG_CLASS(7) << "End of task " << Name << std::endl;
        this->CleanupInternal();
    }
    // Make copy on stack before clearing inRunInternal
    void *ret = this->retValue;
    inRunInternal = false;
    return ret;
}

void mtsTaskFromCallback::StartupInternal(void)
{
    if (!Thread.IsValid()) {
        CMN_LOG_CLASS(5) << "Initializing thread for callback task " << Name << std::endl;
        Thread.CreateFromCurrentThread();
    }
    BaseType::StartupInternal();
}

/********************* Methods to change task state ******************/

void mtsTaskFromCallback::Create(void *data)
{
    if (TaskState != CONSTRUCTED) {
        CMN_LOG_CLASS(1) << "ERROR: task " << Name << " cannot be created, state = " <<
                GetTaskStateName() << std::endl;
        return;
    }
    StateChange.Lock();
    TaskState = INITIALIZING;
}

void mtsTaskFromCallback::Start(void)
{
    if (TaskState == INITIALIZING)
        WaitToStart(3.0);
    if (TaskState == READY) {
        CMN_LOG_CLASS(5) << "Starting task " << Name << std::endl;
        StateChange.Lock();
        TaskState = ACTIVE;
        StateChange.Unlock();
    }
    else
        CMN_LOG_CLASS(1) << "Could not start task " << Name << ", state = " << GetTaskStateName() << std::endl;
}

void mtsTaskFromCallback::Suspend(void)
{
    if (TaskState == ACTIVE) {
        CMN_LOG_CLASS(5) << "Suspending task " << Name << std::endl;
        StateChange.Lock();
        TaskState = READY;
        StateChange.Unlock();
        CMN_LOG_CLASS(5) << "Suspended task " << Name << std::endl;
    }
}

