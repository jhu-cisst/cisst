/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
// $Id: displayTask.cpp,v 1.10 2009/01/09 23:05:28 pkaz Exp $

#include <math.h>
#include "displayTask.h"
#include "displayUI.h"

CMN_IMPLEMENT_SERVICES(displayTask);

displayTask::displayTask(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 5000),
    ExitFlag(false)
{
    mtsRequiredInterface *req = AddRequiredInterface("DataGenerator");
    if (req) {
        req->AddFunction("GetData", Generator.GetData);
        req->AddFunction("SetAmplitude", Generator.SetAmplitude);
        req->AddFunction("SetTriggerValue", Generator.SetTriggerValue);
        req->AddFunction("ResetTrigger", Generator.ResetTrigger);
        // create an event handler associated to the output port.  false
        // means not queued.
        req->AddEventHandlerWrite(&displayTask::HandleTrigger, this,
                                  "TriggerEvent", this->Data, false);
    }
    req = AddRequiredInterface("Clock");
    if (req)
        req->AddFunction("GetTime", Clock.GetClockData);
}

displayTask::~displayTask()
{}

void displayTask::HandleTrigger(const cmnDouble & value)
{
    CMN_LOG(5) << "HandleTrigger: Trigger event (" << this->GetName() << "): "
               << value << std::endl;
    WaitingForTrigger = false;
    Wakeup();
}

void displayTask::Startup(void) 
{
    Amplitude = StartValue;
    TriggerValue = 0.0;
    WaitingForTrigger = false;

    Generator.SetAmplitude(Amplitude);
    Generator.SetTriggerValue(TriggerValue);

    UI.show(0, NULL);
}

void displayTask::Run(void)
{
    const mtsStateIndex now = StateTable.GetIndexWriter();
    Generator.GetData(Data);
    Clock.GetClockData(Time);
    UI.Data->value(Data);
    UI.Time->value(Time); // display in seconds
    if (UI.AmplitudeChanged) {
        Amplitude = UI.Amplitude->value();
        Generator.SetAmplitude(Amplitude);
        UI.AmplitudeChanged = false;
        CMN_LOG(7) << "Run: " << now.Ticks()
                   << " - Amplitude: " << Amplitude << std::endl;
    }
    if (UI.TriggerChanged) {
        TriggerValue = UI.Trigger->value() * Amplitude;
        Generator.SetTriggerValue(TriggerValue);
        UI.TriggerChanged = false;
        CMN_LOG(7) << "Run : " << now.Ticks()
                   << " - Trigger: " << TriggerValue << std::endl;
    }
    if (UI.DoReset) {
        CMN_LOG(7) << "Run : " << now.Ticks()
                   << " - Reset trigger." << std::endl;
        Generator.ResetTrigger();
        UI.DoReset = false;
    }
    if (UI.TriggerWaitChanged) {
        UI.TriggerWaitChanged = false;
        if (UI.WaitForTrigger->value()) {
            CMN_LOG(6) << "Run: Waiting for trigger." << std::endl;
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

    CMN_LOG(7) << "Run: " << now.Ticks()
               << " - Data: " << Data << std::endl;
    
    if (UI.Closed == true) {
        ExitFlag = true;
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
    double maxValue, minValue;
    minValue = 0.5;
    maxValue = 5.0;
    StartValue =  1.0;
    
    CMN_LOG_CLASS(3) << "Configure: Setting bounds to: " << minValue << ", " << maxValue << std::endl;
    CMN_LOG_CLASS(3) << "Configure: Setting start value to: " << StartValue << std::endl;
    
    UI.Amplitude->bounds(minValue, maxValue);
    UI.Amplitude->value(StartValue);
    Amplitude = StartValue;
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
