/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Anton Deguet, Min Yang Jung
  Created on: 2008-01-30

  (C) Copyright 2004-2009 Johns Hopkins University (JHU), All Rights
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
#elif (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX)
    #include <pthread.h>
    #include <errno.h>
    #include <string.h>
#elif (CISST_OS == CISST_WINDOWS)
    #include <windows.h>
#endif


struct osaMutexInternals {
	/*! Pointer to OS dependent semaphore. */
//#if (CISST_OS == CISST_LINUX_RTAI)
//	SEM* Mutex;
//#endif // CISST_LINUX_RTAI
#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_QNX)
    pthread_mutex_t Mutex;
#elif (CISST_OS == CISST_WINDOWS)
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

osaMutex::osaMutex(void)
{
#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_QNX)
    CMN_ASSERT(sizeof(Internals) >= SizeOfInternals());
    if (pthread_mutex_init(&INTERNALS(Mutex), 0) != 0) {
        CMN_LOG_INIT_ERROR << "Class osaMutex: error in constructor \"" << strerror(errno) << "\"" << std::endl;
    }
#elif (CISST_OS == CISST_WINDOWS)
    CMN_ASSERT(sizeof(Internals) >= SizeOfInternals());
    INTERNALS(Mutex) = CreateMutex(NULL, FALSE, NULL);
#endif
}

// #if (CISST_OS == CISST_LINUX_RTAI)
// osaMutex::~osaMutex() {
//    rt_sem_delete(INTERNALS(Mutex));
//}
//#endif // CISST_LINUX_RTAI

osaMutex::~osaMutex() 
{
#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_QNX)
    pthread_mutex_destroy(&INTERNALS(Mutex));
#elif (CISST_OS == CISST_WINDOWS)
    if (INTERNALS(Mutex)) {
        CloseHandle(INTERNALS(Mutex));
    }
#endif
}

unsigned int osaMutex::SizeOfInternals(void) {
    return sizeof(osaMutexInternals);
}


// #if (CISST_OS == CISST_LINUX_RTAI)
// void osaMutex::Lock(void) {
//    rt_sem_wait(INTERNALS(Mutex));
// }
//#endif // CISST_LINUX_RTAI

void osaMutex::Lock(void) 
{
#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_QNX)
    pthread_mutex_lock(&INTERNALS(Mutex));
#elif (CISST_OS == CISST_WINDOWS)
    WaitForSingleObject(INTERNALS(Mutex), INFINITE);
#endif
}

// #if (CISST_OS == CISST_LINUX_RTAI)
// void osaMutex::Unlock(void) {
//    rt_sem_signal(INTERNALS(Mutex));
// }
// #endif // CISST_LINUX_RTAI

void osaMutex::Unlock(void) 
{
#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_QNX)
    pthread_mutex_unlock(&INTERNALS(Mutex));
#elif (CISST_OS == CISST_WINDOWS)
    ReleaseMutex(INTERNALS(Mutex));
#endif
}

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
