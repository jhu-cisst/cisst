/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id$ */

#include <cisstCommon.h>
#include <cisstOSAbstraction.h>
#include <cisstMultiTask.h>

#include "serverTask.h"

int main(int argc, char * argv[])
{
    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    // specify a higher, more verbose log level for these classes
    cmnLogger::SetMaskClassMatching("mts", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("serverTask", CMN_LOG_ALLOW_ALL);

    std::string globalComponentManagerIP;
    bool useGeneric;

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " [GlobalManagerIP] [flag]" << std::endl;
        std::cerr << "       GlobalManagerIP is set as 127.0.0.1 by default" << std::endl;
        std::cerr << "       flag = 1 to use double instead of mtsDouble" << std::endl;
    }

    // Set global component manager's ip and argument type
    // If flag is not specified, or not 1, then use generic type (mtsDouble)
    if (argc == 1) {
        globalComponentManagerIP = "localhost";
        useGeneric = true;
    } else if (argc == 2) {
        globalComponentManagerIP = argv[1];
        useGeneric = true;
    } else if (argc == 3) {
        globalComponentManagerIP = argv[1];
        useGeneric = (argv[2][0] != '1');
    } else {
        exit(-1);
    }
    
    std::cout << "Starting server, IP = " << globalComponentManagerIP << std::endl;
    std::cout << "Use " << (useGeneric ? "mtsDouble" : "double") << std::endl;

    // Get the TaskManager instance and set operation mode
    mtsManagerLocal * taskManager;
    try {
        taskManager = mtsManagerLocal::GetInstance(globalComponentManagerIP, "ProcessServer");
    } catch (...) {
        CMN_LOG_INIT_ERROR << "Failed to initialize local component manager" << std::endl;
        return 1;
    }

    // create our server task
    const double PeriodServer = 10 * cmn_ms; // in milliseconds
    serverTaskBase *server;
    if (useGeneric)
        server = new serverTask<mtsDouble>("Server", PeriodServer);
    else
        server = new serverTask<double>("Server", PeriodServer);

    taskManager->AddComponent(server);

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
