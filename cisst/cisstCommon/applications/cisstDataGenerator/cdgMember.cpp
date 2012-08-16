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
    cdgField * field;
    field = this->AddField("name", "", true);
    CMN_ASSERT(field);

    field = this->AddField("type", "", true);
    CMN_ASSERT(field);

    field = this->AddField("description", "", false);
    CMN_ASSERT(field);

    field = this->AddField("default", "", false);
    CMN_ASSERT(field);

    field = this->AddField("accessors", "all", false);
    CMN_ASSERT(field);
    field->AddPossibleValue("none");
    field->AddPossibleValue("references");
    field->AddPossibleValue("set-get");
    field->AddPossibleValue("all");

    field = this->AddField("visibility", "protected", false);
    CMN_ASSERT(field);
    field->AddPossibleValue("public");
    field->AddPossibleValue("protected");
    field->AddPossibleValue("private");

    field = this->AddField("is-data", "true", false);
    CMN_ASSERT(field);
    field->AddPossibleValue("true");
    field->AddPossibleValue("false");

    field = this->AddField("is-size_t", "false", false);
    CMN_ASSERT(field);
    field->AddPossibleValue("true");
    field->AddPossibleValue("false");
}


cdgScope::Type cdgMember::GetScope(void) const
{
    return cdgScope::CDG_MEMBER;
}


bool cdgMember::HasScope(const std::string & CMN_UNUSED(keyword),
                         cdgScope::Stack & CMN_UNUSED(scopes),
                         unsigned int CMN_UNUSED(lineNumber))
{
    return false;
}


void cdgMember::GenerateHeader(std::ostream & outputStream) const
{
    GenerateLineComment(outputStream);
    const std::string type = this->GetFieldValue("type");
    const std::string name = this->GetFieldValue("name");
    const std::string accessors = this->GetFieldValue("accessors");
    outputStream << " protected:" << std::endl
                 << "    " << type << " " << name << "Member; // " << this->GetFieldValue("description") << std::endl;
    if (accessors != "none") {
        outputStream << " public:" << std::endl;
    }
    if ((accessors == "all")
        || (accessors == "set-get")) {
        outputStream << "    /* accessors is: " << accessors << "*/" << std::endl
                     << "    void Get" << name << "(" << type << " & placeHolder) const;" << std::endl
                     << "    void Set" << name << "(const " << type << " & newValue);" << std::endl;
    }
    if ((accessors == "all")
        || (accessors == "references")) {
        outputStream << "    /* accessors is: " << accessors << "*/" << std::endl
                     << "    const " << type << " & " << name << "(void) const;" << std::endl
                     << "    " << type << " & " << name << "(void);" << std::endl;
    }
}


void cdgMember::GenerateCode(std::ostream & outputStream) const
{
    GenerateLineComment(outputStream);
    const std::string type = this->GetFieldValue("type");
    const std::string name = this->GetFieldValue("name");
    const std::string accessors = this->GetFieldValue("accessors");
    std::string returnType;
    if (UsesClassTypedef) {
        returnType = this->ClassName + "::" + type;
    } else {
        returnType = type;
    }

    if ((accessors == "all")
        || (accessors == "set-get")) {
        outputStream << std::endl
                     << "/* accessors is: " << accessors << "*/" << std::endl
                     << "void " << this->ClassName << "::Get" << name << "(" << type << " & placeHolder) const" << std::endl
                     << "{" << std::endl
                     << "    placeHolder = this->" << name << "Member;" << std::endl
                     << "}" << std::endl
                     << std::endl
                     << "void " << this->ClassName << "::Set" << name << "(const " << type << " & newValue)" << std::endl
                     << "{" << std::endl
                     << "    this->" << name << "Member = newValue;" << std::endl
                     << "}" << std::endl
                     << std::endl;
    }

    if ((accessors == "all")
        || (accessors == "references")) {
        outputStream << std::endl
                     << "/* accessors is: " << accessors << "*/" << std::endl
                     << "const " << type << " & " << this->ClassName << "::" << name << "(void) const" << std::endl
                     << "{" << std::endl
                     << "    return this->" << name << "Member;" << std::endl
                     << "}" << std::endl
                     << std::endl
                     << type << " & " << this->ClassName << "::" << name << "(void)" << std::endl
                     << "{" << std::endl
                     << "    return this->" << name << "Member;" << std::endl
                     << "}" << std::endl
                     << std::endl;
    }
}
