/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Tae Soo Kim
  Created on: 2013-06-01

  (C) Copyright 2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _osaPerformanceCounter_h
#define _osaPerformanceCounter_h

#include <cisstOSAbstraction/osaTimeData.h>

/*!
  \brief Implementation of a reliable performance counter.
  \ingroup cisstOSAbstraction

  The osaPerformanceCounter provides a reliable and an easy to use
  high performance relative counter. It is likely to drift and thus
  this class contains methods to realign and correct the drift.
*/
class osaPerformanceCounter
{
public:
    /*! Default constructor> */
    osaPerformanceCounter(void);

    /*!Default destructor*/
    ~osaPerformanceCounter(){}

    /*! Reset all the counters to zero and stop the watch. */
    void Reset(void);

    /*! Sets the origin of this osaPerformanceCounter to a given osaTimeData. */
    void SetOrigin(osaTimeData & origin);

    /*! Start the stopwatch. */
    void Start(void);

    /*! Stop the stopwatch at current counter state. */
    void Stop(void);

    /*! Returns the state of the counter.
      \return true if counter is running, false if counter is stopped. */
    bool IsRunning(void);

    /*! Return the current read of the counter. */
    osaTimeData GetElapsedTime(void);

    /*! Delays/Sleeps the system by the time represented by osaTimeData. */
    void Delay(osaTimeData delayAmount);

protected:
    osaTimeData Origin;
    osaTimeData End;
    bool HasHighPerformanceCounter;
    bool isRunning;
};

#endif // _osaPerformanceCounter_h
