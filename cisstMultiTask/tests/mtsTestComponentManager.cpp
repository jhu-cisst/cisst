/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung, Anton Deguet
  Created on: 2010-01-20

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstCommon/cmnPortability.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsManagerGlobal.h>
#include <cisstMultiTask/mtsManagerLocal.h>

#include "mtsTestComponents.h"

int main(int CMN_UNUSED(argc), char ** CMN_UNUSED(argv))
{
    std::string command;
    mtsManagerGlobal globalComponentManager;
    mtsManagerLocal * localManager;

    // create and start global component manager
    std::cin >> command;
    if (command == "connect") {
        if (!globalComponentManager.StartServer()) {
            std::cout << "connect failed" << std::endl;
            return 1;
        } else {
            try {
                localManager = mtsManagerLocal::GetInstance(globalComponentManager);
            } catch (...) {
                std::cout << "connect failed" << std::endl;
                return 1;
            }
            std::cout << "component_manager connected" << std::endl;
        }
    } else {
        std::cout << "must use \"connect\" first" << std::endl;
        return 1;
    }

    // create and start local components
    std::cin >> command;
    if (command == "start") {
        // create the tasks, i.e. find the commands
        localManager->CreateAll();
        if (!localManager->WaitForStateAll(mtsComponentState::READY, StateTransitionMaximumDelay)) {
            std::cout << "failed to reach state READY for component_manager" << std::endl;
            return 1;
        }
        // start the periodic Run
        localManager->StartAll();
        if (!localManager->WaitForStateAll(mtsComponentState::ACTIVE, StateTransitionMaximumDelay)) {
            std::cout << "failed to reach state ACTIVE for component_manager" << std::endl;
            return 1;
        }

        // send message to confirm everything seems fine
        std::cout << "start succeeded" << std::endl;
    } else {
        std::cout << "must use \"start\" first" << std::endl;
        return 1;
    }

    // normal operations
    bool stop = false;
    std::string process;
    while (!stop) {
        std::cin >> command;
        if (command == std::string("stop")) {
            stop = true;
        } else if (command == std::string("ping")) {
            std::cout << "ok" << std::endl;
        } else if (command == std::string("has_process")) {
            std::cin >> process;
            if (globalComponentManager.FindProcess(process)) {
                std::cout << process << " found" << std::endl;
            } else {
                std::cout << process << " not found" << std::endl;
            }
        } else {
            std::cout << "unknown command \"" << command << "\"" << std::endl;
        }
        osaSleep(10.0 * cmn_ms);
    }

    // stop component_manager
    localManager->KillAll();
    if (!localManager->WaitForStateAll(mtsComponentState::FINISHED, StateTransitionMaximumDelay)) {
        std::cout << "failed to reach state FINISHED for component_manager" << std::endl;
        return 1;
    }

    if (!globalComponentManager.StopServer()) {
        std::cout << "stop failed" << std::endl;
        return 1;
    } else {
        std::cout << "stop succeeded" << std::endl;
    }

    // wait to be killed by pipe
    while (true) {
        osaSleep(1.0 * cmn_hour);
    }

    return 0;
}
