/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

  Author(s):  Anton Deguet
  Created on: 2010-09-06

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---

*/

#include "cdgClass.h"

void cdgClass::GenerateHeader(std::ostream & output) const
{
    size_t index;

    for (index = 0; index < Includes.size(); index++) {
        output << "#include " << Includes[index] << std::endl;
    }
    output << "class " << Name << ": public mtsGenericObject" << std::endl
           << "{" << std::endl
           << "    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, " << DefaultLogLoD << ");" << std::endl;
    {
        output << " public:" << std::endl
               << "    typedef mtsGenericObject BaseType;" << std::endl;
        for (index = 0; index < Typedefs.size(); index++) {
            Typedefs[index]->GenerateHeader(output);
        }

        for (index = 0; index < Members.size(); index++) {
            Members[index]->GenerateHeaderAccessors(output);
        }

        GenerateStandardMethodsHeader(output);

        output << " protected:" << std::endl;
        for (index = 0; index < Members.size(); index++) {
            Members[index]->GenerateHeaderDeclaration(output);
        }
    }
    output << "};" << std::endl
           << "CMN_DECLARE_SERVICES_INSTANTIATION(" << Name << ");" << std::endl;
}


void cdgClass::GenerateStandardMethodsHeader(std::ostream & output) const
{
    output << "    void SerializeRaw(std::ostream & outputStream) const;" << std::endl
           << "    void DeSerializeRaw(std::istream & inputStream);" << std::endl
           << "    void ToStream(std::ostream & outputStream) const;" << std::endl
           << "    void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' '," << std::endl
           << "                     bool headerOnly = false, const std::string & headerPrefix = \"\") const;" << std::endl;
}


void cdgClass::GenerateCode(std::ostream & output) const
{
    size_t index;
    output << "CMN_IMPLEMENT_SERVICES(" << Name << ");" << std::endl;
    GenerateSerializeRawCode(output);
    GenerateDeSerializeRawCode(output);
    GenerateToStreamCode(output);
    GenerateToStreamRawCode(output);

    for (index = 0; index < Members.size(); index++) {
        Members[index]->GenerateCodeAccessors(output, Name);
    }
}


void cdgClass::GenerateSerializeRawCode(std::ostream & output) const
{
    size_t index;
    output << std::endl
           << "void " << Name << "::SerializeRaw(std::ostream & outputStream) const" << std::endl
           << "{" << std::endl;
    for (index = 0; index < Members.size(); index++) {
        output << "    cmnSerializeRaw(outputStream, this->" << Members[index]->Name << ");" << std::endl;
    }
    output << "}" << std::endl
           << std::endl;
}


void cdgClass::GenerateDeSerializeRawCode(std::ostream & output) const
{
    size_t index;
    output << std::endl
           << "void " << Name << "::DeSerializeRaw(std::istream & inputStream)" << std::endl
           << "{" << std::endl;
    for (index = 0; index < Members.size(); index++) {
        output << "    cmnDeSerializeRaw(inputStream, this->" << Members[index]->Name << ");" << std::endl;
    }
    output << "}" << std::endl
           << std::endl;
}


void cdgClass::GenerateToStreamCode(std::ostream & output) const
{
    size_t index;
    output << std::endl
           << "void " << Name << "::ToStream(std::ostream & outputStream) const" << std::endl
           << "{" << std::endl
           << "    outputStream << \"" << Name << "\" << std::endl;" << std::endl
           << "    BaseType::ToStream(outputStream);" << std::endl
           << "    outputStream" << std::endl;
    for (index = 0; index < Members.size(); index++) {
        output << "        << \"  " << Members[index]->Description << ":\" << this->" << Members[index]->Name;
        if (index == (Members.size() - 1)) {
            output << ";";
        }
        output << std::endl;
    }
    output << "}" << std::endl;
}


void cdgClass::GenerateToStreamRawCode(std::ostream & output) const
{
    size_t index;
    output << std::endl
           << "void " << Name << "::ToStreamRaw(std::ostream & outputStream, const char delimiter, bool headerOnly, const std::string & headerPrefix) const" << std::endl
           << "{" << std::endl
           << "    if (headerOnly) {" << std::endl
           << "        BaseType::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);" << std::endl
           << "        outputStream" << std::endl;
    for (index = 0; index < Members.size(); index++) {
        output << "            << delimiter << headerPrefix << \"-" << Members[index]->Description << "\"";
        if (index == (Members.size() - 1)) {
            output << ";";
        }
        output << std::endl;
    }
    output << "    } else {" << std::endl
           << "        BaseType::ToStreamRaw(outputStream, delimiter);" << std::endl
           << "        outputStream" << std::endl;
    for (index = 0; index < Members.size(); index++) {
        output << "            << delimiter << this->" << Members[index]->Name;
        if (index == (Members.size() - 1)) {
            output << ";";
        }
        output << std::endl;
    }
    output << "    }" << std::endl
           << "}" << std::endl;
}
