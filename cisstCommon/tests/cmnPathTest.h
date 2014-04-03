/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2005-04-24
  
  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "cisstCommon/cmnPath.h"

class cmnPathTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(cmnPathTest);
    CPPUNIT_TEST(TestAdd);
    CPPUNIT_TEST(TestFind);
    CPPUNIT_TEST(TestRemove);
    CPPUNIT_TEST_SUITE_END();
    
 public:
    void setUp(void) {
    }
    
    void tearDown(void) {
    }
    
    /*! Test the Add method */
    void TestAdd(void);

    /*! Test the Find method */
    void TestFind(void);

    /*! Test the Remove and Has methods */
    void TestRemove(void);

};


CPPUNIT_TEST_SUITE_REGISTRATION(cmnPathTest);

