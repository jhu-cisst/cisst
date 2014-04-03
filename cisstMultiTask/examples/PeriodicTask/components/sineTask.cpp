/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2013 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "sineTask.h"

#include <cisstCommon/cmnConstants.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

// required to implement the class services, see cisstCommon
CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(sineTask, mtsTaskPeriodic, mtsTaskPeriodicConstructorArg);

sineTask::sineTask(const std::string & componentName, double periodInSeconds):
    // base constructor, same task name and period.
    mtsTaskPeriodic(componentName, periodInSeconds, false, 500)
{
    SetupInterfaces();
}

sineTask::sineTask(const mtsTaskPeriodicConstructorArg & arg):
    // base constructor, same task name and period.
    mtsTaskPeriodic(arg)
{
    SetupInterfaces();
}

void sineTask::SetupInterfaces(void)
{
    // state table variables
    StateTable.AddData(SineData, "SineData");

    // provided interfaces
    mtsInterfaceProvided * interfaceProvided;
    interfaceProvided = AddInterfaceProvided("MainInterface");
    if (!interfaceProvided) {
        CMN_LOG_CLASS_INIT_ERROR << "failed to add \"MainInterface\" to component \"" << this->GetName() << "\"" << std::endl;
    }
    if (!interfaceProvided->AddCommandWrite(&sineTask::SetAmplitude, this, "SetAmplitude",  mtsDouble(1.0))) {
        CMN_LOG_CLASS_INIT_ERROR << "failed to add command to interface\"" << interfaceProvided->GetFullName() << "\"" << std::endl;
    }
    if (!interfaceProvided->AddCommandReadState(StateTable, SineData, "GetData")) {
        CMN_LOG_CLASS_INIT_ERROR << "failed to add command to interface\"" << interfaceProvided->GetFullName() << "\"" << std::endl;
    }
    if (!interfaceProvided->AddCommandWrite(&sineTask::SetTriggerThreshold, this, "SetTriggerThreshold",  mtsDouble(0.0))) {
        CMN_LOG_CLASS_INIT_ERROR << "failed to add command to interface\"" << interfaceProvided->GetFullName() << "\"" << std::endl;
    }
    if (!interfaceProvided->AddCommandVoid(&sineTask::ResetTrigger, this, "ResetTrigger")) {
        CMN_LOG_CLASS_INIT_ERROR << "failed to add command to interface\"" << interfaceProvided->GetFullName() << "\"" << std::endl;
    }
    if (!interfaceProvided->AddEventVoid(MainInterface.TriggerEvent, "TriggerEvent")) {
        CMN_LOG_CLASS_INIT_ERROR << "failed to add event to interface\"" << interfaceProvided->GetFullName() << "\"" << std::endl;
    }
}

void sineTask::SetAmplitude(const mtsDouble & amplitude)
{
    SineAmplitude = amplitude.Data;
}

void sineTask::SetTriggerThreshold(const mtsDouble & trigger)
{
    TriggerThreshold = trigger.Data;
}

void sineTask::ResetTrigger(void)
{
    TriggerEnabled = true;
}

void sineTask::Startup(void)
{
    SineData = 0.0;
    SineAmplitude = 1.0;
    TriggerThreshold = 0.0;
    TriggerEnabled = false;
}

void sineTask::Run(void)
{
    // process the commands received
    ProcessQueuedCommands();
    SineData = SineAmplitude
        * sin(2 * cmnPI * static_cast<double>(this->GetTick()) * Period / 10.0);
    // check if the trigger is enabled and if the conditions are right
    // to send an event
    if (TriggerEnabled) {
        if  (SineData >= TriggerThreshold) {
            MainInterface.TriggerEvent();
            TriggerEnabled = false;
        }
    }
}
