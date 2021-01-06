/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2010-09-06

  (C) Copyright 2010-2021 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#include "cdgGlobal.h"

CMN_IMPLEMENT_SERVICES(cdgGlobal);


cdgGlobal::cdgGlobal(size_t lineNumber):
    cdgScope("global", lineNumber)
{
    this->AddKnownScope(*this);

    cdgClass newClass(0);
    this->AddSubScope(newClass);

    cdgInline newInline(0, cdgInline::CDG_INLINE_HEADER);
    this->AddSubScope(newInline);

    cdgInline newCode(0, cdgInline::CDG_INLINE_CODE);
    this->AddSubScope(newCode);
}


cdgScope::Type cdgGlobal::GetScope(void) const
{
    return cdgScope::CDG_GLOBAL;
}


cdgScope * cdgGlobal::Create(size_t lineNumber) const
{
    return new cdgGlobal(lineNumber);
}


bool cdgGlobal::Validate(std::string & CMN_UNUSED(errorMessage))
{
    return true;
}


bool cdgGlobal::SetValue(const std::string & keyword,
                         const std::string & CMN_UNUSED(value),
                         std::string & errorMessage)
{
    errorMessage = "unhandled keyword \"" + keyword + "\"";
    return false;
}


void cdgGlobal::GenerateIncludes(std::ostream & outputStream) const
{
    size_t index;
    GenerateLineComment(outputStream);
    for (index = 0; index < Scopes.size(); index++) {
        Scopes[index]->GenerateIncludes(outputStream);
        outputStream << std::endl;
    }

    // check for includes for mts-proxy
    bool needsIncludes = false;
    for (index = 0; index < Scopes.size(); index++) {
        const std::string mtsProxy = Scopes[index]->GetFieldValue("mts-proxy");
        // includes for mts proxy
        if (mtsProxy != "false") {
            needsIncludes = true;
        }
    }
    if (needsIncludes) {
        outputStream << std::endl
                     << "// for mts-proxy " << std::endl
                     << "#include <cisstCommon/cmnClassServices.h>" << std::endl
                     << "#include <cisstCommon/cmnClassRegisterMacros.h>" << std::endl
                     << "#include <cisstMultiTask/mtsGenericObjectProxy.h>" << std::endl
                     << std::endl;
    }
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
