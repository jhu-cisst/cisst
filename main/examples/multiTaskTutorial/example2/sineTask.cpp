/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
// $Id: sineTask.cpp 561 2009-07-18 02:52:33Z adeguet1 $

#include <cisstCommon/cmnConstants.h>
#include "sineTask.h"

CMN_IMPLEMENT_SERVICES(sineTask);

sineTask::sineTask(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 5000)
{
    StateTable.AddData(SineData, "SineData");
    mtsProvidedInterface * mainInterface = AddProvidedInterface("MainInterface");
    if (mainInterface) {
        // add commands to access state table values
        mainInterface->AddCommandReadState(StateTable, SineData, "GetData");
        // add (queued) commands to set member data
        mainInterface->AddCommandWrite(&sineTask::SetAmplitude, this, "SetAmplitude");
        mainInterface->AddCommandWrite(&sineTask::SetTrigger, this, "SetTriggerValue");
        // add a command bound to a user defined method
        mainInterface->AddCommandVoid(&sineTask::ResetTrigger, this,"ResetTrigger");
        // define an event and setup our event sending function
        mtsDouble eventData; // data type used for the event payload
        mainInterface->AddEventWrite(TriggerEvent, "TriggerEvent", eventData);
    }
}

void sineTask::ResetTrigger(void)
{
    TriggerEnabled = true;
}

void sineTask::Startup(void) {
    SineData = 0.0;
    SineAmplitude = 1.0;
    TriggerValue = 0.0;
    TriggerEnabled = false;
}

void sineTask::Run(void) {
    ProcessQueuedCommands();
    SineData = SineAmplitude
        * sin(2 * cmnPI * static_cast<double>(this->GetTick()) * Period / 10.0);
    // check if the trigger is enabled and if the conditions are right
    // to send an event
    if (TriggerEnabled) {
        if  (SineData >= TriggerValue) {
            // use the mtsFunctionWrite to send the event along with
            // the current data
            TriggerEvent(SineData);
            TriggerEnabled = false;
        }
    }
}

/*
  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/
