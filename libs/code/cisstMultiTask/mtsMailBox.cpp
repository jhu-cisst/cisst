/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Peter Kazanzides
  Created on: 2007-09-05

  (C) Copyright 2007 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnAssert.h>
#include <cisstMultiTask/mtsMailBox.h>
#include <cisstMultiTask/mtsCommandQueuedVoid.h>
#include <cisstMultiTask/mtsCommandQueuedWrite.h>


mtsMailBox::mtsMailBox(const std::string & name,
                       size_t size,
                       mtsCommandVoidBase * postCommandQueuedCommand):
    CommandQueue(size, 0),
    Name(name),
    PostCommandQueuedCommand(postCommandQueuedCommand)
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
    if (this->PostCommandQueuedCommand) {
        this->PostCommandQueuedCommand->Execute();
    }
    return result;
}


// return false if nothing to execute; true otherwise.
// NOTE: this will break if exceptions are thrown by the command objects
// because the commands (and parameters) won't be removed from the queues.
bool mtsMailBox::ExecuteNext(void)
{
   mtsCommandBase** command = CommandQueue.Peek();

   // test for empty queue
   if (!command) {
       return false;
   }

   mtsCommandQueuedVoidBase * commandVoid;
   mtsCommandQueuedWriteBase * commandWrite;
   mtsCommandQueuedWriteGeneric * commandWriteGeneric;

   switch ((*command)->NumberOfArguments()) {
   case 0:
       commandVoid = dynamic_cast<mtsCommandQueuedVoidBase *>(*command);
       CMN_ASSERT(commandVoid);
       commandVoid->GetActualCommand()->Execute();
       break;
   case 1:
       commandWrite = dynamic_cast<mtsCommandQueuedWriteBase *>(*command);
       if (commandWrite) {
           commandWrite->GetActualCommand()->Execute(*(commandWrite->ArgumentPeek()));
           commandWrite->ArgumentGet();  // Remove from parameter queue
       } else {
           commandWriteGeneric = dynamic_cast<mtsCommandQueuedWriteGeneric *>(*command);
           CMN_ASSERT(commandWriteGeneric);
           commandWriteGeneric->GetActualCommand()->Execute(*(commandWriteGeneric->ArgumentPeek()));
           commandWriteGeneric->ArgumentGet();  // Remove from parameter queue
       }
       break;
   default:
       CMN_LOG_RUN_ERROR << "Class mtsMailBox: Invalid parameter in ExecuteNext" << std::endl;
       return false;
   }
   CommandQueue.Get();  // Remove command from mailbox queue
   return true;
}

