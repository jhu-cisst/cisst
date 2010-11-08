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

#include "cdgCode.h"

CMN_IMPLEMENT_SERVICES(cdgCode);


cdgScope::Type cdgCode::GetScope(void) const
{
    return cdgScope::CDG_CODE;
}


cdgCode::cdgCode(const Type & type):
    TypeMember(type)
{
}


bool cdgCode::HasKeyword(const std::string & CMN_UNUSED(keyword)) const
{
    return false;
}


bool cdgCode::HasScope(const std::string & CMN_UNUSED(keyword),
                       cdgScope::Stack & CMN_UNUSED(scopes))
{
    return false;
}


bool cdgCode::SetValue(const std::string & keyword,
                       const std::string & CMN_UNUSED(value),
                       std::string & errorMessage)
{
    errorMessage = "unhandled keyword \"" + keyword + "\"";
    return false;
}


bool cdgCode::IsValid(std::string & CMN_UNUSED(errorMessage)) const
{
    return true;
}


void cdgCode::GenerateHeader(std::ostream & outputStream) const
{

}
