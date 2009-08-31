/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstMultiTask/mtsMulticastCommandWriteBase.h>

void mtsMulticastCommandWriteBase::AddCommand(BaseType * command) {
    if (command) {
        CMN_ASSERT(command->GetArgumentPrototype());
        CMN_ASSERT(this->GetArgumentPrototype());
        if (command->GetArgumentPrototype()->Services() != this->GetArgumentPrototype()->Services()) {
            CMN_LOG_INIT_ERROR << "Class mtsMulticastCommandWriteBase: AddCommand: command argument type don't match" << std::endl;
            exit(0);
        } else {
            // copy the multicast command prototype to each added command using in place new
            this->GetArgumentPrototype()->Services()->Create(const_cast<mtsGenericObject *>(command->GetArgumentPrototype()), *(this->GetArgumentPrototype()));
            // add the command to the list
            this->Commands.push_back(command);
        }
    }
}


void mtsMulticastCommandWriteBase::ToStream(std::ostream & outputStream) const {
    outputStream << "mtsMulticastCommandWrite: " << this->Name;
    if (Commands.size() != 0) {
        outputStream << "\n  Registered observers:" << std::endl;
        for (unsigned int i = 0; i < Commands.size(); i++) {
            outputStream << "  . CallBack [" << i << "]: " << *(Commands[i]);
        }
    } else {
        outputStream << " with no registered observers";
    }
}

