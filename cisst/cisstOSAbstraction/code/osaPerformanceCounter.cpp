#include "../osaPerformanceCounter.h"
#include "../osaTimeData.h"

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


