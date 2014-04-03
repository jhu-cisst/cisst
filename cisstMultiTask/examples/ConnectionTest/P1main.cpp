/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstOSAbstraction/osaSleep.h>

#include "C1Task.h"
#include "C2ClientTask.h"

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
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    // add a log per thread
    //osaThreadedLogFile threadedLog("P1");
    //cmnLogger::AddChannel(threadedLog, CMN_LOG_ALLOW_ALL);

    // Get local component manager instance
    mtsManagerLocal * localManager;
    try {
        localManager = mtsManagerLocal::GetInstance(globalComponentManagerIP, "P1");
    } catch (...) {
        CMN_LOG_INIT_ERROR << "Failed to initialize local component manager" << std::endl;
        return 1;
    }
    /* If there are more than one network interfaces installed on this machine
       and an user wants to specify one of them to use, the following codes can
       be used:
        
       std::vector<std::string> ipAddresses = mtsManagerLocal::GetIPAddressList();
       std::string thisProcessIP = ipAddresses[i];  // i=[0, ipAddresses.size()-1]

       mtsManagerLocal * localManager = mtsManagerLocal::GetInstance(globalComponentManagerIP, "P2", thisProcessIP);
    */

    // create our server task
    const double PeriodClient = 10 * cmn_ms; // in milliseconds
    C1Task * C1 = new C1Task("C1", PeriodClient);
    C2ClientTask * C2 = new C2ClientTask("C2", PeriodClient);
    localManager->AddComponent(C1);
    localManager->AddComponent(C2);

    // Connect the tasks across networks
    if (!localManager->Connect("P1", "C1", "r1", "P2", "C2", "p1")) {
        CMN_LOG_INIT_ERROR << "Connect failed: P1:C1:r1-P2:C2:p1" << std::endl;
        return 1;
    }
    if (!localManager->Connect("P1", "C1", "r2", "P2", "C2", "p2")) {
        CMN_LOG_INIT_ERROR << "Connect failed: P1:C2:r1-P2:C2:p2" << std::endl;
        return 1;
    }
    if (!localManager->Connect("P1", "C2", "r1", "P2", "C2", "p2")) {
        CMN_LOG_INIT_ERROR << "Connect failed: P1:C2:r1-P2:C2:p2" << std::endl;
        return 1;
    }

    // create the tasks, i.e. find the commands
    localManager->CreateAll();
    // start the periodic Run
    localManager->StartAll();

    while (1) {
        osaSleep(10 * cmn_ms);
    }
    
    // cleanup
    localManager->KillAll();
    localManager->Cleanup();
    return 0;
}
