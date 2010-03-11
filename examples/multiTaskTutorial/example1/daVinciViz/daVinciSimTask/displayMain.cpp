/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/*
  $Id: DisplayTaskMain.cpp 

  Author(s): Tian Xia 
  Created on: 2010-03-01 

*/ 

#include <cisstConfig.h>

#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask.h>

#include "displayTask.h" 

int main(int argc, char * argv[]) 
{
    if(argc != 2) {
        std::cerr << "Usage: " << argv[0] << " (global component manager IP)" << std::endl;
        return 1;
    }
    
    // Set global component manager IP
    const std::string globalComponentManagerIP(argv[1]);

    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_RUN_ERROR);
    cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);

    // add a log per thread
    osaThreadedLogFile threadedLog("PyQtEmbeddedDisplayTaskMain-");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);

    // set the log level of detail on select tasks
    cmnClassRegister::SetLoD("mtsTaskInterface", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsTaskManager", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("displayTask", CMN_LOG_LOD_VERY_VERBOSE);

    // Get the TaskManager instance and set operation mode
    // Get the local component manager
    mtsManagerLocal * localManager;
    try {
        std::cout<<"Global Component Manager IP "<<globalComponentManagerIP<<std::endl; 
        localManager = mtsManagerLocal::GetInstance(globalComponentManagerIP, "DISP");
    } catch (...) {
        CMN_LOG_INIT_ERROR << "Failed to initialize local component manager" << std::endl;
        return 1;
    }

    const double PeriodDisplay = 50 * cmn_ms; // in milliseconds
    displayTask * displayTaskObject = new displayTask("DISP", PeriodDisplay);
    localManager->AddComponent(displayTaskObject);

    /* 
    // Connect the componens across networks
    if (!localManager->Connect("DISP", "DISP", "Required",
                               "SIN", "SIN", "Provided")) {
        CMN_LOG_INIT_ERROR << "Connect failed" << std::endl;
        return 1;
    }
    */ 

    // create and start all tasks
    localManager->CreateAll();
    localManager->StartAll();
    
    while(!displayTaskObject->IsTerminated()) { 
        osaSleep(100.0 * cmn_ms);   
    }

    // kill all tasks and perform cleanup
    localManager->KillAll();
    localManager->Cleanup();

    return 0;
}
