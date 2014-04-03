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

#include <cisstMultiTask/mtsCommandRead.h>
#include <cisstMultiTask/mtsCallableReadBase.h>


mtsCommandRead::mtsCommandRead(void):
    BaseType()
{}


mtsCommandRead::mtsCommandRead(const std::string & name):
    BaseType(name),
    Callable(0),
    ArgumentPrototype(0)
{
}


mtsCommandRead::mtsCommandRead(mtsCallableReadBase * callable,
                               const std::string & name,
                               const mtsGenericObject * argumentPrototype):
    BaseType(name),
    Callable(callable),
    ArgumentPrototype(argumentPrototype)
{
}


mtsCommandRead::~mtsCommandRead()
{
    delete this->ArgumentPrototype;
}


mtsExecutionResult mtsCommandRead::Execute(mtsGenericObject & argument)
{
    if (this->IsEnabled()) {
        return this->Callable->Execute(argument);
    }
    return mtsExecutionResult::COMMAND_DISABLED;
}


mtsCallableReadBase * mtsCommandRead::GetCallable(void) const
{
    return this->Callable;
}


const mtsGenericObject * mtsCommandRead::GetArgumentPrototype(void) const
{
    return this->ArgumentPrototype;
}


const mtsGenericObject * mtsCommandRead::GetResultPrototype(void) const
{
    return this->ArgumentPrototype;
}


size_t mtsCommandRead::NumberOfArguments(void) const
{
    return 1;
}


bool mtsCommandRead::Returns(void) const
{
    return false;
}


void mtsCommandRead::ToStream(std::ostream & outputStream) const
{
    outputStream << "mtsCommandRead: ";
    if (this->Callable) {
        outputStream << this->Name << "(" << this->GetArgumentPrototype()->Services()->GetName() << "&) const using \""
                     << *Callable << "\" currently "
                     << (this->IsEnabled() ? "enabled" : "disabled");
    } else {
        outputStream << "not associated to a callable object";
    }
}
