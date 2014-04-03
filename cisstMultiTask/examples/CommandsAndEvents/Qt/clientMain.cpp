/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsManagerLocal.h>

#include <QApplication>

#include "clientQtComponent.h"

#if (CISST_OS == CISST_LINUX_XENOMAI)
#include <sys/mman.h>
#endif

int main(int argc, char * argv[])
{
#if (CISST_OS == CISST_LINUX_XENOMAI)
    mlockall(MCL_CURRENT | MCL_FUTURE);
#endif

    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    cmnLogger::SetMaskClassMatching("mts", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("clientQtComponent", CMN_LOG_ALLOW_ALL);

    // set global component manager IP
    std::string globalComponentManagerIP;
    if (argc == 1) {
        std::cerr << "Using default, i.e. 127.0.0.1 to find global component manager" << std::endl;
        globalComponentManagerIP = "127.0.0.1";
    } else if (argc == 2) {
        globalComponentManagerIP = argv[1];
    } else {
        std::cerr << "Usage: " << argv[0] << " (global component manager IP)" << std::endl;
        return -1;
    }

    // create a Qt user interface
    QApplication application(argc, argv);

    // create our client task
    clientQtComponent * client = new clientQtComponent("Client");

    // Get the TaskManager instance and set operation mode
    mtsManagerLocal * componentManager;
    try {
        componentManager = mtsManagerLocal::GetInstance(globalComponentManagerIP, "cisstMultiTaskCommandsAndEventsQtClient");
    } catch (...) {
        CMN_LOG_INIT_ERROR << "Failed to initialize local component manager" << std::endl;
        return 1;
    }
    componentManager->AddComponent(client);

    // Connect the componens across networks
    if (!componentManager->Connect("cisstMultiTaskCommandsAndEventsQtClient", "Client", "Required",
                                   "cisstMultiTaskCommandsAndEventsQtServer", "Server", "Provided")) {
        CMN_LOG_INIT_ERROR << "Connect failed" << std::endl;
        return 1;
    }

    //
    // TODO: Hide this waiting routine inside mtsTaskManager using events or other things.
    //
    osaSleep(0.5 * cmn_s);

    // create and start all tasks
    componentManager->CreateAll();
    componentManager->WaitForStateAll(mtsComponentState::READY);

    componentManager->StartAll();
    componentManager->WaitForStateAll(mtsComponentState::ACTIVE);

    // run Qt user interface
    application.exec();

    // kill all tasks and perform cleanup
    componentManager->KillAll();
    componentManager->WaitForStateAll(mtsComponentState::FINISHED, 2.0 * cmn_s);

    componentManager->Cleanup();
    return 0;
}
