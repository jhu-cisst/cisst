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

cdgScope::cdgScope(size_t lineNumber):
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
    if (!this->Fields.empty()) {
        result.append("\nPossible fields:");
        const FieldsContainer::const_iterator end = this->Fields.end();
        FieldsContainer::const_iterator iter;
        for (iter = this->Fields.begin();
             iter != end;
             iter++) {
            result.append("\n- ");
            result.append(iter->second->GetDescription());
        }
        result.append("\n");
    }
    return result;
}


cdgField * cdgScope::AddField(const std::string & fieldName, const std::string & defaultValue, const bool required)
{
    if (this->Fields.FindItem(fieldName)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddField: field name \"" << fieldName << "\" already defined." << std::endl;
        return 0;
    }
    cdgField * result = new cdgField(fieldName, defaultValue, required);
    Fields.AddItem(fieldName, result);
    return result;
}


bool cdgScope::HasField(const std::string & fieldName) const
{
    if (Fields.FindItem(fieldName)) {
        return true;
    }
    return false;
}


bool cdgScope::SetFieldValue(const std::string & fieldName,
                             const std::string & value,
                             std::string & errorMessage)
{
    cdgField * field = this->Fields.GetItem(fieldName);
    if (field) {
        return field->SetValue(value, errorMessage);
    }
    errorMessage = "unhandled field name \"" + fieldName + "\"";
    return false;
}


std::string cdgScope::GetFieldValue(const std::string & fieldName) const
{
    cdgField * field = this->Fields.GetItem(fieldName);
    if (field) {
        return field->GetValue();
    }
    return std::string("can't find field name \"") + fieldName + "\" in scope \"" + this->GetScopeName() + "\"";
}


bool cdgScope::IsValid(std::string & errorMessage) const
{
    errorMessage.clear();
    bool isValid = true;
    const FieldsContainer::const_iterator end = this->Fields.end();
    FieldsContainer::const_iterator iter;
    for (iter = this->Fields.begin();
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

    const FieldsContainer::const_iterator end = this->Fields.end();
    FieldsContainer::const_iterator iter;
    for (iter = this->Fields.begin();
         iter != end;
         iter++) {
        iter->second->FillInDefaults();
    }
}


void cdgScope::GenerateLineComment(std::ostream & outputStream) const
{
    outputStream << "/* source line: " << LineNumber << " */" << std::endl;
}
