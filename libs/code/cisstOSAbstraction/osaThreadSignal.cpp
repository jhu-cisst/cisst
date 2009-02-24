/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Ankur Kapoor, Peter Kazanzides, Balazs Vagvolgyi, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnAssert.h>
#include <cisstCommon/cmnPortability.h>
#include <cisstOSAbstraction/osaThreadSignal.h>

#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
//PK: Probably some of these include files are not needed
#include <pthread.h>
#include <sched.h>
#include <sys/time.h>
#include <cmath>
#include <errno.h>
#ifdef USE_POSIX_SEMAPHORES
#include <semaphore.h>
#endif // USE_POSIX_SEMAPHORES
#endif // CISST_LINUX_RTAI || CISST_LINUX || CISST_DARWIN || CISST_SOLARIS

#if (CISST_OS == CISST_WINDOWS)
#include <windows.h>
#endif


struct osaThreadSignalInternals {

#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    pthread_mutex_t Mutex;
    pthread_cond_t Condition;
#ifdef USE_POSIX_SEMAPHORES
    sem_t Sem;
#endif

#endif // CISST_LINUX_RTAI || CISST_LINUX || CISST_DARWIN || CISST_SOLARIS

#if (CISST_OS == CISST_WINDOWS)
    HANDLE Event;
#endif // CISST_WINDOWS
};

#define INTERNALS(A) (reinterpret_cast<osaThreadSignalInternals*>(Internals)->A)

unsigned int osaThreadSignal::SizeOfInternals(void) {
    return sizeof(osaThreadSignalInternals);
}

osaThreadSignal::osaThreadSignal()
{
    CMN_ASSERT(sizeof(Internals) >= SizeOfInternals());

#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    pthread_mutex_init(&INTERNALS(Mutex), NULL);
    pthread_cond_init(&INTERNALS(Condition), NULL);
#ifdef USE_POSIX_SEMAPHORES
    // Initialize (unshared) semaphore to 0
    sem_init(&INTERNALS(Sem),0,0);
#endif
#endif // CISST_LINUX_RTAI || CISST_LINUX || CISST_DARWIN || CISST_SOLARIS
#if (CISST_OS == CISST_WINDOWS)
    INTERNALS(Event) = CreateEvent(
                          NULL,                       // default security attributes
                          FALSE,                      // auto-reset event
                          FALSE,                      // initial value
                          NULL);                      // no name
#endif
}

osaThreadSignal::~osaThreadSignal()
{
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    pthread_mutex_destroy(&INTERNALS(Mutex));
    pthread_cond_destroy(&INTERNALS(Condition));
#ifdef USE_POSIX_SEMAPHORES
    sem_destroy(&INTERNALS(Sem));
#endif
#endif // CISST_LINUX_RTAI || CISST_LINUX || CISST_DARWIN || CISST_SOLARIS

#if (CISST_OS == CISST_WINDOWS)
    CloseHandle(INTERNALS(Event));
#endif // CISST_WINDOWS
}

void osaThreadSignal::Wait(void)
{
#if (CISST_OS == CISST_WINDOWS)
    // Note: Windows Vista has condition variables, but older versions of Windows do not.
    // Note: MSDN recommends using MsgWaitForMultipleObjects() if the thread creates any windows
    // so that messages can still be processed.
    DWORD ret = WaitForSingleObject(INTERNALS(Event), INFINITE);
    if (ret != WAIT_OBJECT_0)
        CMN_LOG(1) << "osaThreadSignal::Wait: error return = " << ret << std::endl;
#elif (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN)
#ifdef USE_POSIX_SEMAPHORES
    // Implementation using POSIX semaphores
    sem_wait(&INTERNALS(Sem));
#else
    // Implementation using condition variables & mutexes
    pthread_mutex_lock(&INTERNALS(Mutex));
    pthread_cond_wait(&INTERNALS(Condition), &INTERNALS(Mutex));
    pthread_mutex_unlock(&INTERNALS(Mutex));
#endif  // USE_POSIX_SEMAPHORES
#else
    CMN_LOG(1) << "osaThreadSignal::Wait not implemented." << std::endl;
#endif
}

// returns false if timeout happened
bool osaThreadSignal::Wait(double timeoutInSec)
{
#if (CISST_OS == CISST_WINDOWS)
    unsigned int millisec = static_cast<unsigned int>(timeoutInSec*1000);
    if (WaitForSingleObject(INTERNALS(Event), millisec) == WAIT_TIMEOUT) {
        CMN_LOG(5) << "osaThreadSignal::Wait timed out" << std::endl;
        return false;  // timeout
    }

#elif (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN)
    pthread_mutex_lock(&INTERNALS(Mutex));

    // PKAZ:  Balazs had following conditional.  I don't think we need it.  Also, I would move gettimeofday above the mutex lock.
    // If the condition state is triggered, then release the thread.
    // if (Condition_State == SVL_EVENT_NOT_SIGNALED)

    timeval now;
    timespec timeout;
    gettimeofday(&now, 0);

    double delta_sec;
    double delta_nsec = modf(timeoutInSec, &delta_sec)*1e9;
#if 0
    // Balazs had following loop, but I don't think it is needed
    while (usec >= 1000000) {
        sec ++;
        usec -= 1000000;
    }
#endif
    timeout.tv_sec = now.tv_sec + static_cast<unsigned long>(delta_sec);
    timeout.tv_nsec = now.tv_usec*1000 + static_cast<unsigned long>(delta_nsec);

    int ret = pthread_cond_timedwait(&INTERNALS(Condition), &INTERNALS(Mutex), &timeout);

#if 1   // Cleaner implementation, assuming that AUTOMATIC RESET is not needed 
    pthread_mutex_unlock(&INTERNALS(Mutex));
    return (ret != ETIMEDOUT);
#else
    if (ret == ETIMEDOUT) {
        pthread_mutex_unlock(&INTERNALS(Mutex));
        CMN_LOG(5) << "osaThreadSignal::Wait timed out" << std::endl;
        return false;  // timeout
    }

    // PKAZ:  I don't think following is needed
    // AUTOMATIC RESET:
    // condition is not referenced to anymore so it is safe to release it
    //pthread_cond_destroy(&gnuCondition);
    // reinitializing condition = resetting state
    //pthread_cond_init(&gnuCondition, 0);
    //Condition_State = SVL_EVENT_NOT_SIGNALED;

    // This unlock could be moved before the if statement above    
    pthread_mutex_unlock(&INTERNALS(Mutex));
#endif
#else
    CMN_LOG(1) << "osaThreadSignal::Wait not implemented." << std::endl;
#endif

    return true;
}

void osaThreadSignal::Raise(void)
{
#if (CISST_OS == CISST_WINDOWS)
  // Balazs used ::SetEvent
    if (!SetEvent(INTERNALS(Event)))
        CMN_LOG(1) << "osaThreadSignal::Raise failed" << std::endl;
#elif (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
#ifdef USE_POSIX_SEMAPHORES
    sem_post(&INTERNALS(Sem));
#else
    // Balazs code:
    // pthread_mutex_lock(&gnuMutex);
    // pthread_cond_broadcast(&gnuCondition);
    // Condition_State = SVL_EVENT_SIGNALED;
    // pthread_mutex_unlock(&gnuMutex);
    pthread_cond_signal(&INTERNALS(Condition));
#endif  // USE_POSIX_SEMAPHORES
#else
    CMN_LOG(1) << "osaThreadSignal::Raise not implemented." << std::endl;
#endif
}

