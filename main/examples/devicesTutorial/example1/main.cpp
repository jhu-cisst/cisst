/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: main.cpp 783 2009-09-01 14:54:15Z adeguet1 $ */

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
    cmnClassRegister::SetLoD("mtsTaskInterface", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsTaskManager", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("devSensableHD", CMN_LOG_LOD_VERY_VERBOSE);

    // create our two tasks
    const double PeriodDisplay = 10.0; // in milliseconds
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    displayTask * displayTaskObject =
        new displayTask("DISP", PeriodDisplay * cmn_ms);
    displayTaskObject->Configure();
    taskManager->AddTask(displayTaskObject);

#if (CISST_DEV_HAS_SENSABLEHD == ON)
    // name as defined in Sensable configuration
    std::string omniName("Omni1");
    devSensableHD * robotObject = new devSensableHD("Omni", "Omni1");
	taskManager->AddTask(robotObject);

    // connect the tasks
    taskManager->Connect("DISP", "Robot", "Omni", omniName);
    taskManager->Connect("DISP", "Button1", "Omni", omniName + "Button1");
    taskManager->Connect("DISP", "Button2", "Omni", omniName + "Button2");
#endif

    // generate a nice tasks diagram
    std::ofstream dotFile("example1.dot"); 
    taskManager->ToStreamDot(dotFile);
    dotFile.close();

    // collect all state data in csv file
    mtsCollectorState * collector =
        new mtsCollectorState("Omni", mtsCollectorBase::COLLECTOR_LOG_FORMAT_CSV);
    collector->AddSignal(); // all signals
    taskManager->AddTask(collector);

    // create the tasks, i.e. find the commands
    taskManager->CreateAll();
    // start the periodic Run
    taskManager->StartAll();

    collector->Start(0.0 * cmn_s);

    // wait until the close button of the UI is pressed
    while (true) {
        osaSleep(10.0 * cmn_ms); // sleep to save CPU
        if (displayTaskObject->GetExitFlag()) {
            break;
        }
    }
    // cleanup
    taskManager->KillAll();

    osaSleep(PeriodDisplay * 2);
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
