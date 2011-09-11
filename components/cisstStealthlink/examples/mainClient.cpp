/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: main.cpp 2847 2011-08-19 22:06:16Z adeguet1 $

  Author(s):  Peter Kazanzides, Anton Deguet
  Created on: 2011-07-14

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstStealthlink/mtsStealthlink.h>
#include "mtsStealthlinkExampleComponent.h"

int main(int argc, char * argv[])
{
    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    // get all message to log file
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    // get only errors and warnings to std::cout
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    // specify a higher, more verbose log level for these classes
    cmnLogger::SetMaskClassMatching("mts", CMN_LOG_ALLOW_ALL);

    // enable system-wide thread-safe logging
#ifdef MTS_LOGGING
    mtsManagerLocal::SetLogForwarding(true);
#endif

    std::string globalComponentManagerIP;

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " [GlobalManagerIP] [flag]" << std::endl;
        std::cerr << "       GlobalManagerIP is set as 127.0.0.1 by default" << std::endl;
    }

    // Set global component manager's ip and argument type
    // If flag is not specified, or not 1, then use generic type (mtsDouble)
    if (argc == 1) {
        globalComponentManagerIP = "localhost";
    } else if (argc == 2) {
        globalComponentManagerIP = argv[1];
    } else {
        exit(-1);
    }

    std::cout << "Starting server, IP = " << globalComponentManagerIP << std::endl;

    // Get the TaskManager instance and set operation mode
    mtsManagerLocal * componentManager;
    try {
        componentManager = mtsManagerLocal::GetInstance(globalComponentManagerIP, "ProcessClient");
    } catch (...) {
        CMN_LOG_INIT_ERROR << "Failed to initialize local component manager" << std::endl;
        return 1;
    }

    //client
    mtsStealthlinkExampleComponent * componentExample = new mtsStealthlinkExampleComponent("Example", 50 * cmn_ms);

    // add the components to the component manager
    componentManager->AddComponent(componentExample);

    // Connect the test component to the Stealthlink component
    if (!componentManager->Connect("ProcessClient", componentExample->GetName(), "Stealthlink",
                                   "ProcessServer", "Stealthlink", "Controller")) {
        CMN_LOG_INIT_ERROR << "Could not connect test component to Stealthlink component." << std::endl;
        return 0;
    }

    // Now, connect to the tools (we assume these are pre-defined in the XML file)
    if (!componentManager->Connect("ProcessClient", componentExample->GetName(), "Pointer",
                                   "ProcessServer", "Stealthlink", "Pointer")) {
        CMN_LOG_INIT_WARNING << "Could not connect test component to Pointer tool." << std::endl;
    }
    if (!componentManager->Connect("ProcessClient", componentExample->GetName(), "Frame",
                                   "ProcessServer", "Stealthlink",  "Frame")) {
        CMN_LOG_INIT_WARNING << "Could not connect test component to Frame tool." << std::endl;
    }

    // Connect the registration interfaces
    if (!componentManager->Connect("ProcessClient", componentExample->GetName(), "Registration",
                                   "ProcessServer", "Stealthlink",  "Registration")) {
        CMN_LOG_INIT_ERROR << "Could not connect test component to Registration interface." << std::endl;
        return 0;
    }

    // create the tasks, i.e. find the commands
    componentManager->CreateAll();
    componentManager->WaitForStateAll(mtsComponentState::READY);

    // start the periodic Run
    componentManager->StartAll();
    componentManager->WaitForStateAll(mtsComponentState::ACTIVE);

    bool GCMActive = true;
    while (GCMActive) {
        osaSleep(5.0 * cmn_ms);
        GCMActive = componentManager->IsGCMActive();
    }

    if (!GCMActive) {
        CMN_LOG_RUN_ERROR << "Global Component Manager is disconnected" << std::endl;
    }

    // cleanup
    componentManager->KillAll();
    componentManager->WaitForStateAll(mtsComponentState::FINISHED, 20.0 * cmn_s);

    // delete components
    delete componentExample;

    componentManager->Cleanup();

    return 0;
}

