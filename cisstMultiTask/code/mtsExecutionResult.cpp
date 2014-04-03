/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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


const mtsExecutionResult & mtsExecutionResult::operator = (const Enum & value)
{
    this->SetValue(value);
    return *this;
}


const std::string mtsExecutionResult::ToString(const Enum & value)
{
    return cmnData<mtsExecutionResult::Enum>::HumanReadable(value);
}


bool mtsExecutionResult::operator == (const mtsExecutionResult & result) const
{
    return (this->Value() == result.Value());
}


bool mtsExecutionResult::operator != (const mtsExecutionResult & result) const
{
    return !(*this == result);
}


bool mtsExecutionResult::IsOK(void) const
{
    return ((this->Value() == COMMAND_SUCCEEDED) || (this->Value() == COMMAND_QUEUED));
}


mtsExecutionResult::operator bool (void) const
{
    return this->IsOK();
}
