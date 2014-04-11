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


#include <cisstMultiTask/mtsCommandVoidReturn.h>
#include <cisstMultiTask/mtsCallableVoidReturnMethod.h>

mtsCommandVoidReturn::mtsCommandVoidReturn(void)
{}


mtsCommandVoidReturn::mtsCommandVoidReturn(const std::string & name):
    BaseType(name)
{}


mtsCommandVoidReturn::mtsCommandVoidReturn(mtsCallableVoidReturnBase * callable,
                                           const std::string & name,
                                           const mtsGenericObject * resultPrototype):
    BaseType(name),
    Callable(callable),
    ResultPrototype(resultPrototype)
{}


mtsCommandVoidReturn::~mtsCommandVoidReturn()
{
    delete this->ResultPrototype;
}


mtsExecutionResult mtsCommandVoidReturn::Execute(mtsGenericObject & result)
{
    if (this->IsEnabled()) {
        return this->Callable->Execute(result);
    }
    return mtsExecutionResult::COMMAND_DISABLED;
}


mtsCallableVoidReturnBase * mtsCommandVoidReturn::GetCallable(void) const
{
    return this->Callable;
}


size_t mtsCommandVoidReturn::NumberOfArguments(void) const
{
    return 0;
}


bool mtsCommandVoidReturn::Returns(void) const
{
    return true;
}


const mtsGenericObject * mtsCommandVoidReturn::GetResultPrototype(void) const
{
    return this->ResultPrototype;
}


void mtsCommandVoidReturn::ToStream(std::ostream & outputStream) const
{
    outputStream << "mtsCommandVoidReturn: ";
    if (this->Callable) {
        outputStream << this->Name << "(" << this->GetResultPrototype()->Services()->GetName() << " &) using "
                     << *Callable << "\" currently "
                     << (this->IsEnabled() ? "enabled" : "disabled");
    } else {
        outputStream << "not associated to a callable object";
    }
}


void mtsCommandVoidReturn::SetResultPrototype(const mtsGenericObject * resultPrototype)
{
    this->ResultPrototype = resultPrototype;
}
