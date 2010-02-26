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
    mtsRequiredInterface *req = AddRequiredInterface("DataGenerator");
    if (req) {
       req->AddFunction("GetData", Generator.GetData);
       req->AddFunction("SetAmplitude", Generator.SetAmplitude);
       req->AddFunction("SetAndGetAmplitude", Generator.SetAndGetAmplitude);
       req->AddFunction("ButtonClicked", Generator.ButtonByCommandVoid);
       // event handlers
       req->AddEventHandlerVoid(
           &displayTask::ButtonEventHandler, this, "ButtonEventHandler", false);
       req->AddEventHandlerWrite(&displayTask::HandleEventWrite, this,
                                 "WriteEvent", false);
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
    UI.Amplitude1->bounds(minValue, maxValue);
    UI.Amplitude1->value(startValue);
    UI.Amplitude2->bounds(minValue, maxValue);
    UI.Amplitude2->value(startValue);
    AmplitudeData = startValue;
    AmplitudeDataForQualifiedRead = startValue;
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
    UI.Data1->value(Data);
    // check if the user has entered a new amplitude in UI
    if (UI.Amplitude1Changed) {
        // retrieve the new amplitude and send it to the sine task
        AmplitudeData = UI.Amplitude1->value();
        AmplitudeData.SetTimestamp(mtsTaskManager::GetInstance()->GetTimeServer().GetRelativeTime());
        AmplitudeData.SetValid(true);
        // send it
        Generator.SetAmplitude(AmplitudeData);
        UI.Amplitude1Changed = false;
        CMN_LOG_CLASS_RUN_VERBOSE << "Run - Write command: " << this->GetTick()
                                  << " - Amplitude: " << AmplitudeData << std::endl;
    }
    if (UI.Amplitude2Changed) {
        // retrieve the new amplitude and send it to the sine task
        AmplitudeDataForQualifiedRead = UI.Amplitude2->value();
        AmplitudeDataForQualifiedRead.SetTimestamp(mtsTaskManager::GetInstance()->GetTimeServer().GetRelativeTime());
        AmplitudeDataForQualifiedRead.SetValid(true);
        // send it
        Generator.SetAndGetAmplitude(AmplitudeDataForQualifiedRead, DataFromQualifiedRead);
        UI.Data2->value(DataFromQualifiedRead);
        UI.Amplitude2Changed = false;
        CMN_LOG_CLASS_RUN_VERBOSE << "Run - QualifiedRead command: " << this->GetTick()
                                  << " - Amplitude: " << AmplitudeDataForQualifiedRead << std::endl;
    }
    // check if a user clicks a button.
    if (UI.Button1Clicked) {
        UI.Button1Clicked = false;
        Generator.ButtonByCommandVoid();
        CMN_LOG_CLASS_RUN_VERBOSE << "Button clicked." << std::endl;
    }
    // log some extra information
    //CMN_LOG_CLASS_RUN_VERBOSE << "Run : " << this->GetTick()
    //                          << " - Data: " << Data << std::endl;
    // update the UI, process UI events 
    if (Fl::check() == 0) {
        Kill();
    }
}

void displayTask::HandleEventWrite(const mtsDouble & value)
{
    //CMN_LOG_RUN_VERBOSE << "HandleTrigger: Trigger event (" << this->GetName() << "): "
    //                    << value << std::endl;

    UI.Data3->value(value.Data);
    //Fl::check();
}

void displayTask::ButtonEventHandler()
{
    if (UI.Button2->value()) {
        UI.Button2->value(false);
    } else {
        UI.Button2->value(true);
    }
    //Fl::check();
}

/*
  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/
