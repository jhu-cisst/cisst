/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: osaPerformanceCounter.cpp 3034 2013-05-31 09:53:36Z tkim60 $

  Author(s): Tae Soo Kim
  Created on: 2013-05-31

  (C) Copyright 2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstOSAbstraction/osaPerformanceCounter.h>

#if (CISST_OS == CISST_WINDOWS || CISST_OS == CISST_CYGWIN )
#include <windows.h>
#endif

osaPerformanceCounter::osaPerformanceCounter():
    HasHighPerformanceCounter(false),
    isRunning(false)
{
}

void osaPerformanceCounter::Reset(void)
{
	osaTimeData newTimeData = osaTimeNow(1);
	Origin = newTimeData;
	isRunning = false;
}

void osaPerformanceCounter::SetOrigin(osaTimeData & origin)
{
	Origin = origin;
}

void osaPerformanceCounter::Start(void)
{
	if (isRunning) {
		return;
    } 
	else {
        Origin = osaTimeNow(1);
        isRunning = true;
    }
}

void osaPerformanceCounter::Stop(void)
{
	isRunning = false;
	End = osaTimeNow(1);
}

bool osaPerformanceCounter::IsRunning(void)
{
	return isRunning;
}

osaTimeData osaPerformanceCounter::GetElapsedTime(void)
{
	if (isRunning) {
        End = osaTimeNow(1);
    }
	return (End - Origin);
}

void osaPerformanceCounter::Delay(osaTimeData timeToDelay)
{
#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI)
	struct timespec ts;
	ts.tv_sec = timeToDelay.Seconds();
	ts.tv_nsec = timeToDelay.NanoSeconds();
#if (CISST_OS == CISST_DARWIN) 
    nanosleep(&ts, NULL);
#else
	clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL);	
#endif // (CISST_DARWIN)
#else
	int delay = timeToDelay.GetSeconds() * 1000 + timeToDelay.GetNanoSeconds() / 1000000;  // there is alot of precision lost here. Must FIX
	Sleep(delay);
#endif
}
