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

std::string osaDate::ToString()
{
	std::stringstream ss;
    
	ss<<year<<"/";
    if(month<10)
        ss<<"0";
    ss<<month<<"/";
    if(day<10)
        ss<<"0";
    ss<<day<<" ";
    if(hour<10)
        ss<<"0";
    ss<<hour<<":"; 
    if(minute<10)
        ss<<"0";
    ss<<minute<<":";
    if(second<10)
        ss<<"0";
    ss<<second;
	
	return ss.str();
}
