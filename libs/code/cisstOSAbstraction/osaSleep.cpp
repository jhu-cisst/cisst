/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX_XENOMAI)
    #include <sys/time.h>
    #include <unistd.h>
#elif (CISST_OS == CISST_WINDOWS)
    #include <windows.h>
#elif (CISST_OS == CISST_QNX)
    #include <time.h>
#endif


const long nSecInSec =  1000.0 * 1000.0 * 1000.0;

void osaSleep(double timeInSeconds) 
{
#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX_XENOMAI)

    struct timespec ts;
    ts.tv_sec = static_cast<long> (timeInSeconds);
    ts.tv_nsec = static_cast<long> ( (timeInSeconds-ts.tv_sec) * nSecInSec );
    nanosleep(&ts, NULL);

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

