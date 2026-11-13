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
  \brief Declaration of osaGetTime
 */

#ifndef _osaGetTime_h
#define _osaGetTime_h

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

#endif // _osaGetTime_h
