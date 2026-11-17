/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Ankur Kapoor, Anton Deguet, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Declaration of osaTime
 */

#ifndef _osaTime_h
#define _osaTime_h

#include <chrono>
#include <string>

#include <cisstOSAbstraction/osaExport.h>

/*! Convert current local datetime to string */
void CISST_EXPORT osaGetDateTimeString(std::string& output, char delimiter = '-');
/*! Convert datetime to string */
void CISST_EXPORT osaGetDateTimeString(std::string& output, std::chrono::system_clock::time_point time, char delimiter = '-');
/*! Convert date to (y:m:d) string */
void CISST_EXPORT osaGetDateString(std::string& output, std::chrono::system_clock::time_point time, char delimiter = ':');
/*! Convert time to (h:m:s:ms) string */
void CISST_EXPORT osaGetTimeString(std::string& output, std::chrono::system_clock::time_point time, char delimiter = ':');

/*! Convert current local time to string */
void CISST_EXPORT osaGetTimeString(std::string& output);

// Convert time point to decimal timestamp in seconds since clock's epoch
double CISST_EXPORT getTimestamp(std::chrono::steady_clock::time_point time_point);

template <class Rep, class Period=std::ratio<1>>
double durationToSeconds(std::chrono::duration<Rep, Period> duration) {
    using seconds_d = std::chrono::duration<double>;
    return std::chrono::duration_cast<seconds_d>(duration).count();
}

template <typename clock_t>
typename clock_t::duration durationFromSeconds(double seconds) {
    const auto seconds_d = std::chrono::duration<double>(seconds);
    return std::chrono::duration_cast<typename clock_t::duration>(seconds_d);
}

#endif // _osaTime_h
