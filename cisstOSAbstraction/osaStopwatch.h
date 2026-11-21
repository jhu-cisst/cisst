/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ofri Sadowsky, Min Yang Jung
  Created on: 2005-02-17

  (C) Copyright 2005-2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _osaStopwatch_h
#define _osaStopwatch_h

#include <chrono>

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnUnits.h>
#include <cisstCommon/cmnLogger.h>

// Always include last
#include <cisstOSAbstraction/osaExport.h>

/*! Implement a simple stopwatch mechanism, which is helpful in
  profiling, debugging, and performance estimation.

  The osaStopwatch class uses system calls to obtain time at the
  finest granularity available in the system.  Note that a system call
  may sometimes invoke a context switch, and use them only when needed
  to interfere the least with the normal performance of the program.

  osaStopWatch makes use of a clock_t type that should satisfy the
  C++ Clock requirements, such as std::chrono::steady_clock. On some
  operating systems, such as Xenomai, using std::chrono::steady_clock
  will trigger a domain switch, so a custom Clock should be used instead. 

  Note: *not* thread safe.
*/
template <typename clock_t>
class osaStopwatch
{
public:
    osaStopwatch()
    {
        Reset();
    }

    /*! Reset all the counters to zero, and stop the watch */
    void Reset()
    {
        is_running = false;
        accumulated = std::chrono::duration<double>(0);
    }

    /*! Start the stopwatch from its current counter state.  If the watch is already
      running, do nothing */
    void Start()
    {
        if (IsRunning()) {
            return;
        }

        start_time = clock_t::now();
        is_running = true;
    }

    /*! Stop the stopwatch so that its counter state is kept until a Start or a Reset */
    void Stop()
    {
        if (!IsRunning()) {
            return;
        }

        is_running = false;
        accumulated += GetLastTimeInterval();
    }

    /*! Returns true if the stopwatch is running and counting continues (i.e., if Start
      was called last).  Returns false if the watch is stopped, i.e., if Stop or Reset
      was called last. */
    bool IsRunning() const { return is_running; }

    /*! Return the current read of the stopwatch without changing the
      running state.  Return a time interval in seconds */
    std::chrono::duration<double> GetElapsedTime() const
    {
        if (!IsRunning()) {
            return accumulated;
        }

        return accumulated + GetLastTimeInterval();
    }

private:
    std::chrono::duration<double> GetLastTimeInterval() const
    {
        return clock_t::now() - start_time;
    }

    bool is_running;
    std::chrono::duration<double> accumulated;
    typename clock_t::time_point start_time;
};

#endif  // _osaStopwatch_h
