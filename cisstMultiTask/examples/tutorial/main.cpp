/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2014-05-14

  (C) Copyright 2014-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include "counter.h"
#include "user.h"

#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsManagerLocal.h>

int main(int CMN_UNUSED(argc), char ** CMN_UNUSED(argv))
{
    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    // get all messages to log file
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    // specify a higher, more verbose log level for these classes
    cmnLogger::SetMaskClass("counter", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClass("user", CMN_LOG_ALLOW_ALL);

    // component manager is a singleton
    mtsManagerLocal * componentManager = mtsManagerLocal::GetInstance();

    // create counter and user components
    counter * counterPointer = new counter("counter", 1.0 * cmn_s);
    user * userPointer = new user("user");

    // add the components to the component manager
    componentManager->AddComponent(counterPointer);
    componentManager->AddComponent(userPointer);

    // connect the components, task.RequiresInterface -> task.ProvidesInterface
    componentManager->Connect("user", "Counter", "counter", "User");

    // create the components
    componentManager->CreateAll();
    componentManager->WaitForStateAll(mtsComponentState::READY, 2.0 * cmn_s);

    // start the periodic Run
    componentManager->StartAll();
    componentManager->WaitForStateAll(mtsComponentState::ACTIVE, 2.0 * cmn_s);

    // loop until the user tells us to quit
    while (!userPointer->Quit) {
        osaSleep(100.0 * cmn_ms);
    }

    // cleanup
    componentManager->KillAll();
    componentManager->WaitForStateAll(mtsComponentState::FINISHED, 2.0 * cmn_s);

    componentManager->Cleanup();

    delete counterPointer;
    delete userPointer;

    // stop all logs
    cmnLogger::SetMask(CMN_LOG_ALLOW_NONE);
    return 0;
}
