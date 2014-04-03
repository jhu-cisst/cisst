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

#include "cdgScope.h"


cdgScope::KnownScopesContainer cdgScope::KnownScopes;
cdgScope::SubScopesContainer cdgScope::SubScopes;

cdgScope::cdgScope(const std::string & name, size_t lineNumber):
    LineNumber(lineNumber),
    Name(name)
{
}


const std::string & cdgScope::GetScopeName(void) const
{
    return this->Name;
}


cdgField * cdgScope::AddField(const std::string & fieldName, const std::string & defaultValue, const bool required, const std::string & description)
{
    if (this->Fields.FindItem(fieldName)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddField: field name \"" << fieldName << "\" already defined." << std::endl;
        return 0;
    }
    cdgField * result = new cdgField(fieldName, defaultValue, required, description);
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
                             std::string & errorMessage,
                             const bool & allowOverwrite)
{
    cdgField * field = this->Fields.GetItem(fieldName);
    if (field) {
        return field->SetValue(value, errorMessage, allowOverwrite);
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


bool cdgScope::AddKnownScope(const cdgScope & newScope)
{
	const std::string newScopeName = newScope.GetScopeName();
    if (this->KnownScopes.FindItem(newScopeName)) {
        return false;
    }
	// add a dummy pointer to make sure we don't enter an infinite loop
	this->KnownScopes.AddItem(newScopeName, this);
    cdgScope * copy = newScope.Create(0);
	this->KnownScopes.RemoveItem(newScopeName);
    this->KnownScopes.AddItem(copy->GetScopeName(), copy);
	return true;
}


bool cdgScope::AddSubScope(const cdgScope & subScope)
{
    const std::string & subScopeName = subScope.GetScopeName();
    SubScopesContainer::const_iterator iter;
    for (iter = this->SubScopes.equal_range(this->GetScopeName()).first;
         iter != this->SubScopes.equal_range(this->GetScopeName()).second;
         ++iter) {
        if (iter->second == subScopeName) {
            // subscope has already been added
            return false;
        }
    }
    this->SubScopes.insert(std::make_pair(this->GetScopeName(), subScopeName));
    return true;
}


bool cdgScope::HasSubScope(const std::string & keyword,
                           cdgScope::Stack & scopes,
                           size_t lineNumber)
{
    SubScopesContainer::const_iterator iter;
    for (iter = this->SubScopes.equal_range(this->GetScopeName()).first;
         iter != this->SubScopes.equal_range(this->GetScopeName()).second;
         ++iter) {
        if (iter->second == keyword) {
            cdgScope * subScopeFactory = this->KnownScopes.GetItem(keyword);
            cdgScope * newScope = subScopeFactory->Create(lineNumber);
            scopes.push_back(newScope);
            Scopes.push_back(newScope);
            return true;
        }
    }
    return false;
}


bool cdgScope::ValidateRecursion(std::string & errorMessage)
{
    bool valid = true;
    const ScopesContainer::iterator end = Scopes.end();
    ScopesContainer::iterator iter;
    for (iter = Scopes.begin();
         iter != end;
         ++iter) {
        valid = valid && ((*iter)->Validate(errorMessage));
        valid = valid && ((*iter)->ValidateRecursion(errorMessage));
    }
    return valid;
}


void cdgScope::GenerateLineComment(std::ostream & outputStream) const
{
    outputStream << "/* source line: " << LineNumber << " */" << std::endl;
}


void cdgScope::DisplaySyntax(std::ostream & outputStream, size_t offset, bool recursive, bool skipScopeName) const
{
    const std::string indent(offset, ' ');
    if (!skipScopeName) {
        outputStream << indent << this->GetScopeName() << " {" << std::endl;
    }
    SubScopesContainer::const_iterator iter;
    if (recursive) {
        for (iter = this->SubScopes.equal_range(this->GetScopeName()).first;
             iter != this->SubScopes.equal_range(this->GetScopeName()).second;
             ++iter) {
            cdgScope * subScope = KnownScopes.GetItem(iter->second);
            if (subScope) {
                subScope->DisplaySyntax(outputStream, offset + cdgScope::DISPLAY_OFFSET, true);
            }
        }
    } else {
        for (iter = this->SubScopes.equal_range(this->GetScopeName()).first;
             iter != this->SubScopes.equal_range(this->GetScopeName()).second;
             ++iter) {
            const std::string subIndent(offset + cdgScope::DISPLAY_OFFSET, ' ');
            outputStream << subIndent << iter->second << " {}" << std::endl;
        }
    }
    DisplayFieldsSyntax(outputStream, offset + cdgScope::DISPLAY_OFFSET);
    if (!skipScopeName) {
        outputStream << indent << "}" << std::endl;
    }
}


void cdgScope::DisplayFieldsSyntax(std::ostream & outputStream, size_t offset) const
{

    const FieldsContainer::const_iterator end = this->Fields.end();
    FieldsContainer::const_iterator iter;
    for (iter = this->Fields.begin();
         iter != end;
         iter++) {
        iter->second->DisplaySyntax(outputStream, offset);
        outputStream << std::endl;
    }
}
