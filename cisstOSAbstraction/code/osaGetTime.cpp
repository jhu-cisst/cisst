/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Ankur Kapoor, Anton Deguet, Min Yang Jung
  Created on: 2004

  (C) Copyright 2004-2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <ctime>
#include <iomanip>

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnUnits.h>
#include <cisstCommon/cmnLogger.h>

#include <cisstOSAbstraction/osaGetTime.h>


void osaGetDateTimeString(std::string& output, char delimiter)
{
    const auto now = std::chrono::system_clock::now();
    osaGetDateTimeString(output, now, delimiter);
}

void osaGetDateTimeString(std::string& output, std::chrono::system_clock::time_point time, char delimiter)
{
    const std::time_t time_t = std::chrono::system_clock::to_time_t(time);
    const auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(time);
    const std::tm* tm = std::localtime(&time_t);
    const int ms = std::chrono::duration_cast<std::chrono::milliseconds>(time - seconds).count();

    std::ostringstream oss;
    if (delimiter == ':') {
        oss << std::put_time(tm, "%Y")
            << "-"
            << std::put_time(tm, "%m")
            << "-"
            << std::put_time(tm, "%d")
            << " "
            << std::put_time(tm, "%H")
            << ":"
            << std::put_time(tm, "%M")
            << ":"
            << std::put_time(tm, "%S")
            << ":"
            << std::setfill('0') << std::setw(3) << ms;
    } else {
        oss << std::put_time(tm, "%Y")
            << delimiter
            << std::put_time(tm, "%m")
            << delimiter
            << std::put_time(tm, "%d")
            << delimiter
            << std::put_time(tm, "%H")
            << delimiter
            << std::put_time(tm, "%M")
            << delimiter
            << std::put_time(tm, "%S")
            << delimiter
            << std::setfill('0') << std::setw(3) << ms;
    }

    output = oss.str();
}

void osaGetDateString(std::string& output, std::chrono::system_clock::time_point time, char delimiter)
{
    const std::time_t time_t = std::chrono::system_clock::to_time_t(time);
    const std::tm* tm = std::localtime(&time_t);

    std::ostringstream oss;
    oss << std::put_time(tm, "%y")
        << delimiter
        << std::put_time(tm, "%m")
        << delimiter
        << std::put_time(tm, "%d");

    output = oss.str();
}

void osaGetTimeString(std::string& output, std::chrono::system_clock::time_point time, char delimiter)
{
    const std::time_t time_t = std::chrono::system_clock::to_time_t(time);
    const auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(time);
    const std::tm* tm = std::localtime(&time_t);
    const int ms = std::chrono::duration_cast<std::chrono::milliseconds>(time - seconds).count();

    std::ostringstream oss;
    oss << std::put_time(tm, "%H")
        << delimiter
        << std::put_time(tm, "%M")
        << delimiter
        << std::put_time(tm, "%S")
        << delimiter
        << std::setfill('0') << std::setw(3) << ms;

    output = oss.str();
}

void osaGetTimeString(std::string& output)
{
    const auto now = std::chrono::system_clock::now();
    const auto now_t = std::chrono::system_clock::to_time_t(now);
    const std::tm* tm = std::localtime(&now_t);

    const std::string format = "%H:%M:%S";

    std::ostringstream oss;
    oss << std::put_time(tm, format.c_str()); 
    output = oss.str();
}
