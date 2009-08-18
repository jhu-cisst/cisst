/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ali Uneri
  Created on: 2009-08-11

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

#include "trackerSimulator.h"

int main()
{
    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("devOpenIGTLinkServer", CMN_LOG_LOD_RUN_VERBOSE);
    // add a log per thread
    osaThreadedLogFile threadedLog("example4-");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);

    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();

    // create tasks
    devOpenIGTLinkServer * devOpenIGTLinkServerTask = new devOpenIGTLinkServer("trackerServer", 50.0 * cmn_ms, 18944);
    trackerSimulator * trackerSimulatorTask = new trackerSimulator("trackerSimulator", 50.0 * cmn_ms);

    // add instances to task manager
    taskManager->AddTask(devOpenIGTLinkServerTask);
    taskManager->AddTask(trackerSimulatorTask);

    // connect tasks
    taskManager->Connect(devOpenIGTLinkServerTask->GetName(), "PositionCartesian",
                         trackerSimulatorTask->GetName(), "PositionCartesian");

    // create and start all tasks
    taskManager->CreateAll();
    taskManager->StartAll();

    // loop until GUI exits
    while (!trackerSimulatorTask->GetExitFlag()) {
        osaSleep(100.0 * cmn_ms);
    }

    // kill all tasks
    taskManager->KillAll();
    while (!trackerSimulatorTask->IsTerminated()) {
        osaSleep(100.0 * cmn_ms);
    }

    return 0;
}
