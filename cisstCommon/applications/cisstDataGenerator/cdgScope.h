/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2010-09-06

  (C) Copyright 2010-2013 Johns Hopkins University (JHU), All Rights
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

#include "cdgField.h"

class cdgScope: public cmnGenericObject {

public:

    enum Type {CDG_GLOBAL,
               CDG_CLASS,
               CDG_BASECLASS,
               CDG_TYPEDEF,
               CDG_MEMBER,
               CDG_ENUM,
               CDG_ENUMVALUE,
               CDG_CODE};

    typedef std::vector<cdgScope *> ScopesContainer;
    typedef ScopesContainer Stack; // for parser to manage nested scopes

    typedef cmnNamedMap<cdgField> FieldsContainer;

    // To store the possible scopes and subscopes
    typedef cmnNamedMap<cdgScope> KnownScopesContainer;
    typedef std::multimap<std::string, std::string> SubScopesContainer;

    cdgScope(const std::string & name, size_t lineNumber);

    const std::string & GetScopeName(void) const;
    virtual Type GetScope(void) const = 0;

    // Add a field to that scope
    cdgField * AddField(const std::string & fieldName, const std::string & defaultValue, const bool required, const std::string & description);
    bool HasField(const std::string & fieldName) const;
    bool SetFieldValue(const std::string & fieldName,
                       const std::string & value,
                       std::string & errorMessage,
                       const bool & allowOverwrite = false);
    std::string GetFieldValue(const std::string & fieldName) const;
    bool IsValid(std::string & errorMessage) const;
    void FillInDefaults(void);

    bool AddKnownScope(const cdgScope & newScope);
    bool AddSubScope(const cdgScope & subScope);
    bool HasSubScope(const std::string & fieldName,
                     Stack & scopes,
                     size_t lineNumber);

    virtual bool ValidateRecursion(std::string & errorMessage);
    virtual bool Validate(std::string & errorMessage) = 0;

    virtual void GenerateIncludes(std::ostream & CMN_UNUSED(outputStream)) const {};
    virtual void GenerateHeader(std::ostream & outputStream) const = 0;
    virtual void GenerateCode(std::ostream & outputStream) const = 0;
    void DisplaySyntax(std::ostream & outputStream, size_t offset, bool recursive, bool skipScopeName = false) const;
    void DisplayFieldsSyntax(std::ostream & outputStream, size_t offset) const;

    // Dynamic creation of a new scope, should return an object of
    // "this" type.
    virtual cdgScope * Create(size_t lineNumber) const = 0;

protected:
    // List of possible fields and value
    FieldsContainer Fields;

    // List all possible scopes
    static KnownScopesContainer KnownScopes;
    // List of possible scopes for each scope
    static SubScopesContainer SubScopes;

    // List actual sub scopes found
    ScopesContainer Scopes; // list of "scopes" found in this scope

    // Line number in description file for beginning of scope
    size_t LineNumber;
    void GenerateLineComment(std::ostream & outputStream) const;

    enum {DISPLAY_OFFSET = 2};

    std::string Name;

private:
    cdgScope(void); // make sure constructor with line number is always used.
};

#endif // _cdgScope_h
