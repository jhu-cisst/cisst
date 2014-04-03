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

#include <iostream>

#include <cisstOSAbstraction/osaThreadBuddy.h>
#include <cisstOSAbstraction/osaTimeServer.h>
#include <cisstCommon/cmnUnits.h>
#include <cisstCommon/cmnLogger.h>

#if (CISST_OS == CISST_LINUX_RTAI)
    #include <sys/mman.h> // for mlockall
    #include <rtai_lxrt.h> // for RTAI specific calls
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>
    #include <fstream> // to access /proc/modules
#ifdef CMAKE_HAVE_MODULE_H
    #include <linux/module.h> // to query kernel module information
#endif
    const char __lock_filepath[] = "/var/lock/subsys/rtai";
    const char __lock_filename[] = "/var/lock/subsys/rtai/rtai.lock";
    char __lock_file[256];
#elif (CISST_OS == CISST_LINUX_XENOMAI)

#include <native/task.h>
#include <native/timer.h>
#include <sys/mman.h> // for mlockall                                                                                                      

#elif (CISST_OS == CISST_WINDOWS)
    #include <windows.h>
#elif (CISST_OS == CISST_QNX)
    #include <sys/siginfo.h>
    #include <sys/neutrino.h>
# include <time.h>
#else
    #include <sys/time.h>
    #include <sys/select.h>
    #include <unistd.h>
#endif

#if (CISST_OS == CISST_LINUX_RTAI)
int GetStat(const char * path, struct stat * st)
{
    int ret = stat(path, st);
    if (ret == -1) { // 0: success, -1: error
        std::string msg;
        switch (errno) {
            case EACCES: msg = "Search permission is denied for one of the directories in the path prefix of path"; break;
            case EBADF: msg = "filedes is bad."; break;
            case EFAULT: msg = "Bad address."; break;
            case ELOOP: msg = "Too many symbolic links encountered while traversing the path."; break;
            case ENAMETOOLONG: msg = "File name too long."; break;
            case ENOENT: msg = "A component of the path does not exist, or the path is an empty string."; break;
            case ENOMEM: msg = "Out of memory (i.e. kernel memory)."; break;
            case ENOTDIR: msg = "A component of the path is not a directory."; break;
            default: msg = "Unknown";
        }
        std::cerr << "__osa_init stat() error: " << msg << "(filename: " << path << ")" << std::endl;
    }

    return ret;
}
#endif

void __os_init(void) 
{
#if (CISST_OS == CISST_LINUX_RTAI)
    struct stat st, st_1;
    int ret = GetStat(__lock_filename, &st);
    if (ret == 0) {
        if (st.st_nlink > 1) {
            std::cerr << "__os_init Info: Another real-time program is already running" << std::endl;
        } 
    } else {
        std::cerr << "__os_init Info: failed to get lock file status" << std::endl;
    }

    // Check if all the required realtime kernel modules are loaded properly.
    // MJUNG: This is to prevent cisst from crashing with segmentation fault when
    // some of the kernel modules are not loaded.
    bool allModulesLoaded = true;

    // list of required real-time kernel modules
    std::vector<std::string> RTModuleNames;
    //RTModuleNames.push_back("rtai_hal");
    RTModuleNames.push_back("rtai_lxrt");
    RTModuleNames.push_back("rtai_sem");
    //RTModuleNames.push_back("rtai_shm");
    //RTModuleNames.push_back("rtai_mbx");
    //RTModuleNames.push_back("rtai_fifos");

    // list of kernel modules currently loaded
    std::vector<std::string> loadedModuleNames;
    std::string line;
    std::ifstream moduleList("/proc/modules");
    if (moduleList.is_open()) {
        while (!moduleList.eof()) {
            getline(moduleList, line);
            loadedModuleNames.push_back(line);
        }
        moduleList.close();

        bool found;
        for (size_t i = 0; i < RTModuleNames.size(); ++i) {
            found = false;
            for (size_t j = 0; j < loadedModuleNames.size(); ++j) {
                found |= (loadedModuleNames[j].find(RTModuleNames[i]) != std::string::npos);
                if (found) {
                    break;
                }
            }
            if (!found) {
                std::cerr << "Check real-time module: " << RTModuleNames[i]
                    << " is missing. Please load the module." << std::endl;
            }

            allModulesLoaded &= found;
        }

        if (!allModulesLoaded) {
            std::cerr << "Please load the missing module(s)." << std::endl;
            exit(1);
        }
    } else {
        // Check version of Linux kernel. query_module is only present up until
        // kernel 2.4 and was removed in Linux 2.6.
#if defined(CMAKE_HAVE_MODULE_H) && defined(QM_INFO)
        // Just in case <linux/module.h> does not declare query_module (e.g., Fedora kernel 2.4.20)
        extern "C" int query_module(const char *name, int which, void *buf, size_t bufsize, size_t *ret);

        CMN_LOG_INIT_VERBOSE << "Linux kernel version: 2.4.x" << std::endl;

        bool found;
        int queryResult;
        size_t ret;
        char buf[1024] = "";
        for (size_t i = 0; i < RTModuleNames.size(); ++i) {
            found = false;
            queryResult = query_module(RTModuleNames[i].c_str(), QM_INFO, buf, sizeof(buf), &ret);
            if (queryResult != 0) {
                found = false;
                switch (queryResult) {
                    case ENOENT: 
                        std::cerr << "Check real-time module: " << RTModuleNames[i]
                            << " is missing. Please load the module." << std::endl;
                        break;
                    default:
                        std::cerr << "Check real-time module: unknown error" << std::endl;
                }
            } else {
                found = true;
            }

            allModulesLoaded &= found;
        }

        if (!allModulesLoaded) {
            std::cerr << "Please load the missing module(s)." << std::endl;
            exit(1);
        }
#else
        CMN_LOG_INIT_VERBOSE << "Please make sure all real-time kernel modules are loaded beforehand" << std::endl;
#endif
    }

    // no prog runs, no harm to start again!
    rt_set_oneshot_mode();
    start_rt_timer(nano2count(1*1000*1000)); //the period is ignored, so it doesnt matter

    // obtain a lock
    sprintf(__lock_file, "%s.%d", __lock_filename, getpid());
    link(__lock_filename, __lock_file);
    ret = GetStat(__lock_filename, &st_1);
    if (ret == 0) {
        if (st_1.st_nlink < st.st_nlink) {
            std::cerr << " __os_init Error: Couldnt create a link to lock" << std::endl;
        }
    } else {
        std::cerr << "__os_init Error: failed to get lock file status" << std::endl;
    }
    //rt_linux_use_fpu(1);  // this is only needed if running inside the kernel.
                            // otherwise, simply use the uses_fpu flag at task creation.
#elif (CISST_OS == CISST_WINDOWS)
    //nothing is to be done now. later when we have our own dispatcher we might want to add
    //some code here
#elif (CISST_OS == CISST_QNX)
    // nop
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
    int ret = GetStat(__lock_filename, &st);
    if (ret == 0) {
        if (st.st_nlink > 1) {
            std::cerr << "__os_exit Info: Another real-time program is running, we don't stop the timer." << std::endl;
        } else {
            stop_rt_timer();
            return;
        }
    } else {
        std::cerr << "__os_exit Info: failed to get lock file status" << std::endl;
    }
#elif (CISST_OS == CISST_WINDOWS)
    //nothing is to be done now. later when we have our own dispatcher we might want to add
    //some code here
#else // default unix   
    // nop
#endif
}

struct osaThreadBuddyInternals {
    bool IsSuspended;
#if (CISST_OS == CISST_LINUX_RTAI)
    // A pointer to the thread buddy on RTAI.
    RT_TASK *RTTask;
#elif (CISST_OS == CISST_LINUX_XENOMAI)
    // not internals for xenomai
#elif (CISST_OS == CISST_WINDOWS)
    // A pointer to void on all other operating systems
    HANDLE WaitTimer;
    LARGE_INTEGER DueTime;
    double TimerFrequency;
    char Name[6];
    osaThreadBuddyInternals() : WaitTimer(NULL) {}
#elif (CISST_OS == CISST_QNX)
    //struct timespec DueTime;
    //char Name[6];
    int coid;          // connection id
    int chid;          // channel id
#else
    struct timeval DueTime;
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
void osaThreadBuddy::Create(const char *name, double period, int stack_size )
#endif
{
   osaAbsoluteTime tv;
   tv.sec = (long)( period * cmn_ns );               // number of seconds
   tv.nsec = (long)( period - tv.sec*1000000000 );   // remainder

   Create( name, tv, stack_size );

}

   
#if (CISST_OS == CISST_LINUX_RTAI)
void osaThreadBuddy::Create(const char *name, const osaAbsoluteTime& tv, int stack_size)
#else
void osaThreadBuddy::Create(const char *name, const osaAbsoluteTime& tv, int CMN_UNUSED(stack_size))
#endif
{
   
    Period = tv.sec*1000000000 + tv.nsec;
    Data->IsSuspended = false;

#if (CISST_OS == CISST_LINUX_RTAI)
    // nam2num converts the character string 'name' to a long, using just the first
    // 6 characters.
    Data->RTTask = rt_task_init_schmod(nam2num(name), 0, stack_size, 0, SCHED_FIFO, 0xF);
    if (!Data->RTTask) {
        std::cerr << "osaThreadBuddy: failed to initialize real-time task: \"" << name << "\"" << std::endl;
        std::cerr << "Real-time task needs root permission to run properly. Check if you are root." << std::endl;
        exit(1);
    }
    if (rt_task_use_fpu(Data->RTTask, 1) < 0) {
        std::cerr << "osaThreadBuddy: failed to allocate FPU" << std::endl;
        std::cerr << "Real-time task needs root permission to run properly. Check if you are root." << std::endl;
        exit(1);
    }
    // maybe this should be just before we make code real-time
    // causes failure of memory intensive calls such as openGL
    // so should be done with soft real time.
    // or maybe should have something like "really-soft-real-time"
    mlockall(MCL_CURRENT | MCL_FUTURE);
    if (IsPeriodic()) {
        if (0 != rt_task_make_periodic_relative_ns(Data->RTTask, 0, (unsigned long)Period)) {
            std::cerr << "osaThreadBuddy: failed to make task run periodically" << std::endl;
            std::cerr << "Real-time task needs root permission to run properly. Check if you are root." << std::endl;
            exit(1);
        }
    }

#elif (CISST_OS == CISST_LINUX_XENOMAI)

    RT_TASK* task = rt_task_self();
    if( task != NULL ){
        SRTIME ns = (SRTIME)(Period);
        int retval = 0;
        retval = rt_task_set_periodic( task, TM_NOW, ns );
        if( retval != 0 ){
            CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                              << "rt_task_set_periodic failed. "
                              << strerror(retval) << ": " << retval
                              << std::endl;
        }
    }
    else{
        CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
                            << "Calling thread is not a Xenomai task."
                            << std::endl;
    }
#elif (CISST_OS == CISST_WINDOWS)
    Data->DueTime.QuadPart = 0UL;
    for (unsigned int i = 0; i < sizeof(Data->Name)-1; i++) Data->Name[i] = name[i];
    Data->Name[sizeof(Data->Name)-1] = 0;
    if (IsPeriodic()) {
        // We don't name the timer (third parameter) because if the name has already
        // been used, CreateWaitableTimer will return a handle to the existing timer.
        Data->WaitTimer = CreateWaitableTimer(NULL, true, NULL);
        if (Data->WaitTimer == NULL) {
            CMN_LOG_INIT_ERROR << "osaThreadBuddy: failed to create a waitable timer" << std::endl;
        }
    }

#elif (CISST_OS == CISST_QNX)

   timer_t timerid;                     // timer ID
   struct itimerspec timer;             // the timer data struct
   struct sigevent event;               // event to deliver
   
   // create a communication channel (maintain thread priority)
   Data->chid = ChannelCreate( _NTO_CHF_FIXED_PRIORITY );
   if( Data->chid == -1 )
     { CMN_LOG_INIT_ERROR << "Channel not created" << std::endl; }
   
   // create a connection on the channel
   Data->coid = ConnectAttach( 0, 0, Data->chid, 0, 0 );
   if( Data->coid == -1 )
     { CMN_LOG_INIT_ERROR << "Connection not created" << std::endl; }
   
   // set the pulse event
   SIGEV_PULSE_INIT( &event, Data->coid, SIGEV_PULSE_PRIO_INHERIT, 1, 0 );
   
   // create the timer for the pulse event
   if( timer_create( CLOCK_REALTIME, &event, &timerid ) == -1 )
     { CMN_LOG_INIT_ERROR << "Timer not created" << std::endl; }

   timer.it_value.tv_sec = tv.sec;      // initial event (s)
   timer.it_value.tv_nsec = tv.nsec;    // initial event (ns)
   timer.it_interval.tv_sec = tv.sec;   // periodic event (s)
   timer.it_interval.tv_nsec = tv.nsec; // periodic event (ns)

   // start the timer
   timer_settime( timerid, 0, &timer, NULL );
   
#else // default unix
    Data->DueTime.tv_sec = 0;
    Data->DueTime.tv_usec = 0;
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

#elif (CISST_OS == CISST_LINUX_XENOMAI)

    // DO NOT DELETE THE TASK WHEN DELETING THE BUDDY                                                                                         
    // DESTROY THE TASK IN THE THREAD DESCRUCTOR     
                                                                                         
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
#elif (CISST_OS == CISST_LINUX_XENOMAI)

    unsigned long overruns=0;
    int retval = 0;
    retval = rt_task_wait_period( &overruns );
    if( retval != 0 ){
        std::string errstr;
        switch( -retval ){
        case EWOULDBLOCK:
            errstr = "EWOULDBLOCK";
            break;
        case EINTR:
            errstr = "EINTR";
            break;
        case ETIMEDOUT:
            errstr = "ETIMEDOUT";
        case EPERM:
            errstr = "EPERM";
            break;
        }
        CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                          << "Failed to wait for next period: " << errstr
                          << " Overruns: " << overruns
                          << std::endl;
    }

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

   // wait to receive the next pulse
   struct _pulse pulse;
   MsgReceivePulse( Data->chid, &pulse, sizeof(pulse), NULL );

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
#elif (CISST_OS == CISST_LINUX_XENOMAI)
    if( IsPeriodic() ){
        unsigned long overruns=0;
        int retval = 0;
        retval = rt_task_wait_period( &overruns );

        if( retval != 0 ){            
            std::string errstr;
            switch( -retval ){ 
            case EWOULDBLOCK:
                errstr = "EWOULDBLOCK";
                break; 
            case EINTR:
                errstr = "EINTR";  
                break;
            case ETIMEDOUT:
                errstr = "ETIMEDOUT";
            case EPERM:
                errstr = "EPERM"; 
                break;
            }
            CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                              << "Failed to wait for next period: " << errstr
                              << " Overruns: " << overruns
                              << std::endl;
        }
        
    }

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

    if (!IsPeriodic()) { return; }
   
    // wait to receive the next pulse
    struct _pulse pulse;
    MsgReceivePulse( Data->chid, &pulse, sizeof(pulse), NULL );

#else // default unix
    if (!IsPeriodic()) {
        return;
    }
    double elapsedTimeMicroSec, timeRemainingNanoSec;
    struct timeval timeNow, timeLater;
    struct timespec timeSleep;
    do {
        if (Data->DueTime.tv_usec == 0 && Data->DueTime.tv_sec == 0) {
            // this is the first time this is being called;
            gettimeofday(&Data->DueTime, NULL);
        }
        gettimeofday(&timeNow, NULL);
        elapsedTimeMicroSec = static_cast<double>(1000 * 1000 * (timeNow.tv_sec - Data->DueTime.tv_sec)
                                                  + (timeNow.tv_usec - Data->DueTime.tv_usec)); // in usec
        timeRemainingNanoSec = Period - elapsedTimeMicroSec * 1000.0; // floating point
        timeSleep.tv_sec = 0;
        timeSleep.tv_nsec = static_cast<long>(timeRemainingNanoSec);
        // this is required, at least for Mac OS X
        while (timeSleep.tv_nsec > 1000000000) {
            timeSleep.tv_nsec -= 1000000000;
            timeSleep.tv_sec++;
        }
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
    if (Data->IsSuspended)
        rt_task_resume(Data->RTTask);
#elif (CISST_OS == CISST_LINUX_XENOMAI)

    if( rt_task_self() != NULL ){
        int retval = 0;
        retval = rt_task_resume( rt_task_self() );
        if( retval != 0 ){
            CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                              << "Failed to resume task. "
                              << strerror(retval) << ": " << retval << ". "
                              << std::endl;
        }
    }
    else{
        CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
                            << "Calling thread is not a Xenomai task."
                            << std::endl;
    }

#endif
    Data->IsSuspended = false;
}

void osaThreadBuddy::Suspend(void) 
{
    Data->IsSuspended = true;
#if (CISST_OS == CISST_LINUX_RTAI)
    rt_task_suspend(Data->RTTask);

#elif (CISST_OS == CISST_LINUX_XENOMAI)

    if( rt_task_self() != NULL ){
        int retval = 0;
        retval = rt_task_suspend( rt_task_self() );
        if( retval != 0 ){
            CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                              << "Failed to suspend task. "
                              << strerror(retval) << ": " << retval << ". "
                              << std::endl;
        }
    }
    else{
        CMN_LOG_RUN_WARNING << CMN_LOG_DETAILS
                            << "Calling thread is not a Xenomai task."
                            << std::endl;
    }

#endif
}

// Lock stack growth
void osaThreadBuddy::LockStack() 
{
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX_XENOMAI)
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

