/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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

#include "cdgClass.h"

CMN_IMPLEMENT_SERVICES(cdgClass);


cdgClass::cdgClass(size_t lineNumber):
    cdgScope("class", lineNumber)
{
    CMN_ASSERT(this->AddField("name", "", true, "name of the generated C++ class"));
    CMN_ASSERT(this->AddField("attribute", "", false, "string place between 'class' and the class name (e.g. CISST_EXPORT)"));

    this->AddKnownScope(*this);

    cdgBaseClass newBaseClass(0);
    this->AddSubScope(newBaseClass);

    cdgTypedef newTypedef(0);
    this->AddSubScope(newTypedef);

    cdgMember newMember(0);
    this->AddSubScope(newMember);

    cdgInline newInline(0, cdgInline::CDG_INLINE_HEADER);
    this->AddSubScope(newInline);

    cdgInline newCode(0, cdgInline::CDG_INLINE_CODE);
    this->AddSubScope(newCode);
}


cdgScope::Type cdgClass::GetScope(void) const
{
    return cdgScope::CDG_CLASS;
}


cdgScope * cdgClass::Create(size_t lineNumber) const
{
    return new cdgClass(lineNumber);
}


bool cdgClass::Validate(void)
{
    cdgMember * memberPtr;
    cdgBaseClass * baseClassPtr;
    cdgTypedef * typedefPtr;
    const ScopesContainer::iterator end = Scopes.end();
    ScopesContainer::iterator iter;
    for (iter = Scopes.begin();
         iter != end;
         ++iter) {
        memberPtr = dynamic_cast<cdgMember *>(*iter);
        if (memberPtr) {
            Members.push_back(memberPtr);
        } else {
            baseClassPtr = dynamic_cast<cdgBaseClass *>(*iter);
            if (baseClassPtr) {
                BaseClasses.push_back(baseClassPtr);
            } else {
                typedefPtr = dynamic_cast<cdgTypedef *>(*iter);
                if (typedefPtr) {
                    Typedefs.push_back(typedefPtr);
                }
            }
        }
    }
    return true;
}


void cdgClass::GenerateHeader(std::ostream & outputStream) const
{
    GenerateLineComment(outputStream);
    const std::string className = this->GetFieldValue("name");

    size_t index;
    // for (index = 0; index < Scopes.size(); index++) {
    //     outputStream << "#include " << Includes[index] << std::endl;
    // }

    outputStream << "class " << className << ";" << std::endl;

    GenerateStandardFunctionsHeader(outputStream);
    GenerateDataFunctionsHeader(outputStream);

    outputStream << "class " << this->GetFieldValue("attribute") << " " << className;

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

    // add friendship for cmnDataCopy
    outputStream << " friend void cmnDataCopy(" << className << " & destination, const "
                 << className << " & source);" << std::endl;

    // constructors and destructor
    outputStream << " /* default constructors and destructors. */" << std::endl
                 << " public:" << std::endl
                 << "    " << className << "(void);" << std::endl
                 << "    " << className << "(const " << this->GetFieldValue("name") << " & other);" << std::endl
                 << "    ~" << className << "();" << std::endl << std::endl;

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
    std::string name = this->GetFieldValue("name");
    outputStream << "    /* default data methods */" << std::endl
                 << " public:" << std::endl
                 << "    void SerializeBinary(std::ostream & outputStream) const throw (std::runtime_error);" << std::endl
                 << "    void DeSerializeBinary(std::istream & inputStream, const cmnDataFormat & remoteFormat, const cmnDataFormat & localFormat) throw (std::runtime_error);" << std::endl
                 << "    void SerializeText(std::ostream & outputStream, const char delimiter) const throw (std::runtime_error);" << std::endl
                 << "    std::string SerializeTextDescription(const char delimiter, const std::string & userDescription = \"\") const;" << std::endl
                 << "    void DeSerializeText(std::istream & inputStream, const char delimiter) throw (std::runtime_error);" << std::endl
                 << "    bool ScalarNumberIsFixed(void) const;" << std::endl
                 << "    size_t ScalarNumber(void) const;" << std::endl
                 << "    double Scalar(const size_t index) const throw (std::out_of_range);" << std::endl
                 << "    std::string ScalarDescription(const size_t index, const std::string & userDescription = \"\") const throw (std::out_of_range);" << std::endl
                 << "#if CISST_HAS_JSON" << std::endl
                 << "    void ToJSON(Json::Value & jsonValue) const;" << std::endl
                 << "    void FromJSON(const Json::Value & jsonValue) throw (std::runtime_error);" << std::endl
                 << "#endif // CISST_HAS_JSON" << std::endl
                 << std::endl;
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
    size_t index;

    std::stringstream defaultConstructor;
    std::stringstream copyConstructor;

    // default and copy constructors
    std::string name = this->GetFieldValue("name");
    defaultConstructor << name << "::" << name << "(void)";
    copyConstructor    << name << "::" << name << "(const " << name << " & other)";
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
    std::string memberName;
    for (index = 0; index < Members.size(); index++) {
        memberName = Members[index]->GetFieldValue("name");
        defaultConstructor << "    " << memberName << "Member(" << Members[index]->GetFieldValue("default") << ")";
        copyConstructor    << "    " << memberName << "Member(other." << memberName << "Member)";
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
    outputStream << std::endl
                 << "void " << this->GetFieldValue("name") << "::ToStreamRaw(std::ostream & outputStream, const char delimiter, bool headerOnly, const std::string & headerPrefix) const" << std::endl
                 << "{" << std::endl
                 << "    if (headerOnly) {" << std::endl
                 << "        outputStream << cmnDataSerializeTextDescription(*this, delimiter, headerPrefix);" << std::endl
                 << "    } else {" << std::endl
                 << "        cmnDataSerializeText(outputStream, *this, delimiter);" << std::endl
                 << "    }" << std::endl
                 << "}" << std::endl;
}


void cdgClass::GenerateStandardFunctionsHeader(std::ostream & outputStream) const
{
    const std::string name = this->GetFieldValue("name");
    const std::string attribute = this->GetFieldValue("attribute");
    outputStream << "/* default functions */" << std::endl
                 << "void " << attribute << " cmnSerializeRaw(std::ostream & outputStream, const " << name << " & object);" << std::endl
                 << "void " << attribute << " cmnDeSerializeRaw(std::istream & inputStream, " << name << " & placeHolder);" << std::endl;
}


void cdgClass::GenerateDataFunctionsHeader(std::ostream & outputStream) const
{
    const std::string name = this->GetFieldValue("name");
    const std::string attribute = this->GetFieldValue("attribute");
    outputStream << "/* data functions */" << std::endl
                 << "void " << attribute << " cmnDataCopy(" << name << " & destination, const " << name << " & source);" << std::endl
                 << "void " << attribute << " cmnDataSerializeBinary(std::ostream & outputStream, const " << name << " & data) throw (std::runtime_error);" << std::endl
                 << "void " << attribute << " cmnDataDeSerializeBinary(std::istream & inputStream, " << name
                 << " & data, const cmnDataFormat & remoteFormat, const cmnDataFormat & localFormat) throw (std::runtime_error);"<< std::endl
                 << "void " << attribute << " cmnDataSerializeText(std::ostream & outputStream, const " << name << " & data, const char delimiter) throw (std::runtime_error);" << std::endl
                 << "std::string " << attribute << " cmnDataSerializeTextDescription(const " << name << " & data, const char delimiter, const std::string & userDescription = \"\");" << std::endl
                 << "void " << attribute << " cmnDataDeSerializeText(std::istream & inputStream, " << name << " & data, const char delimiter) throw (std::runtime_error);" << std::endl
                 << "bool " << attribute << " cmnDataScalarNumberIsFixed(const " << name << " & data);" << std::endl
                 << "size_t " << attribute << " cmnDataScalarNumber(const " << name << " & data);" << std::endl
                 << "std::string " << attribute << " cmnDataScalarDescription(const " << name
                 << " & data, const size_t index, const std::string & userDescription = \"\") throw (std::out_of_range);" << std::endl
                 << "double " << attribute << " cmnDataScalar(const " << name << " & data, const size_t index) throw (std::out_of_range);" << std::endl
                 << "#if CISST_HAS_JSON" << std::endl
                 << "#include <json/json.h>" << std::endl
                 << "void " << attribute << " cmnDataToJSON(const " << name << " & data, Json::Value & jsonValue);" << std::endl
                 << "void " << attribute << " cmnDataFromJSON(" << name << " & data, const Json::Value & jsonValue) throw (std::runtime_error);" << std::endl
                 << "#endif // CISST_HAS_JSON" << std::endl;
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
    std::string name, type, suffix, className;

    className = this->GetFieldValue("name");

    outputStream << "/* data functions */" << std::endl;

    outputStream << "void cmnDataCopy(" << className << " & destination, const " << className << " & source) {" << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->GetFieldValue("is-data") == "true") {
            type = BaseClasses[index]->GetFieldValue("type");
            outputStream << "    cmnDataCopy(*(dynamic_cast<" << type << "*>(&destination)), *(dynamic_cast<const " << type << "*>(&source)));" << std::endl;
        }
    }
    for (index = 0; index < Members.size(); index++) {
        if (Members[index]->GetFieldValue("is-data") == "true") {
            suffix = (Members[index]->GetFieldValue("is-size_t") == "true") ? "_size_t" : "";
            name = Members[index]->GetFieldValue("name");
            outputStream << "    cmnDataCopy" << suffix << "(destination." << name << "Member, source." << name << "Member);" << std::endl;
        }
    }
    outputStream << "}" << std::endl;



    outputStream << "void cmnDataSerializeBinary(std::ostream & outputStream, const " << className << " & data) throw (std::runtime_error) {" << std::endl
                 << "     data.SerializeBinary(outputStream);" << std::endl
                 << "}" << std::endl
                 << "void " << className << "::SerializeBinary(std::ostream & outputStream) const throw (std::runtime_error) {" << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->GetFieldValue("is-data") == "true") {
            type = BaseClasses[index]->GetFieldValue("type");
            outputStream << "    cmnDataSerializeBinary(outputStream, *(dynamic_cast<const " << type << "*>(this)));" << std::endl;
        }
    }
    for (index = 0; index < Members.size(); index++) {
        if (Members[index]->GetFieldValue("is-data") == "true") {
            name = Members[index]->GetFieldValue("name");
            suffix = (Members[index]->GetFieldValue("is-size_t") == "true") ? "_size_t" : "";
            outputStream << "    cmnDataSerializeBinary" << suffix << "(outputStream, this->" << name << "Member);" << std::endl;
        }
    }
    outputStream << "}" << std::endl;



    outputStream << "void cmnDataDeSerializeBinary(std::istream & inputStream, " << this->GetFieldValue("name") << " & data," << std::endl
                 << "                              const cmnDataFormat & remoteFormat, const cmnDataFormat & localFormat) throw (std::runtime_error) {"<< std::endl
                 << "    data.DeSerializeBinary(inputStream, remoteFormat, localFormat);" << std::endl
                 << "}" << std::endl
                 << "void " << className << "::DeSerializeBinary(std::istream & inputStream," << std::endl
                 << "                                            const cmnDataFormat & remoteFormat, const cmnDataFormat & localFormat) throw (std::runtime_error) {"<< std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->GetFieldValue("is-data") == "true") {
            type = BaseClasses[index]->GetFieldValue("type");
            outputStream << "    cmnDataDeSerializeBinary(inputStream, *(dynamic_cast<" << type << "*>(this)), remoteFormat, localFormat);" << std::endl;
        }
    }
    for (index = 0; index < Members.size(); index++) {
        if (Members[index]->GetFieldValue("is-data") == "true") {
            name = Members[index]->GetFieldValue("name");
            suffix = (Members[index]->GetFieldValue("is-size_t") == "true") ? "_size_t" : "";
            outputStream << "    cmnDataDeSerializeBinary" << suffix << "(inputStream, this->" << name << "Member, remoteFormat, localFormat);" << std::endl;
        }
    }
    outputStream << "}" << std::endl;



    outputStream << "void cmnDataSerializeText(std::ostream & outputStream, const " << className << " & data, const char delimiter) throw (std::runtime_error) {" << std::endl
                 << "     data.SerializeText(outputStream, delimiter);" << std::endl
                 << "}" << std::endl
                 << "void " << className << "::SerializeText(std::ostream & outputStream, const char delimiter) const throw (std::runtime_error) {" << std::endl
                 << "    bool someData = false;" << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->GetFieldValue("is-data") == "true") {
            type = BaseClasses[index]->GetFieldValue("type");
            outputStream << "    if (someData) {" << std::endl
                         << "        outputStream << delimiter;" << std::endl
                         << "    }" << std::endl
                         << "    someData = true;" << std::endl
                         << "    cmnDataSerializeText(outputStream, *(dynamic_cast<const " << type << "*>(this)), delimiter);" << std::endl;
        }
    }
    for (index = 0; index < Members.size(); index++) {
        if (Members[index]->GetFieldValue("is-data") == "true") {
            name = Members[index]->GetFieldValue("name");
            suffix = (Members[index]->GetFieldValue("is-size_t") == "true") ? "_size_t" : "";
            outputStream << "    if (someData) {" << std::endl
                         << "        outputStream << delimiter;" << std::endl
                         << "    }" << std::endl
                         << "    someData = true;" << std::endl
                         << "    " << "cmnDataSerializeText" << suffix << "(outputStream, this->" << name << "Member, delimiter);" << std::endl;
        }
    }
    outputStream << "}" << std::endl;

    outputStream << "std::string cmnDataSerializeTextDescription(const " << className << " & data, const char delimiter, const std::string & userDescription) {" << std::endl
                 << "     return data.SerializeTextDescription(delimiter, userDescription);" << std::endl
                 << "}" << std::endl
                 << "std::string " << className << "::SerializeTextDescription(const char delimiter, const std::string & userDescription) const {" << std::endl
                 << "    bool someData = false;" << std::endl
                 << "    const std::string prefix = (userDescription == \"\") ? \"\" : (userDescription + \".\");" << std::endl
                 << "    std::stringstream description;" << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->GetFieldValue("is-data") == "true") {
            type = BaseClasses[index]->GetFieldValue("type");
            outputStream << "    if (someData) {" << std::endl
                         << "        description << delimiter;" << std::endl
                         << "    }" << std::endl
                         << "    someData = true;" << std::endl
                         << "    description << cmnDataSerializeTextDescription(*(dynamic_cast<const " << type << "*>(this)), delimiter, userDescription);" << std::endl;
        }
    }
    for (index = 0; index < Members.size(); index++) {
        if (Members[index]->GetFieldValue("is-data") == "true") {
            name = Members[index]->GetFieldValue("name");
            suffix = (Members[index]->GetFieldValue("is-size_t") == "true") ? "_size_t" : "";
            outputStream << "    if (someData) {" << std::endl
                         << "        description << delimiter;" << std::endl
                         << "    }" << std::endl
                         << "    someData = true;" << std::endl
                         << "    description << cmnDataSerializeTextDescription" << suffix << "(this->" << name << "Member, delimiter, prefix + \"" << name << "\");" << std::endl;
        }
    }
    outputStream << "    return description.str();" << std::endl
                 << "}" << std::endl;

    outputStream << "void cmnDataDeSerializeText(std::istream & inputStream, " << this->GetFieldValue("name") << " & data," << std::endl
                 << "                            const char delimiter) throw (std::runtime_error) {"<< std::endl
                 << "    data.DeSerializeText(inputStream, delimiter);" << std::endl
                 << "}" << std::endl
                 << "void " << className << "::DeSerializeText(std::istream & inputStream," << std::endl
                 << "                                          const char delimiter) throw (std::runtime_error) {"<< std::endl
                 << "    bool someData = false;" << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->GetFieldValue("is-data") == "true") {
            type = BaseClasses[index]->GetFieldValue("type");
            outputStream << "    if (someData) {" << std::endl
                         << "        cmnDataDeSerializeTextDelimiter(inputStream, delimiter, \"" << className << "\");" << std::endl
                         << "    }" << std::endl
                         << "    someData = true;" << std::endl
                         << "    cmnDataDeSerializeText(inputStream, *(dynamic_cast<" << type << "*>(this)), delimiter);" << std::endl;
        }
    }
    for (index = 0; index < Members.size(); index++) {
        if (Members[index]->GetFieldValue("is-data") == "true") {
            name = Members[index]->GetFieldValue("name");
            suffix = (Members[index]->GetFieldValue("is-size_t") == "true") ? "_size_t" : "";
            outputStream << "    if (someData) {" << std::endl
                         << "        cmnDataDeSerializeTextDelimiter(inputStream, delimiter, \"" << className << "\");" << std::endl
                         << "    }" << std::endl
                         << "    someData = true;" << std::endl
                         << "    cmnDataDeSerializeText" << suffix << "(inputStream, this->" << name << "Member, delimiter);" << std::endl;
        }
    }
    outputStream << "}" << std::endl;



    outputStream << "bool cmnDataScalarNumberIsFixed(const " << className << " & data) {" << std::endl
                 << "     return data.ScalarNumberIsFixed();" << std::endl
                 << "}" << std::endl
                 << "bool " << className << "::ScalarNumberIsFixed(void) const {" << std::endl
                 << "    return true" << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->GetFieldValue("is-data") == "true") {
            type = BaseClasses[index]->GetFieldValue("type");
            outputStream << "           && cmnDataScalarNumberIsFixed(*(dynamic_cast<const " << type << "*>(this)))" << std::endl;
        }
    }
    for (index = 0; index < Members.size(); index++) {
        if (Members[index]->GetFieldValue("is-data") == "true") {
            name = Members[index]->GetFieldValue("name");
            suffix = (Members[index]->GetFieldValue("is-size_t") == "true") ? "_size_t" : "";
            outputStream << "           && cmnDataScalarNumberIsFixed" << suffix << "(this->" << name << "Member)" << std::endl;
        }
    }
    outputStream << "    ;" << std::endl
                 << "}" << std::endl;


    outputStream << "size_t cmnDataScalarNumber(const " << className << " & data) {" << std::endl
                 << "     return data.ScalarNumber();" << std::endl
                 << "}" << std::endl
                 << "size_t " << className << "::ScalarNumber(void) const {" << std::endl
                 << "    return 0" << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->GetFieldValue("is-data") == "true") {
            type = BaseClasses[index]->GetFieldValue("type");
            outputStream << "           + cmnDataScalarNumber(*(dynamic_cast<const " << type << "*>(this)))" << std::endl;
        }
    }
    for (index = 0; index < Members.size(); index++) {
        if (Members[index]->GetFieldValue("is-data") == "true") {
            name = Members[index]->GetFieldValue("name");
            suffix = (Members[index]->GetFieldValue("is-size_t") == "true") ? "_size_t" : "";
            outputStream << "           + cmnDataScalarNumber" << suffix << "(this->" << name << "Member)" << std::endl;
        }
    }
    outputStream << "    ;" << std::endl
                 << "}" << std::endl;



    outputStream << "std::string cmnDataScalarDescription(const " << className << " & data, const size_t index," << std::endl
                 << "                                     const std::string & userDescription) throw (std::out_of_range) {" << std::endl
                 << "    return data.ScalarDescription(index, userDescription);" << std::endl
                 << "}" << std::endl
                 << "std::string " << className << "::ScalarDescription(const size_t index, const std::string & userDescription) const throw (std::out_of_range) {" << std::endl
                 << "    std::string prefix = (userDescription == \"\") ? \"\" : (userDescription + \".\");" << std::endl
                 << "    size_t baseIndex = 0;" << std::endl
                 << "    size_t currentMaxIndex = 0;" << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->GetFieldValue("is-data") == "true") {
            type = BaseClasses[index]->GetFieldValue("type");
            outputStream << "    currentMaxIndex += cmnDataScalarNumber(*(dynamic_cast<const " << type << "*>(this)));" << std::endl
                         << "    if (index < currentMaxIndex) {" << std::endl
                         << "        return cmnDataScalarDescription(*(dynamic_cast<const " << type << "*>(this)), index - baseIndex, prefix);" << std::endl
                         << "    }" << std::endl
                         << "    baseIndex = currentMaxIndex;" << std::endl;
        }
    }
    for (index = 0; index < Members.size(); index++) {
        if (Members[index]->GetFieldValue("is-data") == "true") {
            name = Members[index]->GetFieldValue("name");
            suffix = (Members[index]->GetFieldValue("is-size_t") == "true") ? "_size_t" : "";
            outputStream << "    currentMaxIndex += cmnDataScalarNumber" << suffix << "(this->" << name << "Member);" << std::endl
                         << "    if (index < currentMaxIndex) {" << std::endl
                         << "        return cmnDataScalarDescription" << suffix << "(this->" << name << "Member, index - baseIndex, prefix + \"" << name << "\");" << std::endl
                         << "    }" << std::endl
                         << "    baseIndex = currentMaxIndex;" << std::endl;
        }
    }
    outputStream << "    cmnThrow(std::out_of_range(\"cmnDataScalarDescription: " << className << " index out of range\"));" << std::endl
                 << "    return \"\";" << std::endl
                 << "}" << std::endl;



    outputStream << "double cmnDataScalar(const " << className << " & data, const size_t index) throw (std::out_of_range) {" << std::endl
                 << "    return data.Scalar(index);" << std::endl
                 << "}" << std::endl
                 << "double " << className << "::Scalar(const size_t index) const throw (std::out_of_range) {" << std::endl
                 << "    size_t baseIndex = 0;" << std::endl
                 << "    size_t currentMaxIndex = 0;" << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->GetFieldValue("is-data") == "true") {
            type = BaseClasses[index]->GetFieldValue("type");
            outputStream << "    currentMaxIndex += cmnDataScalarNumber(*(dynamic_cast<const " << type << "*>(this)));" << std::endl
                         << "    if (index < currentMaxIndex) {" << std::endl
                         << "        return cmnDataScalar(*(dynamic_cast<const " << type << "*>(this)), index - baseIndex);" << std::endl
                         << "    }" << std::endl
                         << "    baseIndex = currentMaxIndex;" << std::endl;
        }
    }
    for (index = 0; index < Members.size(); index++) {
        if (Members[index]->GetFieldValue("is-data") == "true") {
            name = Members[index]->GetFieldValue("name");
            suffix = (Members[index]->GetFieldValue("is-size_t") == "true") ? "_size_t" : "";
            outputStream << "    currentMaxIndex += cmnDataScalarNumber" << suffix << "(this->" << name << "Member);" << std::endl
                         << "    if (index < currentMaxIndex) {" << std::endl
                         << "        return cmnDataScalar" << suffix << "(this->" << name << "Member, index - baseIndex);" << std::endl
                         << "    }" << std::endl
                         << "    baseIndex = currentMaxIndex;" << std::endl;
        }
    }
    outputStream << "    cmnThrow(std::out_of_range(\"cmnDataScalarDescription: " << className << " index out of range\"));" << std::endl
                 << "    return 1.2345;" << std::endl
                 << "}" << std::endl;



    outputStream << "#if CISST_HAS_JSON" << std::endl
                 << "void cmnDataToJSON(const " << className << " & data, Json::Value & jsonValue) {" << std::endl
                 << "    data.ToJSON(jsonValue);" << std::endl
                 << "}" << std::endl
                 << "void " << className << "::ToJSON(Json::Value & jsonValue) const {" << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->GetFieldValue("is-data") == "true") {
            type = BaseClasses[index]->GetFieldValue("type");
            outputStream << "    cmnDataToJSON(*(dynamic_cast<const " << type << "*>(this)), jsonValue);" << std::endl;
        }
    }
    for (index = 0; index < Members.size(); index++) {
        if (Members[index]->GetFieldValue("is-data") == "true") {
            suffix = (Members[index]->GetFieldValue("is-size_t") == "true") ? "_size_t" : "";
            name = Members[index]->GetFieldValue("name");
            outputStream << "    cmnDataToJSON" << suffix << "(this->" << name << "Member, jsonValue[\"" << name << "\"]);" << std::endl;
        }
    }
    outputStream << "}" << std::endl
                 << "void cmnDataFromJSON(" << className << " & data, const Json::Value & jsonValue) throw (std::runtime_error) {" << std::endl
                 << "    data.FromJSON(jsonValue);" << std::endl
                 << "}" << std::endl
                 << "void " << className << "::FromJSON(const Json::Value & jsonValue) throw (std::runtime_error) {" << std::endl;
    for (index = 0; index < BaseClasses.size(); index++) {
        if (BaseClasses[index]->GetFieldValue("is-data") == "true") {
            type = BaseClasses[index]->GetFieldValue("type");
            outputStream << "    cmnDataFromJSON(*(dynamic_cast<" << type << "*>(this)), jsonValue);" << std::endl;
        }
    }
    for (index = 0; index < Members.size(); index++) {
        if (Members[index]->GetFieldValue("is-data") == "true") {
            suffix = (Members[index]->GetFieldValue("is-size_t") == "true") ? "_size_t" : "";
            name = Members[index]->GetFieldValue("name");
            outputStream << "    cmnDataFromJSON" << suffix << "(this->" << name << "Member, jsonValue[\"" << name << "\"]);" << std::endl;
        }
    }
    outputStream << "}" << std::endl
                 << "#endif // CISST_HAS_JSON" << std::endl;
}
