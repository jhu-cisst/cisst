/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Anton Deguet
  Created on: 2008-01-30

  (C) Copyright 2004-2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Declaration of osaMutex
  \ingroup cisstOSAbstraction
 */

#ifndef _osaMutex_h
#define _osaMutex_h

#include <cisstCommon/cmnPortability.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaExport.h>

struct osaMutexInternals;

/*!
  \brief Define a Mutex object

  \ingroup cisstOSAbstraction

  Mutex class provided to create mutual exclusion around critical
  sections.  This class relies on Posix threads mutex when possible.
  On Windows, it uses an actual Mutex which provides a locking
  mechanism between threads as well as processes.  It doesn't rely on
  the more specialized CriticalSection type (between threads only).
*/
class CISST_EXPORT osaMutex {

    /*! Internals that are OS-dependent */
    osaMutexInternals * Internals;

    /*! Locker thread id */
    osaThreadId LockerId;

    bool Locked;

public:
	/* Enum type representing a timeout period of infinity and no wait. */
	enum TimeoutType {
		WAIT_FOREVER = -1, /*! The calling function will be blocked till
                            the function returns */
		NO_WAIT = 0 	   /*! The calling function will not wait for
                             the function */
	};

	/*! Enum type for return values of a lock operation. */
#if 0 // MJ: obsoleted
	enum ReturnType {
		LOCK_FAILED, /*! The lock operation failed because the semaphore couldn't be obtained */
		SUCCESS,     /*! The lock operation was successful */
		TIMED_OUT	 /*! The lock operation timed out after waiting for specified time */
	};
#endif

    /*! Enum type for current lock state */
    enum LockStateType {
        LOCKED,
        UNLOCKED
    };

	/*! Default constructor.  Initialize the underlying mutex. */
	osaMutex(void);

	/*! Default destructor.  Cleanup the underlying mutex. */
	~osaMutex(void);

	/*! Mutex lock operation.  This class doesn't use recursive
        mutexes therefore you must make sure the mutex is not already
        locked.  Locking the same mutex twice from the same threads
        leads to undefined results.
    */
	void Lock(void);

	/*! Mutex unlock operation.  Be careful, one should never unlock
        an already unlocked mutex, this would lead to undefined
        results.
    */
	void Unlock(void);

	/*! Mutex lock operation with timeout

    \param timeout If timeout == WAIT_FOREVER then the calling
	thread is blocked (this is same as calling Lock()).  If timeout ==
	NO_WAIT then the function returns after checking if the mutex
	can be locked or not.  If timeout > 0 then the function waits for
	the specified amount of time (units to be specified later) before
	returning.

    \return An enumerated type representing if a lock was obtained, or
    operation timed out or lock failed
    */
	//ReturnType TryLock(int timeout);

    /*! Check if the current thread locked this mutex earlier */
    bool IsLocker(void) const;

    /*! Get current lock state */
    LockStateType GetLockState(void) const;
};


#endif // _osaMutex_h

