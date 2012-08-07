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
{
    this->AddField("name", "", true);
    this->AddField("attribute", "", false);
}



cdgScope::Type cdgClass::GetScope(void) const
{
    return cdgScope::CDG_CLASS;
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


void cdgClass::GenerateHeader(std::ostream & outputStream) const
{
    GenerateLineComment(outputStream);

    size_t index;
    for (index = 0; index < Includes.size(); index++) {
        outputStream << "#include " << Includes[index] << std::endl;
    }

    outputStream << "class " << this->GetFieldValue("name") << ";" << std::endl;

    GenerateStandardFunctionsHeader(outputStream);

    GenerateDataFunctionsHeader(outputStream);

    outputStream << "class " << this->GetFieldValue("attribute") << " " << this->GetFieldValue("name");

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
                 << "    " << this->GetFieldValue("name") << "(void);" << std::endl
                 << "    " << this->GetFieldValue("name") << "(const " << this->GetFieldValue("name") << " & other);" << std::endl
                 << "    ~" << this->GetFieldValue("name") << "();" << std::endl << std::endl;

    for (index = 0; index < Scopes.size(); index++) {
        Scopes[index]->GenerateHeader(outputStream);
    }

    GenerateStandardMethodsHeader(outputStream);

    GenerateDataMethodsHeader(outputStream);

    outputStream << "};" << std::endl;
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


void cdgClass::GenerateDataMethodsHeader(std::ostream & outputStream) const
{
    outputStream << "    /* default data methods */" << std::endl
                 << " public:" << std::endl
                 << "    inline void SerializeBinary(std::ostream & outputStream) const {" << std::endl
                 << "        cmnDataSerializeBinary(outputStream, *this);" << std::endl
                 << "    }" << std::endl
                 << "    inline void DeSerializeBinary(std::istream & inputStream, const cmnDataFormat & remoteFormat, const cmnDataFormat & localFormat) {" << std::endl
                 << "        cmnDataDeSerializeBinary(inputStream, *this, remoteFormat, localFormat);" << std::endl
                 << "    }" << std::endl
                 << "    inline bool ScalarNumberIsFixed(void) const {" << std::endl
                 << "        return cmnDataScalarNumberIsFixed(*this);" << std::endl
                 << "    }" << std::endl
                 << "    inline size_t ScalarNumber(void) const {" << std::endl
                 << "        return cmnDataScalarNumber(*this);" << std::endl
                 << "    }" << std::endl
                 << "    inline double Scalar(size_t index) const {" << std::endl
                 << "        return cmnDataScalar(*this, index);" << std::endl
                 << "    }" << std::endl
                 << "    inline std::string ScalarDescription(size_t index) const {" << std::endl
                 << "        return cmnDataScalarDescription(*this, index);" << std::endl
                 << "    }" << std::endl;
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
        Members[index]->ClassName = this->GetFieldValue("name");
        Members[index]->GenerateCode(outputStream);
    }

    GenerateStandardFunctionsCode(outputStream);
    GenerateDataFunctionsCode(outputStream);
}


void cdgClass::GenerateConstructorsCode(std::ostream & outputStream) const
{
    unsigned int index;

    std::stringstream defaultConstructor;
    std::stringstream copyConstructor;

    // default constructor
    defaultConstructor << this->GetFieldValue("name") << "::" << this->GetFieldValue("name") << "(void)";
    copyConstructor    << this->GetFieldValue("name") << "::" << this->GetFieldValue("name") << "(const " << this->GetFieldValue("name") << " & other)";
    if ((BaseClasses.size() + Members.size())!= 0) {
        defaultConstructor << ":";
        copyConstructor    << ":";
    }
    defaultConstructor << std::endl;
    copyConstructor    << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        defaultConstructor << "    " << BaseClasses[index]->GetFieldValue("type") << "()";
        copyConstructor    << "    " << BaseClasses[index]->GetFieldValue("type") << "(other)";
        if ((index != (BaseClasses.size() - 1)) || (!Members.empty())) {
            defaultConstructor << ",";
            copyConstructor    << ",";
        }
        defaultConstructor << std::endl;
        copyConstructor    << std::endl;
    }
    for (index = 0; index < Members.size(); index++) {
        defaultConstructor << "    " << Members[index]->GetFieldValue("name") << "Member(" << Members[index]->GetFieldValue("default") << ")";
        copyConstructor    << "    " << Members[index]->GetFieldValue("name") << "Member(other." << Members[index]->GetFieldValue("name") << "Member)";
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
    outputStream << this->GetFieldValue("name") << "::~" << this->GetFieldValue("name") << "(void)"
                 << "{}" << std::endl << std::endl;

}


void cdgClass::GenerateMethodSerializeRawCode(std::ostream & outputStream) const
{
    size_t index;
    outputStream << std::endl
                 << "void " << this->GetFieldValue("name") << "::SerializeRaw(std::ostream & outputStream) const" << std::endl
                 << "{" << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->GetFieldValue("is-data") == "true") {
            outputStream << "    " << BaseClasses[index]->GetFieldValue("type") << "::SerializeRaw(outputStream);" << std::endl;
        }
    }
    for (index = 0; index < Members.size(); index++) {
        if (Members[index]->GetFieldValue("is-data") == "true") {
            outputStream << "    cmnSerializeRaw(outputStream, this->" << Members[index]->GetFieldValue("name") << "Member);" << std::endl;
        }
    }
    outputStream << "}" << std::endl
                 << std::endl;
}


void cdgClass::GenerateMethodDeSerializeRawCode(std::ostream & outputStream) const
{
    size_t index;
    outputStream << std::endl
                 << "void " << this->GetFieldValue("name") << "::DeSerializeRaw(std::istream & inputStream)" << std::endl
                 << "{" << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->GetFieldValue("is-data") == "true") {
            outputStream << "    " << BaseClasses[index]->GetFieldValue("type") << "::DeSerializeRaw(inputStream);" << std::endl;
        }
    }
    for (index = 0; index < Members.size(); index++) {
        if (Members[index]->GetFieldValue("is-data") == "true") {
            outputStream << "    cmnDeSerializeRaw(inputStream, this->" << Members[index]->GetFieldValue("name") << "Member);" << std::endl;
        }
    }
    outputStream << "}" << std::endl
                 << std::endl;
}


void cdgClass::GenerateMethodToStreamCode(std::ostream & outputStream) const
{
    size_t index;
    outputStream << std::endl
                 << "void " << this->GetFieldValue("name") << "::ToStream(std::ostream & outputStream) const" << std::endl
                 << "{" << std::endl
                 << "    outputStream << \"" << this->GetFieldValue("name") << "\" << std::endl;" << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->GetFieldValue("is-data") == "true") {
            outputStream << "    " << BaseClasses[index]->GetFieldValue("type") << "::ToStream(outputStream);" << std::endl;
        }
    }
    outputStream << "    outputStream" << std::endl;
    for (index = 0; index < Members.size(); index++) {
        outputStream << "        << \"  " << Members[index]->GetFieldValue("description") << ":\" << this->" << Members[index]->GetFieldValue("name") << "Member";
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
                 << "void " << this->GetFieldValue("name") << "::ToStreamRaw(std::ostream & outputStream, const char delimiter, bool headerOnly, const std::string & headerPrefix) const" << std::endl
                 << "{" << std::endl
                 << "    if (headerOnly) {" << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->GetFieldValue("is-data") == "true") {
            outputStream << "        " << BaseClasses[index]->GetFieldValue("type") << "::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);" << std::endl;
        }
    }
    outputStream << "        outputStream" << std::endl;
    for (index = 0; index < Members.size(); index++) {
        outputStream << "            << delimiter << headerPrefix << \"-" << Members[index]->GetFieldValue("description") << "\"";
        if (index == (Members.size() - 1)) {
            outputStream << ";";
        }
        outputStream << std::endl;
    }
    outputStream << "    } else {" << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->GetFieldValue("is-data") == "true") {
            outputStream << "    " << BaseClasses[index]->GetFieldValue("type") << "::ToStreamRaw(outputStream, delimiter);" << std::endl;
        }
    }
    outputStream << "        outputStream" << std::endl;
    for (index = 0; index < Members.size(); index++) {
        outputStream << "            << delimiter << this->" << Members[index]->GetFieldValue("name") << "Member";
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
                 << "void cmnSerializeRaw(std::ostream & outputStream, const " << this->GetFieldValue("name") << " & object);" << std::endl
                 << "void cmnDeSerializeRaw(std::istream & inputStream, " << this->GetFieldValue("name") << " & placeHolder);" << std::endl;
}


void cdgClass::GenerateDataFunctionsHeader(std::ostream & outputStream) const
{
    outputStream << "/* data functions */" << std::endl
                 << "void cmnDataCopy(" << this->GetFieldValue("name") << " & destination, const " << this->GetFieldValue("name") << " & source);" << std::endl
                 << "void cmnDataSerializeBinary(std::ostream & outputStream, const " << this->GetFieldValue("name") << " & data);" << std::endl
                 << "void cmnDataDeSerializeBinary(std::istream & inputStream, " << this->GetFieldValue("name") << " & data," << std::endl
                 << "                              const cmnDataFormat & remoteFormat, const cmnDataFormat & localFormat);"<< std::endl
                 << "bool cmnDataScalarNumberIsFixed(const " << this->GetFieldValue("name") << " & data);" << std::endl
                 << "size_t cmnDataScalarNumber(const " << this->GetFieldValue("name") << " & data);" << std::endl
                 << "std::string cmnDataScalarDescription(const " << this->GetFieldValue("name") << " & data, const size_t & index," << std::endl
                 << "                                     const char * userDescription = \"" << this->GetFieldValue("name") << "\");" << std::endl
                 << "double cmnDataScalar(const " << this->GetFieldValue("name") << " & data, const size_t & index);" << std::endl;
}


void cdgClass::GenerateStandardFunctionsCode(std::ostream & outputStream) const
{
    outputStream << "/* default functions */" << std::endl
                 << "void cmnSerializeRaw(std::ostream & outputStream, const " << this->GetFieldValue("name") << " & object)" << std::endl
                 << "{" << std::endl
                 << "    object.SerializeRaw(outputStream);" << std::endl
                 << "}" << std::endl
                 << "void cmnDeSerializeRaw(std::istream & inputStream, " << this->GetFieldValue("name") << " & placeHolder)" << std::endl
                 << "{" << std::endl
                 << "    placeHolder.DeSerializeRaw(inputStream);" << std::endl
                 << "}" << std::endl;
}


void cdgClass::GenerateDataFunctionsCode(std::ostream & outputStream) const
{
    size_t index;

    outputStream << "/* data functions */" << std::endl;

    outputStream << "void cmnDataCopy(" << this->GetFieldValue("name") << " & destination, const " << this->GetFieldValue("name") << " & source) {" << std::endl
                 << "}" << std::endl;



    outputStream << "void cmnDataSerializeBinary(std::ostream & outputStream, const " << this->GetFieldValue("name") << " & data) {" << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->GetFieldValue("is-data") == "true") {
            outputStream << "    cmnDataSerializeBinary(outputStream, *(dynamic_cast<const " << BaseClasses[index]->GetFieldValue("type") << "*>(&data)));" << std::endl;
        }
    }
    for (index = 0; index < Members.size(); index++) {
        if (Members[index]->GetFieldValue("is-data") == "true") {
            outputStream << "    cmnDataSerializeBinary(outputStream, data." << Members[index]->GetFieldValue("name") << "());" << std::endl;
        }
    }
    outputStream << "}" << std::endl;



    outputStream << "void cmnDataDeSerializeBinary(std::istream & inputStream, " << this->GetFieldValue("name") << " & data," << std::endl
                 << "                              const cmnDataFormat & remoteFormat, const cmnDataFormat & localFormat) {"<< std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->GetFieldValue("is-data") == "true") {
            outputStream << "    cmnDataDeSerializeBinary(inputStream, *(dynamic_cast<" << BaseClasses[index]->GetFieldValue("type") << "*>(&data)), remoteFormat, localFormat);" << std::endl;
        }
    }
    for (index = 0; index < Members.size(); index++) {
        if (Members[index]->GetFieldValue("is-data") == "true") {
            outputStream << "    cmnDataDeSerializeBinary(inputStream, data." << Members[index]->GetFieldValue("name") << "(), remoteFormat, localFormat);" << std::endl;
        }
    }
    outputStream << "}" << std::endl;



    outputStream << "bool cmnDataScalarNumberIsFixed(const " << this->GetFieldValue("name") << " & data) {" << std::endl
                 << "    return true" << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->GetFieldValue("is-data") == "true") {
            outputStream << "           && cmnDataScalarNumberIsFixed(*(dynamic_cast<const " << BaseClasses[index]->GetFieldValue("type") << "*>(&data)))" << std::endl;
        }
    }
    for (index = 0; index < Members.size(); index++) {
        if (Members[index]->GetFieldValue("is-data") == "true") {
            outputStream << "           && cmnDataScalarNumberIsFixed(data." << Members[index]->GetFieldValue("name") << "())" << std::endl;
        }
    }
    outputStream << "    ;" << std::endl
                 << "}" << std::endl;

    outputStream << "size_t cmnDataScalarNumber(const " << this->GetFieldValue("name") << " & data) {" << std::endl
                 << "    return 0" << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->GetFieldValue("is-data") == "true") {
            outputStream << "           + cmnDataScalarNumber(*(dynamic_cast<const " << BaseClasses[index]->GetFieldValue("type") << "*>(&data)))" << std::endl;
        }
    }
    for (index = 0; index < Members.size(); index++) {
        if (Members[index]->GetFieldValue("is-data") == "true") {
            outputStream << "           + cmnDataScalarNumber(data." << Members[index]->GetFieldValue("name") << "())" << std::endl;
        }
    }
    outputStream << "    ;" << std::endl
                 << "}" << std::endl;

    outputStream << "std::string cmnDataScalarDescription(const " << this->GetFieldValue("name") << " & data, const size_t & index," << std::endl
                 << "                                     const char * userDescription) {" << std::endl
                 << "    size_t baseIndex = 0;" << std::endl
                 << "    size_t currentMaxIndex = 0;" << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->GetFieldValue("is-data") == "true") {
            outputStream << "    currentMaxIndex += cmnDataScalarNumber(*(dynamic_cast<const " << BaseClasses[index]->GetFieldValue("type") << "*>(&data)));" << std::endl
                         << "    if (index < currentMaxIndex) {" << std::endl
                         << "        return cmnDataScalarDescription(*(dynamic_cast<const " << BaseClasses[index]->GetFieldValue("type") << "*>(&data)), index - baseIndex);" << std::endl
                         << "    }" << std::endl
                         << "    baseIndex = currentMaxIndex;" << std::endl;
        }
    }
    for (index = 0; index < Members.size(); index++) {
        if (Members[index]->GetFieldValue("is-data") == "true") {
            outputStream << "    currentMaxIndex += cmnDataScalarNumber(data." << Members[index]->GetFieldValue("name") << "());" << std::endl
                         << "    if (index < currentMaxIndex) {" << std::endl
                         << "        return cmnDataScalarDescription(data." << Members[index]->GetFieldValue("name") << "(), index - baseIndex, \"" << Members[index]->GetFieldValue("name") << "\");" << std::endl
                         << "    }" << std::endl
                         << "    baseIndex = currentMaxIndex;" << std::endl;
        }
    }
    outputStream << "    cmnThrow(std::out_of_range(\"cmnDataScalarDescription: " << this->GetFieldValue("name") << " index out of range\"));" << std::endl
                 << "    return \"\";" << std::endl
                 << "}" << std::endl;

    outputStream << "double cmnDataScalar(const " << this->GetFieldValue("name") << " & data, const size_t & index) {" << std::endl
                 << "    size_t baseIndex = 0;" << std::endl
                 << "    size_t currentMaxIndex = 0;" << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->GetFieldValue("is-data") == "true") {
            outputStream << "    currentMaxIndex += cmnDataScalarNumber(*(dynamic_cast<const " << BaseClasses[index]->GetFieldValue("type") << "*>(&data)));" << std::endl
                         << "    if (index < currentMaxIndex) {" << std::endl
                         << "        return cmnDataScalar(*(dynamic_cast<const " << BaseClasses[index]->GetFieldValue("type") << "*>(&data)), index - baseIndex);" << std::endl
                         << "    }" << std::endl
                         << "    baseIndex = currentMaxIndex;" << std::endl;
        }
    }
    for (index = 0; index < Members.size(); index++) {
        if (Members[index]->GetFieldValue("is-data") == "true") {
            outputStream << "    currentMaxIndex += cmnDataScalarNumber(data." << Members[index]->GetFieldValue("name") << "());" << std::endl
                         << "    if (index < currentMaxIndex) {" << std::endl
                         << "        return cmnDataScalar(data." << Members[index]->GetFieldValue("name") << "(), index - baseIndex);" << std::endl
                         << "    }" << std::endl
                         << "    baseIndex = currentMaxIndex;" << std::endl;
        }
    }
    outputStream << "    cmnThrow(std::out_of_range(\"cmnDataScalarDescription: " << this->GetFieldValue("name") << " index out of range\"));" << std::endl
                 << "    return 1.2345;" << std::endl
                 << "}" << std::endl;
}
