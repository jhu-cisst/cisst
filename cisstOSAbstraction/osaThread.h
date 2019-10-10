/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Ankur Kapoor
  Created on: 2004-04-30

  (C) Copyright 2004-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Declaration of osaThread
  \ingroup cisstOSAbstraction
 */

#ifndef _osaThread_h
#define _osaThread_h

#include <cisstCommon/cmnPortability.h>
#include <cisstOSAbstraction/osaThreadAdapter.h>
#include <cisstOSAbstraction/osaThreadSignal.h>

// Always include last
#include <cisstOSAbstraction/osaExport.h>


#if (CISST_OS == CISST_WINDOWS)
#define SCHED_FIFO 0   /*! No Scheduling Policy available in Windows */
#endif

#if (CISST_OS == CISST_DARWIN) // SCHED_FIFO is not defined otherwise
#include <pthread.h>
#endif

/*!
  \brief PriorityType and SchedulingPolicyType.

  For now these are just typedefs to integers. I would like to have a
  common return value for all OS. Maybe an enum, but the problem is
  that the granularity in any linux flavor is much much larger than
  windows.
*/
typedef int PriorityType;


/*!
  \brief SchedulingPolicyType.
  For now SchedulingPolicy is typedef'ed to int
  */
typedef int SchedulingPolicyType;



/*!
  \brief ThreadId type

  Abstract the thread Id based on the OS.  Some OSs use a native C type
  (e.g., unsigned long) to identify threads but others use more complex

  structs hence the need for a class to define the == and <<
  operators.

  \sa osaGetCurrentThreadId
*/
class CISST_EXPORT osaThreadId {

    /*! Internals that are OS-dependent in some way */
    enum {INTERNALS_SIZE = 8};
    char Internals[INTERNALS_SIZE];
    bool Valid;

    friend CISST_EXPORT osaThreadId osaGetCurrentThreadId(void);
    friend class osaThread;

    /*! Return the size of the actual object used by the OS.  This is
        used for testing only. */
    static unsigned int SizeOfInternals(void);
    friend class osaThreadTest;

public:

    /*! Default constructor.  Does nothing but checking that the
      internal structure use to store the OS specific information for
      osaThreadId is large enough using CMN_ASSERT. Also sets Valid to false. */
    osaThreadId(void);

    /*! Default destructor.  Does nothing. */
    ~osaThreadId();

    /*! Method to return whether a valid thread id has been set */
    bool IsValid(void) const { return Valid; }

    /*! Method to compare two thread Ids */
    bool Equal(const osaThreadId & other) const;

    /*! Operator to compare two thread Ids */
    inline bool operator == (const osaThreadId & other) const {
        return this->Equal(other);
    }

    /*! Operator to compare two thread Ids */
    inline bool operator != (const osaThreadId & other) const {
        return !this->Equal(other);
    }

    /*! Method to compare two thread Ids, used to sort in std::map. */
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_WINDOWS)
    bool operator () (const osaThreadId & lhs, const osaThreadId & rhs) const;
#endif

    /*! Method to compare two thread Ids, used to sort in std::map. */
    // bool Lesser(const osaThreadId & other) const;

    /*! Operator to compare two thread Ids, used to sort in std::map. */
    // inline bool operator < (const osaThreadId & other) const {
    //    return this->Lesser(other);
    // }

    /*! Print to stream */
    void ToStream(std::ostream & outputStream) const;
};


/*! Stream operator for a thread Id, see osaThreadId. */
inline
std::ostream & operator << (std::ostream & output,
                            const osaThreadId & threadId) {
    threadId.ToStream(output);
    return output;
}


/*!
  Get the ID of the currently executing thread.

 \sa osaThreadId
*/
CISST_EXPORT osaThreadId osaGetCurrentThreadId(void);




/*!
  \brief Define a thread object

  \ingroup cisstOSAbstraction

  The thread class is just a wrapper on the OS-specific thread management functions.
  For now, wraps functions such as CreateThread, DeleteThread, GetThreadPriority,
  SetThreadPriority and so on.   Additional functionality, such as inter-thread
  communication, maintenance of state information, etc., are provided by the mtsTask
  class.

  \sa mtsTask
 */
class CISST_EXPORT osaThread {

    /*! For synchronization */
    osaThreadSignal Signal;

    /*! Internals that are OS-dependent in some way */
    enum { INTERNALS_SIZE = 96 };   // PKAZ: this can be reduced
    char Internals[INTERNALS_SIZE];

    /*! Return the size of the actual object used by the OS.  This is
        used for testing only. */
    static unsigned int SizeOfInternals(void);
    friend class osaThreadTest;

    /*! The name associated with the thread. */
    char Name[6];

    /*! The OS *independent* Scheduling Policy for the thread. For now
       it is typedef'ed to int */
    SchedulingPolicyType Policy;

    /*! The OS *independent* Priority for the thread. For now it is
       typedef'ed to int */
    PriorityType Priority;

    /*! The thread id */
    osaThreadId ThreadId;

    /*! Whether the thread exists. */
    bool Valid;

    /*! Whether the thread is running */
    bool Running;

protected:

    /*! Creates a new thread. */
    void CreateInternal(const char* name, void* func, void* userdata);

    void SetThreadName(const char* name);

public:

    /*! Default constructor.  Does nothing. */
    osaThread();

    /*! Default destructor.  Does nothing. */
    ~osaThread();

    /*! Creates a new thread that will execute the specified function, with no parameters. */
    void Create(void * (*threadStart)(void),
                const char * name = 0, int priority = 0, int policy = SCHED_FIFO)
    {
        Priority = priority;
        Policy = policy;
        CreateInternal(name, (void *)threadStart, 0);
    }

    /*! Creates a new thread that will execute the specified function, taking a single parameter. */
    template <class _userDataType>
    void Create(void * (*threadStart)(_userDataType),
                _userDataType userData = _userDataType(),
                const char * name = 0, int priority = 0, int policy = SCHED_FIFO)
    {
        Priority = priority;
        Policy = policy;
        CreateInternal(name, (void *)threadStart, (void *)userData);
    }

    /*! Creates a new thread that will execute the specified class member function.
        Note that the object is copied to the heap so it remains valid, even if the
        original object is destroyed. */
    template <class _entryType, class _userDataType>
    void Create(_entryType * obj, void * (_entryType::*threadStart)(_userDataType),
                _userDataType userData = _userDataType(),
                const char * name = 0, int priority = 0, int policy = SCHED_FIFO)
    {
        Priority = priority;
        Policy = policy;
        typedef osaHeapCallBack<_entryType, _userDataType, void *, void *> adapter_t;
        // In the following, the third parameter is not the UserData, but rather an osaHeapCallBack
        // object that includes the UserData.  When the CallbackAndDestroy method is called, it takes
        // its void* parameter, casts it to osaHeapCallBack*, and then finds the actual callback method
        // and UserData within the osaHeapCallBack object.
        CreateInternal(name, (void *)adapter_t::CallbackAndDestroy, (void *)adapter_t::Create(obj, threadStart, userData));
    }

    /*! Initialize the thread object so that it refers to the calling thread, rather than
        creating a new thread. This allows existing threads to intermix with newly created threads. */
    void CreateFromCurrentThread(const char * name = 0, int priority = 0, int policy = SCHED_FIFO)
    {
        Priority = priority;
        Policy = policy;
        ThreadId = osaGetCurrentThreadId();
        SetThreadName(name);
        Valid = true;
    }

    void CreateFromThreadId(const osaThreadId &threadId,
                            const char * name = 0, int priority = 0, int policy = SCHED_FIFO)
    {
        Priority = priority;
        Policy = policy;
        ThreadId = threadId;
        SetThreadName(name);
        Valid = true;
    }

    /*! Deletes a thread.  Deletes (removes from memory) everything
      associated with a thread, e.g. stack, local data, static
      variables.
    */
    void Delete(void);

    /*! Wait for the thread to exit. */
    void Wait(void);

    /*! Wait for thread to receive a "wakeup" signal/event. */
    inline void WaitForWakeup(void) { Signal.Wait(); }

    /*! Wait for thread to receive a "wakeup" signal/event. */
    inline void WaitForWakeup(double timeoutInSec) { Signal.Wait(timeoutInSec); }

    /*! Signal the thread to wake up. */
    inline void Wakeup(void) { Signal.Raise(); }

    /*! Return the thread name. */
    inline const char * GetName(void) const { return Name; }

    /*! Return the thread id. */
    inline osaThreadId GetId(void) const { return ThreadId; }

    /*! Get the OS *independent* priority of the thread. */
    PriorityType GetPriority(void) const;

    /*! Set the OS *independent* priority of the thread. */
    void SetPriority(PriorityType priority);

    /*! Get the OS *independent* scheduling policy of the thread. Will not
     be available on all platforms or will return a constant. */
    SchedulingPolicyType GetSchedulingPolicy(void);

    /*! Set the OS *independent* scheduling policy of the thread. Will not
     be available on all platforms. */
    void SetSchedulingPolicy(SchedulingPolicyType policy);

    /*! Delay/suspend execution of the calling thread for the specified time.
        On some platforms, this method provides a better implementation of
        osaSleep, provided that it is called from the thread associated with
        this instance of the class.  In this case, the Sleep will also be
        terminated by any call to Wakeup.  If called from a different thread,
        it just calls osaSleep and is therefore not affected by any calls
        to Wakeup.

        \param timeInSeconds The amount of time the task is suspended (in seconds)
     */
    void Sleep(double timeInSeconds);

    /*! Returns if this thread is running. */
    inline bool IsRunning(void) const { return Running; }

    /*! Returns whether the thread exists (is valid). */
    inline bool IsValid(void) const { return Valid; }

};

/*! Yield the current thread.  Stops the current thread and takes it
    at the end of the list of ready threads having its same
    priority. The scheduler makes the next ready thread of the same
    priority active.
 */
CISST_EXPORT void osaCurrentThreadYield(void);



#endif // _osaThread_h

