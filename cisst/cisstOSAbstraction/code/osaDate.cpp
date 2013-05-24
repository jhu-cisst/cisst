#include "../osaDate.h"

osaDate::osaDate()
{
	timespec t;
	clock_gettime(CLOCK_REALTIME,&t);
	time_t tt = t.tv_sec;
	struct tm *timeinfo;
	timeinfo = localtime(&tt);
	year =timeinfo->tm_year + 1900;
	month = timeinfo->tm_mon + 1;
	day = timeinfo->tm_mday ;  // day of the month 1~31
	hour = timeinfo->tm_hour ; // hour since midnight
	minute = timeinfo->tm_min ;
	second = timeinfo->tm_sec;

}

osaDate::osaDate(osaTimeData t)
{
	struct timespec ts;
	ts.tv_sec = t.GetSeconds();
	ts.tv_nsec = t.GetNanoSeconds();
	struct tm *timeinfo;
	timeinfo = localtime(&ts.tv_sec);
	
	year = timeinfo->tm_year + 1900 ;
	month = timeinfo->tm_mon +1;
	day = timeinfo->tm_mday;
	hour = timeinfo->tm_hour;
	minute = timeinfo->tm_min;
	second = timeinfo->tm_sec;
}

std::string osaDate::toString()
{
	std::stringstream ss;
    
	ss<<year<<"/"<<month<<"/"<<day<<" "<<hour<<":"<<minute<<":"<<second;

	
	return ss.str();
}
