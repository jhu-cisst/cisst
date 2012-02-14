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

#include "cdgClass.h"

CMN_IMPLEMENT_SERVICES(cdgClass);


cdgClass::cdgClass(unsigned int lineNumber):
    cdgScope(lineNumber)
{}


cdgScope::Type cdgClass::GetScope(void) const
{
    return cdgScope::CDG_CLASS;
}


bool cdgClass::HasKeyword(const std::string & keyword) const
{
    if ((keyword == "name")
        || (keyword == "attribute")) {
        return true;
    }
    return false;
}


bool cdgClass::HasScope(const std::string & keyword,
                        cdgScope::Stack & scopes,
                        unsigned lineNumber)
{
    if (keyword == "typedef") {
        cdgTypedef * newTypedef = new cdgTypedef(lineNumber);
        scopes.push_back(newTypedef);
        Scopes.push_back(newTypedef);
        Typedefs.push_back(newTypedef);
        return true;
    } else if (keyword == "member") {
        cdgMember * newMember = new cdgMember(lineNumber);
        scopes.push_back(newMember);
        Scopes.push_back(newMember);
        Members.push_back(newMember);
        return true;
    } else if (keyword == "base-class") {
        cdgBaseClass * newBaseClass = new cdgBaseClass(lineNumber);
        scopes.push_back(newBaseClass);
        BaseClasses.push_back(newBaseClass);
        return true;
    } else if (keyword == "inline-header") {
        cdgInline * newCode = new cdgInline(lineNumber, cdgInline::CDG_INLINE_HEADER);
        scopes.push_back(newCode);
        Scopes.push_back(newCode);
        return true;
    } else if (keyword == "inline-code") {
        cdgInline * newCode = new cdgInline(lineNumber, cdgInline::CDG_INLINE_CODE);
        scopes.push_back(newCode);
        Scopes.push_back(newCode);
        return true;
    }
    return false;
}


bool cdgClass::SetValue(const std::string & keyword,
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
    if (keyword == "attribute") {
        if (!this->Attribute.empty()) {
            errorMessage = "attribute already set";
            return false;
        }
        this->Attribute = value;
        return true;
    }
    errorMessage = "unhandled keyword \"" + keyword + "\"";
    return false;
}


bool cdgClass::IsValid(std::string & errorMessage) const
{
    errorMessage.clear();
    bool isValid = true;
    if (this->Name.empty()) {
        isValid = false;
        errorMessage += " [no name defined] ";
    }
    return isValid;
}


void cdgClass::GenerateHeader(std::ostream & outputStream) const
{
    GenerateLineComment(outputStream);

    size_t index;
    for (index = 0; index < Includes.size(); index++) {
        outputStream << "#include " << Includes[index] << std::endl;
    }
    outputStream << "class " << Attribute << " " << Name;

    if (BaseClasses.size() != 0) {
        outputStream << ": ";
    }
    for (index = 0; index < BaseClasses.size(); index++) {
        BaseClasses[index]->GenerateHeader(outputStream);
        if (index != (BaseClasses.size() - 1)) {
            outputStream << ", ";
        }
    }
    outputStream << std::endl
                 << "{" << std::endl;

    // constructors and destructor
    outputStream << " /* default constructors and destructors. */" << std::endl
                 << " public:" << std::endl
                 << "    " << Name << "(void);" << std::endl
                 << "    " << Name << "(const " << Name << " & other);" << std::endl
                 << "    ~" << Name << "();" << std::endl << std::endl;

    for (index = 0; index < Scopes.size(); index++) {
        Scopes[index]->GenerateHeader(outputStream);
    }

    GenerateStandardMethodsHeader(outputStream);

    outputStream << "};" << std::endl;
}


void cdgClass::GenerateStandardMethodsHeader(std::ostream & outputStream) const
{
    outputStream << "    /* default methods */" << std::endl
                 << "    void SerializeRaw(std::ostream & outputStream) const;" << std::endl
                 << "    void DeSerializeRaw(std::istream & inputStream);" << std::endl
                 << "    void ToStream(std::ostream & outputStream) const;" << std::endl
                 << "    void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' '," << std::endl
                 << "                     bool headerOnly = false, const std::string & headerPrefix = \"\") const;" << std::endl;
}


void cdgClass::GenerateCode(std::ostream & outputStream) const
{
    GenerateLineComment(outputStream);

    size_t index;
    GenerateConstructorsCode(outputStream);
    GenerateSerializeRawCode(outputStream);
    GenerateDeSerializeRawCode(outputStream);
    GenerateToStreamCode(outputStream);
    GenerateToStreamRawCode(outputStream);

    for (index = 0; index < Members.size(); index++) {
        Members[index]->ClassName = Name;
        Members[index]->GenerateCode(outputStream);
    }
}


void cdgClass::GenerateConstructorsCode(std::ostream & outputStream) const
{
    unsigned int index;
    // default constructor
    outputStream << Name << "::" << Name << "(void)";
    if (BaseClasses.size() != 0) {
        outputStream << ":";
    }
    outputStream << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        outputStream << "    " << BaseClasses[index]->Type << "()";
        if (index != (BaseClasses.size() - 1)) {
            outputStream << ",";
        }
        outputStream << std::endl;
    }
    outputStream << "{}" << std::endl << std::endl;

    // copy constructor
    outputStream << Name << "::" << Name << "(const " << Name << " & other)";
    if (BaseClasses.size() != 0) {
        outputStream << ":";
    }
    outputStream << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        outputStream << "    " << BaseClasses[index]->Type << "(other)";
        if (index != (BaseClasses.size() - 1)) {
            outputStream << ",";
        }
        outputStream << std::endl;
    }
    outputStream << "{}" << std::endl << std::endl;

    // destructor
    outputStream << Name << "::~" << Name << "(void)"
                 << "{}" << std::endl << std::endl;

}


void cdgClass::GenerateSerializeRawCode(std::ostream & outputStream) const
{
    size_t index;
    outputStream << std::endl
                 << "void " << Name << "::SerializeRaw(std::ostream & outputStream) const" << std::endl
                 << "{" << std::endl;
    for (index = 0; index < Members.size(); index++) {
        outputStream << "    cmnSerializeRaw(outputStream, this->" << Members[index]->Name << ");" << std::endl;
    }
    outputStream << "}" << std::endl
                 << std::endl;
}


void cdgClass::GenerateDeSerializeRawCode(std::ostream & outputStream) const
{
    size_t index;
    outputStream << std::endl
                 << "void " << Name << "::DeSerializeRaw(std::istream & inputStream)" << std::endl
                 << "{" << std::endl;
    for (index = 0; index < Members.size(); index++) {
        outputStream << "    cmnDeSerializeRaw(inputStream, this->" << Members[index]->Name << ");" << std::endl;
    }
    outputStream << "}" << std::endl
                 << std::endl;
}


void cdgClass::GenerateToStreamCode(std::ostream & outputStream) const
{
    size_t index;
    outputStream << std::endl
                 << "void " << Name << "::ToStream(std::ostream & outputStream) const" << std::endl
                 << "{" << std::endl
                 << "    outputStream << \"" << Name << "\" << std::endl;" << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->IsData == "true") {
            outputStream << "    " << BaseClasses[index]->Type << "::ToStream(outputStream);" << std::endl;
        }
    }
    outputStream << "    outputStream" << std::endl;
    for (index = 0; index < Members.size(); index++) {
        outputStream << "        << \"  " << Members[index]->Description << ":\" << this->" << Members[index]->Name;
        if (index == (Members.size() - 1)) {
            outputStream << ";";
        }
        outputStream << std::endl;
    }
    outputStream << "}" << std::endl;
}


void cdgClass::GenerateToStreamRawCode(std::ostream & outputStream) const
{
    size_t index;
    outputStream << std::endl
                 << "void " << Name << "::ToStreamRaw(std::ostream & outputStream, const char delimiter, bool headerOnly, const std::string & headerPrefix) const" << std::endl
                 << "{" << std::endl
                 << "    if (headerOnly) {" << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->IsData == "true") {
            outputStream << "        " << BaseClasses[index]->Type << "::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);" << std::endl;
        }
    }
    outputStream << "        outputStream" << std::endl;
    for (index = 0; index < Members.size(); index++) {
        outputStream << "            << delimiter << headerPrefix << \"-" << Members[index]->Description << "\"";
        if (index == (Members.size() - 1)) {
            outputStream << ";";
        }
        outputStream << std::endl;
    }
    outputStream << "    } else {" << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->IsData == "true") {
            outputStream << "    " << BaseClasses[index]->Type << "::ToStreamRaw(outputStream, delimiter);" << std::endl;
        }
    }
    outputStream << "        outputStream" << std::endl;
    for (index = 0; index < Members.size(); index++) {
        outputStream << "            << delimiter << this->" << Members[index]->Name;
        if (index == (Members.size() - 1)) {
            outputStream << ";";
        }
        outputStream << std::endl;
    }
    outputStream << "    }" << std::endl
                 << "}" << std::endl;
}
