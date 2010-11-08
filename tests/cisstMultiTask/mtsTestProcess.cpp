/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

  Author(s):  Min Yang Jung, Anton Deguet
  Created on: 2010-09-01

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstCommon/cmnPortability.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsManagerLocal.h>

const double TransitionDelay = 5.0 * cmn_s;

int main(int argc, char * argv[])
{
    // get the process name from command line
    std::string processName;
    if (argc == 2) {
        processName = argv[1];
    } else {
        std::cerr << "Usage: " << argv[0] << " <process_name>" << std::endl;
        return 1;
    }

    std::string command;

    // get local component manager instance
    mtsManagerLocal * componentManager;
    std::cin >> command;
    if (command == "connect") {
        try {
            componentManager = mtsManagerLocal::GetInstance("localhost", processName);
        } catch (...) {
            std::cout << "failed to initialize local component manager" << std::endl;
            return 1;
        }
    } else {
        std::cout << "wrong command" << std::endl;
        return 1;
    }
    // send message to acknowledge connection
    std::cout << processName << " connected" << std::endl;

    // create and start local components
    std::cin >> command;
    if (command == "start") {
        // create the tasks, i.e. find the commands
        componentManager->CreateAll();
        if (!componentManager->WaitForStateAll(mtsComponentState::READY, TransitionDelay)) {
            std::cout << "failed to reach state READY for process \"" << processName << "\"" << std::endl;
            return 1;
        }
        // start the periodic Run
        componentManager->StartAll();
        if (!componentManager->WaitForStateAll(mtsComponentState::ACTIVE, TransitionDelay)) {
            std::cout << "failed to reach state ACTIVE for process \"" << processName << "\"" << std::endl;
            return 1;
        }

        // send message to confirm everything seems fine
        std::cout << "start succeeded" << std::endl;
    } else {
        std::cout << "wrong command" << std::endl;
        return 1;
    }

    // normal operations
    bool stop = false;
    while (!stop) {
        std::cin >> command;
        if (command == std::string("stop")) {
            stop = true;
        } else if (command == std::string("ping")) {
            std::cout << "ok" << std::endl;
        } else {
            std::cout << "unknown command \"" << command << "\"" << std::endl;
        }
        osaSleep(10.0 * cmn_ms);
    }
    
    // stop component manager
    componentManager->KillAll();
    if (!componentManager->WaitForStateAll(mtsComponentState::FINISHED, TransitionDelay)) {
        std::cout << "failed to reach state FINISHED for process \"" << processName << "\"" << std::endl;
        return 1;
    }

    componentManager->Cleanup();
    std::cout << "stop succeeded" << std::endl;

    // wait to be killed by pipe
    while (true) {
        osaSleep(1.0 * cmn_hour);
    }
    
    return 0;
}

