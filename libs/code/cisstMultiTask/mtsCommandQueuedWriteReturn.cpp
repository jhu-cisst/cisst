/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2010-09-16

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstMultiTask/mtsCommandQueuedWriteReturn.h>
#include <cisstMultiTask/mtsCallableWriteReturnBase.h>
#include <cisstMultiTask/mtsMailBox.h>


mtsCommandQueuedWriteReturn::mtsCommandQueuedWriteReturn(mtsCallableWriteReturnBase * callable, const std::string & name,
                                                         const mtsGenericObject * argumentPrototype,
                                                         const mtsGenericObject * resultPrototype,
                                                         mtsMailBox * mailBox):
    BaseType(callable, name, argumentPrototype, resultPrototype),
    MailBox(mailBox)
{}


mtsCommandQueuedWriteReturn::~mtsCommandQueuedWriteReturn()
{}


mtsCommandQueuedWriteReturn * mtsCommandQueuedWriteReturn::Clone(mtsMailBox * mailBox) const
{
    return new mtsCommandQueuedWriteReturn(this->Callable, this->Name,
                                           this->ResultPrototype,
                                           this->ArgumentPrototype,
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
    // test if the mailbox has been emptied already (e.g. post queued command)
    if (MailBox->IsEmpty()) {
        // signal has been raised, reset it
        MailBox->ThreadSignalWait(0.0);
    } else {
        // normal case, wait
        MailBox->ThreadSignalWait();
    }
    return mtsExecutionResult::COMMAND_SUCCEEDED;
}


const mtsGenericObject * mtsCommandQueuedWriteReturn::GetArgumentPointer(void)
{
    return ArgumentPointer;
}


mtsGenericObject * mtsCommandQueuedWriteReturn::GetResultPointer(void)
{
    return ResultPointer;
}


std::string mtsCommandQueuedWriteReturn::GetMailBoxName(void) const
{
    return this->MailBox ? this->MailBox->GetName() : "null pointer!";
}


void mtsCommandQueuedWriteReturn::ToStream(std::ostream & outputStream) const {
    outputStream << "mtsCommandQueuedWriteReturn: MailBox \""
                 << this->GetMailBoxName()
                 << "\" for command(void) with result using " << *(this->Callable)
                 << " currently " << (this->IsEnabled() ? "enabled" : "disabled");
}
