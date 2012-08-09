/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsMonitorComponent.h 3034 2011-10-09 01:53:36Z adeguet1 $

  Author(s):  Min Yang Jung
  Created on: 2011-12-29

  (C) Copyright 2004-2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

/*!
  \file
  \brief Defines fault monitor
*/

#ifndef _mtsMonitorComponent_h
#define _mtsMonitorComponent_h

#include <cisstCommon/cmnNamedMap.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaThreadSignal.h>
#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsSubscriberCallback.h>

#include "cisstMonitor.h"
#include "publisher.h"
#include "subscriber.h"

#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  This class implements an internal component that monitors other component(s) 
  in the same process.

    Requirements:
        - Can be deployed at run-time using dynamic component composition services
        - Can monitor more than one components simultaneously
        - Can spwan internal processing thread(s) if monitoring overhead increases (self-monitoring)
        - Can support flexible deployment of different kinds of "filters"
          1) Basic filters
             . Bypass
             . Sampling
             . Average
             . Median
             . Std
             . Min, Max
             . Trend velocity (dx/dt)
             . Trend acceleration (d/dt)(dx/dt)
             . Thresholding
          2) Advanced filters
             . LPF, HPF
             . FFT
             . Wavelet
             . Statistical distribution
          3) User-supplied filters
        - Can support cascaded filters, i.e., filters can be combined

    Inputs:
        - Raw data collected from target component(s)

    Outputs:
        - Feature vectors
*/

class CISST_EXPORT mtsMonitorComponent : public mtsTaskPeriodic
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    typedef struct {
        osaThread       Thread;
        osaThreadSignal ThreadEventBegin;
        osaThreadSignal ThreadEventEnd;
        bool            Running;
    } InternalThreadType;

protected:
    /*! Named map of components being monitored
    typedef cmnNamedMap<mtsComponent> MonitoringTargetsType;
    MonitoringTargetsType * MonitoringTargets; */

    class TargetComponentAccessor {
    public:
        /*! Copy of monitoring target (SF::cisstMonitor instance) */
        SF::cisstMonitor MonitorTarget;
        /*! Name of this process */
        std::string ProcessName;
        /*! Name of the component being monitored */
        std::string ComponentName;
        /*! Required interface to connect to the target component */
        mtsInterfaceRequired * InterfaceRequired;
        /*! Function to read state variables from target component */
        mtsFunctionRead GetPeriod;
        /*! Placeholder (i.e., copy of samplings of the monitoring targets) */
        // [SFUPDATE]: TODO: add more placeholders depending on monitoring target types
        SF::SamplingPeriodType Period;
        /*! Minimum period that determines when to fetch data from the target
            component's state table.  This is set as the minimum value of 
            the "sampling frequency" field in JSON of all monitoring target 
            elements. */
        SF::SamplingPeriodType MinimumPeriod;
        /*! Timestamp of last sampled data */
        double LastSampledTime;

        TargetComponentAccessor(): 
            InterfaceRequired(0), Period(0.0), MinimumPeriod(1.0), LastSampledTime(0.0) {}
        ~TargetComponentAccessor() {
            if (InterfaceRequired) delete InterfaceRequired;
        }

        bool UpdateMinimumPeriod(SF::SamplingPeriodType newPeriod) {
            if (MinimumPeriod > newPeriod) {
                MinimumPeriod = newPeriod;
                return true; // minimum period value was updated
            }
            return false; // minimum period value remains the same
        }
    };

    /*! List of TargetComponentAccessor structure */
    typedef cmnNamedMap<TargetComponentAccessor> TargetComponentAccessorType;
    TargetComponentAccessorType * TargetComponentAccessors;

    /*! Initialize monitor instance */
    void Init(void);

    /*! Fetch new values from each target component */
    void UpdateFilters(void);

    /*! Print list of target components */
    void PrintTargetComponents(void);

    /*! Install filter to component */
    // [SFUPDATE]
    //bool InstallFilters(mtsComponent * component);
    //bool InstallFilters(mtsTask * task);
    //bool InstallFilters(mtsTaskContinuous * taskContinuous);
    //bool InstallFilters(mtsTaskFromCallback * taskFromCallback);
    //bool InstallFilters(mtsTaskFromSignal * taskFromSignal);
    bool InstallFilters(TargetComponentAccessor * entry, mtsTaskPeriodic * taskPeriodic);

    //
    // Message exchange with Safety Framework
    //
    /*! Ice publisher and subscriber */
    SF::Publisher *  Publisher;
    SF::Subscriber * Subscriber;

    /*! Callback for subscriber */
    mtsSubscriberCallback * SubscriberCallback;

    /*! Container for messages delivered by subscriber */
    mtsSubscriberCallback::MessagesType Messages;

    InternalThreadType ThreadPublisher;
    InternalThreadType ThreadSubscriber;

    void * RunPublisher(unsigned int arg);
    void * RunSubscriber(unsigned int arg);

public:
    mtsMonitorComponent();
    mtsMonitorComponent(double period);
    ~mtsMonitorComponent();

    /*! Pass monitoring target information (i.e., cisstMonitor instance) to the 
        component to be monitored.  The component uses this information to install
        monitoring and FDD pipelines. */
    bool AddMonitorTargetToComponent(SF::cisstMonitor & newMonitorTarget);

    // TODO: replace this with RemoveMonitorTargetFromComponent()
    /*! Unregister component from the registry */
    bool UnregisterComponent(const std::string & componentName);

    /*! Initialize all accessors (make connections between monitor and target components) */
    bool InitializeAccessors(void);

    void Configure(const std::string & CMN_UNUSED(filename) = ""){}
    void Startup(void) {};
    void Run(void);
    void Cleanup(void);

    //  Getters
    /*! Return the name of required interface to access a given task's state table */
    const std::string GetNameOfStateTableAccessInterface(const std::string & taskName) const;

    /*! Return the name of this component */
    static const std::string & GetNameOfMonitorComponent(void);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsMonitorComponent)


#endif // _mtsMonitorComponent_h

