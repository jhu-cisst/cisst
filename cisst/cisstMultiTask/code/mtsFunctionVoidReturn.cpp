/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Anton Deguet
  Created on: 2005-05-02

  (C) Copyright 2010-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstMultiTask/mtsFunctionVoidReturn.h>
#include <cisstMultiTask/mtsCommandVoidReturn.h>


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
        CMN_LOG_INIT_WARNING << "Class mtsFunctionVoidReturn: Bind called on already bound function:" << this << std::endl;
    }
    this->Command = command;
    return (command != 0);
}


mtsExecutionResult mtsFunctionVoidReturn::ExecuteGeneric(mtsGenericObject & result) const
{
    mtsExecutionResult executionResult = Command ?
        Command->Execute(result)
        : mtsExecutionResult::FUNCTION_NOT_BOUND;
    if (executionResult.GetResult() == mtsExecutionResult::COMMAND_QUEUED
        && !this->IsProxy) {
        this->ThreadSignalWait();
        return mtsExecutionResult::COMMAND_SUCCEEDED;
    }
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

