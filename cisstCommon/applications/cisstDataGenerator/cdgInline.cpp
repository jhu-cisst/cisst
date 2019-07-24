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

#include "cdgInline.h"

CMN_IMPLEMENT_SERVICES(cdgInline);


cdgInline::cdgInline(size_t lineNumber, InlineType type):
    cdgScope(((type == CDG_INLINE_HEADER) ? "inline-header" : "inline-code"), lineNumber),
    Type(type)
{
#if CMN_ASSERT_IS_DEFINED
    cdgField * field;
    field =
#endif
        this->AddField("", "", false,
                       (type == CDG_INLINE_HEADER)
                       ? "code that will be placed as-is in the generated header file"
                       : "code that will be placed as-is in the generated source file");
    CMN_ASSERT(field);

    this->AddKnownScope(*this);
}


cdgScope::Type cdgInline::GetScope(void) const
{
    return cdgScope::CDG_CODE;
}


cdgScope * cdgInline::Create(size_t lineNumber) const
{
    return new cdgInline(lineNumber, this->Type);
}


bool cdgInline::Validate(std::string & CMN_UNUSED(errorMessage))
{
    return true;
}


void cdgInline::GenerateHeader(std::ostream & outputStream) const
{
    if (Type == CDG_INLINE_HEADER) {
        GenerateLineComment(outputStream);
        outputStream << this->GetFieldValue("");
    }
}


void cdgInline::GenerateCode(std::ostream & outputStream) const
{
    if (Type == CDG_INLINE_CODE) {
        GenerateLineComment(outputStream);
        outputStream << this->GetFieldValue("");
    }
}
