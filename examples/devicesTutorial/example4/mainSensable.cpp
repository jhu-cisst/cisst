/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ali Uneri
  Created on: 2009-08-13

  (C) Copyright 2007-2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon.h>
#include <cisstOSAbstraction.h>
#include <cisstMultiTask.h>
#include <cisstDevices.h>

#define IS_SERVER true  // make this false to run as a client

int main()
{
    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("devOpenIGTLink", CMN_LOG_LOD_VERY_VERBOSE);

    // add a log per thread
    osaThreadedLogFile threadedLog("example4-");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);

    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();

    // create tasks
#if IS_SERVER
    devOpenIGTLink * devOpenIGTLinkTask = new devOpenIGTLink("trackerServer", 50.0 * cmn_ms, 18944);
#else
    devOpenIGTLink * devOpenIGTLinkTask = new devOpenIGTLink("trackerClient", 50.0 * cmn_ms, "localhost", 18944);
#endif
    devSensableHD * devSensableHDTask = new devSensableHD("devSensableHD", "Omni1");

    // add instances to task manager
    taskManager->AddTask(devOpenIGTLinkTask);
    taskManager->AddTask(devSensableHDTask);

    // connect tasks
    taskManager->Connect(devOpenIGTLinkTask->GetName(), "CartesianPosition",
                         devSensableHDTask->GetName(), "Omni1");

    // create and start all tasks
    taskManager->CreateAll();
    taskManager->StartAll();

    std::cout << "Press 'q' to quit." << std::endl;
    char command;
    do {
        command = cmnGetChar();
        osaSleep(10.0 * cmn_ms);
    } while (command != 'q');

    // kill all tasks
    taskManager->KillAll();
    while (!devSensableHDTask->IsTerminated()) {
        osaSleep(100.0 * cmn_ms);
    }
    taskManager->Cleanup;
    return 0;
}
