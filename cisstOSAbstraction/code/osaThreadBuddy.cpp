/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Ankur Kapoor, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <atomic>
#include <cstring>

#include <cisstCommon/cmnUnits.h>
#include <cisstCommon/cmnLogger.h>
#include <cisstOSAbstraction/osaThreadBuddy.h>

#if (CISST_OS == CISST_WINDOWS)

#include <windows.h>

#elif (CISST_OS == CISST_DARWIN)
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <mach/thread_policy.h>
#include <pthread.h>

namespace {
    inline uint64_t ns_to_mach_ticks(uint64_t ns) {
        static mach_timebase_info_data_t timebase = {0, 0};
        if (timebase.denom == 0) {
            mach_timebase_info(&timebase);
        }
        // Cast to __uint128_t to prevent overflow on long uptimes
        return static_cast<uint64_t>((static_cast<__uint128_t>(ns) * timebase.denom) / timebase.numer);
    }
}

#else // LINUX

#include <time.h>

constexpr int64_t NSEC_PER_SEC = 1'000'000'000LL;

void posix_timespec_add_ns(struct timespec& ts, uint64_t ns) {
    uint64_t total_nsec = ts.tv_nsec + ns;
    ts.tv_sec += total_nsec / NSEC_PER_SEC;
    ts.tv_nsec = total_nsec % NSEC_PER_SEC;
}

#endif



// PIMPL, encapsulates OS-specific code
class osaThreadBuddyInternals {
public:
#if (CISST_OS == CISST_WINDOWS)
    HANDLE WaitTimer = NULL;
    DWORD period_ms = 0;
#elif (CISST_OS == CISST_DARWIN)
    uint64_t due_time_ticks;
    uint64_t period_ticks = 0;
#else
    struct timespec due_time;
    int64_t period_ns = 0;
#endif

    osaThreadBuddyInternals();
    ~osaThreadBuddyInternals();

    /** Configure internal timer's period */
    void setTimerPeriod(double period_in_seconds);

    /** Wait for next timer period. Drift-free, due time is computed
        from last due time. This means if period is overrun slightly,
        next loop has shorter budget. */
    void waitForTimer();

    /* Call hen resuming loop after suspension*/
    void resume();
};

osaThreadBuddyInternals::osaThreadBuddyInternals() {
#if (CISST_OS == CISST_WINDOWS)
    WaitTimer = CreateWaitableTimerExW(
        NULL,
        NULL,
        CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, 
        TIMER_ALL_ACCESS
    );
    if (!WaitTimer) {
        CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
                          << "CreateWaitableTimerExW failed: "
                          << GetLastError()
                          << std::endl;
        return;
    }
#endif
}

osaThreadBuddyInternals::~osaThreadBuddyInternals() {
#if (CISST_OS == CISST_WINDOWS)
    if (WaitTimer != NULL) {
        CloseHandle(WaitTimer);
        WaitTimer = NULL;
    }
#endif
}

void osaThreadBuddyInternals::setTimerPeriod(double period_in_seconds) {
#if (CISST_OS == CISST_WINDOWS)
    LARGE_INTEGER DueTime;
    period_ms = static_cast<LONG>(period_in_seconds * 1000.0);
    // periodic timer will first fire one period from now
    DueTime.QuadPart = -static_cast<LONGLONG>(period_ms) * 10000LL;
    BOOL success = SetWaitableTimer(WaitTimer, &DueTime, (LONG)period_ms, NULL, NULL, FALSE);

    if (!success) {
        CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                          << "SetWaitableTimer failed: "
                          << GetLastError()
                          << std::endl;
        return;
    }
#elif (CISST_OS == CISST_DARWIN)
    period_ticks = ns_to_mach_ticks(period_in_seconds * 1e9);
    due_time_ticks = mach_absolute_time();
#else
    period_ns = static_cast<int64_t>(period_in_seconds * 1e9);
    clock_gettime(CLOCK_MONOTONIC, &due_time);
#endif
}

void osaThreadBuddyInternals::waitForTimer() {
#if (CISST_OS == CISST_WINDOWS)
    if (period_ms == 0) { return; }
    WaitForSingleObject(WaitTimer, INFINITE);
#elif (CISST_OS == CISST_DARWIN)
    if (period_ticks == 0) { return; }
    due_time_ticks += period_ticks;
    mach_wait_until(due_time_ticks);
#else
    posix_timespec_add_ns(due_time, period_ns);
    // Loop on EINTR
    while (clock_nanosleep(
        CLOCK_MONOTONIC,
        TIMER_ABSTIME, 
        &due_time,
        NULL
    ) == EINTR) {}
#endif
}

osaThreadBuddy::osaThreadBuddy(std::thread& thread)
    : internals(std::make_unique<osaThreadBuddyInternals>()),
      thread(thread)
{}

// Need to define here, once osaThreadBuddyInternals source is available
osaThreadBuddy::~osaThreadBuddy() = default;

void osaThreadBuddy::Create(double period)
{
    Period = period;
    internals->setTimerPeriod(period);
    IsSuspended = false;
}

void osaThreadBuddy::WaitForRemainingPeriod(void)
{
    // Check if period has been configured
    if (Period <= 0) { return; }

    do {
        internals->waitForTimer();
    } while (IsSuspended.load(std::memory_order_relaxed));
}

void osaThreadBuddy::MakeHardRealTime(void) 
{
#if (CISST_OS == CISST_LINUX)
    pthread_t native_handle = thread.native_handle();
    struct sched_param param;
    param.sched_priority = 80;

    int result = pthread_setschedparam(native_handle, SCHED_FIFO, &param);
    if (result != 0) {
        CMN_LOG_RUN_ERROR << "Failed to set thread to real-time priority: " 
                  << std::strerror(result) << " (Muwst run with sudo/CAP_SYS_NICE)\n";
    }
#else
    // Mach support for Mac OS?
#endif
}

void osaThreadBuddy::MakeSoftRealTime(void) 
{
#if (CISST_OS == CISST_WINDOWS)
    SetThreadPriority(thread.native_handle(), THREAD_PRIORITY_TIME_CRITICAL);
#elif (CISST_OS == CISST_LINUX)
    pthread_t native_handle = thread.native_handle();
    struct sched_param param;
    param.sched_priority = 50;

    int result = pthread_setschedparam(native_handle, SCHED_FIFO, &param);
    if (result != 0) {
        CMN_LOG_RUN_ERROR << "Failed to set thread to real-time priority: " 
                  << std::strerror(result) << " (Muwst run with sudo/CAP_SYS_NICE)\n";
    }
#else
    // Mach support for Mac OS?
#endif
}

void osaThreadBuddy::Resume(void) 
{
    internals->setTimerPeriod(Period);
    IsSuspended = false;
}

void osaThreadBuddy::Suspend(void) 
{
    IsSuspended = true;
}
