/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2010-09-06

  (C) Copyright 2010-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include "cdgMember.h"

CMN_IMPLEMENT_SERVICES(cdgMember);


cdgMember::cdgMember(size_t lineNumber):
    cdgScope("member", lineNumber),
    UsesClassTypedef(false)
{
    cdgField * field;
    field = this->AddField("name", "", true,
                           "name of the data member, will also be used to generate accessors");
    CMN_ASSERT(field);

    field = this->AddField("type", "", true,
                           "C++ type of the data member (e.g. double, std::string, ...)");
    CMN_ASSERT(field);

    field = this->AddField("description", "", false,
                           "user provided description of the data member");
    CMN_ASSERT(field);

    field = this->AddField("default", "", false,
                           "default value that should be assigned to the data member in the class constructor");
    CMN_ASSERT(field);

    field = this->AddField("accessors", "all/none", false,
                           "indicates which types of accessors should be generated for the data member (default depends on \"visibility\")");
    CMN_ASSERT(field);
    field->AddPossibleValue("none");
    field->AddPossibleValue("references");
    field->AddPossibleValue("set-get");
    field->AddPossibleValue("all");

    field = this->AddField("visibility", "protected", false,
                           "indicates if the data member should be public, ...");
    CMN_ASSERT(field);
    field->AddPossibleValue("public");
    field->AddPossibleValue("protected");
    field->AddPossibleValue("private");

    field = this->AddField("is-data", "true", false,
                           "indicates if the data member is a cisst data type itself");
    CMN_ASSERT(field);
    field->AddPossibleValue("true");
    field->AddPossibleValue("false");

    field = this->AddField("is-size_t", "false", false,
                           "indicates if the data member is a typedef of size_t or size_t");
    CMN_ASSERT(field);
    field->AddPossibleValue("true");
    field->AddPossibleValue("false");

    field = this->AddField("deprecated", "false", false, "indicates if the data member is deprecated, accessors will be marked as deprecated");
    CMN_ASSERT(field);
    field->AddPossibleValue("true");
    field->AddPossibleValue("false");

    this->AddKnownScope(*this);
}


cdgScope::Type cdgMember::GetScope(void) const
{
    return cdgScope::CDG_MEMBER;
}


cdgScope * cdgMember::Create(size_t lineNumber) const
{
    return new cdgMember(lineNumber);
}


bool cdgMember::Validate(std::string & errorMessage)
{
    std::string dummy;
    // set a default description, use the member name
    if (this->GetFieldValue("description") == "") {
        this->SetFieldValue("description",
                            this->GetFieldValue("name"),
                            dummy);
    }
    // check if default has not be overwritten and set real default
    // based on visibility
    if (this->GetFieldValue("accessors") == "all/none") {
        if (this->GetFieldValue("visibility") == "public") {
            this->SetFieldValue("accessors", "none", dummy, true);
        } else {
            this->SetFieldValue("accessors", "all", dummy, true);
        }
    }
    // check that accessors are compatible with visibility
    if (this->GetFieldValue("visibility") == "public") {
        if ((this->GetFieldValue("accessors") == "all")
            || (this->GetFieldValue("accessors") == "references")) {
            errorMessage = errorMessage + "\n" + ClassName + "::" + this->GetFieldValue("name")
                + " can not be declared public if the accessors are set to \"all\" or \"references\"";
            return false;
        }
        MemberName = this->GetFieldValue("name");
    } else {
        MemberName = "m" + this->GetFieldValue("name");
    }
    // check if the type is exactly size_t
    std::string type = this->GetFieldValue("type");
    if (type == "size_t") {
        this->SetFieldValue("is-size_t", "true", dummy, true);
    }
    // check if the type is a C array, looking for square brackets
    size_t openSquareBracket = std::count(type.begin(), type.end(), '[');
    size_t closeSquareBracket = std::count(type.begin(), type.end(), ']');
    if (openSquareBracket != closeSquareBracket) {
        errorMessage = errorMessage + "\n" + ClassName + "::" + this->GetFieldValue("name")
            + " number of \"[\" and \"]\" don't match";
        return false;
    }
    // C-array
    if (openSquareBracket != 0) {
        IsCArray = true;
        CArrayDimension = openSquareBracket;
        size_t firstOpenBracket = type.find('[');
        CArrayType = type.substr(0, firstOpenBracket);
        CArraySize = type.substr(firstOpenBracket);
        if (!(this->GetFieldValue("accessors") == "none")) {
            errorMessage = errorMessage + "\n" + ClassName + "::" + this->GetFieldValue("name")
                + " accessors \"all\", \"references\" or \"set-get\" not supported for C arrays, possible value is \"none\" (you can also make this member public).  Current value is \""
                + this->GetFieldValue("accessors") + "\"";
            return false;
        }
    } else {
        IsCArray = false;
    }

    return true;
}


void cdgMember::GenerateHeader(std::ostream & outputStream) const
{
    GenerateLineComment(outputStream);
    const std::string type = this->GetFieldValue("type");
    const std::string name = this->GetFieldValue("name");
    const std::string accessors = this->GetFieldValue("accessors");
    const std::string deprecated = this->GetFieldValue("deprecated");
    std::string depr = " ";
    if (deprecated == "true") {
        depr = " CISST_DEPRECATED ";
    }
    outputStream << " " << this->GetFieldValue("visibility") << ":" << std::endl;
    if (IsCArray) {
        outputStream << "    " << CArrayType << " " << MemberName << CArraySize << "; // " << this->GetFieldValue("description") << std::endl;
    } else {
        outputStream << "    " << type << " " << MemberName << "; // " << this->GetFieldValue("description") << std::endl;
    }
    if (accessors != "none") {
        outputStream << " public:" << std::endl;
    }
    if ((accessors == "all")
        || (accessors == "set-get")) {
        outputStream << "    /* accessors is set to: " << accessors << " */" << std::endl
                     << "    " << depr << "void Get" << name << "(" << type << " & placeHolder) const;" << std::endl
                     << "    " << depr << "void Set" << name << "(const " << type << " & newValue);" << std::endl;
    }
    if ((accessors == "all")
        || (accessors == "references")) {
        std::string returnType = type + " & ";
        outputStream << "    /* accessors is set to: " << accessors << " */" << std::endl
                     << "    " << depr << "const " << returnType << name << "(void) const;" << std::endl
                     << "    " << depr << returnType << name << "(void);" << std::endl;
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
                     << "/* accessors is set to: " << accessors << " */" << std::endl
                     << "void " << this->ClassName << "::Get" << name << "(" << type << " & placeHolder) const" << std::endl
                     << "{" << std::endl
                     << "    placeHolder = this->" << MemberName << ";" << std::endl
                     << "}" << std::endl
                     << std::endl
                     << "void " << this->ClassName << "::Set" << name << "(const " << type << " & newValue)" << std::endl
                     << "{" << std::endl
                     << "    this->" << MemberName << " = newValue;" << std::endl
                     << "}" << std::endl
                     << std::endl;
    }

    if ((accessors == "all")
        || (accessors == "references")) {
        std::string returnType = type + " & ";
        outputStream << std::endl
                     << "/* accessors is set to: " << accessors << " */" << std::endl
                     << "const " << returnType << this->ClassName << "::" << name << "(void) const" << std::endl
                     << "{" << std::endl
                     << "    return this->" << MemberName << ";" << std::endl
                     << "}" << std::endl
                     << std::endl
                     << returnType << this->ClassName << "::" << name << "(void)" << std::endl
                     << "{" << std::endl
                     << "    return this->" << MemberName << ";" << std::endl
                     << "}" << std::endl
                     << std::endl;
    }
}
