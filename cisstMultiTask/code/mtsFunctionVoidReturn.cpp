/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s): Anton Deguet
  Created on: 2005-05-02

  (C) Copyright 2010-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstMultiTask/mtsFunctionVoidReturn.h>
#include <cisstMultiTask/mtsCommandVoidReturn.h>
#include <cisstMultiTask/mtsEventReceiver.h>


mtsFunctionVoidReturn::mtsFunctionVoidReturn(const bool isProxy):
    mtsFunctionBase(isProxy),
    Command(0)
{}


mtsFunctionVoidReturn::~mtsFunctionVoidReturn()
{}


bool mtsFunctionVoidReturn::Detach(void)
{
    if (this->IsValid()) {
        this->Command = 0;
        return true;
    }
    return false;
}


bool mtsFunctionVoidReturn::IsValid(void) const
{
    return (this->Command != 0);
}


bool mtsFunctionVoidReturn::Bind(CommandType * command)
{
    if (this->Command) {
        CMN_LOG_INIT_WARNING << "Class mtsFunctionVoidReturn: Bind called on already bound function: " << this << std::endl;
    }
    this->Command = command;
    if (this->Command) {
        InitCompletionCommand(this->Command->GetName() + "Result");
    }
    return (command != 0);
}


mtsExecutionResult mtsFunctionVoidReturn::ExecuteGeneric(mtsGenericObject & result) const
{
    if (!Command) {
        return mtsExecutionResult::FUNCTION_NOT_BOUND;
    }
    // If Command is valid (not NULL), then CompletionCommand should also be valid
    CMN_ASSERT(CompletionCommand);
    CompletionCommand->PrepareToWait();
    mtsExecutionResult executionResult = Command->Execute(result, CompletionCommand->GetCommand());
    if (executionResult.GetResult() == mtsExecutionResult::COMMAND_QUEUED)
        executionResult = WaitForResult(result);
    CompletionCommand->ClearWait();
    return executionResult;
}


mtsFunctionVoidReturn::CommandType * mtsFunctionVoidReturn::GetCommand(void) const
{
    return this->Command;
}


const mtsGenericObject * mtsFunctionVoidReturn::GetResultPrototype(void) const
{
    if (this->Command) {
        return this->Command->GetResultPrototype();
    }
    return 0;
}


void mtsFunctionVoidReturn::ToStream(std::ostream & outputStream) const {
    if (this->Command != 0) {
        outputStream << "mtsFunctionVoidReturn for " << *Command;
    } else {
        outputStream << "mtsFunctionVoidReturn not initialized";
    }
}
