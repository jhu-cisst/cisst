/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstCommon.h>
#include <cisstOSAbstraction.h>
#include <cisstMultiTask.h>

#include <components/sineTask.h>
#include <components/clockComponent.h>

#include "displayTask.h"
#include "displayUI.h"

int main(void)
{
    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    // get all messages to log file
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    // specify a higher, more verbose log level for these classes
    cmnLogger::SetMaskClass("sineTask", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("displayTask", CMN_LOG_ALLOW_ALL);

    // create our two tasks
    const double PeriodSine = 5.0 * cmn_ms; // in milliseconds
    const double PeriodDisplay = 50 * cmn_ms; // in milliseconds
    mtsComponentManager * componentManager = mtsComponentManager::GetInstance();
    sineTask * sineIntance = new sineTask("Sine", PeriodSine);
    clockComponent * clockInstance = new clockComponent("Clock");
    displayTask * displayInstance = new displayTask("Display", PeriodDisplay);

    displayInstance->Configure();

    // add the tasks to the component manager
    componentManager->AddComponent(sineIntance);
    componentManager->AddComponent(clockInstance);
    componentManager->AddComponent(displayInstance);

    // connect the components, task.RequiresInterface -> task.ProvidesInterface
    componentManager->Connect("Display", "DataGenerator", "Sine", "MainInterface");
    componentManager->Connect("Display", "TimeGenerator", "Clock", "MainInterface");

    // create the components, i.e. find the commands
    componentManager->CreateAll();
    componentManager->WaitForStateAll(mtsComponentState::READY);

    // start the periodic Run
    componentManager->StartAll();
    componentManager->WaitForStateAll(mtsComponentState::ACTIVE);

    // wait until the close button of the UI is pressed
    while (!displayInstance->IsTerminated()) {
        displayInstance->UpdateUI(); // this has to be done by main thread
        osaSleep(5.0 * cmn_ms); // sleep to save CPU
    }
    // cleanup
    componentManager->KillAll();
    componentManager->WaitForStateAll(mtsComponentState::FINISHED, 2.0 * cmn_s);

    componentManager->Cleanup();

    delete clockInstance;
    delete sineIntance;
    delete displayInstance;

    return 0;
}
