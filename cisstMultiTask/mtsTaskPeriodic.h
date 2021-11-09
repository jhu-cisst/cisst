/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
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
  \brief Defines a periodic task.
*/

#ifndef _mtsTaskPeriodic_h
#define _mtsTaskPeriodic_h

#include <cisstMultiTask/mtsTaskContinuous.h>
#include <cisstOSAbstraction/osaThreadBuddy.h>
#include <cisstOSAbstraction/osaTimeServer.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  This class inherits from mtsTaskContinuous and specializes it to support
  periodic loops, where the user-supplied Run method is called at a defined
  period. It also has a mechanism to make the task hard real time,
  assuming that the underlying operating system provides that capability.
*/
class CISST_EXPORT mtsTaskPeriodic : public mtsTaskContinuous
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    friend class mtsComponentManager;

 public:
    typedef mtsTaskContinuous BaseType;

 protected:
    /************************ Protected task data  *********************/

	/*! The OS independent thread buddy, that makes the thread
	  real-time on __ALL__ platforms. */
	osaThreadBuddy ThreadBuddy;

    /*! The period of the task, in seconds. */
    double Period;
    osaAbsoluteTime AbsoluteTimePeriod;

	/*! True if the task is hard real time. It is always false for non-real
	  time systems. */
	bool IsHardRealTime;

    /********************* Methods that call user methods *****************/

	/*! The member function that is passed as 'start routine' argument for
	  thread creation. */
	virtual void *RunInternal(void* argument);

	/*! The member funtion that is executed as soon as thread gets created.
	  It does some housekeeping before the user code can be executed as real
	  time. */
	void StartupInternal(void);

	/*! The member function that is executed once the task terminates. This
	  does some cleanup work */
	void CleanupInternal(void);

    /*! Called from Start */
    void StartInternal(void);

 public:
    /********************* Task constructor and destructor *****************/

	/*! Create a task with name 'name', periodicity, and a flag that
      sets if the task needs to be hard real time.

      \param name The name of the task
      \param periodicityInSeconds The task period, in seconds
      \param isHardRealTime True if task should run in hard real time
      \param sizeStateTable The history size of the state table
      \param newThread True if a new thread should be created for this task

      \note See note in mtsTask regarding length of string name.  See note
      in mtsTaskContinuous regarding newThread parameter.

    */
    mtsTaskPeriodic(const std::string & name,
                    double periodicityInSeconds,
                    bool isHardRealTime = false,
                    unsigned int sizeStateTable = 256,
                    bool newThread = true);

    mtsTaskPeriodic(const std::string & name,
                    const osaAbsoluteTime& periodicity,
                    bool isHardRealTime = false,
                    unsigned int sizeStateTable = 256,
                    bool newThread = true);

    mtsTaskPeriodic(const mtsTaskPeriodicConstructorArg &arg);

	/*! Default Destructor. */
	virtual ~mtsTaskPeriodic();

    /********************* Methods to change task status *****************/
    /* (use Start, Create and Kill methods from base classes)            */

	/*! Suspend the execution of the task */
	void Suspend(void);

    /********************* Methods for task period and overrun ************/

    /*! Return the periodicity of the task, in seconds */
    double GetPeriodicity(void) const;

    /*! Return true if thread is periodic.  Currently, returns true if
      the thread was created with a period > 0. */
    bool IsPeriodic(void) const;

};


CMN_DECLARE_SERVICES_INSTANTIATION(mtsTaskPeriodic)


#endif // _mtsTaskPeriodic_h
