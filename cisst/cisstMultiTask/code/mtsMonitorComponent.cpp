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
    //: mtsTaskPeriodic(NameOfMonitorComponent, 10.0 * cmn_ms)
    : mtsTaskPeriodic(NameOfMonitorComponent, 1.0 * cmn_s, false, 100) // MJ TEMP
{
    TargetComponents = new TargetComponentsType(true);
}

mtsMonitorComponent::~mtsMonitorComponent()
{
    TargetComponents->DeleteAll();
    delete TargetComponents;
}

void mtsMonitorComponent::Run(void)
{
    ProcessQueuedCommands();
    ProcessQueuedEvents();

    UpdateFilters();
}

void mtsMonitorComponent::UpdateFilters(void)
{
    TargetComponentsType::const_iterator it = TargetComponents->begin();
    const TargetComponentsType::const_iterator itEnd = TargetComponents->end();
    for (; it != itEnd; ++it) {
        TargetComponent * target = it->second;
        CMN_ASSERT(target);
        target->GetPeriod(target->Period);
    }
}

void mtsMonitorComponent::PrintTargetComponents(void)
{
    TargetComponentsType::const_iterator it = TargetComponents->begin();
    const TargetComponentsType::const_iterator itEnd = TargetComponents->end();
    for (; it != itEnd; ++it) {
        TargetComponent * target = it->second;
        CMN_ASSERT(target);
        target->GetPeriod(target->Period);
        std::cout << target->Name << ": " << target->Period << std::endl;
    }
}

bool mtsMonitorComponent::AddTargetComponent(mtsTaskPeriodic * task)
{
    const std::string taskName = task->GetName();

    if (TargetComponents->FindItem(taskName)) {
        CMN_LOG_CLASS_RUN_WARNING << "AddTargetComponent: task \"" << taskName << "\" is already registered" << std::endl;
        return true;
    }

    // Add new target component
    TargetComponent * newTargetComponent = new TargetComponent;
    newTargetComponent->Name = taskName;
    newTargetComponent->InterfaceRequired = AddInterfaceRequired(GetNameOfStateTableAccessInterface(taskName));
    newTargetComponent->InterfaceRequired->AddFunction("GetPeriod", newTargetComponent->GetPeriod);
    if (!TargetComponents->AddItem(taskName, newTargetComponent)) {
        CMN_LOG_CLASS_RUN_ERROR << "AddTargetComponent: Failed to add state table access interface for task \"" << taskName << "\"" << std::endl;
        return false;
    }

    // Add "Period" to the monitoring state table of this component with the name of
    // (component name)+"Period"
    std::string periodName(taskName);
    periodName += "Period";
    // MJ TEMP: Adding a new element to state table on the fly is not thread safe -- need to fix this.
    this->StateTableMonitor.NewElement(periodName, &newTargetComponent->Period);

    // MJ TODO: add clean up codes when error happens
#define ADD_FILTER( _filter )\
    if (!this->StateTableMonitor.AddFilter(_filter)) {\
        CMN_LOG_CLASS_RUN_ERROR\
           << "AddTargetComponent: Failed to add filter \"" << _filter->GetFilterName()\
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
#endif

    // Create moving average filter to define feature
    mtsMonitorFilterAverage * filterAverage = 
        new mtsMonitorFilterAverage(mtsMonitorFilterBase::FEATURE,
                                    periodName,
                                    mtsMonitorFilterBase::SignalElement::SCALAR,
                                    0.25);
    ADD_FILTER(filterAverage);

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
    const double nominalPeriod = task->GetPeriodicity(true); // nominal (expected) period as avg
    mtsFaultDetectorThresholding * detectorThresholding = 
        new mtsFaultDetectorThresholding(filterAverage->GetOutputSignalName(0),
                                         nominalPeriod, 
                                         (size_t)(1.0 / nominalPeriod) * 5.0);
    // Associate thread periodicity fault with the fault detector
    mtsFaultComponentThreadPeriodicity * fault = new mtsFaultComponentThreadPeriodicity;
    //detectorThresholding->
    ADD_FILTER(detectorThresholding);
#undef ADD_FILTER 

    return true;
}

bool mtsMonitorComponent::RemoveTargetComponent(const std::string & taskName)
{
    if (!TargetComponents->FindItem(taskName)) {
        CMN_LOG_CLASS_RUN_WARNING << "RemoveTargetComponent: task \"" << taskName << "\" is not found" << std::endl;
        return false;
    }

    return TargetComponents->RemoveItem(taskName);
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

