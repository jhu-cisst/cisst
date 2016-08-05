/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2010-09-16

  (C) Copyright 2010-2016 Johns Hopkins University (JHU), All Rights Reserved.

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


template <class _Base>
mtsCommandQueuedWriteReturnBase<_Base>::mtsCommandQueuedWriteReturnBase(CallableType * callable, const std::string & name,
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
    ArgumentsQueue.SetSize(size, *argumentPrototype);
    mtsGenericObject *obj = 0;
    ReturnsQueue.SetSize(size, obj);
    mtsCommandWriteBase *cmd = 0;
    FinishedEventQueue.SetSize(size, cmd);
}


template <class _Base>
mtsCommandQueuedWriteReturnBase<_Base>::~mtsCommandQueuedWriteReturnBase()
{
    // Destructor should probably go through the queue and make sure to send a response to any waiting component
    if (!MailBox->IsEmpty()) {
        CMN_LOG_INIT_WARNING << this->GetClassName() << " destructor: mailbox for "
                             << this->GetName() << " is not empty" << std::endl;
    }
}


template <>
std::string mtsCommandQueuedWriteReturnBase<mtsCommandWriteReturn>::GetClassName(void) const
{
    return std::string("mtsCommandQueuedWriteReturn");
}

template <>
std::string mtsCommandQueuedWriteReturnBase<mtsCommandQualifiedRead>::GetClassName(void) const
{
    return std::string("mtsCommandQueuedQualifiedRead");
}

template <class _Base>
mtsCommandQueuedWriteReturnBase<_Base> * mtsCommandQueuedWriteReturnBase<_Base>::Clone(mtsMailBox * mailBox, size_t size) const
{
    return new ThisType(this->Callable, this->Name,
                        this->GetArgumentPrototype(),
                        this->GetResultPrototype(),
                        mailBox, size);
}

template <class _Base>
mtsExecutionResult mtsCommandQueuedWriteReturnBase<_Base>::Execute(const mtsGenericObject & argument,
                                                                   mtsGenericObject & result)
{
    return Execute(argument, result, 0);
}

template <class _Base>
mtsExecutionResult mtsCommandQueuedWriteReturnBase<_Base>::Execute(const mtsGenericObject & argument,
                                                                   mtsGenericObject & result,
                                                                   mtsCommandWriteBase * finishedEventHandler)
{
    // check if this command is enabled
    if (!this->IsEnabled()) {
        return mtsExecutionResult::COMMAND_DISABLED;
    }
    // check if there is a mailbox (i.e. if the command is associated to an interface)
    if (!MailBox) {
        CMN_LOG_RUN_ERROR << GetClassName() << ": Execute: no mailbox for \""
                          << this->Name << "\"" << std::endl;
        return mtsExecutionResult::COMMAND_HAS_NO_MAILBOX;
    }
    // check if all queues have some space
    if (ArgumentsQueue.IsFull() || ReturnsQueue.IsFull() || FinishedEventQueue.IsFull() || MailBox->IsFull()) {
        CMN_LOG_RUN_WARNING << GetClassName() << ": Execute: Queue full for \""
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
        CMN_LOG_RUN_ERROR << GetClassName() << ": Execute: ArgumentsQueue.Put failed for \""
                          << this->Name << "\"" << std::endl;
        cmnThrow(GetClassName()+": Execute: ArgumentsQueue.Put failed");
        return mtsExecutionResult::UNDEFINED;
    }
    // copy the result to the local storage.
    if (!ReturnsQueue.Put(&result)) {
        CMN_LOG_RUN_ERROR << GetClassName() << ": Execute: ReturnsQueue.Put failed for \""
                          << this->Name << "\"" << std::endl;
        ArgumentsQueue.Get();   // Remove the argument that was already queued
        cmnThrow(GetClassName()+": Execute: ReturnsQueue.Put failed");
        return mtsExecutionResult::UNDEFINED;
    }
    // copy the finished event handler to the local storage.
    if (!FinishedEventQueue.Put(finishedEventHandler)) {
        CMN_LOG_RUN_ERROR << GetClassName() << ": Execute: FinishedEventQueue.Put failed for \""
                          << this->Name << "\"" << std::endl;
        ArgumentsQueue.Get();   // Remove the argument that was already queued
        ReturnsQueue.Get();     // Remove the result that was already queued
        cmnThrow(GetClassName()+": Execute: FinishedEventQueue.Put failed");
        return mtsExecutionResult::UNDEFINED;
    }
    // finally try to queue to mailbox
    if (!MailBox->Write(this)) {
        CMN_LOG_RUN_ERROR << GetClassName() << ": Execute: mailbox full for \""
                          << this->Name << "\"" << std::endl;
        ArgumentsQueue.Get();      // Remove the argument that was already queued
        ReturnsQueue.Get();        // Remove the result that was already queued
        FinishedEventQueue.Get();  // Remove the finished event handler that was already queued
        cmnThrow(GetClassName()+": Execute: MailBox.Write failed");
        return mtsExecutionResult::UNDEFINED;
    }
    return mtsExecutionResult::COMMAND_QUEUED;
}

template <class _Base>
std::string mtsCommandQueuedWriteReturnBase<_Base>::GetMailBoxName(void) const
{
    return this->MailBox ? this->MailBox->GetName() : "null pointer!";
}


template <class _Base>
void mtsCommandQueuedWriteReturnBase<_Base>::ToStream(std::ostream & outputStream) const {
    outputStream << GetClassName() << ": MailBox \""
                 << this->GetMailBoxName()
                 << "\" for command(write) with result using " << *(this->Callable)
                 << " currently " << (this->IsEnabled() ? "enabled" : "disabled");
}

// Force instantiation of these two types
template class mtsCommandQueuedWriteReturnBase<mtsCommandWriteReturn>;
template class mtsCommandQueuedWriteReturnBase<mtsCommandQualifiedRead>;
