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

#ifndef _cdgScope_h
#define _cdgScope_h


#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassRegisterMacros.h>
#include <cisstCommon/cmnNamedMap.h>
#include <cisstCommon/cmnAssert.h>

#include "cdgValue.h"

class cdgScope: public cmnGenericObject {

public:

    enum Type {CDG_GLOBAL,
               CDG_CLASS,
               CDG_BASECLASS,
               CDG_TYPEDEF,
               CDG_MEMBER,
               CDG_CODE};

    typedef std::vector<cdgScope *> ScopesContainer;
    typedef ScopesContainer Stack; // for parser to manage nested scopes

    typedef cmnNamedMap<cdgValue> ValuesContainer;

    cdgScope(unsigned int lineNumber);

    std::string GetDescription(void) const;

    const std::string & GetScopeName(void) const;
    virtual Type GetScope(void) const = 0;

    cdgValue * AddValue(const std::string & keyword, const std::string & defaultValue, const bool required);
    bool HasKeyword(const std::string & keyword) const;
    bool SetValue(const std::string & keyword,
                  const std::string & value,
                  std::string & errorMessage);
    std::string GetValue(const std::string & keyword) const;
    bool IsValid(std::string & errorMessage) const;
    void FillInDefaults(void);

    virtual bool HasScope(const std::string & keyword,
                          Stack & scopes,
                          unsigned int lineNumber) = 0;

    virtual void GenerateHeader(std::ostream & outputStream) const = 0;
    virtual void GenerateCode(std::ostream & outputStream) const = 0;

protected:
    ValuesContainer Values;
    ScopesContainer Scopes; // list of "scopes" found in this scope
    unsigned int LineNumber;
    void GenerateLineComment(std::ostream & outputStream) const;

private:
    cdgScope(void); // make sure constructor with line number is always used.
};

#endif // _cdgScope_h
