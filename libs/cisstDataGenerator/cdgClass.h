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

#ifndef _cdgClass_h
#define _cdgClass_h

#include <iostream>
#include <vector>

#include "cdgScope.h"
#include "cdgMember.h"
#include "cdgTypedef.h"
#include "cdgCode.h"

/*

 */

class cdgClass: public cdgScope
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

    /*! Name of the data type to be created. */
    std::string Name;

    /*! Default Log LoD for the class generated.  See also
      cmnGenericObject.  Corresponds to "default-log-lod" in cisst
      data description file.  If not found, the default is
      CMN_LOG_LOD_RUN_ERROR. */ 
    std::string DefaultLogLoD;

    /*! List of header files to include.  Corresponds to keyword
      "include" in cisst data description file. */
    typedef std::vector<std::string> IncludesType;
    IncludesType Includes;

    /*! List of typedefs.  Corresponds to keyword "typedef" in cisst
      data description file.  Typedef are reproduced in the
      generated code and can be used to defined types with spaced in
      their names (e.g "unsigned int", "std::vector<std::vector<int>
      >"). */
    //@{
    typedef std::vector<cdgTypedef *> TypedefsType;
    TypedefsType Typedefs;
    //@}

    typedef std::vector<cdgCode *> CodesType;
    CodesType Codes;

    /*! List of data members.  Corresponds to keyword "member" in
      cisst data description file. */
    typedef std::vector<cdgMember *> MembersType;
    MembersType Members;

public:

    cdgScope::Type GetScope(void) const;
    bool HasKeyword(const std::string & keyword) const;
    bool HasScope(const std::string & keyword,
                  cdgScope::Stack & scopes);
    bool SetValue(const std::string & keyword, const std::string & value,
                  std::string & errorMessage);
    bool IsValid(std::string & errorMessage) const;

    void GenerateHeader(std::ostream & output) const;
    void GenerateCode(std::ostream & output) const;

protected:

    void GenerateStandardMethodsHeader(std::ostream & output) const;

    void GenerateSerializeRawCode(std::ostream & output) const;

    void GenerateDeSerializeRawCode(std::ostream & output) const;

    void GenerateToStreamCode(std::ostream & output) const;

    void GenerateToStreamRawCode(std::ostream & output) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION(cdgClass);

#endif // _cdgClass_h
