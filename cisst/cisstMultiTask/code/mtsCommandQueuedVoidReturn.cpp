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


#include <cisstMultiTask/mtsCommandQueuedVoidReturn.h>
#include <cisstMultiTask/mtsCallableVoidReturnBase.h>
#include <cisstMultiTask/mtsCommandWriteBase.h>
#include <cisstMultiTask/mtsMailBox.h>


mtsCommandQueuedVoidReturn::mtsCommandQueuedVoidReturn(mtsCallableVoidReturnBase * callable, const std::string & name,
                                                       const mtsGenericObject * resultPrototype,
                                                       mtsMailBox * mailBox, size_t size):
    BaseType(callable, name, resultPrototype),
    MailBox(mailBox),
    ArgumentQueueSize(size),
    ReturnsQueue(),
    FinishedEventQueue()
{
    mtsGenericObject *obj = 0;
    ReturnsQueue.SetSize(size, obj);
    mtsCommandWriteBase *cmd = 0;
    FinishedEventQueue.SetSize(size, cmd);
}


mtsCommandQueuedVoidReturn::~mtsCommandQueuedVoidReturn()
{
    // Destructor should probably go through the queue and make sure to send a response to any waiting component
    if (!MailBox->IsEmpty())
        CMN_LOG_INIT_WARNING << "mtsCommandQueuedVoidReturn destructor: mailbox for "
                             << GetName() << " is not empty" << std::endl;
}


mtsCommandQueuedVoidReturn * mtsCommandQueuedVoidReturn::Clone(mtsMailBox * mailBox, size_t size) const
{
    return new mtsCommandQueuedVoidReturn(this->Callable, this->Name, this->ResultPrototype,
                                          mailBox, size);
}


mtsExecutionResult mtsCommandQueuedVoidReturn::Execute(mtsGenericObject & result)
{
    return Execute(result, 0);
}

mtsExecutionResult mtsCommandQueuedVoidReturn::Execute(mtsGenericObject & result,
                                                       mtsCommandWriteBase * finishedEventHandler)
{
    // check if this command is enabled
    if (!this->IsEnabled()) {
        return mtsExecutionResult::COMMAND_DISABLED;
    }
    // check if there is a mailbox (i.e. if the command is associated to an interface)
    if (!MailBox) {
        CMN_LOG_RUN_ERROR << "Class mtsCommandQueuedVoidReturn: Execute: no mailbox for \""
                          << this->Name << "\"" << std::endl;
        return mtsExecutionResult::COMMAND_HAS_NO_MAILBOX;
    }
    // check if all queues have some space
    if (ReturnsQueue.IsFull() || FinishedEventQueue.IsFull() || MailBox->IsFull()) {
        CMN_LOG_RUN_WARNING << "Class mtsCommandQueuedVoidReturn: Execute: Queue full for \""
                            << this->Name << "\" ["
                            << ReturnsQueue.IsFull() << "|"
                            << FinishedEventQueue.IsFull() << "|"
                            << MailBox->IsFull() << "]"
                            << std::endl;
        return mtsExecutionResult::COMMAND_ARGUMENT_QUEUE_FULL;
    }
    // copy the result to the local storage.
    if (!ReturnsQueue.Put(&result)) {
        CMN_LOG_RUN_ERROR << "Class mtsCommandQueuedVoidReturn: Execute: ReturnsQueue.Put failed for \""
                          << this->Name << "\"" << std::endl;
        cmnThrow("mtsCommandQueuedVoidReturn: Execute: ReturnsQueue.Put failed");
        return mtsExecutionResult::UNDEFINED;
    }
    // copy the finished event handler to the local storage.
    if (!FinishedEventQueue.Put(finishedEventHandler)) {
        CMN_LOG_RUN_ERROR << "Class mtsCommandQueuedVoidReturn: Execute: FinishedEventQueue.Put failed for \""
                          << this->Name << "\"" << std::endl;
        ReturnsQueue.Get();     // Remove the result that was already queued
        cmnThrow("mtsCommandQueuedVoidReturn: Execute: FinishedEventQueue.Put failed");
        return mtsExecutionResult::UNDEFINED;
    }
    // finally try to queue to mailbox
    if (!MailBox->Write(this)) {
        CMN_LOG_RUN_ERROR << "Class mtsCommandQueuedVoidReturn: Execute: mailbox full for \""
                          << this->Name << "\"" <<  std::endl;
        ReturnsQueue.Get();        // Remove the result that was already queued
        FinishedEventQueue.Get();  // Remove the finished event handler that was already queued
        cmnThrow("mtsCommandQueuedVoidReturn: Execute: MailBox.Write failed");
        return mtsExecutionResult::UNDEFINED;
    }
    return mtsExecutionResult::COMMAND_QUEUED;
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
