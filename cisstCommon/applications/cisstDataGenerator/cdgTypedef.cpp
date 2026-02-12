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

#include "cdgTypedef.h"

CMN_IMPLEMENT_SERVICES(cdgTypedef);


cdgTypedef::cdgTypedef(size_t lineNumber):
    cdgScope("typedef", lineNumber)
{
#if CMN_ASSERT_IS_DEFINED
    cdgField * field;
    field =
#endif
        this->AddField("name", "", true,
                       "name of the new type defined");
    CMN_ASSERT(field);

#if CMN_ASSERT_IS_DEFINED
    field =
#endif
        this->AddField("type", "", true,
                       "C/C++ type used to define the new type");
    CMN_ASSERT(field);

    this->AddKnownScope(*this);
}


cdgScope::Type cdgTypedef::GetScope(void) const
{
    return cdgScope::CDG_TYPEDEF;
}


cdgScope * cdgTypedef::Create(size_t lineNumber) const
{
    return new cdgTypedef(lineNumber);
}


bool cdgTypedef::Validate(std::string & CMN_UNUSED(errorMessage))
{
    return true;
}


void cdgTypedef::GenerateHeader(std::ostream & outputStream) const
{
    outputStream << "    typedef " << this->GetFieldValue("type") << " " << this->GetFieldValue("name") << ";" << std::endl;
}
