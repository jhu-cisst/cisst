/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsMonitorComponent.cpp 3303 2012-01-02 16:33:23Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2011-12-29

  (C) Copyright 2004-2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#include "dict.h"

#include <cisstCommon/cmnConstants.h>
#include <cisstOSAbstraction/osaGetTime.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsMonitorComponent.h>
#include <cisstMultiTask/mtsFaultDetectorThresholding.h>
#include <cisstMultiTask/mtsMonitorFilterBasics.h>
#include <cisstMultiTask/mtsFaultTypes.h>

using namespace SF::Dict;

CMN_IMPLEMENT_SERVICES(mtsMonitorComponent);

const std::string NameOfMonitorComponent = "SafetyMonitor";

//-------------------------------------------------- 
//  mtsMonitorComponent::TargetComponentAccessor
//-------------------------------------------------- 
mtsMonitorComponent::TargetComponentAccessor::TargetComponentAccessor()
    : InterfaceRequired(0)
{
}

mtsMonitorComponent::TargetComponentAccessor::~TargetComponentAccessor()
{
    if (InterfaceRequired) delete InterfaceRequired;
}

bool mtsMonitorComponent::TargetComponentAccessor::AddMonitorTargetToAccessor(SF::cisstMonitor * monitor)
{
    SF::Monitor::UIDType uid = monitor->GetUIDAsNumber();
    if (FindMonitorTarget(uid))
        return false;

    MonitorTargetSet.insert(std::make_pair(uid, monitor));

    return true;
}

void mtsMonitorComponent::TargetComponentAccessor::RemoveMonitorTargetFromAccessor(SF::Monitor::UIDType uid)
{
    MonitorTargetSet.erase(uid);
}

bool mtsMonitorComponent::TargetComponentAccessor::FindMonitorTarget(SF::Monitor::UIDType uid) const
{
    MonitorTargetSetType::const_iterator it = MonitorTargetSet.find(uid);
    return (it != MonitorTargetSet.end());
}

bool mtsMonitorComponent::TargetComponentAccessor::FindMonitorTarget(const std::string & uidString) const
{
    MonitorTargetSetType::const_iterator it = MonitorTargetSet.begin();
    const MonitorTargetSetType::const_iterator itEnd = MonitorTargetSet.end();
    for (; it != itEnd; ++it) {
        if (it->second->GetUIDAsString().compare(uidString) == 0)
            return true;
    }

    return false;
}

bool mtsMonitorComponent::TargetComponentAccessor::RefreshSamples(double currentTick, SF::Publisher * publisher)
{
    MonitorTargetSetType::iterator it = MonitorTargetSet.begin();
    const MonitorTargetSetType::iterator itEnd = MonitorTargetSet.end();

    bool advance = false;
    SF::cisstMonitor * monitor;
    for (; it != itEnd; ++it) {
        monitor = it->second;

        // Skip inactive monitor
        if (!monitor->IsActive()) continue;
        // Refresh sample only when needed
        if (!monitor->IsSamplingNecessary(currentTick)) continue;

        SF::Monitor::TargetType targetType = monitor->GetTargetType();
        switch (targetType) {
        case SF::Monitor::TARGET_THREAD_PERIOD:
            // Get new period sample
            if (!GetPeriod.IsValid()) {
                CMN_LOG_RUN_WARNING << "TargetComponentAccessor::RefreshSamples: Failed to fetch new sample: invalid GetPeriod function" << std::endl;
            } else {
                double period;
                this->GetPeriod(period);
                publisher->Publish(monitor->GetJsonForPublish(period, currentTick));
                monitor->UpdateLastSamplingTick(currentTick);
                advance = true;
            }
            break;

        case SF::Monitor::TARGET_THREAD_DUTYCYCLE_USER:
            // Get new duty cycle (user) sample
            if (!this->GetExecTimeUser.IsValid()) {
                CMN_LOG_RUN_WARNING << "TargetComponentAccessor::RefreshSamples: Failed to fetch new sample: invalid GetExecTimeUser function" << std::endl;
            } else {
                double execTimeUser;
                this->GetExecTimeUser(execTimeUser);
                publisher->Publish(monitor->GetJsonForPublish(execTimeUser, currentTick));
                monitor->UpdateLastSamplingTick(currentTick);
                advance = true;
            }
            break;

        case SF::Monitor::TARGET_THREAD_DUTYCYCLE_TOTAL:
            // Get new duty cycle (total) sample
            if (!this->GetExecTimeTotal.IsValid()) {
                CMN_LOG_RUN_WARNING << "TargetComponentAccessor::RefreshSamples: Failed to fetch new sample: invalid GetExecTimeTotal function" << std::endl;
            } else {
                double execTimeTotal;
                this->GetExecTimeTotal(execTimeTotal);
                publisher->Publish(monitor->GetJsonForPublish(execTimeTotal, currentTick));
                monitor->UpdateLastSamplingTick(currentTick);
                advance = true;
            }
            break;

            // [SFUPDATE]

        default:
            CMN_LOG_RUN_WARNING << "TargetComponentAccessor::RefreshSamples: not supported monitoring type" << std::endl;
        }
    }

    return advance;
}

//-------------------------------------------------- 
//  mtsMonitorComponent
//-------------------------------------------------- 
mtsMonitorComponent::mtsMonitorComponent()
    // MJ: Maximum monitoring time resolution is 5 msec (somewhat arbitrary but practically
    // enough to cover most monitoring scenarios)
    : mtsTaskPeriodic(NameOfMonitorComponent, 5.0 * cmn_ms, false, 5000),
      Publisher(0), Subscriber(0), SubscriberCallback(0)
{
    Init();
}

mtsMonitorComponent::mtsMonitorComponent(double period)
    // MJ: Maximum monitoring time resolution is 5 msec (somewhat arbitrary but practically
    // enough to cover most monitoring scenarios)
    : mtsTaskPeriodic(NameOfMonitorComponent, period, false, 5000),
      Publisher(0), Subscriber(0), SubscriberCallback(0)
{
    Init();
}

void mtsMonitorComponent::Init(void)
{
    TargetComponentAccessors = new TargetComponentAccessorType(true);

    // The monitoring state table doesn't advance automatically.  It advances only when needed 
    // to reduce run-time overhead because the advancement of the state table results in 
    // running all FDD pipelines.  The manual advancement is controlled by 
    // mtsMonitorComponent::UpdateFilters(void).
    this->StateTableMonitor.SetAutomaticAdvance(false);

    Publisher = new SF::Publisher(TopicNames::Monitor);
    Publisher->Startup();
#if 0
    ThreadPublisher.Thread.Create<mtsMonitorComponent, unsigned int>(this, &mtsMonitorComponent::RunPublisher, 0);
    ThreadPublisher.ThreadEventBegin.Wait();
#endif

    SubscriberCallback = new mtsSubscriberCallback;
    Subscriber = new SF::Subscriber(TopicNames::Supervisor, SubscriberCallback);
    ThreadSubscriber.Thread.Create<mtsMonitorComponent, unsigned int>(this, &mtsMonitorComponent::RunSubscriber, 0);
    ThreadSubscriber.ThreadEventBegin.Wait();
}

mtsMonitorComponent::~mtsMonitorComponent()
{
    TargetComponentAccessors->DeleteAll();
    delete TargetComponentAccessors;

    if (ThreadPublisher.Running || ThreadSubscriber.Running)
        Cleanup();

    if (Publisher) delete Publisher;
}

void mtsMonitorComponent::Run(void)
{
    ProcessQueuedCommands();
    ProcessQueuedEvents();

    UpdateFilters();

    // Subscriber received message(s)
    if (!SubscriberCallback->IsEmptyQueue()) {
        SubscriberCallback->FetchMessages(Messages);
        // smmy
        //std::cout << "MONITOR COMPONENT fetched " << Messages.size() - before << " items: " << before << std::endl;
    }
}

void * mtsMonitorComponent::RunPublisher(unsigned int CMN_UNUSED(arg))
{
#if 0
    ThreadPublisher.Running = true;

    ThreadPublisher.ThreadEventBegin.Raise();

    Publisher->Startup();
    while (ThreadPublisher.Running) {
        Publisher->Run();
        // smmy
        osaSleep(1.0);
    }
    Publisher->Stop();

    ThreadPublisher.ThreadEventEnd.Raise();
#endif

    return 0;
}

void * mtsMonitorComponent::RunSubscriber(unsigned int CMN_UNUSED(arg))
{
    ThreadSubscriber.Running = true;

    ThreadSubscriber.ThreadEventBegin.Raise();

    Subscriber->Startup();
    while (ThreadSubscriber.Running) {
        Subscriber->Run();
    }

    ThreadSubscriber.ThreadEventEnd.Raise();

    return 0;
}

void mtsMonitorComponent::Cleanup(void)
{
#if 0
    ThreadPublisher.Running = false;
    ThreadPublisher.ThreadEventEnd.Wait();
#endif

    //CMN_LOG_CLASS_RUN_DEBUG << "Cleanup: Monitor component is cleaned up" << std::endl;

    if (Subscriber) {
        ThreadSubscriber.Running = false;
        // Terminating subscriber needs to call shutdown() on the Ice communicator
        Subscriber->Stop();
        ThreadSubscriber.ThreadEventEnd.Wait();

        delete Subscriber;
        delete SubscriberCallback;

        Subscriber = 0;
        SubscriberCallback = 0;
    }
}

bool mtsMonitorComponent::AddMonitorTarget(SF::cisstMonitor * monitorTarget)
{
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    SF::cisstEventLocation * locationID = 
        dynamic_cast<SF::cisstEventLocation*>(monitorTarget->GetLocationID());
    CMN_ASSERT(locationID);

    // Validity check: process name
    const std::string thisProcessName = LCM->GetProcessName();
    const std::string processName = locationID->GetProcessName();
    if (thisProcessName.compare(processName)) {
        CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTarget: different process name "
            << "(expected: \"" << thisProcessName << "\", actual: \"" << processName << ")" << std::endl;
        return false;
    }

    // Make sure if the target component exists.
    const std::string targetComponentName = locationID->GetComponentName();
    mtsComponent * targetComponent = LCM->GetComponent(targetComponentName);
    if (!targetComponent) {
        CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTarget: no component \"" << targetComponentName << "\" found" << std::endl;
        return false;
    }

    // Embed all necessary sampling mechanism into component depending on the type of 
    // component (e.g., mtsComponent, mtsTaskPeriodic, ...) and the specification of 
    // monitor target.

    // Fetch target component information if registered already.  If not, create new instance.
    bool newTargetComponent = false;
    TargetComponentAccessor * targetComponentAccessor = TargetComponentAccessors->GetItem(targetComponentName);
    if (!targetComponentAccessor) {
        newTargetComponent = true;
        // Add new target component
        targetComponentAccessor = new TargetComponentAccessor;
        targetComponentAccessor->ProcessName = thisProcessName;
        targetComponentAccessor->ComponentName = targetComponentName;
        targetComponentAccessor->InterfaceRequired = AddInterfaceRequired(GetNameOfStateTableAccessInterface(targetComponentName));
        // Add monitor event handler if new tareget component is to be added.
        targetComponentAccessor->InterfaceRequired->AddEventHandlerWrite(
            &mtsMonitorComponent::HandleMonitorEvent, this, MonitorNames::MonitorEvent);
        // Add fault event handler if new tareget component is to be added.
#if 0
        targetComponentAccessor->InterfaceRequired->AddEventHandlerWrite(
            &mtsMonitorComponent::HandleFaultEvent, this, FaultNames::FaultEvent);
#else
        targetComponentAccessor->InterfaceRequired->AddEventReceiver(
            FaultNames::FaultEvent, targetComponentAccessor->FaultEventReceiver, MTS_OPTIONAL);
        targetComponentAccessor->FaultEventReceiver.SetHandler(
            &mtsMonitorComponent::HandleFaultEvent, this);
#endif
    } else {
        // Check duplicate monitor target
        if (targetComponentAccessor->FindMonitorTarget(monitorTarget->GetUIDAsString())) {
            CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTarget: Failed to install monitor [ " 
                << monitorTarget->GetUIDAsString() << " ] to "
                << targetComponentName << ": already monitored" << std::endl;
            return false;
        }
    }
    // Add monitor target to component accessor
    CMN_ASSERT(targetComponentAccessor->AddMonitorTargetToAccessor(monitorTarget));

    const SF::Monitor::TargetType targetType = monitorTarget->GetTargetType();
    const std::string targetTypeString = SF::Monitor::GetTargetTypeString(targetType);
    switch (targetType) {
        case SF::Monitor::TARGET_THREAD_PERIOD:
            {
                mtsTaskPeriodic * taskPeriodic = dynamic_cast<mtsTaskPeriodic*>(LCM->GetComponent(targetComponentName));
                if (!taskPeriodic) {
                    CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTarget: " << targetTypeString << " is only applicable to periodic task" << std::endl;
                    if (newTargetComponent) delete targetComponentAccessor;
                    return false;
                }

                targetComponentAccessor->InterfaceRequired->AddFunction("GetPeriod", targetComponentAccessor->GetPeriod);

                // MJ TODO: this (i.e., definining filters and setting up FDD pipeline) should 
                // be done via JSON.  For now, install filters and FDD pipelines by default with
                // hard-coded fixed parameters.
                //if (!InstallFilters(targetComponentAccessor, taskPeriodic)) {
                InstallMonitorTarget(taskPeriodic, monitorTarget);
            }
            break;

        case SF::Monitor::TARGET_THREAD_DUTYCYCLE_USER:
            {
                mtsTaskPeriodic * taskPeriodic = dynamic_cast<mtsTaskPeriodic*>(LCM->GetComponent(targetComponentName));
                if (!taskPeriodic) {
                    CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTarget: " << targetTypeString << " is only applicable to periodic task" << std::endl;
                    if (newTargetComponent) delete targetComponentAccessor;
                    return false;
                }

                targetComponentAccessor->InterfaceRequired->AddFunction("GetExecTimeUser", targetComponentAccessor->GetExecTimeUser);

                InstallMonitorTarget(taskPeriodic, monitorTarget);
            }
            break;

        case SF::Monitor::TARGET_THREAD_DUTYCYCLE_TOTAL:
            {
                mtsTaskPeriodic * taskPeriodic = dynamic_cast<mtsTaskPeriodic*>(LCM->GetComponent(targetComponentName));
                if (!taskPeriodic) {
                    CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTarget: " << targetTypeString << " is only applicable to periodic task" << std::endl;
                    if (newTargetComponent) delete targetComponentAccessor;
                    return false;
                }

                targetComponentAccessor->InterfaceRequired->AddFunction("GetExecTimeTotal", targetComponentAccessor->GetExecTimeTotal);

                InstallMonitorTarget(taskPeriodic, monitorTarget);
            }
            break;

        case SF::Monitor::TARGET_FILTER_EVENT:
            {
                // NOP: Do not add function to fetch data sample from target component.
                // Events generated by filters are propagated to the entire system, i.e., 
                // safety supervisor, using publish service that this monitor provides.
            }
            break;

            // [SFUPDATE]

        case SF::Monitor::TARGET_INVALID:
        default:
            CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTarget: invalid or unsupported monitor type: " << std::endl;
            if (newTargetComponent) delete targetComponentAccessor;
            // MJ TODO: implement mtsComponent::RemoveMonitorTarget()
            // targetComponent->RemoveMonitorTarget()
            return false;
            break;
    }

    // Check if the same monitoring target element has already been registered.
    /* smmy MJ TODO: How to efficiently access which quantities are being monitored?
    mtsComponent * targetComponent = LCM->GetComponent(targetComponentName);
    CMN_ASSERT(targetComponent);
    if (!targetComponent->AddMonitorTarget(monitorTarget) {
        CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTarget: duplicate monitoring target element: " << targetTypeString << std::endl;
        return false;
    }
    */

    if (newTargetComponent) {
        if (!TargetComponentAccessors->AddItem(targetComponentName, targetComponentAccessor)) {
            CMN_LOG_CLASS_RUN_ERROR << "RegisterComponent: Failed to add state table access interface for component \"" << targetComponentName << "\"" << std::endl;
            return false;
        }
    }

    PrintTargetComponents();

    CMN_LOG_CLASS_RUN_DEBUG << "RegisterComponent: Successfully registered component: \"" << targetComponentName << "\"" << std::endl;

    return true;
}

bool mtsMonitorComponent::InitializeAccessors(void)
{
    // Connect new monitoring target component to monitor component 
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();

    TargetComponentAccessorType::const_iterator it = TargetComponentAccessors->begin();
    const TargetComponentAccessorType::const_iterator itEnd = TargetComponentAccessors->end();
    TargetComponentAccessor * accessor;
    for (; it != itEnd; ++it) {
        accessor = it->second;

        mtsTask * task = LCM->GetComponentAsTask(accessor->ComponentName); 
        if (!task) { // [SFUPDATE]
            CMN_LOG_CLASS_RUN_ERROR << "Only task-type components can be monitored: component \"" << accessor->ComponentName << "\"" << std::endl;
            return false; // MJ TODO
        }

        if (!LCM->Connect(mtsMonitorComponent::GetNameOfMonitorComponent(), GetNameOfStateTableAccessInterface(accessor->ComponentName),
                          accessor->ComponentName, mtsStateTable::GetNameOfStateTableInterface(task->GetMonitoringStateTableName())))
        {
            if (!UnregisterComponent(accessor->ComponentName)) {
                CMN_LOG_CLASS_RUN_ERROR << "Failed to unregister component \"" << accessor->ComponentName << "\" from monitor component" << std::endl;
            }

            CMN_LOG_CLASS_RUN_ERROR << "Failed to connect component \"" << accessor->ComponentName << "\" to monitor component" << std::endl;
            return false;
        }
    }

    return true;
}

void mtsMonitorComponent::UpdateFilters(void)
{
    const double currentTick = osaGetTime();

    TargetComponentAccessorType::const_iterator it = TargetComponentAccessors->begin();
    const TargetComponentAccessorType::const_iterator itEnd = TargetComponentAccessors->end();
    bool advance = false;
    for (; it != itEnd; ++it) {
        advance |= it->second->RefreshSamples(currentTick, Publisher);
    }

    // Do filtering and run all FDD pipelines (MJ: this can be further optimized such that
    // only updated samples are processed).
    if (advance)
        StateTableMonitor.Advance();
}

void mtsMonitorComponent::PrintTargetComponents(void)
{
    // MJ TODO
    /*
    std::stringstream ss;
    ss << "Monitoring target component: ";

    TargetComponentAccessorType::const_iterator it = TargetComponentAccessors->begin();
    const TargetComponentAccessorType::const_iterator itEnd = TargetComponentAccessors->end();
    int i = 0;
    for (; it != itEnd; ++it) {
        TargetComponentAccessor * target = it->second;
        CMN_ASSERT(target);
        target->GetPeriod(target->Period);
        ss << "[" << ++i << "] " << target->ProcessName << " : " << target->ComponentName << ", period = " << target->Period << std::endl;
    }

    CMN_LOG_CLASS_RUN_DEBUG << ss.str() << std::endl;
    */
}

void mtsMonitorComponent::InstallMonitorTarget(mtsTask * task, SF::Monitor * monitor)
{
    const std::string taskName = task->GetName();
    std::string newElementName(taskName);

    // MJ TEMP: Adding a new element to state table on the fly may be not thread safe.
    switch (monitor->GetTargetType()) {
        case SF::Monitor::TARGET_THREAD_PERIOD:
            // Add "Period" to the monitoring state table of this component with the name of
            // (component name)+"Period"
            newElementName += "Period";
            this->StateTableMonitor.NewElement(newElementName, &monitor->Samples.Period);
            break;

        case SF::Monitor::TARGET_THREAD_DUTYCYCLE_USER:
            newElementName += "ExecTimeUser";
            this->StateTableMonitor.NewElement(newElementName, &monitor->Samples.ExecTimeUser);
            break;

        case SF::Monitor::TARGET_THREAD_DUTYCYCLE_TOTAL:
            newElementName += "ExecTimeTotal";
            this->StateTableMonitor.NewElement(newElementName, &monitor->Samples.ExecTimeTotal);
            break;

        case SF::Monitor::TARGET_FILTER_EVENT:
            // This should not be called: Output of filter doesn't need to be tracked.
            CMN_ASSERT(false);
            break;

        // [SFUPDATE]

        default:
            ;
    }
}

bool mtsMonitorComponent::InstallFilters(TargetComponentAccessor * CMN_UNUSED(entry),
                                         mtsTaskPeriodic * CMN_UNUSED(task))
{
    /*
    CMN_ASSERT(task);

    const std::string taskName = task->GetName();

    // Add "Period" to the monitoring state table of this component with the name of
    // (component name)+"Period"
    std::string periodName(taskName);
    periodName += "Period";
    // MJ TEMP: Adding a new element to state table on the fly is not thread safe -- need to fix this.
    this->StateTableMonitor.NewElement(periodName, &entry->Period);

    // MJ TODO: add clean up codes when error happens
#define ADD_FILTER( _filter )\
    if (!this->StateTableMonitor.AddFilter(_filter)) {\
        CMN_LOG_CLASS_RUN_ERROR\
           << "RegisterTargetComponent: Failed to add filter \"" << _filter->GetFilterName()\
           << "\" for task \"" << taskName << "\"";\
        return false;\
    }

    // Create bypass and 1st order differentiator filters
    // Bypass filter for self monitoring (testing purpose) MJ: remove this later
#if 0
    mtsMonitorFilterBypass * filterBypass = 
        new mtsMonitorFilterBypass(mtsMonitorFilterBase::FEATURE, 
                                   //mtsStateTable::NamesOfDefaultElements::Period,
                                   periodName, // period of target component
                                   mtsMonitorFilterBase::SignalElement::SCALAR);
    ADD_FILTER(filterBypass);

    // 1-st order differentiation filter
    mtsMonitorFilterTrendVel * filterTrendVel = 
        new mtsMonitorFilterTrendVel(mtsMonitorFilterBase::FEATURE, 
                                     periodName, 
                                     mtsMonitorFilterBase::SignalElement::SCALAR);
    ADD_FILTER(filterTrendVel);

    // Create vectorize filter to define feature vector
    mtsMonitorFilterBase::SignalNamesType inputNames;
    inputNames.push_back(periodName);
    inputNames.push_back(filterTrendVel->GetOutputSignalName(0));
    mtsMonitorFilterVectorize * filterVectorize = 
        new mtsMonitorFilterVectorize(mtsMonitorFilterBase::FEATURE_VECTOR, inputNames);
    ADD_FILTER(filterVectorize);

    // Create subtraction filter to define feature vector
    mtsMonitorFilterBase::PlaceholderVectorType vecExpected(2);
    vecExpected(0) = task->GetPeriodicity(true); // Get nominal period
    vecExpected(1) = 0.0;
    mtsMonitorFilterArithmetic * filterArithmetic = 
        new mtsMonitorFilterArithmetic(mtsMonitorFilterBase::FEATURE_VECTOR,
                                       mtsMonitorFilterArithmetic::SUBTRACTION,
                                       // in 1: actual
                                       filterVectorize->GetOutputSignalName(0),
                                       mtsMonitorFilterBase::SignalElement::VECTOR,
                                       // in 2: expected
                                       vecExpected);
    ADD_FILTER(filterArithmetic);

    // Create moving average filter to define feature
    mtsMonitorFilterAverage * filterAverage = 
        new mtsMonitorFilterAverage(mtsMonitorFilterBase::FEATURE,
                                    periodName,
                                    mtsMonitorFilterBase::SignalElement::SCALAR,
                                    0.25);
    ADD_FILTER(filterAverage);
#endif

    // Create subtraction filter to define feature vector
    mtsMonitorFilterBase::PlaceholderType periodExpected = task->GetPeriodicity(true); // Get nominal period
    mtsMonitorFilterArithmetic * filterArithmetic = 
        new mtsMonitorFilterArithmetic(mtsMonitorFilterBase::FEATURE,
                                       mtsMonitorFilterArithmetic::SUBTRACTION,
                                       // in 1: actual
                                       periodName,
                                       mtsMonitorFilterBase::SignalElement::SCALAR,
                                       // in 2: expected
                                       periodExpected);
    ADD_FILTER(filterArithmetic);

    // Create fault detector based on thresholding filter
    mtsFaultDetectorThresholding * detectorThresholding = 
        new mtsFaultDetectorThresholding(//filterAverage->GetOutputSignalName(0),
                                         periodName,
                                         periodExpected, 
                                         10, 1); // MJTEMP
                                         //(size_t)(1.0 / periodExpected) * 5.0);
    ADD_FILTER(detectorThresholding);

    // Associate thread periodicity fault with the fault detector to make the fault
    // detector monitor and generate the specified fault.
    mtsFaultComponentThreadPeriodicity * fault = 
        new mtsFaultComponentThreadPeriodicity(mtsManagerLocal::GetInstance()->GetProcessName(), taskName);
    detectorThresholding->RegisterFault(fault);

#undef ADD_FILTER 
    */

    return true;
}

void mtsMonitorComponent::HandleMonitorEvent(const std::string & json)
{
    // If the monitor component receives an event regardless of its type (monitor or
    // fault), publish the event to the safety framework as is.  The Safety Supervisor
    // will take care of the event.
    Publisher->Publish(json);

    // MJ TODO: Depending on the type of event (esp. in case of fault events),
    // the Safety Coordinator in each process can deal with events or faults locally,
    // i.e., within the process boundary.
    // This would be the best-performance-fault-handling case but it loses the
    // system-wide fault (event) propagation and global coordination by the "brain."
}

void mtsMonitorComponent::HandleFaultEvent(const std::string & json)
{
    // If the monitor component receives an event regardless of its type (monitor or
    // fault), publish the event to the safety framework as is.  The Safety Supervisor
    // will take care of the event.
    Publisher->Publish(json);

    // MJ TODO: Depending on the type of event (esp. in case of fault events),
    // the Safety Coordinator in each process can deal with events or faults locally,
    // i.e., within the process boundary.
    // This would be the best-performance-fault-handling case but it loses the
    // system-wide fault (event) propagation and global coordination by the "brain."
}

bool mtsMonitorComponent::UnregisterComponent(const std::string & componentName)
{
    // MJ TODO: cancel and clean up all monitoring target that the component has

    if (!TargetComponentAccessors->FindItem(componentName)) {
        CMN_LOG_CLASS_RUN_WARNING << "UnregisterComponent: component \"" << componentName << "\" is not found" << std::endl;
        return false;
    }

    bool ret = TargetComponentAccessors->RemoveItem(componentName);
    if (!ret) {
        CMN_LOG_CLASS_RUN_WARNING << "UnregisterComponent: failed to remove component \"" << componentName << "\"" << std::endl;
        return false;
    }

    CMN_LOG_CLASS_RUN_DEBUG << "UnregisterComponent: successfully unregistered component \"" << componentName << "\"" << std::endl;

    return true;
}

//-----------------------------------------------
//  Getters
//-----------------------------------------------
const std::string  mtsMonitorComponent::GetNameOfStateTableAccessInterface(const std::string & taskName) const {
    return "Monitor" + taskName;
}
const std::string & mtsMonitorComponent::GetNameOfMonitorComponent(void) {
    return NameOfMonitorComponent;
}

