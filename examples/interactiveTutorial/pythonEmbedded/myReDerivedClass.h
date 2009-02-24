/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Anton Deguet
  Created on: 2004-10-05

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _myReDerivedClass_h
#define _myReDerivedClass_h


#include "myDerivedClass.h"

// code required for the DLL generation
#ifdef myReDerivedClass_EXPORTS
#define CISST_THIS_LIBRARY_AS_DLL
#endif
#include <cisstCommon/cmnExportMacros.h>
#undef CISST_THIS_LIBRARY_AS_DLL
// end of code for the DLL generation


class CISST_EXPORT myReDerivedClass: public myDerivedClass {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
 public:
    virtual ~myReDerivedClass() {}

    inline void CommonMethod(void) {
        std::cout << "CommonMethod from derived (+=5)" << std::endl;
        fixedVector += 5;
    }

    inline void DerivedMethod(void) {
        std::cout << "DerivedMethod (+=7)" << std::endl;
        fixedVector += 7;
    }
};

CMN_DECLARE_SERVICES_INSTANTIATION(myReDerivedClass);

#endif // _myReDerivedClass_h

