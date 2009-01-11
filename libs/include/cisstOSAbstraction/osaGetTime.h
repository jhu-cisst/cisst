/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: osaGetTime.h,v 1.2 2009/01/05 08:24:44 pkaz Exp $

  Author(s): Ankur Kapoor, Anton Deguet
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

#include <cisstOSAbstraction/osaExport.h>

/*! Return the most accurate time available on the system.
  \return A double representing time in seconds
*/
double CISST_EXPORT osaGetTime(void);

#if 0
#include <ctime>  // for struct timeval and other useful functions

struct osaAbsoluteTime {
    long sec;   // seconds
    long nsec;  // nano-seconds
};

void CISST_EXPORT osaGetAbsoluteTime(osaAbsoluteTime & now);
// Note that time can be printed using ctime(now.sec);
#endif

#endif // _osaGetTime_h

