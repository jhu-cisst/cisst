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

#include "cdgEnum.h"
#include "cdgEnumValue.h"

CMN_IMPLEMENT_SERVICES(cdgEnum);


cdgEnum::cdgEnum(size_t lineNumber):
    cdgScope("enum", lineNumber)
{
    CMN_ASSERT(this->AddField("name", "", true, "name of the enum, will be added to the scope"));
    CMN_ASSERT(this->AddField("description", "", false, "description of the enum"));

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
    outputStream << "public:" << std::endl
                 << "    enum " << this->GetFieldValue("name") << " {";
    for (index = 0; index < Scopes.size(); index++) {
        outputStream << Scopes[index]->GetFieldValue("name");
        if (Scopes[index]->GetFieldValue("value") != "") {
            outputStream << " = " << Scopes[index]->GetFieldValue("value");
        }
        if (index != (Scopes.size() - 1)) {
            outputStream << ", ";
        }
    }
    outputStream << " };" << std::endl;
}


void cdgEnum::GenerateCode(std::ostream & CMN_UNUSED(outputStream)) const
{
}


void cdgEnum::GenerateDataFunctionsHeader(std::ostream & outputStream, const std::string & cScope, const std::string & attribute) const
{
    const std::string name = this->GetFieldValue("name");

    outputStream << "std::string " << attribute << " cmnDataHumanReadable_" << cScope << "_" << name << "(const " << cScope << "::" << name << " & data);" << std::endl
                 << "CMN_DATA_SPECIALIZATION_FOR_ENUM_USER_HUMAN_READABLE(" << cScope << "::" << name << ", int, cmnDataHumanReadable_" << cScope << "_" << name << ");" << std::endl
                 << "#if CISST_HAS_JSON" << std::endl
                 << "  CMN_DECLARE_DATA_FUNCTIONS_JSON_FOR_ENUM(" << cScope << "::" << name << ");" << std::endl
                 << "#endif // CISST_HAS_JSON" << std::endl;
}


void cdgEnum::GenerateDataFunctionsCode(std::ostream & outputStream, const std::string & cScope) const
{
    size_t index;
    const std::string name = this->GetFieldValue("name");
    outputStream << "std::string cmnDataHumanReadable_" << cScope << "_" << name << "(const " << cScope << "::" << name << " & data) {" << std::endl
                 << "    switch (data) {" << std::endl;
    for (index = 0; index < Scopes.size(); index++) {
        outputStream << "        case " << cScope << "::" << Scopes[index]->GetFieldValue("name") << ": return \""
                     << Scopes[index]->GetFieldValue("description") << "\";" << std::endl
                     << "            break;" << std::endl;
    }
    outputStream << "        default: return \"undefined enum " << cScope << "::" << name << "\";" << std::endl
                 << "            break;" << std::endl
                 << "    }" << std::endl
                 << "}" << std::endl
                 << std::endl
                 << "#if CISST_HAS_JSON" << std::endl
                 << "  CMN_IMPLEMENT_DATA_FUNCTIONS_JSON_FOR_ENUM(" << cScope << "::" << name << ", int);" << std::endl
                 << "#endif // CISST_HAS_JSON" << std::endl;
}
