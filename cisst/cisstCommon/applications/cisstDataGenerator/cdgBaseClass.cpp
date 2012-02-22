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
}


cdgScope::Type cdgBaseClass::GetScope(void) const
{
    return cdgScope::CDG_BASECLASS;
}


bool cdgBaseClass::HasKeyword(const std::string & keyword) const
{
    if ((keyword == "type")
        || (keyword == "visibility")
        || (keyword == "is-data")) {
        return true;
    }
    return false;
}


bool cdgBaseClass::HasScope(const std::string & CMN_UNUSED(keyword),
                            cdgScope::Stack & CMN_UNUSED(scopes),
                            unsigned int CMN_UNUSED(lineNumber))
{
    return false;
}


bool cdgBaseClass::SetValue(const std::string & keyword,
                            const std::string & value,
                            std::string & errorMessage)
{
    errorMessage.clear();
    if (keyword == "type") {
        if (!this->Type.empty()) {
            errorMessage = "type already set";
            return false;
        }
        this->Type = value;
        return true;
    }
    if (keyword == "visibility") {
        if (!this->Visibility.empty()) {
            errorMessage = "visibility already set";
            return false;
        }
        if ((value == "public")
            || (value == "private")
            || (value == "protected")) {
            this->Visibility = value;
            return true;
        } else {
            errorMessage = "visibility must be \"public\", \"protected\" or \"private\", not \"" + keyword + "\"";
            return false;
        }
    }
    if (keyword == "is-data") {
        if (!this->IsData.empty()) {
            errorMessage = "is-data already set";
            return false;
        }
        if ((value == "true")
            || (value == "false")) {
            this->IsData = value;
            return true;
        } else {
            errorMessage = "is-data must be \"true\" or \"false\", not \"" + keyword + "\"";
            return false;
        }
    }
    errorMessage = "unhandled keyword \"" + keyword + "\"";
    return false;
}


bool cdgBaseClass::IsValid(std::string & errorMessage) const
{
    errorMessage.clear();
    bool isValid = true;
    if (this->Type.empty()) {
        isValid = false;
        errorMessage += " [no type defined] ";
    }
    return isValid;
}


void FillInDefaults(void)
{
}


void cdgBaseClass::GenerateHeader(std::ostream & outputStream) const
{
    outputStream << Visibility << " " << Type;
}


void cdgBaseClass::GenerateCode(std::ostream & outputStream) const
{
}
