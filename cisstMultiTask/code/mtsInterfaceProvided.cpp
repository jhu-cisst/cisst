/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2022 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnStrings.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaThreadBuddy.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsCommandQueuedVoid.h>
#include <cisstMultiTask/mtsCommandQueuedVoidReturn.h>
#include <cisstMultiTask/mtsCommandQueuedWrite.h>
#include <cisstMultiTask/mtsCommandQueuedWriteReturn.h>
#include <cisstMultiTask/mtsCommandFilteredWrite.h>
#include <cisstMultiTask/mtsCommandFilteredQueuedWrite.h>
#include <cisstMultiTask/mtsComponent.h>

#include <iostream>
#include <string>


mtsInterfaceProvided::mtsInterfaceProvided(const std::string & name, mtsComponent * component,
                                           mtsInterfaceQueueingPolicy queueingPolicy,
                                           mtsCallableVoidBase * postCommandQueuedCallable,
                                           bool isProxy):
    BaseType(name, component),
    IsProxy(isProxy),
    MailBox(0),
    QueueingPolicy(queueingPolicy),
    ArgumentQueuesSize(DEFAULT_MAIL_BOX_AND_ARGUMENT_QUEUES_SIZE),
    BlockingCommandExecuted(0),
    BlockingCommandReturnExecuted(0),
    OriginalInterface(0),
    EndUserInterface(false),
    UserName("OriginalInterface"),
    UserCounter(0),
    CommandsVoid("CommandsVoid", true),
    CommandsVoidReturn("CommandsVoidReturn", true),
    CommandsWrite("CommandsWrite", true),
    CommandsWriteReturn("CommandsWriteReturn", true),
    CommandsRead("CommandsRead", true),
    CommandsQualifiedRead("CommandsQualifiedRead", true),
    EventVoidGenerators("EventVoidGenerators", true),
    EventWriteGenerators("EventWriteGenerators", true),
    CommandsInternal("CommandsInternal", true),
    PostCommandQueuedCallable(postCommandQueuedCallable)
{
    // make sure queueing policy is set
    if (queueingPolicy == MTS_COMPONENT_POLICY) {
        CMN_LOG_CLASS_INIT_ERROR << "constructor: interface queueing policy has not been set correctly for component \""
                                 << name << "\"" << std::endl;
    }

    // consistency check
    if (postCommandQueuedCallable && (queueingPolicy == MTS_COMMANDS_SHOULD_NOT_BE_QUEUED)) {
        CMN_LOG_CLASS_INIT_ERROR << "constructor: a post command queued callable has been provided while queueing is turned off for component \""
                                 << name << "\"" << std::endl;
    }

    // set queue size, 0 meanings to queue to be used (e.g. mtsComponent)
    if (queueingPolicy == MTS_COMMANDS_SHOULD_NOT_BE_QUEUED) {
        MailBoxSize = 0;
    } else {
        MailBoxSize = DEFAULT_MAIL_BOX_AND_ARGUMENT_QUEUES_SIZE;
    }

    // set owner for all maps (to make logs more readable)
    CommandsVoid.SetOwner(*this);
    CommandsVoidReturn.SetOwner(*this);
    CommandsWrite.SetOwner(*this);
    CommandsWriteReturn.SetOwner(*this);
    CommandsRead.SetOwner(*this);
    CommandsQualifiedRead.SetOwner(*this);
    EventVoidGenerators.SetOwner(*this);
    EventWriteGenerators.SetOwner(*this);
    CommandsInternal.SetOwner(*this);
}

template <class _MapType, class _QueuedType>
void mtsInterfaceProvided::CloneCommands(const std::string &cmdType, const _MapType &CommandMapIn, _MapType &CommandMapOut)
{
    typename _MapType::const_iterator iter;
    typename _MapType::element_type * command;
    _QueuedType * commandQueued;
    for (iter = CommandMapIn.begin(); iter != CommandMapIn.end(); iter++) {
        commandQueued = dynamic_cast<_QueuedType *>(iter->second);
        if (commandQueued) {
            command = commandQueued->Clone(this->MailBox, ArgumentQueuesSize);
            CMN_LOG_CLASS_INIT_VERBOSE << "factory constructor: cloned queued " << cmdType << " command \"" << iter->first
                                       << "\" for \"" << this->GetFullName() << "\"" << std::endl;
        } else {
            command = iter->second;
            CMN_LOG_CLASS_INIT_VERBOSE << "factory constructor: using existing pointer on " << cmdType << " command \"" << iter->first
                                       << "\" for \"" << this->GetFullName() << "\"" << std::endl;
        }
        CommandMapOut.AddItem(iter->first, command, CMN_LOG_LEVEL_INIT_ERROR);
    }
}

mtsInterfaceProvided::mtsInterfaceProvided(mtsInterfaceProvided * originalInterface,
                                           const std::string & userName,
                                           size_t mailBoxSize,
                                           size_t argumentQueuesSize):
    BaseType(mtsInterfaceProvided::GenerateEndUserInterfaceName(originalInterface, userName),
             originalInterface->Component),
    MailBox(0),
    QueueingPolicy(MTS_COMMANDS_SHOULD_BE_QUEUED),
    MailBoxSize(mailBoxSize),
    ArgumentQueuesSize(argumentQueuesSize),
    BlockingCommandExecuted(0),
    BlockingCommandReturnExecuted(0),
    OriginalInterface(originalInterface),
    EndUserInterface(true),
    UserName(userName),
    UserCounter(0),
    CommandsVoid("CommandsVoid", true),
    CommandsVoidReturn("CommandsVoidReturn", true),
    CommandsWrite("CommandsWrite", true),
    CommandsWriteReturn("CommandsWriteReturn", true),
    CommandsRead("CommandsRead", true),
    CommandsQualifiedRead("CommandsQualifiedRead", true),
    EventVoidGenerators("EventVoidGenerators", true),
    EventWriteGenerators("EventWriteGenerators", true),
    CommandsInternal("CommandsInternal", true),
    PostCommandQueuedCallable(originalInterface->PostCommandQueuedCallable)
{
    // set owner for all maps (to make logs more readable)
    CommandsVoid.SetOwner(*this);
    CommandsVoidReturn.SetOwner(*this);
    CommandsWrite.SetOwner(*this);
    CommandsWriteReturn.SetOwner(*this);
    CommandsRead.SetOwner(*this);
    CommandsQualifiedRead.SetOwner(*this);
    EventVoidGenerators.SetOwner(*this);
    EventWriteGenerators.SetOwner(*this);
    CommandsInternal.SetOwner(*this);

    if (mailBoxSize != 0) {
        // duplicate what needs to be duplicated (i.e. void and write commands)
        MailBox = new mtsMailBox(this->GetName(),
                                 mailBoxSize,
                                 this->PostCommandQueuedCallable);

        // clone void commands
        CloneCommands<CommandVoidMapType, mtsCommandQueuedVoid>("void", originalInterface->CommandsVoid, CommandsVoid);
        // clone read commands
        CloneCommands<CommandReadMapType, mtsCommandQueuedRead>("read", originalInterface->CommandsRead, CommandsRead);
        // clone void return commands
        CloneCommands<CommandVoidReturnMapType, mtsCommandQueuedVoidReturn>("void return", originalInterface->CommandsVoidReturn, CommandsVoidReturn);
        // clone write commands
        CloneCommands<CommandWriteMapType, mtsCommandQueuedWriteBase>("write", originalInterface->CommandsWrite, CommandsWrite);
        // clone qualified read commands
        CloneCommands<CommandQualifiedReadMapType, mtsCommandQueuedQualifiedRead>("qualified read", originalInterface->CommandsQualifiedRead, CommandsQualifiedRead);
        // clone write return commands
        CloneCommands<CommandWriteReturnMapType, mtsCommandQueuedWriteReturn>("write return", originalInterface->CommandsWriteReturn, CommandsWriteReturn);
    }
}


mtsInterfaceProvided::~mtsInterfaceProvided()
{
	CMN_LOG_CLASS_INIT_VERBOSE << "Class mtsInterfaceProvided: Class destructor" << std::endl;
    // ADV: Need to add all cleanup, i.e. make sure all mailboxes are
    // properly deleted.
}


void mtsInterfaceProvided::Cleanup(void)
{
#if 0
    // Free memory for state filters (added by AddCommandFilteredReadStateInternal)
    for (size_t i = 0; i < StateTableFilters.size(); i++)
        delete StateTableFilters[i];
    StateTableFilters.clear();
#endif
#if 0 // adeguet1, adv
    InterfacesProvidedCreatedType::iterator op;
    for (op = QueuedCommands.begin(); op != QueuedCommands.end(); op++) {
        delete op->second->GetMailBox();
        delete op->second;
    }
    QueuedCommands.erase(QueuedCommands.begin(), QueuedCommands.end());
#endif
#if 0 // pk: the following might be good to add
    // Unblock any clients that may be waiting
    if (BlockingCommandExecuted)
        BlockingCommandExecuted->Execute(MTS_NOT_BLOCKING);
    if (BlockingCommandReturnExecuted)
        BlockingCommandReturnExecuted->Execute(MTS_NOT_BLOCKING);
#endif
	CMN_LOG_CLASS_INIT_VERBOSE << "Done base class Cleanup " << Name << std::endl;
}


void mtsInterfaceProvided::SetMailBoxSize(size_t desiredSize)
{
    if (this->QueueingPolicy == MTS_COMMANDS_SHOULD_NOT_BE_QUEUED) {
        CMN_LOG_CLASS_INIT_WARNING << "SetMailBoxSize: interface \"" << this->GetFullName()
                                   << "\" is not queuing commands, calling SetMailBoxSize has no effect"
                                   << std::endl;
    }
    this->MailBoxSize = desiredSize;
}


void mtsInterfaceProvided::SetArgumentQueuesSize(size_t desiredSize)
{
    if (this->QueueingPolicy == MTS_COMMANDS_SHOULD_NOT_BE_QUEUED) {
        CMN_LOG_CLASS_INIT_WARNING << "SetArgumentQueuesSize: interface \"" << this->GetFullName()
                                   << "\" is not queuing commands, calling SetArgumentQueuesSize has no effect"
                                   << std::endl;
    }
    if (desiredSize > this->MailBoxSize) {
        CMN_LOG_CLASS_INIT_WARNING << "SetArgumentQueuesSize: interface \"" << this->GetFullName()
                                   << "\" new size (" << desiredSize
                                   << ") is smaller than command mail box size (" << this->MailBoxSize
                                   << "), the extra space won't be used" << std::endl;
    }
    this->ArgumentQueuesSize = desiredSize;
}


void mtsInterfaceProvided::SetMailBoxAndArgumentQueuesSize(size_t desiredSize)
{
    this->SetMailBoxSize(desiredSize);
    this->SetArgumentQueuesSize(desiredSize);
}



// Execute all commands in the mailbox.  This is just a temporary implementation, where
// all commands in a mailbox are executed before moving on the next mailbox.  The final
// implementation will probably look at timestamps.  We may also want to pass in a
// parameter (enum) to set the mailbox processing policy.
size_t mtsInterfaceProvided::ProcessMailBoxes(void)
{
    if (!this->EndUserInterface) {
        size_t numberOfCommands = 0;
        InterfaceProvidedCreatedListType::iterator iterator = InterfacesProvidedCreated.begin();
        //const InterfaceProvidedCreatedVectorType::iterator end = InterfacesProvidedCreated.end();
        mtsMailBox * mailBox;
        for (;
             //iterator != end;
             iterator != InterfacesProvidedCreated.end();
             ++iterator) {
            mailBox = iterator->second->GetMailBox();
            if (mailBox) {
                // process everything that is available now
                size_t commandsInMailbox = mailBox->GetAvailable();
                while (commandsInMailbox && mailBox->ExecuteNext()) {
                    numberOfCommands++;
                    commandsInMailbox--;
                }
                // process whatever arrived while queue was being
                // processed to reduce latency on client side
                commandsInMailbox = mailBox->GetAvailable();
                while (commandsInMailbox && mailBox->ExecuteNext()) {
                    numberOfCommands++;
                    commandsInMailbox--;
                }
            }
        }
        return numberOfCommands;
    }
    CMN_LOG_CLASS_RUN_ERROR << "ProcessMailBoxes: called on end user interface for " << this->GetFullName() << std::endl;
    return 0;
}


void mtsInterfaceProvided::ToStream(std::ostream & outputStream) const
{
    outputStream << "Provided Interface \"" << this->GetFullName() << "\"" << std::endl;
    CommandsVoid.ToStream(outputStream);
    CommandsVoidReturn.ToStream(outputStream);
    CommandsWrite.ToStream(outputStream);
    CommandsWriteReturn.ToStream(outputStream);
    CommandsRead.ToStream(outputStream);
    CommandsQualifiedRead.ToStream(outputStream);
    EventVoidGenerators.ToStream(outputStream);
    EventWriteGenerators.ToStream(outputStream);
}



mtsMailBox * mtsInterfaceProvided::GetMailBox(void)
{
    return this->MailBox;
}


bool mtsInterfaceProvided::UseQueueBasedOnInterfacePolicy(mtsCommandQueueingPolicy queueingPolicy,
                                                          const std::string & methodName,
                                                          const std::string & commandName)
{
    if (queueingPolicy == MTS_INTERFACE_COMMAND_POLICY) {
        if (this->QueueingPolicy == MTS_COMMANDS_SHOULD_BE_QUEUED) {
            return true;
        } else {
            return false;
        }
    }
    if (queueingPolicy == MTS_COMMAND_NOT_QUEUED) {
        // send warning if queueing is "disabled"
        if (this->QueueingPolicy == MTS_COMMANDS_SHOULD_BE_QUEUED) {
            CMN_LOG_CLASS_INIT_VERBOSE << methodName << ": adding non queued command \""
                                       << commandName << "\" to provided interface \""
                                       << this->GetFullName()
                                       << "\" which has beed created with policy MTS_COMMANDS_SHOULD_BE_QUEUED, thread safety has to be provided by the underlying method"
                                       << std::endl;
        } else {
            // send message to tell explicit queueing policy is useless
            CMN_LOG_CLASS_INIT_DEBUG << methodName << ": adding non queued command \""
                                     << commandName << "\" to provided interface \""
                                     << this->GetFullName()
                                     << "\" which has beed created with policy MTS_COMMANDS_SHOULD_NOT_BE_QUEUED, this is the default therefore there is no need to explicitely define the queueing policy"
                                     << std::endl;
        }
        return false;
    }
    if (queueingPolicy == MTS_COMMAND_QUEUED) {
        // send error if the interface has no mailbox, can not queue
        if (this->QueueingPolicy == MTS_COMMANDS_SHOULD_BE_QUEUED) {
            // send message to tell explicit queueing policy is useless
            CMN_LOG_CLASS_INIT_DEBUG << methodName << ": adding queued command \""
                                     << commandName << "\" to provided interface \""
                                     << this->GetFullName()
                                     << "\" which has beed created with policy MTS_COMMANDS_SHOULD_BE_QUEUED, this is the default therefore there is no need to explicitely define the queueing policy"
                                     << std::endl;
            return true;
        } else {
            // this is a case we can not handle
            CMN_LOG_CLASS_INIT_ERROR << methodName << ": adding queued command \""
                                     << commandName << "\" to provided interface \""
                                     << this->GetFullName()
                                     << "\" which has beed created with policy MTS_COMMANDS_SHOULD_NOT_BE_QUEUED is not possible.  The command will NOT be queued"
                                     << std::endl;
            return false;
        }
    }
    CMN_LOG_CLASS_INIT_ERROR << "UseQueueBasedOnInterfacePolicy: this case should nerver happen" << std::endl;
    return false;
}


mtsCommandVoid * mtsInterfaceProvided::AddCommandVoid(mtsCallableVoidBase * callable,
                                                      const std::string & name,
                                                      mtsCommandQueueingPolicy queueingPolicy)
{
    // check that the input is valid
    if (callable) {
        // determine if this should be a queued command or not
        bool queued = this->UseQueueBasedOnInterfacePolicy(queueingPolicy, "AddCommandVoid", name);
        if (!queued) {
            mtsCommandVoid * command = new mtsCommandVoid(callable, name);
            if (!CommandsVoid.AddItem(name, command, CMN_LOG_LEVEL_INIT_ERROR)) {
                CMN_LOG_CLASS_INIT_ERROR << "AddCommandVoid: unable to add command \""
                                         << command->GetName() << "\"" << std::endl;
                delete command;
                command = 0;
            }
            return command;
        } else {
            // create with no mailbox
            mtsCommandQueuedVoid * queuedCommand = new mtsCommandQueuedVoid(callable, name, 0, 0);
            if (!CommandsVoid.AddItem(name, queuedCommand, CMN_LOG_LEVEL_INIT_ERROR)) {
                CMN_LOG_CLASS_INIT_ERROR << "AddCommandVoid: unable to add queued command \""
                                         << queuedCommand->GetName() << "\"" << std::endl;
                delete queuedCommand;
                queuedCommand = 0;
            }
            return queuedCommand;
        }
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddCommandVoid: attempt to add undefined command (null callable pointer) to interface \""
                             << this->GetFullName() << "\"" << std::endl;
    return 0;
}


mtsCommandVoid * mtsInterfaceProvided::AddCommandVoid(mtsCommandVoid * command)
{
    // check that the input is valid
    if (command) {
        if (!CommandsVoid.AddItem(command->GetName(), command, CMN_LOG_LEVEL_INIT_ERROR)) {
            CMN_LOG_CLASS_INIT_ERROR << "AddCommandVoid: unable to add command \""
                                     << command->GetName() << "\"" << std::endl;
            command = 0;
        }
        return command;
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddCommandVoid: attempt to add undefined command (null command pointer) to interface \""
                             << this->GetFullName() << "\"" << std::endl;
    return 0;
}


mtsCommandVoidReturn * mtsInterfaceProvided::AddCommandVoidReturn(mtsCallableVoidReturnBase * callable,
                                                                  const std::string & name,
                                                                  const mtsGenericObject * resultPrototype,
                                                                  mtsCommandQueueingPolicy queueingPolicy)
{
    // check that the input is valid
    if (callable) {
        // determine if this should be a queued command or not
        bool queued = this->UseQueueBasedOnInterfacePolicy(queueingPolicy, "AddCommandVoidReturn", name);
        if (!queued) {
            mtsCommandVoidReturn * command = new mtsCommandVoidReturn(callable, name, resultPrototype);
            if (!CommandsVoidReturn.AddItem(name, command, CMN_LOG_LEVEL_INIT_ERROR)) {
                CMN_LOG_CLASS_INIT_ERROR << "AddCommandVoidReturn: unable to add command \""
                                         << command->GetName() << "\"" << std::endl;
                delete command;
                command = 0;
            }
            return command;
        } else {
            // create with no mailbox
            mtsCommandQueuedVoidReturn * queuedCommand = new mtsCommandQueuedVoidReturn(callable, name, resultPrototype, 0, 0);
            if (!CommandsVoidReturn.AddItem(name, queuedCommand, CMN_LOG_LEVEL_INIT_ERROR)) {
                CMN_LOG_CLASS_INIT_ERROR << "AddCommandVoidReturn: unable to add queued command \""
                                         << queuedCommand->GetName() << "\"" << std::endl;
                delete queuedCommand;
                queuedCommand = 0;
            }
            return queuedCommand;
        }
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddCommandVoidReturn: attempt to add undefined command (null callable pointer) to interface \""
                             << this->GetFullName() << "\"" << std::endl;
    return 0;
}


mtsCommandVoidReturn * mtsInterfaceProvided::AddCommandVoidReturn(mtsCommandVoidReturn * command)
{
    // check that the input is valid
    if (command) {
        if (!CommandsVoidReturn.AddItem(command->GetName(), command, CMN_LOG_LEVEL_INIT_ERROR)) {
            CMN_LOG_CLASS_INIT_ERROR << "AddCommandVoidReturn: unable to add command \""
                                     << command->GetName() << "\"" << std::endl;
            command = 0;
        }
        return command;
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddCommandVoidReturn: attempt to add undefined command (null command pointer) to interface \""
                             << this->GetFullName() << "\"" << std::endl;
    return 0;
}


mtsCommandWriteBase * mtsInterfaceProvided::AddCommandWrite(mtsCommandWriteBase * command, mtsCommandQueueingPolicy queueingPolicy)
{
    // check that the input is valid
    if (command) {
        // determine if this should be a queued command or not
        bool queued = this->UseQueueBasedOnInterfacePolicy(queueingPolicy, "AddCommandWrite", command->GetName());
        // check if command is already queued
        mtsCommandQueuedWriteBase * queuedCommand = dynamic_cast<mtsCommandQueuedWriteBase *>(command);
        if (!queued) {
            if (queuedCommand) {
                CMN_LOG_CLASS_INIT_WARNING << "AddCommandWrite: command \"" << command->GetName()
                                           << "\" is queued, but should not be" << std::endl;
            }
            if (!CommandsWrite.AddItem(command->GetName(), command, CMN_LOG_LEVEL_INIT_ERROR)) {
                CMN_LOG_CLASS_INIT_ERROR << "AddCommandWrite: unable to add command \""
                                         << command->GetName() << "\"" << std::endl;
                command = 0;
            }
            return command;
        } else {
            bool wasCreated = false;
            if (!queuedCommand) {
                // if not already queued, create with no mailbox
                queuedCommand = new mtsCommandQueuedWriteGeneric(0, command, 0);
                wasCreated = true;
            }
            if (!CommandsWrite.AddItem(command->GetName(), queuedCommand, CMN_LOG_LEVEL_INIT_ERROR)) {
                CMN_LOG_CLASS_INIT_ERROR << "AddCommandWrite: unable to add queued command \""
                                         << queuedCommand->GetName() << "\"" << std::endl;
                if (wasCreated) delete queuedCommand;
                queuedCommand = 0;
            }
            return queuedCommand;
        }
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddCommandWrite: attempt to add undefined command (null pointer) to interface \""
                             << this->GetFullName() << "\"" << std::endl;
    return 0;
}


mtsCommandWriteReturn * mtsInterfaceProvided::AddCommandWriteReturn(mtsCallableWriteReturnBase * callable,
                                                                    const std::string & name,
                                                                    const mtsGenericObject * argumentPrototype,
                                                                    const mtsGenericObject * resultPrototype,
                                                                    mtsCommandQueueingPolicy queueingPolicy)
{
    // check that the input is valid
    if (callable) {
        // determine if this should be a queued command or not
        bool queued = this->UseQueueBasedOnInterfacePolicy(queueingPolicy, "AddCommandWriteReturn", name);
        if (!queued) {
            mtsCommandWriteReturn * command = new mtsCommandWriteReturn(callable, name, argumentPrototype, resultPrototype);
            if (!CommandsWriteReturn.AddItem(name, command, CMN_LOG_LEVEL_INIT_ERROR)) {
                CMN_LOG_CLASS_INIT_ERROR << "AddCommandWriteReturn: unable to add command \""
                                         << command->GetName() << "\"" << std::endl;
                delete command;
                command = 0;
            }
            return command;
        } else {
            // create with no mailbox
            mtsCommandQueuedWriteReturn * queuedCommand = new mtsCommandQueuedWriteReturn(callable, name, argumentPrototype, resultPrototype, 0, 0);
            if (!CommandsWriteReturn.AddItem(name, queuedCommand, CMN_LOG_LEVEL_INIT_ERROR)) {
                CMN_LOG_CLASS_INIT_ERROR << "AddCommandWriteReturn: unable to add queued command \""
                                         << queuedCommand->GetName() << "\"" << std::endl;
                delete queuedCommand;
                queuedCommand = 0;
            }
            return queuedCommand;
        }
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddCommandWriteReturn: attempt to add undefined command (null callable pointer) to interface \""
                             << this->GetFullName() << "\"" << std::endl;
    return 0;
}


mtsCommandWriteReturn * mtsInterfaceProvided::AddCommandWriteReturn(mtsCommandWriteReturn * command)
{
    // check that the input is valid
    if (command) {
        if (!CommandsWriteReturn.AddItem(command->GetName(), command, CMN_LOG_LEVEL_INIT_ERROR)) {
            CMN_LOG_CLASS_INIT_ERROR << "AddCommandWriteReturn: unable to add command \""
                                     << command->GetName() << "\"" << std::endl;
            command = 0;
        }
        return command;
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddCommandWriteReturn: attempt to add undefined command (null command pointer) to interface \""
                             << this->GetFullName() << "\"" << std::endl;
    return 0;
}


mtsCommandRead * mtsInterfaceProvided::AddCommandRead(mtsCallableReadBase * callable,
                                                      const std::string & name,
                                                      const mtsGenericObject * argumentPrototype)
{
    // check that the input is valid
    if (callable) {
        mtsCommandRead * command = new mtsCommandRead(callable, name, argumentPrototype);
        if (!CommandsRead.AddItem(name, command, CMN_LOG_LEVEL_INIT_ERROR)) {
            CMN_LOG_CLASS_INIT_ERROR << "AddCommandRead: unable to add command \""
                                     << command->GetName() << "\"" << std::endl;
            delete command;
            command = 0;
        }
        return command;
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddCommandRead: attempt to add undefined command (null callable pointer) to interface \""
                             << this->GetFullName() << "\"" << std::endl;
    return 0;
}


mtsCommandRead * mtsInterfaceProvided::AddCommandRead(mtsCommandRead * command)
{
    if (command) {
        if (!CommandsRead.AddItem(command->GetName(), command, CMN_LOG_LEVEL_INIT_ERROR)) {
            CMN_LOG_CLASS_INIT_ERROR << "AddCommandRead: unable to add command \""
                                     << command->GetName() << "\"" << std::endl;
            command = 0;
        }
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "AddCommandRead: unable to create command \""
                                 << command->GetName() << "\"" << std::endl;
    }
    return command;
}


mtsCommandQualifiedRead * mtsInterfaceProvided::AddCommandQualifiedRead(mtsCallableQualifiedReadBase * callable,
                                                                        const std::string & name,
                                                                        const mtsGenericObject * argument1Prototype,
                                                                        const mtsGenericObject * argument2Prototype)
{
    // check that the input is valid
    if (callable) {
        mtsCommandQualifiedRead * command =
            new mtsCommandQualifiedRead(callable, name, argument1Prototype, argument2Prototype);
        if (!CommandsQualifiedRead.AddItem(name, command, CMN_LOG_LEVEL_INIT_ERROR)) {
            CMN_LOG_CLASS_INIT_ERROR << "AddCommandQualifiedRead: unable to add command \""
                                     << command->GetName() << "\"" << std::endl;
            delete command;
            command = 0;
        }
        return command;
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddCommandQualifiedRead: attempt to add undefined command (null callable pointer) to interface \""
                             << this->GetName() << "\"" << std::endl;
    return 0;
}


mtsCommandQualifiedRead * mtsInterfaceProvided::AddCommandQualifiedRead(mtsCommandQualifiedRead * command)
{
    if (command) {
        if (!CommandsQualifiedRead.AddItem(command->GetName(), command, CMN_LOG_LEVEL_INIT_ERROR)) {
            CMN_LOG_CLASS_INIT_ERROR << "AddCommandQualifiedRead: unable to add command \""
                                     << command->GetName() << "\"" << std::endl;
            command = 0;
        }
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "AddCommandQualifiedRead: unable to create command \""
                                 << command->GetName() << "\"" << std::endl;
    }
    return command;
}


mtsCommandWriteBase * mtsInterfaceProvided::AddCommandFilteredWrite(mtsCommandQualifiedRead * filter,
                                                                    mtsCommandWriteBase * command,
                                                                    mtsCommandQueueingPolicy queueingPolicy)
{
    if (filter && command) {
        if (!CommandsInternal.AddItem(filter->GetName(), filter, CMN_LOG_LEVEL_INIT_ERROR)) {
            CMN_LOG_CLASS_INIT_ERROR << "AddCommandFilteredWrite: unable to add filter \""
                                     << command->GetName() << "\"" << std::endl;
            return 0;
        }
        // The mtsCommandWrite is called commandName because that name will be used by mtsCommandFilteredQueuedWrite.
        //  For clarity, we store it in the internal map under the name commandName+"Write".
        if (!CommandsInternal.AddItem(command->GetName()+"Write", command, CMN_LOG_LEVEL_INIT_ERROR)) {
            CMN_LOG_CLASS_INIT_ERROR << "AddCommandFilteredWrite: unable to add command \""
                                     << command->GetName() << "\"" << std::endl;
            CommandsInternal.RemoveItem(filter->GetName(), CMN_LOG_LEVEL_INIT_ERROR);
            return 0;
        }
        bool queued = this->UseQueueBasedOnInterfacePolicy(queueingPolicy, "AddCommandFilteredWrite", command->GetName());
        mtsCommandWriteBase * filteredCommand;
        if (!queued) {
            filteredCommand = new mtsCommandFilteredWrite(filter, command);
        } else {
            filteredCommand = new mtsCommandFilteredQueuedWrite(filter, command);
        }
        if (filteredCommand && CommandsWrite.AddItem(command->GetName(), filteredCommand, CMN_LOG_LEVEL_INIT_ERROR)) {
            return filteredCommand;
        } else {
            CommandsInternal.RemoveItem(filter->GetName(), CMN_LOG_LEVEL_INIT_ERROR);
            CommandsInternal.RemoveItem(command->GetName(), CMN_LOG_LEVEL_INIT_ERROR);
            if (filteredCommand) {
                delete filteredCommand;
            }
            CMN_LOG_CLASS_INIT_ERROR << "AddCommandFilteredWrite: unable to add queued command \""
                                     << command->GetName() << "\"" << std::endl;
            return 0;
        }
    }
    return 0;
}


std::string mtsInterfaceProvided::GenerateEndUserInterfaceName(const mtsInterfaceProvided * originalInterface,
                                                               const std::string & userName)
{
    return originalInterface->GetName() + "[" + userName + "]";
}


// Protected function, should only be called from mtsComponent
mtsInterfaceProvided * mtsInterfaceProvided::GetEndUserInterface(const std::string & userName)
{
    // note that we don't check for duplicate user names (don't need
    // to care whether there are duplicates)
    this->UserCounter++;
    this->UserName = userName;
    CMN_LOG_CLASS_INIT_VERBOSE << "GetEndUserInterface: interface \"" << this->GetFullName()
                               << "\" creating new copy (#" << this->UserCounter
                               << ") for user \"" << userName << "\"" << std::endl;
    // new end user interface created with default size for mailbox; also adds system events
    mtsInterfaceProvided * interfaceProvided = new mtsInterfaceProvided(this,
                                                                        userName,
                                                                        this->MailBoxSize,
                                                                        this->ArgumentQueuesSize);
    InterfacesProvidedCreated.push_back(InterfaceProvidedCreatedPairType(this->UserCounter, interfaceProvided));

    // finally, add system events
    if (!this->IsProxy) {
        interfaceProvided->AddSystemEvents();
    }

    return interfaceProvided;
}


std::vector<std::string> mtsInterfaceProvided::GetListOfUserNames(void) const
{
    std::vector<std::string> userNames;

    if (!OriginalInterface) {
        return userNames;
    }

    const InterfaceProvidedCreatedListType::const_iterator end = InterfacesProvidedCreated.end();
    InterfaceProvidedCreatedListType::const_iterator iterator;
    for (iterator = InterfacesProvidedCreated.begin();
         iterator != end; ++iterator) {
        userNames.push_back(iterator->second->UserName);
    }

    return userNames;
}


// Remove the end-user interface specified by the parameter interfaceProvided.
// Note that there are two mtsInterfaceProvided objects:  (1) the interfaceProvided parameter, which should be a
// pointer to the end-user interface to be removed and (2) the "this" pointer, which should point to the original interface.
mtsInterfaceProvided * mtsInterfaceProvided::RemoveEndUserInterface(mtsInterfaceProvided *interfaceProvided,
                                                                    const std::string & userName)
{
    // First, do some error checking
    // 1) Make sure interfaceProvided is non-zero
    if (!interfaceProvided) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveEndUserInterface: interface \"" << this->GetFullName()
                                << "\": null provided interface" << std::endl;
        return interfaceProvided;
    }
    // 2) The interfaceProvided parameter should be an end-user interface
    if (!interfaceProvided->EndUserInterface) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveEndUserInterface: interface \"" << this->GetFullName()
                                << "\": parameter not an end-user interface" << std::endl;
        return interfaceProvided;
    }
    // 3) This object should be an original interface (i.e., the OriginalInterface pointer should be 0)
    if (this->OriginalInterface) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveEndUserInterface: interface \"" << this->GetFullName()
                                << "\": called on object that is not an original interface" << std::endl;
        return (interfaceProvided ? interfaceProvided : this->OriginalInterface);
    }

    // now, handle the case where this object is also an end-user
    // interface, which would occur when there are no queued commands.
    if (this->EndUserInterface) {
        if (interfaceProvided && (interfaceProvided != this)) {
            CMN_LOG_CLASS_RUN_WARNING << "RemoveEndUserInterface: interface \"" << this->GetFullName()
                                      << "\": original interface inconsistent with provided end-user interface" << std::endl;
        }
        CMN_LOG_CLASS_RUN_VERBOSE << "RemoveEndUserInterface: interface \"" << this->GetFullName()
                                  << "\": original interface is also the end-user interface" << std::endl;
        return 0;
    }

    // finally, remove the end-user interface from the list of
    // end-user interfaces (InterfacesProvidedCreated).
    const InterfaceProvidedCreatedListType::iterator end = InterfacesProvidedCreated.end();
    InterfaceProvidedCreatedListType::iterator iterator;
    for (iterator = InterfacesProvidedCreated.begin();
         iterator != end;
         ++iterator) {
        if (iterator->second == interfaceProvided) {
            CMN_LOG_CLASS_RUN_VERBOSE << "RemoveEndUserInterface: interface \"" << this->GetFullName()
                                      << "\" removing copy (#" << iterator->first
                                      << ") for user \"" << userName << "\"" << std::endl;
            InterfacesProvidedCreated.erase(iterator);
            delete interfaceProvided;
            return 0;
        }
    }

    CMN_LOG_CLASS_RUN_ERROR << "RemoveEndUserInterface: interface \"" << this->GetFullName()
                            << "\" could not find end-user interface for user \""
                            << userName << "\"" << std::endl;
    return interfaceProvided;
}


mtsInterfaceProvided * mtsInterfaceProvided::GetOriginalInterface(void) const
{
    return this->OriginalInterface;
}


mtsInterfaceProvided * mtsInterfaceProvided::FindEndUserInterfaceByName(const std::string & userName)
{
    // First, check if there is just a single provided interface (i.e., no queued commands)
    if ((this->OriginalInterface == 0) && this->EndUserInterface) {
        return this;
    }
    const InterfaceProvidedCreatedListType::const_iterator end = InterfacesProvidedCreated.end();
    InterfaceProvidedCreatedListType::const_iterator iterator;
    for (iterator = InterfacesProvidedCreated.begin();
         iterator != end;
         ++iterator) {
        if (iterator->second->UserName == userName) {
            return iterator->second;
        }
    }
    return 0;
}


int mtsInterfaceProvided::GetNumberOfEndUsers(void) const
{
    return static_cast<int>(InterfacesProvidedCreated.size());
}


mtsCommandVoid * mtsInterfaceProvided::AddEventVoid(const std::string & eventName)
{
    mtsMulticastCommandVoid * eventMulticastCommand = new mtsMulticastCommandVoid(eventName);
    if (eventMulticastCommand) {
        if (AddEvent(eventName, eventMulticastCommand)) {
            return eventMulticastCommand;
        }
        delete eventMulticastCommand;
        CMN_LOG_CLASS_INIT_ERROR << "AddEventVoid: unable to add event \""
                                 << eventName << "\"" << std::endl;
        return 0;
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddEventVoid: unable to create multi-cast command for event \""
                             << eventName << "\"" << std::endl;
    return 0;
}


bool mtsInterfaceProvided::AddEventVoid(mtsFunctionVoid & eventTrigger,
                                        const std::string & eventName)
{
    mtsCommandVoid * command;
    command = this->AddEventVoid(eventName);
    if (command) {
        eventTrigger.Bind(command);
        return true;
    }
    return false;
}


mtsCommandWriteBase * mtsInterfaceProvided::AddEventWriteGeneric(const std::string & eventName,
                                                                 const mtsGenericObject & argumentPrototype) {
    mtsMulticastCommandWriteBase * eventMulticastCommand = new mtsMulticastCommandWriteGeneric(eventName, argumentPrototype);
    if (eventMulticastCommand) {
        if (AddEvent(eventName, eventMulticastCommand)) {
            return eventMulticastCommand;
        }
        delete eventMulticastCommand;
        CMN_LOG_CLASS_INIT_ERROR << "AddEventWriteGeneric: unable to add event \""
                                 << eventName << "\"" << std::endl;
        return 0;
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddEventWriteGeneric: unable to create multi-cast command for event \""
                             << eventName << "\"" << std::endl;
    return 0;
}


bool mtsInterfaceProvided::AddEventWriteGeneric(mtsFunctionWrite & eventTrigger, const std::string & eventName,
                                                const mtsGenericObject & argumentPrototype) {
    mtsCommandWriteBase * command;
    command = this->AddEventWriteGeneric(eventName, argumentPrototype);
    if (command) {
        eventTrigger.Bind(command);
        return true;
    }
    return false;
}


bool mtsInterfaceProvided::AddEvent(const std::string & name, mtsMulticastCommandVoid * generator)
{
    if (EventWriteGenerators.GetItem(name, CMN_LOG_LEVEL_NONE)) {
        // Is this check really needed?
        CMN_LOG_CLASS_INIT_VERBOSE << "AddEvent (void): event " << name << " already exists as write event, ignored." << std::endl;
        return false;
    }
    return EventVoidGenerators.AddItem(name, generator, CMN_LOG_LEVEL_INIT_ERROR);
}


bool mtsInterfaceProvided::AddEvent(const std::string & name, mtsMulticastCommandWriteBase * generator)
{
    if (EventVoidGenerators.GetItem(name, CMN_LOG_LEVEL_NONE)) {
        // Is this check really needed?
        CMN_LOG_CLASS_INIT_VERBOSE << "AddEvent (write): event " << name << " already exists as void event, ignored." << std::endl;
        return false;
    }
    return EventWriteGenerators.AddItem(name, generator, CMN_LOG_LEVEL_INIT_ERROR);
}

bool mtsInterfaceProvided::IsSystemEventVoid(const std::string & name)
{
    return ((name == "BlockingCommandExecuted") || (name == "BlockingCommandReturnExecuted"));
}

bool mtsInterfaceProvided::AddSystemEvents(void)
{
    this->BlockingCommandExecuted = AddEventVoid("BlockingCommandExecuted");
    if (!(this->BlockingCommandExecuted)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddSystemEvents: unable to add void event \"BlockingCommandExecuted\" to interface \""
                                 << this->GetFullName() << "\"" << std::endl;
        return false;
    }
    if (this->MailBox) {
        MailBox->SetPostCommandDequeuedCommand(this->BlockingCommandExecuted);
    } else {
        CMN_LOG_CLASS_INIT_VERBOSE << "AddSystemEvents: can not set mailbox post dequeued command for blocking commands for interface \""
                                   << this->GetFullName() << "\"" << std::endl;
    }

    this->BlockingCommandReturnExecuted = AddEventVoid("BlockingCommandReturnExecuted");
    if (!(this->BlockingCommandReturnExecuted)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddSystemEvents: unable to add void event \"BlockingCommandReturnExecuted\" to interface \""
                                 << this->GetFullName() << "\"" << std::endl;
        return false;
    }
    if (this->MailBox) {
        MailBox->SetPostCommandReturnDequeuedCommand(this->BlockingCommandReturnExecuted);
    } else {
        CMN_LOG_CLASS_INIT_VERBOSE << "AddSystemEvents: can not set mailbox post dequeued command for blocking return commands for interface \""
                                   << this->GetFullName() << "\"" << std::endl;
    }
    return true;
}


std::vector<std::string> mtsInterfaceProvided::GetNamesOfCommands(void) const
{
    std::vector<std::string> commands = GetNamesOfCommandsVoid();
    std::vector<std::string> tmp;

    tmp = GetNamesOfCommandsVoidReturn();
    commands.insert(commands.begin(), tmp.begin(), tmp.end());
    tmp.clear();

    tmp = GetNamesOfCommandsWrite();
    commands.insert(commands.begin(), tmp.begin(), tmp.end());
    tmp.clear();

    tmp = GetNamesOfCommandsWriteReturn();
    commands.insert(commands.begin(), tmp.begin(), tmp.end());
    tmp.clear();

    tmp = GetNamesOfCommandsRead();
    commands.insert(commands.begin(), tmp.begin(), tmp.end());
    tmp.clear();

    tmp = GetNamesOfCommandsQualifiedRead();
    commands.insert(commands.begin(), tmp.begin(), tmp.end());
    return commands;
}


std::vector<std::string> mtsInterfaceProvided::GetNamesOfCommandsVoid(void) const
{
    return CommandsVoid.GetNames();
}


std::vector<std::string> mtsInterfaceProvided::GetNamesOfCommandsVoidReturn(void) const
{
    return CommandsVoidReturn.GetNames();
}


std::vector<std::string> mtsInterfaceProvided::GetNamesOfCommandsWrite(void) const
{
    return CommandsWrite.GetNames();
}


std::vector<std::string> mtsInterfaceProvided::GetNamesOfCommandsWriteReturn(void) const
{
    return CommandsWriteReturn.GetNames();
}


std::vector<std::string> mtsInterfaceProvided::GetNamesOfCommandsRead(void) const
{
    return CommandsRead.GetNames();
}


std::vector<std::string> mtsInterfaceProvided::GetNamesOfCommandsQualifiedRead(void) const
{
    return CommandsQualifiedRead.GetNames();
}


std::vector<std::string> mtsInterfaceProvided::GetNamesOfEventsVoid(void) const
{
    // should also get names of events defined in the end-user interface
    if (this->OriginalInterface) {
        return this->OriginalInterface->EventVoidGenerators.GetNames();
    }
    return EventVoidGenerators.GetNames();
}


std::vector<std::string> mtsInterfaceProvided::GetNamesOfEventsWrite(void) const
{
    if (this->OriginalInterface) {
        return this->OriginalInterface->EventWriteGenerators.GetNames();
    }
    return EventWriteGenerators.GetNames();
}


mtsCommandVoid * mtsInterfaceProvided::GetCommandVoid(const std::string & commandName,
                                                      const mtsRequiredType required) const
{
    cmnLogLevel logLevel;
    if (required == MTS_REQUIRED) {
        logLevel = CMN_LOG_LEVEL_INIT_ERROR;
    } else {
        logLevel = CMN_LOG_LEVEL_INIT_VERBOSE;
    }
    if (this->EndUserInterface) {
        if (this->MailBox) {
            return this->CommandsVoid.GetItem(commandName, logLevel);
        } else {
            CMN_ASSERT(this->OriginalInterface);
            return this->OriginalInterface->CommandsVoid.GetItem(commandName, logLevel);
        }
    }
    CMN_LOG_CLASS_INIT_ERROR << "GetCommandVoid: cannot retrieve command " << commandName << " from \"factory\" interface \""
                             << this->GetFullName()
                             << "\", you must call GetEndUserInterface to make sure you are using an end-user interface"
                             << std::endl;
    return 0;
}


mtsCommandVoidReturn * mtsInterfaceProvided::GetCommandVoidReturn(const std::string & commandName,
                                                                  const mtsRequiredType required) const
{
    cmnLogLevel logLevel;
    if (required == MTS_REQUIRED) {
        logLevel = CMN_LOG_LEVEL_INIT_ERROR;
    } else {
        logLevel = CMN_LOG_LEVEL_INIT_VERBOSE;
    }
    if (this->EndUserInterface) {
        if (this->MailBox) {
            return this->CommandsVoidReturn.GetItem(commandName, logLevel);
        } else {
            CMN_ASSERT(this->OriginalInterface);
            return this->OriginalInterface->CommandsVoidReturn.GetItem(commandName, logLevel);
        }
    }
    CMN_LOG_CLASS_INIT_ERROR << "GetCommandVoidReturn: cannot retrieve command " << commandName << " from \"factory\" interface \""
                             << this->GetFullName()
                             << "\", you must call GetEndUserInterface to make sure you are using an end-user interface"
                             << std::endl;
    return 0;
}


mtsCommandWriteBase * mtsInterfaceProvided::GetCommandWrite(const std::string & commandName,
                                                            const mtsRequiredType required) const
{
    cmnLogLevel logLevel;
    if (required == MTS_REQUIRED) {
        logLevel = CMN_LOG_LEVEL_INIT_ERROR;
    } else {
        logLevel = CMN_LOG_LEVEL_INIT_VERBOSE;
    }
    if (this->EndUserInterface) {
        if (this->MailBox) {
            return this->CommandsWrite.GetItem(commandName, logLevel);
        } else {
            CMN_ASSERT(this->OriginalInterface);
            return this->OriginalInterface->CommandsWrite.GetItem(commandName, logLevel);
        }
    }
    CMN_LOG_CLASS_INIT_ERROR << "GetCommandWrite: cannot retrieve command " << commandName << " from \"factory\" interface \""
                             << this->GetFullName()
                             << "\", you must call GetEndUserInterface to make sure you are using an end-user interface"
                             << std::endl;
    return 0;
}


mtsCommandWriteReturn * mtsInterfaceProvided::GetCommandWriteReturn(const std::string & commandName,
                                                                    const mtsRequiredType required) const
{
    cmnLogLevel logLevel;
    if (required == MTS_REQUIRED) {
        logLevel = CMN_LOG_LEVEL_INIT_ERROR;
    } else {
        logLevel = CMN_LOG_LEVEL_INIT_VERBOSE;
    }
    if (this->EndUserInterface) {
        if (this->MailBox) {
            return this->CommandsWriteReturn.GetItem(commandName, logLevel);
        } else {
            CMN_ASSERT(this->OriginalInterface);
            return this->OriginalInterface->CommandsWriteReturn.GetItem(commandName, logLevel);
        }
    }
    CMN_LOG_CLASS_INIT_ERROR << "GetCommandWriteReturn: cannot retrieve command " << commandName << " from \"factory\" interface \""
                             << this->GetFullName()
                             << "\", you must call GetEndUserInterface to make sure you are using an end-user interface"
                             << std::endl;
    return 0;
}


mtsCommandRead * mtsInterfaceProvided::GetCommandRead(const std::string & commandName,
                                                      const mtsRequiredType required) const
{
    cmnLogLevel logLevel;
    if (required == MTS_REQUIRED) {
        logLevel = CMN_LOG_LEVEL_INIT_ERROR;
    } else {
        logLevel = CMN_LOG_LEVEL_INIT_VERBOSE;
    }
    if (this->EndUserInterface) {
        if (this->MailBox) {
            return this->CommandsRead.GetItem(commandName, logLevel);
        } else {
            CMN_ASSERT(this->OriginalInterface);
            return this->OriginalInterface->CommandsRead.GetItem(commandName, logLevel);
        }
    }
    CMN_LOG_CLASS_INIT_ERROR << "GetCommandRead: cannot retrieve command " << commandName << " from \"factory\" interface \""
                             << this->GetFullName()
                             << "\", you must call GetEndUserInterface to make sure you are using an end-user interface"
                             << std::endl;
    return 0;
}


mtsCommandQualifiedRead * mtsInterfaceProvided::GetCommandQualifiedRead(const std::string & commandName,
                                                                        const mtsRequiredType required) const
{
    cmnLogLevel logLevel;
    if (required == MTS_REQUIRED) {
        logLevel = CMN_LOG_LEVEL_INIT_ERROR;
    } else {
        logLevel = CMN_LOG_LEVEL_INIT_VERBOSE;
    }
    if (this->EndUserInterface) {
        if (this->MailBox) {
            return this->CommandsQualifiedRead.GetItem(commandName, logLevel);
        } else {
            CMN_ASSERT(this->OriginalInterface);
            return this->OriginalInterface->CommandsQualifiedRead.GetItem(commandName, logLevel);
        }
    }
    CMN_LOG_CLASS_INIT_ERROR << "GetCommandQualifiedRead: cannot retrieve command " << commandName << " from \"factory\" interface \""
                             << this->GetFullName()
                             << "\", you must call GetEndUserInterface to make sure you are using an end-user interface"
                             << std::endl;
    return 0;
}


const cmnClassServicesBase * mtsInterfaceProvided::GetCommandWriteArgumentServices(const std::string & commandName) const
{
    const mtsCommandWriteBase * command = this->CommandsWrite.GetItem(commandName, CMN_LOG_LEVEL_INIT_VERBOSE);
    if (command) {
        return command->GetArgumentClassServices();
    }
    return 0;
}

const cmnClassServicesBase * mtsInterfaceProvided::GetCommandReadArgumentServices(const std::string & commandName) const
{
    const mtsCommandRead * command = this->CommandsRead.GetItem(commandName, CMN_LOG_LEVEL_INIT_VERBOSE);
    if (command) {
        return command->GetArgumentPrototype()->Services();
    }
    return 0;
}

mtsMulticastCommandVoid * mtsInterfaceProvided::GetEventVoid(const std::string & eventName) const
{
    // this event might be owned by the end user provided interface, e.g. event for end of blocking command
    if (this->OriginalInterface) {
        // try to find the event locally
        mtsMulticastCommandVoid * eventGenerator = EventVoidGenerators.GetItem(eventName, CMN_LOG_LEVEL_INIT_DEBUG);
        if (eventGenerator) {
            CMN_LOG_CLASS_INIT_DEBUG << "GetEventVoid: found event \"" << eventName << "\" in provided interface \""
                                     << this->GetFullName() << "\"" << std::endl;
            return eventGenerator;
        } else {
            CMN_LOG_CLASS_INIT_DEBUG << "GetEventVoid: looking for event \"" << eventName << "\" in provided interface \""
                                     << this->OriginalInterface->GetFullName() << "\"" << std::endl;
            // the "master" provided interface might have the event generator
            return this->OriginalInterface->GetEventVoid(eventName);
        }
    }
    // try from the "master" provided interface
    return EventVoidGenerators.GetItem(eventName, CMN_LOG_LEVEL_INIT_ERROR);
}


mtsMulticastCommandWriteBase * mtsInterfaceProvided::GetEventWrite(const std::string & eventName) const
{
    if (this->OriginalInterface) {
        return this->OriginalInterface->GetEventWrite(eventName);
    }
    return EventWriteGenerators.GetItem(eventName, CMN_LOG_LEVEL_INIT_ERROR);
}


bool mtsInterfaceProvided::AddObserver(const std::string & eventName,
                                       mtsCommandVoid * handler,
                                       const mtsRequiredType required)
{
    mtsMulticastCommandVoid * multicastCommand = GetEventVoid(eventName); // EventVoidGenerators.GetItem(eventName);
    if (multicastCommand) {
        return multicastCommand->AddCommand(handler);
    }
    // maybe the event is not defined at the end-user level but in
    // the original interface?
    if (this->OriginalInterface) {
        return this->OriginalInterface->AddObserver(eventName, handler, required);
    }
    if (required == MTS_REQUIRED) {
        CMN_LOG_CLASS_INIT_ERROR << "AddObserver (void) for \"" << GetFullName()
                                 << "\": cannot find event named \"" << eventName
                                 << "\", the following events are available: "
                                 << cmnStringFromVectorOfStrings(GetNamesOfEventsVoid())
                                 << std::endl;
    }
    return false;
}


bool mtsInterfaceProvided::AddObserver(const std::string & eventName,
                                       mtsCommandWriteBase * handler,
                                       const mtsRequiredType required)
{
    if (this->OriginalInterface) {
        return this->OriginalInterface->AddObserver(eventName, handler, required);
    }
    mtsMulticastCommandWriteBase * multicastCommand = EventWriteGenerators.GetItem(eventName);
    if (multicastCommand) {
        return multicastCommand->AddCommand(handler);
    }
    if (required == MTS_REQUIRED) {
        CMN_LOG_CLASS_INIT_ERROR << "AddObserver (write) for \"" << GetFullName()
                                 << "\": cannot find event named \"" << eventName
                                 << "\", the following events are available: "
                                 << cmnStringFromVectorOfStrings(GetNamesOfEventsWrite())
                                 << std::endl;
    }
    return false;
}


void mtsInterfaceProvided::AddObserverList(const mtsEventHandlerList & argin, mtsEventHandlerList & argout)
{
    argout = argin;
    size_t i;
    for (i = 0; i < argin.VoidEvents.size(); i++) {
        argout.VoidEvents[i].Result = argin.Provided->AddObserver(argin.VoidEvents[i].EventName,
                                                                  argin.VoidEvents[i].HandlerPointer,
                                                                  argin.VoidEvents[i].Required);
    }
    for (i = 0; i < argin.WriteEvents.size(); i++) {
        argout.WriteEvents[i].Result = argin.Provided->AddObserver(argin.WriteEvents[i].EventName,
                                                                   argin.WriteEvents[i].HandlerPointer,
                                                                   argin.WriteEvents[i].Required);
    }
}

bool mtsInterfaceProvided::RemoveObserver(const std::string & eventName, mtsCommandVoid * handler)
{
    if (this->OriginalInterface) {
        return this->OriginalInterface->RemoveObserver(eventName, handler);
    }
    mtsMulticastCommandVoid * multicastCommand = GetEventVoid(eventName); // EventVoidGenerators.GetItem(eventName);
    if (multicastCommand) {
        if (!multicastCommand->RemoveCommand(handler)) {
            CMN_LOG_CLASS_INIT_ERROR << "RemoveObserver (void): did not find handler for event " << eventName << std::endl;
            return false;
        }
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveObserver (void): cannot find event named \"" << eventName << "\"" << std::endl;
        return false;
    }
    return true;
}

bool mtsInterfaceProvided::RemoveObserver(const std::string & eventName, mtsCommandWriteBase * handler)
{
    if (this->OriginalInterface) {
        return this->OriginalInterface->RemoveObserver(eventName, handler);
    }
    mtsMulticastCommandWriteBase * multicastCommand = EventWriteGenerators.GetItem(eventName);
    if (multicastCommand) {
        if (!multicastCommand->RemoveCommand(handler)) {
            CMN_LOG_CLASS_INIT_ERROR << "RemoveObserver (write): did not find handler for event " << eventName << std::endl;
            return false;
        }
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "RemoveObserver (write): cannot find event named \"" << eventName << "\"" << std::endl;
        return false;
    }
    return true;
}

void mtsInterfaceProvided::RemoveObserverList(const mtsEventHandlerList & argin, mtsEventHandlerList & argout)
{
    argout = argin;
    size_t i;
    for (i = 0; i < argin.VoidEvents.size(); i++) {
        argout.VoidEvents[i].Result = argin.Provided->RemoveObserver(argin.VoidEvents[i].EventName, argin.VoidEvents[i].HandlerPointer);
    }
    for (i = 0; i < argin.WriteEvents.size(); i++) {
        argout.WriteEvents[i].Result = argin.Provided->RemoveObserver(argin.WriteEvents[i].EventName, argin.WriteEvents[i].HandlerPointer);
    }
}


void mtsInterfaceProvided::AddMessageEvents(void)
{
    // first attempt to add all three events
    if ((!AddEventWrite(mMessages.StatusEvent, "status", mMessages.StatusMessage))
        || (!AddEventWrite(mMessages.WarningEvent, "warning", mMessages.WarningMessage))
        || (!AddEventWrite(mMessages.ErrorEvent, "error", mMessages.ErrorMessage))
        ) {
        CMN_LOG_CLASS_INIT_ERROR << "AddMessageEvents for " << GetFullName()
                                 << " failed, make sure there is no other write event named \"status\", \"warning\" or \"error\"" << std::endl;
        return;
    }
}


void mtsInterfaceProvided::SendStatus(const std::string & message)
{
    mMessages.StatusMessage.SetMessage(message);
    CMN_LOG_CLASS_INSTANCE(this->Component, CMN_LOG_LEVEL_RUN_VERBOSE) << GetFullName()
                                                                       << ": " << mMessages.StatusMessage.Timestamp
                                                                       << "s, #" << mMessages.StatusMessage.Counter
                                                                       << ": " << mMessages.StatusMessage.Message
                                                                       << std::endl;
    mMessages.StatusEvent(mMessages.StatusMessage);
}


void mtsInterfaceProvided::SendWarning(const std::string & message)
{
    mMessages.WarningMessage.SetMessage(message);
    CMN_LOG_CLASS_INSTANCE(this->Component, CMN_LOG_LEVEL_RUN_WARNING) << GetFullName()
                                                                       << ": " << mMessages.WarningMessage.Timestamp
                                                                       << "s, #" << mMessages.WarningMessage.Counter
                                                                       << ": " << mMessages.WarningMessage.Message
                                                                       << std::endl;
    mMessages.WarningEvent(mMessages.WarningMessage);
}


void mtsInterfaceProvided::SendError(const std::string & message)
{
    mMessages.ErrorMessage.SetMessage(message);
    CMN_LOG_CLASS_INSTANCE(this->Component, CMN_LOG_LEVEL_RUN_ERROR) << GetFullName()
                                                                     << ": " << mMessages.ErrorMessage.Timestamp
                                                                     << "s, #" << mMessages.ErrorMessage.Counter
                                                                     << ": " << mMessages.ErrorMessage.Message
                                                                     << std::endl;
    mMessages.ErrorEvent(mMessages.ErrorMessage);
}


bool mtsInterfaceProvided::GetDescription(mtsInterfaceProvidedDescription & providedInterfaceDescription)
{
    bool success = true;

    // Serializer initialization
    std::stringstream streamBuffer;
    cmnSerializer serializer(streamBuffer);

    // Get information about commands and events.  Note that we fetch void
    // command objects from non-queued command container assuming the non-
    // queued container is updated for both queued and non-queued commands.

    // Extract void commands
    mtsCommandVoid * voidCommand;
    mtsCommandVoidDescription elementCommandVoid;
    const std::vector<std::string> namesOfVoidCommand = GetNamesOfCommandsVoid();
    for (size_t i = 0; i < namesOfVoidCommand.size(); ++i) {
        voidCommand = CommandsVoid.GetItem(namesOfVoidCommand[i]);
        if (!voidCommand) {
            CMN_LOG_CLASS_RUN_ERROR << "GetDescription: null void command: " << namesOfVoidCommand[i] << std::endl;
            success = false;
            continue;
        }

        elementCommandVoid.Name = voidCommand->GetName();
        providedInterfaceDescription.CommandsVoid.push_back(elementCommandVoid);
    }

    // Extract write commands
    mtsCommandWriteBase * writeCommand;
    mtsCommandWriteDescription elementCommandWrite;
    const std::vector<std::string> namesOfWriteCommand = GetNamesOfCommandsWrite();
    for (size_t i = 0; i < namesOfWriteCommand.size(); ++i) {
        writeCommand = CommandsWrite.GetItem(namesOfWriteCommand[i]);
        if (!writeCommand) {
            CMN_LOG_CLASS_RUN_ERROR << "GetDescription: null write command: " << namesOfWriteCommand[i] << std::endl;
            success = false;
            continue;
        }

        elementCommandWrite.Name = writeCommand->GetName();
        // serialize argument
        streamBuffer.str("");
        serializer.Reset();
        serializer.Serialize(*(writeCommand->GetArgumentPrototype()));
        elementCommandWrite.ArgumentPrototypeSerialized = streamBuffer.str();
        providedInterfaceDescription.CommandsWrite.push_back(elementCommandWrite);
    }

    // Extract read commands
    mtsCommandRead * readCommand;
    mtsCommandReadDescription elementCommandRead;
    const std::vector<std::string> namesOfReadCommand = GetNamesOfCommandsRead();
    for (size_t i = 0; i < namesOfReadCommand.size(); ++i) {
        readCommand = CommandsRead.GetItem(namesOfReadCommand[i]);
        if (!readCommand) {
            CMN_LOG_CLASS_RUN_ERROR << "GetDescription: null read command: " << namesOfReadCommand[i] << std::endl;
            success = false;
            continue;
        }

        elementCommandRead.Name = readCommand->GetName();
        // serialize argument
        streamBuffer.str("");
        serializer.Reset();
        serializer.Serialize(*(readCommand->GetArgumentPrototype()));
        elementCommandRead.ArgumentPrototypeSerialized = streamBuffer.str();
        providedInterfaceDescription.CommandsRead.push_back(elementCommandRead);
    }

    // Extract qualified read commands
    mtsCommandQualifiedRead * qualifiedReadCommand;
    mtsCommandQualifiedReadDescription elementCommandQualifiedRead;
    const std::vector<std::string> namesOfQualifiedReadCommand = GetNamesOfCommandsQualifiedRead();
    for (size_t i = 0; i < namesOfQualifiedReadCommand.size(); ++i) {
        qualifiedReadCommand = CommandsQualifiedRead.GetItem(namesOfQualifiedReadCommand[i]);
        if (!qualifiedReadCommand) {
            CMN_LOG_CLASS_RUN_ERROR << "GetDescription: null qualified read command: " << namesOfQualifiedReadCommand[i] << std::endl;
            success = false;
            continue;
        }

        elementCommandQualifiedRead.Name = qualifiedReadCommand->GetName();
        // serialize argument1
        streamBuffer.str("");
        serializer.Reset();
        serializer.Serialize(*(qualifiedReadCommand->GetArgument1Prototype()));
        elementCommandQualifiedRead.Argument1PrototypeSerialized = streamBuffer.str();
        // serialize argument2
        streamBuffer.str("");
        serializer.Reset();
        serializer.Serialize(*(qualifiedReadCommand->GetArgument2Prototype()));
        elementCommandQualifiedRead.Argument2PrototypeSerialized = streamBuffer.str();
        providedInterfaceDescription.CommandsQualifiedRead.push_back(elementCommandQualifiedRead);
    }

    // Extract void return commands
    mtsCommandVoidReturn * voidReturnCommand;
    mtsCommandVoidReturnDescription elementCommandVoidReturn;
    const std::vector<std::string> namesOfVoidReturnCommand = GetNamesOfCommandsVoidReturn();
    for (size_t i = 0; i < namesOfVoidReturnCommand.size(); ++i) {
        voidReturnCommand = CommandsVoidReturn.GetItem(namesOfVoidReturnCommand[i]);
        if (!voidReturnCommand) {
            CMN_LOG_CLASS_RUN_ERROR << "GetDescription: null void return command: " << namesOfVoidReturnCommand[i] << std::endl;
            success = false;
            continue;
        }

        elementCommandVoidReturn.Name = voidReturnCommand->GetName();
        // serialize result
        streamBuffer.str("");
        serializer.Reset();
        serializer.Serialize(*(voidReturnCommand->GetResultPrototype()));
        elementCommandVoidReturn.ResultPrototypeSerialized = streamBuffer.str();
        providedInterfaceDescription.CommandsVoidReturn.push_back(elementCommandVoidReturn);
    }

    // Extract write return commands
    mtsCommandWriteReturn * writeReturnCommand;
    mtsCommandWriteReturnDescription elementCommandWriteReturn;
    const std::vector<std::string> namesOfWriteReturnCommand = GetNamesOfCommandsWriteReturn();
    for (size_t i = 0; i < namesOfWriteReturnCommand.size(); ++i) {
        writeReturnCommand = CommandsWriteReturn.GetItem(namesOfWriteReturnCommand[i]);
        if (!writeReturnCommand) {
            CMN_LOG_CLASS_RUN_ERROR << "GetDescription: null write return command: " << namesOfWriteReturnCommand[i] << std::endl;
            success = false;
            continue;
        }

        elementCommandWriteReturn.Name = writeReturnCommand->GetName();
        // serialize argument
        streamBuffer.str("");
        serializer.Reset();
        serializer.Serialize(*(writeReturnCommand->GetArgumentPrototype()));
        elementCommandWriteReturn.ArgumentPrototypeSerialized = streamBuffer.str();
        // serialize result
        streamBuffer.str("");
        serializer.Reset();
        serializer.Serialize(*(writeReturnCommand->GetResultPrototype()));
        elementCommandWriteReturn.ResultPrototypeSerialized = streamBuffer.str();
        providedInterfaceDescription.CommandsWriteReturn.push_back(elementCommandWriteReturn);
    }

    // Extract void events
    mtsMulticastCommandVoid * voidEvent;
    mtsEventVoidDescription elementEventVoid;
    const std::vector<std::string> namesOfVoidEvent = GetNamesOfEventsVoid();
    for (size_t i = 0; i < namesOfVoidEvent.size(); ++i) {
        voidEvent = EventVoidGenerators.GetItem(namesOfVoidEvent[i]);
        if (!voidEvent) {
            CMN_LOG_CLASS_RUN_ERROR << "GetDescription: null void event: " << namesOfVoidEvent[i] << std::endl;
            success = false;
            continue;
        }
        elementEventVoid.Name = voidEvent->GetName();
        providedInterfaceDescription.EventsVoid.push_back(elementEventVoid);
    }

    // Extract write events
    mtsMulticastCommandWriteBase * writeEvent;
    mtsEventWriteDescription elementEventWrite;
    const std::vector<std::string> namesOfWriteEvent = GetNamesOfEventsWrite();
    for (size_t i = 0; i < namesOfWriteEvent.size(); ++i) {
        writeEvent = EventWriteGenerators.GetItem(namesOfWriteEvent[i]);
        if (!writeEvent) {
            CMN_LOG_CLASS_RUN_ERROR << "GetDescription: null write event: " << namesOfWriteEvent[i] << std::endl;
            success = false;
            continue;
        }
        elementEventWrite.Name = writeEvent->GetName();
        // serialize argument
        streamBuffer.str("");
        serializer.Reset();
        serializer.Serialize(*(writeEvent->GetArgumentPrototype()));
        elementEventWrite.ArgumentPrototypeSerialized = streamBuffer.str();
        providedInterfaceDescription.EventsWrite.push_back(elementEventWrite);
    }

    return success;
}
