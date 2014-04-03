/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Ankur Kapoor, Min Yang Jung
  Created on: 2004

  (C) Copyright 2004-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

// Implementations of osaSleep functions.  These were originally implemented as
// inline functions (in osaSleep.h), but were moved to this file to avoid OS dependencies
// in osaSleep.h. The extra function call (rather than inlining) will add a little delay
// to functions such as GetCurrentTime(), but this should be tolerable.

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnLogger.h>
#include <cisstOSAbstraction/osaSleep.h>

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    #include <sys/time.h>
    #include <unistd.h>
#elif (CISST_OS == CISST_LINUX_RTAI)
    #include <rtai_lxrt.h>
#elif (CISST_OS == CISST_LINUX_XENOMAI)

#include <native/task.h>
#include <native/timer.h>

#elif (CISST_OS == CISST_WINDOWS)
    #include <windows.h>
#elif (CISST_OS == CISST_QNX)
    #include <time.h>
#endif


const long nSecInSec =  1000 * 1000 * 1000;

void osaSleep(double timeInSeconds) 
{
#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)

    struct timespec ts;
    ts.tv_sec = static_cast<long> (timeInSeconds);
    ts.tv_nsec = static_cast<long> ( (timeInSeconds-ts.tv_sec) * nSecInSec );
    nanosleep(&ts, NULL);

#elif (CISST_OS == CISST_LINUX_RTAI)
    // check if this called by a real time task or not
    if (rt_is_hard_real_time(rt_buddy())) {
        rt_sleep(nano2count(static_cast<long> (timeInSeconds * nSecInSec)));
    } else {
        struct timespec ts;
        ts.tv_sec = static_cast<long> (timeInSeconds);
        ts.tv_nsec = static_cast<long> ( (timeInSeconds-ts.tv_sec) * nSecInSec );
        nanosleep(&ts, NULL);
    }

#elif (CISST_OS == CISST_LINUX_XENOMAI)

    if( rt_task_self() != NULL ){
        RTIME ns = RTIME( timeInSeconds * 1000000000 );
        int retval = 0;
        retval = rt_task_sleep( rt_timer_ns2ticks( ns ) );
        if( retval != 0 ){
            CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                              << "rt_task_sleep failed. "
                              << strerror(retval) << ": " << retval
                              << std::endl;
        }
    }
    else{
        struct timespec ts;
        ts.tv_sec = static_cast<long> (timeInSeconds);
        ts.tv_nsec = static_cast<long> ( (timeInSeconds-ts.tv_sec) * nSecInSec );
        nanosleep(&ts, NULL);
    }

#elif (CISST_OS == CISST_WINDOWS)
    // A waitable timer seems to be better than the Windows Sleep().
    HANDLE WaitTimer;
    LARGE_INTEGER dueTime;
    timeInSeconds *= -10.0 * 1000.0 * 1000.0;
    dueTime.QuadPart = static_cast<LONGLONG>(timeInSeconds); //dueTime is in 100ns
    // We don't name the timer (third parameter) because CreateWaitableTimer will fail if the name
    // matches an existing name (e.g., if two threads call osaSleep).
    WaitTimer = CreateWaitableTimer(NULL, true, NULL);
    SetWaitableTimer(WaitTimer, &dueTime, 0, NULL, NULL, 0);
    WaitForSingleObject(WaitTimer, INFINITE);
    CloseHandle(WaitTimer);

#elif (CISST_OS == CISST_QNX)
    struct timespec ts;
    _uint64 nsec = (_uint64) (timeInSeconds * 1000.0 * 1000.0 * 1000.0);
    nsec2timespec(&ts, nsec);
    nanosleep(&ts, NULL);
#endif
}

