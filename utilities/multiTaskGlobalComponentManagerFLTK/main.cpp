/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: main.cpp 682 2009-08-14 03:18:33Z adeguet1 $
  
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
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);
    // add a log per thread
    osaThreadedLogFile threadedLog("GlobalComponentManagerFLTK");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);
    // specify a higher, more verbose log level for these classes
    cmnClassRegister::SetLoD("mtsManagerGlobal", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("GCMUITask", CMN_LOG_LOD_VERY_VERBOSE);

    // Create and start global component manager that serves local component
    // managers running across networks.
    mtsManagerGlobal globalComponentManager;
    if (!globalComponentManager.StartServer()) {
        CMN_LOG_INIT_ERROR << "Failed to start global component manager." << std::endl;
        return 1;
    }
    CMN_LOG_INIT_VERBOSE << "Global component manager started successfully." << std::endl;

    // Get the local component manager as standalone mode
    mtsTaskManager * taskManager;
    try {
        taskManager = mtsTaskManager::GetInstance();
    } catch (...) {
        CMN_LOG_INIT_ERROR << "Failed to initialize local component manager" << std::endl;
        return 1;
    }

    // Create GCM UI task
    const double period = 1 * cmn_ms;
    GCMUITask * GCMUITaskObject = new GCMUITask("GCMUI", period, globalComponentManager);
    GCMUITaskObject->Configure();
    taskManager->AddComponent(GCMUITaskObject);

    // Create task and start local component manager
    taskManager->CreateAll();
    taskManager->StartAll();


    // Wait until the close button of the UI is pressed
    while (!GCMUITaskObject->IsTerminated()) {
        osaSleep(10 * cmn_ms);
    }

    // Cleanup local component manager
    taskManager->KillAll();
    taskManager->Cleanup();

    // Cleanup global component manager
    if (!globalComponentManager.StopServer()) {
        CMN_LOG_RUN_ERROR << "Failed to stop global component manager." << std::endl;
    }

    return 0;
}
