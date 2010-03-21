/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id$ */

/*
  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2010 Johns Hopkins University (JHU), All Rights
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
#include <cisstDevices.h>

#include <cisstDevices/devSensableHD.h>

int main(int argc, char * argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " (global component manager IP)" << std::endl;
        return 1;
    }

    // Set global component manager IP
    const std::string globalComponentManagerIP(argv[1]);
    std::cout << "Global component manager IP: " << globalComponentManagerIP << std::endl;

    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_RUN_ERROR);
    cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);
    // add a log per thread
    osaThreadedLogFile threadedLog("deviceExample1Server");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);
    // specify a higher, more verbose log level for these classes
    cmnClassRegister::SetLoD("mtsTaskInterface", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsTaskManager", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("devSensableHD", CMN_LOG_LOD_VERY_VERBOSE);

    // Get the local component manager
    mtsTaskManager * taskManager;
    try {
        taskManager = mtsTaskManager::GetInstance(globalComponentManagerIP, "ProcDisp");
    } catch (...) {
        CMN_LOG_INIT_ERROR << "Failed to initialize local component manager" << std::endl;
        return 1;
    }
    /* If there are more than one network interfaces installed on this machine
       and an user wants to specify one of them to use, the following codes can
       be used:
        
       std::vector<std::string> ipAddresses = mtsTaskManager::GetIPAddressList();
       std::string thisProcessIP = ipAddresses[i];  // i=[0, ipAddresses.size()-1]

       mtsTaskManager * taskManager = mtsTaskManager::GetInstance(globalComponentManagerIP, "P2", thisProcessIP);
    */

    // create our server task
    devSensableHD * robotObject = new devSensableHD("Omni", "Omni1", true);
    taskManager->AddComponent(robotObject);
    
    // create the tasks, i.e. find the commands
    taskManager->CreateAll();
    // start the periodic Run
    taskManager->StartAll();
    
    while (1) {
        osaSleep(10 * cmn_ms);
    }
    
    // cleanup
    taskManager->KillAll();
    taskManager->Cleanup();
    
    return 0;
}

