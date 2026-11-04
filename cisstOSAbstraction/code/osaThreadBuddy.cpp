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

#include <cisstCommon/cmnUnits.h>
#include <cisstCommon/cmnLogger.h>
#include <cisstOSAbstraction/osaThreadBuddy.h>
#include <cisstOSAbstraction/osaTimeServer.h>

#if (CISST_OS == CISST_WINDOWS)

#include <windows.h>

#else // POSIX

#include <sys/time.h>
#include <sys/select.h>
#include <unistd.h>

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
    if (Data->WaitTimer != NULL) {
        CloseHandle(Data->WaitTimer);
        Data->WaitTimer = NULL;
    }
#endif
}

void osaThreadBuddyInternals::setTimerPeriod(double period_in_seconds) {
#if (CISST_OS == CISST_WINDOWS)
    LARGE_INTEGER DueTime;
    DueTime.QuadPart = 0LL; // start timer immediately
    period_ms = static_cast<LONG>(period_in_seconds * 1000.0);
    BOOL success = SetWaitableTimer(WaitTimer, &dueTime, (LONG)period_ms, NULL, NULL, FALSE);

    if (!success) {
        CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                          << "SetWaitableTimer failed: "
                          << GetLastError()
                          << std::endl;
        return;
    }
#else
    period_ns = static_cast<int64_t>(period_in_seconds * 1e9);
    clock_gettime(CLOCK_MONOTONIC, &due_time);
#endif
}

void osaThreadBuddyInternals::waitForTimer() {
#if (CISST_OS == CISST_WINDOWS)
    if (period_ms == 0) { return; }
    WaitForSingleObject(Data->WaitTimer, INFINITE);
#else
    posix_timespec_add_ns(due_time, period_ns);
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &due_time, NULL);
#endif
}

osaThreadBuddy::osaThreadBuddy(std::thread& thread)
    : internals(std::make_unique<osaThreadBuddyInternals>()),
      thread(&thread)
{}

// Need to define here, once osaThreadBuddyInternals source is available
osaThreadBuddy::~osaThreadBuddy() = default;

void osaThreadBuddy::Create(double period)
{
    Period = period;
    internals->setTimerPeriod(period);
    IsSuspended = false;
}

void osaThreadBuddy::Create(const osaAbsoluteTime& tv)
{
    Period = tv.sec*1000000000 + tv.nsec;
    Create(Period);
}

void osaThreadBuddy::WaitForRemainingPeriod(void)
{
    if (!IsPeriodic()) { return; }

    do {
        internals->waitForTimer();
    } while (IsSuspended.load(std::memory_order_relaxed));
}

void osaThreadBuddy::MakeHardRealTime(void) 
{
#if (CISST_OS == CISST_LINUX)
    pthread_t native_handle = thread->native_handle();
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
    SetThreadPriority(thread->native_handle(), THREAD_PRIORITY_TIME_CRITICAL);
#elif (CISST_OS == CISST_LINUX)
    pthread_t native_handle = thread->native_handle();
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
    IsSuspended = false;
}

void osaThreadBuddy::Suspend(void) 
{
    IsSuspended = true;
}
