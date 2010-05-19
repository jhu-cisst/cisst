/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Ankur Kapoor, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <iostream>

#include <cisstOSAbstraction/osaThreadBuddy.h>
#include <cisstCommon/cmnLogger.h>

#if (CISST_OS == CISST_LINUX_RTAI)
    #include <rtai_lxrt.h>
    #include <sys/mman.h> // for mlockall
    #include <rtai_lxrt.h> // for RTAI specific calls
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #include <iostream>
    const char __lock_filepath[] = "/var/lock/subsys/rtai";
    const char __lock_filename[] = "/var/lock/subsys/rtai/rtai.lock";
    char __lock_file[256];
#elif (CISST_OS == CISST_WINDOWS)
    #include <windows.h>
#elif (CISST_OS == CISST_QNX)
    #include <time.h>
#else
    #include <sys/time.h>
    #include <sys/select.h>
    #include <unistd.h>
#endif

void __os_init(void) 
{
#if (CISST_OS == CISST_LINUX_RTAI)
    struct stat st, st_1;
    stat(__lock_filename, &st);
    if (st.st_nlink > 1) {
        std::cerr << "__os_init Info: Another real-time program is already running" << std::endl;
    } 

    // no prog runs, no harm to start again!
    rt_set_oneshot_mode();
    start_rt_timer(nano2count(1*1000*1000)); //the period is ignored, so it doesnt matter

    // obtain a lock
    sprintf(__lock_file, "%s.%d", __lock_filename, getpid());
    link(__lock_filename, __lock_file);
    stat(__lock_filename, &st_1);
    if (st_1.st_nlink < st.st_nlink) {
        std::cerr << " __os_init Error: Couldnt create a link to lock" << std::endl;
    }
    //rt_linux_use_fpu(1);  // this is only needed if running inside the kernel.
                            // otherwise, simply use the uses_fpu flag at task creation.
#elif (CISST_OS == CISST_WINDOWS)
    //nothing is to be done now. later when we have our own dispatcher we might want to add
    //some code here
#elif (CISST_OS == CISST_QNX)
    // MJUNG: TODO: need to research how to setup hard realtime under QNX, if necessary.
#else // default unix   
    // nop
#endif
}

void __os_exit(void) 
{
#if (CISST_OS == CISST_LINUX_RTAI)
    // should check if more task (started by other processes are there)
    struct stat st;
    // unlink lock file
    unlink(__lock_file);
    stat(__lock_filename, &st);
    if (st.st_nlink > 1) {
        std::cerr << "__os_exit Info: Another real-time program is running, we don't stop the timer." << std::endl;
    } else {
        stop_rt_timer();
        return;
    }
#elif (CISST_OS == CISST_WINDOWS)
    //nothing is to be done now. later when we have our own dispatcher we might want to add
    //some code here
#else // default unix   
    // nop
#endif
}

struct osaThreadBuddyInternals {
#if (CISST_OS == CISST_LINUX_RTAI)
    // A pointer to the thread buddy on RTAI.
    RT_TASK *RTTask;
#elif (CISST_OS == CISST_WINDOWS)
    // A pointer to void on all other operating systems
    HANDLE WaitTimer;
    LARGE_INTEGER DueTime;
    bool IsSuspended;
    double TimerFrequency;
    char Name[6];
    osaThreadBuddyInternals() : WaitTimer(NULL) {}
#elif (CISST_OS == CISST_QNX)
    struct timespec DueTime;
    bool IsSuspended;
    char Name[6];
#else
    struct timeval DueTime;
    bool IsSuspended;
    char Name[6];
#endif // end of others
};

// Constructor. Allocates memory for thread buddy internal data.
osaThreadBuddy::osaThreadBuddy() {
    Data = new osaThreadBuddyInternals;
}

// Destructor. Frees memory for thread buddy internal data.
osaThreadBuddy::~osaThreadBuddy() {
    Delete();
    delete Data;
}


#if (CISST_OS == CISST_LINUX_RTAI)
void osaThreadBuddy::Create(const char *name, double period, int stack_size)
#else
void osaThreadBuddy::Create(const char *name, double period, int CMN_UNUSED(stack_size))
#endif
{
    Period = period;
#if (CISST_OS == CISST_LINUX_RTAI)
    // nam2num converts the character string 'name' to a long, using just the first
    // 6 characters.
    Data->RTTask = rt_task_init_schmod(nam2num(name), 0, stack_size, 0, SCHED_FIFO, 0xF);
    if (!Data->RTTask) {
        CMN_LOG_INIT_ERROR << "OOPS!!! Couldn't create Proxy object" << std::endl;
        exit(1);
    }
    if (rt_task_use_fpu(Data->RTTask, 1) < 0) {
        CMN_LOG_INIT_ERROR << "OOPS!!! FPU couldn't be allocated" << std::endl;
    }
    // maybe this should be just before we make code real-time
    // causes failure of memory intensive calls such as openGL
    // so should be done with soft real time.
    // or maybe should have something like "really-soft-real-time"
    mlockall(MCL_CURRENT | MCL_FUTURE);
    if (IsPeriodic())
        rt_task_make_periodic_relative_ns(Data->RTTask, 0, (unsigned long)period);

#elif (CISST_OS == CISST_WINDOWS)
    Data->DueTime.QuadPart = 0UL;
    Data->IsSuspended = false;
    for (unsigned int i = 0; i < sizeof(Data->Name)-1; i++) Data->Name[i] = name[i];
    Data->Name[sizeof(Data->Name)-1] = 0;
    if (IsPeriodic()) {
        // We don't name the timer (third parameter) because if the name has already
        // been used, CreateWaitableTimer will return a handle to the existing timer.
        Data->WaitTimer = CreateWaitableTimer(NULL, true, NULL);
        if (Data->WaitTimer == NULL) {
            CMN_LOG_INIT_ERROR << "OOPS! Couldn't create a waitable timer" << std::endl;
        }
    }

#elif (CISST_OS == CISST_QNX)
    Data->DueTime.tv_sec = 0;
    Data->DueTime.tv_nsec = 0;
    Data->IsSuspended = false;
    for (unsigned int i = 0; i < sizeof(Data->Name); i++) Data->Name[i] = name[i];
    Data->Name[sizeof(Data->Name)-1] = 0;

#else // default unix
    Data->DueTime.tv_sec = 0;
    Data->DueTime.tv_usec = 0;
    Data->IsSuspended = false;
    for (unsigned int i = 0; i < sizeof(Data->Name); i++) Data->Name[i] = name[i];
    Data->Name[sizeof(Data->Name)-1] = 0;
#endif    
}

void osaThreadBuddy::Delete()
{
#if (CISST_OS == CISST_LINUX_RTAI)
    if (Data->RTTask) {
        rt_task_delete(Data->RTTask);
    }
#elif (CISST_OS == CISST_WINDOWS)
    if (Data->WaitTimer != NULL) {
        CloseHandle(Data->WaitTimer);
        Data->WaitTimer = NULL;
    }
#endif // CISST_WINDOWS
}

void osaThreadBuddy::WaitForPeriod(void) 
{
    if (!IsPeriodic()) return;

#if (CISST_OS == CISST_LINUX_RTAI)
    rt_task_wait_period();

#elif (CISST_OS == CISST_WINDOWS)
    // this might change in future when we introduce our own
    // scheduler/dispatcher but even then our sleep might be better
    // that using Sleep() provided by windows;
    LARGE_INTEGER dueTime;
    LONGLONG waitTime = (LONGLONG)(-Period/100);
    dueTime.QuadPart = waitTime; //dueTime is in 100ns
    SetWaitableTimer(Data->WaitTimer, &dueTime, 0, NULL, NULL, 0);
    WaitForSingleObject(Data->WaitTimer, INFINITE);

#elif (CISST_OS == CISST_QNX)
    /* MJUNG: Here nanosleep was used to replace pselect() but there is another function:
       nanospin() - BUSY-WAIT WITHOUT THREAD BLOCKING for a period of time.
       (cf. nanosleep() - SUSPEND a thread until a timeout or signal occurs) 
       
       Refer to the folowing note: 

       The nanospin() functions are designed for use with hardware that requires short 
       time delays between accesses. You should use them to delay only for times less 
       than a few milliseconds. For longer delays, use the POSIX timer_*() functions. 
       (http://www.qnx.com/developers/docs/6.4.1/neutrino/lib_ref/n/nanospin.html)
    */
    struct timespec timeout;
    timeout.tv_sec = 0;
    timeout.tv_nsec = (long)(Period);
    if (nanosleep(&timeout, NULL) != 0) {
        CMN_LOG_RUN_WARNING << "WaitForPeriod: nanosleep() was interrupted by a signal" << std::endl;
    }

#else // default unix
    struct timespec timeout;
    timeout.tv_sec = 0;
    timeout.tv_nsec = (long)(Period);
    pselect(0, NULL, NULL, NULL, &timeout, NULL);
#endif
}

void osaThreadBuddy::WaitForRemainingPeriod(void)
{
#if (CISST_OS == CISST_LINUX_RTAI)
    if (IsPeriodic())
        rt_task_wait_period();

#elif (CISST_OS == CISST_WINDOWS)
    if (!IsPeriodic())
        return;
    // In a periodic loop at the end we want to wait only for the
    // portion of time that is left of the period. Now if someone else
    // was generating the time event, then we wait for that
    // event. but for now we are keeping our own time, so we must
    // wait for remaining portion of the time.
    LARGE_INTEGER liTimerFrequency, liTimeRemaining, liTimeNow;
    double elapsedTime, timeRemaining;
    do {
#if 1
        if (Data->DueTime.QuadPart == 0) {
            // this is the first time it is being called
#if 0
            SYSTEMTIME sysnow;
            FILETIME now;
            GetSystemTime(&sysnow);
            SystemTimeToFileTime(&sysnow, &now);
            ULARGE_INTEGER ulnow;
            ulnow.HighPart = now.dwHighDateTime;
            ulnow.LowPart = now.dwLowDateTime;
            Data->DueTime.QuadPart = ulnow.QuadPart + Period;
#endif
            if (QueryPerformanceCounter(&Data->DueTime) ==0 ) {
                CMN_LOG_INIT_ERROR << "No performance counter available" << std::endl;
            }
            if (QueryPerformanceFrequency(&liTimerFrequency) == 0) {
                CMN_LOG_INIT_ERROR << "No performance counter available" << std::endl;
            }
            // QueryPerformanceFrequency returns counts per second. Divide by 1e9 to
            // get counts per nanosecond.
            Data->TimerFrequency = (double)liTimerFrequency.QuadPart/(double)(1000000000);
        }
        QueryPerformanceCounter(&liTimeNow);
        elapsedTime = (double)(liTimeNow.QuadPart - Data->DueTime.QuadPart);
        elapsedTime /= Data->TimerFrequency; //this is in nano seconds
        timeRemaining = Period - elapsedTime; //this is in nano seconds
        liTimeRemaining.QuadPart = (LONGLONG)(-timeRemaining/100);

        SetWaitableTimer(Data->WaitTimer, &liTimeRemaining, 0, NULL, NULL, 0);//dueTime is in 100ns
        WaitForSingleObject(Data->WaitTimer, INFINITE);
        QueryPerformanceCounter(&Data->DueTime);
#endif

#if 0
    LARGE_INTEGER dueTime;
    LONGLONG waitTime = (LONGLONG)(-Period/100);
    dueTime.QuadPart = waitTime; //dueTime is in 100ns
    SetWaitableTimer(Data->WaitTimer, &dueTime, 0, NULL, NULL, 0);
    WaitForSingleObject(Data->WaitTimer, INFINITE);
#endif
    } while (Data->IsSuspended);

#elif (CISST_OS == CISST_QNX)
    if (!IsPeriodic())
        return;
    double elapsedTime, timeRemaining;
    struct timespec timeNow, timeLater;
    struct timespec timeSleep;
    do {
        if (Data->DueTime.tv_nsec == 0 && Data->DueTime.tv_sec == 0) {
            // this is the first time this is being called;
            if (clock_gettime(CLOCK_REALTIME, &Data->DueTime) == -1) {
                CMN_LOG_RUN_ERROR << "WaitForRemainingPeriod(): failed to get time information" << std::endl;
            }
        }
        if (clock_gettime(CLOCK_REALTIME, &timeNow) == -1) {
            CMN_LOG_RUN_ERROR << "WaitForRemainingPeriod(): failed to get time information" << std::endl;
        }
        elapsedTime = (double)(1000 * 1000 * 1000 * (timeNow.tv_sec - Data->DueTime.tv_sec) 
                + (timeNow.tv_nsec - Data->DueTime.tv_nsec)); // in nsec
        timeRemaining = Period - elapsedTime;
        timeSleep.tv_sec = 0;
        timeSleep.tv_nsec = (long)timeRemaining;
        if (nanosleep(&timeSleep, NULL) != 0) {
            CMN_LOG_RUN_WARNING << "WaitForPeriod: nanosleep() was interrupted by a signal" << std::endl;
        }
        if (clock_gettime(CLOCK_REALTIME, &timeLater) == -1) {
            CMN_LOG_RUN_ERROR << "WaitForRemainingPeriod(): failed to get time information" << std::endl;
        }
        Data->DueTime.tv_sec = timeLater.tv_sec;
        Data->DueTime.tv_nsec = timeLater.tv_nsec;
    } while (Data->IsSuspended);

#else // default unix
    if (!IsPeriodic())
        return;
    double elapsedTime, timeRemaining;
    struct timeval timeNow, timeLater;
    struct timespec timeSleep;
    do {
        if (Data->DueTime.tv_usec == 0 && Data->DueTime.tv_sec == 0) {
            // this is the first time this is being called;
            gettimeofday(&Data->DueTime, NULL);
        }
        gettimeofday(&timeNow, NULL);
        elapsedTime = (double)(1000 * 1000 * (timeNow.tv_sec - Data->DueTime.tv_sec)
                + (timeNow.tv_usec - Data->DueTime.tv_usec)); // in usec
        timeRemaining = Period - elapsedTime*1000;
        timeSleep.tv_sec = 0;
        timeSleep.tv_nsec = (long)timeRemaining;
        pselect(0, NULL, NULL, NULL, &timeSleep, NULL);
        gettimeofday(&timeLater, NULL);
        Data->DueTime.tv_sec = timeLater.tv_sec;
        Data->DueTime.tv_usec = timeLater.tv_usec;
    } while (Data->IsSuspended);
#endif
}

void osaThreadBuddy::MakeHardRealTime(void) 
{
#if (CISST_OS == CISST_LINUX_RTAI)
    rt_make_hard_real_time();
#else
    //a NOP on all other os
#endif
}

void osaThreadBuddy::MakeSoftRealTime(void) 
{
#if (CISST_OS == CISST_LINUX_RTAI)
    rt_make_soft_real_time();
#else
    //a NOP on all other os
#endif
}

void osaThreadBuddy::Resume(void) 
{
#if (CISST_OS == CISST_LINUX_RTAI)
    rt_task_resume(Data->RTTask);
#else
    Data->IsSuspended = false;
#endif
}

void osaThreadBuddy::Suspend(void) 
{
#if (CISST_OS == CISST_LINUX_RTAI)
    rt_task_suspend(Data->RTTask);
#else 
    Data->IsSuspended = true;
#endif
}

// Lock stack growth
void osaThreadBuddy::LockStack() 
{
#if (CISST_OS == CISST_LINUX_RTAI)
    mlockall( MCL_CURRENT | MCL_FUTURE );
#endif
}

// Unlock stack
void osaThreadBuddy::UnlockStack() 
{
#if (CISST_OS == CISST_LINUX_RTAI)
    munlockall();
#endif
}

