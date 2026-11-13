/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Peter Kazanzides, Anton Deguet

  (C) Copyright 2007-2026 Johns Hopkins University (JHU), All Rights Reserved.

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


mtsFunctionBase::mtsFunctionBase():
    ThreadSignal(nullptr),
    CompletionCommand(nullptr)
{}

// Define once definition of mtsEventReceiverWrite is available
mtsFunctionBase::~mtsFunctionBase() = default;

void mtsFunctionBase::InitCompletionCommand(const std::string &name)
{
    if (!CompletionCommand) {
        CompletionCommand = std::make_unique<mtsEventReceiverWrite>();
    }

    CompletionCommand->SetName(name);
    CompletionCommand->SetThreadSignal(ThreadSignal);
}

void mtsFunctionBase::DeleteCompletionCommand(void)
{
    CompletionCommand = nullptr;
}

void mtsFunctionBase::SetThreadSignal(osaThreadSignal * threadSignal)
{
    this->ThreadSignal = threadSignal;
    if (this->CompletionCommand)
        this->CompletionCommand->SetThreadSignal(this->ThreadSignal);
}

// TODO BB: deprecate/delete?
void mtsFunctionBase::ThreadSignalWait(void) const
{
    if (this->ThreadSignal) {
        this->ThreadSignal->Wait();
    } else {
        CMN_LOG_RUN_WARNING << "mtsFunctionBase::ThreadSignalWait: no thread signal available, function will not block!" << std::endl; 
    }
}

mtsExecutionResult mtsFunctionBase::WaitForResult() const
{
    mtsExecutionResult ret(mtsExecutionResult::UNDEFINED);
    if (CompletionCommand && CompletionCommand->Wait()) {
        ret = CompletionCommand->IsArgValid() ? mtsExecutionResult::COMMAND_SUCCEEDED : mtsExecutionResult::METHOD_OR_FUNCTION_FAILED;
    }
    return ret;
}
