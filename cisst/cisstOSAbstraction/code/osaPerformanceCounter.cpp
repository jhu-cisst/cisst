

/*
  $Id: osaPerformanceCounter.cpp 3034 2013-05-31 09:53:36Z tkim60 $

  Author(s):  Tae Soo Kim
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

void osaPerformanceCounter::Reset()
{
	osaTimeData newTimeData = osaTimeNow(1);
	Origin = newTimeData;
	isRunning = false;
}
void osaPerformanceCounter::SetOrigin(osaTimeData &origin)
{
	Origin = origin;
}
void osaPerformanceCounter::Start()
{
	if(isRunning)
		return ;
	else
	{
		osaTimeData now = osaTimeNow(1);
		SetOrigin(now);
        isRunning = true;
	}
}
void osaPerformanceCounter::Stop()
{
	isRunning = false;
	osaTimeData newEnd = osaTimeNow(1);
	End = newEnd;	
}
bool osaPerformanceCounter::IsRunning()
{
	return isRunning;
}
osaTimeData osaPerformanceCounter::GetElapsedTime()
{
	if(isRunning)
	{
        End = osaTimeNow(1);
    }
	osaTimeData diff = (End-Origin);
	return diff;
}
void osaPerformanceCounter::Delay(osaTimeData timeToDelay)
{
#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI)
	struct timespec ts;
	ts.tv_sec = timeToDelay.GetSeconds();
	ts.tv_nsec = timeToDelay.GetNanoSeconds();
	clock_nanosleep(CLOCK_MONOTONIC,0,&ts,NULL);	
#else
	int delay = timeToDelay.GetSeconds()*1000 + timeToDelay.GetNanoSeconds()/1000000;  // there is alot of precision lost here. Must FIX
	Sleep(delay);
#endif
}


