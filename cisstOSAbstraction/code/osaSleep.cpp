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

#if (CISST_OS == CISST_WINDOWS)
    #include <windows.h>
#else
    #include <sys/time.h>
    #include <unistd.h>
#endif

const long nSecInSec = 1000L * 1000L * 1000L;

void osaSleep(double duration_in_seconds) 
{
#if (CISST_OS == CISST_WINDOWS)
    // A waitable timer seems to be better than the Windows Sleep().
    HANDLE timer = CreateWaitableTimerExW(
        NULL,
        NULL,
        CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, 
        TIMER_ALL_ACCESS
    );
    if (!timer) {
        CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                          << "CreateWaitableTimerExW failed: "
                          << GetLastError()
                          << std::endl;
        return;
    }

    LARGE_INTEGER due_time;
    timeInSeconds *= -10L * 1000L * 1000L; // Negative value to specify relative time
    dueTime.QuadPart = static_cast<LONGLONG>(duration_in_seconds); // dueTime is in 100ns
    BOOL success = SetWaitableTimer(timer, &dueTime, (LONG)0, NULL, NULL, FALSE);
    if (!success) {
        CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                          << "SetWaitableTimer failed: "
                          << GetLastError()
                          << std::endl;
        CloseHandle(timer);
        return;
    }

    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
#else
    struct timespec ts;
    ts.tv_sec = static_cast<long>(duration_in_seconds);
    ts.tv_nsec = static_cast<long>((duration_in_seconds-ts.tv_sec) * nSecInSec);
    nanosleep(&ts, NULL);
#endif

}
