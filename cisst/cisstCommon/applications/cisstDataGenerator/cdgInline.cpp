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

#include "cdgInline.h"

CMN_IMPLEMENT_SERVICES(cdgInline);


cdgInline::cdgInline(size_t lineNumber, InlineType type):
    cdgScope(lineNumber),
    Type(type)
{
    cdgField * field;
    field = this->AddField("", "", false);
    CMN_ASSERT(field);
}


cdgScope::Type cdgInline::GetScope(void) const
{
    return cdgScope::CDG_CODE;
}


bool cdgInline::HasScope(const std::string & CMN_UNUSED(keyword),
                         cdgScope::Stack & CMN_UNUSED(scopes),
                         size_t CMN_UNUSED(lineNumber))
{
    return false;
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
