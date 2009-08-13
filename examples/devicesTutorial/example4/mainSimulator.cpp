/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
 $Id$

 Author(s): Ali Uneri

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
    // add a log per thread
    osaThreadedLogFile threadedLog("example4-");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);

    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();

    // create instances
    devOpenIGTLink * devOpenIGTLinkObj = new devOpenIGTLink("client", 50.0 * cmn_ms, "127.0.0.1", 18944);
    trackerSimulator * trackerSimulatorObj = new trackerSimulator("trackerSimulator", 50.0 * cmn_ms);

    // add instances to task manager
    taskManager->AddTask(devOpenIGTLinkObj);
    taskManager->AddTask(trackerSimulatorObj);

    // connect tasks
    taskManager->Connect(devOpenIGTLinkObj->GetName(), "CartesianPosition",
                         trackerSimulatorObj->GetName(), "CartesianPosition");

    // create and start tasks
    taskManager->CreateAll();
    taskManager->StartAll();

    // loop until GUI exits
    while (!trackerSimulatorObj->GetExitFlag()) {
        osaSleep(100.0 * cmn_ms);
    }

    // kill tasks
    taskManager->KillAll();
    while (!trackerSimulatorObj->IsTerminated()) {
        osaSleep(250.0 * cmn_ms);
    }

    return 0;
}
