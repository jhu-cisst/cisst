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

    // this is not great if one has both sensable and novint, we pick sensable by default
#if CISST_DEV_HAS_SENSABLEHD
    // name as defined in Sensable configuration
    std::string omniName("Omni1");
    devSensableHD * robotObject = new devSensableHD("Omni", "Omni1");
	taskManager->AddTask(robotObject);

    // connect the tasks
    taskManager->Connect("DISP", "Robot", "Omni", omniName);
    taskManager->Connect("DISP", "Button1", "Omni", omniName + "Button1");
    taskManager->Connect("DISP", "Button2", "Omni", omniName + "Button2");
#elif CISST_DEV_HAS_NOVINTHDL
    // name as defined in Sensable configuration
    devNovintHDL * robotObject = new devNovintHDL("Novint", "Novint");
	taskManager->AddTask(robotObject);

    // connect the tasks
    taskManager->Connect("DISP", "Robot", "Novint", "Novint");
    taskManager->Connect("DISP", "Button1", "Novint", "NovintButton1");
    taskManager->Connect("DISP", "Button2", "Novint", "NovintButton2");
#endif
    // collect all state data in csv file
    mtsCollectorState * collector =
        new mtsCollectorState(robotObject->GetName(),
                              robotObject->GetDefaultStateTableName(),
                              mtsCollectorBase::COLLECTOR_FILE_FORMAT_CSV);
    collector->AddSignal(); // all signals
    collector->Connect();
    taskManager->AddTask(collector);

    // create the tasks, i.e. find the commands
    taskManager->CreateAll();
    // start the periodic Run
    taskManager->StartAll();

    collector->StartCollection(0.0 * cmn_ms);

    // wait until the close button of the UI is pressed
    while (true) {
        osaSleep(10.0 * cmn_ms); // sleep to save CPU
        if (displayTaskObject->GetExitFlag()) {
            break;
        }
    }

    collector->StopCollection(0.0 * cmn_ms);
    osaSleep(10 * cmn_ms);

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
