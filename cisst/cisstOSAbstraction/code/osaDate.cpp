
/*
  $Id: osaDate.cpp 3034 2013-05-31 09:53:36Z tkim60 $

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

#include <cisstOSAbstraction/osaDate.h>

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI)
#include <sys/time.h>

#elif (CISST_OS == CISST_WINDOWS || CISST_OS == CISST_CYGWIN )
#include <cmath>
#include <windows.h>

#endif
osaDate::osaDate():
	YearMember(0),
	MonthMember(0),
	DayMember(0),   // day of the month 1~31
	HourMember(0), // hour since midnight
	MinuteMember(0),
	SecondMember(0),
    NanoSeconds(0)
{
}

osaDate::osaDate(osaTimeData &t)
{
    From(t);
}

osaDate::osaDate(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, unsigned int second, int_type nanoSecond):
	YearMember(year),
	MonthMember(month),
	DayMember(day),   
	HourMember(hour), // hour since midnight
	MinuteMember(minute),
	SecondMember(second),
    NanoSeconds(nanoSecond)
{
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
    ss<<".";
  
    unsigned int firstDigit = NanoSeconds/100000000;
    unsigned int secondDigit = (NanoSeconds - firstDigit*100000000)/10000000;
    unsigned int thirdDigit = (NanoSeconds - firstDigit*100000000-secondDigit*10000000)/1000000;
    ss<<firstDigit;
    ss<<secondDigit;
    ss<<thirdDigit;
	
	return ss.str();
}

void osaDate::From(osaTimeData &timeData)
{
    timeData.Normalize();
    
    try
    {
        if (!timeData.IsPositive())
            throw std::runtime_error("Can not create an osaDate from a negative osaTimeData");
    }
    catch(std::exception const& e)
    {
        std::cout << "Exception: " << e.what() << "\n";
    }


#if (CISST_OS == CISST_WINDOWS) || (CISST_OS == CISST_CYGWIN)
	timeval ts;
	ts.tv_sec = timeData.GetSeconds();
	ts.tv_usec = timeData.GetNanoSeconds()/1000;	
#else
	struct timespec ts;
	ts.tv_sec = timeData.GetSeconds();
	ts.tv_nsec = timeData.GetNanoSeconds();	
#endif

	
	struct tm *timeinfo;
	timeinfo = localtime((const time_t*)&ts.tv_sec);
	
	YearMember = timeinfo->tm_year + 1900 ;
	MonthMember = timeinfo->tm_mon +1;
	DayMember = timeinfo->tm_mday;
	HourMember = timeinfo->tm_hour;
	MinuteMember = timeinfo->tm_min;
	SecondMember = timeinfo->tm_sec;
    NanoSeconds = timeData.GetNanoSeconds();
}

void osaDate::To(osaTimeData &timeData) const
{
    time_t seconds;
    struct tm time_info ;

    time_info.tm_year = YearMember - 1900;
    time_info.tm_mon = MonthMember - 1;
    time_info.tm_mday = DayMember;
    time_info.tm_hour = HourMember;
    time_info.tm_min = MinuteMember;
    time_info.tm_sec = SecondMember;

    seconds = mktime(&time_info);
    timeData.SetSeconds(seconds);
    timeData.SetNanoSeconds(NanoSeconds);
}

unsigned int& osaDate::GetYear(void)
{
    return YearMember;
}

const unsigned int& osaDate::GetYear(void) const
{
    return YearMember;
}

unsigned int& osaDate::GetMonth(void)
{
    return MonthMember;
}

const unsigned int& osaDate::GetMonth(void) const
{
    return MonthMember;
}

unsigned int& osaDate::GetDay(void)
{
    return DayMember;
}
 
const unsigned int& osaDate::GetDay(void) const
{
    return DayMember;
}
 
unsigned int& osaDate::GetHour(void)
{
    return HourMember;
}

const unsigned int& osaDate::GetHour(void) const
{
    return HourMember;
}
   
unsigned int& osaDate::GetMinute(void)
{
    return MinuteMember;
}
 
const unsigned int& osaDate::GetMinute(void) const
{
    return MinuteMember;
}

unsigned int& osaDate::GetSecond(void)
{
    return SecondMember;
}

const unsigned int& osaDate::GetSecond(void) const
{
    return SecondMember;
}

osaDate osaDateNow(void)
{
/*	timespec t;
	clock_gettime(CLOCK_REALTIME,&t);
	time_t tt = t.tv_sec;
	struct tm *timeinfo;
	timeinfo = localtime(&tt);
    osaDate result(
	timeinfo->tm_year + 1900,
    timeinfo->tm_mon + 1,
	timeinfo->tm_mday,   // day of the month 1~31
	timeinfo->tm_hour, // hour since midnight
	timeinfo->tm_min,
	timeinfo->tm_sec,
    0);*/
    osaTimeData now = osaTimeNow();
    osaDate dateFrom(now);
    return dateFrom;
}
