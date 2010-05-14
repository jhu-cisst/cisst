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


mtsTaskInterface::mtsTaskInterface(const std::string & name, mtsTask * task,
                                   mtsCommandVoidBase * postCommandQueuedCommand):
    BaseType(name, task),
    CommandsQueuedVoid("CommandsQueuedVoid"),
    CommandsQueuedWrite("CommandsQueuedWrite"),
    PostCommandQueuedCommand(postCommandQueuedCommand),
    Mutex()
{
    CommandsQueuedVoid.SetOwner(*this);
    CommandsQueuedWrite.SetOwner(*this);
}


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
        mtsMailBox * mailBox = iterator->second->GetMailBox();
        while (mailBox->ExecuteNext()) {
            numberOfCommands++;
        }
    }
    return numberOfCommands;
}


std::vector<std::string> mtsTaskInterface::GetNamesOfCommandsVoid(void) const {
    return CommandsQueuedVoid.GetNames();
}

std::vector<std::string> mtsTaskInterface::GetNamesOfCommandsWrite(void) const {
    return CommandsQueuedWrite.GetNames();
}

mtsCommandVoidBase * mtsTaskInterface::GetCommandVoid(const std::string & commandName,
                                                      unsigned int userId) const {
    ThreadResourcesMapType::const_iterator iterator = ThreadResourcesMap.begin();
    bool found = false;
    while (!found && iterator != ThreadResourcesMap.end()) {
        if (iterator->first == userId) {
            found = true;
        } else {
            iterator++;
        }
    }
    if (found) {
        CMN_LOG_CLASS_INIT_VERBOSE << this->GetName()
                                   << " found resource to look for void command \""
                                   << commandName << "\" for user [" << userId << "]"
                                   << std::endl;
        return iterator->second->GetCommandVoid(commandName);
    } else {
        CMN_LOG_CLASS_INIT_ERROR << this->GetName()
                                 << " can not provide void command \""
                                 << commandName
                                 << "\" to user ["
                                 << userId << "] as this user did not use AllocateResources first."
                                 << std::endl;
        return 0;
    }
}


mtsCommandWriteBase * mtsTaskInterface::GetCommandWrite(const std::string & commandName,
                                                        unsigned int userId) const {
    ThreadResourcesMapType::const_iterator iterator = ThreadResourcesMap.begin();
    bool found = false;
    while (!found && iterator != ThreadResourcesMap.end()) {
        if (iterator->first == userId) {
            found = true;
        } else {
            iterator++;
        }
    }
    if (found) {
        CMN_LOG_CLASS_INIT_VERBOSE << this->GetName()
                                   << " found user resource to look for write command \""
                                   << commandName << "\" for user [" << userId << "]"
                                   << std::endl;
        return iterator->second->GetCommandWrite(commandName);
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "Task " << this->GetName()
                                 << " can not provide write command \""
                                 << commandName
                                 << "\" to user ["
                                 << userId << "] as this user did not use AllocateResources first."
                                 << std::endl;
        return 0;
    }
}


mtsCommandVoidBase* mtsTaskInterface::AddCommandVoid(mtsCommandVoidBase *command)
{
    mtsCommandQueuedVoidBase * queuedCommand = 0;
    // Call base class AddCommandVoid to add to CommandsVoid map.
    command = mtsDeviceInterface::AddCommandVoid(command);
    if (command) {
        queuedCommand = new mtsCommandQueuedVoid(0, command);
        if (!CommandsQueuedVoid.AddItem(command->GetName(), queuedCommand, CMN_LOG_LOD_INIT_ERROR)) {
            CommandsVoid.RemoveItem(command->GetName(), CMN_LOG_LOD_INIT_ERROR);
            delete queuedCommand;
            queuedCommand = 0;
            CMN_LOG_CLASS_INIT_ERROR << "AddCommandVoid: unable to add queued command \""
                                     << command->GetName() << "\"" << std::endl;
        }
    }
    return queuedCommand;
}


mtsCommandWriteBase* mtsTaskInterface::AddCommandWrite(mtsCommandWriteBase *command)
{
    mtsCommandQueuedWriteBase * queuedCommand = 0;
    // Call base class AddCommandWrite to add to CommandsWrite map.
    command = mtsDeviceInterface::AddCommandWrite(command);
    if (command) {
        // Create with no mailbox and 0 size argument queue.
        queuedCommand = new mtsCommandQueuedWriteGeneric(0, command, 0);
        if (!CommandsQueuedWrite.AddItem(command->GetName(), queuedCommand, CMN_LOG_LOD_INIT_ERROR)) {
            CommandsVoid.RemoveItem(command->GetName(), CMN_LOG_LOD_INIT_ERROR);
            delete queuedCommand;
            queuedCommand = 0;
            CMN_LOG_CLASS_INIT_ERROR << "AddCommandWrite: unable to add queued command \""
                                     << command->GetName() << "\"" << std::endl;
        }
    }
    return queuedCommand;
}

mtsCommandWriteBase* mtsTaskInterface::AddCommandFilteredWrite(mtsCommandQualifiedReadBase *filter, mtsCommandWriteBase *command)
{
    if (filter && command) {
        if (!CommandsInternal.AddItem(filter->GetName(), filter, CMN_LOG_LOD_INIT_ERROR)) {
            CMN_LOG_CLASS_INIT_ERROR << "AddCommandFilteredWrite: unable to add filter \""
                                     << command->GetName() << "\"" << std::endl;
            return 0;
        }
        // The mtsCommandWrite is called commandName because that name will be used by mtsCommandFilteredQueuedWrite.
        //  For clarity, we store it in the internal map under the name commandName+"Write".
        if (!CommandsInternal.AddItem(command->GetName()+"Write", command, CMN_LOG_LOD_INIT_ERROR)) {
            CMN_LOG_CLASS_INIT_ERROR << "AddCommandFilteredWrite: unable to add command \""
                                     << command->GetName() << "\"" << std::endl;
            CommandsInternal.RemoveItem(filter->GetName(), CMN_LOG_LOD_INIT_ERROR);
            return 0;
        }
        mtsCommandQueuedWriteBase * queuedCommand = new mtsCommandFilteredQueuedWrite(filter, command);
        if (CommandsQueuedWrite.AddItem(command->GetName(), queuedCommand, CMN_LOG_LOD_INIT_ERROR)) {
            return queuedCommand;
        } else {
            CommandsInternal.RemoveItem(filter->GetName(), CMN_LOG_LOD_INIT_ERROR);
            CommandsInternal.RemoveItem(command->GetName(), CMN_LOG_LOD_INIT_ERROR);
            delete queuedCommand;
            CMN_LOG_CLASS_INIT_ERROR << "AddCommandFilteredWrite: unable to add queued command \""
                                     << command->GetName() << "\"" << std::endl;
            return 0;
        }
    }
    return 0;
}

unsigned int mtsTaskInterface::AllocateResources(const std::string & userName)
{
    // keep track of threads using this and create a thread resource
    // per "user thread", i.e. create/clone of mailboxes for queued
    // commands.
    Mutex.Lock();
    this->UserCounter++;
    unsigned int userId = this->UserCounter;
    CMN_LOG_CLASS_INIT_VERBOSE << "AllocateResources: interface \"" << this->Name
                               << "\" created new Id [" << userId
                               << "] for user \"" << userName << "\"" << std::endl;
    std::stringstream mailBoxName;
    mailBoxName << this->GetName() << "-" << userId << "-" << userName;
    ThreadResources * newThreadResources = new ThreadResources(mailBoxName.str(),
                                                               DEFAULT_ARG_BUFFER_LEN,
                                                               this->PostCommandQueuedCommand);
    CMN_LOG_CLASS_INIT_VERBOSE << "AllocateResources: created mailbox \"" << newThreadResources->GetMailBox()->GetName()
                               << "\"" << std::endl;
    newThreadResources->CloneCommands(*this);
    ThreadResourcesMap.resize(ThreadResourcesMap.size() + 1,
                              ThreadResourcesPairType(userId, newThreadResources));
    Mutex.Unlock();
    return userId;
}

