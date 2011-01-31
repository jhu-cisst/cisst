/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2011 Johns Hopkins University (JHU), All Rights
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

#include "clientTask.h"


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
    cmnLogger::SetMaskClassMatching("clientTask", CMN_LOG_ALLOW_ALL);

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
    mtsManagerLocal * componentManager;
    try {
        componentManager = mtsManagerLocal::GetInstance(globalComponentManagerIP, "ProcessClient");
    } catch (...) {
        CMN_LOG_INIT_ERROR << "Failed to initialize local component manager" << std::endl;
        return 1;
    }

    // create our client task
    const double PeriodClient = 10 * cmn_ms; // in milliseconds
    clientTaskBase * client;
    if (useGeneric) {
        client = new clientTask<mtsDouble>("Client", PeriodClient);
    } else {
        client = new clientTask<double>("Client", PeriodClient);
    }

    componentManager->AddComponent(client);

    // Connect the tasks across networks
    if (!componentManager->Connect("ProcessClient", "Client", "Required", "ProcessServer", "Server", "Provided")) {
        CMN_LOG_INIT_ERROR << "Connect failed" << std::endl;
        return 1;
    }

    // create the tasks, i.e. find the commands
    componentManager->CreateAll();
    // start the periodic Run
    componentManager->StartAll();

    while (1) {
        Fl::lock();
        {
            Fl::check();
        }
        Fl::unlock();
        Fl::awake();
        osaSleep(5.0 * cmn_ms);
    }

    // cleanup
    componentManager->KillAll();
    componentManager->Cleanup();
    return 0;
}
