/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
// $Id$

#include <cisstCommon.h>
#include <cisstOSAbstraction.h>
#include <cisstMultiTask.h>

#include "sineTask.h"
#include "clockDevice.h"
#include "displayTask.h"
#include "displayUI.h"

using namespace std;

int main(void)
{
    // log configuration, see previous examples
    cmnLogger::SetLoD(5);
    cmnLogger::GetMultiplexer()->AddChannel(cout, 10);
    cmnLogger::HaltDefaultLog();
    cmnLogger::ResumeDefaultLog(5);
    cmnClassRegister::SetLoD("sineTask", 10);
    cmnClassRegister::SetLoD("displayTask", 10);

    // create our two tasks
    const double PeriodSine = 1 * cmn_ms; // in milliseconds
    const double PeriodDisplay = 50 * cmn_ms; // in milliseconds
    // create the task manager and the tasks/devices
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    sineTask * sineTaskObject =
        new sineTask("SIN", PeriodSine);
    clockDevice * clockDeviceObject =
        new clockDevice("CLOC");
    displayTask * displayTaskObject =
        new displayTask("DISP", PeriodDisplay);
    displayTaskObject->Configure();
    // add the tasks to the task manager and connect them
    taskManager->AddTask(sineTaskObject);
    taskManager->AddDevice(clockDeviceObject);
    taskManager->AddTask(displayTaskObject);
    taskManager->Connect("DISP", "DataGenerator", "SIN", "MainInterface");
    taskManager->Connect("DISP", "Clock", "CLOC", "MainInterface");
    std::ofstream dotFile("example2.dot"); 
    taskManager->ToStreamDot(dotFile);
    dotFile.close();

    taskManager->CreateAll();
    taskManager->StartAll();

    while (1) {
        if (displayTaskObject->GetExitFlag()) {
            break;
        }
    }

    taskManager->KillAll();

    osaSleep(PeriodDisplay * 2);
    while (!sineTaskObject->IsTerminated()) osaSleep(PeriodDisplay);
    while (!displayTaskObject->IsTerminated()) osaSleep(PeriodDisplay);

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
