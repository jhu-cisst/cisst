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

#include <cisstOSAbstraction/osaSleep.h>

#include <cstdint>

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnLogger.h>

#if (CISST_OS == CISST_WINDOWS)
    #include <windows.h>
    #include <winnt.h>
    #include <synchapi.h>
#else
    #include <sys/time.h>
    #include <unistd.h>
#endif

constexpr int64_t NSEC_PER_SEC = 1'000'000'000LL;

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
    duration_in_seconds *= -10L * 1000L * 1000L; // Negative value to specify relative time
    due_time.QuadPart = static_cast<LONGLONG>(duration_in_seconds); // due_time is in 100ns
    BOOL success = SetWaitableTimer(timer, &due_time, (LONG)0, NULL, NULL, FALSE);
    if (!success) {
        CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                          << "SetWaitableTimer failed: "
                          << GetLastError()
                          << std::endl;
        CloseHandle(timer);
        return;
    }

    // No need to loop, will ignore signals
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
#else // POSIX
    const auto total_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::duration<double>(duration_in_seconds)
    );
    const auto secs = std::chrono::duration_cast<std::chrono::seconds>(total_ns);
    const auto nsecs = total_ns - secs;

    struct timespec req;
    req.tv_sec = static_cast<time_t>(secs.count());
    req.tv_nsec = static_cast<long>(nsecs.count());

    while (nanosleep(&req, &req) != 0) {
        if (errno != EINTR) {
            CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                            << "nanosleep failed with error: "
                            << errno
                            << std::endl;
            return;
        }
    }
#endif // CISST_OS != CISST_WINDOWS

}
