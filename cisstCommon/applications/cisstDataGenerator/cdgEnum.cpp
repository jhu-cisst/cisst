/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2010-09-06

  (C) Copyright 2010-2024 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "cdgEnum.h"
#include "cdgEnumValue.h"

CMN_IMPLEMENT_SERVICES(cdgEnum);


cdgEnum::cdgEnum(size_t lineNumber):
    cdgScope("enum", lineNumber)
{
#if CMN_ASSERT_IS_DEFINED
    cdgField * field;
    field =
#endif
        this->AddField("name", "", true, "name of the enum, will be added to the scope");

    CMN_ASSERT(field);

#if CMN_ASSERT_IS_DEFINED
    field =
#endif
        this->AddField("description", "", false, "description of the enum");
    CMN_ASSERT(field);

#if CMN_ASSERT_IS_DEFINED
    field =
#endif
        this->AddField("prefix", "", false, "prefix used for c enum values (e.g. _ to allow values starting with a number)");
    CMN_ASSERT(field);

#if CMN_ASSERT_IS_DEFINED
    field =
#endif
        this->AddField("attribute", "", false, "string placed in front of all global methods declaration (e.g. CISST_EXPORT)");
    CMN_ASSERT(field);

    this->AddKnownScope(*this);

    cdgEnumValue newEnumValue(0);
    this->AddSubScope(newEnumValue);
}


cdgScope::Type cdgEnum::GetScope(void) const
{
    return cdgScope::CDG_ENUM;
}


cdgScope * cdgEnum::Create(size_t lineNumber) const
{
    return new cdgEnum(lineNumber);
}


bool cdgEnum::Validate(std::string & CMN_UNUSED(errorMessage))
{
    std::string dummy;
    if (this->GetFieldValue("description") == "") {
        this->SetFieldValue("description",
                            this->GetFieldValue("name"),
                            dummy);
    }
    return true;
}


void cdgEnum::GenerateHeader(std::ostream & outputStream) const
{
    GenerateLineComment(outputStream);
    size_t index;
    const std::string name = this->GetFieldValue("name");
    const std::string prefix = this->GetFieldValue("prefix");
    std::string _indent;
    std::string _prefix;
    if (this->ClassName != "") {
        outputStream << "public:" << std::endl;
        _indent = "    ";
        _prefix.append("static ");
    } else {
        _prefix = this->GetFieldValue("attribute");
        if (_prefix != "") {
            _prefix.append(" ");
        }
    }
    outputStream << _indent << "enum " << name << " {";
    for (index = 0; index < Scopes.size(); index++) {
        outputStream << prefix << Scopes[index]->GetFieldValue("name");
        if (Scopes[index]->GetFieldValue("value") != "") {
            outputStream << " = " << Scopes[index]->GetFieldValue("value");
        }
        if (index != (Scopes.size() - 1)) {
            outputStream << ", ";
        }
    }
    outputStream << " };" << std::endl
                 << _prefix << "std::string " << name << "ToString(const " << name << " & value) CISST_THROW(std::runtime_error);" << std::endl
                 << _prefix << name << " " << name << "FromString(const std::string & value) CISST_THROW(std::runtime_error);" << std::endl
                 << _prefix << "const std::vector<int> & " << name << "VectorInt(void);" << std::endl
                 << _prefix << "const std::vector<std::string> & " << name << "VectorString(void);" << std::endl;

    if (ClassName == "") {
        outputStream << "CMN_DATA_SPECIALIZATION_FOR_ENUM_USER_HUMAN_READABLE(" << name << ", int, " << name << "ToString);" << std::endl
                     << "#if CISST_HAS_JSON" << std::endl
                     << "  CMN_DECLARE_DATA_FUNCTIONS_JSON_FOR_ENUM_EXPORT(" << name << ");" << std::endl
                     << "#endif // CISST_HAS_JSON" << std::endl;
    }
}


void cdgEnum::GenerateCode(std::ostream & CMN_UNUSED(outputStream)) const
{
}


void cdgEnum::GenerateDataFunctionsHeader(std::ostream & outputStream, const std::string & attribute) const
{
    const std::string name = this->GetFieldValue("name");
    std::string scope = this->ClassName;
    if (scope != "") {
        scope.append("::");
    }
    std::string scopeUnderscore = this->ClassName;
    std::replace(scopeUnderscore.begin(), scopeUnderscore.end(), ':', '_');

    outputStream << "std::string " << attribute << " cmnDataHumanReadable_" << scopeUnderscore << "_" << name << "(const " << scope << name << " & data);" << std::endl
                 << "CMN_DATA_SPECIALIZATION_FOR_ENUM_USER_HUMAN_READABLE(" << this->ClassName << "::" << name << ", int, cmnDataHumanReadable_" << scopeUnderscore << "_" << name << ");" << std::endl
                 << "#if CISST_HAS_JSON" << std::endl
                 << "  CMN_DECLARE_DATA_FUNCTIONS_JSON_FOR_ENUM(" << scope << name << ");" << std::endl
                 << "#endif // CISST_HAS_JSON" << std::endl;
}


void cdgEnum::GenerateDataFunctionsCode(std::ostream & outputStream) const
{
    size_t index;
    const std::string name = this->GetFieldValue("name");
    const std::string prefix = this->GetFieldValue("prefix");
    std::string scope = this->ClassName;
    if (scope != "") {
        scope.append("::");
    }
    std::string scopeName = scope + name;
    std::string scopeUnderscore = this->ClassName;
    std::replace(scopeUnderscore.begin(), scopeUnderscore.end(), ':', '_');

    // human readable
    outputStream << "std::string cmnDataHumanReadable_" << scopeUnderscore << "_" << name << "(const " << scopeName << " & data) {" << std::endl
                 << "    switch (data) {" << std::endl;
    for (index = 0; index < Scopes.size(); index++) {
        outputStream << "        case " << scope << prefix << Scopes[index]->GetFieldValue("name") << ":" << std::endl
                     << "            return \"" << Scopes[index]->GetFieldValue("description") << "\";" << std::endl
                     << "            break;" << std::endl;
    }
    outputStream << "        default: return \"undefined enum " << scopeName << "\";" << std::endl
                 << "            break;" << std::endl
                 << "    }" << std::endl
                 << "}" << std::endl
                 << std::endl;

    // to string
    std::string methodName = scopeName + "ToString";
    outputStream << "std::string " << methodName << "(const " << scopeName << " & data) CISST_THROW(std::runtime_error) {" << std::endl
                 << "    switch (data) {" << std::endl;
    for (index = 0; index < Scopes.size(); index++) {
        outputStream << "        case " << scope << prefix << Scopes[index]->GetFieldValue("name") << ":" << std::endl
                     << "            return \"" << Scopes[index]->GetFieldValue("name") << "\";" << std::endl
                     << "            break;" << std::endl;
    }
    outputStream << "        default:" << std::endl
                 << "            break;" << std::endl
                 << "    }" << std::endl
                 << "    cmnThrow(\"" << methodName << " called with invalid enum\");" << std::endl
                 << "    return \"\";" << std::endl
                 << "}" << std::endl
                 << std::endl;

    // from string
    methodName = scopeName + "FromString";
    outputStream << scopeName << " " << methodName << "(const std::string & value) CISST_THROW(std::runtime_error) {" << std::endl;
    for (index = 0; index < Scopes.size(); index++) {
        outputStream << "    if (value == \"" << Scopes[index]->GetFieldValue("name") << "\") {" << std::endl
                     << "        return " << scope << prefix << Scopes[index]->GetFieldValue("name") << ";" << std::endl
                     << "    };" << std::endl;
    }
    outputStream << "    std::string message = \"" << methodName << " can't find matching enum for \" + value + \".  Options are: \";" << std::endl
                 << "    std::vector<std::string> options = " << name << "VectorString();" << std::endl
                 << "    for (std::vector<std::string>::const_iterator i = options.begin(); i != options.end(); ++i) message += *i + \" \";" << std::endl
                 << "    cmnThrow(message);" << std::endl
                 << "    return static_cast<" << scopeName << " >(0);" << std::endl
                 << "}" << std::endl
                 << std::endl;

    // to vector int
    methodName = scopeName + "VectorInt(void)";
    outputStream << "const std::vector<int> & " << methodName << " {" << std::endl
                 << "    static std::vector<int> vectorInt;" << std::endl
                 << "    if (vectorInt.empty()) {" << std::endl;
    for (index = 0; index < Scopes.size(); index++) {
        outputStream << "        vectorInt.push_back(static_cast<int>("
                     << prefix << Scopes[index]->GetFieldValue("name") << "));" << std::endl;
    }
    outputStream << "    }" << std::endl
                 << "    return vectorInt;" << std::endl
                 << "}" << std::endl
                 << std::endl;

    // to vector string
    methodName = scopeName + "VectorString(void)";
    outputStream << "const std::vector<std::string> & " << methodName << " {" << std::endl
                 << "    static std::vector<std::string> vectorString;" << std::endl
                 << "    if (vectorString.empty()) {" << std::endl;
    for (index = 0; index < Scopes.size(); index++) {
        outputStream << "        vectorString.push_back(\""
                     << Scopes[index]->GetFieldValue("name") << "\");" << std::endl;
    }
    outputStream << "    }" << std::endl
                 << "    return vectorString;" << std::endl
                 << "}" << std::endl
                 << std::endl;

    outputStream << "#if CISST_HAS_JSON" << std::endl
                 << "  CMN_IMPLEMENT_DATA_FUNCTIONS_JSON_FOR_ENUM_AS_STRING(" << scopeName << ", " << scopeName << "ToString, " << scopeName << "FromString);" << std::endl
                 << "#endif // CISST_HAS_JSON" << std::endl;
}
