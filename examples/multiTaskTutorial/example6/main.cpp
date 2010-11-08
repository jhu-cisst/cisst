/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id$ */

#include <cisstCommon.h>
#include <cisstOSAbstraction.h>
#include <cisstMultiTask.h>

#include "robotLowLevel.h"
#include "userInterface.h"

using namespace std;

int main(void)
{
    // Log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::GetMultiplexer()->AddChannel(cout, CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::HaltDefaultLog();
    cmnLogger::ResumeDefaultLog(CMN_LOG_LOD_VERY_VERBOSE);
    // add a log per thread
    osaThreadedLogFile threadedLog("example6-");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);

    // create a single task
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    robotLowLevel * robotTask = new robotLowLevel("RobotControl", 100 * cmn_ms);

    // add single task, do not connect anything
    taskManager->AddComponent(robotTask);
    // create the thread for the task
    taskManager->CreateAll();

    // look for the interface we are going to use
    mtsInterfaceProvided * robotInterface = robotTask->GetInterfaceProvided("Robot1");
    userInterface * UI = 0;
    if (robotInterface) {
        // instantiate the UI in the current thread
        UI = new userInterface("Robot1", robotInterface);
    } else {
        std::cerr << "It looks like there is no \"Robot1\" interface" << std::endl;
    }

    // start the task
    taskManager->StartAll();

    // loop while the UI did not get a close request
    while (!UI->CloseRequested) {
        // the user interface method Update is equivalent to the task
        // Run method
        UI->Update();
        osaSleep(10 * cmn_ms); // ask for an update every 10 ms
    }

    // stop and cleanup
    taskManager->KillAll();
    if (UI) {
        delete UI;
    }

    taskManager->Cleanup();
    return 0;
}

/*
  Author(s):  Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/
