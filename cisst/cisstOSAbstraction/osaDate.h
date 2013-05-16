
/*
  Author(s): Tae Soo Kim
  Created on: 2012-04-03

  (C) Copyright 2008-2011 Johns Hopkins University (JHU), All Rights Reserved.

*/

#ifndef osaDate_h
#define osaDate_h

#include <time.h>
#include <string>
#include "osaTimeData.h"

/*! \brief Implementation of a simple object to represent date
	\ingroup cisstOSAbstraction

	The osaDate provides an easy way to visualize and store date information
*/
class osaDate
{
public:
	/*! Default contructor*/
	osaDate()
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
	/*! Create an osaDate object from a given osaTimeData object 
		\param _otd osaTimeData object that will be converted to a osaDate object
	*/
	osaDate(osaTimeData t)
	{
		struct timespec ts;
		ts.tv_sec = t.getSeconds();
		ts.tv_nsec = t.getNSeconds();
		struct tm *timeinfo;
		timeinfo = localtime(&ts.tv_sec);
		
		year = timeinfo->tm_year + 1900 ;
		month = timeinfo->tm_mon +1;
		day = timeinfo->tm_mday;
		hour = timeinfo->tm_hour;
		minute = timeinfo->tm_min;
		second = timeinfo->tm_sec;
	}
	/*!Default destructor*/
	~osaDate(); 
	/*! Creates a std::string representation of this osaDate object.
	*/
	std::string toString();
private:
protected:
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
};

#endif
