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


#include "counter.h"

#include <cisstCommon/cmnConstants.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

// required to implement the class services, see cisstCommon
CMN_IMPLEMENT_SERVICES_DERIVED(counter, mtsTaskPeriodic);

counter::counter(const std::string & componentName, double periodInSeconds):
    // base constructor, same task name and period.
    mtsTaskPeriodic(componentName, periodInSeconds, false, 500)
{
    SetupInterfaces();
}

void counter::SetupInterfaces(void)
{
    // state table variables
    StateTable.AddData(Counter, "Counter");

    // provided interfaces
    mtsInterfaceProvided * interfaceProvided;
    interfaceProvided = AddInterfaceProvided("User");
    if (!interfaceProvided) {
        CMN_LOG_CLASS_INIT_ERROR << "failed to add \"User\" to component \"" << this->GetName() << "\"" << std::endl;
        return;
    }
    if (!interfaceProvided->AddCommandVoid(&counter::Reset, this, "Reset")) {
        CMN_LOG_CLASS_INIT_ERROR << "failed to add command to interface \"" << interfaceProvided->GetFullName() << "\"" << std::endl;
    }
    if (!interfaceProvided->AddCommandReadState(StateTable, Counter, "GetValue")) {
        CMN_LOG_CLASS_INIT_ERROR << "failed to add command to interface \"" << interfaceProvided->GetFullName() << "\"" << std::endl;
    }
    if (!interfaceProvided->AddCommandWrite(&counter::SetIncrement, this, "SetIncrement", 1.0)) {
        CMN_LOG_CLASS_INIT_ERROR << "failed to add command to interface \"" << interfaceProvided->GetFullName() << "\"" << std::endl;
    }
    if (!interfaceProvided->AddEventVoid(OverflowEvent, "Overflow")) {
        CMN_LOG_CLASS_INIT_ERROR << "failed to add event to interface \"" << interfaceProvided->GetFullName() << "\"" << std::endl;
    }
    if (!interfaceProvided->AddEventWrite(InvalidIncrementEvent, "InvalidIncrement", std::string())) {
        CMN_LOG_CLASS_INIT_ERROR << "failed to add event to interface \"" << interfaceProvided->GetFullName() << "\"" << std::endl;
    }
}

void counter::Reset(void)
{
    Counter = 0;
}

void counter::SetIncrement(const double & increment)
{
    if (increment > 10) {
        mtsExecutionResult result = InvalidIncrementEvent(std::string("increment must be less than 10.0"));
        if (!result) {
            CMN_LOG_CLASS_RUN_ERROR << "SetIncrement: trigger \"InvalidIncrementEvent\" returned " << result << std::endl;
        }
        return;
    }
    if (increment < 1.0) {
        mtsExecutionResult result = InvalidIncrementEvent(std::string("increment must be greater than 1.0"));
        if (!result) {
            CMN_LOG_CLASS_RUN_ERROR << "SetIncrement: trigger \"InvalidIncrementEvent\" returned " << result << std::endl;
        }
        return;
    }
    Increment = increment;
}

void counter::Startup(void)
{
    Counter = 0.0;
    Increment = 1.0;
}

void counter::Run(void)
{
    // process the commands received
    ProcessQueuedCommands();

    Counter += Increment;
    if (Counter > 100.0) {
        Counter = 0.0;
        mtsExecutionResult result = OverflowEvent();
        if (!result) {
            CMN_LOG_CLASS_RUN_ERROR << "SetIncrement: trigger \"OverflowEvent\" returned " << result << std::endl;
        }
    }
}
