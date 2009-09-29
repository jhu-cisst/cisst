/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: osaMutex.cpp 433 2009-06-09 22:10:24Z adeguet1 $

  Author(s): Anton Deguet
  Created on: 2008-01-30

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstCommon/cmnExport.h>
#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnAssert.h>
#include <cisstOSAbstraction/osaMutex.h>


#if (CISST_OS == CISST_LINUX_RTAI)
#include <rtai_sem.h>
#include <rtai_types.h>
#endif // CISST_LINUX_RTAI

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
#include <pthread.h>
#include <errno.h>
#include <string.h>
#endif // CISST_LINUX_RTAI || CISST_LINUX || CISST_DARWIN || CISST_SOLARIS

#if (CISST_OS == CISST_WINDOWS)
#include <windows.h>
#endif // CISST_WINDOWS


struct osaMutexInternals {
	/*! Pointer to OS dependent semaphore. */
//#if (CISST_OS == CISST_LINUX_RTAI)
//	SEM* Mutex;
//#endif // CISST_LINUX_RTAI
#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI)
    pthread_mutex_t Mutex;
#endif // CISST_LINUX || CISST_DARWIN || CISST_SOLARIS || CISST_RTAI
#if (CISST_OS == CISST_WINDOWS)
	HANDLE Mutex;
#endif
};


#define INTERNALS(A) (reinterpret_cast<osaMutexInternals*>(Internals)->A)


// #if (CISST_OS == CISST_LINUX_RTAI)
// osaMutex::osaMutex(void)
// {
//    int semName = (int)((osaMutex*)this);
//    CMN_ASSERT(sizeof(Internals) >= SizeOfInternals());
//    INTERNALS(Mutex) = rt_typed_sem_init(semName, 1, BIN_SEM);
//}
//#endif // CISST_LINUX_RTAI

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI)
osaMutex::osaMutex(void)
{
    CMN_ASSERT(sizeof(Internals) >= SizeOfInternals());
    if (pthread_mutex_init(&INTERNALS(Mutex), 0) != 0) {
        CMN_LOG_INIT_ERROR << "Class osaMutex: error in constructor \"" << strerror(errno) << "\"" << std::endl;
    }
}
#endif // CISST_LINUX || CISST_DARWIN || CISST_SOLARIS || CISST_RTAI

#if (CISST_OS == CISST_WINDOWS)
osaMutex::osaMutex(void)
{
    CMN_ASSERT(sizeof(Internals) >= SizeOfInternals());
    INTERNALS(Mutex) = CreateMutex(NULL, FALSE, NULL);
}
#endif // CISST_WINDOWS




// #if (CISST_OS == CISST_LINUX_RTAI)
// osaMutex::~osaMutex() {
//    rt_sem_delete(INTERNALS(Mutex));
//}
//#endif // CISST_LINUX_RTAI

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI)
osaMutex::~osaMutex() {
    pthread_mutex_destroy(&INTERNALS(Mutex));
}
#endif // CISST_LINUX || CISST_DARWIN || CISST_SOLARIS || CISST_LINUX_RTAI

#if (CISST_OS == CISST_WINDOWS)
osaMutex::~osaMutex() {
    if (INTERNALS(Mutex)) {
        CloseHandle(INTERNALS(Mutex));
    }
}
#endif // CISST_WINDOWS




unsigned int osaMutex::SizeOfInternals(void) {
    return sizeof(osaMutexInternals);
}


// #if (CISST_OS == CISST_LINUX_RTAI)
// void osaMutex::Lock(void) {
//    rt_sem_wait(INTERNALS(Mutex));
// }
//#endif // CISST_LINUX_RTAI

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI)
void osaMutex::Lock(void) {
    pthread_mutex_lock(&INTERNALS(Mutex));
}
#endif // CISST_LINUX || CISST_DARWIN || CISST_SOLARIS || CISST_LINUX_RTAI

#if (CISST_OS == CISST_WINDOWS)
void osaMutex::Lock(void) {
    WaitForSingleObject(INTERNALS(Mutex), INFINITE);
}
#endif // CISST_WINDOWS




// #if (CISST_OS == CISST_LINUX_RTAI)
// void osaMutex::Unlock(void) {
//    rt_sem_signal(INTERNALS(Mutex));
// }
// #endif // CISST_LINUX_RTAI

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI)
void osaMutex::Unlock(void) {
    pthread_mutex_unlock(&INTERNALS(Mutex));
}
#endif // CISST_LINUX || CISST_DARWIN || CISST_SOLARIS || CISST_LINUX_RTAI

#if (CISST_OS == CISST_WINDOWS)
void osaMutex::Unlock(void) {
    ReleaseMutex(INTERNALS(Mutex));
}
#endif // CISST_WINDOWS



#if 0
#if (CISST_OS == CISST_LINUX_RTAI)
osaMutex::ReturnType osaMutex::TryLock(int timeout) {
    ReturnType result;
    if (timeout == NO_WAIT) {
        if (rt_sem_wait_if(INTERNALS(Mutex)) > 0) {
            result = SUCCESS;
        } else {
            result = LOCK_FAILED;
        }
    } else if (timeout == WAIT_FOREVER) {
        if (rt_sem_wait(INTERNALS(Mutex)) < SEM_TIMOUT) {
            result = SUCCESS;
        } else {
            result = LOCK_FAILED;
        }
    } else {
        if (rt_sem_wait_timed(INTERNALS(Mutex), nano2count(timeout)) < SEM_TIMOUT) {
            result = SUCCESS;
        } else {
            result = TIMED_OUT;
        }
    }
    return result;
}
#endif // CISST_LINUX_RTAI
#endif
