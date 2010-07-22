/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
// $Id$

#include <math.h>
#include "displayTask.h"
#include "displayUI.h"

CMN_IMPLEMENT_SERVICES(displayTask);

displayTask::displayTask(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 5000)
{
    mtsInterfaceRequired *req = AddInterfaceRequired("DataGenerator");
    if (req) {
        req->AddFunction("GetData", Generator.GetData);
        req->AddFunction("SetAmplitude", Generator.SetAmplitude);
        req->AddFunction("SetTriggerValue", Generator.SetTriggerValue);
        req->AddFunction("ResetTrigger", Generator.ResetTrigger);
        // create an event handler associated to the output port.  false
        // means not queued.
        req->AddEventHandlerWrite(&displayTask::HandleTrigger, this,
                                  "TriggerEvent", MTS_EVENT_NOT_QUEUED);
    }
    req = AddInterfaceRequired("Clock");
    if (req)
        req->AddFunction("GetTime", Clock.GetClockData);
}

displayTask::~displayTask()
{}

void displayTask::HandleTrigger(const mtsDouble & value)
{
    CMN_LOG_RUN_VERBOSE << "HandleTrigger: Trigger event (" << this->GetName() << "): "
                        << value << std::endl;
    WaitingForTrigger = false;
    Wakeup();
}

void displayTask::Startup(void)
{
    TriggerValue = 0.0;
    WaitingForTrigger = false;

    Generator.SetAmplitude(Amplitude);
    Generator.SetTriggerValue(TriggerValue);

    UI.show(0, NULL);
}

void displayTask::Run(void)
{
    Generator.GetData(Data);
    Clock.GetClockData(Time);
    UI.Data->value(Data);
    UI.Time->value(Time); // display in seconds
    if (UI.AmplitudeChanged) {
        Amplitude = UI.Amplitude->value();
        Generator.SetAmplitude(Amplitude);
        UI.AmplitudeChanged = false;
        CMN_LOG_RUN_VERBOSE << "Run: " << this->GetTick()
                            << " - Amplitude: " << Amplitude << std::endl;
    }
    if (UI.TriggerChanged) {
        TriggerValue = UI.Trigger->value() * Amplitude;
        Generator.SetTriggerValue(TriggerValue);
        UI.TriggerChanged = false;
        CMN_LOG_RUN_VERBOSE << "Run : " << this->GetTick()
                            << " - Trigger: " << TriggerValue << std::endl;
    }
    if (UI.DoReset) {
        CMN_LOG_RUN_VERBOSE << "Run : " << this->GetTick()
                            << " - Reset trigger." << std::endl;
        Generator.ResetTrigger();
        UI.DoReset = false;
    }
    if (UI.TriggerWaitChanged) {
        UI.TriggerWaitChanged = false;
        if (UI.WaitForTrigger->value()) {
            CMN_LOG_RUN_WARNING << "Run: Waiting for trigger." << std::endl;
            // Reset trigger to make sure we get one
            Generator.ResetTrigger();
            WaitingForTrigger = true;
            while (WaitingForTrigger) {
                displayUI::Semaphore = false;
                // use mtsTask::WaitForWakeup to freeze until trigger
                // event wakes us up
                WaitForWakeup();
            }
            // Uncheck the box
            UI.WaitForTrigger->value(0);
        }
    }
    ProcessQueuedEvents();

    if (UI.Closed == true) {
        Kill();
    } else {
        if (!displayUI::Semaphore) {
            displayUI::Semaphore = true;
            Fl::check();
            displayUI::Semaphore = false;
        }
    }
}

void displayTask::Configure(const std::string & CMN_UNUSED(filename))
{
    double maxValue, minValue, startValue;
    minValue = 0.5;
    maxValue = 5.0;
    startValue =  1.0;

    CMN_LOG_CLASS_INIT_VERBOSE << "Configure: Setting bounds to: " << minValue << ", " << maxValue << std::endl;
    CMN_LOG_CLASS_INIT_VERBOSE << "Configure: Setting start value to: " << startValue << std::endl;

    UI.Amplitude->bounds(minValue, maxValue);
    UI.Amplitude->value(startValue);
    Amplitude = startValue;
    TriggerValue = 0.0;
}

/*
  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/
