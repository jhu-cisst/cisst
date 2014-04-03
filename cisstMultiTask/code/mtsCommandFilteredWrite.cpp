/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Peter Kazanzides, Anton Deguet

  (C) Copyright 2005-2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstMultiTask/mtsCommandFilteredWrite.h>
#include <cisstMultiTask/mtsCommandQualifiedRead.h>

mtsCommandFilteredWrite::mtsCommandFilteredWrite(mtsCommandQualifiedRead * filter,
                                                 mtsCommandWriteBase * command):
    BaseType(),
    Command(command),
    Filter(filter)
{
    if (command) {
        this->Name = command->GetName();
    }
    // PK: is there a better way to do this?
    if (filter) {
        FilterOutput = dynamic_cast<mtsGenericObject *>(filter->GetArgument2Prototype()->Services()->Create());
    }
}


mtsCommandFilteredWrite::~mtsCommandFilteredWrite()
{
    if (FilterOutput) {
        delete FilterOutput;
    }
}


mtsExecutionResult mtsCommandFilteredWrite::Execute(const mtsGenericObject & argument, mtsBlockingType blocking)
{
    if (this->IsEnabled()) {
        // First, call the filter (qualified read)
        mtsExecutionResult result = this->Filter->Execute(argument, *FilterOutput);
        if (!result.IsOK()) {
            return result;
        }
        // Next, queue the write command
        return this->Command->Execute(*FilterOutput, blocking);
    }
    return mtsExecutionResult::COMMAND_DISABLED;
}


const mtsGenericObject * mtsCommandFilteredWrite::GetArgumentPrototype(void) const
{
    return this->Filter->GetArgument1Prototype();
}


void mtsCommandFilteredWrite::ToStream(std::ostream & outputStream) const
{
    outputStream << "mtsCommandFilteredWrite: ";
    if (this->Command) {
        outputStream << this->Name << "(const " << this->GetArgumentPrototype()->Services()->GetName() << " & , "
                     << this->GetArgumentPrototype()->Services()->GetName() << " &) using "
                     << *Command << "\" currently "
                     << (this->IsEnabled() ? "enabled" : "disabled");
    } else {
        outputStream << "not associated to a command";
    }
}
