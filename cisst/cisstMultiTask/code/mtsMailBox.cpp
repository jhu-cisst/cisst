/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Peter Kazanzides, Anton Deguet
  Created on: 2007-09-05

  (C) Copyright 2007-2013 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnAssert.h>
#include <cisstMultiTask/mtsMailBox.h>
#include <cisstMultiTask/mtsCallableVoidBase.h>
#include <cisstMultiTask/mtsCallableVoidReturnBase.h>
#include <cisstMultiTask/mtsCallableWriteReturnBase.h>
#include <cisstMultiTask/mtsCommandQueuedVoid.h>
#include <cisstMultiTask/mtsCommandQueuedVoidReturn.h>
#include <cisstMultiTask/mtsCommandQueuedWrite.h>
#include <cisstMultiTask/mtsCommandQueuedWriteReturn.h>


mtsMailBox::mtsMailBox(const std::string & name,
                       size_t size,
                       mtsCallableVoidBase * postCommandQueuedCallable):
    CommandQueue(size, 0),
    Name(name),
    PostCommandQueuedCallable(postCommandQueuedCallable),
    PostCommandDequeuedCommand(0),
    PostCommandReturnDequeuedCommand(0)
{}


mtsMailBox::~mtsMailBox(void)
{}


const std::string & mtsMailBox::GetName(void) const
{
    return this->Name;
}


bool mtsMailBox::Write(mtsCommandBase * command)
{
    bool result;
    result = (CommandQueue.Put(command) != 0);
    if (this->PostCommandQueuedCallable) {
        this->PostCommandQueuedCallable->Execute();
    }
    return result;
}


// return false if nothing to execute; true otherwise.
bool mtsMailBox::ExecuteNext(void)
{
   mtsCommandBase** command = CommandQueue.Peek();

   // test for empty queue
   if (!command) {
       return false;
   }

   mtsCommandQueuedVoid * commandVoid;
   mtsCommandQueuedWriteBase * commandWrite;
   mtsCommandQueuedWriteGeneric * commandWriteGeneric;
   mtsCommandQueuedVoidReturn * commandVoidReturn;
   mtsCommandQueuedWriteReturn * commandWriteReturn;

   mtsExecutionResult result;

   bool isBlocking = false;
   bool isBlockingReturn = false;
   try {
       if (!(*command)->Returns()) {
           switch ((*command)->NumberOfArguments()) {
           case 0:
               commandVoid = dynamic_cast<mtsCommandQueuedVoid *>(*command);
               CMN_ASSERT(commandVoid);
               isBlocking = (commandVoid->BlockingFlagGet() == MTS_BLOCKING);
               result = commandVoid->GetCallable()->Execute();
               break;
           case 1:
               commandWrite = dynamic_cast<mtsCommandQueuedWriteBase *>(*command);
               if (commandWrite) {
                   isBlocking = (commandWrite->BlockingFlagGet() == MTS_BLOCKING);
                   try {
                       result = commandWrite->GetActualCommand()->Execute(*(commandWrite->ArgumentPeek()), MTS_NOT_BLOCKING);
                   }
                   catch (...) {
                       commandWrite->ArgumentGet();  // Remove from parameter queue
                       throw;
                   }
                   commandWrite->ArgumentGet();  // Remove from parameter queue
               } else {
                   commandWriteGeneric = dynamic_cast<mtsCommandQueuedWriteGeneric *>(*command);
                   CMN_ASSERT(commandWriteGeneric);
                   isBlocking = (commandWriteGeneric->BlockingFlagGet() == MTS_BLOCKING);
                   try {
                       result = commandWriteGeneric->GetActualCommand()->Execute(*(commandWriteGeneric->ArgumentPeek()), MTS_NOT_BLOCKING);
                   }
                   catch (...) {
                       commandWriteGeneric->ArgumentGet();  // Remove from parameter queue
                       throw;
                   }
                   commandWriteGeneric->ArgumentGet();  // Remove from parameter queue
               }
               break;
           default:
               CMN_LOG_RUN_ERROR << "Class mtsMailBox: Invalid parameter in ExecuteNext" << std::endl;
               return false;
           }
       } else {
           mtsGenericObject *resultPointer;
           mtsCommandWriteBase *finishedEvent;
           switch ((*command)->NumberOfArguments()) {
           case 0:
               commandVoidReturn = dynamic_cast<mtsCommandQueuedVoidReturn *>(*command);
               CMN_ASSERT(commandVoidReturn);
               isBlockingReturn = true;
               resultPointer = commandVoidReturn->ReturnGet();
               finishedEvent = commandVoidReturn->FinishedEventGet();
               result = commandVoidReturn->GetCallable()->Execute(*resultPointer);
               if (result.IsOK() && finishedEvent) {
                   mtsExecutionResult evt_result = finishedEvent->Execute(*resultPointer, MTS_NOT_BLOCKING);
                   if (!evt_result.IsOK())
                       CMN_LOG_RUN_ERROR << "mtsMailbox: Failed to execute FinishedEvent for command void return " << commandVoidReturn->GetName()
                                         << ", result = " << evt_result << std::endl;
                }
               break;
           case 1:
               commandWriteReturn = dynamic_cast<mtsCommandQueuedWriteReturn *>(*command);
               CMN_ASSERT(commandWriteReturn);
               isBlockingReturn = true;
               resultPointer = commandWriteReturn->ReturnGet();
               finishedEvent = commandWriteReturn->FinishedEventGet();
               try {
                   result = commandWriteReturn->GetCallable()->Execute( *(commandWriteReturn->ArgumentPeek()), *resultPointer);
               }
               catch (...) {
                   commandWriteReturn->ArgumentGet();  // Remove from parameter queue
                   throw;
               }
               commandWriteReturn->ArgumentGet();  // Remove from parameter queue
               if (result.IsOK() && finishedEvent) {
                   mtsExecutionResult evt_result = finishedEvent->Execute(*resultPointer, MTS_NOT_BLOCKING);
                   if (!evt_result.IsOK())
                       CMN_LOG_RUN_ERROR << "mtsMailbox: Failed to execute FinishedEvent for command write return " << commandWriteReturn->GetName()
                                         << ", result = " << evt_result << std::endl;
               }
               break;
           default:
               CMN_LOG_RUN_ERROR << "Class mtsMailBox: Invalid parameter in ExecuteNext" << std::endl;
               return false;
           }
       }
   }
   catch (std::exception & exceptionCaught) {
       CMN_LOG_RUN_WARNING << "mtsMailbox \"" << GetName() << "\": ExecuteNext for command \"" << (*command)->GetName()
                           << "\" caught exception \"" << exceptionCaught.what() << "\"" << std::endl;
       this->TriggerPostQueuedCommandIfNeeded(isBlocking, isBlockingReturn);
       CommandQueue.Get();  // Remove command from mailbox queue
       throw;
   }
   catch (...) {
       CMN_LOG_RUN_WARNING << "mtsMailbox \"" << GetName() << "\": ExecuteNext for command \"" << (*command)->GetName()
                           << "\" caught exception, blocking = " << isBlocking << std::endl;
       this->TriggerPostQueuedCommandIfNeeded(isBlocking, isBlockingReturn);
       CommandQueue.Get();  // Remove command from mailbox queue
       throw;
   }
   this->TriggerPostQueuedCommandIfNeeded(isBlocking, isBlockingReturn);
   if (!result.IsOK()) {
       CMN_LOG_RUN_WARNING << "mtsMailbox \"" << GetName() << "\": ExecuteNext for command \"" << (*command)->GetName()
                           << "\" failed, execution result is \"" << result << "\"" << std::endl;
   }
   CommandQueue.Get();  // Remove command from mailbox queue
   return true;
}


void mtsMailBox::TriggerPostQueuedCommandIfNeeded(bool isBlocking, bool isBlockingReturn)
{
   if (isBlocking && this->PostCommandDequeuedCommand) {
       this->PostCommandDequeuedCommand->Execute(MTS_NOT_BLOCKING);
   } else {
       if (isBlockingReturn && this->PostCommandReturnDequeuedCommand) {
           this->PostCommandReturnDequeuedCommand->Execute(MTS_NOT_BLOCKING);
       }
   }
}


void mtsMailBox::SetSize(size_t size)
{
    if (CommandQueue.GetSize() != size) {
        CommandQueue.SetSize(size, 0); // array of null pointers
    }
}


bool mtsMailBox::IsEmpty(void) const
{
    return CommandQueue.IsEmpty();
}


bool mtsMailBox::IsFull(void) const
{
    return CommandQueue.IsFull();
}


void mtsMailBox::SetPostCommandDequeuedCommand(mtsCommandVoid * command)
{
    this->PostCommandDequeuedCommand = command;
}


void mtsMailBox::SetPostCommandReturnDequeuedCommand(mtsCommandVoid * command)
{
    this->PostCommandReturnDequeuedCommand = command;
}
