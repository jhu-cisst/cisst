


/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: osaTimeData.cpp 3034 2013-05-31 09:53:36Z tkim60 $

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


#include <iomanip>
#include <cisstOSAbstraction/osaTimeData.h>
#include <cmath>
#include <iostream>


#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI)
#include <sys/time.h>

#elif (CISST_OS == CISST_WINDOWS || CISST_OS == CISST_CYGWIN )
#include <cmath>
#include <windows.h>

#endif


osaTimeData::osaTimeData(void):
    Seconds(0),
    NanoSeconds(0),
	Positive(true)
{
}

osaTimeData::osaTimeData(int_type seconds,int_type nseconds, bool positive_flag)
{
    Seconds = seconds;
    NanoSeconds = nseconds;
    Positive = positive_flag ; 
	Normalize();
//    struct timespec res;
//    clock_getres( CLOCK_REALTIME, &res);
//    Resolution = static_cast<long long>(res.tv_nsec);
}

osaTimeData::osaTimeData(double dseconds)
{
	bool flag = true;
	if(dseconds < 0 )
	{
		flag = false;
		dseconds = dseconds * -1;
	}
	int_type fractpart ;      
	SplitDoubles(dseconds, Seconds, fractpart);
	NanoSeconds = fractpart;
	Positive = flag;
	Normalize();
//	struct timespec res;
//	clock_getres(CLOCK_REALTIME, &res);
//	Resolution = static_cast<long long>(res.tv_nsec); 
}


void osaTimeData::SetTime(const osaTimeData &newTime)
{
	Seconds = newTime.Seconds;
	NanoSeconds = newTime.NanoSeconds;
	Positive = newTime.Positive;	
}

void osaTimeData::Normalize()
{
	while(NanoSeconds < 0)
	{
		NanoSeconds = NanoSeconds + 1000000000;
		Seconds--;
	}
	while(this->NanoSeconds >= 1000000000)
	{
		this->NanoSeconds = this->NanoSeconds - 1000000000;
		this->Seconds++;
	}	
	if(Seconds < 0 )
	{
		if(Positive) // if it was a positive time, make it negative
			Positive = false;
		else  // if it was a negative time, make it positive
			Positive = true;
		Seconds = Seconds * -1;
	}
}

bool osaTimeData::Equals(const osaTimeData &compareTo) const
{
	
	if(Seconds==compareTo.Seconds && NanoSeconds==compareTo.NanoSeconds && Positive == compareTo.Positive )
		return true;
	else
		return false;
}


double osaTimeData::ToSeconds()
{
	if(Positive)
		return Seconds+(NanoSeconds/1000000000.0);
	else
		return -1*(Seconds+NanoSeconds/1000000000.0);
}

osaTimeData osaTimeData::From(double doubleSeconds)
{
	bool flag = true;
	if(doubleSeconds < 0 )
	{
		flag = false;
		doubleSeconds = doubleSeconds * -1;
	}
	int_type fractpart , intpart ;      
	SplitDoubles(doubleSeconds,intpart,fractpart);
	return osaTimeData(intpart,fractpart,flag);
}

void osaTimeData::Add(const osaTimeData &rhs)
{
	Seconds = Seconds + rhs.Seconds;
	NanoSeconds = NanoSeconds + rhs.NanoSeconds;	
	
	Normalize();
}

void osaTimeData::Subtract(const osaTimeData &rhs)
{
	if( (!Positive && rhs.Positive ) ||  (Positive && !rhs.Positive)) // lhs is negative, rhs is positive
	{
		Seconds = Seconds + rhs.Seconds;
		NanoSeconds = NanoSeconds + rhs.NanoSeconds;
	}
	else if(Positive && rhs.Positive)
	{
		Seconds = Seconds - rhs.Seconds;
		NanoSeconds = NanoSeconds - rhs.NanoSeconds;
	}
	else // both negatives
	{
		if(Seconds < rhs.Seconds)
		{
			Seconds =rhs.Seconds - Seconds ;
			NanoSeconds = rhs.NanoSeconds - NanoSeconds ;
			Positive = true; // it is now positive
		}
		else
		{
			Seconds = Seconds - rhs.Seconds;
			NanoSeconds = NanoSeconds - rhs.NanoSeconds;
		}
	}
	Normalize();

}


void osaTimeData::SumOf(osaTimeData &first, osaTimeData &second)
{
	osaTimeData temp(first);
	temp.Add(second);
	Seconds = temp.Seconds;
	NanoSeconds = temp.NanoSeconds;
}

std::string osaTimeData::ToString() const
{	
	std::stringstream ss;
   	long long tempArray[9];
 
	if(!Positive)
		ss<<"-"; 
	ss<<Seconds<<".";

	long long temp =  NanoSeconds ;
	for(int i = 0 ; i < 9 ; i++)
	{
		tempArray[8-i] = (temp % 10 );
		temp = temp/10;
	}
	for(int i = 0 ; i < 9 ; i++)
	{
		ss<<tempArray[i];
	}

	return ss.str();
}

bool osaTimeData::IsPositive(void) const
{
    return Positive;
}

void osaTimeData::SetPositive(bool flag)
{
    Positive = flag ;
}

long long osaTimeData::GetSeconds() const
{
	return Seconds;
}

long long osaTimeData::GetNanoSeconds() const
{
	return NanoSeconds;
}

void osaTimeData::SetSeconds(long long s) 
{
	Seconds = s;	
}

void osaTimeData::SetNanoSeconds(long long s)
{
	NanoSeconds = s;
}
/*long long osaTimeData::GetResolution()
{
	return Resolution;
}*/

void osaTimeData::SplitDoubles(const double &seconds, int_type &fullSeconds, int_type &nanoSeconds)
{

	long long temp = static_cast<long long> ( seconds * 1000000000);
	long long computation=0;
	for(int i = 0 ; i < 9 ; i++)
	{
		computation = computation+  (temp % 10 ) * pow(10.0,i);
		temp = temp/10;
	}
	nanoSeconds = computation;
	fullSeconds= temp;
}

const osaTimeData osaTimeData::operator=(const osaTimeData &rhs)
{
	if(!(this->Equals(rhs))	)
	{
		SetTime(rhs);
	}
	return *this;
}

osaTimeData osaTimeData::operator+(const osaTimeData &rhs)
{
	Add(rhs);
	return *this;
}


osaTimeData osaTimeData::operator-(const osaTimeData &rhs)
{
	Subtract(rhs);
	return *this;	
}

bool osaTimeData::operator==(const osaTimeData &rhs) const
{
	return this->Equals(rhs);
}

bool osaTimeData::operator>(const osaTimeData &rhs) const
{
    if(this->IsPositive() && !rhs.IsPositive())
        return true;
    else if(!this->IsPositive() && rhs.IsPositive())
        return false;
    else if(!this->IsPositive() && !rhs.IsPositive())
    {
	    if(this->Seconds < rhs.Seconds)
	    	return true;
	    else if(this->Seconds == rhs.Seconds && this->NanoSeconds < rhs.NanoSeconds)
	    	return true;
    	else
	    	return false;
    }
    else
    {
	    if(this->Seconds > rhs.Seconds)
	    	return true;
	    else if(this->Seconds == rhs.Seconds && this->NanoSeconds > rhs.NanoSeconds)
	    	return true;
    	else
	    	return false;
    }
}
bool osaTimeData::operator<(const osaTimeData &rhs) const
{
    if(this->IsPositive() && !rhs.IsPositive())
        return false;
    else if(!this->IsPositive() && rhs.IsPositive())
        return true;
    else if(!this->IsPositive() && !rhs.IsPositive())
    {
	    if(this->Seconds > rhs.Seconds)
	    	return true;
	    else if(this->Seconds == rhs.Seconds && this->NanoSeconds > rhs.NanoSeconds)
	    	return true;
    	else
	    	return false;
    }
    else
    {
	    if(this->Seconds < rhs.Seconds)
    		return true;
    	else if(this->Seconds == rhs.Seconds && this->NanoSeconds < rhs.NanoSeconds)
    		return true;
    	else
    		return false;
    }
}
bool osaTimeData::operator>=(const osaTimeData &rhs) const
{
    if(this->IsPositive() && !rhs.IsPositive())
        return true;
    else if(!this->IsPositive() && rhs.IsPositive())
        return false;
    else if(!this->IsPositive() && !rhs.IsPositive())
    {
	    if(this->Seconds < rhs.Seconds)
	    	return true;
	    else if(this->Seconds == rhs.Seconds && this->NanoSeconds <= rhs.NanoSeconds)
	    	return true;
    	else
	    	return false;
    }
    else
    {
	    if(this->Seconds > rhs.Seconds)
	    	return true;
	    else if(this->Seconds == rhs.Seconds && this->NanoSeconds >= rhs.NanoSeconds)
	    	return true;
    	else
	    	return false;
    }

}
bool osaTimeData::operator<=(const osaTimeData &rhs) const
{
    if(this->IsPositive() && !rhs.IsPositive())
        return false;
    else if(!this->IsPositive() && rhs.IsPositive())
        return true;
    else if(!this->IsPositive() && !rhs.IsPositive())
    {
	    if(this->Seconds > rhs.Seconds)
	    	return true;
	    else if(this->Seconds == rhs.Seconds && this->NanoSeconds >= rhs.NanoSeconds)
	    	return true;
    	else
	    	return false;
    }
    else
    {
	    if(this->Seconds < rhs.Seconds)
    		return true;
    	else if(this->Seconds == rhs.Seconds && this->NanoSeconds <= rhs.NanoSeconds)
    		return true;
    	else
    		return false;
    }

}
osaTimeData osaTimeData::operator*(const double &rhs) 
{
	double me = ToSeconds();
	return From(me*rhs);	
}
osaTimeData osaTimeData::operator/(const double &rhs) 
{
	double me = ToSeconds();
	return From(me/rhs);	
}


/*returns the osaTimeData object of current time
\param indicator 0-real time, 1-monotonic*/
osaTimeData osaTimeNow(int indicator)
{
#if (CISST_OS == CISST_WINDOWS) || (CISST_OS == CISST_CYGWIN)
	timeval now;
	windows_gettime(&now);
	long long seconds = static_cast<long long> (now.tv_sec);
	long long nanoSeconds = static_cast<long long> (now.tv_usec); 
	return  osaTimeData(seconds,nanoSeconds);
#else
//	timespec res;
	timespec now;
    if(indicator==0)
    	clock_gettime(CLOCK_REALTIME, &now);
    else
    	clock_gettime(CLOCK_MONOTONIC, &now);
	long long seconds = static_cast<long long> (now.tv_sec);
	long long nanoSeconds = static_cast<long long> (now.tv_nsec); 
//	Resolution = static_cast<long long>(res.tv_nsec);
	return osaTimeData(seconds,nanoSeconds);
#endif
    
}

#if (CISST_OS == CISST_WINDOWS) || (CISST_OS == CISST_CYGWIN)
LARGE_INTEGER getFILETIMEoffset()
{
    SYSTEMTIME s;
    FILETIME f;
    LARGE_INTEGER t;

    s.wYear = 1970;
    s.wMonth = 1;
    s.wDay = 1;
    s.wHour = 0;
    s.wMinute = 0;
    s.wSecond = 0;
    s.wMilliseconds = 0;
    SystemTimeToFileTime(&s, &f);
    t.QuadPart = f.dwHighDateTime;
    t.QuadPart <<= 32;
    t.QuadPart |= f.dwLowDateTime;
    return (t);
}

void windows_gettime(struct timeval *tv)
{
    LARGE_INTEGER t;
    FILETIME f;
    double microseconds;
    static LARGE_INTEGER offset;
    static double frequencyToMicroseconds;
    static int initialized = 0;
    static BOOL usePerformanceCounter = 0;

    if (!initialized) 
    {
        LARGE_INTEGER performanceFrequency;
        initialized = 1;
        usePerformanceCounter = QueryPerformanceFrequency(&performanceFrequency);
        
        if (usePerformanceCounter) 
        {
            QueryPerformanceCounter(&offset);
            frequencyToMicroseconds = (double)performanceFrequency.QuadPart / 1000000.;
        } 
        else 
        {
            offset = getFILETIMEoffset();
            frequencyToMicroseconds = 10.;
                                                                                        
		}
                                         
	}                      
        if (usePerformanceCounter) 
            QueryPerformanceCounter(&t);
            
        else 
        {
            GetSystemTimeAsFileTime(&f);
            t.QuadPart |= f.dwLowDateTime;
            
        }
        t.QuadPart -= offset.QuadPart;
        microseconds = (double)t.QuadPart / frequencyToMicroseconds;
        t.QuadPart = microseconds;
        tv->tv_sec = t.QuadPart / 1000000;
        tv->tv_usec = t.QuadPart % 1000000;
        
        return ;
}

#endif


