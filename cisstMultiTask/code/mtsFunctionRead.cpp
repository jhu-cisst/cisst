/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Peter Kazanzides, Anton Deguet

  (C) Copyright 2007-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstMultiTask/mtsFunctionRead.h>
#include <cisstMultiTask/mtsCommandRead.h>
#include <cisstMultiTask/mtsEventReceiver.h>


mtsFunctionRead::mtsFunctionRead(void):
    mtsFunctionBase(false),
    Command(0)
{}


mtsFunctionRead::~mtsFunctionRead()
{}


bool mtsFunctionRead::Detach(void) {
    if (this->IsValid()) {
        Command = 0;
        return true;
    }
    return false;
}


bool mtsFunctionRead::IsValid(void) const {
    return (this->Command != 0);
}


bool mtsFunctionRead::Bind(CommandType * command) {
    Command = command;
    if (Command) {
        InitCompletionCommand(Command->GetName() + "Blocking");
    }
    return (command != 0);
}


mtsExecutionResult mtsFunctionRead::ExecuteGeneric(mtsGenericObject & argument) const
{
    if (!Command) {
        return mtsExecutionResult::FUNCTION_NOT_BOUND;
    }
    mtsExecutionResult executionResult;
    // If Command is valid (not NULL), then CompletionCommand should also be valid
    CMN_ASSERT(CompletionCommand);
    CompletionCommand->PrepareToWait();
    executionResult = Command->Execute(argument, CompletionCommand->GetCommand());
    if (executionResult.GetResult() == mtsExecutionResult::COMMAND_QUEUED)
        executionResult = WaitForResult(argument);
    CompletionCommand->ClearWait();
    return executionResult;
}


mtsCommandRead * mtsFunctionRead::GetCommand(void) const {
    return Command;
}


const mtsGenericObject * mtsFunctionRead::GetArgumentPrototype(void) const
{
    if (this->Command) {
        return this->Command->GetArgumentPrototype();
    }
    return 0;
}


void mtsFunctionRead::ToStream(std::ostream & outputStream) const
{
    if (this->Command) {
        outputStream << "mtsFunctionRead for " << *Command;
    } else {
        outputStream << "mtsFunctionRead not initialized";
    }
}
