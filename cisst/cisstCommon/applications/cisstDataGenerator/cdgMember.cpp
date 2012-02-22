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

#include "cdgMember.h"

CMN_IMPLEMENT_SERVICES(cdgMember);


cdgMember::cdgMember(unsigned int lineNumber):
    cdgScope(lineNumber),
    UsesClassTypedef(false)
{
}


cdgScope::Type cdgMember::GetScope(void) const
{
    return cdgScope::CDG_MEMBER;
}


bool cdgMember::HasKeyword(const std::string & keyword) const
{
    if ((keyword == "name")
        || (keyword == "type")
        || (keyword == "description")
        || (keyword == "default")
        || (keyword == "accessors")
        || (keyword == "visibility")) {
        return true;
    }
    return false;
}


bool cdgMember::HasScope(const std::string & CMN_UNUSED(keyword),
                         cdgScope::Stack & CMN_UNUSED(scopes),
                         unsigned int CMN_UNUSED(lineNumber))
{
    return false;
}


bool cdgMember::SetValue(const std::string & keyword,
                         const std::string & value,
                         std::string & errorMessage)
{
    errorMessage.clear();
    if (keyword == "name") {
        if (!this->Name.empty()) {
            errorMessage = "name already set";
            return false;
        }
        this->Name = value;
        return true;
    }
    if (keyword == "type") {
        if (!this->Type.empty()) {
            errorMessage = "type already set";
            return false;
        }
        this->Type = value;
        return true;
    }
    if (keyword == "description") {
        if (!this->Description.empty()) {
            errorMessage = "description already set";
            return false;
        }
        this->Description = value;
        return true;
    }
    if (keyword == "default") {
        std::cerr << "------------ default not handled yet ---------" << std::endl;
        return true;
    }
    if (keyword == "accessors") {
        std::cerr << "------------ accessors not handled yet ---------" << std::endl;
        return true;
    }
    if (keyword == "visibility") {
        if (!this->Visibility.empty()) {
            errorMessage = "visibility already set";
            return false;
        }
        if ((value == "public")
            || (value == "private")
            || (value == "protected")) {
            this->Visibility = value;
            return true;
        } else {
            errorMessage = "visibility must be \"public\", \"protected\" or \"private\", not \"" + keyword + "\"";
            return false;
        }
    }
    errorMessage = "unhandled keyword \"" + keyword + "\"";
    return false;
}


bool cdgMember::IsValid(std::string & errorMessage) const
{
    errorMessage.clear();
    bool isValid = true;
    if (this->Name.empty()) {
        isValid = false;
        errorMessage += " [no name defined] ";
    }
    if (this->Type.empty()) {
        isValid = false;
        errorMessage += " [no type defined] ";
    }
    return isValid;
}


void cdgMember::GenerateHeader(std::ostream & outputStream) const
{
    GenerateLineComment(outputStream);
    outputStream << " protected:" << std::endl
                 << "    " << Type << " " << Name << "; // " << Description << std::endl
                 << " public:" << std::endl
                 << "    const " << Type << " & Get" << Name << "(void) const;" << std::endl
                 << "    void Set" << Name << "(const " << Type << " & newValue);" << std::endl;
}


void cdgMember::GenerateCode(std::ostream & outputStream) const
{
    GenerateLineComment(outputStream);
    std::string returnType;
    if (UsesClassTypedef) {
        returnType = ClassName + "::" + Type;
    } else {
        returnType = Type;
    }
    outputStream << std::endl
                 << "const " << returnType << " & " << ClassName << "::Get" << Name << "(void) const" << std::endl
                 << "{" << std::endl
                 << "    return this->" << Name << ";" << std::endl
                 << "}" << std::endl
                 << std::endl
                 << "void " << ClassName << "::Set" << Name << "(const " << Type << " & newValue)" << std::endl
                 << "{" << std::endl
                 << "    this->" << Name << " = newValue;" << std::endl
                 << "}" << std::endl
                 << std::endl;
}
