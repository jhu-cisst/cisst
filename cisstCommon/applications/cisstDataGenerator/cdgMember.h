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

#ifndef _cdgMember_h
#define _cdgMember_h

#include "cdgScope.h"

class cdgMember: public cdgScope
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

    friend class cdgFile;
    friend class cdgClass;

    bool UsesClassTypedef;
    std::string ClassName;

    // for C array types, e.g.  double data[10]
    bool IsCArray;
    std::string CArraySize; // [10]
    std::string CArrayType; // double
    size_t CArrayDimension;

public:
    cdgMember(size_t lineNumber);
    cdgScope::Type GetScope(void) const;
    cdgScope * Create(size_t lineNumber) const;
    bool Validate(std::string & errorMessage);
    void GenerateHeader(std::ostream & outputStream) const;
    void GenerateCode(std::ostream & outputStream) const;

    std::string MemberName; // name used internally

private:
    cdgMember(void); // make sure constructor with line number is always used.
};

CMN_DECLARE_SERVICES_INSTANTIATION(cdgMember);

#endif // _cdgMember_h
