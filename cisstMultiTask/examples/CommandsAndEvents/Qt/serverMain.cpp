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

#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>

#include <QApplication>

#include "serverQtComponent.h"

#if (CISST_OS == CISST_LINUX_XENOMAI)
#include <sys/mman.h>
#endif

int main(int argc, char * argv[])
{
#if (CISST_OS == CISST_LINUX_XENOMAI)
    mlockall(MCL_CURRENT | MCL_FUTURE);
#endif

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

    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    cmnLogger::SetMaskClassMatching("mts", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("serverQtComponent", CMN_LOG_ALLOW_ALL);

    // create a Qt user interface
    QApplication application(argc, argv);

    // create our server component
    serverQtComponent * server = new serverQtComponent("Server");

    // Get the ComponentManager instance and set operation mode
    mtsComponentManager * componentManager;
    try {
        componentManager = mtsComponentManager::GetInstance(globalComponentManagerIP, "cisstMultiTaskCommandsAndEventsQtServer");
    } catch (...) {
        CMN_LOG_INIT_ERROR << "Failed to initialize component manager" << std::endl;
        return 1;
    }
    componentManager->AddComponent(server);

    //
    // TODO: Hide this waiting routine inside mtsComponentManager using events or other things.
    //
    osaSleep(0.5 * cmn_s);

    // create and start all components
    componentManager->CreateAll();
    componentManager->WaitForStateAll(mtsComponentState::READY);

    componentManager->StartAll();
    componentManager->WaitForStateAll(mtsComponentState::ACTIVE);

    // run Qt user interface
    application.exec();

    // kill all components and perform cleanup
    componentManager->KillAll();
    componentManager->WaitForStateAll(mtsComponentState::FINISHED, 2.0 * cmn_s);

    componentManager->Cleanup();
    return 0;
}
