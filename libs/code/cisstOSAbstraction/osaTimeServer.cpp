/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Peter Kazanzides
  Created on: 12-30-2008

  (C) Copyright 2008-2009 Johns Hopkins University (JHU), All Rights Reserved.

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


// PKAZ: IMPORTANT NOTES:
//
//   1) For now, using Linux implementation for RTAI, but need to check if there is something
//      better available on RTAI.
//
//   2) According to the documentation, clock_gettime is available on Solaris, but should be tested.
//
//   3) OS X (Mac) does not have clock_gettime, so gettimeofday is used instead.  Need to see
//      if there are any other (higher-resolution) options.
//
//   4) The struct timespec fields are tv_sec and tv_nsec. Perhaps in some versions they might
//      be named ts_sec and ts_nsec.


struct osaTimeServerInternals {
#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI)
    struct timespec TimeOrigin;
#elif (CISST_OS == CISST_DARWIN) 
    struct timeval TimeOrigin;
#elif (CISST_OS == CISST_WINDOWS)
    ULONGLONG TimeOrigin;
    ULONGLONG CounterOrigin;
    unsigned long CounterFrequency;
    double CounterResolution;    // 1/CounterFrequency (resolution in seconds)
    void Synchronize(void);
#endif
};

#if (CISST_OS == CISST_WINDOWS)
// This function is called if the performance counter exists (i.e., non-zero frequency).
// It synchronizes the performance counter with the result from GetSystemTimeAsFileTime.
void osaTimeServerInternals::Synchronize(void)
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
    CMN_LOG_INIT_VERBOSE << "osaTimeServer: initial minimum counter difference = " << min_delta << std::endl;

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
            if (delta < 3*min_delta) {
                counterAvg[i] = counterPre.QuadPart + (delta+1)/2;
                counterDelta[i] = delta;
                sumDelta += delta;
                i++;
            }
        }
    }
    CMN_LOG_INIT_VERBOSE << "osaTimeServer: average counter difference = " << ((double)sumDelta)/(NUM_SAMPLES-1) << std::endl;

    LONGLONG sumOffset = 0LL;
    // For each pair of (counter, ftime) samples, the offset is given by:
    //    offset = counter - CounterFrequency*(ftime-TimeOrigin)/OSA_100NSEC_PER_SEC
    // To improve accuracy, we compute an average value.
    for (i = 1; i < NUM_SAMPLES; i++) {
         ULONGLONG ftime_delta = ftimes[i].QuadPart - TimeOrigin;
         // Note that overflow should not be a problem because ftime_delta should be relatively small.
         offset[i] = counterAvg[i] - (CounterFrequency*ftime_delta+OSA_100NSEC_PER_SEC/2)/OSA_100NSEC_PER_SEC;
         sumOffset += offset[i];
         CMN_LOG_INIT_VERBOSE << "Data " << i << ": delta = " << counterDelta[i]
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
    CMN_LOG_INIT_VERBOSE << "osaTimeServer: mean = " << meanOffset << ", stdDev = " << stdDev << std::endl;

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
        CMN_LOG_INIT_VERBOSE << "osaTimeServer: recomputing offset after removing outliers, number of samples = "
                             << numValid << std::endl;
    }
    // Now, set the offset in the internal data structure.
    CounterOrigin = meanOffset;

    CMN_LOG_INIT_VERBOSE << "osaTimeServer: Synchronization offset = " << CounterOrigin << std::endl;
    CMN_LOG_INIT_DEBUG << "osaTimeServer: Checking result:" << std::endl;
    for (i = 1; i < NUM_SAMPLES; i++) {
        double tCounter = (counterAvg[i] - CounterOrigin)*CounterResolution;
        double tFtime = (ftimes[i].QuadPart - TimeOrigin)/((double)OSA_100NSEC_PER_SEC);
        CMN_LOG_INIT_DEBUG << "  ftime = " << tFtime << ", counter = " << tCounter
                           << " (diff = " << (tFtime - tCounter)*1e6 << " usec)" << std::endl;
    }
}
#endif // CISST_WINDOWS

#define INTERNALS(A) (reinterpret_cast<osaTimeServerInternals*>(Internals)->A)

#define INTERNALS_CONST(A) (reinterpret_cast<osaTimeServerInternals*>(const_cast<osaTimeServer *>(this)->Internals)->A)

osaTimeServer::osaTimeServer()
{
    CMN_ASSERT(sizeof(Internals) >= SizeOfInternals());
#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI)
    struct timespec ts;
    clock_getres(CLOCK_REALTIME, &ts);
    CMN_LOG_INIT_VERBOSE << "osaTimeServer:  clock resolution is " << ts.tv_nsec << " nsec." << std::endl;
    if (ts.tv_sec)
        CMN_LOG_INIT_WARNING << "osaTimeServer:  WARNING, clock resolution in seconds is " << ts.tv_sec << " sec." << std::endl;
    INTERNALS(TimeOrigin).tv_sec = 0L;
    INTERNALS(TimeOrigin).tv_nsec = 0L;
#elif (CISST_OS == CISST_DARWIN)
    INTERNALS(TimeOrigin).tv_sec = 0L;
    INTERNALS(TimeOrigin).tv_usec = 0L;
#elif (CISST_OS == CISST_WINDOWS)
    INTERNALS(TimeOrigin) = 0LL;
    INTERNALS(CounterFrequency) = 0L;
    INTERNALS(CounterResolution) = 0.0;
    LARGE_INTEGER counterFrequency;
    if (QueryPerformanceFrequency(&counterFrequency) == 0) {
        CMN_LOG_INIT_ERROR << "osaTimeServer:  performance counter does not exist" << std::endl;
    }
    else {
        INTERNALS(CounterResolution) = 1.0/counterFrequency.QuadPart;
        CMN_LOG_INIT_VERBOSE << "osaTimeServer:  performance counter frequency = "
                             << counterFrequency.QuadPart << ", resolution = " 
                             << INTERNALS(CounterResolution)*1e9 << " nsec" << std::endl;
        // It is very unlikely that the frequency will ever be more than 32 bits. The CounterFrequency
        // variable could be changed to 64-bit, at the expense of computation time.
        if (counterFrequency.HighPart != 0) {
            CMN_LOG_INIT_ERROR << "osaTimeServer: frequency too high -- performance counter not used." << std::endl;
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
#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI)
    if (clock_gettime(CLOCK_REALTIME, &INTERNALS(TimeOrigin)) != 0)
        CMN_LOG_INIT_ERROR << "osaTimeServer::SetTimeOrigin: error return from clock_gettime" << std::endl;
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
        INTERNALS(Synchronize());
    }
#endif
}

bool osaTimeServer::GetTimeOrigin(osaAbsoluteTime & origin) const
{
#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI)
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
    origin.nsec = (long) ((tmp.LowPart)%OSA_100NSEC_PER_SEC)*100L;
#endif
    return true;
}

double osaTimeServer::GetRelativeTime(void) const
{
    double answer;
#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI)
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
        CMN_LOG_INIT_VERBOSE << "EstimateDrift:  T-system = " << answer2 << ", T-corrected = " << answer1
                             << ", diff = " << (answer2-answer1)*1e6 << " usec, (overhead = " << delta << ")" << std::endl;
        return answer2-answer1;
    }
#endif // CISST_WINDOWS
    return 0.0;
}

void osaTimeServer::RelativeToAbsolute(double relative, osaAbsoluteTime & absolute) const
{
    if (!GetTimeOrigin(absolute))
        CMN_LOG_INIT_ERROR << "osaTimeServer::RelativeToAbsolute: time origin not valid!" << std::endl;
    double delta_sec, delta_nsec;
    delta_nsec = modf(relative, &delta_sec)/cmn_ns;
    absolute.sec += (long) delta_sec;
    absolute.nsec += (long) delta_nsec;
}

double osaTimeServer::AbsoluteToRelative(const osaAbsoluteTime & absolute) const
{
    osaAbsoluteTime origin;
    if (!GetTimeOrigin(origin))
        CMN_LOG_INIT_ERROR << "osaTimeServer::AbsoluteToRelative: time origin not valid!" << std::endl;
    long delta_sec = absolute.sec - origin.sec;
    long delta_nsec = absolute.nsec - origin.nsec;
    if (delta_nsec < 0) {
        delta_sec--;
        delta_nsec += 1000000000L;
    }
    else if (delta_nsec > 1000000000L) {
        delta_sec++;
        delta_nsec -= 1000000000L;
    }
    double answer = delta_sec + delta_nsec*cmn_ns;

#if (CISST_OS == CISST_WINDOWS)
    // In Windows, there is another way to compute this. For now, we just use this as a check.
    ULONGLONG ftime = absolute.sec*OSA_100NSEC_PER_SEC + (absolute.nsec+50)/100;
    LONGLONG deltat = ftime-INTERNALS_CONST(TimeOrigin);  // could be negative
    double answer2 = deltat/((double)OSA_100NSEC_PER_SEC);

    CMN_LOG_INIT_VERBOSE << "osaTimeServer::AbsoluteToRelative: answers = " << answer << ", " << answer2 << std::endl;
#endif

    return answer;
}

