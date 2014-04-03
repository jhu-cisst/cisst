/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Peter Kazanzides, Anton Deguet

  (C) Copyright 2007-2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/



/*!
  \file
  \brief Defines a base function object to allow heterogeneous containers of functions.
*/


#include <cisstMultiTask/mtsFunctionBase.h>
#include <cisstMultiTask/mtsEventReceiver.h>
#include <cisstOSAbstraction/osaThreadSignal.h>


mtsFunctionBase::mtsFunctionBase(const bool isProxy):
    ThreadSignal(0),
    CompletionCommand(0),
    IsProxy(isProxy)
{}

mtsFunctionBase::~mtsFunctionBase()
{
    delete CompletionCommand;
}

void mtsFunctionBase::InitCompletionCommand(const std::string &name)
{
    if (!this->CompletionCommand)
        this->CompletionCommand = new mtsEventReceiverWrite;
    this->CompletionCommand->SetName(name);
    this->CompletionCommand->SetThreadSignal(this->ThreadSignal);
}

void mtsFunctionBase::SetThreadSignal(osaThreadSignal * threadSignal)
{
    this->ThreadSignal = threadSignal;
    if (this->CompletionCommand)
        this->CompletionCommand->SetThreadSignal(this->ThreadSignal);
}


void mtsFunctionBase::ThreadSignalWait(void) const
{
    if (this->ThreadSignal) {
        this->ThreadSignal->Wait();
    } else {
        CMN_LOG_RUN_WARNING << "mtsFunctionBase::ThreadSignalWait: no thread signal available, function will not block!" << std::endl; 
    }
}

mtsExecutionResult mtsFunctionBase::WaitForResult(mtsGenericObject &arg) const
{
    mtsExecutionResult ret(mtsExecutionResult::INVALID_INPUT_TYPE);
    if (CompletionCommand && CompletionCommand->Wait(arg))
        ret = (arg.Valid() ? mtsExecutionResult::COMMAND_SUCCEEDED : mtsExecutionResult::METHOD_OR_FUNCTION_FAILED);
    return ret;
}

mtsExecutionResult mtsFunctionBase::WaitForResult(void) const
{
    mtsExecutionResultProxy remoteResult;
    if (CompletionCommand && CompletionCommand->Wait(remoteResult))
        return remoteResult.GetData();
    return mtsExecutionResult::INVALID_INPUT_TYPE;
}
