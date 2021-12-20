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


#include <cisstMultiTask/mtsFunctionQualifiedRead.h>
#include <cisstMultiTask/mtsCommandQualifiedRead.h>
#include <cisstMultiTask/mtsEventReceiver.h>


mtsFunctionQualifiedRead::mtsFunctionQualifiedRead(void):
    mtsFunctionBase(false),
    Command(0)
{}


mtsFunctionQualifiedRead::~mtsFunctionQualifiedRead()
{}


// documented in base class
bool mtsFunctionQualifiedRead::Detach(void) {
    if (this->IsValid()) {
        Command = 0;
        return true;
    }
    return false;
}


bool mtsFunctionQualifiedRead::IsValid(void) const {
    return (this->Command != 0);
}


bool mtsFunctionQualifiedRead::Bind(CommandType * command) {
    Command = command;
    if (this->Command) {
        InitCompletionCommand(this->Command->GetName() + "Result");
    }
    return (command != 0);
}


mtsExecutionResult mtsFunctionQualifiedRead::ExecuteGeneric(const mtsGenericObject & qualifier,
                                                            mtsGenericObject & argument) const
{
    if (!Command) {
        return mtsExecutionResult::FUNCTION_NOT_BOUND;
    }
    // If Command is valid (not NULL), then CompletionCommand should also be valid
    CMN_ASSERT(CompletionCommand);
    CompletionCommand->PrepareToWait();
    mtsExecutionResult executionResult = Command->Execute(qualifier, argument, CompletionCommand->GetCommand());
    if (executionResult.GetResult() == mtsExecutionResult::COMMAND_QUEUED)
        executionResult = WaitForResult(argument);
    CompletionCommand->ClearWait();
    return executionResult;
}


mtsFunctionQualifiedRead::CommandType * mtsFunctionQualifiedRead::GetCommand(void) const {
    return Command;
}


const mtsGenericObject * mtsFunctionQualifiedRead::GetArgument1Prototype(void) const
{
    if (this->Command) {
        return this->Command->GetArgument1Prototype();
    }
    return 0;
}


const mtsGenericObject * mtsFunctionQualifiedRead::GetArgument2Prototype(void) const
{
    if (this->Command) {
        return this->Command->GetArgument2Prototype();
    }
    return 0;
}


void mtsFunctionQualifiedRead::ToStream(std::ostream & outputStream) const {
    if (this->Command != 0) {
        outputStream << "mtsFunctionQualifiedRead for " << *Command;
    } else {
        outputStream << "mtsFunctionQualifiedRead not initialized";
    }
}
