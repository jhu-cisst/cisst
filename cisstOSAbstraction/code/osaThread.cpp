/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Ankur Kapoor, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnAssert.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaSleep.h>

#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX)
    #include <pthread.h>
    #include <sched.h>
    #include <string.h>
#elif (CISST_OS == CISST_LINUX_XENOMAI)
#include <native/task.h>
#elif (CISST_OS == CISST_WINDOWS)
    #include <windows.h>
#endif

struct osaThreadIdInternals {
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX)
    /*! OS dependent thread id. */
    pthread_t ThreadId;
#elif (CISST_OS == CISST_LINUX_XENOMAI)
    RT_TASK* Task;
#elif (CISST_OS == CISST_WINDOWS)
    DWORD ThreadId;
#endif // CISST_WINDOWS
};


// Constructor. Does nothing.
osaThreadId::osaThreadId(void) : Valid(false)
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
// This probably should check whether ThreadIds are valid, but that may break existing code, since
// Valid flag was recently added (10/2019).
bool osaThreadId::Equal(const osaThreadId & other) const 
{
    osaThreadId * nonConstThis = const_cast<osaThreadId *>(this);
    osaThreadId * nonConstOther = const_cast<osaThreadId *>(&other);
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX)
    return (pthread_equal(reinterpret_cast<osaThreadIdInternals*>(nonConstThis->Internals)->ThreadId,
                          reinterpret_cast<osaThreadIdInternals*>(nonConstOther->Internals)->ThreadId) != 0 ? true : false); 
#elif (CISST_OS == CISST_LINUX_XENOMAI)

    RT_TASK* t1 = reinterpret_cast<osaThreadIdInternals*>(nonConstThis->Internals)->Task;
    RT_TASK* t2 = reinterpret_cast<osaThreadIdInternals*>(nonConstOther->Internals)->Task;

    if( t1 != NULL && t2 != NULL ){
        int same = rt_task_same( t1, t2 );
        return (same != 0) ? true : false;
    }
    else return false;
#elif (CISST_OS == CISST_WINDOWS)
    return
        reinterpret_cast<osaThreadIdInternals*>(nonConstThis->Internals)->ThreadId
		== reinterpret_cast<osaThreadIdInternals*>(nonConstOther->Internals)->ThreadId;
#endif
}


// MJ: Don't use this comparator on Mac and Xenomai
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_WINDOWS)
// This probably should check whether ThreadIds are valid, but that may break existing code, since
// Valid flag was recently added (10/2019).
bool osaThreadId::operator()(const osaThreadId & lhs, const osaThreadId & rhs) const
{
    osaThreadId * _lhs = const_cast<osaThreadId *>(&lhs);
    osaThreadId * _rhs = const_cast<osaThreadId *>(&rhs);

    osaThreadIdInternals * _lhsInternal = reinterpret_cast<osaThreadIdInternals*>(_lhs->Internals);
    osaThreadIdInternals * _rhsInternal = reinterpret_cast<osaThreadIdInternals*>(_rhs->Internals);

    // MJ TODO: FIX THIS LATER
    // We cannot directly compare pthread_t that should be handled as an opaque type.
    // As a temporary solution, however, use unsigned int assuming pthread_t can be 
    // converted to unsigned int.  Other software packages (such as OpenSSL and ZeroC Ice)
    // are doing this:
    // http://www.zeroc.com/forums/bug-reports/44-freebsd-compilation-problem-pthread_t-not-unsigned-long.html
#if (CISST_OS == CISST_WINDOWS)
    return (_lhsInternal->ThreadId < _rhsInternal->ThreadId);
#else
    return ((unsigned int) _lhsInternal->ThreadId < (unsigned int) _rhsInternal->ThreadId);
#endif
}
#endif


void osaThreadId::ToStream(std::ostream & outputStream) const
{
    if (this->Valid) {
        osaThreadId * nonConstThis = const_cast<osaThreadId *>(this);
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_WINDOWS) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX)
        outputStream << reinterpret_cast<osaThreadIdInternals*>(nonConstThis->Internals)->ThreadId;
#elif (CISST_OS == CISST_LINUX_XENOMAI)
        outputStream << reinterpret_cast<osaThreadIdInternals*>(nonConstThis->Internals)->Task;
#elif (CISST_OS == CISST_DARWIN)
        outputStream << &(reinterpret_cast<osaThreadIdInternals*>(nonConstThis->Internals)->ThreadId);
#endif
    }
    else
        outputStream << "invalid";
}



osaThreadId osaGetCurrentThreadId(void)
{
    osaThreadId result;
#if (CISST_OS == CISST_WINDOWS)
    reinterpret_cast<osaThreadIdInternals*>(result.Internals)->ThreadId = GetCurrentThreadId();
#elif (CISST_OS == CISST_LINUX_XENOMAI)
    reinterpret_cast<osaThreadIdInternals*>(result.Internals)->Task = rt_task_self();
#else
    // RTAI can also use pthread_self(). If a thread buddy (task) is created,
    // one can call rt_buddy(). The rt_whoami() and rt_lxrt_whoami() can't
    // be used because they are only defined in the kernel.
    reinterpret_cast<osaThreadIdInternals*>(result.Internals)->ThreadId = pthread_self();
#endif
    result.Valid = true;
    return result;
}


struct osaThreadInternals {

#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX)
    pthread_t Thread;
#elif (CISST_OS == CISST_LINUX_XENOMAI)
    RT_TASK Task;
#elif (CISST_OS == CISST_WINDOWS)
    HANDLE Thread;
#endif
};

#define INTERNALS(A) (reinterpret_cast<osaThreadInternals*>(Internals)->A)

// Constructor. Does nothing.
osaThread::osaThread() : Signal(), Valid(false), Running(false)
{
    CMN_ASSERT(sizeof(Internals) >= SizeOfInternals());
}

    
// Destructor. Does nothing.
osaThread::~osaThread(){
#if (CISST_OS == CISST_LINUX_XENOMAI)
    /* 
       if( rt_task_self() != NULL )                                                                                                               
       { rt_task_delete( rt_task_self() ); }                                                                                                  
       else{                                                                                                                                      
       CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS                                                                                                 
    << "Calling thread is not a Xenomai task."                                                                         
    << std::endl;                                                                                                      
    }                                                                                                                                          
    */
#endif
}


unsigned int osaThread::SizeOfInternals(void) {
    return sizeof(osaThreadInternals);
}


// OS-dependent implementation of templated Create().  Note that void* parameters are used,
// which should be fine for all OS variations.
void osaThread::CreateInternal(const char *name, void* cb, void* userdata)
{
    CMN_LOG_INIT_VERBOSE << "osaThread::CreateInternal: create thread named: " << (name?name:"Unnamed") 
                         << std::endl;
    
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX)
    pthread_attr_t new_attr;
    pthread_attr_init(&new_attr);
    /*
      pthread_attr_setdetachstate(&new_attr, PTHREAD_CREATE_DETACHED);
      pthread_attr_setschedpolicy(&new_attr, policy);
      new_param.sched_priority = priority;
      pthread_attr_setschedparam(&new_attr, &new_param);
    */
    typedef void *(*CB_FuncType)(void *);
    int retval = pthread_create(&INTERNALS(Thread), &new_attr, (CB_FuncType)cb, userdata);
    if( retval != 0 ){
        CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                          << "pthread_create failed. "
                          << strerror(retval) << ": " << retval 
                          << std::endl;
    }
    pthread_attr_destroy(&new_attr);
    // Set thread Id
    reinterpret_cast<osaThreadIdInternals*>(ThreadId.Internals)->ThreadId = INTERNALS(Thread);
#elif (CISST_OS == CISST_LINUX_XENOMAI)

    typedef void (*CB_FuncType)(void *);
    int retval = 0;
    retval = rt_task_spawn( &INTERNALS(Task),
                            0,
                            0,
                            89,
                            T_FPU | T_JOINABLE,
                            (CB_FuncType)cb,
                            (void*)userdata );
    if( retval != 0 ){
#if 0
        CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                          << "rt_task_spawn failed. "
                          << strerror(retval) << ": " << retval
                          << std::endl;
#endif
        std::string err;
        switch (-retval) {
        case ENOMEM: err = "ENOMEM: the system fails to get enough dynamic memory from the global real-time heap in order to create the new task's stack space or register the task."; break;
        case EEXIST: err = "EEXIST: the name is already in use by some registered object."; break;
        case EPERM: err = "EPERM: this service was called from an asynchronous context."; break;
        }
        CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                          << "rt_task_spawn failed: " << err << std::endl;
    }
    // copy the address
    reinterpret_cast<osaThreadIdInternals*>(ThreadId.Internals)->Task = &INTERNALS(Task);
#elif (CISST_OS == CISST_WINDOWS)
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

    ThreadId.Valid = true;
    SetThreadName(name);
    Valid = true;
    Running = true;
}

void osaThread::SetThreadName(const char* name)
{
    unsigned int i;
    if (name) {
        for (i=0; (i < sizeof(Name)-1) && (name[i]!='\0'); i++)
            Name[i] = name[i];
    } else {
        for (i=0; i < sizeof(Name)-1; i++)
            Name[i] = 'X';
    }
    Name[sizeof(Name)-1] = 0;
}

// PK: Should we check if Delete is being called on self?
void osaThread::Delete(void)
{
    if (Valid) {
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX)
        int retval = pthread_cancel (INTERNALS(Thread));
        if( retval != 0 ){
            CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                              << "pthread_cancel failed. " 
                              << strerror(retval) << ": " << retval 
                              << std::endl;
        }
#elif (CISST_OS == CISST_LINUX_XENOMAI)
        /*                                                                                                                                    
        int retval = 0;                                                                                                                       
        retval = rt_task_delete( &INTERNALS( Task ) );                                                                                        
        if( retval != 0 ){                                                                                                                    
        CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS                                                                                              
        << "rt_task_delete failed. "                                                                                     
        << strerror(retval) << ": " << retval                                                                            
        << std::endl;                                                                                                    
        }                                                                                                                                     
        */
#elif (CISST_OS == CISST_WINDOWS)
        TerminateThread(INTERNALS(Thread), 0);
#endif // CISST_WINDOWS

        Valid = false;
        Running = false;

        // wait for thread to terminate
        Wait();
    }
}


// PK: Should we check if Wait is being called on self?  Pthreads will detect this as a deadlock,
// what about Windows?
void osaThread::Wait(void) 
{
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX)
    // will return EDEADLK if called from current thread
    int retval = pthread_join(INTERNALS(Thread), NULL);
    if( retval != 0 ){
        CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                          << "pthread_join failed. "
                          << strerror(retval) << ": " << retval 
                          << std::endl;
    }
#elif (CISST_OS == CISST_LINUX_XENOMAI)

    int retval = 0;
    retval = rt_task_join( &INTERNALS( Task ) );
    if( retval != 0 ){
        CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                          << "rt_task_join failed. "
                          << strerror(retval) << ": " << retval
                          << std::endl;
    }

#elif (CISST_OS == CISST_WINDOWS)
    WaitForSingleObject(INTERNALS(Thread), INFINITE);
#endif // CISST_WINDOWS
}


PriorityType osaThread::GetPriority(void) const 
{
    return Priority;
}

void osaThread::SetPriority(PriorityType priority) 
{
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX)
    struct sched_param param;
    param.sched_priority = priority;
    pthread_setschedparam(INTERNALS(Thread), Policy, &param);
    Priority = priority;
#elif (CISST_OS == CISST_LINUX_XENOMAI)

    int retval = 0;
    retval = rt_task_set_priority( &INTERNALS( Task ), priority );
    if( retval != 0 ){
        CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                          << "rt_task_set_priority failed. "
                          << strerror(retval) << ": " << retval
                          << std::endl;
    }
    Priority = priority;

#elif (CISST_OS == CISST_WINDOWS)
    // Not yet implemented.
#endif // CISST_WINDOWS
}

SchedulingPolicyType osaThread::GetSchedulingPolicy(void) 
{
    return Policy;
}


void osaThread::SetSchedulingPolicy(SchedulingPolicyType policy) 
{
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX)
    struct sched_param param;
    param.sched_priority = Priority;
    pthread_setschedparam(INTERNALS(Thread), policy, &param);
    Policy = policy;
#elif (CISST_OS == CISST_LINUX_XENOMAI)
#elif (CISST_OS == CISST_WINDOWS)
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
    else {
        Running = false;
        Signal.Wait(timeInSeconds);
        Running = true;
    }
}


void osaCurrentThreadYield(void)
{
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX)
    // Using sched_yield instead of pthread_yield because the former is in the POSIX standard
    // (pthread_yield was only in the draft of the standard).
    sched_yield();

#elif (CISST_OS == CISST_LINUX_XENOMAI)

    int retval = 0;
    retval = rt_task_yield();
    if( retval != 0 ){
        CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                          << "rt_task_yield failed. "
                          << strerror(retval) << ": " << retval
                          << std::endl;
    }

#elif (CISST_OS == CISST_WINDOWS)
    // Other options are SleepEx(0,0) and Sleep(0)
    SwitchToThread();
#endif // CISST_WINDOWS
}


