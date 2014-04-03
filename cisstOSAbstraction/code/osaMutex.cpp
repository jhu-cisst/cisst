/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Anton Deguet, Min Yang Jung
  Created on: 2008-01-30

  (C) Copyright 2004-2012 Johns Hopkins University (JHU), All Rights
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
#elif (CISST_OS == CISST_LINUX_XENOMAI)

#include <native/mutex.h>
#include <native/task.h>
#include <pthread.h>

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
#elif (CISST_OS == CISST_LINUX_XENOMAI)

    //RT_MUTEX RTMutex;
    pthread_mutex_t POSIXMutex;

#elif (CISST_OS == CISST_WINDOWS)
	HANDLE Mutex;
#endif
};

osaMutex::osaMutex(void):
    Internals(0),
    Locked(false)
{
    this->Internals = new osaMutexInternals;
#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_QNX)
    if (pthread_mutex_init(&(Internals->Mutex), 0) != 0) {
        CMN_LOG_INIT_ERROR << "Class osaMutex: error in constructor \"" << strerror(errno) << "\"" << std::endl;
    }
#elif (CISST_OS == CISST_LINUX_XENOMAI)

    /*
    if( rt_task_self() != NULL ){
        int retval = 0;
        retval = rt_mutex_create( &INTERNALS(RTMutex), NULL );
        std::cout<< "rt_mutex_create"  << Internals << std::endl;
        if( retval != 0 ){
            CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                              << "Failed to create mutex. "
                              << strerror(retval) << ": " << retval << ". "
                              << std::endl;
        }
    }
    */
    //else{
    //std::cout << __FILE__ << ": " << __LINE__ << std::endl;
    //  std::cout<< "pthread_mutex_init"  << Internals << std::endl;
    if( pthread_mutex_init( &(Internals->POSIXMutex), 0 ) != 0 ) {
            CMN_LOG_INIT_ERROR << "Class osaMutex: error in constructor \""
                               << strerror(errno) << "\"" << std::endl;
        }
        //  std::cout << __FILE__ << ": " << __LINE__ << std::endl;
        //}

#elif (CISST_OS == CISST_WINDOWS)
    Internals->Mutex = CreateMutex(NULL, FALSE, NULL);
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
    pthread_mutex_destroy(&(Internals->Mutex));
#elif (CISST_OS == CISST_LINUX_XENOMAI)
    /*
    if( rt_task_self() != NULL ){
        int retval = 0;
        retval = rt_mutex_delete( &INTERNALS(RTMutex) );
        if( retval != 0 ){
            CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                              << "Failed to delete mutex. "
                              << strerror(retval) << ": " << retval << ". "
                              << std::endl;
        }
    }
    else{
    */
    //std::cout << __FILE__ << ": " << __LINE__ << std::endl;
    pthread_mutex_destroy( &(Internals->POSIXMutex) );
            //      std::cout << __FILE__ << ": " << __LINE__ << std::endl;
            //}

#elif (CISST_OS == CISST_WINDOWS)
    if (Internals->Mutex) {
        CloseHandle(Internals->Mutex);
    }
#endif
    delete this->Internals;
}

// #if (CISST_OS == CISST_LINUX_RTAI)
// void osaMutex::Lock(void) {
//    rt_sem_wait(INTERNALS(Mutex));
// }
//#endif // CISST_LINUX_RTAI

void osaMutex::Lock(void)
{
    // Remember locker id
    LockerId = osaGetCurrentThreadId();
    Locked = true;

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_QNX)
    pthread_mutex_lock(&(Internals->Mutex));
#elif (CISST_OS == CISST_LINUX_XENOMAI)
    /*
    if( rt_task_self() != NULL ){
        int retval = 0;
        std::cout << "rt_mutex_acquire: " << Internals << std::endl;
        retval = rt_mutex_acquire( &INTERNALS( RTMutex ), TM_INFINITE );
        if( retval != 0 ){
            std::string errstr;
            switch( -retval ){
            case EINVAL:
                errstr = "EINVAL";
                break;
            case EIDRM:
                errstr = "EIDRM";
                break;
            case EWOULDBLOCK:
                errstr = "EWOULDBLOCK";
                break;
            case EINTR:
                errstr = "EINTR";
                break;
            case ETIMEDOUT:
                errstr = "ETIMEDOUT";
                break;
            case EPERM:
                errstr = "EPERM";
                break;
            }
            CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                              << "Failed to acquire mutex: "
                              << errstr << std::endl;
        }
    }
    else {
    */
    //            std::cout << __FILE__ << ": " << __LINE__ << std::endl;
    pthread_mutex_lock( &(Internals->POSIXMutex) );
            //      std::cout << __FILE__ << ": " << __LINE__ << std::endl;
            //  }

#elif (CISST_OS == CISST_WINDOWS)
    WaitForSingleObject(Internals->Mutex, INFINITE);
#endif
}

// #if (CISST_OS == CISST_LINUX_RTAI)
// void osaMutex::Unlock(void) {
//    rt_sem_signal(INTERNALS(Mutex));
// }
// #endif // CISST_LINUX_RTAI

void osaMutex::Unlock(void)
{
    // Reset locker id.  LockerId gets invalidated.
    Locked = false;

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_QNX)
    pthread_mutex_unlock(&(Internals->Mutex));
#elif (CISST_OS == CISST_LINUX_XENOMAI)
    /*
    if( rt_task_self() != NULL ){
        int retval = 0;
        retval = rt_mutex_release( &INTERNALS(RTMutex) );
        if( retval != 0 ){
            std::string errstr;
            switch( -retval ){
            case EINVAL:
                errstr = "EINVAL";
                break;
            case EIDRM:
                errstr = "EIDRM";
                break;
            case EPERM:
                errstr = "EPERM";
                break;
            }
            CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                              << "Failed to release mutex: "
                              << errstr << std::endl;
        }
    }
    else  {
    */
    //            std::cout << __FILE__ << ": " << __LINE__ << std::endl;
    pthread_mutex_unlock( &(Internals->POSIXMutex) );
            //      std::cout << __FILE__ << ": " << __LINE__ << std::endl;
            //}

#elif (CISST_OS == CISST_WINDOWS)
    ReleaseMutex(Internals->Mutex);
#endif
}

bool osaMutex::IsLocker(void) const
{
    if (!Locked) return false;

    return (osaGetCurrentThreadId().Equal(LockerId));
}

#if 0
#if (CISST_OS == CISST_LINUX_RTAI)
osaMutex::ReturnType osaMutex::TryLock(int timeout) {
    ReturnType result;
    if (timeout == NO_WAIT) {
        if (rt_sem_wait_if(Internals->Mutex) > 0) {
            result = SUCCESS;
        } else {
            result = LOCK_FAILED;
        }
    } else if (timeout == WAIT_FOREVER) {
        if (rt_sem_wait(Internals->Mutex) < SEM_TIMOUT) {
            result = SUCCESS;
        } else {
            result = LOCK_FAILED;
        }
    } else {
        if (rt_sem_wait_timed(Internals->Mutex, nano2count(timeout)) < SEM_TIMOUT) {
            result = SUCCESS;
        } else {
            result = TIMED_OUT;
        }
    }
    return result;
}
#endif // CISST_LINUX_RTAI
#endif
