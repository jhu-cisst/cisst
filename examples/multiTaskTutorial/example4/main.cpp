/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
// $Id$

#include <cisstCommon.h>
#include <cisstOSAbstraction.h>
#include <cisstInteractive.h>
#include <cisstMultiTask.h>

#include <Python.h>

#include "sineTask.h"
#include "clockDevice.h"
#include "displayTask.h"
#include "displayUI.h"

using namespace std;

int main(int argc, char **argv)
{
    // log configuration, see previous examples
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL); // for cisstLog.txt
    cmnLogger::SetMaskClass("sineTask", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("displayTask", CMN_LOG_ALLOW_ALL);

    // create our two tasks
    const double PeriodSine = 1 * cmn_ms; // in milliseconds
    const double PeriodDisplay = 50 * cmn_ms; // in milliseconds
    // create the task manager and the tasks/devices
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    cmnObjectRegister::Register("TaskManager", taskManager);

    sineTask * sineTaskObject =
        new sineTask("SIN", PeriodSine);
    clockDevice * clockDeviceObject =
        new clockDevice("CLOC");
    displayTask * displayTaskObject =
        new displayTask("DISP", PeriodDisplay);
    displayTaskObject->Configure();
    // add the tasks to the task manager and connect them
    taskManager->AddComponent(sineTaskObject);
    taskManager->AddComponent(clockDeviceObject);
    taskManager->AddComponent(displayTaskObject);
    taskManager->Connect("DISP", "DataGenerator", "SIN", "MainInterface");
    taskManager->Connect("DISP", "Clock", "CLOC", "MainInterface");

    ireTask *ire = new ireTask("IRE", "from example4 import *");
    taskManager->AddComponent(ire);

    // create and add Component Viewer
    mtsComponentViewer * componentViewer = new mtsComponentViewer("ComponentViewer");
    taskManager->AddComponent(componentViewer);

    taskManager->CreateAll();
    taskManager->StartAll();

    // Loop until IRE and display task are both exited
    while (!ire->IsTerminated() || !displayTaskObject->IsTerminated())
        osaSleep(0.5 * cmn_s);  // Wait 0.5 seconds

    osaSleep(PeriodDisplay * 2);
    sineTaskObject->Kill();
    displayTaskObject->Kill();
    osaSleep(PeriodDisplay * 2);
    while (!sineTaskObject->IsTerminated()) osaSleep(PeriodDisplay);
    while (!displayTaskObject->IsTerminated()) osaSleep(PeriodDisplay);

    taskManager->Cleanup();
    return 0;
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
