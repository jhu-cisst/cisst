#include "../osaDate.h"

osaDate::osaDate()
{
	timespec t;
	clock_gettime(CLOCK_REALTIME,&t);
	time_t tt = t.tv_sec;
	struct tm *timeinfo;
	timeinfo = localtime(&tt);
	YearMember =timeinfo->tm_year + 1900;
	MonthMember = timeinfo->tm_mon + 1;
	DayMember = timeinfo->tm_mday ;  // day of the month 1~31
	HourMember = timeinfo->tm_hour ; // hour since midnight
	MinuteMember = timeinfo->tm_min ;
	SecondMember = timeinfo->tm_sec;

}

osaDate::osaDate(const osaTimeData &t)
{
	struct timespec ts;
	ts.tv_sec = t.GetSeconds();
	ts.tv_nsec = t.GetNanoSeconds();
	struct tm *timeinfo;
	timeinfo = localtime(&ts.tv_sec);
	
	YearMember = timeinfo->tm_year + 1900 ;
	MonthMember = timeinfo->tm_mon +1;
	DayMember = timeinfo->tm_mday;
	HourMember = timeinfo->tm_hour;
	MinuteMember = timeinfo->tm_min;
	SecondMember = timeinfo->tm_sec;
}

std::string osaDate::ToString() const
{
	std::stringstream ss;
    
	ss<<YearMember<<"/";
    if(MonthMember<10)
        ss<<"0";
    ss<<MonthMember<<"/";
    if(DayMember<10)
        ss<<"0";
    ss<<DayMember<<" ";
    if(HourMember<10)
        ss<<"0";
    ss<<HourMember<<":"; 
    if(MinuteMember<10)
        ss<<"0";
    ss<<MinuteMember<<":";
    if(SecondMember<10)
        ss<<"0";
    ss<<SecondMember;
	
	return ss.str();
}

int& osaDate::GetYear(void)
{
    return YearMember;
}

const int& osaDate::GetYear(void) const
{
    return YearMember;
}

int& osaDate::GetMonth(void)
{
    return MonthMember;
}

const int& osaDate::GetMonth(void) const
{
    return MonthMember;
}

int& osaDate::GetDay(void)
{
    return DayMember;
}
 
const int& osaDate::GetDay(void) const
{
    return DayMember;
}
 
int& osaDate::GetHour(void)
{
    return HourMember;
}

const int& osaDate::GetHour(void) const
{
    return HourMember;
}
   
int& osaDate::GetMinute(void)
{
    return MinuteMember;
}
 
const int& osaDate::GetMinute(void) const
{
    return MinuteMember;
}

int& osaDate::GetSecond(void)
{
    return SecondMember;
}

const int& osaDate::GetSecond(void) const
{
    return SecondMember;
}
