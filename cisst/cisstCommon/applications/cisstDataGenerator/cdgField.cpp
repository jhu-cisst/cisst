/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2010-09-06

  (C) Copyright 2010-2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include "cdgField.h"

CMN_IMPLEMENT_SERVICES(cdgField);

cdgField::cdgField(const std::string & keyword, const std::string & defaultValue, const bool required):
    Keyword(keyword),
    Value(""),
    Default(defaultValue),
    Required(required)
{
}


void cdgField::AddPossibleValue(const std::string & possibleValue)
{
    this->PossibleValues.push_back(possibleValue);
}


const std::string & cdgField::GetValue(void) const
{
    return this->Value;
}


std::string cdgField::GetDescription(void) const
{
    std::string result = "\"" + this->Keyword + "\" can be ";
    if (this->PossibleValues.empty()) {
        result.append("anything");
    } else {
        result.append("must be one of [");
        const ValuesContainer::const_iterator end = this->PossibleValues.end();
        ValuesContainer::const_iterator iter;
        for (iter = this->PossibleValues.begin();
             iter != end;
             iter++) {
            result.append(" ");
            result.append(*iter);
            result.append(" ");
        }
        result.append("]");
    }
    if (this->Required) {
        result.append(".  This is required.");
    } else {
        result.append(".  This is optional.");
    }
    return result;
}


bool cdgField::SetValue(const std::string & value, std::string & errorMessage)
{
    if (!this->Value.empty()) {
        errorMessage = Keyword + " already set";
        return false;
    }
    if (!this->PossibleValues.empty()) {
        const ValuesContainer::const_iterator end = this->PossibleValues.end();
        ValuesContainer::const_iterator iter;
        iter = this->PossibleValues.begin();
        bool found = false;
        while ((iter != end) && (found == false)) {
            if (*iter == value) {
                found = true;
            }
            iter++;
        }
        if (!found) {
            errorMessage = value + " not supported. " + this->GetDescription();
            return false;
        }
    }
    this->Value = value;
    return true;
}


bool cdgField::IsValid(std::string & errorMessage) const
{
    if (this->Required && (this->Value == "")) {
        errorMessage.append("\"");
        errorMessage.append(this->Keyword);
        errorMessage.append("\" is required ");

        return false;
    }
    return true;
}


void cdgField::FillInDefaults(void)
{
    if ((this->Default != "") && this->Value.empty()) {
        this->Value = this->Default;
    }
}
