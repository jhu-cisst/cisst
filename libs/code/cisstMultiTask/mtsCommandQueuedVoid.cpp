/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2005-05-02

  (C) Copyright 2005-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstMultiTask/mtsCommandQueuedVoid.h>
#include <cisstMultiTask/mtsCallableVoidBase.h>

mtsCommandQueuedVoid::mtsCommandQueuedVoid(void):
    BaseType(),
    MailBox(0)
{}


mtsCommandQueuedVoid::mtsCommandQueuedVoid(mtsCallableVoidBase * callable,
                                           const std::string & name,
                                           mtsMailBox * mailBox,
                                           size_t size):
    BaseType(callable, name),
    MailBox(mailBox),
    BlockingFlagQueue(size, MTS_NOT_BLOCKING)
{}


mtsCommandQueuedVoid * mtsCommandQueuedVoid::Clone(mtsMailBox * mailBox, size_t size) const
{
    return new mtsCommandQueuedVoid(this->Callable, this->Name,
                                    mailBox, size);
}


mtsExecutionResult mtsCommandQueuedVoid::Execute(mtsBlockingType blocking)
{
    if (this->IsEnabled()) {
        if (!MailBox) {
            CMN_LOG_RUN_ERROR << "Class mtsCommandQueuedVoid: Execute: no mailbox for \""
                              << this->Name << "\"" << std::endl;
            return mtsExecutionResult::COMMAND_HAS_NO_MAILBOX;
        }
        if (BlockingFlagQueue.Put(blocking)) {
            if (MailBox->Write(this)) {
                if ((blocking == MTS_BLOCKING) && !MailBox->IsEmpty()) {
                    MailBox->ThreadSignalWait();
                    return mtsExecutionResult::COMMAND_SUCCEEDED;
                }
                return mtsExecutionResult::COMMAND_QUEUED;
            } else {
                CMN_LOG_RUN_ERROR << "Class mtsCommandQueuedVoid: Execute: Mailbox full for \""
                                  << this->Name << "\"" <<  std::endl;
                BlockingFlagQueue.Get(); // pop blocking flag from local storage
                return mtsExecutionResult::INTERFACE_COMMAND_MAILBOX_FULL;
            }
        } else {
            CMN_LOG_RUN_ERROR << "Class mtsCommandQueuedVoid: Execute: BlockingFlagQueue full for \""
                              << this->Name << "\"" << std::endl;
        }
        return mtsExecutionResult::COMMAND_ARGUMENT_QUEUE_FULL;
    }
    return mtsExecutionResult::COMMAND_DISABLED;
}


mtsBlockingType mtsCommandQueuedVoid::BlockingFlagGet(void)
{
    return *(this->BlockingFlagQueue.Get());
}


std::string mtsCommandQueuedVoid::GetMailBoxName(void) const
{
    return this->MailBox ? this->MailBox->GetName() : "null pointer!";
}


void mtsCommandQueuedVoid::ToStream(std::ostream & outputStream) const
{
    outputStream << "mtsCommandQueuedVoid: Mailbox \""
                 << this->GetMailBoxName()
                 << "\" for command(void) using " << *(this->Callable)
                 << " currently " << (this->IsEnabled() ? "enabled" : "disabled");
}
