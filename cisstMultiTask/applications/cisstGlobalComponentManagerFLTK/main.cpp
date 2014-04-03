/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Min Yang Jung
  Created on: 2010-02-25

  (C) Copyright 2010-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnLogger.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstMultiTask/mtsManagerGlobal.h>
#include <cisstMultiTask/mtsManagerLocal.h>

#include "GCMUITask.h"

int main(void)
{
    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    // get all message to log file
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    // get only errors and warnings to std::cout
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    // specify a higher, more verbose log level for these classes
    cmnLogger::SetMaskClass("mtsManagerGlobal", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClassMatching("GCMUITask", CMN_LOG_ALLOW_ALL);
    // enable system-wide logger
    mtsManagerLocal::SetLogForwarding(true);

    // Create and start global component manager
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
    const double period = 50 * cmn_ms; // if this value changes, update auto refresh rate of GCMUITask::Run()
    GCMUITask * GCMUITaskObject = new GCMUITask("GCMUI", period, *globalComponentManager);
    GCMUITaskObject->Configure();
    taskManager->AddComponent(GCMUITaskObject);

    // StatusMonitor - system-wide logging
    CONNECT_LOCAL(GCMUITaskObject->GetName(),
                  mtsManagerComponentBase::InterfaceNames::InterfaceSystemLoggerRequired,
                  mtsManagerComponentBase::ComponentNames::ManagerComponentServer,
                  mtsManagerComponentBase::InterfaceNames::InterfaceSystemLoggerProvided);

    // Create task and start local component manager
    taskManager->CreateAll();
    taskManager->WaitForStateAll(mtsComponentState::READY);

    taskManager->StartAll();
    taskManager->WaitForStateAll(mtsComponentState::ACTIVE);

    // Wait until the close button of the UI is pressed
    while (GCMUITaskObject->UIOpened()) {
        Fl::lock();
        {
            Fl::check();
        }
        Fl::unlock();
        Fl::awake();

        osaSleep(20 * cmn_ms);
    }

    // Cleanup global component manager
    if (!globalComponentManager->StopServer()) {
        CMN_LOG_RUN_ERROR << "Failed to stop global component manager." << std::endl;
    }

    // Cleanup local component manager
    taskManager->KillAll();
    taskManager->WaitForStateAll(mtsComponentState::FINISHED, 5.0 * cmn_s);

    delete GCMUITaskObject;

    taskManager->Cleanup();

    return 0;
}
