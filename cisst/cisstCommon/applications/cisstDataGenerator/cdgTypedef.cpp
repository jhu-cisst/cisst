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

#include "cdgTypedef.h"

CMN_IMPLEMENT_SERVICES(cdgTypedef);


cdgTypedef::cdgTypedef(unsigned int lineNumber):
    cdgScope(lineNumber)
{}


cdgScope::Type cdgTypedef::GetScope(void) const
{
    return cdgScope::CDG_TYPEDEF;
}


bool cdgTypedef::HasKeyword(const std::string & keyword) const
{
    if ((keyword == "name")
        || (keyword == "type")) {
        return true;
    }
    return false;
}


bool cdgTypedef::HasScope(const std::string & CMN_UNUSED(keyword),
                          cdgScope::Stack & CMN_UNUSED(scopes),
                          unsigned int CMN_UNUSED(lineNumber))
{
    return false;
}


bool cdgTypedef::SetValue(const std::string & keyword,
                          const std::string & value,
                          std::string & errorMessage)
{
    if (keyword == "name") {
        if (!this->Name.empty()) {
            errorMessage = "name already set";
            return false;
        }
        this->Name = value;
        return true;
    }
    if (keyword == "type") {
        if (!this->Type.empty()) {
            errorMessage = "type already set";
            return false;
        }
        this->Type = value;
        return true;
    }
    errorMessage = "unhandled keyword \"" + keyword + "\"";
    return false;
}


bool cdgTypedef::IsValid(std::string & errorMessage) const
{
    bool isValid = true;
    if (this->Name.empty()) {
        isValid = false;
        errorMessage += " [no name defined] ";
    }
    if (this->Type.empty()) {
        isValid = false;
        errorMessage += " [no type defined] ";
    }
    return isValid;
}


void cdgTypedef::GenerateHeader(std::ostream & outputStream) const
{
    outputStream << "    typedef " << Type << " " << Name << ";" << std::endl;
}
