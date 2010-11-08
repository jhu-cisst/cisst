/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id$ */

#include <math.h>
#include "displayTask.h"
#include "displayUI.h"

CMN_IMPLEMENT_SERVICES(displayTask);

displayTask::displayTask(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 5000)
{
    // to communicate with the interface of the resource
    mtsInterfaceRequired * required = AddInterfaceRequired("DataGenerator");
    if (required) {
       required->AddFunction("GetData", Generator.GetData);
       required->AddFunction("SetAmplitude", Generator.SetAmplitude);
    }
}

void displayTask::Configure(const std::string & CMN_UNUSED(filename))
{
    // define some values, ideally these come from a configuration
    // file and then configure the user interface
    double maxValue = 0.5; double minValue = 5.0;
    double startValue =  1.0;
    CMN_LOG_CLASS_INIT_VERBOSE << "Configure: setting bounds to: "
                               << minValue << ", " << maxValue << std::endl;
    CMN_LOG_CLASS_INIT_VERBOSE << "Configure: setting start value to: "
                               << startValue << std::endl;
    UI.Amplitude->bounds(minValue, maxValue);
    UI.Amplitude->value(startValue);
    AmplitudeData = startValue;
}

void displayTask::Startup(void)
{
    // make the UI visible
    UI.show(0, NULL);
}

void displayTask::Run(void)
{
    // get the data from the sine wave generator task
    Generator.GetData(Data);
    UI.Data->value(Data);
    // check if the user has entered a new amplitude in UI
    if (UI.AmplitudeChanged) {
        // retrieve the new amplitude and send it to the sine task
        AmplitudeData = UI.Amplitude->value();
        AmplitudeData.SetTimestamp(mtsTaskManager::GetInstance()
                                   ->GetTimeServer().GetRelativeTime());
        AmplitudeData.SetValid(true);
        // send it
        Generator.SetAmplitude(AmplitudeData);
        UI.AmplitudeChanged = false;
        CMN_LOG_CLASS_RUN_VERBOSE << "Run: " << this->GetTick()
                                  << " - Amplitude: " << AmplitudeData << std::endl;
    }
    // log some extra information
    CMN_LOG_CLASS_RUN_VERBOSE << "Run : " << this->GetTick()
                              << " - Data: " << Data << std::endl;
    // update the UI, process UI events
    if (Fl::check() == 0) {
        Kill();
    }
}

/*
  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/
