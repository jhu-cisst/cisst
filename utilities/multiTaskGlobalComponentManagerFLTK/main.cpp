/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):  Min Yang Jung
  Created on: 2010-02-25

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/
#include <cisstCommon.h>
#include <cisstOSAbstraction.h>
#include <cisstMultiTask.h>

#include "GCMUITask.h"

int main(void)
{
    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    // specify a higher, more verbose log level for these classes
    cmnLogger::SetMaskClass("mtsManagerGlobal", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("GCMUITask", CMN_LOG_ALLOW_ALL);

    // Create and start global component manager that serves local component
    // managers running across networks.
    mtsManagerGlobal * globalComponentManager = new mtsManagerGlobal;
    if (!globalComponentManager->StartServer()) {
        CMN_LOG_INIT_ERROR << "Failed to start global component manager." << std::endl;
        return 1;
    }
    CMN_LOG_INIT_VERBOSE << "Global component manager started successfully." << std::endl;

    // Get the local component manager as "networked with GCM" mode
    mtsTaskManager * taskManager;
    try {
        taskManager = mtsTaskManager::GetInstance(*globalComponentManager);
    } catch (...) {
        CMN_LOG_INIT_ERROR << "Failed to initialize local component manager" << std::endl;
        return 1;
    }

    // Create GCM UI task
    const double period = 1 * cmn_ms;
    GCMUITask * GCMUITaskObject = new GCMUITask("GCMUI", period, *globalComponentManager);
    GCMUITaskObject->Configure();
    taskManager->AddComponent(GCMUITaskObject);

    // Create task and start local component manager
    taskManager->CreateAll();
    taskManager->StartAll();


    // Wait until the close button of the UI is pressed
    while (!GCMUITaskObject->IsTerminated()) {
        osaSleep(10 * cmn_ms);
    }

    // Cleanup global component manager
    if (!globalComponentManager->StopServer()) {
        CMN_LOG_RUN_ERROR << "Failed to stop global component manager." << std::endl;
    }

    // Cleanup local component manager
    taskManager->KillAll();
    taskManager->Cleanup();

    return 0;
}
