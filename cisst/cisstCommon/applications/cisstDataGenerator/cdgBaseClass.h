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

#ifndef _cdgBaseClass_h
#define _cdgBaseClass_h

#include "cdgScope.h"

class cdgBaseClass: public cdgScope
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

    friend class cdgFile;
    friend class cdgClass;

    std::string Type;
    std::string Visibility;
    std::string IsData;
public:
    cdgBaseClass(unsigned int lineNumber);

    cdgScope::Type GetScope(void) const;
    bool HasKeyword(const std::string & keyword) const;
    bool HasScope(const std::string & keyword,
                  cdgScope::Stack & scopes,
                  unsigned int lineNumber);
    bool SetValue(const std::string & keyword, const std::string & value,
                  std::string & errorMessage);
    bool IsValid(std::string & errorMessage) const;

    void FillInDefaults(void);
    void GenerateHeader(std::ostream & outputStream) const;
    void GenerateCode(std::ostream & outputStream) const;

private:
    cdgBaseClass(void); // make sure constructor with line number is always used.
};

CMN_DECLARE_SERVICES_INSTANTIATION(cdgBaseClass);

#endif // _cdgBaseClass_h
