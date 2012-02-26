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

#ifndef _cdgInline_h
#define _cdgInline_h

#include <iostream>
#include <vector>

#include "cdgScope.h"

/*

 */

class cdgInline: public cdgScope
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

    friend class cdgGlobal;
    friend class cdgClass;

public:
    typedef enum {CDG_INLINE_HEADER, CDG_INLINE_CODE} InlineType;
    cdgInline(unsigned int lineNumber, InlineType type);

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
protected:
    std::string Value;
    InlineType Type;
};

CMN_DECLARE_SERVICES_INSTANTIATION(cdgInline);

#endif // _cdgInline_h