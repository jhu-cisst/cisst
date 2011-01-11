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

#ifndef _cdgScope_h
#define _cdgScope_h


#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassRegisterMacros.h>

class cdgScope: public cmnGenericObject {

public:
    
    enum Type {CDG_GLOBAL,
               CDG_CLASS,
               CDG_TYPEDEF,
               CDG_MEMBER,
               CDG_CODE};

    typedef std::list<cdgScope *> Stack;

    const std::string & GetScopeName(void) const;
    virtual Type GetScope(void) const = 0;
    virtual bool HasKeyword(const std::string & keyword) const = 0;
    virtual bool HasScope(const std::string & keyword,
                          Stack & scopes) = 0;
    virtual bool SetValue(const std::string & keyword,
                          const std::string & value,
                          std::string & errorMessage) = 0;
    virtual bool IsValid(std::string & errorMessage) const = 0;
};

#endif // _cdgScope_h
