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


#include <cisstMultiTask/mtsCommandQueuedVoidReturn.h>
#include <cisstMultiTask/mtsCallableVoidReturnBase.h>
#include <cisstMultiTask/mtsMailBox.h>


mtsCommandQueuedVoidReturn::mtsCommandQueuedVoidReturn(mtsCallableVoidReturnBase * callable, const std::string & name,
                                                       const mtsGenericObject * resultPrototype,
                                                       mtsMailBox * mailBox):
    BaseType(callable, name, resultPrototype),
    MailBox(mailBox)
{}


mtsCommandQueuedVoidReturn::~mtsCommandQueuedVoidReturn()
{}


mtsCommandQueuedVoidReturn * mtsCommandQueuedVoidReturn::Clone(mtsMailBox * mailBox) const
{
    return new mtsCommandQueuedVoidReturn(this->Callable, this->Name, this->ResultPrototype,
                                          mailBox);
}


mtsExecutionResult mtsCommandQueuedVoidReturn::Execute(mtsGenericObject & result)
{
    if (this->IsEnabled()) {
        if (!MailBox) {
            CMN_LOG_RUN_ERROR << "Class mtsCommandQueuedVoidReturn: Execute: no mailbox for \""
                              << this->Name << "\"" << std::endl;
            return mtsExecutionResult::NO_MAILBOX;
        }
        // preserve address of result and wait to be dequeued
        ResultPointer = &result;
        if (!MailBox->Write(this)) {
            CMN_LOG_RUN_ERROR << "Class mtsCommandQueuedVoidReturn: Execute: mailbox full for \""
                              << this->Name << "\"" <<  std::endl;
            return mtsExecutionResult::MAILBOX_FULL;
        }
        if (!MailBox->IsEmpty())
            MailBox->ThreadSignalWait();
        return mtsExecutionResult::DEV_OK;
    }
    return mtsExecutionResult::DISABLED;
}


mtsGenericObject * mtsCommandQueuedVoidReturn::GetResultPointer(void)
{
    return ResultPointer;
}


std::string mtsCommandQueuedVoidReturn::GetMailBoxName(void) const
{
    return this->MailBox ? this->MailBox->GetName() : "null pointer!";
}


void mtsCommandQueuedVoidReturn::ToStream(std::ostream & outputStream) const {
    outputStream << "mtsCommandQueuedVoidReturn: MailBox \""
                 << this->GetMailBoxName()
                 << "\" for command(void) with result using " << *(this->Callable)
                 << " currently " << (this->IsEnabled() ? "enabled" : "disabled");
}
