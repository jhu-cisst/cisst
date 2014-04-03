/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Min Yang Jung
  Created on: 2010-03-05

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include "SignalGeneratorTask.h"

#include <cisstCommon/cmnLogger.h>
#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstOSAbstraction/osaSleep.h>

#define MAX_SIGNAL_COUNT 12

int main(int argc, char * argv[])
{
    // Set global component manager IP and signal generator id
    std::string globalComponentManagerIP;
    int id;

    if (argc == 1) {
        globalComponentManagerIP = "localhost";
        id = 1;
    } else if (argc == 2) {
        globalComponentManagerIP = argv[1];
        id = 1;
    } else if (argc == 3) {
        globalComponentManagerIP = argv[1];
        id = atoi(argv[2]);
    } else {
        std::cerr << "Usage: " << argv[0] << " (global component manager IP)" << std::endl;
        return 1;
    }

    std::cout << "Global component manager IP: " << globalComponentManagerIP << std::endl;
    std::cout << "ID of this signal generator: " << id << std::endl;

    // Get process name with signal generator id
    char processName[20] = "";
    sprintf(processName, "SignalGenerator%d", id);

    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    // add a log per thread
    osaThreadedLogFile threadedLog(processName);
    cmnLogger::AddChannel(threadedLog, CMN_LOG_ALLOW_ALL);

    // Get local component manager instance
    mtsTaskManager * taskManager;
    try {
        taskManager = mtsTaskManager::GetInstance(globalComponentManagerIP, processName);
    } catch (...) {
        CMN_LOG_INIT_ERROR << "Failed to initialize local component manager" << std::endl;
        return 1;
    }

    // create signal generator component
    const double PeriodClient = 10 * cmn_ms;
    SignalGenerator * signalGenerator = new SignalGenerator("SignalGenerator", PeriodClient);
    taskManager->AddComponent(signalGenerator);

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
