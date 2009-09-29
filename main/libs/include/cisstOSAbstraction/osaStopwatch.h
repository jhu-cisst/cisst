/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: osaStopwatch.h 433 2009-06-09 22:10:24Z adeguet1 $

  Author(s):  Ofri Sadowsky
  Created on: 2005-02-17

  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _osaStopwatch_h
#define _osaStopwatch_h

#include <iostream>
#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnUnits.h>
#include <cisstCommon/cmnLogger.h>

#if CISST_OS_IS_WINDOWS || (CISST_OS == CISST_CYGWIN)
#include <Windows.h>
#elif (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
#include <sys/time.h>

#if (CISST_OS == CISST_SOLARIS) || (CISST_COMPILER == CISST_GCC) 
#ifndef timersub
#define timersub(a, b, res)                           \
  do {                                                \
    (res)->tv_sec = (a)->tv_sec - (b)->tv_sec;        \
    (res)->tv_usec = (a)->tv_usec - (b)->tv_usec;     \
    if ((res)->tv_usec < 0)                           \
      {                                               \
        (res)->tv_sec--;                              \
        (res)->tv_usec += 1000000;                    \
      }                                               \
      } while (0)
    #endif /*timersub*/
#endif
#endif // CISST_OS == CISST_SOLARIS


// Always include last
#include <cisstOSAbstraction/osaExport.h>

/*! Implement a simple stopwatch mechanism, which is helpful in
  profiling, debugging, and performance estimation.

  The osaStopwatch class uses system calls to obtain time at the
  finest granularity available in the system.  Note that a system call
  may sometimes invoke a context switch, and use them only when needed
  to interfere the least with the normal performance of the program.

  To reduce overhead (for more efficient time polling), all the
  methods are inlined.
*/
class osaStopwatch
{
public:
    typedef unsigned long MillisecondsCounter;
    typedef double SecondsCounter;

    osaStopwatch(void):
        HasHighPerformanceCounter(false)
    {
#if (CISST_OS == CISST_WINDOWS) || (CISST_OS == CISST_CYGWIN)
    static LARGE_INTEGER frequency = { 0, 0 };
    HasHighPerformanceCounter = (::QueryPerformanceFrequency( &frequency )) ? true : false;
    if (HasHighPerformanceCounter) {
        this->TimeGranularity = 1.0 / static_cast<double>(frequency.QuadPart); // this value should be in seconds
        CMN_LOG_INIT_WARNING << "Class osaStopwatch: Can use HighPerformance Counter, time granularity is: "
                             << TimeGranularity * cmn_ms << " ms" << std::endl;
    } else {
        this->TimeGranularity = 1.0e-3; // 1 millisecond
        CMN_LOG_INIT_WARNING << "Class osaStopwatch: Can NOT use HighPerformance Counter, time granularity is about: "
                             << TimeGranularity * cmn_ms << " ms" << std::endl;
    }
#elif (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    this->TimeGranularity = 1.0e-6; // 1 microsecond
#endif
        Reset();
    }

    /*! Return the granularity of time in the system, in seconds.  The fraction part is
      the fraction of second that the system time granularity supports */
    double CISST_EXPORT GetTimeGranularity(void) const;

    /*! Reset all the counters to zero, and stop the watch */
    void Reset(void)
    {
        RunningFlag = false;
        AccumulatedTime = 0;
#if (CISST_OS == CISST_WINDOWS) || (CISST_OS == CISST_CYGWIN)
        StartTicks = 0;
#elif (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
        StartTimeOfDay.tv_sec = 0;
        StartTimeOfDay.tv_usec = 0;
#endif
    }

    /*! Start the stopwatch from its current counter state.  If the watch is already
      running, do nothing */
    inline void Start(void)
    {
        if (IsRunning()) {
            return;
        }

#if (CISST_OS == CISST_WINDOWS) || (CISST_OS == CISST_CYGWIN)
        if (HasHighPerformanceCounter) {
            LARGE_INTEGER ticks;
            ::QueryPerformanceCounter(&ticks);
            StartTicks = GetTimeGranularity() * static_cast<double>(ticks.QuadPart);
        }
        else {
            StartTicks = ::GetTickCount();
        }
#elif (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
        gettimeofday(&StartTimeOfDay, 0);
#endif
        RunningFlag = true;
    }


    /*! Stop the stopwatch so that its counter state is kept until a Start or a Reset */
    inline void Stop(void)
    {
        if (!IsRunning())
            return;
        RunningFlag = false;
        AccumulatedTime += GetLastTimeInterval();
    }


    /*! Returns true if the stopwatch is running and counting continues (i.e., if Start
      was called last).  Returns false if the watch is stopped, i.e., if Stop or Reset
      was called last. */
    inline bool IsRunning(void) const
    {
        return RunningFlag;
    }

    
    /*! Return the current read of the stopwatch without changing the
      running state.  This method is now deprecated as it returns a
      time interval in milliseconds (integer).  Use GetElapsedTime()
      instead. */
    inline MillisecondsCounter CISST_DEPRECATED GetCurrentRead(void) const
    {
        if (!IsRunning()) {
            return static_cast<MillisecondsCounter>(AccumulatedTime * 1000.0);
        }
        return static_cast<MillisecondsCounter>((AccumulatedTime + GetLastTimeInterval()) * 1000.0);
    }

    /*! Return the current read of the stopwatch without changing the
      running state.  Return a time interval in seconds using a
      "double" precision floating point number. */
    inline SecondsCounter GetElapsedTime(void) const
    {
        if (!IsRunning()) {
            return AccumulatedTime;
        }
        return AccumulatedTime + GetLastTimeInterval();
    }

private:

    inline SecondsCounter GetLastTimeInterval(void) const
    {
#if (CISST_OS == CISST_WINDOWS) || (CISST_OS == CISST_CYGWIN)
        SecondsCounter endTicks;
        SecondsCounter totalSeconds;
        if (HasHighPerformanceCounter) {
            LARGE_INTEGER ticks;
            QueryPerformanceCounter(&ticks);
            endTicks = GetTimeGranularity() * static_cast<double>(ticks.QuadPart);
            totalSeconds = endTicks - StartTicks;
        } else {
            endTicks = ::GetTickCount();
            totalSeconds = static_cast<double>(endTicks - StartTicks) / 1000.0;
        }
#elif (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
        timeval endTimeOfDay;
        gettimeofday(&endTimeOfDay, 0);
        timeval diffTime;
        timersub(&endTimeOfDay, &StartTimeOfDay, &diffTime);
        SecondsCounter totalSeconds = diffTime.tv_sec + diffTime.tv_usec / 1000000.0;
#endif
        return totalSeconds;
    }

    bool RunningFlag;
    SecondsCounter AccumulatedTime;

    bool HasHighPerformanceCounter;
	double TimeGranularity;

#if (CISST_OS == CISST_WINDOWS) || (CISST_OS == CISST_CYGWIN)
    SecondsCounter StartTicks;
#elif (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    timeval StartTimeOfDay;
#endif

};


#endif  // _osaStopwatch_h

