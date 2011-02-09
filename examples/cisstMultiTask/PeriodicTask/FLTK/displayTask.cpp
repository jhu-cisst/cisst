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

#include <math.h>
#include "displayTask.h"
#include "displayUI.h"


#define FLTK_CRITICAL_SECTION Fl::lock(); for (bool firstRun = true; firstRun; firstRun = false, Fl::unlock(), Fl::awake())

CMN_IMPLEMENT_SERVICES(displayTask);

displayTask::displayTask(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 5000)
{
    // call generated method to configure this component
    InitComponent();
}

void displayTask::Configure(const std::string & CMN_UNUSED(filename))
{
    // define some values, ideally these come from a configuration
    // file and then configure the user interface
    double maxValue = 5.0; double minValue = 0.5;
    double startValue =  1.0;
    UI.Amplitude->bounds(minValue, maxValue);
    UI.Amplitude->value(startValue);
    UI.AmplitudeValue->value(startValue);
    UI.Trigger->bounds(-0.9, 0.9);
    UI.Trigger->value(0.0);
    UI.TriggerValue->value(0.0);
    // plotting
    size_t traceId;
    UI.Plot->AddTrace("Data", traceId);
    UI.Plot->AddTrace("Trigger", traceId);
    UI.Plot->SetColor(1, vct3(0.0, 1.0, 0.0));
    // display user interface
    UI.show(0, NULL);
    UI.Plot->show();
}

void displayTask::Startup(void)
{
}

void displayTask::HandleTrigger(void)
{
    ThreadSignal.Raise();
}

void displayTask::Run(void)
{
    // get the data from the sine wave generator task
    DataGenerator.GetData(Data);
    TimeGenerator.GetTime(Time);
    FLTK_CRITICAL_SECTION
    {
        UI.Data->value(Data);
        UI.Time->value(Time); // display in seconds
        UI.Plot->AddPoint(0, vctDouble2(Data.Timestamp(), Data.Data));
        UI.Plot->AddPoint(1, vctDouble2(Data.Timestamp(),
                                        UI.Trigger->value() * UI.Amplitude->value()));
        UI.Plot->redraw();
        // check if the user has entered a new amplitude in UI
        if (UI.AmplitudeChanged) {
            // retrieve the new amplitude and send it to the sine task
            AmplitudeData = UI.Amplitude->value();
            AmplitudeData.SetTimestamp(mtsTaskManager::GetInstance()
                                       ->GetTimeServer().GetRelativeTime());
            AmplitudeData.SetValid(true);
            // send it
            DataGenerator.SetAmplitude(AmplitudeData);
            UI.AmplitudeChanged = false;
            CMN_LOG_CLASS_RUN_VERBOSE << "Run: " << this->GetTick()
                                      << " - Amplitude: " << AmplitudeData << std::endl;
        }
        if (UI.TriggerChanged) {
            DataGenerator.SetTriggerThreshold(UI.Trigger->value() * UI.Amplitude->value());
            UI.TriggerChanged = false;
            CMN_LOG_CLASS_RUN_VERBOSE << "Run : " << this->GetTick()
                                      << " - Trigger: " << UI.Trigger->value() << std::endl;
        }
        if (UI.TriggerWaitChanged) {
            UI.TriggerWaitChanged = false;
            if (UI.WaitForTrigger->value()) {
                CMN_LOG_RUN_VERBOSE << "Run: Waiting for trigger." << std::endl;
                DataGenerator.ResetTrigger();
                ThreadSignal.Wait();
                UI.WaitForTrigger->value(0);
            }
        }
    }

    // log some extra information
    CMN_LOG_CLASS_RUN_VERBOSE << "Run : " << this->GetTick()
                              << " - Data: " << Data << std::endl;
}

void displayTask::UpdateUI(void)
{
    // update the UI, process UI events
    FLTK_CRITICAL_SECTION {
        if (Fl::check() == 0) {
            Kill();
        }
    }
}
