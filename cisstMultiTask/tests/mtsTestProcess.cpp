/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung, Anton Deguet
  Created on: 2010-09-01

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
#include <cisstMultiTask/mtsManagerLocal.h>

#include <fstream>
#include "mtsTestComponents.h"

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

    // configure log
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClassAll(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskFunction(CMN_LOG_ALLOW_ALL);
    cmnLogger::HaltDefaultLog();
    std::string logFileName = processName + "-log.txt";
    std::ofstream logFile(logFileName.c_str());
    cmnLogger::AddChannel(logFile, CMN_LOG_ALLOW_ALL);

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
        std::cout << "must use \"connect\" first" << std::endl;
        return 1;
    }
    // send message to acknowledge connection
    std::cout << processName << " connected" << std::endl;

    // create and start local components
    std::cin >> command;
    if (command == "start") {
        // create the tasks, i.e. find the commands
        componentManager->CreateAll();
        if (!componentManager->WaitForStateAll(mtsComponentState::READY, StateTransitionMaximumDelay)) {
            std::cout << "failed to reach state READY for process \"" << processName << "\"" << std::endl;
            return 1;
        }
        // start the periodic Run
        componentManager->StartAll();
        if (!componentManager->WaitForStateAll(mtsComponentState::ACTIVE, StateTransitionMaximumDelay)) {
            std::cout << "failed to reach state ACTIVE for process \"" << processName << "\"" << std::endl;
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
    std::string
        componentName,
        interfaceType,
        interfaceName,
        commandName;
    int parameter;
    mtsExecutionResult executionResult;

    while (!stop) {
        std::cin >> command;
        if (command == std::string("stop")) {
            stop = true;

        } else if (command == std::string("ping")) {
            std::cout << "ok" << std::endl;

        } else if (command == std::string("has_component")) {
            std::cin >> componentName;
            mtsComponent * component = componentManager->GetComponent(componentName);
            if (component) {
                std::cout << component->GetName() << " found" << std::endl;
            } else {
                std::cout << componentName << " not found" << std::endl;
            }
        } else if (command == std::string("command")) {
            std::cin >> componentName;
            std::cin >> interfaceType;
            std::cin >> interfaceName;
            std::cin >> commandName;
            mtsComponent * component = componentManager->GetComponent(componentName);
            mtsTestComponent * testComponent = dynamic_cast<mtsTestComponent *>(component);
            if (!testComponent) {
                std::cout << "failed to cast component to test component" << std::endl;
            } else {
                if (interfaceType == "required") {
                    mtsTestInterfaceRequiredBase * interfaceRequired = testComponent->TestInterfacesRequired[interfaceName];
                    if (!interfaceRequired) {
                        std::cout << "failed to find test interface required \"" << interfaceName << "\" on component \"" << componentName << "\"" << std::endl;
                    } else {
                        if (commandName == "get_value") {
                            std::cout << interfaceRequired->GetValue() << std::endl;
                        } else if (commandName == "function_void") {
                            executionResult = interfaceRequired->FunctionVoid();
                            if (!executionResult) {
                                std::cout << executionResult << std::endl;
                            } else {
                                std::cout << "ok" << std::endl;
                            }
                        } else if (commandName == "function_write") {
                            std::cin >> parameter;
                            executionResult = interfaceRequired->FunctionWrite(parameter);
                            if (!executionResult) {
                                std::cout << executionResult << std::endl;
                            } else {
                                std::cout << "ok" << std::endl;
                            }
                        } else {
                            std::cout << "unknown command name \"" << commandName << "\"";
                        }
                    }
                } else if (interfaceType == "provided") {
                    mtsTestInterfaceProvidedBase * interfaceProvided = testComponent->TestInterfacesProvided[interfaceName];
                    if (!interfaceProvided) {
                        std::cout << "failed to find test interface required \"" << interfaceName << "\" on component \"" << componentName << "\"" << std::endl;
                    } else {
                        if (commandName == "get_value") {
                            std::cout << interfaceProvided->GetValue() << std::endl;
                        } else if (commandName == "event_void") {
                            executionResult = interfaceProvided->EventVoid();
                            if (!executionResult) {
                                std::cout << "trigger event void result: " << executionResult << std::endl;
                            } else {
                                std::cout << "ok" << std::endl;
                            }
                        } else if (commandName == "event_write") {
                            std::cin >> parameter;
                            executionResult = interfaceProvided->EventWrite(parameter);
                            if (!executionResult) {
                                std::cout << "trigger event write result: " << executionResult << ": " << parameter << std::endl;
                            } else {
                                std::cout << "ok" << std::endl;
                            }
                        } else {
                            std::cout << "unknown command name \"" << commandName << "\"";
                        }
                    }
                } else {
                    std::cout << "incorrect type of interface \"" << interfaceType << "\", options are \"required\" or \"provided\"" << std::endl;
                }
            }
        } else {
            std::cout << "unknown command \"" << command << "\"" << std::endl;
        }
        osaSleep(10.0 * cmn_ms);
    }

    // stop component manager
    componentManager->KillAll();
    if (!componentManager->WaitForStateAll(mtsComponentState::FINISHED, StateTransitionMaximumDelay)) {
        std::cout << "failed to reach state FINISHED for process \"" << processName << "\"" << std::endl;
        return 1;
    }

    std::cout << "stop succeeded" << std::endl;

    // wait to be killed by pipe
    while (true) {
        osaSleep(1.0 * cmn_hour);
    }

    // stop log
    cmnLogger::SetMask(CMN_LOG_ALLOW_NONE);
    logFile.close();

    return 0;
}
