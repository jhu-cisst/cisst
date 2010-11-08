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

#include "cdgGlobal.h"

CMN_IMPLEMENT_SERVICES(cdgGlobal);


cdgScope::Type cdgGlobal::GetScope(void) const
{
    return cdgScope::CDG_GLOBAL;
}


bool cdgGlobal::HasKeyword(const std::string & keyword) const
{
    if ((keyword == "name")
        || (keyword == "include")) {
        return true;
    }
    return false;
}


bool cdgGlobal::HasScope(const std::string & keyword,
                         cdgScope::Stack & scopes)
{
    if (keyword == "class") {
        cdgClass * newClass = new cdgClass;
        scopes.push_back(newClass);
        Classes.push_back(newClass);
        return true;
    }
    return false;
}


bool cdgGlobal::SetValue(const std::string & keyword,
                        const std::string & value,
                        std::string & errorMessage)
{
    errorMessage.clear();
    if (keyword == "name") {
        if (!this->Name.empty()) {
            errorMessage = "name already set";
            return false;
        }
        this->Name = value;
        return true;
    }
    if (keyword == "include") {
        this->Includes.push_back(value);
        return true;
    }
    errorMessage = "unhandled keyword \"" + keyword + "\"";
    return false;
}


bool cdgGlobal::IsValid(std::string & errorMessage) const
{
    errorMessage.clear();
    bool isValid = true;
    if (this->Name.empty()) {
        isValid = false;
        errorMessage += " [no name defined] ";
    }
    return isValid;
}


void cdgGlobal::GenerateHeader(std::ostream & outputStream) const
{
    size_t index;
    outputStream << "#ifndef _" << Name << "_h" << std::endl
                 << "#define _" << Name << "_h" << std::endl
                 << "#include <cisstMultiTask/mtsGenericObject.h>" << std::endl;

    for (index = 0; index < Includes.size(); index++) {
        outputStream << "#include " << Includes[index] << std::endl;
    }
    for (index = 0; index < Classes.size(); index++) {
        Classes[index]->GenerateHeader(outputStream);
    }
    outputStream << "#endif // _" << Name << "_h" << std::endl;
}


void cdgGlobal::GenerateCode(std::ostream & outputStream,
                             const std::string & header) const
{
    size_t index;
    outputStream << "#include <" << header << ">" << std::endl
                 << "#include <cisstCommon/cmnSerializer.h>" << std::endl
                 << "#include <cisstCommon/cmnDeSerializer.h>" << std::endl;
    
    for (index = 0; index < Classes.size(); index++) {
        Classes[index]->GenerateCode(outputStream);
    }
}
