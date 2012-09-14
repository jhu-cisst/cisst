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

mtsSafetyCoordinator::mtsSafetyCoordinator()
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

bool mtsSafetyCoordinator::AddMonitor(SF::Monitor * baseMonitor)
{
    if (!baseMonitor) {
        CMN_LOG_CLASS_RUN_ERROR << "NULL cisstMonitor instance error" << std::endl;
        return false;
    }

    SF::cisstMonitor * monitor = dynamic_cast<SF::cisstMonitor*>(baseMonitor);
    CMN_ASSERT(monitor);
    SF::cisstEventLocation * locationID = dynamic_cast<SF::cisstEventLocation*>(monitor->GetLocationID());
    CMN_ASSERT(locationID);

    const std::string targetUID = monitor->GetUIDAsString();
    const std::string monitorInJson = monitor->GetMonitorJSON();

    // Check if same monitoring target is already registered
    if (this->IsDuplicateUID(targetUID)) {
        CMN_LOG_CLASS_RUN_ERROR << "Target is already being monitored: " << targetUID << std::endl;
        return false;
    }

    // Check if json syntax is valid
    SF::JSON json;
    if (!json.Read(monitorInJson.c_str())) {
        CMN_LOG_CLASS_RUN_ERROR << "Failed to parse json for monitor target: " << targetUID
            << "\nJSON: " << monitorInJson << std::endl;
        return false;
    }

    // Parse json and extract information of interest
    /*
    SF::cisstMonitor newMonitorTarget;
    if (!ParseJSON(json, newMonitorTarget)) {
        CMN_LOG_CLASS_RUN_ERROR << "Failed to extract information from json: " << targetUID
            << "\nJSON: " << monitorInJson << std::endl;
        return false;
    }
    */
    
    // Monitor cannot monitor itself
    const std::string targetComponentName = locationID->GetComponentName();
    if (targetComponentName.compare(mtsMonitorComponent::GetNameOfMonitorComponent()) == 0) {
        CMN_LOG_CLASS_RUN_ERROR << "Monitor cannot monitor itself: " << locationID->GetComponentName() << std::endl;
        return false;
    }

    // Add new monitor target to monitor 
    // [SFUPDATE] Use single monitor instance per process.  Should more than one
    // monitoring component be deployed within the same process, comments of
    //
    // bool mtsSafetyCoordinator::AddFilter(SF::FilterBase * filter)
    // 
    // needs to be reviewed.
    mtsMonitorComponent * monitorComponent = Monitors[0];
    if (!monitorComponent->AddMonitorTarget(monitor)) {
        CMN_LOG_CLASS_RUN_ERROR << "Failed to add monitor target to monitor component: " << targetUID << std::endl;
        return false;
    }

    CMN_LOG_CLASS_RUN_DEBUG << "AddMonitor: successfully added monitor target: " << targetUID 
        << "\nJSON: " << json.GetJSON() << std::endl;

    this->MonitorTargetMap[targetUID] = monitorInJson;

    return true;
}

bool mtsSafetyCoordinator::AddFilter(SF::FilterBase * filter)
{
    if (!filter) {
        CMN_LOG_CLASS_RUN_ERROR << "AddFilter: NULL filter error" << std::endl;
        return false;
    }

    // Collect arguments
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
    mtsHistoryBuffer * historyBuffer;
    std::string signalName;

    // Active monitoring: filter is run by the target component
    if (filteringType == SF::FilterBase::ACTIVE) {
        // Instantiate history buffer for each input signal
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
            mtsStateTable * stateTable = targetTask->GetDefaultStateTable();
            CMN_ASSERT(stateTable);

            int stateVectorId = stateTable->GetStateVectorID(signalName);
            if (stateVectorId != mtsStateTable::INVALID_STATEVECTOR_ID) {
                historyBuffer = new mtsHistoryBuffer(stateTable);
                signal->SetHistoryBufferInstance(historyBuffer);
                // associate output signal with state vector
                signal->SetHistoryBufferIndex(stateVectorId);
            } else {
                stateTable = targetTask->GetMonitoringStateTable();
                stateVectorId = stateTable->GetStateVectorID(signalName);
                if (stateVectorId != mtsStateTable::INVALID_STATEVECTOR_ID) {
                    historyBuffer = new mtsHistoryBuffer(stateTable);
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
        const size_t totalOutputCount = filter->GetNumberOfOutputs();
        for (size_t i = 0; i < totalOutputCount; ++i) {
            signal = filter->GetOutputSignalElement(i);
            CMN_ASSERT(signal);
            signalName = signal->GetName();

            // All output signals are added to the monitoring state table.
            mtsStateTable * stateTable = targetTask->GetMonitoringStateTable();
            CMN_ASSERT(stateTable);
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

        // Register filter to the state table.  Active filters are run by target component.
        mtsStateTable * stateTable = targetTask->GetMonitoringStateTable();
        stateTable->RegisterFilter(filter);
    }
    // Passive monitoring: filter is run by the monitoring component
    else {
        CMN_ASSERT(false); // MJ TODO: implement later
        //historyBuffer = new mtsHistoryBuffer(Monitors[0]->GetMonitoringStateTable());
    }

    // If this filter is the last one of filtering pipeline, install a new monitor
    // instance to publish events or faults detected by this filter.
    if (filter->IsLastFilterOfPipeline()) {
        // Define target
        SF::cisstEventLocation * locationID = new SF::cisstEventLocation;
        locationID->SetProcessName(mtsManagerLocal::GetInstance()->GetProcessName());
        locationID->SetComponentName(filter->GetNameOfTargetComponent());

        SF::cisstMonitor * monitor = new SF::cisstMonitor(SF::Monitor::TARGET_FILTER_EVENT,
                                                          locationID,
                                                          SF::Monitor::STATE_ON,
                                                          SF::Monitor::OUTPUT_EVENT);
        if (!AddMonitor(monitor)) {
            CMN_LOG_CLASS_RUN_ERROR << "AddFilter: Failed to install new monitor. Filter: [" << *filter << "], Monitor: [" << *monitor << "]" << std::endl;
            // MJ TODO: undo signal installation above
            delete monitor;
            return false;
        }

        // Create new instance of mtsEventPublisher and install it on this filter.
        // MJ TODO: Then, the mtsFunction inside the mtsEventPublisher should be added
        // to the internal monitoring required interface (so that the command can use
        // mts services for fault report).
        mtsEventPublisher * eventPublisher = new mtsEventPublisher;
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
    // MJ: For now, keep monitor component only one that monitor all components in the
    // same process.  More monitor components can be dynamically deployed later
    // considering run-time overhead of fault detection and diagnosis methods.
    if (!Monitors.empty()) {
        CMN_LOG_CLASS_RUN_WARNING << "Monitor was already intialized" << std::endl;
        return true;
    }

    mtsMonitorComponent * monitor = new mtsMonitorComponent;
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
    // MJ: Nothing to print out for now
    //outputStream << " Process: \""        << this->Process << "\","
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

