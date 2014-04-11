/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <algorithm>
#include <cisstMultiTask/mtsMulticastCommandVoid.h>


mtsMulticastCommandVoid::mtsMulticastCommandVoid(const std::string & name):
    BaseType(0, name)
{}


mtsMulticastCommandVoid::~mtsMulticastCommandVoid()
{}


bool mtsMulticastCommandVoid::AddCommand(BaseType * command) {
    if (command) {
        VectorType::iterator it = std::find(Commands.begin(), Commands.end(), command);
        if (it != Commands.end()) {
            CMN_LOG_INIT_WARNING << "Class mtsMulticastCommandVoid: AddCommand: command " 
                                 << command->GetName() << " already added" << std::endl;
            return false;
        }
        this->Commands.push_back(command);
        return true;
    }
    return false;
}

bool mtsMulticastCommandVoid::RemoveCommand(BaseType * command) {
    if (command) {
        VectorType::iterator it = std::find(Commands.begin(), Commands.end(), command);
        if (it != Commands.end()) {
            Commands.erase(it);
            return true;
        }
    }
    return false;
}

mtsExecutionResult mtsMulticastCommandVoid::Execute(mtsBlockingType CMN_UNUSED(blocking))
{
    size_t index;
    const size_t commandsSize = Commands.size();
    for (index = 0; index < commandsSize; index++) {
        Commands[index]->Execute(MTS_NOT_BLOCKING);
    }
    return mtsExecutionResult::COMMAND_SUCCEEDED;
}


void mtsMulticastCommandVoid::ToStream(std::ostream & outputStream) const {
    outputStream << "mtsMulticastCommandVoid: \"" << this->Name << "\"";
    if (Commands.size() != 0) {
        outputStream << "\n  Registered observers:" << std::endl;
        for (unsigned int i = 0; i < Commands.size(); i++) {
            outputStream << "  . CallBack [" << i << "]: " << *(Commands[i]);
        }
    } else {
        outputStream << " with no registered observers";
    }
}

