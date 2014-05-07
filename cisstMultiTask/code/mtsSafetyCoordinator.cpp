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

#include <cisstMultiTask/mtsGenericObjectProxy.h>
#include <cisstMultiTask/mtsSafetyCoordinator.h>
#include <cisstMultiTask/mtsMonitorComponent.h>
#include <cisstMultiTask/mtsHistoryBuffer.h>
#include <cisstMultiTask/mtsEventPublisher.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

#include "dict.h"
#include "utils.h"
#include "signal.h"
#include "jsonSerializer.h"
#include "filters/threshold.h"
#include "filterFactory.h"

//! Const to set the decimal precision to format floating-point values
//static const int PRECISION = 9;

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

    if (!Filters.empty())
        for (FiltersType::iterator it = Filters.begin(); it != Filters.end(); ++it)
            delete it->second;
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
    //      bool mtsSafetyCoordinator::AddFilter(SF::FilterBase * filter)
    // Also, update the following important method as well:
    //      bool mtsMonitorComponent::InitializeAccessors(void)
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

bool mtsSafetyCoordinator::AddMonitorTarget(const SF::JSON::JSONVALUE & targets)
{
    if (targets.isNull() || targets.size() == 0) {
        CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTarget: No monitor specification found in json: " << targets << std::endl;
        return false;
    }

    // Create and install monitor instances, iterating each monitor specification
    for (size_t i = 0; i < targets.size(); ++i) {
        // Create monitoring target instance
        SF::cisstMonitor * cisstMonitorTarget = new SF::cisstMonitor(targets[i]);

        // Check if same monitoring target is already registered
        const std::string targetUID = cisstMonitorTarget->GetUIDAsString();
        if (this->FindMonitoringTarget(targetUID)) {
            CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTarget: Target is already being monitored: " << targetUID << std::endl;
            return false;
        }

        // Deploy monitoring target to monitor component
        const std::string targetJSON = cisstMonitorTarget->GetMonitorJSON();
        if (!DeployMonitorTarget(targetJSON, cisstMonitorTarget)) {
            CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTarget: Failed to deploy monitoring target [ " << targetJSON << " ]" << std::endl;
            return false;
        }
    }

    return true;
}

bool mtsSafetyCoordinator::AddMonitorTargetFromJSON(const std::string & jsonString)
{
    // Construct JSON structure from JSON string
    SF::JSON json;
    if (!json.Read(jsonString.c_str())) {
        CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTargetFromJSON: Failed to read json string: " << jsonString << std::endl;
        return false;
    }

    const SF::JSON::JSONVALUE targets = json.GetRoot()[SF::Dict::Json::monitor];
    bool ret = AddMonitorTarget(targets);

    if (ret) {
        CMN_LOG_CLASS_RUN_DEBUG << "AddMonitorTargetFromJSON: Successfully added monitoring target(s) using json string: " << jsonString << std::endl;
        CMN_LOG_CLASS_RUN_DEBUG << *this << std::endl;
    } else {
        CMN_LOG_CLASS_RUN_DEBUG << "AddMonitorTargetFromJSON: Failed to add monitoring target(s) using json string: " << jsonString << std::endl;
    }

    return ret;
}

bool mtsSafetyCoordinator::AddMonitorTargetFromJSONFile(const std::string & jsonFileName)
{
    // Construct JSON structure from JSON file
    SF::JSON json;
    if (!json.ReadFromFile(jsonFileName)) {
        CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTargetFromJSONFile: Failed to read json file: " << jsonFileName << std::endl;
        return false;
    }

    const SF::JSON::JSONVALUE targets = json.GetRoot()[SF::Dict::Json::monitor];
    bool ret = AddMonitorTarget(targets);

    if (ret) {
        CMN_LOG_CLASS_RUN_DEBUG << "AddMonitorTargetFromJSONFile: Successfully added monitoring target(s) from JSON file: " << jsonFileName << std::endl;
        CMN_LOG_CLASS_RUN_DEBUG << *this << std::endl;
    } else {
        CMN_LOG_CLASS_RUN_DEBUG << "AddMonitorTargetFromJSONFile: Failed to add monitoring target(s) from JSON file: " << jsonFileName << std::endl;
    }

    return ret;
}

bool mtsSafetyCoordinator::AddFilterActive(SF::FilterBase * filter, mtsTask * targetTask)
{
    std::string         signalName;
    SF::SignalElement * signal = 0;
    mtsHistoryBuffer  * historyBuffer = 0;
    mtsStateTable     * stateTable = 0;

    // Instantiate history buffer for each input signal
    const size_t numberOfInputs = filter->GetNumberOfInputs();
    for (size_t i = 0; i < numberOfInputs; ++i) {
        signal = filter->GetInputSignalElement(i);
        CMN_ASSERT(signal);
        signalName = signal->GetName();

        // Each component has two state tables -- the default state table 
        // and the monitoring state table (if the SF plug-in option enabled).
        // Because input signals can be either raw signals from other component,
        // which is managed by the default one, or output of other filters
        // that the monitoring state table maintains, we first need to figure out 
        // which state table has the input signal being added.

        // First search for signal name in default state table
        stateTable = targetTask->GetDefaultStateTable();
        int stateVectorId = stateTable->GetStateVectorID(signalName);

        if (stateVectorId != mtsStateTable::INVALID_STATEVECTOR_ID) {
            historyBuffer = new mtsHistoryBuffer(SF::FilterBase::ACTIVE, stateTable);
            signal->SetHistoryBufferInstance(historyBuffer);
            // associate output signal with state vector
            signal->SetHistoryBufferIndex(stateVectorId);
        } else {
            // If not found, try monitoring state table
            stateTable = targetTask->GetMonitoringStateTable();
            stateVectorId = stateTable->GetStateVectorID(signalName);

            if (stateVectorId != mtsStateTable::INVALID_STATEVECTOR_ID) {
                historyBuffer = new mtsHistoryBuffer(SF::FilterBase::ACTIVE, stateTable);
                signal->SetHistoryBufferInstance(historyBuffer);
                // associate output signal with state vector
                signal->SetHistoryBufferIndex(stateVectorId);
            } else {
                CMN_LOG_CLASS_RUN_ERROR << "AddFilterActive: no input signal \"" << signalName << "\" found" << std::endl;
                return false;
            }
        }
    }

    // Instantiate history buffer for each output signal.
    // Output signals are maintained by monitoring state table.
    stateTable = targetTask->GetMonitoringStateTable();
    const size_t numberOfOutputs = filter->GetNumberOfOutputs();
    for (size_t i = 0; i < numberOfOutputs; ++i) {
        signal = filter->GetOutputSignalElement(i);
        CMN_ASSERT(signal);
        signalName = signal->GetName();

        SF::SignalElement::SignalType outputSignalType = signal->GetSignalType();
        // Create new state vector for output and add it to the monitoring state table.
        // MJ: adding a new element on the fly may not be thread-safe but adding a new
        // filter is usually done before starting components.
        // Should filters be added at run-time, this thread-safety issue may need to
        // be reviewed.
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

    CMN_LOG_CLASS_RUN_DEBUG << "AddFilterActive: Successfully added ACTIVE filter to component \"" 
        << targetTask->GetName() << "\", filter: " << *filter << std::endl;

    return true;
}

bool mtsSafetyCoordinator::AddFilterPassive(SF::FilterBase    * filter,
                                            mtsTask           * targetTask,
                                            const std::string & targetProcessName,
                                            const std::string & targetComponentName)
{
    std::string         signalName;
    SF::SignalElement * signal;
    mtsHistoryBuffer  * historyBuffer = 0;

    // Get target component accessor
    mtsMonitorComponent::TargetComponentAccessor * accessor
        = Monitors[0]->GetTargetComponentAccessor(targetComponentName);
    if (!accessor) {
        //
        // FIXME: THIS SHOULD BE UPDATED ACCORDINGLY TO BE ABLE TO SUPPORT CUSTOM_TARGET
        //
        // If target component accessor is null, create new accessor and add it
        // to the monitoring component.
        accessor = Monitors[0]->CreateTargetComponentAccessor(targetProcessName, 
                                                              targetComponentName, 
                                                              SF::Monitor::TARGET_FILTER_EVENT, 
                                                              false, // this is not active filter
                                                              true); // add accessor to internal data structure
        if (!accessor) {
            CMN_LOG_CLASS_RUN_ERROR << "AddFilterPassive: Failed to add passive filter: " << *filter << std::endl;
            return false;
        }
    }

    // Instantiate history buffer for each input signal
    mtsStateTable * stateTable = Monitors[0]->GetMonitoringStateTable();
    CMN_ASSERT(stateTable);
    const size_t numberOfInputs = filter->GetNumberOfInputs();
    for (size_t i = 0; i < numberOfInputs; ++i) {
        signal = filter->GetInputSignalElement(i);
        CMN_ASSERT(signal);
        signalName = signal->GetName();

        // TODO: Establish mts connection between monitor::required interface and
        // target component's state table::provided interface

        // Check if a read-from-statetable command that the input signal uses
        // has been registered to the default state table of the target component.
        // If not, the input signal cannot fetch new values from the state table and
        // thus always have zero value(s).
        // If the input signal exists, create new historyBuffer instance that contains 
        // the read functions, and add the read functions to the required interface of 
        // the target component accessor.

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
    const size_t numberOfOutputs = filter->GetNumberOfOutputs();
    for (size_t i = 0; i < numberOfOutputs; ++i) {
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

    CMN_LOG_CLASS_RUN_DEBUG << "AddFilterPassive: Successfully added PASSIVE filter to component \"" 
        << targetTask->GetName() << "\", filter: " << *filter << std::endl;

    return true;
}

bool mtsSafetyCoordinator::AddFilterFromJSONFileToComponent(const std::string & jsonFileName,
                                                            const std::string & targetComponentName)
{
    // Construct JSON structure from JSON file
    SF::JSON json;
    if (!json.ReadFromFile(jsonFileName)) {
        CMN_LOG_CLASS_RUN_ERROR << "AddFilterFromJSONFileToComponent: Failed to read json file: " << jsonFileName << std::endl;
        return false;
    }
    CMN_LOG_CLASS_RUN_DEBUG << "AddFilterFromJSONFileToComponent: Successfully read json file: " << jsonFileName << std::endl;

    // Replace placeholder for target component name with actual target component name
    std::string filterClassName;
    SF::JSON::JSONVALUE & filters = json.GetRoot()[SF::Dict::Json::filter];
    for (size_t i = 0; i < filters.size(); ++i) {
        SF::JSON::JSONVALUE & filter = filters[i];
        filter[SF::Dict::Json::target_component] = targetComponentName;
    }

    bool ret = AddFilter(filters);
    if (!ret) {
        CMN_LOG_CLASS_RUN_DEBUG << "AddFilterFromJSONFile: Failed to add filter(s) from JSON file: " << jsonFileName << std::endl;
        return false;
    }

    CMN_LOG_CLASS_RUN_DEBUG << "AddFilterFromJSONFile: Successfully added filter(s) from JSON file: " << jsonFileName << std::endl;
    CMN_LOG_CLASS_RUN_DEBUG << *this << std::endl;

    return true;
}

bool mtsSafetyCoordinator::AddFilterFromJSONFile(const std::string & jsonFileName)
{
    // Construct JSON structure from JSON file
    SF::JSON json;
    if (!json.ReadFromFile(jsonFileName)) {
        CMN_LOG_CLASS_RUN_ERROR << "AddFilterFromJSONFile: Failed to read json file: " << jsonFileName << std::endl;
        return false;
    }

    const SF::JSON::JSONVALUE filters = json.GetRoot()[SF::Dict::Json::filter];
    bool ret = AddFilter(filters);

    if (!ret) {
        CMN_LOG_CLASS_RUN_DEBUG << "AddFilterFromJSONFile: Failed to add filter(s) from JSON file: " << jsonFileName << std::endl;
        return false;
    }

    CMN_LOG_CLASS_RUN_DEBUG << "AddFilterFromJSONFile: Successfully added filter(s) from JSON file: " << jsonFileName << std::endl;
    CMN_LOG_CLASS_RUN_DEBUG << *this << std::endl;

    return ret;
}

bool mtsSafetyCoordinator::AddFilterFromJSON(const std::string & jsonString)
{
    // Construct JSON structure from JSON string
    SF::JSON json;
    if (!json.Read(jsonString.c_str())) {
        CMN_LOG_CLASS_RUN_ERROR << "AddFilterFromJSON: Failed to read json string: " << jsonString << std::endl;
        return false;
    }

    const SF::JSON::JSONVALUE filters = json.GetRoot()[SF::Dict::Json::filter];
    bool ret = AddFilter(filters);

    if (!ret) {
        CMN_LOG_CLASS_RUN_DEBUG << "AddFilterFromJSON: Failed to add filter(s) using json string: " << jsonString << std::endl;
        return false;
    }

    CMN_LOG_CLASS_RUN_DEBUG << "AddFilterFromJSON: Successfully added filter(s) using json string: " << jsonString << std::endl;
    CMN_LOG_CLASS_RUN_DEBUG << *this << std::endl;

    return true;
}

bool mtsSafetyCoordinator::AddFilter(const SF::JSON::JSONVALUE & filters)
{
    if (filters.isNull() || filters.size() == 0) {
        CMN_LOG_CLASS_RUN_ERROR << "AddFilter: No filter specification found in json: " << filters << std::endl;
        return false;
    }

    // Create filter target instance

    // Figure out how many filters are defined, and 
    // create and install filter instances while iterating each filter specification
    std::string filterClassName;
    SF::FilterBase * filter = 0; 
    for (size_t i = 0; i < filters.size(); ++i) {
        filterClassName = SF::JSON::GetSafeValueString(filters[i], SF::Dict::Json::class_name);
        // Create filter instance based on filter class name using filter factory
        filter = SF::FilterFactory::GetInstance()->CreateFilter(filterClassName, filters[i]);
        if (!filter) {
            CMN_LOG_CLASS_RUN_ERROR << "AddFilter: Failed to create filter instance \"" << filter->GetFilterName() << "\"\n";
            continue;
        }

        // Install filter to the target component
        if (!AddFilter(filter)) {
            CMN_LOG_CLASS_RUN_ERROR << "AddFilter: Failed to add filter \"" << filter->GetFilterName() << "\"\n";
            delete filter;
            return false;
        }
        CMN_LOG_CLASS_RUN_DEBUG << "[" << (i + 1) << "/" << filters.size() << "] "
            << "Successfully installed filter: \"" << filter->GetFilterName() << "\"" << std::endl;
        CMN_LOG_CLASS_RUN_DEBUG << *filter << std::endl;
   }

    return true;
}

bool mtsSafetyCoordinator::AddFilter(SF::FilterBase * filter)
{
    if (!filter) {
        CMN_LOG_CLASS_RUN_ERROR << "AddFilter: NULL filter instance" << std::endl;
        return false;
    }

    // Collect arguments
    const std::string targetProcessName   = mtsManagerLocal::GetInstance()->GetName();
    const std::string targetComponentName = filter->GetNameOfTargetComponent();
    const SF::FilterBase::FilteringType filteringType = filter->GetFilteringType();

    // filter can be only deployed to the process where target component runs.
    if (mtsManagerLocal::GetInstance()->GetProcessName() != targetProcessName) {
        CMN_LOG_CLASS_RUN_WARNING << "AddFilter: filters can be deployed to the same process: current process \"" 
                                  << mtsManagerLocal::GetInstance()->GetProcessName() << "\", target process \""
                                  << targetProcessName << "\"" << std::endl;
        return true;
    }

    // Check if task-type target component exists
    // Framework-level properties such as thread overrun flag or exception occurrence flag
    // are defined only for task-type components.
    mtsComponent * component = mtsManagerLocal::GetInstance()->GetComponent(targetComponentName);
    if (!component) {
        CMN_LOG_CLASS_RUN_ERROR << "AddFilter: no component found: \"" << targetComponentName << "\"" << std::endl;
        return false;
    }

    mtsTask * targetTask = mtsManagerLocal::GetInstance()->GetComponentAsTask(targetComponentName);
    if (!targetTask) {
        CMN_LOG_CLASS_RUN_WARNING << "AddFilter: component found but not task-type: \"" << targetComponentName << "\"" << std::endl;
        return true;
    }

    // Active filters are run by the target component and passive filters are run by
    // the monitoring component.
    if (filteringType == SF::FilterBase::ACTIVE) {
        if (!AddFilterActive(filter, targetTask)) {
            CMN_LOG_CLASS_RUN_ERROR << "AddFilter: failed to add active filter for: " << *filter << std::endl;
            return false;
        }
    }
    else
        AddFilterPassive(filter, targetTask, targetProcessName, targetComponentName);
    
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

        // Define monitoring target
        SF::cisstEventLocation * locationID = new SF::cisstEventLocation;
        locationID->SetProcessName(targetProcessName);
        locationID->SetComponentName(targetComponentName);

        SF::cisstMonitor * monitor = new SF::cisstMonitor(SF::Monitor::TARGET_FILTER_EVENT,
                                                            locationID,
                                                            SF::Monitor::STATE_ON,
                                                            SF::Monitor::OUTPUT_EVENT);
        if (filteringType == SF::FilterBase::ACTIVE) {
            // Install fault event handler to target component accessor of the monitor component
            monitor->SetAttachedToActiveFilter(true);

            if (!AddMonitorTarget(monitor)) {
                CMN_LOG_CLASS_RUN_ERROR << "AddFilter: Failed to install new monitor. Filter: [" << *filter << "], Monitor: [" << *monitor << "]" << std::endl;
                // MJ TODO: undo signal installation process above
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

            if (!AddMonitorTarget(monitor)) {
                CMN_LOG_CLASS_RUN_ERROR << "AddFilter: Failed to install new monitor. Filter: [" << *filter << "], Monitor: [" << *monitor << "]" << std::endl;
                // MJ TODO: undo signal installation above
                delete monitor;
                return false;
            }
        }
    }

    // Add filter to the list of filters deployed
    const SF::FilterBase::FilterIDType uid = filter->GetFilterUID();
    if (Filters.find(uid) == Filters.end())
        Filters[uid] = filter;

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

void mtsSafetyCoordinator::OnFaultEvent(const std::string & json)
{
    SF::JSONSerializer jsonSerializer;
    if (!jsonSerializer.ParseJSON(json)) {
        CMN_LOG_CLASS_RUN_ERROR << "OnFaultEvent: failed to parse json:\n" << json << std::endl;
        return;
    }

    // Get filter instance based on filter UID
    const SF::FilterBase::FilterIDType uid = jsonSerializer.GetFilterUID();
    FiltersType::const_iterator it = Filters.find(uid);
    if (it == Filters.end()) {
        CMN_LOG_CLASS_RUN_WARNING << "OnFaultEvent: no filter with uid " << uid << " found" << std::endl;
        return;
    }
    SF::FilterBase * filter = it->second;
    CMN_ASSERT(filter);

    // If filter has reported an event which has not been resolved yet, ignore it.
    // MJTODO: If SF::FilterBase is updated to handle multiple events simultaneously, 
    // HasPendingEvent can accept argument to specify the type of event of interest.
    if (!filter->HasPendingEvent()) {
        CMN_LOG_CLASS_RUN_WARNING << "OnFaultEvent: Filter [ " << filter->GetFilterName() << " ] "
            << "detected fault: \n" << json << std::endl;
        if (!filter->SetEventDetected(json)) {
            CMN_LOG_CLASS_RUN_WARNING << "OnFaultEvent: Failed to set event detected: \n" << json << std::endl;
            return;
        }
    }

#if 0 // MJTEMP: disabled test codes
    const std::string targetComponentName = filter->GetNameOfTargetComponent();
    mtsComponent * component = mtsManagerLocal::GetInstance()->GetComponentAsTask(targetComponentName);
    if (!component) {
        CMN_LOG_CLASS_RUN_WARNING << "OnFaultEvent: no component found: \""
            << targetComponentName << "\"" << std::endl;
        return;
    }

    // change state to ERROR
    // TEMP: THIS SHOULD BE OTHER STATE depending on json (e.g., severity information)
    static int cnt = 0;
    if (cnt++ == 0) {
        component->FaultState->ProcessEvent(SF::State::ERROR_DETECTION);
    }
    if (cnt == 500) {
        component->FaultState->ProcessEvent(SF::State::ERROR_REMOVAL);
        cnt = 0;
    }
#endif
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
    mtsMonitorComponent * monitor;
    try {
        monitor = new mtsMonitorComponent(false);
    } catch (const std::exception & e) {
        return false;
    }
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

const std::string mtsSafetyCoordinator::GetJsonForPublish(
    const SF::cisstMonitor & monitorTarget, mtsGenericObject * payload, double timestamp)
{
    if (!payload)
        return std::string("");

    // JSONSerializer instance
    SF::JSONSerializer serializer;

    // Predefined monitoring targets are handled by SF::cisstMonitor
    const SF::Monitor::TargetType targetType = monitorTarget.GetTargetType();
    if (targetType != SF::Monitor::TARGET_CUSTOM)
        return std::string("");

    // Populate common fields
    SF::cisstEventLocation * locationID =
        dynamic_cast<SF::cisstEventLocation*>(monitorTarget.GetLocationID());
    serializer.SetTopicType(SF::JSONSerializer::MONITOR);
    serializer.SetEventLocation(locationID);
    serializer.SetTimestamp(timestamp);
    serializer.SetMonitorTargetType(SF::Monitor::TARGET_CUSTOM);

    // Populate monitor-specific fields
    SF::JSON::JSONVALUE & fields = serializer.GetMonitorFields();

    // Extract double values from payload
    std::stringstream ss;
    payload->ToStreamRaw(ss);
    std::vector<double> values;
    size_t numberOfScalar = mtsSafetyCoordinator::ExtractScalarValues(ss, values);
    if (numberOfScalar == 1) {
        // If single element scalr, key is "value" (otherwise, "values")
        fields[SF::Dict::Json::value] = values[0];
    } else {
        fields[SF::Dict::Json::values].resize(numberOfScalar);
        for (size_t i = 0; i < numberOfScalar; ++i)
            fields[SF::Dict::Json::values][i] = values[i];
    }

#if 0
    // Handle instances of mtsGenericObject and mtsGenericObjectProxy differently
    size_t numberOfElement = payload->ScalarNumber();
    // If single element scalr, key is "value" (otherwise, "values")
    if (numberOfElement == 1)
        fields[SF::Dict::Json::value] = payload->Scalar(0);
    else {
        std::stringstream ss;
        ss << "[ ";
        for (size_t i = 0; i < numberOfElement; ++i) {
            if (i > 0) ss << ", ";
            ss << std::setprecision(PRECISION) << payload->Scalar(i);
        }
        ss << " ]";
        fields[SF::Dict::Json::values] = ss.str();
    }
#endif

    return serializer.GetJSON();
}

size_t mtsSafetyCoordinator::ExtractScalarValues(const std::stringstream & ss, 
                                                 std::vector<double> & values)
{
#define DELIMITER " "
    char * str = new char[ss.str().size()];
    memcpy(str, ss.str().c_str(), ss.str().size());
    char * ptr = strtok(str, DELIMITER);
    int cnt = 1;
    while (ptr) {
        // skip three base numeric fields from cmnGenericObject
        if (cnt++ <= 3) {
            ptr = strtok(0, DELIMITER);
            continue;
        }
        values.push_back(atof(ptr));
        ptr = strtok(0, DELIMITER);
    }
#undef DELIMITER

    return values.size();
}

