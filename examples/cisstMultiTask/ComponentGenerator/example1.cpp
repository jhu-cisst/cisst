/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id$ */

#include <cisstCommon.h>
#include <cisstOSAbstraction.h>
#include <cisstMultiTask.h>

#include "sineTask.h"
#include "displayTask.h"
#include "displayUI.h"

using namespace std;

int main(void)
{
    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    // add a log per thread
    osaThreadedLogFile threadedLog("example1-");
    cmnLogger::AddChannel(threadedLog, CMN_LOG_ALLOW_ALL);
    // specify a higher, more verbose log level for these classes
    cmnLogger::SetMaskClassAll(CMN_LOG_ALLOW_ALL);

    // create our two tasks
    const double PeriodSine = 1 * cmn_ms; // in milliseconds
    const double PeriodDisplay = 50 * cmn_ms; // in milliseconds
    mtsComponentManager * componentManager = mtsComponentManager::GetInstance();
    sineTask * sineTaskObject = new sineTask("SIN", PeriodSine);
    displayTask * displayTaskObject = new displayTask("DISP", PeriodDisplay);
    displayTaskObject->Configure();

    // add the tasks to the component manager
    componentManager->AddComponent(sineTaskObject);
    componentManager->AddComponent(displayTaskObject);

    // connect the components, task.RequiresInterface -> task.ProvidesInterface
    componentManager->Connect("DISP", "DataGenerator", "SIN", "MainInterface");

    // generate a nice components diagram
    std::ofstream dotFile("example1.dot");
    componentManager->ToStreamDot(dotFile);
    dotFile.close();

    // create the components, i.e. find the commands
    componentManager->CreateAll();
    // start the periodic Run
    componentManager->StartAll();

    // wait until the close button of the UI is pressed
    while (!displayTaskObject->IsTerminated()) {
        osaSleep(100.0 * cmn_ms); // sleep to save CPU
    }
    // cleanup
    componentManager->KillAll();

    osaSleep(PeriodSine * 2);
    while (!sineTaskObject->IsTerminated()) osaSleep(PeriodSine);

    componentManager->Cleanup();
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
