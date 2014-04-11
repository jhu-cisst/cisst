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

#include <cisstMultiTask/mtsCommandQualifiedRead.h>
#include <cisstMultiTask/mtsCallableQualifiedReadBase.h>


mtsCommandQualifiedRead::mtsCommandQualifiedRead(void):
    BaseType()
{}


mtsCommandQualifiedRead::mtsCommandQualifiedRead(const std::string & name):
    BaseType(name),
    Callable(0),
    Argument1Prototype(0),
    Argument2Prototype(0)
{}


mtsCommandQualifiedRead::mtsCommandQualifiedRead(mtsCallableQualifiedReadBase * callable,
                                                 const std::string & name,
                                                 const mtsGenericObject * argument1Prototype,
                                                 const mtsGenericObject * argument2Prototype):
    BaseType(name),
    Callable(callable),
    Argument1Prototype(argument1Prototype),
    Argument2Prototype(argument2Prototype)
{
}


mtsCommandQualifiedRead::~mtsCommandQualifiedRead()
{
    if (this->Argument1Prototype == this->Argument2Prototype)
        delete this->Argument1Prototype;
    else {
        delete this->Argument1Prototype;
        delete this->Argument2Prototype;
    }
}


mtsExecutionResult mtsCommandQualifiedRead::Execute(const mtsGenericObject & argument1,
                                                    mtsGenericObject & argument2)
{
    if (this->IsEnabled()) {
        return this->Callable->Execute(argument1, argument2);
    }
    return mtsExecutionResult::COMMAND_DISABLED;
}


const mtsGenericObject * mtsCommandQualifiedRead::GetArgument1Prototype(void) const
{
    return this->Argument1Prototype;
}


const mtsGenericObject * mtsCommandQualifiedRead::GetArgument2Prototype(void) const
{
    return this->Argument2Prototype;
}


const mtsGenericObject * mtsCommandQualifiedRead::GetArgumentPrototype(void) const
{
    return this->Argument1Prototype;
}


const mtsGenericObject * mtsCommandQualifiedRead::GetResultPrototype(void) const
{
    return this->Argument2Prototype;
}


mtsCallableQualifiedReadBase * mtsCommandQualifiedRead::GetCallable(void) const
{
    return this->Callable;
}


size_t mtsCommandQualifiedRead::NumberOfArguments(void) const
{
    return 2;
}


bool mtsCommandQualifiedRead::Returns(void) const
{
    return false;
}


void mtsCommandQualifiedRead::ToStream(std::ostream & outputStream) const
{
    outputStream << "mtsCommandQualifiedRead: ";
    if (this->Callable) {
        outputStream << this->Name << "(const "
                     << this->GetArgument1Prototype()->Services()->GetName() << "&, "
                     << this->GetArgument2Prototype()->Services()->GetName() << "&) const using \""
                     << *Callable << "\" currently "
                     << (this->IsEnabled() ? "enabled" : "disabled");
    } else {
        outputStream << "not associated to a callable object";
    }
}
