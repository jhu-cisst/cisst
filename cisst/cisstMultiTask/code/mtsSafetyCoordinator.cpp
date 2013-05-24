/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsSafetyCoordinator.cpp 3303 2012-01-02 16:33:23Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2012-07-14

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#include <cisstMultiTask/mtsSafetyCoordinator.h>
#include <cisstMultiTask/mtsMonitorComponent.h>
#include <cisstMultiTask/mtsHistoryBuffer.h>
#include <cisstMultiTask/mtsEventPublisher.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

#include "dict.h"
#include "signal.h"

using namespace SF::Dict::Json;

CMN_IMPLEMENT_SERVICES(mtsSafetyCoordinator);

mtsSafetyCoordinator::mtsSafetyCoordinator() : SF::Coordinator()
{
}

mtsSafetyCoordinator::~mtsSafetyCoordinator()
{
    if (!Monitors.empty()) {
        for (size_t i = 0; i < Monitors.size(); ++i)
            delete Monitors[i];
    }

    if (!FilterSet.empty()) {
        for (FilterSetType::iterator it = FilterSet.begin();
             it != FilterSet.end(); ++it) 
        {
            FiltersType * filters = it->second;
            if (!filters->empty()) {
                for (FiltersType::iterator _it = filters->begin();
                     _it != filters->end(); ++_it)
                {
                    delete (*_it);
                }
            }
            delete filters;
        }
    }
}

bool mtsSafetyCoordinator::DeployMonitorTarget(const std::string & targetJSON, 
                                               SF::cisstMonitor * targetInstance)
{
    if (!targetInstance) {
        CMN_LOG_CLASS_RUN_ERROR << "Null cisstMonitorTargetInstance" << std::endl;
        return false;
    }

    if (!targetInstance->GetLocationID()) {
        CMN_LOG_CLASS_RUN_ERROR << "Null monitoring target location instance" << std::endl;
        return false;
    }

    // Check if json syntax is valid
    SF::JSON json;
    if (!json.Read(targetJSON.c_str())) {
        CMN_LOG_CLASS_RUN_ERROR << "Failed to parse json for monitor target: [ " << targetJSON << " ]" << std::endl;
        return false;
    }
    
    // Monitor cannot monitor itself
    const std::string targetComponentName = targetInstance->GetLocationID()->GetComponentName();
    if (targetComponentName.compare(mtsMonitorComponent::GetNameOfMonitorComponent()) == 0) {
        CMN_LOG_CLASS_RUN_ERROR << "Monitor cannot monitor itself: " 
            << targetInstance->GetLocationID()->GetComponentName() << std::endl;
        return false;
    }

    // Add new monitor target to monitor 
    // [SFUPDATE] Use single monitor instance per process.  Should more than one
    // monitoring component be deployed within the same process, review the comments of
    // bool mtsSafetyCoordinator::AddFilter(SF::FilterBase * filter)
    const std::string targetUID = targetInstance->GetUIDAsString();
    mtsMonitorComponent * monitorComponent = Monitors[0];
    if (!monitorComponent->AddMonitorTarget(targetInstance)) {
        CMN_LOG_CLASS_RUN_ERROR << "Failed to add monitor target to monitor component: " << targetUID << std::endl;
        return false;
    }

    if (!this->AddMonitoringTarget(targetUID, targetJSON)) {
        CMN_LOG_CLASS_RUN_DEBUG << "Failed to add monitor target to coordinator: " << targetUID << std::endl;
        return false;
    }

    CMN_LOG_CLASS_RUN_DEBUG << "Successfully deployed monitoring target [ " << targetJSON << " ]" << std::endl;

    return true;
}

bool mtsSafetyCoordinator::AddMonitorTarget(SF::cisstMonitor * cisstMonitorTarget)
{
    if (!cisstMonitorTarget) {
        CMN_LOG_CLASS_RUN_ERROR << "Null cisst monitoring target" << std::endl;
        return false;
    }

    SF::cisstEventLocation * locationID = cisstMonitorTarget->GetLocationID();
    if (!locationID) {
        CMN_LOG_CLASS_RUN_ERROR << "Null cisst monitoring target location" << std::endl;
        return false;
    }

    // Deploy monitoring target to monitor component
    const std::string targetJSON = cisstMonitorTarget->GetMonitorJSON();
    if (!DeployMonitorTarget(targetJSON, cisstMonitorTarget)) {
        CMN_LOG_CLASS_RUN_ERROR << "Failed to deploy monitoring target [ " << targetJSON << " ]" << std::endl;
        return false;
    }

    CMN_LOG_CLASS_RUN_DEBUG << "Successfully added monitoring target using monitoring target instance" << std::endl;
    CMN_LOG_CLASS_RUN_DEBUG << *this << std::endl;

    return true;
}

bool mtsSafetyCoordinator::AddMonitorTarget(const std::string & jsonFileName)
{
    // Construct JSON structure from file
    SF::JSON json;
    if (!json.ReadFromFile(jsonFileName)) {
        CMN_LOG_CLASS_RUN_ERROR << "Failed to read json file: " << jsonFileName << std::endl;
        return false;
    }

    const SF::JSON::JSONVALUE targets = json.GetRoot()[SF::Dict::Json::monitor];
    if (targets.isNull() || targets.size() == 0) {
        CMN_LOG_CLASS_RUN_ERROR << "No monitor specification found in json file: " << jsonFileName << std::endl;
        return false;
    }

    // Create and install monitor instances, iterating each monitor specification
    for (size_t i = 0; i < targets.size(); ++i) {
        // Create monitoring target instance
        SF::cisstMonitor * cisstMonitorTarget = new SF::cisstMonitor(targets[i]);

        // Check if same monitoring target is already registered
        const std::string targetUID = cisstMonitorTarget->GetUIDAsString();
        if (this->FindMonitoringTarget(targetUID)) {
            CMN_LOG_CLASS_RUN_ERROR << "Target is already being monitored: " << targetUID << std::endl;
            return false;
        }

        // Deploy monitoring target to monitor component
        const std::string targetJSON = cisstMonitorTarget->GetMonitorJSON();
        if (!DeployMonitorTarget(targetJSON, cisstMonitorTarget)) {
            CMN_LOG_CLASS_RUN_ERROR << "Failed to deploy monitoring target [ " << targetJSON << " ]" << std::endl;
            return false;
        }
    }

    CMN_LOG_CLASS_RUN_DEBUG << "Successfully added monitoring target(s) from json file: " << jsonFileName << std::endl;
    CMN_LOG_CLASS_RUN_DEBUG << *this << std::endl;

    return true;
}

bool mtsSafetyCoordinator::AddFilter(SF::FilterBase * filter)
{
    if (!filter) {
        CMN_LOG_CLASS_RUN_ERROR << "AddFilter: NULL filter error" << std::endl;
        return false;
    }

    // Collect arguments
    const std::string targetProcessName = mtsManagerLocal::GetInstance()->GetName();
    const std::string targetComponentName(filter->GetNameOfTargetComponent());
    const SF::FilterBase::FilteringType filteringType = filter->GetFilteringType();

    // Check if target component of type task exists
    mtsTask * targetTask = mtsManagerLocal::GetInstance()->GetComponentAsTask(targetComponentName);
    if (!targetTask) {
        CMN_LOG_CLASS_RUN_ERROR << "AddFilter: no task-type component \"" << targetComponentName << "\" found" << std::endl;
        return false;
    }

    // Install filter
    SF::SignalElement * signal;
    mtsHistoryBuffer * historyBuffer = 0;
    std::string signalName;

    // Active filtering: filter is run by the target component
    if (filteringType == SF::FilterBase::ACTIVE) {
        // Instantiate history buffer for each input signal
        mtsStateTable * stateTable = targetTask->GetDefaultStateTable();
        CMN_ASSERT(stateTable);
        const size_t totalInputCount = filter->GetNumberOfInputs();
        for (size_t i = 0; i < totalInputCount; ++i) {
            signal = filter->GetInputSignalElement(i);
            CMN_ASSERT(signal);
            signalName = signal->GetName();

            // Each component has two state tables -- the default state table 
            // and the monitoring state table (if the SF plug-in option enabled).
            // Because input signals can be either raw signals from other component
            // (which is managed by the default one) or output of other filters
            // (which the monitoring state table maintains), we first
            // figure out which state table has the input signal being added.
            int stateVectorId = stateTable->GetStateVectorID(signalName);
            if (stateVectorId != mtsStateTable::INVALID_STATEVECTOR_ID) {
                historyBuffer = new mtsHistoryBuffer(SF::FilterBase::ACTIVE, stateTable);
                signal->SetHistoryBufferInstance(historyBuffer);
                // associate output signal with state vector
                signal->SetHistoryBufferIndex(stateVectorId);
            } else {
                stateTable = targetTask->GetMonitoringStateTable();
                stateVectorId = stateTable->GetStateVectorID(signalName);
                if (stateVectorId != mtsStateTable::INVALID_STATEVECTOR_ID) {
                    historyBuffer = new mtsHistoryBuffer(SF::FilterBase::ACTIVE, stateTable);
                    signal->SetHistoryBufferInstance(historyBuffer);
                    // associate output signal with state vector
                    signal->SetHistoryBufferIndex(stateVectorId);
                } else {
                    CMN_LOG_CLASS_RUN_ERROR << "AddFilter: no input signal \"" << signalName << "\" found" << std::endl;
                    return false;
                }
            }
        }
        // Instantiate history buffer for each output signal
        stateTable = targetTask->GetMonitoringStateTable();
        CMN_ASSERT(stateTable);
        const size_t totalOutputCount = filter->GetNumberOfOutputs();
        for (size_t i = 0; i < totalOutputCount; ++i) {
            signal = filter->GetOutputSignalElement(i);
            CMN_ASSERT(signal);
            signalName = signal->GetName();

            // All output signals are added to the monitoring state table.
            SF::SignalElement::SignalType outputSignalType = signal->GetSignalType();
            // Create new state vector and add it to the monitoring state table.
            // MJ: adding a new element on the fly may not be thread-safe (?)
            mtsStateDataId id;
            if (outputSignalType == SF::SignalElement::SCALAR) {
                id = stateTable->NewElement(signalName, signal->GetPlaceholderScalarPointer());
            } else {
                id = stateTable->NewElement(signalName, signal->GetPlaceholderVectorPointer());
            }
            // associate output signal with state vector
            id = stateTable->GetStateVectorID(signalName);
            CMN_ASSERT(id != mtsStateTable::INVALID_STATEVECTOR_ID);
            signal->SetHistoryBufferInstance(historyBuffer);
            signal->SetHistoryBufferIndex(id);
        }

        // Associate filter with the monitoring state table of the target component.
        // Active filters are run by target component.
        stateTable->RegisterFilter(filter);
    }
    // Passive filtering: filter is run by the monitoring component
    else {
        // Get target component accessor
        mtsMonitorComponent::TargetComponentAccessor * accessor
            = Monitors[0]->GetTargetComponentAccessor(targetComponentName);
        if (!accessor) {
            // If target component accessor is null, create new accessor and add it
            // to the monitoring component.
            accessor = Monitors[0]->CreateTargetComponentAccessor(
                targetProcessName, targetComponentName, 
                false, // this is not active filter
                true); // add accessor to internal data structure
        }

        // Instantiate history buffer for each input signal
        mtsStateTable * stateTable = Monitors[0]->GetMonitoringStateTable();
        CMN_ASSERT(stateTable);
        const size_t totalInputCount = filter->GetNumberOfInputs();
        for (size_t i = 0; i < totalInputCount; ++i) {
            signal = filter->GetInputSignalElement(i);
            CMN_ASSERT(signal);
            signalName = signal->GetName();

            // TODO: Establish mts connection between monitor::required interface and
            // target component's state table::provided interface

            // Check if a read-from-statetable command that the input signal uses
            // has been registered to the default state table of the target component.
            // If not, the input signal cannot fetch new values from the state table and
            // thus always have zero value(s).
            // If yes, create new historyBuffer instance that contains read functions, and
            // add the read functions to the required interface of the target component 
            // accessor.

            // Get the name of provided interface of the monitoring state table
            const std::string providedInterfaceName
                = mtsStateTable::GetNameOfStateTableInterface(targetTask->GetMonitoringStateTableName());
            // Get the instance of the provided interface
            mtsInterfaceProvided * provided = targetTask->GetInterfaceProvided(providedInterfaceName);
            CMN_ASSERT(provided);

            // Set the name of the read-from-statetable command
            std::string commandName("Get");
            commandName += signalName;
            // Add read-from-statetable command for each input signal
            if (signal->GetSignalType() == SF::SignalElement::SCALAR) {
                provided->AddCommandReadStateInternalScalar(targetTask->StateTable, signalName, commandName);
            } else {
                provided->AddCommandReadStateInternalVector(targetTask->StateTable, signalName, commandName);
            }

            // Get the name of required interface of the target component accessor
            const std::string requiredInterfaceName
                = mtsMonitorComponent::GetNameOfStateTableAccessInterface(targetComponentName);
            // Get the instance of the required interface 
            mtsInterfaceRequired * required = Monitors[0]->GetInterfaceRequired(requiredInterfaceName);
            CMN_ASSERT(required);

            historyBuffer = new mtsHistoryBuffer(SF::FilterBase::PASSIVE, stateTable);
            signal->SetHistoryBufferInstance(historyBuffer);

            // Add functions to fetch data from the monitoring state table of the target component
            // using read-from-statetable commands added above.
            if (signal->GetSignalType() == SF::SignalElement::SCALAR) {
                required->AddFunction(commandName, historyBuffer->FetchScalarValue);
            } else {
                required->AddFunction(commandName, historyBuffer->FetchVectorValue);
            }
        }

        // Instantiate history buffer for each output signal
        const size_t totalOutputCount = filter->GetNumberOfOutputs();
        for (size_t i = 0; i < totalOutputCount; ++i) {
            signal = filter->GetOutputSignalElement(i);
            CMN_ASSERT(signal);
            signalName = signal->GetName();

            // The monitoring state table of the monitor component maintains all output signals
            SF::SignalElement::SignalType outputSignalType = signal->GetSignalType();
            // Create new state vector and add it to the monitoring state table.
            // MJ: adding a new element on the fly may not be thread-safe (?)
            mtsStateDataId id;
            if (outputSignalType == SF::SignalElement::SCALAR) {
                id = stateTable->NewElement(signalName, signal->GetPlaceholderScalarPointer());
            } else {
                id = stateTable->NewElement(signalName, signal->GetPlaceholderVectorPointer());
            }
            // associate output signal with state vector
            id = stateTable->GetStateVectorID(signalName);
            CMN_ASSERT(id != mtsStateTable::INVALID_STATEVECTOR_ID);
            historyBuffer = new mtsHistoryBuffer(SF::FilterBase::PASSIVE, stateTable);
            signal->SetHistoryBufferInstance(historyBuffer);
            signal->SetHistoryBufferIndex(id);
        }

        // Associate filter with the monitoring state table of the monitoring component 
        // Passive filters are run by the Monitor component.
        stateTable->RegisterFilter(filter);
    }

    // If this filter is the last one of filtering pipeline, install a new monitor
    // instance to publish events or faults detected by this filter.
    if (filter->IsLastFilterOfPipeline()) {
        // Active filtering: A new instance of monitor to generate and propagate fault events
        // is created and is attached to the provided interface for the monitoring state table 
        // of the target component.
        // When a fault occurs, a filter generates a fault event by calling the mts function
        // of the mtsEventPublisher associated with the filter, and the fault event is
        // propagated to one of target component accessors of the monitoring component.
        // Then, the fault event is handled by mtsMonitorComponent::HandleFaultEvent() which
        // propagates the event to the Safety Supervisor.
        if (filteringType == SF::FilterBase::ACTIVE) {
            // Define target
            SF::cisstEventLocation * locationID = new SF::cisstEventLocation;
            locationID->SetProcessName(targetProcessName);
            locationID->SetComponentName(targetComponentName);

            SF::cisstMonitor * monitor = new SF::cisstMonitor(SF::Monitor::TARGET_FILTER_EVENT,
                                                              locationID,
                                                              SF::Monitor::STATE_ON,
                                                              SF::Monitor::OUTPUT_EVENT);
            // Install fault event handler to target component accessor of the monitor component
            monitor->SetAttachedToActiveFilter(true);

            if (!AddMonitorTarget(monitor)) {
                CMN_LOG_CLASS_RUN_ERROR << "AddFilter: Failed to install new monitor. Filter: [" << *filter << "], Monitor: [" << *monitor << "]" << std::endl;
                // MJ TODO: undo signal installation above
                delete monitor;
                return false;
            }


            // Create new instance of mtsEventPublisher and install it on this filter.
            // MJ TODO: Then, the mtsFunction of mtsEventPublisher should be added
            // to the internal monitoring required interface (so that the command can use
            // mts-communication for fault report).
            mtsEventPublisher * eventPublisher = new mtsEventPublisher(SF::FilterBase::ACTIVE);
            filter->SetEventPublisherInstance(eventPublisher);

            // Add event generator of the event publisher to the target component's internal
            // required interface for monitoring.
            // First, get target component.
            mtsComponent * targetComponent = mtsManagerLocal::GetInstance()->GetComponent(targetComponentName);
            CMN_ASSERT(targetComponent);
            // Then, get provided interface instance of the target component, which the
            // monitoring mechanism uses.
            mtsInterfaceProvided * provided = targetComponent->GetInterfaceProvided(
                 mtsStateTable::GetNameOfStateTableInterface(mtsStateTable::NameOfStateTableForMonitoring));
            CMN_ASSERT(provided);
            // Register event generator to the required interface so that filter can publish
            // events or faults through the monitoring mechanism.
            provided->AddEventWrite(eventPublisher->GetEventPublisherFunction(),
                                    SF::Dict::FaultNames::FaultEvent, std::string());
            // MJ: DESIGN REVIEW:  For now, each process has a single instance of
            // mtsMonitorComponent and thus having an event publisher as an event generator
            // of the monitoring provided interface of the target component is fine.
            // Should two or more monitoring components be introduced in the same process,
            // however, this becomes problematic because a single fault event generation 
            // would result in multiple instances of the fault and the Safety Supervisor will 
            // receive multiple reports for the same fault (because of broadcasting mechanism
            // of mtsInterfaceProvided).
            // A quick solution to avoid this issue is to set up another internal required 
            // interface at the target component, to have another internal provided interface 
            // at the monitoring component, and to use this connection.
        }
        // Passive filtering: The fault event generation and propagation mechanism of
        // passive filters is relatively simpler than that of active filters because
        // passive filters are run by the monitoring component and thus fault events are
        // detected and generated by the same thread space, i.e., the monitoring
        // component's thread.  Thus, if a fault is detected, the monitor component simply
        // pass the fault to mtsMonitorComponent::HandleFaultEvent() which then propagates
        // the event to the Safety Supervisor.
        else {
            mtsEventPublisher * eventPublisher = new mtsEventPublisher(SF::FilterBase::PASSIVE);
            filter->SetEventPublisherInstance(eventPublisher);

            // Register the instance of the monitoring component to the event publisher so
            // that the publisher can publish events or faults.  Events or faults
            // detectors, i.e., passive filters, and publishers, i.e., an instance of mtsEventPublisher,
            // use the same thread space.
            // MJ TODO: better way than setting instance of monitor component??
            eventPublisher->SetMonitorComponentInstance(Monitors[0]);

            // Define target
            SF::cisstEventLocation * locationID = new SF::cisstEventLocation;
            locationID->SetProcessName(targetProcessName);
            locationID->SetComponentName(targetComponentName);

            SF::cisstMonitor * monitor = new SF::cisstMonitor(SF::Monitor::TARGET_FILTER_EVENT,
                                                              locationID,
                                                              SF::Monitor::STATE_ON,
                                                              SF::Monitor::OUTPUT_EVENT);
            if (!AddMonitorTarget(monitor)) {
                CMN_LOG_CLASS_RUN_ERROR << "AddFilter: Failed to install new monitor. Filter: [" << *filter << "], Monitor: [" << *monitor << "]" << std::endl;
                // MJ TODO: undo signal installation above
                delete monitor;
                return false;
            }
        }
    }

    // Add filter to the list of filters deployed
    FilterSetType::iterator it = FilterSet.find(targetComponentName);
    // First time to deploy a filter to the target component
    if (it == FilterSet.end()) {
        FiltersType * filterList = new FiltersType;
        filterList->push_back(filter);
        CMN_LOG_CLASS_RUN_DEBUG << "AddFilter: Installed new filter to component \"" << targetComponentName << "\" for the first time.  Filter info: " << (*filter) << std::endl;
    } else {
        it->second->push_back(filter);
        CMN_LOG_CLASS_RUN_DEBUG << "AddFilter: Installed new filter to component \"" << targetComponentName << "\".  Filter info: " << (*filter) << std::endl;
    }

    // Set other properties of filter
    // MJ: Should more fields be required for event localization, middleware-specific class
    // which is derived from the base class can be defined.
    SF::cisstEventLocation * location = new SF::cisstEventLocation;
    location->SetProcessName(mtsManagerLocal::GetInstance()->GetName());
    location->SetComponentName(targetComponentName);
    filter->SetEventLocationInstance(location);

    filter->Enable();

    return true;
}

bool mtsSafetyCoordinator::DeployMonitorsAndFDDs(void)
{
    for (size_t i = 0; i < Monitors.size(); ++i) {
        if (!Monitors[i]->InitializeAccessors()) {
            CMN_LOG_CLASS_RUN_ERROR << "DeployMonitorsAndFDDs: failed to initialize accessors in monitor" << std::endl;
            return false;
        }
    }

    return true;
}

bool mtsSafetyCoordinator::CreateMonitor(void)
{
    // MJ: For now, only one monitor instance is used to monitor all the components 
    // in the same process.  More monitor components can be deployed later considering 
    // the run-time overhead of fault detection and diagnosis methods.
    if (!Monitors.empty()) {
        CMN_LOG_CLASS_RUN_WARNING << "Monitor was already intialized" << std::endl;
        return true;
    }

    // MJTODO:
    // Because mtsMonitorComponent has two different configurations depending on
    // mtsMonitorComponent::ManualAdvance, there has to be at least two monitoring
    // components with manual and automatic advance, for monitoring and filtering,
    // respectively.  Refer to mtsMonitorComponent's constructors for further comments.
 
    // Create monitoring component with manual state table advance.  This monitor component
    // is used only for monitoring, not filtering (filtering requires automatic advance).
    mtsMonitorComponent * monitor = new mtsMonitorComponent(false);
    mtsManagerLocal * componentManager = mtsManagerLocal::GetInstance(); 
    if (!componentManager->AddComponent(monitor)) {
        CMN_LOG_CLASS_RUN_ERROR << "Failed to create monitor" << std::endl;
        delete monitor;
        return false;
    }

    Monitors.push_back(monitor);

    CMN_LOG_CLASS_RUN_DEBUG << "CreateMonitor: created monitor instance (total instance count: " << Monitors.size() << ")" << std::endl;

    return true;
}

void mtsSafetyCoordinator::ToStream(std::ostream & outputStream) const
{
    mtsGenericObject::ToStream(outputStream);
    this->PrintMonitoringTargets(outputStream);
}

void mtsSafetyCoordinator::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);
    // MJ: Nothing to serialize for now
    //cmnSerializeRaw(outputStream, Process);
}

void mtsSafetyCoordinator::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);
    // MJ: Nothing to deserialize for now
    //cmnDeSerializeRaw(inputStream, Process);
}

