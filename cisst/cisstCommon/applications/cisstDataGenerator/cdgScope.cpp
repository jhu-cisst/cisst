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

#include "cdgScope.h"

const std::string cdgScopeNames[] = {"global",
                                     "class",
                                     "base-class",
                                     "typedef",
                                     "member",
                                     "code"};

cdgScope::cdgScope(unsigned int lineNumber):
    LineNumber(lineNumber)
{
}


const std::string & cdgScope::GetScopeName(void) const
{
    return cdgScopeNames[this->GetScope()];
}


std::string cdgScope::GetDescription(void) const
{
    std::string result = "Scope \"" + this->GetScopeName() + "\"";
    if (!this->Values.empty()) {
        result.append("\nPossible values:");
        const ValuesContainer::const_iterator end = this->Values.end();
        ValuesContainer::const_iterator iter;
        for (iter = this->Values.begin();
             iter != end;
             iter++) {
            result.append("\n- ");
            result.append(iter->second->GetDescription());
        }
        result.append("\n");
    }
    return result;
}


cdgValue * cdgScope::AddValue(const std::string & keyword, const std::string & defaultValue, const bool required)
{
    if (this->Values.FindItem(keyword)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddValue: keyword \"" << keyword << "\" already defined." << std::endl;
        return 0;
    }
    cdgValue * result = new cdgValue(keyword, defaultValue, required);
    Values.AddItem(keyword, result);
    return result;
}


bool cdgScope::HasKeyword(const std::string & keyword) const
{
    if (Values.FindItem(keyword)) {
        return true;
    }
    return false;
}


bool cdgScope::SetValue(const std::string & keyword,
                        const std::string & value,
                        std::string & errorMessage)
{
    cdgValue * field = this->Values.GetItem(keyword);
    if (field) {
        return field->SetValue(value, errorMessage);
    }
    errorMessage = "unhandled keyword \"" + keyword + "\"";
    return false;
}


std::string cdgScope::GetValue(const std::string & keyword) const
{
    cdgValue * value = this->Values.GetItem(keyword);
    if (value) {
        return value->GetValue();
    }
    return std::string("can't find keyword \"") + keyword + "\" in scope \"" + this->GetScopeName() + "\"";
}


bool cdgScope::IsValid(std::string & errorMessage) const
{
    errorMessage.clear();
    bool isValid = true;
    const ValuesContainer::const_iterator end = this->Values.end();
    ValuesContainer::const_iterator iter;
    for (iter = this->Values.begin();
         iter != end;
         iter++) {
        if (!iter->second->IsValid(errorMessage)) {
            isValid = false;
        }
    }
    return isValid;
}


void cdgScope::FillInDefaults(void)
{
    size_t index;
    for (index = 0; index < Scopes.size(); index++) {
        Scopes[index]->FillInDefaults();
    }

    const ValuesContainer::const_iterator end = this->Values.end();
    ValuesContainer::const_iterator iter;
    for (iter = this->Values.begin();
         iter != end;
         iter++) {
        iter->second->FillInDefaults();
    }
}


void cdgScope::GenerateLineComment(std::ostream & outputStream) const
{
    outputStream << "/* source line: " << LineNumber << " */" << std::endl;
}
