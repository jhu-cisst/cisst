#include "../osaPerformanceCounter.h"
#include "../osaTimeData.h"

void osaPerformanceCounter::Reset()
{
	osaTimeData newTimeData;
	origin = newTimeData;
	isRunning = false;
	
}
void osaPerformanceCounter::SetOrigin(osaTimeData &origin)
{
	this->origin = origin;
}
void osaPerformanceCounter::Start()
{
	if(isRunning)
		return ;
	else
	{
		osaTimeData now;
		SetOrigin(now);
	}
}
void osaPerformanceCounter::Stop()
{
	isRunning = false;
	osaTimeData newEnd;
	end = newEnd;	
}
bool osaPerformanceCounter::IsRunning()
{
	return isRunning;
}
osaTimeData osaPerformanceCounter::GetElapsedTime()
{
	if(isRunning)
		osaTimeData end;
	osaTimeData diff = (end-origin);
	osaTimeData offset(0,247818,true);//this is the experimentally determined overhead of getting elapsed time
	return diff-offset;
}
void osaPerformanceCounter::delay(osaTimeData timeToDelay)
{

	struct timespec ts;
	ts.tv_sec = timeToDelay.getSeconds();
	ts.tv_nsec = timeToDelay.getNSeconds();
	clock_nanosleep(CLOCK_MONOTONIC,0,&ts,NULL);	
}


