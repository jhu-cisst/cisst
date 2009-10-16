/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ali Uneri
  Created on: 2009-10-13

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstDevices/devKeyboard.h>
#include <cisstDevices/devNDiSerial.h>


int main(void)
{
    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("devNDiSerial", CMN_LOG_LOD_VERY_VERBOSE);

    // add a log per thread
    osaThreadedLogFile threadedLog("example6-");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);

    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();

    // create tasks
    devKeyboard * devKeyboardTask = new devKeyboard();
    devNDiSerial * devNDiSerialTask = new devNDiSerial("devNDiSerial", "COM1");

    // add tasks to task manager
    taskManager->AddTask(devKeyboardTask);
    taskManager->AddTask(devNDiSerialTask);

    // create and start all tasks
    taskManager->CreateAll();
    taskManager->StartAll();

    devKeyboardTask->SetQuitKey('q');
    devKeyboardTask->SetDone(true);  // kill the task
    do {
        osaSleep(100.0 * cmn_ms);
    } while (!devKeyboardTask->Done());

    // kill all tasks
    taskManager->KillAll();
    osaSleep(100.0 * cmn_ms);
    taskManager->Cleanup();
    osaSleep(100.0 * cmn_ms);

    CMN_LOG_INIT_WARNING << "main: end of program" << std::endl;

    return 0;
}
