/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Peter Kazanzides
  Created on: 2008-11-13

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsRequiredInterface.h>

CMN_IMPLEMENT_SERVICES(mtsRequiredInterface)


mtsRequiredInterface::mtsRequiredInterface(const std::string & interfaceName, mtsMailBox * mbox) :
        Name(interfaceName),
        MailBox(mbox),
        OtherInterface(0),
        CommandPointersVoid("CommandPointerVoid"),
        CommandPointersRead("CommandPointerRead"),
        CommandPointersWrite("CommandPointerWrite"),
        CommandPointersQualifiedRead("CommandPointerQualifiedRead"),
        EventHandlersVoid("EventHandlerVoid"),
        EventHandlersWrite("EventHandlerWrite")
{
}

mtsRequiredInterface::~mtsRequiredInterface()
{
    CommandPointersVoid.DeleteAll();
    CommandPointersRead.DeleteAll();
    CommandPointersWrite.DeleteAll();
    CommandPointersQualifiedRead.DeleteAll();
}

std::vector<std::string> mtsRequiredInterface::GetNamesOfCommandPointers(void) const {
    std::vector<std::string> commands = GetNamesOfCommandPointersVoid();
    std::vector<std::string> tmp = GetNamesOfCommandPointersRead();
    commands.insert(commands.begin(), tmp.begin(), tmp.end());
    tmp.clear();
    tmp = GetNamesOfCommandPointersWrite();
    commands.insert(commands.begin(), tmp.begin(), tmp.end());
    tmp.clear();
    tmp = GetNamesOfCommandPointersQualifiedRead();
    commands.insert(commands.begin(), tmp.begin(), tmp.end());
    return commands;
}


std::vector<std::string> mtsRequiredInterface::GetNamesOfCommandPointersVoid(void) const {
    return CommandPointersVoid.GetNames();
}


std::vector<std::string> mtsRequiredInterface::GetNamesOfCommandPointersRead(void) const {
    return CommandPointersRead.GetNames();
}


std::vector<std::string> mtsRequiredInterface::GetNamesOfCommandPointersWrite(void) const {
    return CommandPointersWrite.GetNames();
}


std::vector<std::string> mtsRequiredInterface::GetNamesOfCommandPointersQualifiedRead(void) const {
    return CommandPointersQualifiedRead.GetNames();
}


std::vector<std::string> mtsRequiredInterface::GetNamesOfEventHandlersVoid(void) const {
    return EventHandlersVoid.GetNames();
}


std::vector<std::string> mtsRequiredInterface::GetNamesOfEventHandlersWrite(void) const {
    return EventHandlersWrite.GetNames();
}

mtsCommandVoidBase * mtsRequiredInterface::GetEventHandlerVoid(const std::string & eventName) const {
    return EventHandlersVoid.GetItem(eventName);
}

mtsCommandWriteBase * mtsRequiredInterface::GetEventHandlerWrite(const std::string & eventName) const {
    return EventHandlersWrite.GetItem(eventName);
}

void mtsRequiredInterface::Disconnect(void)
{
    // First, do the command pointers.  In the future, it may be better to set the pointers to NOPVoid, NOPRead, etc.,
    // which can be static members of the corresponding command classes.
    CommandPointerVoidMapType::MapType::iterator iterVoid;
    for (iterVoid = CommandPointersVoid.GetMap().begin(); iterVoid != CommandPointersVoid.GetMap().end(); iterVoid++)
        iterVoid->second->Clear();
    CommandPointerReadMapType::MapType::iterator iterRead;
    for (iterRead = CommandPointersRead.GetMap().begin(); iterRead != CommandPointersRead.GetMap().end(); iterRead++)
        iterRead->second->Clear();
    CommandPointerWriteMapType::MapType::iterator iterWrite;
    for (iterWrite = CommandPointersWrite.GetMap().begin(); iterWrite != CommandPointersWrite.GetMap().end(); iterWrite++)
        iterWrite->second->Clear();
    CommandPointerQualifiedReadMapType::MapType::iterator iterQualRead;
    for (iterQualRead = CommandPointersQualifiedRead.GetMap().begin();
         iterQualRead != CommandPointersQualifiedRead.GetMap().end(); iterQualRead++)
        iterQualRead->second->Clear();
#if 0
    // Now, do the event handlers.  Still need to implement RemoveObserver
    EventHandlerVoidMapType::MapType::iterator iterEventVoid;
    for (iterEventVoid = EventHandlersVoid.GetMap().begin(); iterEventVoid != EventHandlersVoid.GetMap().end(); iterEventVoid++)
        OtherInterface->RemoveObserver(iterEventVoid->first, iterEventVoid->second);
    EventHandlerWriteMapType::MapType::iterator iterEventWrite;
    for (iterEventWrite = EventHandlersWrite.GetMap().begin(); iterEventWrite != EventHandlersWrite.GetMap().end(); iterEventWrite++)
        OtherInterface->RemoveObserver(iterEventWrite->first, iterEventWrite->second);
#endif
}

bool mtsRequiredInterface::BindCommandsAndEvents(void)
{
    bool success = true;
    bool result;
    // First, do the command pointers
    CommandPointerVoidMapType::MapType::iterator iterVoid;
    for (iterVoid = CommandPointersVoid.GetMap().begin();
         iterVoid != CommandPointersVoid.GetMap().end();
         iterVoid++) {
        result = iterVoid->second->Bind(OtherInterface->GetCommandVoid(iterVoid->first));
        if (!result) {
            CMN_LOG_CLASS(2) << "BindCommandsAndEvents: failed for void command \""
                             << iterVoid->first << "\"" << std::endl;
        } else {
            CMN_LOG_CLASS(4) << "BindCommandsAndEvents: succeeded for void command \""
                             << iterVoid->first << "\"" << std::endl;
        }
        success &= result;
    }
    CommandPointerReadMapType::MapType::iterator iterRead;
    for (iterRead = CommandPointersRead.GetMap().begin();
         iterRead != CommandPointersRead.GetMap().end();
         iterRead++) {
        result = iterRead->second->Bind(OtherInterface->GetCommandRead(iterRead->first));
        if (!result) {
            CMN_LOG_CLASS(2) << "BindCommandsAndEvents: failed for read command \""
                             << iterRead->first << "\"" << std::endl;
        } else {
            CMN_LOG_CLASS(4) << "BindCommandsAndEvents: succeeded for read command \""
                             << iterRead->first << "\"" << std::endl;
        }
        success &= result;
    }
    CommandPointerWriteMapType::MapType::iterator iterWrite;
    for (iterWrite = CommandPointersWrite.GetMap().begin();
         iterWrite != CommandPointersWrite.GetMap().end();
         iterWrite++) {
        result = iterWrite->second->Bind(OtherInterface->GetCommandWrite(iterWrite->first));
        if (!result) {
            CMN_LOG_CLASS(2) << "BindCommandsAndEvents: failed for write command \""
                             << iterWrite->first << "\"" << std::endl;
        } else {
            CMN_LOG_CLASS(4) << "BindCommandsAndEvents: succeeded for write command \""
                             << iterWrite->first << "\"" << std::endl;
        }
        success &= result;
    }
    CommandPointerQualifiedReadMapType::MapType::iterator iterQualRead;
    for (iterQualRead = CommandPointersQualifiedRead.GetMap().begin();
         iterQualRead != CommandPointersQualifiedRead.GetMap().end();
         iterQualRead++) {
        result = iterQualRead->second->Bind(OtherInterface->GetCommandQualifiedRead(iterQualRead->first));
        if (!result) {
            CMN_LOG_CLASS(2) << "BindCommandsAndEvents: failed for qualified read command \""
                             << iterQualRead->first << "\"" << std::endl;
        } else {
            CMN_LOG_CLASS(4) << "BindCommandsAndEvents: succeeded for qualified read command \""
                             << iterQualRead->first << "\"" << std::endl;
        }
        success &= result;
    }

    if (!success) {
        CMN_LOG_CLASS(1) << "BindCommandsAndEvents: required commands missing (ERROR)" << std::endl;
    }

    // Now, do the event handlers
    EventHandlerVoidMapType::MapType::iterator iterEventVoid;
    for (iterEventVoid = EventHandlersVoid.GetMap().begin();
         iterEventVoid != EventHandlersVoid.GetMap().end();
         iterEventVoid++) {
        result = OtherInterface->AddObserver(iterEventVoid->first, iterEventVoid->second);
        if (!result) {
            CMN_LOG_CLASS(2) << "BindCommandsAndEvents: failed to add observer for void event \""
                             << iterEventVoid->first << "\"" << std::endl;
        } else {
            CMN_LOG_CLASS(4) << "BindCommandsAndEvents: succeeded to add observer for void event \""
                             << iterEventVoid->first << "\"" << std::endl;
        }
    }

    EventHandlerWriteMapType::MapType::iterator iterEventWrite;
    for (iterEventWrite = EventHandlersWrite.GetMap().begin();
         iterEventWrite != EventHandlersWrite.GetMap().end();
         iterEventWrite++) {
        result = OtherInterface->AddObserver(iterEventWrite->first, iterEventWrite->second);
        if (!result) {
            CMN_LOG_CLASS(2) << "BindCommandsAndEvents: failed to add observer for write event \""
                             << iterEventWrite->first << "\"" << std::endl;
        } else {
            CMN_LOG_CLASS(4) << "BindCommandsAndEvents: succeeded to add observer for write event \""
                             << iterEventWrite->first << "\"" << std::endl;
        }
    }
    return success;
}

unsigned int mtsRequiredInterface::ProcessMailBoxes(void)
{
    unsigned int numberOfCommands = 0;
    while (MailBox->ExecuteNext()) {
        numberOfCommands++;
    }
    return numberOfCommands;
}

void mtsRequiredInterface::ToStream(std::ostream & outputStream) const
{
    outputStream << "Required Interface name: " << Name << std::endl;
    CommandPointersVoid.ToStream(outputStream);
    CommandPointersRead.ToStream(outputStream);
    CommandPointersWrite.ToStream(outputStream);
    CommandPointersQualifiedRead.ToStream(outputStream);
    EventHandlersVoid.ToStream(outputStream);
    EventHandlersWrite.ToStream(outputStream);
}

