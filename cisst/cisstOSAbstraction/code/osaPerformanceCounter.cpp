
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

#include <cisstOSAbstraction/osaTimeData.h>
#include <cisstOSAbstraction/osaPerformanceCounter.h>

void osaPerformanceCounter::Reset()
{
	osaTimeData newTimeData;
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
		osaTimeData now;
        now.Now();
		SetOrigin(now);
	}
}
void osaPerformanceCounter::Stop()
{
	isRunning = false;
	osaTimeData newEnd;
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
        osaTimeData End;
        End.Now();
    }
	osaTimeData diff = (End-Origin);
	osaTimeData offset(0,247818,true);//this is the experimentally determined overhead of getting elapsed time
	return diff-offset;
}
void osaPerformanceCounter::Delay(osaTimeData timeToDelay)
{

	struct timespec ts;
	ts.tv_sec = timeToDelay.GetSeconds();
	ts.tv_nsec = timeToDelay.GetNanoSeconds();
	clock_nanosleep(CLOCK_MONOTONIC,0,&ts,NULL);	
}


