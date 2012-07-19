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
        Scopes.push_back(newBaseClass);
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
    outputStream << "class " << this->Attribute << " " << this->Name;

    if (BaseClasses.size() != 0) {
        outputStream << ": ";
    }
    for (index = 0; index < BaseClasses.size(); index++) {
        BaseClasses[index]->GenerateHeaderInheritance(outputStream);
        if (index != (BaseClasses.size() - 1)) {
            outputStream << ", ";
        }
    }
    outputStream << std::endl
                 << "{" << std::endl;

    // constructors and destructor
    outputStream << " /* default constructors and destructors. */" << std::endl
                 << " public:" << std::endl
                 << "    " << this->Name << "(void);" << std::endl
                 << "    " << this->Name << "(const " << this->Name << " & other);" << std::endl
                 << "    ~" << this->Name << "();" << std::endl << std::endl;

    for (index = 0; index < Scopes.size(); index++) {
        Scopes[index]->GenerateHeader(outputStream);
    }

    GenerateStandardMethodsHeader(outputStream);

    outputStream << "};" << std::endl;

    GenerateStandardFunctionsHeader(outputStream);

    GenerateDataFunctionsHeader(outputStream);
}


void cdgClass::GenerateStandardMethodsHeader(std::ostream & outputStream) const
{
    outputStream << "    /* default methods */" << std::endl
                 << " public:" << std::endl
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
    GenerateMethodSerializeRawCode(outputStream);
    GenerateMethodDeSerializeRawCode(outputStream);
    GenerateMethodToStreamCode(outputStream);
    GenerateMethodToStreamRawCode(outputStream);

    for (index = 0; index < Members.size(); index++) {
        Members[index]->ClassName = this->Name;
        Members[index]->GenerateCode(outputStream);
    }

    GenerateStandardFunctionsCode(outputStream);
}


void cdgClass::GenerateConstructorsCode(std::ostream & outputStream) const
{
    unsigned int index;

    std::stringstream defaultConstructor;
    std::stringstream copyConstructor;

    // default constructor
    defaultConstructor << this->Name << "::" << this->Name << "(void)";
    copyConstructor    << this->Name << "::" << this->Name << "(const " << this->Name << " & other)";
    if ((BaseClasses.size() + Members.size())!= 0) {
        defaultConstructor << ":";
        copyConstructor    << ":";
    }
    defaultConstructor << std::endl;
    copyConstructor    << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        defaultConstructor << "    " << BaseClasses[index]->Type << "()";
        copyConstructor    << "    " << BaseClasses[index]->Type << "(other)";
        if ((index != (BaseClasses.size() - 1)) || (!Members.empty())) {
            defaultConstructor << ",";
            copyConstructor    << ",";
        }
        defaultConstructor << std::endl;
        copyConstructor    << std::endl;
    }
    for (index = 0; index < Members.size(); index++) {
        defaultConstructor << "    " << Members[index]->Name << "Member(" << Members[index]->Default << ")";
        copyConstructor    << "    " << Members[index]->Name << "Member(other." << Members[index]->Name << "Member)";
        if (index != (Members.size() - 1)) {
            defaultConstructor << ",";
            copyConstructor    << ",";
        }
        defaultConstructor << std::endl;
        copyConstructor    << std::endl;
    }

    defaultConstructor << "{}" << std::endl << std::endl;
    copyConstructor    << "{}" << std::endl << std::endl;

    outputStream << defaultConstructor.str();
    outputStream << copyConstructor.str();

    // destructor
    outputStream << this->Name << "::~" << this->Name << "(void)"
                 << "{}" << std::endl << std::endl;

}


void cdgClass::GenerateMethodSerializeRawCode(std::ostream & outputStream) const
{
    size_t index;
    outputStream << std::endl
                 << "void " << this->Name << "::SerializeRaw(std::ostream & outputStream) const" << std::endl
                 << "{" << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->IsData == "true") {
            outputStream << "    " << BaseClasses[index]->Type << "::SerializeRaw(outputStream);" << std::endl;
        }
    }
    for (index = 0; index < Members.size(); index++) {
        outputStream << "    cmnSerializeRaw(outputStream, this->" << Members[index]->Name << "Member);" << std::endl;
    }
    outputStream << "}" << std::endl
                 << std::endl;
}


void cdgClass::GenerateMethodDeSerializeRawCode(std::ostream & outputStream) const
{
    size_t index;
    outputStream << std::endl
                 << "void " << this->Name << "::DeSerializeRaw(std::istream & inputStream)" << std::endl
                 << "{" << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->IsData == "true") {
            outputStream << "    " << BaseClasses[index]->Type << "::DeSerializeRaw(inputStream);" << std::endl;
        }
    }
    for (index = 0; index < Members.size(); index++) {
        outputStream << "    cmnDeSerializeRaw(inputStream, this->" << Members[index]->Name << "Member);" << std::endl;
    }
    outputStream << "}" << std::endl
                 << std::endl;
}


void cdgClass::GenerateMethodToStreamCode(std::ostream & outputStream) const
{
    size_t index;
    outputStream << std::endl
                 << "void " << this->Name << "::ToStream(std::ostream & outputStream) const" << std::endl
                 << "{" << std::endl
                 << "    outputStream << \"" << this->Name << "\" << std::endl;" << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->IsData == "true") {
            outputStream << "    " << BaseClasses[index]->Type << "::ToStream(outputStream);" << std::endl;
        }
    }
    outputStream << "    outputStream" << std::endl;
    for (index = 0; index < Members.size(); index++) {
        outputStream << "        << \"  " << Members[index]->Description << ":\" << this->" << Members[index]->Name << "Member";
        if (index == (Members.size() - 1)) {
            outputStream << ";";
        }
        outputStream << std::endl;
    }
    outputStream << "}" << std::endl;
}


void cdgClass::GenerateMethodToStreamRawCode(std::ostream & outputStream) const
{
    size_t index;
    outputStream << std::endl
                 << "void " << this->Name << "::ToStreamRaw(std::ostream & outputStream, const char delimiter, bool headerOnly, const std::string & headerPrefix) const" << std::endl
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
        outputStream << "            << delimiter << this->" << Members[index]->Name << "Member";
        if (index == (Members.size() - 1)) {
            outputStream << ";";
        }
        outputStream << std::endl;
    }
    outputStream << "    }" << std::endl
                 << "}" << std::endl;
}


void cdgClass::GenerateStandardFunctionsHeader(std::ostream & outputStream) const
{
    outputStream << "/* default functions */" << std::endl
                 << "void cmnSerializeRaw(std::ostream & outputStream, const " << this->Name << " & object);" << std::endl
                 << "void cmnDeSerializeRaw(std::istream & inputStream, " << this->Name << " & placeHolder);" << std::endl;
}


void cdgClass::GenerateDataFunctionsHeader(std::ostream & outputStream) const
{
    outputStream << "/* data functions */" << std::endl
                 << "void cmnDataCopy(" << this->Name << " & destination, const " << this->Name << " & source);" << std::endl
                 << "void cmnDataSerializeBinary(std::ostream & outputStream, const " << this->Name << " & data);" << std::endl
                 << "void cmnDataDeSerializeBinary(std::istream & inputStream, " << this->Name << " & data," << std::endl
                 << "                              const cmnDataFormat & remoteFormat, const cmnDataFormat & localFormat);"<< std::endl
                 << "bool cmnDataScalarNumberIsFixed(const " << this->Name << " & data);" << std::endl
                 << "size_t cmnDataScalarNumber(const " << this->Name << " & data);" << std::endl
                 << "std::string cmnDataScalarDescription(const " << this->Name << " & data, const size_t & index," << std::endl
                 << "                                     const char * userDescription = \"" << this->Name << "\");" << std::endl
                 << " double cmnDataScalar(const " << this->Name << " & data, const size_t & index);" << std::endl;
}


void cdgClass::GenerateStandardFunctionsCode(std::ostream & outputStream) const
{
    outputStream << "/* default functions */" << std::endl
                 << "void cmnSerializeRaw(std::ostream & outputStream, const " << this->Name << " & object)" << std::endl
                 << "{" << std::endl
                 << "    object.SerializeRaw(outputStream);" << std::endl
                 << "}" << std::endl
                 << "void cmnDeSerializeRaw(std::istream & inputStream, " << this->Name << " & placeHolder)" << std::endl
                 << "{" << std::endl
                 << "    placeHolder.DeSerializeRaw(inputStream);" << std::endl
                 << "}" << std::endl;
}
