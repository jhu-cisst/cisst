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
                                                         mtsMailBox * mailBox, size_t size):
    BaseType(callable, name, argumentPrototype, resultPrototype),
    MailBox(mailBox),
    ArgumentQueueSize(size),
    ArgumentsQueue(),
    ReturnsQueue(),
    FinishedEventQueue()
{
    const mtsGenericObject * arg = dynamic_cast<const mtsGenericObject *>(this->GetArgumentPrototype());
    if (arg)
        ArgumentsQueue.SetSize(size, *arg);
    else
        CMN_LOG_INIT_WARNING << "Class mtsCommandQueuedWriteReturn: constructor: can't find argument prototype for command \""
                             << this->GetName() << "\"" << std::endl;
    mtsGenericObject *obj = 0;
    ReturnsQueue.SetSize(size, obj);
    mtsCommandWriteBase *cmd = 0;
    FinishedEventQueue.SetSize(size, cmd);
}


mtsCommandQueuedWriteReturn::~mtsCommandQueuedWriteReturn()
{
    // Destructor should probably go through the queue and make sure to send a response to any waiting component
    if (!MailBox->IsEmpty())
        CMN_LOG_INIT_WARNING << "mtsCommandQueuedWriteReturn destructor: mailbox for "
                             << GetName() << " is not empty" << std::endl;
}


mtsCommandQueuedWriteReturn * mtsCommandQueuedWriteReturn::Clone(mtsMailBox * mailBox, size_t size) const
{
    return new mtsCommandQueuedWriteReturn(this->Callable, this->Name,
                                           this->ArgumentPrototype,
                                           this->ResultPrototype,
                                           mailBox, size);
}


mtsExecutionResult mtsCommandQueuedWriteReturn::Execute(const mtsGenericObject & argument,
                                                        mtsGenericObject & result)
{
    return Execute(argument, result, 0);
}

mtsExecutionResult mtsCommandQueuedWriteReturn::Execute(const mtsGenericObject & argument,
                                                        mtsGenericObject & result,
                                                        mtsCommandWriteBase * finishedEventHandler)
{
    // check if this command is enabled
    if (!this->IsEnabled()) {
        return mtsExecutionResult::COMMAND_DISABLED;
    }
    // check if there is a mailbox (i.e. if the command is associated to an interface)
    if (!MailBox) {
        CMN_LOG_RUN_ERROR << "Class mtsCommandQueuedWriteReturn: Execute: no mailbox for \""
                          << this->Name << "\"" << std::endl;
        return mtsExecutionResult::COMMAND_HAS_NO_MAILBOX;
    }
    // check if all queues have some space
    if (ArgumentsQueue.IsFull() || ReturnsQueue.IsFull() || FinishedEventQueue.IsFull() || MailBox->IsFull()) {
        CMN_LOG_RUN_WARNING << "Class mtsCommandQueuedWriteReturn: Execute: Queue full for \""
                            << this->Name << "\" ["
                            << ArgumentsQueue.IsFull() << "|"
                            << ReturnsQueue.IsFull() << "|"
                            << FinishedEventQueue.IsFull() << "|"
                            << MailBox->IsFull() << "]"
                            << std::endl;
        return mtsExecutionResult::COMMAND_ARGUMENT_QUEUE_FULL;
    }
    // copy the argument to the local storage.
    if (!ArgumentsQueue.Put(argument)) {
        CMN_LOG_RUN_ERROR << "Class mtsCommandQueuedWriteReturn: Execute: ArgumentsQueue.Put failed for \""
                          << this->Name << "\"" << std::endl;
        cmnThrow("mtsCommandQueuedWriteReturn: Execute: ArgumentsQueue.Put failed");
        return mtsExecutionResult::UNDEFINED;
    }
    // copy the result to the local storage.
    if (!ReturnsQueue.Put(&result)) {
        CMN_LOG_RUN_ERROR << "Class mtsCommandQueuedWriteReturn: Execute: ReturnsQueue.Put failed for \""
                          << this->Name << "\"" << std::endl;
        ArgumentsQueue.Get();   // Remove the argument that was already queued
        cmnThrow("mtsCommandQueuedWriteReturn: Execute: ReturnsQueue.Put failed");
        return mtsExecutionResult::UNDEFINED;
    }
    // copy the finished event handler to the local storage.
    if (!FinishedEventQueue.Put(finishedEventHandler)) {
        CMN_LOG_RUN_ERROR << "Class mtsCommandQueuedWriteReturn: Execute: FinishedEventQueue.Put failed for \""
                          << this->Name << "\"" << std::endl;
        ArgumentsQueue.Get();   // Remove the argument that was already queued
        ReturnsQueue.Get();     // Remove the result that was already queued
        cmnThrow("mtsCommandQueuedWriteReturn: Execute: FinishedEventQueue.Put failed");
        return mtsExecutionResult::UNDEFINED;
    }
    // finally try to queue to mailbox
    if (!MailBox->Write(this)) {
        CMN_LOG_RUN_ERROR << "Class mtsCommandQueuedWriteReturn: Execute: mailbox full for \""
                          << this->Name << "\"" << std::endl;
        ArgumentsQueue.Get();      // Remove the argument that was already queued
        ReturnsQueue.Get();        // Remove the result that was already queued
        FinishedEventQueue.Get();  // Remove the finished event handler that was already queued
        cmnThrow("mtsCommandQueuedWriteReturn: Execute: MailBox.Write failed");
        return mtsExecutionResult::UNDEFINED;
    }
    return mtsExecutionResult::COMMAND_QUEUED;
}

std::string mtsCommandQueuedWriteReturn::GetMailBoxName(void) const
{
    return this->MailBox ? this->MailBox->GetName() : "null pointer!";
}


void mtsCommandQueuedWriteReturn::ToStream(std::ostream & outputStream) const {
    outputStream << "mtsCommandQueuedWriteReturn: MailBox \""
                 << this->GetMailBoxName()
                 << "\" for command(write) with result using " << *(this->Callable)
                 << " currently " << (this->IsEnabled() ? "enabled" : "disabled");
}
