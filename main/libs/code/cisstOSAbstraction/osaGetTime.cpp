/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: osaGetTime.cpp 792 2009-09-01 20:39:02Z pkazanz1 $

  Author(s): Ankur Kapoor, Anton Deguet
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

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
#include <sys/time.h>
#include <unistd.h>
#endif // CISST_LINUX || CISST_DARWIN || CISST_SOLARIS

#if (CISST_OS == CISST_WINDOWS)
#include <windows.h>
#include <time.h>
#endif // CISST_WINDOWS


double osaGetTime(void) {
#if (CISST_OS == CISST_LINUX_RTAI)
    return rt_get_time_ns() * cmn_ns;
#endif // CISST_LINUX_RTAI
#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    return ((double) currentTime.tv_sec) + ((double)currentTime.tv_usec) / (1000.0 * 1000.0);
#endif // CISST_LINUX || CISST_DARWIN || CISST_SOLARIS
#if (CISST_OS == CISST_WINDOWS)
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
#endif // CISST_WINDOWS
}

void osaGetDateTimeString(std::string & str)
{
    time_t tim = time(0);
    tm * now = localtime(&tim);
    char buffer[50];
    sprintf(buffer, "%d%s%02d%s%02d%s%02d%s%02d%s%02d",
            now->tm_year + 1900, "-",
            now->tm_mon + 1, "-",
            now->tm_mday, "_",
            now->tm_hour, "-",
            now->tm_min, "-",
            now->tm_sec);
    str = buffer;
}
