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

#ifndef _cdgClass_h
#define _cdgClass_h

#include <iostream>
#include <vector>

#include "cdgScope.h"
#include "cdgBaseClass.h"
#include "cdgMember.h"
#include "cdgTypedef.h"
#include "cdgEnum.h"

/*

 */

class cdgClass: public cdgScope
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

    /*! List of header files to include.  Corresponds to keyword
      "include" in cisst data description file. */
    //    typedef std::vector<std::string> IncludesType;
    //    IncludesType Includes;

    /*! List of typedefs.  Corresponds to keyword "typedef" in cisst
      data description file.  Typedef are reproduced in the
      generated code and can be used to defined types with spaced in
      their names (e.g "unsigned int", "std::vector<std::vector<int>
      >"). */
    //@{
    typedef std::vector<cdgTypedef *> TypedefsType;
    TypedefsType Typedefs;
    //@}

    /*! List of data members.  Corresponds to keyword "member" in
      cisst data description file. */
    typedef std::vector<cdgMember *> MembersType;
    MembersType Members;

    typedef std::vector<cdgBaseClass *> BaseClassesType;
    BaseClassesType BaseClasses;

    typedef std::vector<cdgEnum *> EnumsType;
    EnumsType Enums;

public:
    cdgClass(size_t lineNumber);
    cdgScope::Type GetScope(void) const;
    cdgScope * Create(size_t lineNumber) const;
    bool Validate(std::string & errorMessage);
    void GenerateHeader(std::ostream & outputStream) const;
    void GenerateCode(std::ostream & outputStream) const;

protected:

    void GenerateStandardMethodsHeader(std::ostream & outputStream) const;
    void GenerateConstructorsCode(std::ostream & outputStream) const;
    void GenerateMethodSerializeRawCode(std::ostream & outputStream) const;
    void GenerateMethodDeSerializeRawCode(std::ostream & outputStream) const;
    void GenerateMethodToStreamCode(std::ostream & outputStream) const;
    void GenerateMethodToStreamRawCode(std::ostream & outputStream) const;
    void GenerateStandardFunctionsHeader(std::ostream & outputStream) const;
    void GenerateStandardFunctionsCode(std::ostream & outputStream) const;

    void GenerateDataMethodsHeader(std::ostream & outputStream) const;
    void GenerateDataFunctionsHeader(std::ostream & outputStream) const;
    void GenerateDataFunctionsCode(std::ostream & outputStream) const;

    std::string ClassWithNamespace(void) const;
    std::string SkipIfEmpty(const std::string & code) const;
    std::string CMN_UNUSED_wrapped(const std::string & parameter) const;

private:
    cdgClass(void); // make sure constructor with line number is always used.
};

CMN_DECLARE_SERVICES_INSTANTIATION(cdgClass);

#endif // _cdgClass_h
