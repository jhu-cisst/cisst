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

#include <cisstCommon/cmnPortability.h>
#include <cisstOSAbstraction/osaTimeServer.h>
#include <cisstOSAbstraction/osaThread.h>

// Always include last
#include <cisstOSAbstraction/osaExport.h>

/*! \ingroup cisstOSAbstraction
  The OS dependent initialization that is
  to be executed for real-time support. This is called from the
  constructor of TaskManager, which is implemented as singleton. */
CISST_EXPORT void __os_init(void);

/*! \ingroup cisstOSAbstraction
  The OS dependent cleanup that is to be executed for real-time support.
  This is called from the destructor of TaskManager. */
CISST_EXPORT void __os_exit(void);


struct osaThreadBuddyInternals;

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

    osaThreadBuddyInternals* Data;

    /*! Thread period (if > 0) */
    double Period;

public:
    /*! Constructor. Allocates internal data. */
    osaThreadBuddy();

    /*! Destructor. */
    ~osaThreadBuddy();

    /*! Create a thread buddy with a name 'name' 
    
    \param name Name of thread buddy, the thread will also be referred
    using the same name.
    
    \param period The period of the thread (in nanosecond)
    
    \param stack_size The stack size allocated to the thread. A
      default value of 8Kb is used. */
    void Create(const char *name, double period, int stack_size = 1024*256);
    void Create(const char *name, const osaAbsoluteTime& tv, int stack_size = 1024*256 );

    /*! Delete the thread buddy. */
    void Delete();

    /*! Return true if thread is periodic.  Currently, returns true if
      the thread was created with a period > 0. */
    bool IsPeriodic() const { return Period > 0; }

    /*! Suspend the execution of the real time thread until the next
      period is reached. */
    void WaitForPeriod(void);

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

    /*! Lock stack growth */
    void LockStack();

    /*! Unlock stack */
    void UnlockStack();
};


#endif // _osaThreadBuddy_h

