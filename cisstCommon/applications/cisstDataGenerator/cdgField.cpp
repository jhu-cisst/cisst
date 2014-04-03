/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2010-09-06

  (C) Copyright 2010-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include "cdgField.h"

CMN_IMPLEMENT_SERVICES(cdgField);

cdgField::cdgField(const std::string & keyword, const std::string & defaultValue, const bool required, const std::string & description):
    Keyword(keyword),
    Value(""),
    Default(defaultValue),
    Required(required),
    Description(description)
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


bool cdgField::SetValue(const std::string & value, std::string & errorMessage, const bool & allowOverwrite)
{
    if (!this->Value.empty() && !allowOverwrite) {
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
            errorMessage = value + " not supported. ";
            std::stringstream temp;
            this->DisplaySyntax(temp, 0);
            errorMessage.append(temp.str());
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


void cdgField::DisplaySyntax(std::ostream & outputStream, size_t offsetSize) const
{
    const std::string offset(offsetSize, ' ');
    if (this->Keyword != "") {
        outputStream << offset << this->Keyword
                     << " = <";
        if (!this->PossibleValues.empty()) {
            outputStream << "value must be one of";
            const ValuesContainer::const_iterator end = this->PossibleValues.end();
            ValuesContainer::const_iterator iter = this->PossibleValues.begin();
            for (;iter != end; ++iter) {
                outputStream << " '" << *iter << "'";
            }
        } else {
            outputStream << "user defined string";
        }
        if (this->Default != "") {
            outputStream << ": default is '" << this->Default << "'";
        }
        outputStream << ">; // " << (this->Required ? "(required)" : "(optional)");
    } else {
        outputStream << offset << "C++ code snippet";
    }
    outputStream << " - " << this->Description;
}
