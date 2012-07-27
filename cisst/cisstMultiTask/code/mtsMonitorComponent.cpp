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

#include <cisstMultiTask/mtsMonitorComponent.h>
#include <cisstMultiTask/mtsFaultDetectorThresholding.h>
#include <cisstMultiTask/mtsMonitorFilterBasics.h>
#include <cisstMultiTask/mtsFaultTypes.h>

CMN_IMPLEMENT_SERVICES(mtsMonitorComponent);

const std::string NameOfMonitorComponent = "Monitor";

mtsMonitorComponent::mtsMonitorComponent()
    // MJ: Maximum monitoring time resolution is 10 msec (somewhat arbitrary but practically
    // enough to cover most monitoring scenarios)
    : mtsTaskPeriodic(NameOfMonitorComponent, 10.0 * cmn_ms, false, 1000)
{
    TargetComponentAccessors = new TargetComponentAccessorType(true);
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

    UpdateFilters();
}

bool mtsMonitorComponent::AddMonitorTargetToComponent(SF::cisstMonitor & newMonitorTarget)
{
    mtsManagerLocal * LCM = mtsManagerLocal::GetInstance();

    // Validity check: process name
    const std::string thisProcessName = LCM->GetProcessName();
    const std::string processName = newMonitorTarget.GetTargetID().ProcessName;
    if (thisProcessName.compare(processName)) {
        CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTargetToComponent: different process name "
            << "(expected: \"" << thisProcessName << "\", actual: \"" << processName << ")" << std::endl;
        return false;
    }

    // Make sure if the target component exists.
    const std::string targetComponentName = newMonitorTarget.GetTargetID().ComponentName;
    if (!LCM->FindComponent(targetComponentName)) {
        CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTargetToComponent: no component \"" << targetComponentName << "\" found" << std::endl;
        return false;
    }

    /*
    // MJ TODO: embed all necessary sampling mechanism into component depending on the
    // type of component (e.g., mtsComponent, mtsTaskPeriodic, ...) and the specification
    // of monitor target
    if (TargetComponents->FindItem(targetComponentName)) {
        CMN_LOG_CLASS_RUN_WARNING << "RegisterComponent: component \"" << targetComponentName << "\" is already registered" << std::endl;
        return true;
    }
    */

    // Fetch target component information if registered already.  If not, create new instance.
    bool newTargetComponent = false;
    TargetComponentAccessor * targetComponentAccessor = TargetComponentAccessors->GetItem(targetComponentName);
    if (!targetComponentAccessor) {
        // Add new target component
        targetComponentAccessor = new TargetComponentAccessor;
        targetComponentAccessor->Name = targetComponentName;
        targetComponentAccessor->InterfaceRequired = AddInterfaceRequired(GetNameOfStateTableAccessInterface(targetComponentName));
        newTargetComponent = true;
    }

    // [SFUPDATE]
    const SF::Fault::FaultType faultType = newMonitorTarget.GetFaultType();
    switch (faultType) {
        case SF::Fault::FAULT_COMPONENT_PERIOD:
            {
                mtsTaskPeriodic * taskPeriodic = dynamic_cast<mtsTaskPeriodic*>(LCM->GetComponent(targetComponentName));
                if (taskPeriodic) {
                    targetComponentAccessor->InterfaceRequired->AddFunction("GetPeriod", targetComponentAccessor->GetPeriod);
                    /* TODO
                    if (!InstallFilters(targetComponent, taskPeriodic)) {
                        CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTargetToComponent: Failed to install filters to periodic task \"" << targetComponentName << "\"" << std::endl;
                        delete targetComponent;
                        return false;
                    }
                    */
                } else {
                    CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTargetToComponent: " << SF::Fault::GetFaultString(faultType)
                        << " is only supported for periodic task (of type mtsTaskPeriodic)" << std::endl;
                    if (newTargetComponent) delete targetComponentAccessor;
                    return false;
                }
            }
            break;
        case SF::Fault::FAULT_INVALID:
        default:
            CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTargetToComponent: invalid or unsupported fault type: " << newMonitorTarget << std::endl;
            if (newTargetComponent) delete targetComponentAccessor;
            // MJ TODO: implement mtsComponent::RemoveMonitorTarget()
            // targetComponent->RemoveMonitorTarget()
            return false;
            break;
    }
    
    // Check if the same monitoring target element has already been registered.
    mtsComponent * targetComponent = LCM->GetComponent(targetComponentName);
    CMN_ASSERT(targetComponent);
    if (!targetComponent->AddMonitorTarget(newMonitorTarget)) {
        CMN_LOG_CLASS_RUN_ERROR << "AddMonitorTargetToComponent: duplicate monitoring target element: " << newMonitorTarget << std::endl;
        return false;
    }

    if (!TargetComponentAccessors->AddItem(targetComponentName, targetComponentAccessor)) {
        CMN_LOG_CLASS_RUN_ERROR << "RegisterComponent: Failed to add state table access interface for component \"" << targetComponentName << "\"" << std::endl;
        return false;
    }

    PrintTargetComponents();

    CMN_LOG_CLASS_RUN_DEBUG << "RegisterComponent: Successfully registered component: \"" << targetComponentName << "\"" << std::endl;

    return true;
}

void mtsMonitorComponent::UpdateFilters(void)
{
    TargetComponentAccessorType::const_iterator it = TargetComponentAccessors->begin();
    const TargetComponentAccessorType::const_iterator itEnd = TargetComponentAccessors->end();
    TargetComponentAccessor * accessor;
    for (; it != itEnd; ++it) {
        accessor = it->second;
        accessor->GetPeriod(accessor->Period);
    }
}

void mtsMonitorComponent::PrintTargetComponents(void)
{
    std::stringstream ss;
    ss << "Monitoring target component: ";

    TargetComponentAccessorType::const_iterator it = TargetComponentAccessors->begin();
    const TargetComponentAccessorType::const_iterator itEnd = TargetComponentAccessors->end();
    int i = 0;
    for (; it != itEnd; ++it) {
        TargetComponentAccessor * target = it->second;
        CMN_ASSERT(target);
        target->GetPeriod(target->Period);
        ss << "[" << ++i << "] " << target->Name << ": period = " << target->Period << std::endl;
    }

    CMN_LOG_CLASS_RUN_DEBUG << ss.str() << std::endl;
}

bool mtsMonitorComponent::InstallFilters(TargetComponentAccessor * entry, mtsTaskPeriodic * task)
{
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

    return true;
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
