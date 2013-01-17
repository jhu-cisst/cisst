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


cdgBaseClass::cdgBaseClass(size_t lineNumber):
    cdgScope(lineNumber)
{
    cdgField * field;
    field = this->AddField("type", "", true);
    CMN_ASSERT(field);
    
    field = this->AddField("visibility", "public", false);
    CMN_ASSERT(field);
    field->AddPossibleValue("public");
    field->AddPossibleValue("private");
    field->AddPossibleValue("protected");

    field = this->AddField("is-data", "true", false);
    CMN_ASSERT(field);
    field->AddPossibleValue("true");
    field->AddPossibleValue("false");
}


cdgScope::Type cdgBaseClass::GetScope(void) const
{
    return cdgScope::CDG_BASECLASS;
}


bool cdgBaseClass::HasScope(const std::string & CMN_UNUSED(keyword),
                            cdgScope::Stack & CMN_UNUSED(scopes),
                            size_t CMN_UNUSED(lineNumber))
{
    return false;
}


void cdgBaseClass::GenerateHeaderInheritance(std::ostream & outputStream) const
{
    outputStream << this->GetFieldValue("visibility") << " " << this->GetFieldValue("type");
}


void cdgBaseClass::GenerateHeader(std::ostream & CMN_UNUSED(outputStream)) const
{
}


void cdgBaseClass::GenerateCode(std::ostream & CMN_UNUSED(outputStream)) const
{
}
