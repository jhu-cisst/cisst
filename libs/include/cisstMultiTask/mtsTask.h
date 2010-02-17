/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2009 Johns Hopkins University (JHU), All Rights Reserved.

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
#include <cisstMultiTask/mtsDevice.h>
#include <cisstMultiTask/mtsHistory.h>
#include <cisstMultiTask/mtsFunctionVoid.h>
#include <cisstMultiTask/mtsTaskInterface.h>

#include <set>
#include <map>

// Always include last
#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  This class provides the base for implementing tasks that have
  a thread, a state table to store the state at each 'tick' (increment)
  of the task, and queues to receive messages (commands) from other tasks.
  It is derived from mtsDevice, so it also contains the provided and required
  interfaces, with their lists of commands.
*/

class CISST_EXPORT mtsTask: public mtsDevice
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

    friend class mtsManagerLocal;
    friend class mtsManagerLocalTest;

public:
    typedef mtsDevice BaseType;

    /*! The task states:

        CONSTRUCTED  -- Initial state set by mtsTask constructor.
        INITIALIZING -- Set by mtsTask::Create.  The task stays in this state until the
                        thread calls mtsTask::RunInternal, which calls mtsTask::StartupInternal
                        and the user-supplied mtsTask::Startup. If a new thread is created,
                        the call to mtsTask::RunInternal happens some time after the call
                        to mtsTask::Create.
        READY        -- Set by mtsTask::StartupInternal. This means that the task is ready
                        to be used (i.e., all interfaces have been initialized). Also,
                        a task can return to the READY state (from the ACTIVE state) in
                        response to a call to mtsTask::Suspend.
        ACTIVE       -- Set by mtsTask::Start.  This is the normal running state, where
                        the task is calling the user-supplied mtsTask::Run method.
        FINISHING    -- Set by mtsTask::Kill. If the mtsTask::Run method is currently
                        executing, it will finish, but no further calls will be made.
                        The task will then call mtsTask::CleanupInternal, which calls
                        the user-supplied mtsTask::Cleanup. The state will then be set
                        to FINISHED.
        FINISHED     -- The task has finished.
    */

    enum TaskStateType { CONSTRUCTED, INITIALIZING, READY, ACTIVE, FINISHING, FINISHED };

protected:
    /************************ Protected task data  *********************/

	/*! The OS independent thread object. */
	osaThread Thread;

    /*! The task state. */
    TaskStateType TaskState;

    /*! Mutex used when changing task states. Do not change this directly, use the 
        ChangeState method instead. */
    osaMutex StateChange;

    /*! Signal for caller to wait on task state changes. */
    osaThreadSignal StateChangeSignal;

	/*! The state data table object to store the states of the task. */
	mtsStateTable StateTable;
    
    /*! Map of state tables, includes the default StateTable under the
      name "StateTable" */
    typedef cmnNamedMap<mtsStateTable> StateTableMapType;
    StateTableMapType StateTables;

	/*! True if the task took more time to do computation than allocated time.
	  */
    bool OverranPeriod;

    /*! The data passed to the thread. */
    void * ThreadStartData;

    /*! The return value for RunInternal. */
    void * ReturnValue;

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

    /********************* Methods to process queues  *********************/

    /*! Process all messages in mailboxes. Returns number of commands processed. */
    virtual unsigned int ProcessMailBoxes(ProvidedInterfacesMapType & interfaces);

    /*! Process all queued commands. Returns number of events processed.
        These are the commands provided by all interfaces of the task. */
    inline unsigned int ProcessQueuedCommands(void) 
        { return ProcessMailBoxes(ProvidedInterfaces); }

    
    /*! Process all queued events. Returns number of events processed.
        These are the commands queued following events currently observed
        via the required interfaces. */
    unsigned int ProcessQueuedEvents(void);

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

    /*! Helper function to change the task state. */
    void ChangeState(TaskStateType newState);

    /*! Helper function to wait on a state change, with specified timeout in seconds. */
    bool WaitForState(TaskStateType desiredState, double timeout);

public:
    /********************* Task constructor and destructor *****************/

	/*! Create a task with name 'name' and set the state table size (see mtsStateTable).
        This is the task base class. Tasks should be derived from one of the
        existing derived classes:  mtsTaskContinuous, mtsTaskPeriodic, and
        mtsTaskFromCallback.

        \param name  The name for the task
        \param sizeStateTable The history size of the state table

        \note The full string name is maintained in the class member data
              (in mtsDevice base class).  But, be aware that when a thread and/or
              thread buddy is created, only the first 6 characters of this name
              are used with the thread or thread buddy.  This is an artifact
              of the 6 character limit imposed by RTAI/Linux.

        \sa mtsDevice, mtsTaskContinuous, mtsTaskPeriodic, mtsTaskFromCallback
	 */
    mtsTask(const std::string & name, 
            unsigned int sizeStateTable = 256);

    /*! Default Destructor. */
    virtual ~mtsTask();

    /********************* Methods to be defined by user *****************/
    /* The Run, Startup, and Cleanup methods could be made protected.    */

	/*! Virtual method that gets overloaded to run the actual task.
	  */
    virtual void Run(void) = 0;

	/*! Virtual method that gets overloaded, and is run before the
	    task is started.
	  */
    virtual void Startup(void) {}

	/*! Virtual method that gets overloaded, and is run after the
	    task gets killed using Kill() method.
	  */
	virtual void Cleanup(void) {}

	/*! Virtual method that gets called when the task/interface needs
	  to be configured. Should it take XML info?? */
	virtual void Configure(const std::string & CMN_UNUSED(filename) = "") {}

    /********************* Methods to change task state ******************/
    /* Maybe some or all of these should be pure virtual functions.      */

    /* Create a new thread (if needed). */
    virtual void Create(void *data) = 0;
    void Create(void) { Create(0); }

	/*! Start or resume execution of the task. */
	virtual void Start(void) = 0;

	/*! Suspend the execution of the task. */
    virtual void Suspend(void) = 0;

	/*! End the task */
	virtual void Kill(void);

    /********************* Methods to query the task state ****************/
	
	/*! Return true if task is active. */
    inline bool Running(void) const { return (TaskState == ACTIVE); }

	/*! Return true if task was started. */
	inline bool IsStarted(void) const { return (TaskState >= READY); }

    /*! Return true if task is terminated. */
	inline bool IsTerminated(void) const { return (TaskState == FINISHED); }

	/*! Return true if task is marked for killing. */
	inline bool IsEndTask(void) const { return (TaskState >= FINISHING); }

    /*! Return task state. */
    inline TaskStateType GetTaskState(void) const { return TaskState; }

    /*! Convert tasks state to string representation. */
    const char * TaskStateName(TaskStateType state) const;

    /*! Return task state as a string. */
    inline const char * GetTaskStateName(void) const { return TaskStateName(TaskState); }

    /*! Return the average period. */
    double GetAveragePeriod(void) const { return StateTable.GetAveragePeriod(); }

    /*! Return the name of this state table. */
    inline const std::string GetDefaultStateTableName(void) const {
        return StateTable.GetName();
    }

    /*! Return the pointer to the default state table or a specific one if a name is provided. */
    mtsStateTable * GetStateTable(const std::string & stateTableName) {
        return this->StateTables.GetItem(stateTableName, CMN_LOG_LOD_INIT_ERROR);
    }

    /*! Add an existing state table to the list of known state tables
      in this task.  This method will add an interface for the state
      table using the name "StateTable" +
      existingStateTable->GetName() unless the caller specifies that
      no interface should be created. */
    bool AddStateTable(mtsStateTable * existingStateTable, bool addProvidedInterface = true);

    /********************* Methods to manage interfaces *******************/
	
    /* documented in base class */
    virtual mtsDeviceInterface * AddProvidedInterface(const std::string & newInterfaceName);

    
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

    /*! Send a human readable description of the device. */
    void ToStream(std::ostream & outputStream) const;
};


CMN_DECLARE_SERVICES_INSTANTIATION(mtsTask)


#endif // _mtsTask_h

