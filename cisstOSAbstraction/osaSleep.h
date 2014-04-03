/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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
  \brief Declaration of osaSleep
 */

#ifndef _osaSleep_h
#define _osaSleep_h

// Include cmnUnits to have constants available
#include <cisstCommon/cmnUnits.h>

// Always include last
#include <cisstOSAbstraction/osaExport.h>


/*!
  \brief Define osaSleep

  \ingroup cisstOSAbstraction
 
  Execute a wait or sleep. This is intended for general purpose use; no time
  guarantees are made on any OS.  Within a thread, it is preferable to call
  osaThread::Sleep.  The mtsTask class provides mtsTask::Sleep, which invokes
  osaThread::Sleep.

  On some platforms, osaSleep might block the calling thread and not relinquish
  the processor to other threads. In particular, there are reports that
  calling osaSleep in RTAI may lead to stack overflows.
     
  \param time Time in seconds to sleep */
void CISST_EXPORT osaSleep(double timeInSeconds);


#endif // _osaSleep_h

