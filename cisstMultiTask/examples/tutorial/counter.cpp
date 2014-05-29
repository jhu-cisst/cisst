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
    // base constructor, same component name and period.  Third
    // parameter is "false" because we don't need hard realtime.  Last
    // parameter, 500, is the size of the default state table
    mtsTaskPeriodic(componentName, periodInSeconds, false, 500),
    // store no more than 50 elements in configuration table
    ConfigurationStateTable(50, "Configuration")
{
    SetupInterfaces();
}

void counter::SetupInterfaces(void)
{
    // state table variables
    StateTable.AddData(Counter, "Counter");

    // user defined configuration state table
    // first you need to add the state table to the component
    AddStateTable(&ConfigurationStateTable);
    // second, make sure we control when the table "advances"
    ConfigurationStateTable.SetAutomaticAdvance(false);
    // finally, add data to the state table
    ConfigurationStateTable.AddData(Increment, "Increment");

    // add a provided interface
    mtsInterfaceProvided * interfaceProvided = AddInterfaceProvided("User");

    // for applications dynamically creating interfaces, the user
    // should make sure the interface has been added properly.
    // AddInterfaceProvided could fail if there is already an
    // interface with the same name.
    if (!interfaceProvided) {
        CMN_LOG_CLASS_INIT_ERROR << "failed to add \"User\" to component \""
                                 << this->GetName() << "\"" << std::endl;
        return;
    }

    // add a void command.  The signature of the method used should be
    // "void method(void)".  As for the interface, it is possible to
    // check if a command has been added properly using the returned
    // value.
    if (!interfaceProvided->AddCommandVoid(&counter::Reset, this, "Reset")) {
        CMN_LOG_CLASS_INIT_ERROR << "failed to add command to interface \""
                                 << interfaceProvided->GetFullName()
                                 << "\"" << std::endl;
    }

    // in this example, all the commands have a different name so
    // there is really no need to check the returned value for
    // AddCommand ...

    // add a write command.  The signature of the method used should
    // be "void method(const type & payload)".  We also need to
    // provide the default value expected by this command.
    interfaceProvided->AddCommandWrite(&counter::SetIncrement, this,
                                       "SetIncrement", 1.0);

    // add a command to read the latest value from a state table
    interfaceProvided->AddCommandReadState(StateTable, Counter,
                                           "GetValue");
    interfaceProvided->AddCommandReadState(ConfigurationStateTable, Increment,
                                           "GetIncrement");

    // add a void event.  We need to provide the function
    // (mtsFunction) that will be used to trigger the event.
    interfaceProvided->AddEventVoid(OverflowEvent, "Overflow");

    // add a write event, i.e. an event with a payload.  We need to
    // provide the function (mtsFunction) that will be used to trigger
    // the event as well as the default value/type of the payload.
    interfaceProvided->AddEventWrite(InvalidIncrementEvent, "InvalidIncrement",
                                     std::string());
}

void counter::Reset(void)
{
    Counter = 0;
}

void counter::SetIncrement(const double & increment)
{
    // throw an event if the increment is too high
    if (increment > 10) {
        mtsExecutionResult result
            = InvalidIncrementEvent(std::string("increment must be less than 10.0"));
        // error should only occur if the programmer forgot to use
        // this function for an event
        if (!result) {
            CMN_LOG_CLASS_RUN_ERROR << "SetIncrement: trigger \"InvalidIncrementEvent\" returned "
                                    << result << std::endl;
        }
        return;
    }
    // or too low
    if (increment < 1.0) {
        mtsExecutionResult result
            = InvalidIncrementEvent(std::string("increment must be greater than 1.0"));
        if (!result) {
            CMN_LOG_CLASS_RUN_ERROR << "SetIncrement: trigger \"InvalidIncrementEvent\" returned "
                                    << result << std::endl;
        }
        return;
    }
    // now we can add to the state table
    ConfigurationStateTable.Start();
    Increment = increment;
    // make the circular buffer move one step
    ConfigurationStateTable.Advance();
}

void counter::Startup(void)
{
    // initialize counter and increment
    Counter = 0.0;
    // for the increment, use the method to make sure state table gets
    // updated as well
    SetIncrement(1.0);
}

void counter::Run(void)
{
    // process the commands received
    ProcessQueuedCommands();

    Counter += Increment;
    if (Counter > 100.0) {
        Counter = 0.0;
        // it's good practice to check the returned value
        mtsExecutionResult result = OverflowEvent();
        if (!result) {
            CMN_LOG_CLASS_RUN_ERROR << "SetIncrement: trigger \"OverflowEvent\" returned "
                                    << result << std::endl;
        }
    }
}
