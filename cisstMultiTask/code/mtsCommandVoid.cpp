/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ankur Kapoor, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstMultiTask/mtsCommandVoid.h>
#include <cisstMultiTask/mtsCallableVoidBase.h>


mtsCommandVoid::mtsCommandVoid(const ThisType & CMN_UNUSED(other)):
    BaseType()
{}


mtsCommandVoid::mtsCommandVoid(void):
    BaseType(),
    Callable(0)
{}


mtsCommandVoid::mtsCommandVoid(mtsCallableVoidBase * callable, const std::string & name):
    BaseType(name),
    Callable(callable)
{}


mtsCommandVoid::~mtsCommandVoid()
{}


mtsExecutionResult mtsCommandVoid::Execute(mtsBlockingType CMN_UNUSED(blocking))
{
    if (this->IsEnabled()) {
        return this->Callable->Execute();
    }
    return mtsExecutionResult::COMMAND_DISABLED;
}


void mtsCommandVoid::ToStream(std::ostream & outputStream) const
{
    outputStream << "mtsCommandVoid: ";
    if (this->Callable) {
        outputStream << this->Name << "(void) using " << *Callable << " currently "
                     << (this->IsEnabled() ? "enabled" : "disabled");
    } else {
        outputStream << "not associated to a callable object";
    }
}


mtsCallableVoidBase * mtsCommandVoid::GetCallable(void) const
{
    return this->Callable;
}


size_t mtsCommandVoid::NumberOfArguments(void) const
{
    return 0;
}


bool mtsCommandVoid::Returns(void) const
{
    return false;
}
