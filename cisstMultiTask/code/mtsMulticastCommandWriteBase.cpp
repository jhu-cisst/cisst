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
#include <cisstMultiTask/mtsMulticastCommandWriteBase.h>
#include <cisstMultiTask/mtsCommandWrite.h>

bool mtsMulticastCommandWriteBase::AddCommand(BaseType * command) {
    if (command) {
        VectorType::iterator it = std::find(Commands.begin(), Commands.end(), command);
        if (it != Commands.end()) {
            CMN_LOG_INIT_WARNING << "Class mtsMulticastCommandWriteBase: AddCommand: command " 
                                 << command->GetName() << " already added" << std::endl;
            return false;
        }
        // check if the command already has an argument prototype
        if (command->GetArgumentPrototype()) {
            CMN_ASSERT(this->GetArgumentPrototype());
            if (command->GetArgumentPrototype()->Services() != this->GetArgumentPrototype()->Services()) {
                CMN_LOG_INIT_ERROR << "Class mtsMulticastCommandWriteBase: AddCommand: command argument types don't match, this multicast command uses "
                                   << this->GetArgumentPrototype()->Services()->GetName()
                                   << " but the command added (event handler potentially) uses "
                                   << command->GetArgumentPrototype()->Services()->GetName()
                                   << std::endl;
                return false;
            } else {
                // copy the multicast command prototype to each added command using in place new
                this->GetArgumentPrototype()->Services()->Create(const_cast<mtsGenericObject *>(command->GetArgumentPrototype()), *(this->GetArgumentPrototype()));
                // Add the command to the list
                this->Commands.push_back(command);
                return true;
            }
        } else {
            // create a new object
            command->SetArgumentPrototype(reinterpret_cast<const mtsGenericObject *>(this->GetArgumentPrototype()->Services()->Create(*(this->GetArgumentPrototype()))));
            // Add the command to the list
            this->Commands.push_back(command);
            return true;
        }
    }
    return false;
}


bool mtsMulticastCommandWriteBase::RemoveCommand(BaseType * command) {
    if (command) {
        VectorType::iterator it = std::find(Commands.begin(), Commands.end(), command);
        if (it != Commands.end()) {
            Commands.erase(it);
            return true;
        }
        // TODO: 
        //   1) If AddCommand created a new object, we should delete it here to avoid a memory leak
        //      this->GetArgumentPrototype()->Services()->Delete(command->GetArgumentPrototype());
        //   2) If AddCommand set the argument prototype, we should clear it here.
        //      command->SetArgumentPrototype(0);
        //   (alternatively, could add method to delete and set to 0)
    }
    return false;
}

void mtsMulticastCommandWriteBase::ToStream(std::ostream & outputStream) const {
    outputStream << "mtsMulticastCommandWrite: \"" << this->Name << "\"";
    if (Commands.size() != 0) {
        outputStream << "\n  Registered observers:" << std::endl;
        size_t i;
        for (i = 0; i < Commands.size(); i++) {
            outputStream << "  . CallBack [" << i << "]: " << *(Commands[i]);
        }
    } else {
        outputStream << " with no registered observers";
    }
}

