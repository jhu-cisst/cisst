/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: displayTask.cpp,v 1.15 2008/12/14 06:43:16 pkaz Exp $ */

#include <math.h>
#include "displayTask.h"
#include "displayUI.h"

CMN_IMPLEMENT_SERVICES(displayTask);

displayTask::displayTask(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 5000),
    ExitFlag(false)
#ifdef CISST_GETVECTOR
    ,DataVec(10)
#endif
{
    // to communicate with the interface of the resource
    mtsRequiredInterface *req = AddRequiredInterface("DataGenerator");
    if (req) {
       req->AddFunction("GetData", Generator.GetData);
       req->AddFunction("GetStateIndex", Generator.GetStateIndex);
#ifdef CISST_GETVECTOR
       req->AddFunction("GetDataVector", Generator.GetDataVector);
#endif
       req->AddFunction("SetAmplitude", Generator.SetAmplitude);
    }
}

void displayTask::Configure(const std::string & CMN_UNUSED(filename))
{
    // define some values, ideally these come from a configuration
    // file and then configure the user interface
    double maxValue = 0.5; double minValue = 5.0;
    StartValue =  1.0;
    CMN_LOG_CLASS(3) << "Configure: setting bounds to: "
                     << minValue << ", " << maxValue << std::endl;
    CMN_LOG_CLASS(3) << "Configure: setting start value to: "
                     << StartValue << std::endl;
    UI.Amplitude->bounds(minValue, maxValue);
    UI.Amplitude->value(StartValue);
    AmplitudeData.Data = StartValue;
}

void displayTask::Startup(void) 
{
    // set the initial amplitude based on the configuration
    AmplitudeData.Data = StartValue;

    // make the UI visible
    UI.show(0, NULL);
}

void displayTask::Run(void)
{
    // get the current time index to display it in the UI
    const mtsStateIndex now = StateTable.GetIndexWriter();
    // get the data from the sine wave generator task
    Generator.GetData(Data);
    Generator.GetStateIndex(StateIndex);
    UI.Data->value(Data.Data);
#ifdef CISST_GETVECTOR
    Generator.GetDataVector(StateIndex, DataVec);
#endif
    // check if the user has entered a new amplitude in UI
    if (UI.AmplitudeChanged) {
        // retrieve the new amplitude and send it to the sine task
        AmplitudeData.Data = UI.Amplitude->value();
        Generator.SetAmplitude(AmplitudeData);
        UI.AmplitudeChanged = false;
        CMN_LOG_CLASS(7) << "Run: " << now.Ticks()
                         << " - Amplitude: " << AmplitudeData.Data << std::endl;
    }
    // log some extra information
    CMN_LOG_CLASS(7) << "Run : " << now.Ticks()
                     << " - Data: " << Data << std::endl;
#ifdef CISST_GETVECTOR
    CMN_LOG_CLASS(7) << "Last 10: " << DataVec << std::endl;
#endif
    // update the UI, process UI events 
    if (Fl::check() == 0) {
        ExitFlag = true;
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
