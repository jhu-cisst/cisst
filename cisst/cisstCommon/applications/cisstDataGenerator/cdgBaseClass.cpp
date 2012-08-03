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

#include "cdgBaseClass.h"

CMN_IMPLEMENT_SERVICES(cdgBaseClass);


cdgBaseClass::cdgBaseClass(unsigned int lineNumber):
    cdgScope(lineNumber)
{
    cdgValue * value;
    value = this->AddValue("type", "", true);
    CMN_ASSERT(value);
    
    value = this->AddValue("visibility", "public", false);
    CMN_ASSERT(value);
    value->AddPossibleValue("public");
    value->AddPossibleValue("private");
    value->AddPossibleValue("protected");

    value = this->AddValue("is-data", "true", false);
    CMN_ASSERT(value);
    value->AddPossibleValue("true");
    value->AddPossibleValue("false");
}


cdgScope::Type cdgBaseClass::GetScope(void) const
{
    return cdgScope::CDG_BASECLASS;
}


bool cdgBaseClass::HasScope(const std::string & CMN_UNUSED(keyword),
                            cdgScope::Stack & CMN_UNUSED(scopes),
                            unsigned int CMN_UNUSED(lineNumber))
{
    return false;
}


void cdgBaseClass::GenerateHeaderInheritance(std::ostream & outputStream) const
{
    outputStream << this->GetValue("visibility") << " " << this->GetValue("type");
}


void cdgBaseClass::GenerateHeader(std::ostream & CMN_UNUSED(outputStream)) const
{
}


void cdgBaseClass::GenerateCode(std::ostream & CMN_UNUSED(outputStream)) const
{
}
