/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2010-09-06

  (C) Copyright 2010-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "cdgEnumValue.h"

CMN_IMPLEMENT_SERVICES(cdgEnumValue);


cdgEnumValue::cdgEnumValue(size_t lineNumber):
    cdgScope("enum-value", lineNumber)
{
#if CMN_ASSERT_IS_DEFINED
    cdgField * field;
    field =
#endif
        this->AddField("name", "", true,
                       "name of the enum value (e.g. enum {NAME, ...})");
    CMN_ASSERT(field);

#if CMN_ASSERT_IS_DEFINED
    field =
#endif
        this->AddField("description", "", false,
                       "description of the enum");
    CMN_ASSERT(field);

#if CMN_ASSERT_IS_DEFINED
    field =
#endif
        this->AddField("value", "", false,
                       "value of the enum (e.g. enum {NAME = 4})");
    CMN_ASSERT(field);
    this->AddKnownScope(*this);
}


cdgScope::Type cdgEnumValue::GetScope(void) const
{
    return cdgScope::CDG_ENUMVALUE;
}


cdgScope * cdgEnumValue::Create(size_t lineNumber) const
{
    return new cdgEnumValue(lineNumber);
}


bool cdgEnumValue::Validate(std::string & CMN_UNUSED(errorMessage))
{
    std::string dummy;
    if (this->GetFieldValue("description") == "") {
        this->SetFieldValue("description",
                            this->GetFieldValue("name"),
                            dummy);
    }
    return true;
}


void cdgEnumValue::GenerateHeader(std::ostream & CMN_UNUSED(outputStream)) const
{
}


void cdgEnumValue::GenerateCode(std::ostream & CMN_UNUSED(outputStream)) const
{
}
