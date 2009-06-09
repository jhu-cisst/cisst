/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides
  Created on: 2004-04-30

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnExport.h>
#include <cisstCommon/cmnPortability.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaThreadBuddy.h>
#include <cisstMultiTask/mtsTaskInterface.h>
#include <cisstMultiTask/mtsTask.h>

#include <iostream>
#include <string>


CMN_IMPLEMENT_SERVICES(mtsTaskInterface)
CMN_IMPLEMENT_SERVICES(mtsTaskInterface::ThreadResources)


mtsTaskInterface::mtsTaskInterface(const std::string & name, mtsTask * task):
    BaseType(name, task),
    CommandsQueuedVoid("CommandQueuedVoid"),
    CommandsQueuedWrite("CommandQueuedWrite"),
    Mutex()
{}



mtsTaskInterface::~mtsTaskInterface() {
	CMN_LOG_CLASS_INIT_VERBOSE << "Class mtsTaskInterface: Class destructor" << std::endl;
    // ADV: Need to add all cleanup, i.e. make sure all mailboxes are
    // properly deleted.
}


void mtsTaskInterface::Cleanup() {
    ThreadResourcesMapType::iterator op;
    for (op = QueuedCommands.begin(); op != QueuedCommands.end(); op++) {
        delete op->second->GetMailBox();
        delete op->second;
    }
    QueuedCommands.erase(QueuedCommands.begin(), QueuedCommands.end());
	CMN_LOG_CLASS_INIT_VERBOSE << "Done base class Cleanup " << Name << std::endl;
}


// Execute all commands in the mailbox.  This is just a temporary implementation, where
// all commands in a mailbox are executed before moving on the next mailbox.  The final
// implementation will probably look at timestamps.  We may also want to pass in a
// parameter (enum) to set the mailbox processing policy.
unsigned int mtsTaskInterface::ProcessMailBoxes(void)
{
    int numberOfCommands = 0;
    ThreadResourcesMapType::iterator iterator = ThreadResourcesMap.begin();
    const ThreadResourcesMapType::iterator end = ThreadResourcesMap.end();
    for (;
         iterator != end;
         ++iterator) {
        mtsMailBox *mailBox = iterator->second->GetMailBox();
        while (mailBox->ExecuteNext()) {
            numberOfCommands++;
        }
    }
    return numberOfCommands;
}


mtsCommandVoidBase * mtsTaskInterface::GetCommandVoid(const std::string & commandName) const {
    const osaThreadId threadId = osaGetCurrentThreadId();
    ThreadResourcesMapType::const_iterator iterator = ThreadResourcesMap.begin();
    bool found = false;
    while (!found && iterator != ThreadResourcesMap.end()) {
        if ((iterator->first).Equal(threadId)) {
            found = true;
        } else {
            iterator++;
        }
    }
    if (found) {
        CMN_LOG_CLASS_INIT_VERBOSE << this->GetName()
                                   << " found thread resource to look for void command \""
                                   << commandName << "\""
                                   << std::endl;
        return iterator->second->GetCommandVoid(commandName);
    } else {
        CMN_LOG_CLASS_INIT_ERROR << this->GetName()
                                 << " can not provide void command \""
                                 << commandName
                                 << "\" to thread ["
                                 << threadId << "] as this thread did not use AllocateResourcesForCurrentThread first."
                                 << std::endl;
        return 0;
    }
}


mtsCommandWriteBase * mtsTaskInterface::GetCommandWrite(const std::string & commandName) const {
    const osaThreadId threadId = osaGetCurrentThreadId();
    ThreadResourcesMapType::const_iterator iterator = ThreadResourcesMap.begin();
    bool found = false;
    while (!found && iterator != ThreadResourcesMap.end()) {
        if ((iterator->first).Equal(threadId)) {
            found = true;
        } else {
            iterator++;
        }
    }
    if (found) {
        CMN_LOG_CLASS_INIT_VERBOSE << this->GetName()
                                   << " found thread resource to look for write command \""
                                   << commandName << "\""
                                   << std::endl;
        return iterator->second->GetCommandWrite(commandName);
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "Task " << this->GetName()
                                 << " can not provide write command \""
                                 << commandName
                                 << "\" to thread ["
                                 << threadId << "] as this thread did not use AllocateResourcesForCurrentThread first."
                                 << std::endl;
        return 0;
    }
}



unsigned int mtsTaskInterface::AllocateResourcesForCurrentThread(void)
{
    // keep track of threads using this and create a thread resource
    // per "user thread", i.e. create/clone of mailboxes for queued
    // commands.
    Mutex.Lock();
    const osaThreadId consumerId = osaGetCurrentThreadId();
    ThreadIdCountersType::iterator iterator = ThreadIdCounters.begin();
    bool found = false;
    while (!found && iterator != ThreadIdCounters.end()) {
        if ((iterator->first).Equal(consumerId)) {
            found = true;
        } else {
            iterator++;
        }
    }
    if (!found) {
        CMN_LOG_CLASS_INIT_VERBOSE << "AllocateResourcesForCurrentThread: found new thread Id [" << consumerId << "]" << std::endl;
        ThreadIdCounters.resize(ThreadIdCounters.size() + 1,
                                ThreadIdCounterPairType(consumerId, 1));
        std::stringstream mailBoxName;
        mailBoxName << this->GetName() << ThreadIdCounters.size();
        ThreadResources * newThreadResources = new ThreadResources(mailBoxName.str(),
                                                                   DEFAULT_ARG_BUFFER_LEN);
        CMN_LOG_CLASS_INIT_VERBOSE << "AllocateResourcesForCurrentThread: created mailbox " << newThreadResources->GetMailBox()->GetName()
                                   << std::endl;
        newThreadResources->CloneCommands(*this);
        ThreadResourcesMap.resize(ThreadResourcesMap.size() + 1,
                                  ThreadResourcesPairType(consumerId, newThreadResources));
        Mutex.Unlock();
        return 1;
    } else {
        CMN_LOG_CLASS_INIT_VERBOSE << "AllocateResourcesForCurrentThread: already registered thread Id (" << consumerId << ")" << std::endl;
        Mutex.Unlock();
        return (iterator->second)++;
    }
}

