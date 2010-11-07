/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: serverMain.cpp 1660 2010-07-22 20:22:54Z adeguet1 $ */

/*
  Author(s):  Min Yang Jung
  Created on: 2010-09-01

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

#include "ManagerComponent.h"

int main(int argc, char * argv[])
{
    // Set global component manager IP
    std::string globalComponentManagerIP;
    if (argc == 1) {
        globalComponentManagerIP = "localhost";
    } else if (argc == 2) {
        globalComponentManagerIP = argv[1];
    } else {
        std::cerr << "Usage: " << argv[0] << " (global component manager IP)" << std::endl;
        return 1;
    }
    std::cout << "Global component manager IP: " << globalComponentManagerIP << std::endl;

    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);
    // add a log per thread
    //osaThreadedLogFile threadedLog("ProcessControl");
    //cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);

    // Get local component manager instance
    mtsManagerLocal * ComponentManager;
    try {
        ComponentManager = mtsManagerLocal::GetInstance(globalComponentManagerIP, "ProcessControl");
    } catch (...) {
        CMN_LOG_INIT_ERROR << "Failed to initialize local component manager" << std::endl;
        return 1;
    }

    // create our server task
    ManagerComponent * managerComponent = new ManagerComponent("ManagerComponent", 1 * cmn_s);
    if (!ComponentManager->AddComponent(managerComponent)) {
        std::cerr << "Failed to add component: " << managerComponent->GetName() << std::endl;
        return 1;
    }

    // create the tasks, i.e. find the commands
    ComponentManager->CreateAll();
    // start the periodic Run
    ComponentManager->StartAll();
    
    while (1) {
        osaSleep(100 * cmn_ms);
    }
    
    // cleanup
    ComponentManager->KillAll();
    ComponentManager->Cleanup();

    return 0;
}

