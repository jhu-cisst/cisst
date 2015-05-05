/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2014-07-24

  (C) Copyright 2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/
// For cisst
#include <cisstCommon/cmnGetChar.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#if (CISST_OS == CISST_LINUX_XENOMAI)
#include <sys/mman.h>
#endif

// For casros
#include "config.h"
#include "common.h"
#include "json.h"

using namespace SC;

mtsManagerLocal      * ComponentManager = 0;
mtsSafetyCoordinator * SafetyCoordinator = 0;

// Server component
class Server: public mtsTaskPeriodic {
    void Void(void) {
        std::cout << "#" << std::flush;
    }
public:
    Server(const std::string & name, double period)
        : mtsTaskPeriodic(name, period, false, 5000)
    {
        mtsInterfaceProvided * provided = AddInterfaceProvided("provided");
        CMN_ASSERT(provided);
        provided->AddCommandVoid(&Server::Void, this, "void");
    }

    void Configure(const std::string & CMN_UNUSED(filename) = "") {}
    void Startup(void) {}
    void Run(void) {
        ProcessQueuedCommands();
        ProcessQueuedEvents();
    }
    void Cleanup(void) {}
};

// Client component
class Client: public mtsTaskPeriodic {

    mtsFunctionVoid RunVoid;

public:
    Client(const std::string & name, double period)
        : mtsTaskPeriodic(name, period, false, 5000)
    {
        mtsInterfaceRequired * required = AddInterfaceRequired("required");
        CMN_ASSERT(required);
        required->AddFunction("void", RunVoid, MTS_REQUIRED);
    }

    void Configure(const std::string & CMN_UNUSED(filename) = "") {}
    void Startup(void) {}
    void Run(void) {
        ProcessQueuedCommands();
        ProcessQueuedEvents();

        RunVoid();
        std::cout << "." << std::flush;
    }
    void Cleanup(void) {}
};


int main(int, char *[])
{
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskFunction(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    //cmnLogger::SetMaskClassMatching("mtsSafetyCoordinator", CMN_LOG_ALLOW_ALL);
    //cmnLogger::SetMaskClassMatching("mtsMonitorComponent", CMN_LOG_ALLOW_ALL);
    
    // Enable casros framework
    mtsComponentManager::InstallSafetyCoordinator();

    // Get instance of local component manager
    try {
        ComponentManager = mtsComponentManager::GetInstance();
    } catch (std::exception & e) {
        std::cerr << "Failed to initialize local component manager: " << e.what() << std::endl;
        return 1;
    }

    // Get instance of safety coordinator
    SafetyCoordinator = ComponentManager->GetCoordinator();

    // Create two test components
    Server server("server", 1 * cmn_s);
    Client client("client", 10 * cmn_ms);
    ComponentManager->AddComponent(&server);
    ComponentManager->AddComponent(&client);
    ComponentManager->Connect("client", "required", "server", "provided");

    ComponentManager->CreateAll();
    ComponentManager->WaitForStateAll(mtsComponentState::READY);

    ComponentManager->StartAll();
    ComponentManager->WaitForStateAll(mtsComponentState::ACTIVE);

    std::cout << "Press 'q' to quit." << std::endl;

    // loop until 'q' is pressed
    int key = ' ';
    while (key != 'q') {
        key = cmnGetChar();
        osaSleep(100 * cmn_ms);
    }
    std::cout << std::endl;

    // Clean up resources
    std::cout << "Cleaning up..." << std::endl;

    ComponentManager->KillAll();
    ComponentManager->WaitForStateAll(mtsComponentState::FINISHED, 2.0 * cmn_s);
    ComponentManager->Cleanup();

    return 0;
}
