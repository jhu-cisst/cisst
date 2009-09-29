/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsTaskFromCallback.cpp 794 2009-09-01 21:43:56Z pkazanz1 $

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

void * mtsTaskFromCallback::RunInternal(void * CMN_UNUSED(data)) {
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
    	CMN_LOG_CLASS_INIT_VERBOSE << "RunInternal: end of task " << this->GetName() << std::endl;
        this->CleanupInternal();
    }
    // Make copy on stack before clearing inRunInternal
    void * ret = this->ReturnValue;
    inRunInternal = false;
    return ret;
}

void mtsTaskFromCallback::StartupInternal(void)
{
    if (!Thread.IsValid()) {
        CMN_LOG_CLASS_INIT_VERBOSE << "StartupInternal: initializing thread for callback task " << this->GetName() << std::endl;
        Thread.CreateFromCurrentThread();
    }
    BaseType::StartupInternal();
}

/********************* Methods to change task state ******************/

void mtsTaskFromCallback::Create(void * CMN_UNUSED(data))
{
    if (TaskState != CONSTRUCTED) {
        CMN_LOG_CLASS_INIT_ERROR << "Create: task " << this->GetName() << " cannot be created, state = "
                                 << GetTaskStateName() << std::endl;
        return;
    }
    ChangeState(INITIALIZING);
}

void mtsTaskFromCallback::Start(void)
{
    if (TaskState == INITIALIZING)
        WaitToStart(3.0);
    if (TaskState == READY) {
        CMN_LOG_CLASS_INIT_VERBOSE << "Start: starting task " << this->GetName() << std::endl;
        ChangeState(ACTIVE);
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "Start: could not start task " << this->GetName() << ", state = " << GetTaskStateName() << std::endl;
    }
}

void mtsTaskFromCallback::Suspend(void)
{
    if (TaskState == ACTIVE) {
        CMN_LOG_CLASS_RUN_VERBOSE << "Suspend: suspending task " << this->GetName() << std::endl;
        ChangeState(READY);
        CMN_LOG_CLASS_RUN_VERBOSE << "Suspend: suspended task " << this->GetName() << std::endl;
    }
}

