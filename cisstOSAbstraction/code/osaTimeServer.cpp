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

#if (CISST_OS == CISST_LINUX_RTAI)
#include <rtai_lxrt.h>
#include <unistd.h>
#endif // CISST_LINUX_RTAI

#if (CISST_OS == CISST_LINUX_XENOMAI)
#include <native/timer.h>         // xenomai native timer service
#endif

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI)
#include <sys/time.h>
#include <unistd.h>
#endif // CISST_LINUX || CISST_DARWIN || CISST_SOLARIS

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


struct osaTimeServerInternals {
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX_XENOMAI)
    struct timespec TimeOrigin;
    RTIME CounterOrigin;
#elif (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX)
    struct timespec TimeOrigin;
#elif (CISST_OS == CISST_DARWIN)
    struct timeval TimeOrigin;
#elif (CISST_OS == CISST_WINDOWS)
    ULONGLONG TimeOrigin;
    ULONGLONG CounterOrigin;
    unsigned long CounterFrequency;
    double CounterResolution;    // 1/CounterFrequency (resolution in seconds)
#endif
};

#define INTERNALS(A) (reinterpret_cast<osaTimeServerInternals*>((this)->Internals)->A)

#define INTERNALS_CONST(A) (reinterpret_cast<osaTimeServerInternals*>(const_cast<osaTimeServer *>(this)->Internals)->A)

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

#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX_XENOMAI)
// PK: although this synchronization seems to work, on some machines it seems that the CPU
//     time-stamp counter (TSC) is poorly calibrated and so the reading (from rt_get_time_ns)
//     will drift significantly with respect to the absolute time returned by clock_gettime.
void osaTimeServer::Synchronize(void)
{
#if (CISST_OS == CISST_LINUX_RTAI)
    RTIME counterPre, counterPost, counterAvg, timediff;
    struct timespec curTime;

    counterPre = rt_get_time_ns();
    int rc = clock_gettime(CLOCK_REALTIME, &curTime);
    counterPost = rt_get_time_ns();
#elif (CISST_OS == CISST_LINUX_XENOMAI)
    SRTIME counterPre, counterPost, counterAvg, timediff;
    struct timespec curTime;
    counterPre = rt_timer_tsc2ns( rt_timer_tsc() );
    int rc = clock_gettime(CLOCK_REALTIME, &curTime);
    counterPost = rt_timer_tsc2ns( rt_timer_tsc() );
#endif

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
#endif // CISST_LINUX_RTAI

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_DARWIN)
void osaTimeServer::Synchronize(void)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Synchronize: no synchronization provided/required for this OS" << std::endl;
}
#endif


osaTimeServer::osaTimeServer()
{
    CMN_ASSERT(sizeof(Internals) >= SizeOfInternals());
#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_QNX)
    struct timespec ts;
    clock_getres(CLOCK_REALTIME, &ts);
    CMN_LOG_CLASS_INIT_VERBOSE << "constructor: clock resolution is " << ts.tv_nsec << " nsec." << std::endl;
    if (ts.tv_sec) {
        CMN_LOG_CLASS_INIT_WARNING << "constructor: clock resolution in seconds is " << ts.tv_sec << " sec." << std::endl;
    }
    INTERNALS(TimeOrigin).tv_sec = 0L;
    INTERNALS(TimeOrigin).tv_nsec = 0L;
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX_XENOMAI)
    INTERNALS(CounterOrigin) = 0LL;
#endif
#elif (CISST_OS == CISST_DARWIN)
    INTERNALS(TimeOrigin).tv_sec = 0L;
    INTERNALS(TimeOrigin).tv_usec = 0L;
    CMN_LOG_CLASS_INIT_VERBOSE << "constructor: clock resolution not available on Darwin (clock_getres not supported)" << std::endl;
#elif (CISST_OS == CISST_WINDOWS)
    INTERNALS(TimeOrigin) = 0LL;
    INTERNALS(CounterFrequency) = 0L;
    INTERNALS(CounterResolution) = 0.0;
    LARGE_INTEGER counterFrequency;
    if (QueryPerformanceFrequency(&counterFrequency) == 0) {
        CMN_LOG_CLASS_INIT_ERROR << "constructor: performance counter does not exist." << std::endl;
    } else {
        INTERNALS(CounterResolution) = 1.0/counterFrequency.QuadPart;
        CMN_LOG_CLASS_INIT_VERBOSE << "constructor: performance counter frequency = "
                                   << counterFrequency.QuadPart << ", resolution = "
                                   << INTERNALS(CounterResolution)*1e9 << " nsec." << std::endl;
        // It is very unlikely that the frequency will ever be more than 32 bits. The CounterFrequency
        // variable could be changed to 64-bit, at the expense of computation time.
        if (counterFrequency.HighPart != 0) {
            CMN_LOG_CLASS_INIT_ERROR << "constructor: frequency too high -- performance counter not used." << std::endl;
        } else {
            INTERNALS(CounterFrequency) = counterFrequency.LowPart;
        }
    }
    // Compute offset between 1/1/1601 (Windows epoch) and 1/1/1970 (Unix epoch)
    // because our absolute time is based on the Unix epoch. We then check against our
    // hard-coded constant.
    SYSTEMTIME stime1970;
    ULARGE_INTEGER ftime1970;
    stime1970.wYear = 1970;
    stime1970.wMonth = 1;
    stime1970.wDayOfWeek = 4;
    stime1970.wDay = 1;
    stime1970.wHour = 0;
    stime1970.wMinute = 0;
    stime1970.wSecond = 0;
    stime1970.wMilliseconds = 0;
    SystemTimeToFileTime(&stime1970, (FILETIME *)&ftime1970.u);
    CMN_ASSERT(ftime1970.QuadPart == OSA_OFFSET_TO_1970);
#endif  // CISST_WINDOWS
}

osaTimeServer::~osaTimeServer()
{}

unsigned int osaTimeServer::SizeOfInternals(void) {
    return sizeof(osaTimeServerInternals);
}


void osaTimeServer::SetTimeOrigin(void)
{
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX_XENOMAI)
    if (clock_gettime(CLOCK_REALTIME, &INTERNALS(TimeOrigin)) == 0) {
        // On RTAI, synchronize rt_get_time_ns with clock_gettime
        this->Synchronize();
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "SetTimeOrigin: error return from clock_gettime." << std::endl;
    }
#elif (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX)
    if (clock_gettime(CLOCK_REALTIME, &INTERNALS(TimeOrigin)) != 0) {
        CMN_LOG_CLASS_INIT_ERROR << "SetTimeOrigin: error return from clock_gettime." << std::endl;
    }
#elif (CISST_OS == CISST_DARWIN)
    gettimeofday(&INTERNALS(TimeOrigin), NULL);
#elif (CISST_OS == CISST_WINDOWS)
    FILETIME currentTime;
    GetSystemTimeAsFileTime(&currentTime);
    ULARGE_INTEGER time;
    time.LowPart = currentTime.dwLowDateTime;
    time.HighPart = currentTime.dwHighDateTime;
    INTERNALS(TimeOrigin) = time.QuadPart;
    if (INTERNALS(CounterFrequency)) {
        // On Windows, we need to synchronize QueryPerformanceCounter (if it exists) with
        // GetSystemTimeAsFileTime
        this->Synchronize();
    }
#endif
}

void osaTimeServer::SetTimeOriginFrom(const osaTimeServer *other)
{
    CMN_LOG_RUN_VERBOSE << "osaTimeServer: setting time origin from existing time server" << std::endl;
    osaTimeServerInternals *thisInternals = reinterpret_cast<osaTimeServerInternals*>(this->Internals);
    const osaTimeServerInternals *otherInternals = reinterpret_cast<const osaTimeServerInternals*>(other->Internals);
    thisInternals->TimeOrigin = otherInternals->TimeOrigin;
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX_XENOMAI)
    thisInternals->CounterOrigin = otherInternals->CounterOrigin;
#elif (CISST_OS == CISST_WINDOWS)
    // The counter frequency and resolution should be set in the constructor
    if ((thisInternals->CounterFrequency != otherInternals->CounterFrequency) ||
        (thisInternals->CounterResolution != thisInternals->CounterResolution))
        CMN_LOG_RUN_WARNING << "osaTimeServer: mismatch in counter frequency or resolution" << std::endl;
    thisInternals->CounterOrigin = otherInternals->CounterOrigin;
#endif
}

bool osaTimeServer::GetTimeOrigin(osaAbsoluteTime & origin) const
{
#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_QNX)|| (CISST_OS == CISST_LINUX_XENOMAI)
    origin.sec = INTERNALS_CONST(TimeOrigin).tv_sec;
    origin.nsec = INTERNALS_CONST(TimeOrigin).tv_nsec;
    return (origin.sec != 0) || (origin.nsec != 0);
#elif (CISST_OS == CISST_DARWIN)
    origin.sec = INTERNALS_CONST(TimeOrigin).tv_sec;
    origin.nsec = INTERNALS_CONST(TimeOrigin).tv_usec*1000;
    return (origin.sec != 0) || (origin.nsec != 0);
#elif (CISST_OS == CISST_WINDOWS)
    if (!INTERNALS_CONST(TimeOrigin)) {
        origin.sec = 0L;
        origin.nsec = 0L;
        return false;
    }
    // Windows file time is a 64-bit value representing the number of 100-nanosecond
    // intervals since January 1, 1601.
    ULARGE_INTEGER tmp;
    tmp.QuadPart = INTERNALS_CONST(TimeOrigin) - OSA_OFFSET_TO_1970;
    origin.sec = (long) (tmp.QuadPart/OSA_100NSEC_PER_SEC);
    //origin.nsec = (long) ((tmp.LowPart)%OSA_100NSEC_PER_SEC)*100L;
    //Following line should be equivalent to the one above
    origin.nsec = (long) (tmp.QuadPart - origin.sec*OSA_100NSEC_PER_SEC)*100L;
#endif
    return true;
}

double osaTimeServer::GetRelativeTime(void) const
{
    double answer;
#if (CISST_OS == CISST_LINUX_RTAI)
    RTIME time = rt_get_time_ns();  // RTIME is long long (64 bits)
    answer = static_cast<double>(time-INTERNALS_CONST(CounterOrigin))*cmn_ns;
#elif (CISST_OS == CISST_LINUX_XENOMAI)
    RTIME time = rt_timer_tsc2ns( rt_timer_tsc() );
    answer = static_cast<double>(time-INTERNALS_CONST(CounterOrigin))*cmn_ns;
#elif (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX)
    struct timespec currentTime;
    clock_gettime(CLOCK_REALTIME, &currentTime);
    answer = (currentTime.tv_sec-INTERNALS_CONST(TimeOrigin).tv_sec) + (currentTime.tv_nsec-INTERNALS_CONST(TimeOrigin).tv_nsec)*cmn_ns;
#elif (CISST_OS == CISST_DARWIN)
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    answer = (currentTime.tv_sec-INTERNALS_CONST(TimeOrigin).tv_sec) + (currentTime.tv_usec-INTERNALS_CONST(TimeOrigin).tv_usec)*cmn_us;
#elif (CISST_OS == CISST_WINDOWS)
    if (INTERNALS_CONST(CounterFrequency)) {
        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);
        answer = (counter.QuadPart - INTERNALS_CONST(CounterOrigin))*INTERNALS_CONST(CounterResolution);
    }
    else {
        ULARGE_INTEGER currentTime;
        GetSystemTimeAsFileTime((FILETIME *)&currentTime.u);
        answer = (currentTime.QuadPart - INTERNALS_CONST(TimeOrigin))/((double)OSA_100NSEC_PER_SEC);
    }
#endif
    return answer;
}


double osaTimeServer::GetAbsoluteTimeInSeconds(void) const {

    return GetAbsoluteTime().ToSeconds();

}

osaAbsoluteTime osaTimeServer::GetAbsoluteTime(void) const {

    osaAbsoluteTime absTime;
    RelativeToAbsolute(GetRelativeTime(), absTime);
    return absTime;

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

void osaTimeServer::RelativeToAbsolute(double relative, osaAbsoluteTime & absolute) const
{
    if (!GetTimeOrigin(absolute)) {
        CMN_LOG_CLASS_RUN_ERROR << "RelativeToAbsolute: time origin not valid!" << std::endl;
    }
    double delta_sec, delta_nsec;
    delta_nsec = modf(relative, &delta_sec)/cmn_ns;
    absolute.sec += (long) delta_sec;
    absolute.nsec += (long) delta_nsec;
    if (absolute.nsec >= 1000000000L) {
        absolute.sec++;
        absolute.nsec -= 1000000000L;
    }
}

double osaTimeServer::AbsoluteToRelative(const osaAbsoluteTime & absolute) const
{
    osaAbsoluteTime origin;
    if (!GetTimeOrigin(origin)) {
        CMN_LOG_CLASS_RUN_ERROR << "AbsoluteToRelative: time origin not valid!" << std::endl;
    }
    long delta_sec = absolute.sec - origin.sec;
    long delta_nsec = absolute.nsec - origin.nsec;
    if (delta_nsec < 0) {
        delta_sec--;
        delta_nsec += 1000000000L;
    }
    else if (delta_nsec >= 1000000000L) {
        delta_sec++;
        delta_nsec -= 1000000000L;
    }
    double answer = delta_sec + delta_nsec*cmn_ns;

#if (CISST_OS == CISST_WINDOWS)
    // In Windows, there is another way to compute this. For now, we just use this as a check.
    ULONGLONG ftime = absolute.sec*OSA_100NSEC_PER_SEC + (absolute.nsec+50)/100;
    LONGLONG deltat = ftime-INTERNALS_CONST(TimeOrigin);  // could be negative
    double answer2 = deltat/((double)OSA_100NSEC_PER_SEC);

    CMN_LOG_CLASS_RUN_VERBOSE << "AbsoluteToRelative: answers = " << answer << ", " << answer2 << std::endl;
#endif

    return answer;
}

double osaAbsoluteTime::ToSeconds(void) const
{
    return static_cast<double>(sec) + static_cast<double>(nsec) * cmn_ns;
}

void osaAbsoluteTime::FromSeconds(double timeInSeconds)
{
    sec = static_cast<long>(floor(timeInSeconds));
    nsec = static_cast<long>((timeInSeconds - sec) * 1000000000); // nano seconds, 10^9
}

