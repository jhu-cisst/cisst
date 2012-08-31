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

#include "cdgGlobal.h"

CMN_IMPLEMENT_SERVICES(cdgGlobal);


cdgGlobal::cdgGlobal(unsigned int lineNumber):
    cdgScope(lineNumber)
{
}


cdgScope::Type cdgGlobal::GetScope(void) const
{
    return cdgScope::CDG_GLOBAL;
}


bool cdgGlobal::HasScope(const std::string & keyword,
                         cdgScope::Stack & scopes,
                         unsigned int lineNumber)
{
    if (keyword == "class") {
        cdgClass * newClass = new cdgClass(lineNumber);
        scopes.push_back(newClass);
        Scopes.push_back(newClass);
        Classes.push_back(newClass);
        return true;
    } else if (keyword == "inline-header") {
        cdgInline * newCode = new cdgInline(lineNumber, cdgInline::CDG_INLINE_HEADER);
        scopes.push_back(newCode);
        Scopes.push_back(newCode);
        return true;
    } else if (keyword == "inline-code") {
        cdgInline * newCode = new cdgInline(lineNumber, cdgInline::CDG_INLINE_CODE);
        scopes.push_back(newCode);
        Scopes.push_back(newCode);
        return true;
    }
    return false;
}


bool cdgGlobal::SetValue(const std::string & keyword,
                         const std::string & CMN_UNUSED(value),
                         std::string & errorMessage)
{
    errorMessage = "unhandled keyword \"" + keyword + "\"";
    return false;
}


void cdgGlobal::GenerateHeader(std::ostream & outputStream) const
{
    size_t index;
    GenerateLineComment(outputStream);
    for (index = 0; index < Scopes.size(); index++) {
        Scopes[index]->GenerateHeader(outputStream);
        outputStream << std::endl;
    }
}


void cdgGlobal::GenerateCode(std::ostream & outputStream) const
{
    size_t index;
    GenerateLineComment(outputStream);
    for (index = 0; index < Scopes.size(); index++) {
        Scopes[index]->GenerateCode(outputStream);
        outputStream << std::endl;
    }
}
