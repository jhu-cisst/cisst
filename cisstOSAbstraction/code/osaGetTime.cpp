/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Ankur Kapoor, Anton Deguet, Min Yang Jung
  Created on: 2004

  (C) Copyright 2004-2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnUnits.h>
#include <cisstCommon/cmnLogger.h>

#include <cisstOSAbstraction/osaGetTime.h>

#if (CISST_OS == CISST_LINUX_RTAI)
#include <rtai_lxrt.h>
#include <unistd.h>
#endif // CISST_LINUX_RTAI

#if (CISST_OS == CISST_LINUX_XENOMAI)
#include <native/timer.h>
#endif

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
#include <sys/time.h>
#include <unistd.h>
#endif // CISST_LINUX || CISST_DARWIN || CISST_SOLARIS

#if (CISST_OS == CISST_WINDOWS)
#include <windows.h>
#include <time.h>
#endif // CISST_WINDOWS

#if (CISST_OS == CISST_QNX)
#include <time.h>
#endif // CISST_QNX

double osaGetTime(void)
{
#if (CISST_OS == CISST_LINUX_RTAI)
    return rt_get_time_ns() * cmn_ns;

#elif (CISST_OS == CISST_LINUX_XENOMAI)

    return rt_timer_read() * cmn_ns;

#elif (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    return ((double) currentTime.tv_sec) + ((double)currentTime.tv_usec) * cmn_us;

#elif (CISST_OS == CISST_WINDOWS)
    LARGE_INTEGER liTimerFrequency, liTimeNow;
    double timerFrequency, time;
    if (QueryPerformanceCounter(&liTimeNow) ==0 )    {
        CMN_LOG_INIT_ERROR << "No performance counter available" << std::endl;
    }
    if (QueryPerformanceFrequency(&liTimerFrequency) ==    0) {
        CMN_LOG_INIT_ERROR << "No performance counter available" << std::endl;
    }
    timerFrequency = (double)liTimerFrequency.QuadPart;
    time = (double)liTimeNow.QuadPart/timerFrequency;
    return time;

#elif (CISST_OS == CISST_QNX)
    struct timespec currentTime;
    if (clock_gettime(CLOCK_REALTIME, &currentTime) == -1) {
        CMN_LOG_INIT_ERROR << "osaGetTime() failed" << std::endl;
        return 0;
    }
    return ((double)currentTime.tv_sec) + ((double)currentTime.tv_nsec) * cmn_ns;

#endif
}

void osaGetDateTimeString(std::string & str, const char delimiter)
{
    time_t tim = time(0);
    tm * now = localtime(&tim);
    char buffer[50];

    if (delimiter == ':') {
        sprintf(buffer, "%d-%02d-%02d %02d:%02d:%02d",
                now->tm_year + 1900,
                now->tm_mon + 1,
                now->tm_mday,
                now->tm_hour,
                now->tm_min,
                now->tm_sec);
    }  else {
        sprintf(buffer, "%d%c%02d%c%02d%c%02d%c%02d%c%02d",
                now->tm_year + 1900,
                delimiter,
                now->tm_mon + 1,
                delimiter,
                now->tm_mday,
                delimiter,
                now->tm_hour,
                delimiter,
                now->tm_min,
                delimiter,
                now->tm_sec);
    }
    str = buffer;
}

void osaGetDateTimeString(std::string & str, double time, const char delimiter){

    time_t tim = static_cast<time_t>(time);
    tm * now = localtime(&tim);
    char buffer[50];

    int ms = static_cast<int>( (double)( time - static_cast<int>(time)) * 1000);

    if (delimiter == ':') {
           sprintf(buffer, "%d-%02d-%02d %02d:%02d:%02d:%03d",
                   now->tm_year + 1900,
                   now->tm_mon + 1,
                   now->tm_mday,
                   now->tm_hour,
                   now->tm_min,
                   now->tm_sec,
                   ms);
       } else {
           sprintf(buffer, "%d%c%02d%c%02d%c%02d%c%02d%c%02d%c%03d",
                   now->tm_year + 1900,
                   delimiter,
                   now->tm_mon + 1,
                   delimiter,
                   now->tm_mday,
                   delimiter,
                   now->tm_hour,
                   delimiter,
                   now->tm_min,
                   delimiter,
                   now->tm_sec,
                   delimiter,
                   ms);
       }
       str = buffer;
}

void osaGetTimeString(std::string & str, double time, const char delimiter){

    time_t tim = static_cast<time_t>(time);
    tm * now = localtime(&tim);
    char buffer[50];

    int ms = static_cast<int>( (double)( time - static_cast<int>(time)) * 1000);
    sprintf(buffer, "%02d%c%02d%c%02d%c%03d",
                   now->tm_hour,
                   delimiter,
                   now->tm_min,
                   delimiter,
                   now->tm_sec,
                   delimiter,
                   ms);

   str = buffer;
}

void osaGetDateString(std::string & str, double time, const char delimiter){

    time_t tim = static_cast<time_t>(time);
    tm * now = localtime(&tim);
    char buffer[50];

    sprintf(buffer, "%d%c%02d%c%02d",
                      now->tm_year + 1900,
                      delimiter,
                      now->tm_mon + 1,
                      delimiter,
                      now->tm_mday);
          str = buffer;
}


void osaGetTimeString(std::string & str)
{
    time_t tim = time(0);
    tm * now = localtime(&tim);
    char buffer[50];
    sprintf(buffer, "%02d%s%02d%s%02d",
            now->tm_hour, ":",
            now->tm_min, ":",
            now->tm_sec);
    str = buffer;
}
