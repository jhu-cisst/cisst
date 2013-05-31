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

osaDate::osaDate()
{
/*	timespec t;
	clock_gettime(CLOCK_REALTIME,&t);
	time_t tt = t.tv_sec;
	struct tm *timeinfo;
	timeinfo = localtime(&tt);
	YearMember =timeinfo->tm_year + 1900;
	MonthMember = timeinfo->tm_mon + 1;
	DayMember = timeinfo->tm_mday ;  // day of the month 1~31
	HourMember = timeinfo->tm_hour ; // hour since midnight
	MinuteMember = timeinfo->tm_min ;
	SecondMember = timeinfo->tm_sec;*/

	YearMember =0;
	MonthMember = 0;
	DayMember = 0 ;  // day of the month 1~31
	HourMember = 0 ; // hour since midnight
	MinuteMember = 0 ;
	SecondMember = 0;

    NanoSeconds = 0;
}

osaDate::osaDate(osaTimeData &t)
{
    From(t);
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

void osaDate::From(osaTimeData &timeData)
{
    osaTimeData zero(0.0);
    timeData.Normalize();
    
    try
    {
        if (timeData<zero)
            throw std::runtime_error("Can not create an osaDate from a negative osaTimeData");
    }
    catch(std::exception const& e)
    {
        std::cout << "Exception: " << e.what() << "\n";
    }


	struct timespec ts;
	ts.tv_sec = timeData.GetSeconds();
	ts.tv_nsec = timeData.GetNanoSeconds();
	struct tm *timeinfo;
	timeinfo = localtime(&ts.tv_sec);
	
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
