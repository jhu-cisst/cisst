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

CMN_IMPLEMENT_SERVICES(mtsMonitorComponent);

const std::string NameOfMonitorComponent = "cisstMonitor";

mtsMonitorComponent::mtsMonitorComponent()
    : mtsTaskPeriodic(NameOfMonitorComponent, 10.0 * cmn_ms)
{
    StateTableAccessors = new StateTableAccessorType(true);
    StateTableAccessInterfaces = new StateTableAccessInterfaceType(true);
}

mtsMonitorComponent::~mtsMonitorComponent()
{
    StateTableAccessors->DeleteAll();
    StateTableAccessInterfaces->DeleteAll();

    delete StateTableAccessors;
    delete StateTableAccessInterfaces;
}

bool mtsMonitorComponent::AddTargetComponent(mtsTask * task)
{
    const std::string taskName = task->GetName();

    if (StateTableAccessInterfaces->FindItem(taskName) || StateTableAccessors->FindItem(taskName)) {
        CMN_LOG_CLASS_RUN_WARNING << "AddTargetComponent: task \"" << taskName << "\" is already registered" << std::endl;
        return true;
    }

    StateTableAccessor * accessor = new StateTableAccessor;
    CMN_ASSERT(accessor);
    mtsInterfaceRequired * required = AddInterfaceRequired(GetNameOfStateTableAccessInterface(taskName));
    CMN_ASSERT(required);

    required->AddFunction("GetPeriod", accessor->GetPeriod);
    // more to come ...

    if (!StateTableAccessInterfaces->AddItem(taskName, required)) {
        CMN_LOG_CLASS_RUN_ERROR << "AddTargetComponent: Failed to add state table access interface for task \"" << taskName << "\"" << std::endl;
        return false;
    }
    if (!StateTableAccessors->AddItem(taskName, accessor)) {
        StateTableAccessInterfaces->RemoveItem(taskName);
        CMN_LOG_CLASS_RUN_ERROR << "AddTargetComponent: Failed to add state table accessor for task \"" << taskName << "\"" << std::endl;
        return false;
    }

    return true;
}

bool mtsMonitorComponent::RemoveTargetComponent(const std::string & taskName)
{
    if (!StateTableAccessInterfaces->FindItem(taskName) || !StateTableAccessors->FindItem(taskName)) {
        CMN_LOG_CLASS_RUN_WARNING << "RemoveTargetComponent: task \"" << taskName << "\" is not found" << std::endl;
        return true;
    }

    bool success = true;
    success &= StateTableAccessInterfaces->RemoveItem(taskName);
    success &= StateTableAccessors->RemoveItem(taskName);

    return success;
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

