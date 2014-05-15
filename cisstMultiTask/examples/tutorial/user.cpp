/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2014-05-14

  (C) Copyright 2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "user.h"

#include <cisstCommon/cmnKbHit.h>
#include <cisstCommon/cmnGetChar.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>

// required to implement the class services, see cisstCommon
CMN_IMPLEMENT_SERVICES_DERIVED(user, mtsTaskContinuous);

user::user(const std::string & componentName):
    // base constructor, same task name and period.
    mtsTaskContinuous(componentName, 500),
    Quit(false)
{
    SetupInterfaces();
}

void user::SetupInterfaces(void)
{
    mtsInterfaceRequired * interfaceRequired = this->AddInterfaceRequired("Counter");
    if (!interfaceRequired) {
        CMN_LOG_CLASS_INIT_ERROR << "failed to add \"Counter\" to component \"" << this->GetName() << "\"" << std::endl;
        return;
    }
    if (!(interfaceRequired->AddFunction("Reset", this->Reset))) {
        CMN_LOG_CLASS_INIT_ERROR << "failed to add function to interface \"" << interfaceRequired->GetFullName() << "\"" << std::endl;
    }
    if (!(interfaceRequired->AddFunction("GetValue", this->GetValue))) {
        CMN_LOG_CLASS_INIT_ERROR << "failed to add function to interface \"" << interfaceRequired->GetFullName() << "\"" << std::endl;
    }
    if (!(interfaceRequired->AddFunction("SetIncrement", this->SetIncrement))) {
        CMN_LOG_CLASS_INIT_ERROR << "failed to add function to interface \"" << interfaceRequired->GetFullName() << "\"" << std::endl;
    }
    if (!(interfaceRequired->AddEventHandlerVoid(&user::OverflowHandler, this, "Overflow"))) {
        CMN_LOG_CLASS_INIT_ERROR << "failed to add event handler to interface \"" << interfaceRequired->GetFullName() << "\"" << std::endl;
    }
    if (!(interfaceRequired->AddEventHandlerWrite(&user::InvalidIncrementHandler, this, "InvalidIncrement"))) {
        CMN_LOG_CLASS_INIT_ERROR << "failed to add event handler to interface \"" << interfaceRequired->GetFullName() << "\"" << std::endl;
    }
}

void user::PrintUsage(void) const
{
    std::cout << "Press ..." << std::endl
              << " [g] to get current counter value" << std::endl
              << " [r] to reset counter" << std::endl
              << " [i] to set a new counter increment" << std::endl
              << " [q] to quit" << std::endl;
}

void user::Startup(void)
{
    PrintUsage();
}

void user::OverflowHandler(void)
{
    std::cout << "Counter overflow detected, reset to zero" << std::endl;
}

void user::InvalidIncrementHandler(const std::string & message)
{
    std::cout << "Invalid increment detected with message \"" << message << "\"" << std::endl;
}

void user::Run(void)
{
    // process the events received
    ProcessQueuedEvents();

    // detect if the user has pressed a key
    if (!cmnKbHit()) {
        osaSleep(10.0 * cmn_ms); // sleep just a big to not hog resources
        return;
    }

    // get the character that triggered the keyboard hit
    char c = cmnGetChar();
    std::cout << std::endl;

    mtsExecutionResult result;
    double value;

    switch (c) {
    case 'q':
        // set the quit flag so that main() can start killing all components
        Quit = true;
        break;

    case 'r':
        // reset counter
        result = Reset();
        if (!result) {
            CMN_LOG_CLASS_RUN_ERROR << "Run: execute \"Reset\" returned: " << result << std::endl;
        } else {
            std::cout << "Sent command to reset counter" << std::endl;
        }
        break;

    case 'g':
        // get current value from counter
        result = GetValue(value);
        if (!result) {
            CMN_LOG_CLASS_RUN_ERROR << "Run: execute \"Reset\" returned: " << result << std::endl;
        } else {
            std::cout << "Current value: " << value << std::endl;
        }
        break;

    case 'i':
        // set new increment for counter, does not check if the value is correct
        std::cout << "Enter new value for increment" << std::endl;
        std::cin >> value;
        result = SetIncrement(value);
        if (!result) {
            CMN_LOG_CLASS_RUN_ERROR << "Run: execute \"SetIncrement\" returned: " << result << std::endl;
        } else {
            std::cout << "Sent command to set increment to " << value << std::endl;
        }
        break;

    default:
        PrintUsage();
        break;
    }
}
