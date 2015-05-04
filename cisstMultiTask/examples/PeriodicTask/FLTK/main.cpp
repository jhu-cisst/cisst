/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/*

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

#include <components/sineTask.h>
#include <components/clockComponent.h>

#include <cisstOSAbstraction/osaSleep.h>

#include "displayTask.h"

int main(int CMN_UNUSED(argc), char ** CMN_UNUSED(argv))
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
    mtsComponentManager * componentManager = mtsComponentManager::GetInstance();
    sineTask * sineInstance = new sineTask("Sine", PeriodSine);
    clockComponent * clockInstance = new clockComponent("Clock");
    displayTask * uiInstance = new displayTask("Display");

    // add the tasks to the component manager
    componentManager->AddComponent(sineInstance);
    componentManager->AddComponent(clockInstance);
    componentManager->AddComponent(uiInstance);

    // connect the components, task.RequiresInterface -> task.ProvidesInterface
    componentManager->Connect("Display", "DataGenerator", "Sine", "MainInterface");
    componentManager->Connect("Display", "TimeGenerator", "Clock", "MainInterface");

    // create the components, i.e. find the commands
    componentManager->CreateAll();
    componentManager->WaitForStateAll(mtsComponentState::READY, 2.0 * cmn_s);

    // start the periodic Run
    componentManager->StartAll();
    componentManager->WaitForStateAll(mtsComponentState::ACTIVE, 2.0 * cmn_s);

    // Start FLTK run loop (i.e., call Fl::run). Note that calling Fl::run directly
    // might not work; for example, if static FLTK libraries are used and displayTask
    // is dynamically loaded, then (on Windows) this program and displayTask will have
    // different copies of Fl::run (seems to work fine on Linux).
    displayTask::StartRunLoop();

    // cleanup
    componentManager->KillAll();
    componentManager->WaitForStateAll(mtsComponentState::FINISHED, 2.0 * cmn_s);

    componentManager->Cleanup();

    delete uiInstance;
    delete clockInstance;
    delete sineInstance;

    // stop all logs
    cmnLogger::SetMask(CMN_LOG_ALLOW_NONE);

    return 0;
}
