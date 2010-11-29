/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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

#ifndef _cdgMember_h
#define _cdgMember_h

#include "cdgScope.h"

class cdgMember: public cdgScope
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

    friend class cdgFile;
    friend class cdgClass;

    std::string Type;
    bool UsesClassTypedef;
    std::string Name;
    std::string Description;

public:
    cdgMember(void);

    cdgScope::Type GetScope(void) const;
    bool HasKeyword(const std::string & keyword) const;
    bool HasScope(const std::string & keyword,
                  cdgScope::Stack & scopes);
    bool SetValue(const std::string & keyword, const std::string & value,
                  std::string & errorMessage);
    bool IsValid(std::string & errorMessage) const;

    void GenerateHeaderDeclaration(std::ostream & output) const;
    void GenerateHeaderAccessors(std::ostream & output) const;
    void GenerateCodeAccessors(std::ostream & output, const std::string & className) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION(cdgMember);

#endif // _cdgMember_h
