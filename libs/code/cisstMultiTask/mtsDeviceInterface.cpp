/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnGenericObjectProxy.h>
#include <cisstMultiTask/mtsDeviceInterface.h>
#include <cisstMultiTask/mtsFunctionVoid.h>


mtsDeviceInterface::mtsDeviceInterface(const std::string & interfaceName,
                                       mtsDevice * device):
    Name(interfaceName),
    Device(device),
    Registered(false),
    UserCounter(0),
    CommandsVoid("CommandsVoid"),
    CommandsRead("CommandsRead"),
    CommandsWrite("CommandsWrite"),
    CommandsQualifiedRead("CommandsQualifiedRead"),
    EventVoidGenerators("EventVoidGenerators"),
    EventWriteGenerators("EventWriteGenerators")
{
    CommandsVoid.SetOwner(*this);
    CommandsRead.SetOwner(*this);
    CommandsWrite.SetOwner(*this);
    CommandsQualifiedRead.SetOwner(*this);
    EventVoidGenerators.SetOwner(*this);
    EventWriteGenerators.SetOwner(*this);
}

mtsCommandVoidBase * mtsDeviceInterface::GetCommandVoid(const std::string & commandName,
                                                        unsigned int CMN_UNUSED(userId)) const {
    return CommandsVoid.GetItem(commandName, CMN_LOG_LOD_INIT_ERROR);
}

mtsCommandReadBase * mtsDeviceInterface::GetCommandRead(const std::string & commandName) const {
    return CommandsRead.GetItem(commandName, CMN_LOG_LOD_INIT_ERROR);
}

mtsCommandWriteBase * mtsDeviceInterface::GetCommandWrite(const std::string & commandName,
                                                          unsigned int CMN_UNUSED(userId)) const {
    return CommandsWrite.GetItem(commandName, CMN_LOG_LOD_INIT_ERROR);
}

mtsCommandQualifiedReadBase * mtsDeviceInterface::GetCommandQualifiedRead(const std::string & commandName) const {
    return CommandsQualifiedRead.GetItem(commandName, CMN_LOG_LOD_INIT_ERROR);
}

std::vector<std::string> mtsDeviceInterface::GetNamesOfCommands(void) const {
    std::vector<std::string> commands = GetNamesOfCommandsVoid();
    std::vector<std::string> tmp = GetNamesOfCommandsRead();
    commands.insert(commands.begin(), tmp.begin(), tmp.end());
    tmp.clear();
    tmp = GetNamesOfCommandsWrite();
    commands.insert(commands.begin(), tmp.begin(), tmp.end());
    tmp.clear();
    tmp = GetNamesOfCommandsQualifiedRead();
    commands.insert(commands.begin(), tmp.begin(), tmp.end());
    return commands;
}


std::vector<std::string> mtsDeviceInterface::GetNamesOfCommandsVoid(void) const {
    return CommandsVoid.GetNames();
}


std::vector<std::string> mtsDeviceInterface::GetNamesOfCommandsRead(void) const {
    return CommandsRead.GetNames();
}


std::vector<std::string> mtsDeviceInterface::GetNamesOfCommandsWrite(void) const {
    return CommandsWrite.GetNames();
}


std::vector<std::string> mtsDeviceInterface::GetNamesOfCommandsQualifiedRead(void) const {
    return CommandsQualifiedRead.GetNames();
}


mtsDeviceInterface::CommandVoidMapType & mtsDeviceInterface::GetCommandVoidMap(void) {
    return CommandsVoid;
}


mtsDeviceInterface::CommandReadMapType & mtsDeviceInterface::GetCommandReadMap(void) {
    return CommandsRead;
}


mtsDeviceInterface::CommandWriteMapType & mtsDeviceInterface::GetCommandWriteMap(void) {
    return CommandsWrite;
}


mtsDeviceInterface::CommandQualifiedReadMapType & mtsDeviceInterface::GetCommandQualifiedReadMap(void) {
    return CommandsQualifiedRead;
}


mtsCommandVoidBase * mtsDeviceInterface::AddEventVoid(const std::string & eventName) {
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


bool mtsDeviceInterface::AddEventVoid(mtsFunctionVoid & eventTrigger,
                                      const std::string eventName) {
    mtsCommandVoidBase * command;
    command = this->AddEventVoid(eventName);
    if (command) {
        eventTrigger.Bind(command);
        return true;
    }
    return false;
}


bool mtsDeviceInterface::AddEvent(const std::string & name, mtsMulticastCommandVoid * generator)
{
    if (EventWriteGenerators.GetItem(name, CMN_LOG_LOD_NOT_USED)) {
        // Is this check really needed?
        CMN_LOG_CLASS_INIT_VERBOSE << "AddEvent (void): event " << name << " already exists as write event, ignored." << std::endl;
        return false;
    }
    return EventVoidGenerators.AddItem(name, generator, CMN_LOG_LOD_INIT_ERROR);
}


bool mtsDeviceInterface::AddEvent(const std::string & name, mtsMulticastCommandWriteBase * generator)
{
    if (EventVoidGenerators.GetItem(name, CMN_LOG_LOD_NOT_USED)) {
        // Is this check really needed?
        CMN_LOG_CLASS_INIT_VERBOSE << "AddEvent (write): event " << name << " already exists as void event, ignored." << std::endl;
        return false;
    }
    return EventWriteGenerators.AddItem(name, generator, CMN_LOG_LOD_INIT_ERROR);
}


std::vector<std::string> mtsDeviceInterface::GetNamesOfEventsVoid(void) const {
    return EventVoidGenerators.GetNames();
}


std::vector<std::string> mtsDeviceInterface::GetNamesOfEventsWrite(void) const {
    return EventWriteGenerators.GetNames();
}


bool mtsDeviceInterface::AddObserver(const std::string & eventName, mtsCommandVoidBase * handler)
{
    mtsMulticastCommandVoid * multicastCommand = EventVoidGenerators.GetItem(eventName);
    if (multicastCommand) {
        // should probably check for duplicates (have AddCommand return bool?)
        multicastCommand->AddCommand(handler);
        return true;
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "AddObserver (void): cannot find event named \"" << eventName << "\"" << std::endl;
        return false;
    }
}


bool mtsDeviceInterface::AddObserver(const std::string & eventName, mtsCommandWriteBase * handler)
{
    mtsMulticastCommandWriteBase * multicastCommand = EventWriteGenerators.GetItem(eventName);
    if (multicastCommand) {
        // should probably check for duplicates (have AddCommand return bool?)
        multicastCommand->AddCommand(handler);
        return true;
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "AddObserver (write): cannot find event named \"" << eventName << "\"" << std::endl;
        return false;
    }
}


bool mtsDeviceInterface::AddObserver(const std::string & eventName, mtsCommandWriteGenericBase * handler)
{
    mtsMulticastCommandWriteBase * multicastCommand = EventWriteGenerators.GetItem(eventName);
    if (multicastCommand) {
        // should probably check for duplicates (have AddCommand return bool?)
        multicastCommand->AddCommand(handler);
        return true;
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "AddObserver (write): cannot find event named \"" << eventName << "\"" << std::endl;
        return false;
    }
}


unsigned int mtsDeviceInterface::AllocateResources(const std::string & userName)
{
    // no queued commands in this interface, we just keep track of the
    // requests
    this->UserCounter++;
    CMN_LOG_CLASS_INIT_VERBOSE << "AllocateResource: interface \"" << this->Name
                               << "\"received request number "
                               << this->UserCounter << " from \""
                               << userName << "\"" << std::endl;
    return this->UserCounter;
}


void mtsDeviceInterface::ToStream(std::ostream & outputStream) const
{
    outputStream << "Provided Interface name: " << Name << std::endl;
    // Previous version of code numbered all commands in a single series.
    // Now, we restart numbering for each type of command.
    CommandsVoid.ToStream(outputStream);
    CommandsRead.ToStream(outputStream);
    CommandsWrite.ToStream(outputStream);
    CommandsQualifiedRead.ToStream(outputStream);
    // Previous version of code numbered all events in a single series.
    // Now, we restart numbering for each type (e.g., void or write).
    EventVoidGenerators.ToStream(outputStream);
    EventWriteGenerators.ToStream(outputStream);
}

