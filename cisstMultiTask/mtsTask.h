/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2013 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines a periodic task.
*/

#ifndef _mtsTask_h
#define _mtsTask_h

#include <stdexcept>

#include <cisstCommon/cmnPortability.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaMutex.h>

#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsStateTable.h>
#include <cisstMultiTask/mtsMailBox.h>
#include <cisstMultiTask/mtsCommandVoid.h>
#include <cisstMultiTask/mtsCommandRead.h>
#include <cisstMultiTask/mtsCommandWrite.h>
#include <cisstMultiTask/mtsCommandQueuedVoid.h>
#include <cisstMultiTask/mtsCommandQueuedWrite.h>
#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsFunctionVoid.h>
#include <cisstMultiTask/mtsFunctionWrite.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  This class provides the base for implementing tasks that have
  a thread, a state table to store the state at each 'tick' (increment)
  of the task, and queues to receive messages (commands) from other tasks.
  It is derived from mtsComponent, so it also contains the provided and required
  interfaces, with their lists of commands.
*/

class CISST_EXPORT mtsTask: public mtsComponent
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    friend class mtsManagerLocal;

public:
    typedef mtsComponent BaseType;

    static std::runtime_error UnknownException;

protected:
    /************************ Protected task data  *********************/

    /*! The OS independent thread object. */
    osaThread Thread;

    /*! Delay given for the task to start.  During initialization the
      task thread will wait for the specified delay (set by default to
      3 seconds in the constructor).  Increasing this delay can be
      useful when using mtsTaskFromCallback if the callback might take
      more than 3 seconds to trigger the task's Run method. */
    double InitializationDelay;

    /*! Mutex used when changing task states. Do not change this directly, use the
        ChangeState method instead. */
    osaMutex StateChange;

    /*! Signal for caller to wait on task state changes. */
    osaThreadSignal StateChangeSignal;

    /*! The state data table object to store the states of the task. */
    mtsStateTable StateTable;

    /*! True if the task took more time to do computation than allocated time.
      */
    bool OverranPeriod;

    /*! The data passed to the thread. */
    void * ThreadStartData;

    /*! The return value for RunInternal. */
    void * ReturnValue;

    /*! Callable object used when queueing. */
    mtsCallableVoidBase * InterfaceProvidedToManagerCallable;

    /******************** ExecIn interface *************************/

    /*! ExecIn required interface. */
    mtsInterfaceRequired * ExecIn;

    /*! Run event handler. */
    void RunEventHandler(void);

    /*! Change state event handler. */
    void ChangeStateEventHandler(const mtsComponentState &newState);

    /******************** ExecOut interface ************************/

    /*! ExecOut provided interface. */
    mtsInterfaceProvided * ExecOut;

    /*! Event generator used to pass thread of execution to another task.
      User code should not call this directly (call RunEvent instead). */
    mtsFunctionVoid RunEventInternal;
    bool RunEventCalled;
    mtsExecutionResult RunEvent(bool check = true);

    /*! Event generator used to change state of dependent tasks. */
    mtsFunctionWrite ChangeStateEvent;

    /********************* Methods that call user methods *****************/

    /*! The member function that is passed as 'start routine' argument for
      thread creation. */
    virtual void * RunInternal(void * argument) = 0;

    /*! The main part of the Run loop that is the same for all derived classes.
        This should not be overloaded. */
    void DoRunInternal(void);

    /*! The member funtion that is executed as soon as the thread gets created.
      It does some housekeeping before the user code can be executed. */
    virtual void StartupInternal(void);

    /*! The member function that is executed once the task terminates. This
      does some cleanup work. */
    virtual void CleanupInternal(void);

    /**************** Methods for managing task timing ********************/

    /*! Delay the task by the specified amount. This is a protected member
        function because it should only be called from within the task.
        Otherwise, use osaSleep. */
    void Sleep(double timeInSeconds);

    /*! Return the current tick count. */
    mtsStateIndex::TimeTicksType GetTick(void) const;

    /*********** Methods for thread start data and return values **********/

    /*! Save any 'user data' that was passed to the thread start routine. */
    virtual void SaveThreadStartData(void * data);

    virtual void SetThreadReturnValue(void * returnValue);

    /*********** Methods for changing task state **************************/

    /* documented in base class */
    void ChangeState(mtsComponentState::Enum newState);

    /* documented in base class */
    bool WaitForState(mtsComponentState desiredState, double timeout);

public:
    /********************* Task constructor and destructor *****************/

    /*! Create a task with name 'name' and set the state table size (see mtsStateTable).
        This is the task base class. Tasks should be derived from one of the
        existing derived classes:  mtsTaskContinuous, mtsTaskPeriodic,
        mtsTaskFromCallback, and mtsTaskFromSignal.

        \param name  The name for the task
        \param sizeStateTable The history size of the state table

        \note The full string name is maintained in the class member data
              (in mtsComponent base class).  But, be aware that when a thread and/or
              thread buddy is created, only the first 6 characters of this name
              are used with the thread or thread buddy.  This is an artifact
              of the 6 character limit imposed by RTAI/Linux.

        \sa mtsComponent, mtsTaskContinuous, mtsTaskPeriodic, mtsTaskFromCallback, mtsTaskFromSignal
     */
    mtsTask(const std::string & name,
            unsigned int sizeStateTable = 256);

    /*! Default Destructor. */
    virtual ~mtsTask();

    /********************* Methods to be defined by user *****************/
    /* The Run, Startup, and Cleanup methods could be made protected.    */
    /* Note that Startup and Cleanup are now defind in mtsComponent.     */

    /*! Virtual method that gets called if an exception is thrown in the
        Startup method. This could be moved to mtsComponent. */
    virtual void OnStartupException(const std::exception &excp);

    /*! Pure virtual method that gets overloaded to run the actual task.
      */
    virtual void Run(void) = 0;

    /*! Virtual method that gets called if an exception is thrown in the
        Run method. */
    virtual void OnRunException(const std::exception &excp);

    /*! Virtual method that gets called when the task/interface needs
      to be configured. Should it take XML info?? */
    virtual void Configure(const std::string & CMN_UNUSED(filename) = "") {}

    /*! Set the initialization delay.  See InitializationDelay. */
    void SetInitializationDelay(double delay);

    /********************* Methods to change task state ******************/
    /* Maybe some or all of these should be pure virtual functions.      */

    /* Create a new thread (if needed). */
    virtual void Create(void * data) = 0;
    inline void Create(void) { Create(0); }

    /*! End the task */
    void Kill(void);

    /*! Return the average period. */
    double GetAveragePeriod(void) const { return StateTable.GetAveragePeriod(); }

    /*! Return the name of this state table. */
    inline const std::string GetDefaultStateTableName(void) const {
        return StateTable.GetName();
    }

    /*! Return a pointer to the default state table.  See
      GetStateTable and GetDefaultStateTableName. */
    inline mtsStateTable * GetDefaultStateTable(void) {
        return this->StateTables.GetItem(this->GetDefaultStateTableName(), CMN_LOG_LEVEL_INIT_ERROR);
    }

    /********************* Methods to manage interfaces *******************/

    /* documented in base class */
    mtsInterfaceRequired * AddInterfaceRequiredWithoutSystemEventHandlers(const std::string & interfaceRequiredName,
                                                                          mtsRequiredType required = MTS_REQUIRED);

    /* documented in base class */
    mtsInterfaceProvided * AddInterfaceProvidedWithoutSystemEvents(const std::string & newInterfaceName,
                                                                   mtsInterfaceQueueingPolicy queueingPolicy = MTS_COMPONENT_POLICY,
                                                                   bool isProxy = false);

    /********************* Methods for task synchronization ***************/

    /*! Wait for task to start.
        \param timeout The timeout in seconds
        \returns true if task has started; false if timeout occurred before task started.
     */
    virtual bool WaitToStart(double timeout);

    /*! Wait for task to finish (after issuing a task Kill).
      \param timeout  The timeout in seconds
      \returns  true if the task terminated without timeout happening;
                false if timeout occured and task did not finish
      */
    virtual bool WaitToTerminate(double timeout);

    /*! Suspend this task until the Wakeup method is called. */
    inline virtual void WaitForWakeup(void) {
        Thread.WaitForWakeup();
    }

    /*! Wakeup the task. */
    inline virtual void Wakeup(void) {
        Thread.Wakeup();
    }

    /*! Conditionally process internal mailbox */
    void ProcessManagerCommandsIfNotActive();

    /*! Returns true if currently executing in thread-space of component. */
    bool CheckForOwnThread(void) const;

    /********************* Methods for task period and overrun ************/

    /*! Return true if thread is periodic. */
    inline virtual bool IsPeriodic(void) const {
        return false;
    }

    /*! Return true if task overran allocated period. Note that this is not
        restricted to mtsTaskPeriodic.  For example, an mtsTaskFromCallback
        can overrun if a second callback occurs before the first is finished. */
    inline virtual bool IsOverranPeriod(void) const {
        return OverranPeriod;
    }

    /*! Reset overran period flag. */
    inline virtual void ResetOverranPeriod(void) {
        OverranPeriod = false;
    }
};


CMN_DECLARE_SERVICES_INSTANTIATION(mtsTask)


#endif // _mtsTask_h

