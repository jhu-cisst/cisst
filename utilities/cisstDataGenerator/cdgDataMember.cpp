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

#include "cdgDataMember.h"

CMN_IMPLEMENT_SERVICES(cdgDataMember);


cdgDataMember::cdgDataMember():
    UsesClassTypedef(false),
    Description("no-description-provided")
{
}


void cdgDataMember::GenerateHeaderDeclaration(std::ostream & output) const
{
    output << "    " << Type << " " << Name << "; // " << Description << std::endl;    
}


void cdgDataMember::GenerateHeaderAccessors(std::ostream & output) const
{
    output << "    const " << Type << " & Get" << Name << "(void) const;" << std::endl
           << "    void Set" << Name << "(const " << Type << " & newValue);" << std::endl;
}


void cdgDataMember::GenerateCodeAccessors(std::ostream & output, const std::string & className) const
{
    std::string returnType;
    if (UsesClassTypedef) {
        returnType = className + "::" + Type;
    } else {
        returnType = Type;
    }
    output << std::endl
           << "const " << returnType << " & " << className << "::Get" << Name << "(void) const" << std::endl
           << "{" << std::endl
           << "    return this->" << Name << ";" << std::endl
           << "}" << std::endl
           << std::endl
           << "void " << className << "::Set" << Name << "(const " << Type << " & newValue)" << std::endl
           << "{" << std::endl
           << "    this->" << Name << " = newValue;" << std::endl
           << "}" << std::endl
           << std::endl;
}
