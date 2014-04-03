/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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

#include <cisstMultiTask/mtsMulticastCommandQualifiedReadOrWrite.h>


template <class _argumentType>
void mtsMulticastCommandQualifiedReadOrWrite<_argumentType>::AddCommand(BaseType * command) {
    if (command) {
        this->Name += std::string("+") + command->Name;
        this->Commands.push_back(command);
    }
}


template <class _argumentType>
mtsExecutionResult mtsMulticastCommandQualifiedReadOrWrite<_argumentType>::Execute(const cmnGenericObject & qualifier, ArgumentType argument) {
    int result = static_cast<int>(mtsExecutionResult::DEV_OK);
    for (unsigned int i = 0; i < Commands.size(); i++) {
        result =
            (result << static_cast<int>(mtsExecutionResult::RETURN_TYPE_BIT_SIZE))
            | static_cast<int>(Commands[i]->Execute(qualifier, argument));
    }
    return static_cast<mtsExecutionResult::ReturnType>(result);
}


template <class _argumentType>
void mtsMulticastCommandQualifiedReadOrWrite<_argumentType>::ToStream(std::ostream & out) const {
    out << "Multicast Command Qualified Read or Write:";
    for (unsigned int i = 0; i < Commands.size(); i++) {
        out << " " << Commands[i];
    }
}


// force instantiation for both useful types
template class mtsMulticastCommandQualifiedReadOrWrite<cmnGenericObject &>;

