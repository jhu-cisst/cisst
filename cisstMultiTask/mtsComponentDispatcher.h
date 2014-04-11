/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Peter Kazanzides
  Created on: 2011-09-19

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines the component dispatchers, which are "execution engines" that
         generate events that can be processed by any number of components.
*/

#ifndef _mtsComponentDispatcher_h
#define _mtsComponentDispatcher_h

#error mtsComponentDispatcher is obsolete: use component ExecOut/ExecIn instead

#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsTaskContinuous.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>


template <class BaseClass, class ConstructorArgType>
class CISST_EXPORT mtsComponentDispatcherBase : public BaseClass
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION_ONEARG, CMN_LOG_ALLOW_DEFAULT);

    mtsFunctionVoid StartupEvent;
    mtsFunctionVoid RunPriorityEvent;
    mtsFunctionVoid RunEvent;
    mtsFunctionVoid CleanupEvent;

public:
    mtsComponentDispatcherBase(const ConstructorArgType &arg) : BaseClass(arg)
    {
        mtsInterfaceProvided *prov = this->AddInterfaceProvided("Dispatch");
        if (prov) {
            prov->AddEventVoid(StartupEvent, "StartupEvent");
            prov->AddEventVoid(RunEvent, "RunEvent");
            prov->AddEventVoid(RunPriorityEvent, "RunPriorityEvent");
            prov->AddEventVoid(CleanupEvent, "CleanupEvent");
        }

    }

    ~mtsComponentDispatcherBase() {}

    void Configure(const std::string &) {}

    void Startup(void)
    {
        StartupEvent();
    }
 
    void Run(void)
    { 
        RunPriorityEvent();
        this->ProcessQueuedCommands();
        RunEvent();
    }

    void Cleanup(void)
    {
        CleanupEvent();
    }

};

typedef mtsComponentDispatcherBase<mtsTaskPeriodic, mtsTaskPeriodicConstructorArg>
        mtsComponentDispatcherPeriodic;

CMN_DECLARE_SERVICES_INSTANTIATION(mtsComponentDispatcherPeriodic);

typedef mtsComponentDispatcherBase<mtsTaskContinuous, mtsTaskContinuousConstructorArg>
        mtsComponentDispatcherContinuous;

CMN_DECLARE_SERVICES_INSTANTIATION(mtsComponentDispatcherContinuous);

typedef mtsComponentDispatcherBase<mtsTaskMain, std::string>
        mtsComponentDispatcherMain;

CMN_DECLARE_SERVICES_INSTANTIATION(mtsComponentDispatcherMain);

#endif // _mtsComponentDispatcher_h
