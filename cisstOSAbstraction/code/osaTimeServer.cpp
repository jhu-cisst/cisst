/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Peter Kazanzides
  Created on: 12-30-2008

  (C) Copyright 2008-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnAssert.h>
#include <cisstCommon/cmnLogger.h>
#include <cisstCommon/cmnUnits.h>

#include <cisstOSAbstraction/osaTimeServer.h>

#if (CISST_OS == CISST_LINUX_XENOMAI)
#include <native/timer.h>         // xenomai native timer service
#endif

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN)
#include <sys/time.h>
#include <unistd.h>
#endif // CISST_LINUX || CISST_DARWIN

#if (CISST_OS == CISST_WINDOWS)
#include <cmath>
#include <windows.h>

// FILETIME offset from 1/1/1601 to 1/1/1970
const ULONGLONG OSA_OFFSET_TO_1970 = 116444736000000000ULL;

// Number of 100 nsec per sec
const unsigned long OSA_100NSEC_PER_SEC = 10000000UL;
#endif // CISST_WINDOWS

#if (CISST_OS == CISST_QNX)
#include <time.h>
#endif // CISST_QNX

std::chrono::steady_clock::time_point SynchronizedMonotonicClock::last_steady_time_;
SynchronizedMonotonicClock::duration SynchronizedMonotonicClock::virtual_time_;
double SynchronizedMonotonicClock::rate_multiplier_ = 1.0;

// PKAZ: IMPORTANT NOTES:
//
//   1) In RTAI, calling clock_gettime or gettimeofday from hard-real-time tasks will switch them
//      to soft-real-time.  Thus, GetRelativeTime must call rt_get_time_ns. It seems that rt_get_time_ns
//      has significant drift with respect to clock_gettime, at least on some machines (such as the
//      Neuromate control computer) -- this must be fixed. Note that we are using the RT timer in
//      one-shot mode -- this is set in __os_init (osaThreadBuddy.cpp).
//
//   2) According to the documentation, clock_gettime is available on Solaris, but should be tested.
//
//   3) OS X (Mac) does not have clock_gettime, so gettimeofday is used instead.  Need to see
//      if there are any other (higher-resolution) options.
//
//   4) The struct timespec fields are tv_sec and tv_nsec. Perhaps in some versions they might
//      be named ts_sec and ts_nsec.
//
//   5) QNX provides gettimeofday() function only for the purpose of porting existing code.
//      In new code, clock_gettime() should be used instead, which has nanosecond-level resolution.
//      This number increases by some multiple of nanoseconds, based on the system clock's
//      resolution (MJUNG).


#if (CISST_OS == CISST_WINDOWS)
// This function is called if the performance counter exists (i.e., non-zero frequency).
// It synchronizes the performance counter with the result from GetSystemTimeAsFileTime.
void osaTimeServer::Synchronize(void)
{
    int i;
    LARGE_INTEGER counterPre, counterPost;
    const int NUM_SAMPLES = 31;
    ULARGE_INTEGER ftimes[NUM_SAMPLES];
    ULONGLONG counterAvg[NUM_SAMPLES];
    long counterDelta[NUM_SAMPLES];
    ULONGLONG offset[NUM_SAMPLES];

    // First, do a quick test to find the minimum latency between the two reads
    // of the performance counter.
    long min_delta = 10000L;   // a ridiculously high value
    for (i = 0; i < 10; i++) {
        QueryPerformanceCounter(&counterPre);
        GetSystemTimeAsFileTime((FILETIME *)&ftimes[0].u);
        QueryPerformanceCounter(&counterPost);
        long delta = (long)(counterPost.QuadPart-counterPre.QuadPart);
        if (delta < min_delta)
            min_delta = delta;
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "Synchronize: initial minimum counter difference = " << min_delta << std::endl;

    // Now, collect the data.  We only keep samples where the FILETIME has just changed
    // and where the delta is less than the threshold (currently, 3*min_delta).
    // Note that we don't use the first sample (ftimes[0]) in later computations.
    i = 1;
    long sumDelta = 0L;
    while (i < NUM_SAMPLES) {
        QueryPerformanceCounter(&counterPre);
        GetSystemTimeAsFileTime((FILETIME *)&ftimes[i].u);
        QueryPerformanceCounter(&counterPost);
        // If the file time has changed AND the time difference is less than the threshold
        // (currently, 3*min_delta), keep this sample by incrementing i.
        // It is only necessary to check whether the lower 32 bits have changed.
        if (ftimes[i].LowPart != ftimes[i-1].LowPart) {
            long delta = (long) (counterPost.QuadPart-counterPre.QuadPart);
            if (delta <= (3 * min_delta)) {
                counterAvg[i] = counterPre.QuadPart + (delta+1)/2;
                counterDelta[i] = delta;
                sumDelta += delta;
                i++;
            }
        }
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "Synchronize: average counter difference = " << ((double)sumDelta)/(NUM_SAMPLES-1) << std::endl;

    LONGLONG sumOffset = 0LL;
    // For each pair of (counter, ftime) samples, the offset is given by:
    //    offset = counter - CounterFrequency*(ftime-TimeOrigin)/OSA_100NSEC_PER_SEC
    // To improve accuracy, we compute an average value.
    for (i = 1; i < NUM_SAMPLES; i++) {
         ULONGLONG ftime_delta = ftimes[i].QuadPart - INTERNALS(TimeOrigin);
         // Note that overflow should not be a problem because ftime_delta should be relatively small.
         offset[i] = counterAvg[i] - (INTERNALS(CounterFrequency) * ftime_delta + OSA_100NSEC_PER_SEC / 2) / OSA_100NSEC_PER_SEC;
         sumOffset += offset[i];
         CMN_LOG_CLASS_INIT_VERBOSE << "Synchronize: data " << i << ": delta = " << counterDelta[i]
                                    << ", offset = " << offset[i] << std::endl;
    }

    // Compute the mean and standard deviation of the offset.
    LONGLONG meanOffset = (sumOffset + (NUM_SAMPLES-1)/2)/(NUM_SAMPLES-1);
    unsigned long stdDev = 0L;
    for (i = 1; i < NUM_SAMPLES; i++) {
        long dOffset = (long)(offset[i]-meanOffset);
        stdDev += (unsigned long)(dOffset*dOffset);
    }
    stdDev = static_cast<unsigned long>(sqrt(((double)stdDev)/(NUM_SAMPLES-2)));
    CMN_LOG_CLASS_INIT_VERBOSE << "Synchronize: mean = " << meanOffset << ", stdDev = " << stdDev << std::endl;

    // Try to improve the result by eliminating outliers (more than 1 standard deviation away)
    int numValid = 0;
    sumOffset = 0L;
    for (i = 1; i < NUM_SAMPLES; i++) {
        long dOffset = (long)(offset[i]-meanOffset);
        if (dOffset < 0) dOffset = -dOffset;
        if ((unsigned long)dOffset <= stdDev) {
            numValid++;
            sumOffset += offset[i];
        }
    }
    // If we got at least 10 data points within 1 standard deviation, recompute offset.
    if (numValid > 10) {
        meanOffset = (sumOffset + numValid/2)/numValid;
        CMN_LOG_CLASS_INIT_VERBOSE << "Synchronize: recomputing offset after removing outliers, number of samples = "
                                   << numValid << std::endl;
    }
    // Now, set the offset in the internal data structure.
    INTERNALS(CounterOrigin) = meanOffset;

    CMN_LOG_CLASS_INIT_VERBOSE << "Synchronize: synchronization offset = " << INTERNALS(CounterOrigin) << std::endl;
    CMN_LOG_CLASS_INIT_DEBUG << "Synchronize: checking result:" << std::endl;
    for (i = 1; i < NUM_SAMPLES; i++) {
        double tCounter = (counterAvg[i] - INTERNALS(CounterOrigin)) * INTERNALS(CounterResolution);
        double tFtime = (ftimes[i].QuadPart - INTERNALS(TimeOrigin))/(static_cast<double>(OSA_100NSEC_PER_SEC));
        CMN_LOG_CLASS_INIT_DEBUG << "    ftime = " << tFtime << ", counter = " << tCounter
                                 << "    (diff = " << (tFtime - tCounter)*1e6 << " usec)" << std::endl;
    }
}
#endif // CISST_WINDOWS

#if (CISST_OS == CISST_LINUX_XENOMAI)
// PK: although this synchronization seems to work, on some machines it seems that the CPU
//     time-stamp counter (TSC) is poorly calibrated and so the reading (from rt_get_time_ns)
//     will drift significantly with respect to the absolute time returned by clock_gettime.
void osaTimeServer::Synchronize(void)
{
    SRTIME counterPre, counterPost, counterAvg, timediff;
    struct timespec curTime;
    counterPre = rt_timer_tsc2ns( rt_timer_tsc() );
    int rc = clock_gettime(CLOCK_REALTIME, &curTime);
    counterPost = rt_timer_tsc2ns( rt_timer_tsc() );

    counterAvg = (counterPost + counterPre + 1)/2;
    if (rc == 0) {
        timediff = (curTime.tv_sec - INTERNALS(TimeOrigin).tv_sec)*1000000000LL +
                   (curTime.tv_nsec - INTERNALS(TimeOrigin).tv_nsec);
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "Synchronize: error return from clock_gettime" << std::endl;
        timediff = 0LL;
    }
    INTERNALS(CounterOrigin) = counterAvg - timediff;
    CMN_LOG_CLASS_INIT_VERBOSE << "Synchronize: counterAvg = " << counterAvg
                               << ", timediff = " << timediff << std::endl;
}
#endif // CISST_LINUX_XENOMAI

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_DARWIN)
void osaTimeServer::Synchronize(void)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Synchronize: no synchronization provided/required for this OS" << std::endl;
}
#endif


osaTimeServer::osaTimeServer() : epoch(SynchronizedMonotonicClock::now()) {}

void osaTimeServer::SetTimeOrigin()
{
    epoch = SynchronizedMonotonicClock::now();
}

void osaTimeServer::SetTimeOriginFrom(const osaTimeServer *other)
{
    epoch = other->epoch;
}

bool osaTimeServer::GetTimeOrigin(SynchronizedMonotonicClock::time_point& origin) const
{
    origin = epoch;
    return true;
}

SynchronizedMonotonicClock::duration osaTimeServer::GetRelativeTime(void) const
{
    return SynchronizedMonotonicClock::now() - epoch;
}

double osaTimeServer::GetRelativeTimeInSeconds(void) const {
    const auto elapsed = GetRelativeTime();
    using seconds_d = std::chrono::duration<double>;
    return std::chrono::duration_cast<seconds_d>(elapsed).count();
}

double osaTimeServer::GetAbsoluteTimeInSeconds(void) const {
    const auto elapsed = SynchronizedMonotonicClock::now().time_since_epoch();
    using seconds_d = std::chrono::duration<double>;
    return std::chrono::duration_cast<seconds_d>(elapsed).count();
}

SynchronizedMonotonicClock::time_point osaTimeServer::GetAbsoluteTime(void) const {
    return SynchronizedMonotonicClock::now();
}

double osaTimeServer::EstimateDrift(void) const
{
#if (CISST_OS == CISST_WINDOWS)
    if (INTERNALS_CONST(CounterFrequency)) {
        LARGE_INTEGER counterPre, counterPost;
        ULARGE_INTEGER oldTime, newTime;
        GetSystemTimeAsFileTime((FILETIME *)&oldTime.u);
        do {
            QueryPerformanceCounter(&counterPre);
            GetSystemTimeAsFileTime((FILETIME *)&newTime.u);
            QueryPerformanceCounter(&counterPost);
        } while (newTime.LowPart == oldTime.LowPart);
        long delta = (long)(counterPost.QuadPart-counterPre.QuadPart);
        ULONGLONG counterAvg = counterPre.QuadPart + delta/2;
        double answer1 = (counterAvg - INTERNALS_CONST(CounterOrigin))*INTERNALS_CONST(CounterResolution);
        double answer2 = (newTime.QuadPart - INTERNALS_CONST(TimeOrigin))/((double)OSA_100NSEC_PER_SEC);
        CMN_LOG_CLASS_INIT_VERBOSE << "EstimateDrift: T-system = " << answer2 << ", T-corrected = " << answer1
                                   << ", diff = " << (answer2-answer1)*1e6 << " usec, (overhead = " << delta << ")" << std::endl;
        return answer2-answer1;
    }
#endif // CISST_WINDOWS
    return 0.0;
}

SynchronizedMonotonicClock::time_point osaTimeServer::RelativeToAbsolute(SynchronizedMonotonicClock::duration relative) const
{
    return relative + epoch;
}

SynchronizedMonotonicClock::duration osaTimeServer::AbsoluteToRelative(SynchronizedMonotonicClock::time_point absolute) const
{
    return absolute - epoch;
}
