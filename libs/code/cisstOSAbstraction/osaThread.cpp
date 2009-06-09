/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Ankur Kapoor
  Created on: 2004-04-30

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnAssert.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaSleep.h>

#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
#include <pthread.h>
#include <sched.h>
#endif // CISST_LINUX_RTAI || CISST_LINUX || CISST_DARWIN || CISST_SOLARIS

#if (CISST_OS == CISST_WINDOWS)
#include <windows.h>
#endif



struct osaThreadIdInternals {

#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    /*! OS dependent thread id. */
    pthread_t ThreadId;
#endif // CISST_LINUX_RTAI || CISST_LINUX || CISST_DARWIN || CISST_SOLARIS

#if (CISST_OS == CISST_WINDOWS)
    DWORD ThreadId;
#endif // CISST_WINDOWS
};


// Constructor. Does nothing.
osaThreadId::osaThreadId(void)
{
    CMN_ASSERT(sizeof(Internals) >= SizeOfInternals());
}


// Destructor. Does nothing.
osaThreadId::~osaThreadId()
{
}


unsigned int osaThreadId::SizeOfInternals(void) {
    return sizeof(osaThreadIdInternals);
}


// Compare thread Ids
bool osaThreadId::Equal(const osaThreadId & other) const {
    osaThreadId * nonConstThis = const_cast<osaThreadId *>(this);
    osaThreadId * nonConstOther = const_cast<osaThreadId *>(&other);
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    return (pthread_equal(reinterpret_cast<osaThreadIdInternals*>(nonConstThis->Internals)->ThreadId,
                          reinterpret_cast<osaThreadIdInternals*>(nonConstOther->Internals)->ThreadId) != 0 ? true : false); 
#endif // CISST_LINUX_RTAI || CISST_LINUX || CISST_DARWIN || CISST_SOLARIS
    
#if (CISST_OS == CISST_WINDOWS)
    return
        reinterpret_cast<osaThreadIdInternals*>(nonConstThis->Internals)->ThreadId
		== reinterpret_cast<osaThreadIdInternals*>(nonConstOther->Internals)->ThreadId;
#endif // CISST_WINDOWS    
}


void osaThreadId::ToStream(std::ostream & outputStream) const {
    osaThreadId * nonConstThis = const_cast<osaThreadId *>(this);
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_WINDOWS) || (CISST_SOLARIS)
    outputStream << reinterpret_cast<osaThreadIdInternals*>(nonConstThis->Internals)->ThreadId;
#endif // CISST_LINUX_RTAI || CISST_LINUX || CISST_WINDOWS || CISST_SOLARIS
    
#if (CISST_OS == CISST_DARWIN)
    outputStream << &(reinterpret_cast<osaThreadIdInternals*>(nonConstThis->Internals)->ThreadId);
#endif // CISST_DARWIN
}



osaThreadId osaGetCurrentThreadId(void)
{
    osaThreadId result;
#if (CISST_OS == CISST_WINDOWS)
    reinterpret_cast<osaThreadIdInternals*>(result.Internals)->ThreadId = GetCurrentThreadId();
#else
    // RTAI can also use pthread_self(). If a thread buddy (task) is created,
    // one can call rt_buddy(). The rt_whoami() and rt_lxrt_whoami() can't
    // be used because they are only defined in the kernel.
    reinterpret_cast<osaThreadIdInternals*>(result.Internals)->ThreadId = pthread_self();
#endif
    return result;
}


struct osaThreadInternals {

#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    pthread_t Thread;
#endif // CISST_LINUX_RTAI || CISST_LINUX || CISST_DARWIN || CISST_SOLARIS
#if (CISST_OS == CISST_WINDOWS)
    HANDLE Thread;
#endif // CISST_WINDOWS
};

#define INTERNALS(A) (reinterpret_cast<osaThreadInternals*>(Internals)->A)

// Constructor. Does nothing.
osaThread::osaThread() : Signal(), Valid(false)
{
    CMN_ASSERT(sizeof(Internals) >= SizeOfInternals());
}

    
// Destructor. Does nothing.
osaThread::~osaThread()
{}


unsigned int osaThread::SizeOfInternals(void) {
    return sizeof(osaThreadInternals);
}


// OS-dependent implementation of templated Create().  Note that void* parameters are used,
// which should be fine for all OS variations.
void osaThread::CreateInternal(const char *name, void* cb, void* userdata, bool newThread)
{
    CMN_LOG_INIT_VERBOSE << "osaThread::CreateInternal: create thread named: " << (name?name:"Unnamed") 
                         << (newThread?" (new)":"") << std::endl;
    
    if (newThread) {
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
        pthread_attr_t new_attr;
        pthread_attr_init(&new_attr);
        /*
          pthread_attr_setdetachstate(&new_attr, PTHREAD_CREATE_DETACHED);
          pthread_attr_setschedpolicy(&new_attr, policy);
          new_param.sched_priority = priority;
          pthread_attr_setschedparam(&new_attr, &new_param);
        */
        typedef void *(*CB_FuncType)(void *);
        pthread_create(&INTERNALS(Thread), &new_attr, (CB_FuncType)cb, userdata);
        pthread_attr_destroy(&new_attr);
        // Set thread Id
        reinterpret_cast<osaThreadIdInternals*>(ThreadId.Internals)->ThreadId = INTERNALS(Thread);
#endif // CISST_LINUX_RTAI || CISST_LINUX || CISST_DARWIN || CISST_SOLARIS
#if (CISST_OS == CISST_WINDOWS)
        DWORD threadId;
        INTERNALS(Thread) = CreateThread(
                              NULL,                       //default security attributes
                              0,                          //use default stack size
                              (LPTHREAD_START_ROUTINE) cb,     //thread 'start routine'
                              (LPVOID) userdata,          //argument to 'start routine'
                              0,                          //default creation flags
                              &threadId                   //keep Id for later, we might need it
                              );
        // Set thread Id
        reinterpret_cast<osaThreadIdInternals*>(ThreadId.Internals)->ThreadId = threadId;
#endif // CISST_WINDOWS
    }
    else {
        ThreadId = osaGetCurrentThreadId();
    }
    unsigned int i;
    if (name) {
        for (i=0; (i < sizeof(Name)-1) && (name[i]!='\0'); i++)
            Name[i] = name[i];
    } else {
        for (i=0; i < sizeof(Name)-1; i++)
            Name[i] = 'X';
    }
    Name[sizeof(Name)-1] = 0;
    Valid = true;
}

// PK: Should we check if Delete is being called on self?
void osaThread::Delete(void)
{
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    pthread_cancel (INTERNALS(Thread));
#endif // CISST_LINUX_RTAI || CISST_LINUX || CISST_DARWIN || CISST_SOLARIS

#if (CISST_OS == CISST_WINDOWS)
    TerminateThread(INTERNALS(Thread), 0);
#endif // CISST_WINDOWS

    Valid = false;

    // wait for thread to terminate
    Wait();
}


// PK: Should we check if Wait is being called on self?  Pthreads will detect this as a deadlock,
// what about Windows?
void osaThread::Wait(void) {
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    // will return EDEADLK if called from current thread
    pthread_join(INTERNALS(Thread), NULL);
#endif // CISST_LINUX_RTAI || CISST_LINUX || CISST_DARWIN || CISST_SOLARIS
#if (CISST_OS == CISST_WINDOWS)
    WaitForSingleObject(INTERNALS(Thread), INFINITE);
#endif // CISST_WINDOWS
}


PriorityType osaThread::GetPriority(void) const {
    return Priority;
}

void osaThread::SetPriority(PriorityType priority) {
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    struct sched_param param;
    param.sched_priority = priority;
    pthread_setschedparam(INTERNALS(Thread), Policy, &param);
    Priority = priority;
#endif // CISST_LINUX_RTAI || CISST_LINUX || CISST_DARWIN || CISST_SOLARIS
#if (CISST_OS == CISST_WINDOWS)
    // Not yet implemented.
#endif // CISST_WINDOWS
}

SchedulingPolicyType osaThread::GetSchedulingPolicy(void) {
    return Policy;
}


void osaThread::SetSchedulingPolicy(SchedulingPolicyType policy) {
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    struct sched_param param;
    param.sched_priority = Priority;
    pthread_setschedparam(INTERNALS(Thread), policy, &param);
    Policy = policy;
#endif // CISST_LINUX_RTAI || CISST_LINUX || CISST_DARWIN || CISST_SOLARIS
#if (CISST_OS == CISST_WINDOWS)
    // Not yet implemented.
#endif // CISST_WINDOWS
}


void osaThread::Sleep(double timeInSeconds)
{
    if (osaGetCurrentThreadId() != GetId()) {
        // If the current thread is not the same as this thread, call osaSleep.
        CMN_LOG_RUN_VERBOSE << "osaThread::Sleep: Calling osaSleep for external thread" << std::endl;
        osaSleep(timeInSeconds);
    }
    else
        Signal.Wait(timeInSeconds);
}


void osaCurrentThreadYield(void)
{
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    // Using sched_yield instead of pthread_yield because the former is in the POSIX standard
    // (pthread_yield was only in the draft of the standard).
    sched_yield();
#elif (CISST_OS == CISST_WINDOWS)
    // Other options are SleepEx(0,0) and Sleep(0)
    SwitchToThread();
#endif // CISST_WINDOWS
}


