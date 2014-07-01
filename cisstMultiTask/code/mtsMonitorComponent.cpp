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
#include "jsonSerializer.h"

#include <cisstMultiTask/mtsMonitorComponent.h>

#include <cisstCommon/cmnConstants.h>
#include <cisstOSAbstraction/osaGetTime.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsFaultDetectorThresholding.h>
#include <cisstMultiTask/mtsMonitorFilterBasics.h>
#include <cisstMultiTask/mtsFaultTypes.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

CMN_IMPLEMENT_SERVICES(mtsMonitorComponent);

const std::string NameOfMonitorComponent = "SafetyMonitor";

//-------------------------------------------------- 
//  mtsMonitorComponent::TargetComponentAccessor
//-------------------------------------------------- 
mtsMonitorComponent::TargetComponentAccessor::TargetComponentAccessor(void)
    : ManualAdvance(false), InterfaceRequiredPredefined(0)
{}
        
mtsMonitorComponent::TargetComponentAccessor::TargetComponentAccessor(bool manualAdvance)
    : ManualAdvance(manualAdvance), InterfaceRequiredPredefined(0)
{}

mtsMonitorComponent::TargetComponentAccessor::~TargetComponentAccessor()
{
    if (InterfaceRequiredPredefined) delete InterfaceRequiredPredefined;

    // TODO: add nother cleanups
}

bool mtsMonitorComponent::TargetComponentAccessor::AddMonitorTargetToAccessor(SF::cisstMonitor * monitorTarget)
{
    std::string targetUID = monitorTarget->GetUIDAsString();
    if (FindMonitorTargetFromAccessor(targetUID))
        return false;

    MonitorTargetSet.insert(std::make_pair(targetUID, monitorTarget));

    return true;
}

void mtsMonitorComponent::TargetComponentAccessor::RemoveMonitorTargetFromAccessor(const std::string & targetUID)
{
    MonitorTargetSet.erase(targetUID);
}

bool mtsMonitorComponent::TargetComponentAccessor::FindMonitorTargetFromAccessor(const std::string & targetUID) const
{
    MonitorTargetSetType::const_iterator it = MonitorTargetSet.begin();
    const MonitorTargetSetType::const_iterator itEnd = MonitorTargetSet.end();
    for (; it != itEnd; ++it) {
        if (it->second->GetUIDAsString().compare(targetUID) == 0)
            return true;
    }

    return false;
}

bool mtsMonitorComponent::TargetComponentAccessor::RefreshSamples(double currentTick, SF::Publisher * publisher)
{
    // used only for manual advance
    bool advance = false;
    SF::cisstMonitor * monitor = 0;
    // for custom monitoring type
    mtsInterfaceRequired * required = 0;
    mtsFunctionRead * functionRead = 0;
    InterfaceRequiredCustomType::const_iterator it2;

    MonitorTargetSetType::iterator it = MonitorTargetSet.begin();
    const MonitorTargetSetType::iterator itEnd = MonitorTargetSet.end();
    for (; it != itEnd; ++it) {
        monitor = it->second;

        // Skip inactive monitor
        if (!monitor->IsActive())
            continue;
        // Refresh sample only when needed
        if (!monitor->IsSamplingNecessary(currentTick))
            continue;

        SF::Monitor::TargetType targetType = monitor->GetTargetType();
        switch (targetType) {
        case SF::Monitor::TARGET_THREAD_PERIOD:
            // Get new period sample
            if (!AccessFunctions.GetPeriod.IsValid()) {
                CMN_LOG_RUN_WARNING << "TargetComponentAccessor::RefreshSamples: Failed to fetch new sample: invalid GetPeriod function" << std::endl;
            } else {
                double period;
                this->AccessFunctions.GetPeriod(period);
                publisher->PublishData(SF::Topic::Data::MONITOR, 
                                       monitor->GetJsonForPublish(period, currentTick));
                monitor->UpdateLastSamplingTick(currentTick);
                if (ManualAdvance)
                    advance = true;
            }
            break;

        case SF::Monitor::TARGET_THREAD_DUTYCYCLE_USER:
            // Get new duty cycle (user) sample
            if (!this->AccessFunctions.GetExecTimeUser.IsValid()) {
                CMN_LOG_RUN_WARNING << "TargetComponentAccessor::RefreshSamples: Failed to fetch new sample: invalid GetExecTimeUser function" << std::endl;
            } else {
                double execTimeUser;
                this->AccessFunctions.GetExecTimeUser(execTimeUser);
                publisher->PublishData(SF::Topic::Data::MONITOR,
                                       monitor->GetJsonForPublish(execTimeUser, currentTick));
                monitor->UpdateLastSamplingTick(currentTick);
                if (ManualAdvance)
                    advance = true;
            }
            break;

        case SF::Monitor::TARGET_THREAD_DUTYCYCLE_TOTAL:
            // Get new duty cycle (total) sample
            if (!this->AccessFunctions.GetExecTimeTotal.IsValid()) {
                CMN_LOG_RUN_WARNING << "TargetComponentAccessor::RefreshSamples: Failed to fetch new sample: invalid GetExecTimeTotal function" << std::endl;
            } else {
                double execTimeTotal;
                this->AccessFunctions.GetExecTimeTotal(execTimeTotal);
                publisher->PublishData(SF::Topic::Data::MONITOR,
                                       monitor->GetJsonForPublish(execTimeTotal, currentTick));
                monitor->UpdateLastSamplingTick(currentTick);
                if (ManualAdvance)
                    advance = true;
            }
            break;

        case SF::Monitor::TARGET_CUSTOM:
            it2 = InterfaceRequiredCustom.find(monitor->GetLocationID()->GetInterfaceProvidedName());
            CMN_ASSERT(it2 != InterfaceRequiredCustom.end());
            required = it2->second;
            functionRead = required->GetFunctionRead(monitor->GetLocationID()->GetCommandName());
            CMN_ASSERT(functionRead);
            if (!functionRead->IsValid()) {
                CMN_LOG_RUN_WARNING << "TargetComponentAccessor::RefreshSamples: Failed to fetch new sample: invalid custom function (\"" 
                    << monitor->GetLocationID()->GetCommandName() << "\"" << std::endl;
            } else {
                // Create a temporary argument which includes dynamic allocation internally.
                // Therefore, this object should be deallocated manually.
                mtsFunctionRead::CommandType * command = functionRead->GetCommand();
                CMN_ASSERT(command);
                mtsGenericObject * tempArgument;
                {
                    tempArgument = dynamic_cast<mtsGenericObject *>(
                        command->GetArgumentPrototype()->Services()->Create());
                    if (!tempArgument) {
                        CMN_LOG_RUN_WARNING << "TargetComponentAccessor::RefreshSamples: Failed to created argument prototype for command \"" 
                            << command->GetName() << "\"" << std::endl;
                        continue;
                    }

                    // fetch new value
                    (*functionRead)(*tempArgument);

                    // publish new reading to the system via Safety Coordinator's publisher
                    //publisher->Publish(monitor->GetJsonForPublish(sample, currentTick));
                    publisher->PublishData(SF::Topic::Data::MONITOR,
                                           mtsSafetyCoordinator::GetJsonForPublish(
                                               *monitor, tempArgument, osaGetTime()));
                }
                delete tempArgument;

                monitor->UpdateLastSamplingTick(currentTick);
                if (ManualAdvance)
                    advance = true;
            }
            break;

            // [SFUPDATE]

        default:
            CMN_LOG_RUN_WARNING << "TargetComponentAccessor::RefreshSamples: not supported monitoring type" << std::endl;
        }
    }

    return (ManualAdvance ? advance : true);
}

bool mtsMonitorComponent::TargetComponentAccessor::AddMonitoringFunction(SF::Monitor::TargetType type,
                                                                         const std::string & providedInterfaceName,
                                                                         const std::string & targetCommandName)
{
    bool ret = false;

    switch (type) {
        case SF::Monitor::TARGET_THREAD_PERIOD:
            ret = InterfaceRequiredPredefined->AddFunction("GetPeriod", this->AccessFunctions.GetPeriod);
            break;
        case SF::Monitor::TARGET_THREAD_DUTYCYCLE_USER:
            ret = InterfaceRequiredPredefined->AddFunction("GetExecTimeUser", this->AccessFunctions.GetExecTimeUser);
            break;
        case SF::Monitor::TARGET_THREAD_DUTYCYCLE_TOTAL:
            ret = InterfaceRequiredPredefined->AddFunction("GetExecTimeTotal", this->AccessFunctions.GetExecTimeTotal);
            break;
        case SF::Monitor::TARGET_FILTER_EVENT: // MJTEMP: what is this???
            // NOP
            break;
        case SF::Monitor::TARGET_CUSTOM:
            {
                // Check if required interface for custom monitoring target already exists
                CMN_ASSERT(InterfaceRequiredCustom.find(providedInterfaceName) != InterfaceRequiredCustom.end());
                mtsInterfaceRequired * required = (InterfaceRequiredCustom.find(providedInterfaceName))->second;
                CMN_ASSERT(required);
                mtsFunctionRead * function = new mtsFunctionRead;
                CMN_ASSERT(required->AddFunction(targetCommandName, *function));
                ret = true;
            }
            break;
        case SF::Monitor::TARGET_INVALID:
            ret = false;
            break;
        default:
            SFASSERT(false);
    }

    return ret;
}

void mtsMonitorComponent::TargetComponentAccessor::ToStream(std::ostream & outputStream) const
{
    outputStream << "Target process  : \"" << this->ProcessName << "\"" << std::endl
                 << "Target component: \"" << this->ComponentName << "\"" << std::endl 
                 << "Monitor targets:" << std::endl;
    // Monitor targets
    MonitorTargetSetType::const_iterator it1 = MonitorTargetSet.begin();
    int i = 0;
    for (; it1 != MonitorTargetSet.end(); ++it1)
        outputStream << "\t[" << ++i << "] " << *(it1->second) << std::endl;

    // Access functions
    outputStream << "Predefined targets -------------------------------------" << std::endl
                 << "\tGetPeriod: " << AccessFunctions.GetPeriod << std::endl
                 << "\tGetExecTimeUser: " << AccessFunctions.GetExecTimeUser << std::endl
                 << "\tGetExecTimeTotal: " << AccessFunctions.GetExecTimeTotal << std::endl;
    outputStream << "\tInterfaceRequiredPredefined: ";
    if (InterfaceRequiredPredefined == 0)
        outputStream << "NULL" << std::endl;
    else
        outputStream << *InterfaceRequiredPredefined << std::endl;

    InterfaceRequiredCustomType::const_iterator it2 = InterfaceRequiredCustom.begin();
    outputStream << "Custom targets:" << std::endl;
    for (; it2 != InterfaceRequiredCustom.end(); ++it2) {
        outputStream << "\tprv.int \"" << it2->first << "\" -------------------------------------" << std::endl;
        outputStream << *(it2->second) << std::endl;
    }
}

//-------------------------------------------------- 
//  mtsMonitorComponent
//-------------------------------------------------- 
// MJ: Default monitoring time period is set as 5 msec, which is somewhat arbitrary 
// but practically short enough to cover most monitoring scenarios.
const double DefaultMonitoringPeriod = 5.0 * cmn_ms;

mtsMonitorComponent::mtsMonitorComponent(void)
    : mtsTaskPeriodic(NameOfMonitorComponent, DefaultMonitoringPeriod, false, 5000),
      ManualAdvance(false)
{
    Initialize();
}

mtsMonitorComponent::mtsMonitorComponent(double period)
    : mtsTaskPeriodic(NameOfMonitorComponent, period, false, 5000),
      ManualAdvance(false)
{
    Initialize();
}

mtsMonitorComponent::mtsMonitorComponent(bool automaticAdvance)
    : mtsTaskPeriodic(NameOfMonitorComponent, DefaultMonitoringPeriod, false, 5000),
      ManualAdvance(!automaticAdvance)
{
    Initialize();
}

mtsMonitorComponent::mtsMonitorComponent(double period, bool automaticAdvance)
    : mtsTaskPeriodic(NameOfMonitorComponent, period, false, 5000),
      ManualAdvance(!automaticAdvance)
{
    Initialize();
}

void mtsMonitorComponent::Initialize(void)
{
    Publisher = 0;
    //Subscriber = 0;
    //SubscriberCallback = 0;

    TargetComponentAccessors = new TargetComponentAccessorType(ManualAdvance);

    // The monitoring state table doesn't advance automatically.  It advances only when needed 
    // to reduce run-time overhead because the advancement of the state table results in 
    // running all FDD pipelines.  The manual advancement is controlled by 
    // mtsMonitorComponent::RunMonitors(void).
    this->StateTableMonitor.SetAutomaticAdvance(!ManualAdvance);

    Publisher = new SF::Publisher(SF::Dict::TopicNames::DATA);
    if (!Publisher->Startup()) {
        std::stringstream ss;
        ss << "mtsMonitorComponent: Failed to initialize publisher for topic \""
           << SF::Dict::TopicNames::DATA << "\"";
        cmnThrow(ss.str());
    }
#if 0
    ThreadPublisher.Thread.Create<mtsMonitorComponent, unsigned int>(this, &mtsMonitorComponent::RunPublisher, 0);
    ThreadPublisher.ThreadEventBegin.Wait();
#endif

#if 0
    SubscriberCallback = new mtsSubscriberCallback;
    Subscriber = new SF::Subscriber(SF::Dict::TopicNames::CONTROL, SubscriberCallback);
    ThreadSubscriber.Thread.Create<mtsMonitorComponent, unsigned int>(this, &mtsMonitorComponent::RunSubscriber, 0);
    ThreadSubscriber.ThreadEventBegin.Wait();
#endif
}

mtsMonitorComponent::~mtsMonitorComponent()
{
    TargetComponentAccessors->DeleteAll();
    delete TargetComponentAccessors;
}

void mtsMonitorComponent::Run(void)
{
    ProcessQueuedCommands();
    ProcessQueuedEvents();

    RunMonitors();
}

void mtsMonitorComponent::Cleanup(void)
{
}

mtsMonitorComponent::TargetComponentAccessor * mtsMonitorComponent::CreateTargetComponentAccessor(
    SF::cisstMonitor * monitorTarget)
{
    const std::string targetProcessName(monitorTarget->GetLocationID()->GetProcessName());
    const std::string targetComponentName(monitorTarget->GetLocationID()->GetComponentName());
    bool  attachedToActiveFilter = monitorTarget->IsAttachedToActiveFilter();
    const SF::Monitor::TargetType targetType = monitorTarget->GetTargetType();
    
    return CreateTargetComponentAccessor(targetProcessName, 
                                         targetComponentName, 
                                         targetType,
                                         attachedToActiveFilter,
                                         false); // MJFIXME: true in case of active filtering (?)
}

mtsMonitorComponent::TargetComponentAccessor * 
    mtsMonitorComponent::CreateTargetComponentAccessor(const std::string & targetProcessName, 
                                                       const std::string & targetComponentName,
                                                       const SF::Monitor::TargetType targetType,
                                                       bool attachFaultEventHandler, 
                                                       bool addAccessor)
{
    TargetComponentAccessor * targetComponentAccessor = new TargetComponentAccessor;
    targetComponentAccessor->ProcessName = targetProcessName;
    targetComponentAccessor->ComponentName = targetComponentName;

    // For predefined monitoring targets, add required interface that accesses target
    // component's monitoring state table
    mtsInterfaceRequired * required = 0;
    // If predefined targets
    if (targetType != SF::Monitor::TARGET_CUSTOM) {
        targetComponentAccessor->InterfaceRequiredPredefined = 
            this->AddInterfaceRequired(GetNameOfStateTableAccessInterface(targetComponentName), MTS_OPTIONAL);
        required = targetComponentAccessor->InterfaceRequiredPredefined;

        CMN_LOG_CLASS_RUN_DEBUG << "CreateTargetComponentAccessor: Created internal required interface "
            << "\"" << GetNameOfStateTableAccessInterface(targetComponentName) << "\" for predefined "
            << "monitoring target" << std::endl;

        // MJ FIXME: can't HandleMonitorEvent be moved to if () down below?
        // Add monitor event handler if new tareget component is to be added.
        required->AddEventHandlerWrite(&mtsMonitorComponent::HandleMonitorEvent, this, SF::Dict::MonitorNames::MonitorEvent);
        // Add fault event handler if new tareget component is to be added.
        if (attachFaultEventHandler) {
            required->AddEventReceiver(SF::Dict::FaultNames::FaultEvent, targetComponentAccessor->FaultEventReceiver, MTS_OPTIONAL);
            targetComponentAccessor->FaultEventReceiver.SetHandler(&mtsMonitorComponent::HandleFaultEvent, this);
        }

        if (addAccessor) {
            if (!TargetComponentAccessors->AddItem(targetComponentName, targetComponentAccessor)) {
                CMN_LOG_CLASS_RUN_ERROR << "CreateTargetComponentAccessor: Failed to add state table access interface for component \"" << targetComponentName << "\"" << std::endl;
                delete targetComponentAccessor;
                return 0;
            }
        }
    } else {
        // FIXME: WHAT IF CUSTOM TARGET?? Doesn't it need event handlers??????
        // -> will handle it later when working on filtering.
    }

    return targetComponentAccessor;
}

bool mtsMonitorComponent::AddMonitorTarget(SF::cisstMonitor * monitorTarget)
{
    if (!monitorTarget) {
        CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTarget: null monitoring target instance" << std::endl;
        return false;
    }

    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();
    SF::cisstEventLocation * locationID = monitorTarget->GetLocationID();
    if (!locationID) {
        CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTarget: Null monitoring target location" << std::endl;
        return false;
    }

    const std::string thisProcessName = LCM->GetProcessName();
    const std::string targetProcessName = locationID->GetProcessName();

    // Validity check: process name
    // MJ: Components that contain monitoring targets should be in the same process with
    // the monitoring component to make sure the target is monitored.  If not, monitoring
    // mechanisms have to rely on network middlewares is NOT guaranteed due to many network-related issues
    if (thisProcessName.compare(targetProcessName)) {
        CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTarget: target and current process names should match "
            << "(target: \"" << targetProcessName << "\", current: \"" << thisProcessName << "\")" << std::endl;
        return false;
    }

    // Check if the target component exists (within the same process).
    const std::string targetComponentName = locationID->GetComponentName();
    if (!LCM->FindComponent(targetComponentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTarget: no component \"" << targetComponentName << "\" found" << std::endl;
        return false;
    }

    // If monitoring target is of type CUSTOM, check if the target provided interface and command exist.
    // NOTE: currently, required interface, function, and event generators cannot be used
    // as part of monitoring targets.
    const SF::Monitor::TargetType targetType = monitorTarget->GetTargetType();
    const std::string targetPrvIntfName = locationID->GetInterfaceProvidedName();
    const std::string targetCommandName = locationID->GetCommandName();
    if (targetType == SF::Monitor::TARGET_CUSTOM) {
        // check name of provided interface
        if (targetPrvIntfName.empty()) {
            CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTarget: null provided interface name" << std::endl;
            return false;
        }
        // check name of command 
        if (targetCommandName.empty()) {
            CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTarget: null command name: \"" << targetCommandName << "\"" << std::endl;
            return false;
        }
        // check if the provided interface exists
        mtsComponent * component = LCM->GetComponent(targetComponentName);
        CMN_ASSERT(component); // component should exist
        mtsInterfaceProvided * interfaceProvided = component->GetInterfaceProvided(targetPrvIntfName);
        if (!interfaceProvided) {
            CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTarget: no provided interface found: \"" << targetPrvIntfName << "\"" << std::endl;
            return false;
        }
        // check if the command exists
        mtsCommandRead * command = interfaceProvided->GetEndUserInterface(targetCommandName)->GetCommandRead(targetCommandName);
        if (!command) {
            CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTarget: no command \"" << targetCommandName 
                << "\" in provided interface \"" << targetPrvIntfName << "\"" << std::endl;
            return false;
        }
    }

    // Fetch target component information if registered already.  If not, create new instance.
    bool newTargetComponent = false;
    const std::string targetUID(monitorTarget->GetUIDAsString());
    TargetComponentAccessor * targetComponentAccessor = TargetComponentAccessors->GetItem(targetComponentName);
    if (!targetComponentAccessor) {
        newTargetComponent = true;
        // Create required interface to access monitoring target component, but
        // mtsFunctionRead object is NOT created here.
        targetComponentAccessor = CreateTargetComponentAccessor(monitorTarget);
    } 

    // Check duplicate monitor target
    if (targetComponentAccessor->FindMonitorTargetFromAccessor(targetUID)) {
        CMN_LOG_CLASS_RUN_DEBUG << "AddMonitorTarget: skip installing monitor target [ " 
            << monitorTarget->GetUIDAsString() << " ] to component "
            << targetComponentName << ": already being monitored" << std::endl;
        return true;
    }

    // In case of CUSTOM target, create required interface if provided interface is
    // not yet known to the target component accessor.
    if (targetType == SF::Monitor::TARGET_CUSTOM) {
        if (targetComponentAccessor->InterfaceRequiredCustom.find(targetPrvIntfName) 
            == targetComponentAccessor->InterfaceRequiredCustom.end())
        {
            // MJ: this code has overlap with CreateTargetComponentAccessor() method.
            std::string reqIntName("ReqIntfFor[");
            reqIntName += targetPrvIntfName;
            reqIntName += "]";

            mtsInterfaceRequired * required = this->AddInterfaceRequired(reqIntName, MTS_OPTIONAL);
            targetComponentAccessor->InterfaceRequiredCustom[targetPrvIntfName] = required;

            CMN_LOG_CLASS_RUN_DEBUG << "AddMonitorTarget: Created internal required interface "
                << "\"" << reqIntName << "\" for custom monitoring target: " 
                << monitorTarget->GetLocationID()->GetIDString() << std::endl;
        }
    }

    // Add monitor target to component accessor
    CMN_ASSERT(targetComponentAccessor->AddMonitorTargetToAccessor(monitorTarget));

    // Create mtsFunctionRead object
    const std::string targetTypeString = SF::Monitor::GetTargetTypeString(targetType);
    switch (targetType) {
        case SF::Monitor::TARGET_THREAD_PERIOD:
        case SF::Monitor::TARGET_THREAD_DUTYCYCLE_USER:
        case SF::Monitor::TARGET_THREAD_DUTYCYCLE_TOTAL:
            {
                mtsTaskPeriodic * targetTaskPeriodic = dynamic_cast<mtsTaskPeriodic*>(LCM->GetComponent(targetComponentName));
                if (!targetTaskPeriodic) {
                    CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTarget: " << targetTypeString << " is only applicable to periodic task" << std::endl;
                    if (newTargetComponent) delete targetComponentAccessor;
                    return false;
                }
                targetComponentAccessor->AddMonitoringFunction(targetType);

                AddStateVectorForMonitoring(targetTaskPeriodic, monitorTarget);
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

        case SF::Monitor::TARGET_CUSTOM:
            {
                mtsTask * task = LCM->GetComponentAsTask(targetComponentName);
                if (!task) {
                    CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTarget: invalid target component type (component name:"
                                            << "\"" << targetComponentName << "\"" << std::endl;
                    if (newTargetComponent) delete targetComponentAccessor;
                    return false;
                }
                targetComponentAccessor->AddMonitoringFunction(targetType, targetPrvIntfName, targetCommandName);

                // In case of custom targets, the target component accessor directly accesses the 
                // target command and thus no additional state vector is required.
                //AddStateVectorForMonitoring(task, monitorTarget);
            }
            break;

        case SF::Monitor::TARGET_INVALID:
        default:
            CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTarget: invalid or unsupported monitor type: " << std::endl;
            if (newTargetComponent) delete targetComponentAccessor;
            // MJ TODO: implement mtsComponent::RemoveMonitorTarget()
            // targetComponent->RemoveMonitorTarget()
            return false;
            break;
    }

    if (newTargetComponent) {
        if (!TargetComponentAccessors->AddItem(targetComponentName, targetComponentAccessor)) {
            CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTarget: Failed to add state table access interface for component \"" << targetComponentName << "\"" << std::endl;
            return false;
        }
    }

    CMN_LOG_CLASS_RUN_DEBUG << "AddMonitorTarget: Successfully added monitoring target: \n" << *monitorTarget << std::endl;
    //CMN_LOG_CLASS_RUN_DEBUG << "AddMonitorTarget: target component accessor: \n" << *targetComponentAccessor << std::endl;

    return true;
}

bool mtsMonitorComponent::InitializeAccessors(void)
{
    // Connect new monitoring target component to monitor component 
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();

    bool ret = true;
    mtsTask * task = 0; 
    TargetComponentAccessor * accessor = 0;
    std::string clientComponentName, reqIntName;
    std::string serverComponentName, prvIntName;

    TargetComponentAccessorType::const_iterator it = TargetComponentAccessors->begin();
    const TargetComponentAccessorType::const_iterator itEnd = TargetComponentAccessors->end();
    for (; it != itEnd; ++it) {
        accessor = it->second;
        // Establish connections for predefined monitoring targets
        if (accessor->InterfaceRequiredPredefined) {
            task = LCM->GetComponentAsTask(accessor->ComponentName); 
            if (!task) { // [SFUPDATE]
                CMN_LOG_CLASS_RUN_ERROR << "Only task-type components can be monitored: component \"" << accessor->ComponentName << "\"" << std::endl;
            } else {
                clientComponentName = mtsMonitorComponent::GetNameOfMonitorComponent();
                reqIntName = GetNameOfStateTableAccessInterface(accessor->ComponentName);
                serverComponentName = accessor->ComponentName;
                prvIntName = mtsStateTable::GetNameOfStateTableInterface(task->GetMonitoringStateTableName());
                // try to connect internal required interfaces to target provided interface
                // MJ: We don't use 6-parameter version of Connect() because monitor
                // components are assumed to be in the same process as the target components.
                if (!LCM->Connect(clientComponentName, reqIntName, serverComponentName, prvIntName)) {
                    if (!UnregisterComponent(serverComponentName)) // MJTEMP: do i need this?
                        CMN_LOG_CLASS_RUN_ERROR << "Failed to unregister component \"" << serverComponentName << "\" from monitor component" << std::endl;

                    CMN_LOG_CLASS_RUN_ERROR << "Failed to connect component \"" << serverComponentName << "\" to monitor component" << std::endl;
                    ret = false;
                }
            }
        }
        // Establish connections for custom monitoring targets
        if (!accessor->InterfaceRequiredCustom.empty()) {
            // Monitoring component is right now singleton.
            // If multiple monitoring components are deployed, this should be updated as well.
            clientComponentName = mtsMonitorComponent::GetNameOfMonitorComponent();
            serverComponentName = accessor->ComponentName;

            TargetComponentAccessor::InterfaceRequiredCustomType::const_iterator it2 = accessor->InterfaceRequiredCustom.begin();
            const TargetComponentAccessor::InterfaceRequiredCustomType::const_iterator it2End = accessor->InterfaceRequiredCustom.end();
            for (; it2 != it2End; ++it2) {
                prvIntName = it2->first;
                reqIntName = it2->second->GetName();
                // try to connect internal required interfaces to target provided interface
                if (!LCM->Connect(clientComponentName, reqIntName, serverComponentName, prvIntName)) {
                    if (!UnregisterComponent(serverComponentName)) // MJTEMP: do i need this?
                        CMN_LOG_CLASS_RUN_ERROR << "Failed to unregister component \"" << serverComponentName << "\" from monitor component" << std::endl;

                    CMN_LOG_CLASS_RUN_ERROR << "Failed to connect component \"" << serverComponentName << "\" to monitor component" << std::endl;
                    ret = false;
                }
            }
        }
    }

    return ret;
}

void mtsMonitorComponent::RunMonitors(void)
{
    const double currentTick = (ManualAdvance ? osaGetTime() : this->GetTick());

    TargetComponentAccessorType::const_iterator it = TargetComponentAccessors->begin();
    const TargetComponentAccessorType::const_iterator itEnd = TargetComponentAccessors->end();

    if (ManualAdvance) {
        bool advance = false;
        for (; it != itEnd; ++it)
            advance |= it->second->RefreshSamples(currentTick, Publisher);
        // Do filtering and run all FDD pipelines (MJ: this can be further optimized such that
        // only updated samples are processed).
        if (advance)
            StateTableMonitor.Advance();
    } else {
        for (; it != itEnd; ++it)
            it->second->RefreshSamples(currentTick, Publisher);
    }
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

void mtsMonitorComponent::AddStateVectorForMonitoring(mtsTaskPeriodic * targetTaskPeriodic, SF::cisstMonitor * monitor)
{
    const std::string taskName = targetTaskPeriodic->GetName();
    std::string newElementName(taskName);

    // MJ TEMP: Adding a new element (column vector) to state table on the fly may be not thread safe.
    switch (monitor->GetTargetType()) {
        case SF::Monitor::TARGET_THREAD_PERIOD:
            // Add "Period" to the monitoring state table of this component with the name of
            // (component name)+"Period"
            newElementName += "PeriodActual";
            this->StateTableMonitor.NewElement(newElementName, &monitor->Samples.PeriodActual);
            // Set nominal period value
            monitor->Samples.PeriodNominal = targetTaskPeriodic->GetPeriodicity(true);
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

        case SF::Monitor::TARGET_CUSTOM:
            //newElementName = monitor->GetUIDAsString();
            //this->StateTableMonitor.NewElement(newElementName, &monitor->Samples.ExecTimeTotal);
            // Custom monitoring target doesn't need
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
                                         10, 1);
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
    Publisher->PublishData(SF::Topic::Data::MONITOR, json);

    // MJ TODO: Depending on the type of event (esp. in case of fault events),
    // the Safety Coordinator in each process can deal with events or faults locally,
    // i.e., within the process boundary.
    // This would be the best-performance-fault-handling case but it loses the
    // system-wide fault (event) propagation and global coordination by the "brain."

    // TODO: This could be a place to do interesting things!
}

void mtsMonitorComponent::HandleFaultEvent(const std::string & json)
{
    // If the monitor component receives an event regardless of its type (monitor or
    // fault), publish the event to the safety framework.  The Safety Supervisor
    // will handle the event "accordingly" (TODO).
    Publisher->PublishData(SF::Topic::Data::EVENT, json);

    // Report event to Safety Coordinator
    mtsManagerLocal::GetInstance()->GetCoordinator()->OnFaultEvent(json);
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
const std::string  mtsMonitorComponent::GetNameOfStateTableAccessInterface(const std::string & taskName) {
    return "Monitor" + taskName;
}
const std::string & mtsMonitorComponent::GetNameOfMonitorComponent(void) {
    return NameOfMonitorComponent;
}

mtsMonitorComponent::TargetComponentAccessor * mtsMonitorComponent::GetTargetComponentAccessor(const std::string & targetComponentName)
{
    return TargetComponentAccessors->GetItem(targetComponentName);
}
