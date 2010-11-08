/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsMulticastCommandVoid.h>


mtsMulticastCommandVoid::mtsMulticastCommandVoid(const std::string & name):
    BaseType(0, name)
{}


mtsMulticastCommandVoid::~mtsMulticastCommandVoid()
{}


void mtsMulticastCommandVoid::AddCommand(BaseType * command) {
    if (command) {
        this->Commands.push_back(command);
    }
}


mtsExecutionResult mtsMulticastCommandVoid::Execute(mtsBlockingType CMN_UNUSED(blocking)) {
    int result = static_cast<int>(mtsExecutionResult::DEV_OK);
    for (size_t i = 0; i < Commands.size(); i++) {
        result =
            (result << static_cast<int>(mtsExecutionResult::RETURN_TYPE_BIT_SIZE))
            | static_cast<int>((Commands[i]->Execute(MTS_NOT_BLOCKING)).GetResult());
    }
    return static_cast<mtsExecutionResult::Enum>(result);
}


void mtsMulticastCommandVoid::ToStream(std::ostream & outputStream) const {
    outputStream << "mtsMulticastCommandVoid: " << this->Name;
    if (Commands.size() != 0) {
        outputStream << "\n  Registered observers:" << std::endl;
        for (unsigned int i = 0; i < Commands.size(); i++) {
            outputStream << "  . CallBack [" << i << "]: " << *(Commands[i]);
        }
    } else {
        outputStream << " with no registered observers";
    }
}

