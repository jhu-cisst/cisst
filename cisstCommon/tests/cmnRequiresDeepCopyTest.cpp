/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2009-11-17
  
  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "cmnRequiresDeepCopyTest.h"

#include <cisstCommon/cmnRequiresDeepCopy.h>

void cmnRequiresDeepCopyTest::TestNativesTypes(void) {
    // use this to verify default
    CPPUNIT_ASSERT(cmnRequiresDeepCopy<std::string>());

    // verify some template specializations
    CPPUNIT_ASSERT(!cmnRequiresDeepCopy<float>());
    CPPUNIT_ASSERT(!cmnRequiresDeepCopy<double>());
    CPPUNIT_ASSERT(!cmnRequiresDeepCopy<long double>());
    CPPUNIT_ASSERT(!cmnRequiresDeepCopy<bool>());
    CPPUNIT_ASSERT(!cmnRequiresDeepCopy<char>());
    CPPUNIT_ASSERT(!cmnRequiresDeepCopy<unsigned char>());
    CPPUNIT_ASSERT(!cmnRequiresDeepCopy<short>());
    CPPUNIT_ASSERT(!cmnRequiresDeepCopy<unsigned short>());
    CPPUNIT_ASSERT(!cmnRequiresDeepCopy<int>());
    CPPUNIT_ASSERT(!cmnRequiresDeepCopy<unsigned int>());
    CPPUNIT_ASSERT(!cmnRequiresDeepCopy<long long int>());
    CPPUNIT_ASSERT(!cmnRequiresDeepCopy<unsigned long long int>());
}
