
/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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
#include <iomanip>

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI)
#include <sys/time.h>

#elif (CISST_OS == CISST_WINDOWS || CISST_OS == CISST_CYGWIN )
#include <cmath>
#include <windows.h>

#endif
osaDate::osaDate():
    Years_(0),
    Months_(0),
    Days_(0),   // day of the month 1~31
    Hours_(0), // hour since midnight
    Minutes_(0),
    Seconds_(0),
    NanoSeconds_(0)
{
}

osaDate::osaDate(osaTimeData & timeData)
{
    From(timeData);
}

osaDate::osaDate(unsigned int years, unsigned int months, unsigned int days,
                 unsigned int hours, unsigned int minutes, unsigned int seconds, int_type nanoSeconds):
    Years_(years),
    Months_(months),
    Days_(days),
    Hours_(hours), // hour since midnight
    Minutes_(minutes),
    Seconds_(seconds),
    NanoSeconds_(nanoSeconds)
{
}

std::string osaDate::ToString(void) const
{
    std::stringstream ss;
    ss<<std::setfill('0') << std::setw(2);
    
    ss << Years_ <<"/"<<Months_<<"/"<<Days_<<" "<<Hours_<<":"<<Minutes_<<":"<<Seconds_<<".";

    ss<<NanoSeconds_/1000000;
   
    return ss.str();
}

void osaDate::From(osaTimeData & timeData)
{
    timeData.Normalize();

    if (!timeData.IsPositive()) {
        cmnThrow("Can not create an osaDate from a negative osaTimeData");
    }

	struct tm * timeinfo;

#if (CISST_OS == CISST_WINDOWS) || (CISST_OS == CISST_CYGWIN)
    //timespec ts;
    //ts.tv_sec = timeData.Seconds();
	time_t seconds = timeData.Seconds();
//	ts.tv_usec = timeData.NanoSeconds() / 1000;
   // ts.tv_usec = timeData.NanoSeconds() / 1000;
	timeinfo = localtime(&seconds);

#else
    struct timespec ts;
    ts.tv_sec = timeData.Seconds();
    ts.tv_nsec = timeData.NanoSeconds();
	timeinfo = localtime((const time_t*)&ts.tv_sec);

#endif


    //struct tm * timeinfo;
	//timeinfo = localtime((const time_t*)&ts.tv_sec);

    Years_ = timeinfo->tm_year + 1900 ;
    Months_ = timeinfo->tm_mon +1;
    Days_ = timeinfo->tm_mday;
    Hours_ = timeinfo->tm_hour;
    Minutes_ = timeinfo->tm_min;
    Seconds_ = timeinfo->tm_sec;
    NanoSeconds_ = timeData.NanoSeconds();
}

void osaDate::To(osaTimeData & timeData) const
{
    time_t seconds;
    struct tm time_info ;

    time_info.tm_year = Years_ - 1900;
    time_info.tm_mon = Months_ - 1;
    time_info.tm_mday = Days_;
    time_info.tm_hour = Hours_;
    time_info.tm_min = Minutes_;
    time_info.tm_sec = Seconds_;

    seconds = mktime(&time_info);
    timeData.SetSeconds(seconds);
    timeData.SetNanoSeconds(NanoSeconds_);
}

unsigned int & osaDate::Years(void)
{
    return Years_;
}

const unsigned int & osaDate::Years(void) const
{
    return Years_;
}

unsigned int & osaDate::Months(void)
{
    return Months_;
}

const unsigned int & osaDate::Months(void) const
{
    return Months_;
}

unsigned int & osaDate::Days(void)
{
    return Days_;
}

const unsigned int & osaDate::Days(void) const
{
    return Days_;
}

unsigned int & osaDate::Hours(void)
{
    return Hours_;
}

const unsigned int & osaDate::Hours(void) const
{
    return Hours_;
}

unsigned int & osaDate::Minutes(void)
{
    return Minutes_;
}

const unsigned int & osaDate::Minutes(void) const
{
    return Minutes_;
}

unsigned int & osaDate::Seconds(void)
{
    return Seconds_;
}

const unsigned int & osaDate::Seconds(void) const
{
    return Seconds_;
}

osaDate osaDateNow(void)
{
    /*   timespec t;
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
    osaTimeData now = osaTimeNow(0);
    osaDate dateFrom(now);
    return dateFrom;
}



