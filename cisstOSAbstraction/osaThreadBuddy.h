/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Ankur Kapoor, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Declaration of osaThreadBuddy
*/

#ifndef _osaThreadBuddy_h
#define _osaThreadBuddy_h

#include <atomic>
#include <thread>

#include <cisstCommon/cmnPortability.h>
#include <cisstOSAbstraction/osaTimeServer.h>

// Always include last
#include <cisstOSAbstraction/osaExport.h>

// PIMPL
class osaThreadBuddyInternals;

/*!  \brief Implementation of the Thread Proxy object in an OS
  independent manner.

  \ingroup cisstOSAbstraction

  RTAI provides "buddy" or proxy objects so that any thread created
  using pthread can be made real time. Our rtsTask class uses this
  mechanism to provide real time abstract tasks that have a state and
  a mailbox. This class makes it possible to keep the rtsTask class free
  from OS dependent stuff, which is moved here. This would also make
  it easy to provide Soft Real Time tasks in Vanila Linux flavor.
 */
class CISST_EXPORT osaThreadBuddy {
private:
    std::unique_ptr<osaThreadBuddyInternals> internals;

    std::thread* thread;

    /*! Thread period (if > 0) */
    double Period = 0;

    std::atomic<bool> IsSuspended = false;

public:
    /*! The thread we are buddying to must outlive the buddy*/
    osaThreadBuddy(std::thread& thread);

    ~osaThreadBuddy();

    /*! Create a thread buddy
    \param period The period of the thread (in nanosecond)
    */
    void Create(double period);
    void Create(const osaAbsoluteTime& tv);

    /*! Return true if thread is periodic.  Currently, returns true if
      the thread was created with a period > 0. */
    bool IsPeriodic() const { return Period > 0; }

    /*! Suspend the execution of the real time thread for the
      remainder of the current period. */
    void WaitForRemainingPeriod(void);

    /*! Make a thread hard real time. */
    void MakeHardRealTime(void);

    /*! Make a thread soft real time. */
    void MakeSoftRealTime(void);

    /*! Resume execution of the thread. */
    void Resume(void);

    /*! Suspend execution of the thread. */
    void Suspend(void);
};


#endif // _osaThreadBuddy_h

