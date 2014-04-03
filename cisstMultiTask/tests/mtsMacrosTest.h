/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2009-06-11
  
  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "mtsMacrosTestClasses.h"

class mtsMacrosTest: public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(mtsMacrosTest);
    CPPUNIT_TEST(TestMTS_DECLARE_MEMBER_AND_ACCESSORS);
    CPPUNIT_TEST(TestMTS_MULTIPLE_INHERITANCE_FROM_MTS_GENERIC);
    CPPUNIT_TEST_SUITE_END();
    
public:
    void setUp(void) {}
    
    void tearDown(void) {}
    
    /*! Test the MTS_DECLARE_MEMBER_AND_ACCESSORS */
    void TestMTS_DECLARE_MEMBER_AND_ACCESSORS(void);

    /*! Test the MTS_MULTIPLE_INHERITANCE_FROM_MTS_GENERIC */
    void TestMTS_MULTIPLE_INHERITANCE_FROM_MTS_GENERIC(void);
};


CPPUNIT_TEST_SUITE_REGISTRATION(mtsMacrosTest);

