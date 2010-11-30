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
    this->Value = DEV_OK;
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
    static const std::string stringDEV_OK("OK");
    static const std::string stringDEV_NOT_OK("Failed");
    static const std::string stringBAD_COMMAND("Failed, bad command");
    static const std::string stringNO_MAILBOX("Failed, no mailbox");
    static const std::string stringBAD_INPUT("Failed, bad input");
    static const std::string stringNO_INTERFACE("Failed, no interface");
    static const std::string stringMAILBOX_FULL("Failed, mailbox full");
    static const std::string stringDISABLED("Failed, disabled");
    static const std::string stringCOMMAND_FAILED("Failed, underlying method or function returned 'false'");
    static const std::string stringDEFAULT("Unknown execution result");
    switch (value) {
    case DEV_OK:
        return stringDEV_OK;
        break;
    case DEV_NOT_OK:
        return stringDEV_NOT_OK;
        break;
    case BAD_COMMAND:
        return stringBAD_COMMAND;
        break;
    case NO_MAILBOX:
        return stringNO_MAILBOX;
        break;
    case BAD_INPUT:
        return stringBAD_INPUT;
        break;
    case NO_INTERFACE:
        return stringNO_INTERFACE;
        break;
    case MAILBOX_FULL:
        return stringMAILBOX_FULL;
        break;
    case DISABLED:
        return stringDISABLED;
        break;
    case COMMAND_FAILED:  // Read or QualifiedRead returned 'false'
        return stringCOMMAND_FAILED;
        break;
    default:
        return stringDEFAULT;
    }
}


bool mtsExecutionResult::operator == (const mtsExecutionResult & result) const
{
    return (this->Value == result.Value);
}


bool mtsExecutionResult::operator != (const mtsExecutionResult & result) const
{
    return !(*this == result);
}
