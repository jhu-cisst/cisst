/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Peter Kazanzides, Anton Deguet

  (C) Copyright 2007 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstOSAbstraction/osaThreadSignal.h>
#include <cisstMultiTask/mtsFunctionVoid.h>
#include <cisstMultiTask/mtsCommandVoid.h>


mtsFunctionVoid::~mtsFunctionVoid()
{}


bool mtsFunctionVoid::Detach(void)
{
    if (this->IsValid()) {
        this->Command = 0;
        return true;
    }
    return false;
}


bool mtsFunctionVoid::IsValid(void) const
{
    return (this->Command != 0);
}


bool mtsFunctionVoid::Bind(CommandType * command)
{
    if (this->Command) {
        CMN_LOG_INIT_WARNING << "Class mtsFunctionVoid: Bind called on already bound function:" << this << std::endl;
    }
    this->Command = command;
    return (command != 0);
}


mtsExecutionResult mtsFunctionVoid::Execute(void) const
{
    return Command ? Command->Execute(MTS_NOT_BLOCKING) : mtsExecutionResult::FUNCTION_NOT_BOUND;
}


mtsExecutionResult mtsFunctionVoid::ExecuteBlocking(void) const
{
    mtsExecutionResult result;
    result = Command ? Command->Execute(MTS_BLOCKING) : mtsExecutionResult::FUNCTION_NOT_BOUND;
    if (result.GetResult() == mtsExecutionResult::COMMAND_QUEUED) {
        ThreadSignal->Wait();
        return mtsExecutionResult::COMMAND_SUCCEEDED;
    }
    return result;
}


mtsCommandVoid * mtsFunctionVoid::GetCommand(void) const {
    return Command;
}


void mtsFunctionVoid::ToStream(std::ostream & outputStream) const {
    if (this->Command != 0) {
        outputStream << "mtsFunctionVoid for " << *Command;
    } else {
        outputStream << "mtsFunctionVoid not initialized";
    }
}

