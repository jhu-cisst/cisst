/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2010-09-14

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#include <cisstMultiTask/mtsExecutionResult.h>
#include <iostream>


mtsExecutionResult::mtsExecutionResult(void)
{
    this->Value = COMMAND_SUCCEEDED;
}


mtsExecutionResult::mtsExecutionResult(const Enum & value)
{
    this->Value = value;
}


mtsExecutionResult::~mtsExecutionResult()
{}


const mtsExecutionResult & mtsExecutionResult::operator = (const Enum & value)
{
    this->Value = value;
    return *this;
}


void mtsExecutionResult::ToStream(std::ostream & outputStream) const
{
    outputStream << mtsExecutionResult::ToString(this->Value);
}


const std::string & mtsExecutionResult::ToString(const Enum & value)
{
    static const std::string resultDescription[] = {
        "command succeeded",
        "command queued",
        "function not bound to a command",
        "queued command has no mailbox",
        "command disabled",
        "interface command mailbox full",
        "command argument queue full",
        "invalid input type",
        "underlying method or function returned \"false\"",
        "network error",
        "invalid network command Id", 
        "unable to dynamically create an argument",
        "serialization failed"
        "deserialization failed"
    };
    return resultDescription[value];
}


bool mtsExecutionResult::operator == (const mtsExecutionResult & result) const
{
    return (this->Value == result.Value);
}


bool mtsExecutionResult::operator != (const mtsExecutionResult & result) const
{
    return !(*this == result);
}


bool mtsExecutionResult::IsOK(void) const
{
    return ((this->Value == COMMAND_SUCCEEDED) || (this->Value == COMMAND_QUEUED));
}


mtsExecutionResult::operator bool (void) const
{
    return this->IsOK();
}
