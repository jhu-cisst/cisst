/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstCommon/cmnConstants.h>
#include "sineTask.h"

// required to implement the class services, see cisstCommon
CMN_IMPLEMENT_SERVICES_ONEARG(sineTask, mtsTaskPeriodicConstructorArg);

sineTask::sineTask(const std::string & taskName, double period):
    // base constructor, same task name and period.
    mtsTaskPeriodic(taskName, period, false, 500)
{
    // call generated method to configure this component
    InitComponent();
}

sineTask::sineTask(const mtsTaskPeriodicConstructorArg &arg):
    // base constructor, same task name and period.
    mtsTaskPeriodic(arg)
{
    // call generated method to configure this component
    InitComponent();
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
