/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Anton Deguet
  Created on: 2010-09-16

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstMultiTask/mtsCommandWriteReturn.h>
#include <cisstMultiTask/mtsCallableWriteReturnMethod.h>

mtsCommandWriteReturn::mtsCommandWriteReturn(void)
{}


mtsCommandWriteReturn::mtsCommandWriteReturn(const std::string & name):
    BaseType(name)
{}


mtsCommandWriteReturn::mtsCommandWriteReturn(mtsCallableWriteReturnBase * callable,
                                             const std::string & name,
                                             const mtsGenericObject * argumentPrototype,
                                             const mtsGenericObject * resultPrototype):
    BaseType(name),
    Callable(callable),
    ArgumentPrototype(argumentPrototype),
    ResultPrototype(resultPrototype)
{}


mtsCommandWriteReturn::~mtsCommandWriteReturn()
{
    if (this->ArgumentPrototype == this->ResultPrototype)
        delete this->ArgumentPrototype;
    else {
        delete this->ArgumentPrototype;
        delete this->ResultPrototype;
    }
}


mtsExecutionResult mtsCommandWriteReturn::Execute(const mtsGenericObject & argument,
                                                  mtsGenericObject & result)
{
    if (this->IsEnabled()) {
        return this->Callable->Execute(argument, result);
    }
    return mtsExecutionResult::COMMAND_DISABLED;
}


mtsCallableWriteReturnBase * mtsCommandWriteReturn::GetCallable(void) const
{
    return this->Callable;
}


size_t mtsCommandWriteReturn::NumberOfArguments(void) const
{
    return 1;
}


bool mtsCommandWriteReturn::Returns(void) const
{
    return true;
}


const mtsGenericObject * mtsCommandWriteReturn::GetArgumentPrototype(void) const
{
    return this->ArgumentPrototype;
}


const mtsGenericObject * mtsCommandWriteReturn::GetResultPrototype(void) const
{
    return this->ResultPrototype;
}


void mtsCommandWriteReturn::ToStream(std::ostream & outputStream) const
{
    outputStream << "mtsCommandWriteReturn: ";
    if (this->Callable) {
        outputStream << this->Name << "(const " << this->GetArgumentPrototype()->Services()->GetName() << " & , "
                     << this->GetResultPrototype()->Services()->GetName() << " &) using "
                     << *Callable << "\" currently "
                     << (this->IsEnabled() ? "enabled" : "disabled");
    } else {
        outputStream << "not associated to a callable object";
    }
}


void mtsCommandWriteReturn::SetResultPrototype(const mtsGenericObject * resultPrototype)
{
    this->ResultPrototype = resultPrototype;
}
