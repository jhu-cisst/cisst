/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsMonitorComponent.h 3034 2011-10-09 01:53:36Z adeguet1 $

  Author(s):  Min Yang Jung
  Created on: 2011-12-29

  (C) Copyright 2012-2014 Johns Hopkins University (JHU), All Rights Reserved.

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
//#include <cisstMultiTask/mtsSubscriberCallback.h>
#include <cisstMultiTask/mtsEventReceiver.h>

#include "cisstMonitor.h"
#include "publisher.h"
//#include "subscriber.h"

#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  This class implements the monitor component that monitors other component(s) 
  in the same process.
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

    /*! A mtsMonitorComponent can monitor multiple components simultaneously.
        In this case, multiple instances of TargetComponentAccessor are created and
        managed by the mtsMonitorComponent instance.
        An instance of TargetComponentAccessor is instantiated per one target component
        and maintains all the necessary internal accessors such as mtsFunctionRead that 
        allows mtsMonitorComponent to fetch data from target components.  */
    class TargetComponentAccessor {
    public:
        /*! Typedef for cisstMonitor container 
         *
         *  key: string representation of target UID
         *  value: instance of SF::cisstMonitor 
         */
        typedef std::map<std::string, SF::cisstMonitor *> MonitorTargetSetType;

        /*! Predefined monitoring targets are accessed via this->InterfaceRequiredPredefined
         *  and custom monitoring targets are accessed via separate required interface.
         *
         *  key: provided interface name
         *  value: instance of FunctionObjects
         */
        typedef std::map<std::string, mtsInterfaceRequired *> InterfaceRequiredCustomType;

    protected:
        /*! Copy of mtsMonitorComponent::ManualAdvance */
        bool ManualAdvance;

        /*! Set of SF::cisstMonitor instances (each defines one monitoring target element)
            in the same component */
        // MJ: SF::cisstMonitor instances should be allocated and deallocated externally.
        MonitorTargetSetType MonitorTargetSet;

        /*! Functions to read state variables from target component */
        struct {
            mtsFunctionRead GetPeriod;        // for Monitor::TARGET_THREAD_PERIOD
            mtsFunctionRead GetExecTimeUser;  // for Monitor::TARGET_THREAD_DUTYCYCLE_USER
            mtsFunctionRead GetExecTimeTotal; // for Monitor::TARGET_THREAD_DUTYCYCLE_TOTAL
        } AccessFunctions;

    public:
        /*! Default constructor (ManualAdvance is set as false) */
        TargetComponentAccessor(void);
        /*! Constructor with manual advance flag */
        TargetComponentAccessor(bool manualAdvance);
        /*! Destructor */
        ~TargetComponentAccessor();

        /*! Name of process and component which this instance manages */
        std::string ProcessName;
        std::string ComponentName;

        /*! Required interface to access predefined monitoring targets */
        mtsInterfaceRequired * InterfaceRequiredPredefined;
        /*! Map of required interfaces to access custom monitoring targets */
        InterfaceRequiredCustomType InterfaceRequiredCustom;

        /*! Event receiver to receive events */
        mtsEventReceiverWrite FaultEventReceiver;

        /*! Add new cisstMonitor instance.  Returns false if duplicate. */
        bool AddMonitorTargetToAccessor(SF::cisstMonitor * monitor);
        /*! Check if given monitor target is already being monitored. */
        bool FindMonitorTargetFromAccessor(const std::string & targetUID) const;
        /*! Remove cisstMonitor instance.  Returns false if not found. */
        void RemoveMonitorTargetFromAccessor(const std::string & targetUID);

        /*! Get new samples for all monitor targets if necessary */
        bool RefreshSamples(double currentTick, SF::Publisher * publisher);

        /*! Add mts function object to this accessor
         *  \param type Monitoring target type (see SF::Monitor::TargetType)
         *  \param targetLocationID UID string of monitoring target.  Should be specified
         *         if type is SF::Monitor::CUSTOM.
         */
        bool AddMonitoringFunction(SF::Monitor::TargetType type, 
                                   const std::string & providedInterfaceName = "",
                                   const std::string & targetCommandName = "");

        /*! Generate contents of this class in human readable format */
        void ToStream(std::ostream & outputStream) const;
    };

protected:
    /*! Flag to determine state table's automatic advance */
    bool ManualAdvance;
     
    /*! List of TargetComponentAccessor structure */
    typedef cmnNamedMap<TargetComponentAccessor> TargetComponentAccessorType;
    TargetComponentAccessorType * TargetComponentAccessors;

    /*! Initialize monitor instance */
    void Initialize(void);

    /*! Fetch new values from each target component */
    void RunMonitors(void);

    /*! Print list of target components */
    void PrintTargetComponents(void);

    /*! Install filter to component */
    // [SFUPDATE]
    //bool InstallFilters(mtsComponent * component);
    //bool InstallFilters(mtsTask * task);
    //bool InstallFilters(mtsTaskContinuous * taskContinuous);
    //bool InstallFilters(mtsTaskFromCallback * taskFromCallback);
    //bool InstallFilters(mtsTaskFromSignal * taskFromSignal);
    // MJ TEMP: this is not being used (replaced by AddStateVectorForMonitoring())
    bool InstallFilters(TargetComponentAccessor * entry, mtsTaskPeriodic * taskPeriodic);

    /*! Add new column vector to the monitoring state table */
    void AddStateVectorForMonitoring(mtsTaskPeriodic * targetTaskPeriodic,
                                     SF::cisstMonitor * monitor);

    /*! Receive event notifications from target components and publishes them to
        the Safety Supervisor of Safety Framework. */
    // MJ TODO: Is there a better way to allow mtsEventPublisher to be able to access
    // the following methods, rather than making them as public methods?
public: // MJ TEMP
    void HandleMonitorEvent(const std::string & json);
    void HandleFaultEvent(const std::string & json);

protected:
    //
    // Message exchange with Safety Framework
    //
    /*! Ice publisher and subscriber */
    SF::Publisher *  Publisher;
    //SF::Subscriber * Subscriber;

    /*! Callback for subscriber */
    //mtsSubscriberCallback * SubscriberCallback;

    InternalThreadType ThreadPublisher;
    //InternalThreadType ThreadSubscriber;

    void * RunPublisher(unsigned int arg);
    //void * RunSubscriber(unsigned int arg);

public:
    /*! Default constructor (default: 5 msec period with automatic state table advance) */
    mtsMonitorComponent(void);
    /*! Constructor with custom period and automatic state table advance */
    mtsMonitorComponent(double period);
    /*! Constructor with custom state table advance option and 5 msec period
 
        For monitoring mechanism (and period example), automaticAdvance should be OFF.
        For passive filtering mechanism (and event example), automaticAdvance should be ON.
    */
    mtsMonitorComponent(bool automaticAdvance);
    /*! Constructor with custom period and custom state table advance option */
    mtsMonitorComponent(double period, bool automaticAdvance);
    /*! Destructor */
    ~mtsMonitorComponent();

    /*! Pass monitoring target information (i.e., cisstMonitor instance) to the 
        component to be monitored.  The component uses this information to install
        monitoring and FDD pipelines. */
    bool AddMonitorTarget(SF::cisstMonitor * monitorTarget);

    /*! Create target component accessor (useful when creating monitor for filter) */
    TargetComponentAccessor * CreateTargetComponentAccessor(SF::cisstMonitor * monitorTarget);
    TargetComponentAccessor * CreateTargetComponentAccessor(const std::string & targetProcessName, 
                                                            const std::string & targetComponentName,
                                                            SF::Monitor::TargetType targetType,
                                                            bool attachFaultEventHandler, 
                                                            bool addAccessor);

    // TODO: replace this with RemoveMonitorTargetFromComponent()
    /*! Unregister component from the registry */
    bool UnregisterComponent(const std::string & componentName);

    /*! Initialize all accessors (make connections between monitor and target components) */
    bool InitializeAccessors(void);

    void Configure(const std::string & CMN_UNUSED(filename) = ""){}
    void Startup(void) {};
    void Run(void);
    void Cleanup(void);

    //-------------------------------------------------- 
    //  Getters
    //-------------------------------------------------- 
    /*! Return the name of required interface to access a given task's state table */
    static const std::string GetNameOfStateTableAccessInterface(const std::string & taskName);

    /*! Return the name of this component */
    static const std::string & GetNameOfMonitorComponent(void);

    /*! Get target component accessor using component name */
    TargetComponentAccessor * GetTargetComponentAccessor(const std::string & targetComponentName);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsMonitorComponent)

inline std::ostream & operator << (std::ostream & outputStream, 
                                   const mtsMonitorComponent::TargetComponentAccessor & accessor)
{
    accessor.ToStream(outputStream);
    return outputStream;
}

#endif // _mtsMonitorComponent_h

