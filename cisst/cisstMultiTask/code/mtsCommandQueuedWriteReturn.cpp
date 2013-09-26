/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2010-09-16

  (C) Copyright 2010-2013 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstMultiTask/mtsCommandQueuedWriteReturn.h>
#include <cisstMultiTask/mtsCallableWriteReturnBase.h>
#include <cisstMultiTask/mtsCommandWriteBase.h>
#include <cisstMultiTask/mtsMailBox.h>


mtsCommandQueuedWriteReturn::mtsCommandQueuedWriteReturn(mtsCallableWriteReturnBase * callable, const std::string & name,
                                                         const mtsGenericObject * argumentPrototype,
                                                         const mtsGenericObject * resultPrototype,
                                                         mtsMailBox * mailBox):
    BaseType(callable, name, argumentPrototype, resultPrototype),
    MailBox(mailBox),
    FinishedEvent(0)
{}


mtsCommandQueuedWriteReturn::~mtsCommandQueuedWriteReturn()
{}


mtsCommandQueuedWriteReturn * mtsCommandQueuedWriteReturn::Clone(mtsMailBox * mailBox) const
{
    return new mtsCommandQueuedWriteReturn(this->Callable, this->Name,
                                           this->ArgumentPrototype,
                                           this->ResultPrototype,
                                           mailBox);
}


mtsExecutionResult mtsCommandQueuedWriteReturn::Execute(const mtsGenericObject & argument,
                                                        mtsGenericObject & result)
{
    // check if this command is enabled
    if (!this->IsEnabled()) {
        return mtsExecutionResult::COMMAND_DISABLED;
    }
    // check if there is a mailbox (i.e. if the command is associated to an interface
    if (!MailBox) {
        CMN_LOG_RUN_ERROR << "Class mtsCommandQueuedWriteReturn: Execute: no mailbox for \""
                          << this->Name << "\"" << std::endl;
        return mtsExecutionResult::COMMAND_HAS_NO_MAILBOX;
    }
    // preserve address of result and wait to be dequeued
    ArgumentPointer = &argument;
    ResultPointer = &result;
    if (!MailBox->Write(this)) {
        CMN_LOG_RUN_ERROR << "Class mtsCommandQueuedWriteReturn: Execute: mailbox full for \""
                          << this->Name << "\"" <<  std::endl;
        return mtsExecutionResult::INTERFACE_COMMAND_MAILBOX_FULL;
    }
    return mtsExecutionResult::COMMAND_QUEUED;
}


const mtsGenericObject * mtsCommandQueuedWriteReturn::GetArgumentPointer(void)
{
    return this->ArgumentPointer;
}


mtsGenericObject * mtsCommandQueuedWriteReturn::GetResultPointer(void)
{
    return this->ResultPointer;
}


std::string mtsCommandQueuedWriteReturn::GetMailBoxName(void) const
{
    return this->MailBox ? this->MailBox->GetName() : "null pointer!";
}


void mtsCommandQueuedWriteReturn::EnableFinishedEvent(mtsCommandWriteBase *cmd)
{
    FinishedEvent = cmd;
}

bool mtsCommandQueuedWriteReturn::GenerateFinishedEvent(const mtsGenericObject &arg) const
{
    bool ret = false;
    if (FinishedEvent) {
        mtsExecutionResult result = FinishedEvent->Execute(arg, MTS_NOT_BLOCKING);
        if (!result.IsOK())
            CMN_LOG_RUN_ERROR << "mtsCommandQueuedWriteReturn: FinishedEvent returned " << result << std::endl;
        ret = result.IsOK();
    }
    return ret;
}

void mtsCommandQueuedWriteReturn::ToStream(std::ostream & outputStream) const {
    outputStream << "mtsCommandQueuedWriteReturn: MailBox \""
                 << this->GetMailBoxName()
                 << "\" for command(void) with result using " << *(this->Callable)
                 << " currently " << (this->IsEnabled() ? "enabled" : "disabled");
}
