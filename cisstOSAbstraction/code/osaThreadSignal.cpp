/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Ankur Kapoor, Peter Kazanzides, Balazs Vagvolgyi, Anton Deguet,
             Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2012 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaThreadSignal.h>

#include <chrono>

#include <cisstCommon/cmnAssert.h>
#include <cisstCommon/cmnPortability.h>
#include <cisstOSAbstraction/osaThread.h>

osaThreadId osaThreadSignal::callback_thread_id;
void (*osaThreadSignal::pre_wait_callback)(void) = nullptr;
void (*osaThreadSignal::post_wait_callback)(void) = nullptr;

void osaThreadSignal::Raise()
{
    {
        std::unique_lock lock(state_mutex);
        state = true;
    }

    cond.notify_all();
}

void osaThreadSignal::Wait(void)
{
    Wait(10000.0);
}

bool osaThreadSignal::Wait(double timeoutInSec)
{
    bool do_callback = false;
    if (pre_wait_callback && post_wait_callback && (osaGetCurrentThreadId() == callback_thread_id)) {
        do_callback = true;
    }

    if (do_callback) {
        pre_wait_callback();
    }

    unsigned int millisec = (unsigned int)(timeoutInSec * 1000);

    bool success = false;
    {
        std::unique_lock lock(state_mutex);
        cond.wait_for(lock, std::chrono::milliseconds(millisec), [&](){ return this->state; });
        success = this->state;
    }

    if (do_callback) {
        post_wait_callback();
    }

    return success;
}

void osaThreadSignal::Clear() {
    {
        std::unique_lock lock(state_mutex);
        state = false;
    }
}

bool osaThreadSignal::raised() {
    {
        std::unique_lock lock(state_mutex);
        return state;
    }
}

void osaThreadSignal::ToStream(std::ostream & outputStream) const
{
    std::unique_lock lock(state_mutex);
    outputStream << "osaThreadSignal: state=" << state;
}

void osaThreadSignal::SetWaitCallbacks(const osaThreadId& thread_id, void (*pre)(void), void (*post)(void))
{
    callback_thread_id = thread_id;
    pre_wait_callback = pre;
    post_wait_callback = post;
}
