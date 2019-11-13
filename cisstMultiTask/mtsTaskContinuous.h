/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Peter Kazanzides
  Created on: 2008-09-23

  (C) Copyright 2008-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines a continuously executing task.
*/

#ifndef _mtsTaskContinuous_h
#define _mtsTaskContinuous_h

#include <cisstMultiTask/mtsTask.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  This class inherits from mtsTask and provides the base for implementing
  continuous loops, where the user-supplied Run method is repeatedly
  invoked.  It also introduces the possibility to create a new thread or
  to use an existing thread for the task.
*/
class CISST_EXPORT mtsTaskContinuous: public mtsTask
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    friend class mtsComponentManager;

public:
    typedef mtsTask BaseType;

protected:
    /************************ Protected task data  *********************/

    /*! True if a new thread should be created for this task. */
    bool NewThread;

    /*! If a new thread is not created, we need to capture the current thread
        on the first call to Start. */
    bool CaptureThread;

    /********************* Methods that call user methods *****************/

	/*! The member function that is passed as 'start routine' argument for
	  thread creation. */
	virtual void *RunInternal(void* argument);

    /*! Called from Start */
    virtual void StartInternal(void);

public:
    /********************* Task constructor and destructor *****************/

	/*! Create a task with name 'name' that has a state table of the specified size.

        \param name The name of the task
        \param sizeStateTable The history size of the state table
        \param newThread True if a new thread should be created for this task

        \note See note in mtsTask regarding length of string name.

        If newThread is false, the task will not create a new thread, but will rather
        "capture" an existing thread.  This is done as follows:

        1) Call mtsTaskContinuous::Create from the thread to be captured. The class
           will get the thread id for the current thread and use it to initialize all
           interfaces in mtsTask::StartupInternal.  Once this is done, Create
           returns to the calling thread.

        2) Call mtsTaskContinuous::Start from the thread to be captured. This function
           captures the thread, and uses it to call the mtsTask::Run method.  It does not
           return until the task is terminated.

        Other tasks can use mtsTask::Suspend and mtsTask::Start to suspend and resume
        this task, or mtsTask::Kill to terminate it.  In other words, once started the
        task should behave the same regardless of whether or not a new thread was created.

        Note that mtsTaskFromCallback provides another mechanism for using an existing
        thread.  In that case, the task does not capture the thread, but just registers
        itself as a callback.

        \sa mtsTask, mtsTaskPeriodic, mtsTaskFromSignal, mtsTaskFromCallback

	 */
    mtsTaskContinuous(const std::string & name,
                      unsigned int sizeStateTable = 256,
                      bool newThread = true);

    mtsTaskContinuous(const mtsTaskContinuousConstructorArg &arg);

    /*! Default Destructor. */
    virtual ~mtsTaskContinuous();

    /********************* Methods to change task status *****************/

    /* Create a new thread (if needed). */
    void Create(void *data = 0);

    /*! Start/resume execution of the task */
    void Start(void);

    /*! Suspend the execution of the task */
    void Suspend(void);

    /*! End the task */
    void Kill(void);
};


CMN_DECLARE_SERVICES_INSTANTIATION(mtsTaskContinuous)

// Continuous task that doesn't create a thread (uses main thread).

class CISST_EXPORT mtsTaskMain : public mtsTaskContinuous
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
public:
    mtsTaskMain(const std::string &name) : mtsTaskContinuous(name, 256, false) {}
    ~mtsTaskMain() {}
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsTaskMain)

#endif // _mtsTaskContinuous_h
