/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id$ */

#include <cisstVector.h>
#include <cisstOSAbstraction.h>
#include <cisstDevices.h>

#include "displayTask.h"
#include "displayUI.h"

using namespace std;

int main(void)
{
    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::GetMultiplexer()->AddChannel(cout, CMN_LOG_LOD_VERY_VERBOSE);
    // add a log per thread
    osaThreadedLogFile threadedLog("example1-");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);
    // specify a higher, more verbose log level for these classes
    cmnClassRegister::SetLoD("mtsTaskInterface", CMN_LOG_LOD_RUN_ERROR);
    cmnClassRegister::SetLoD("mtsTaskManager", CMN_LOG_LOD_RUN_ERROR);
    cmnClassRegister::SetLoD("devSartoriusSerial", CMN_LOG_LOD_RUN_ERROR);

    // create our two tasks
    const long PeriodDisplay = 10; // in milliseconds
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    displayTask * displayTaskObject =
        new displayTask("Display", PeriodDisplay * cmn_ms);
    displayTaskObject->Configure();
    taskManager->AddTask(displayTaskObject);

    devSartoriusSerial * scaleObject = new devSartoriusSerial("Sartorius", "/dev/tty.KeySerial1");
	taskManager->AddTask(scaleObject);

    // connect the tasks
    taskManager->Connect("Display", "Scale", "Sartorius", "Scale");

    // generate a nice tasks diagram
    std::ofstream dotFile("example1.dot"); 
    taskManager->ToStreamDot(dotFile);
    dotFile.close();

    // create the tasks, i.e. find the commands
    taskManager->CreateAll();
    // start the periodic Run
    taskManager->StartAll();

    // wait until the close button of the UI is pressed
    while (1) {
        osaSleep(100.0 * cmn_ms); // sleep to save CPU
        if (displayTaskObject->GetExitFlag()) {
            std::cout << "Quitting " << std::flush;
            break;
        }
    }
    // cleanup
    taskManager->KillAll();

    osaSleep(PeriodDisplay * 2);
    while (!displayTaskObject->IsTerminated()) {
        osaSleep(PeriodDisplay);
        std::cout << "." << std::flush;
    }
    taskManager->Cleanup();
    return 0;
}

/*
  Author(s):  Anton Deguet
  Created on: 2009-03-27

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/
