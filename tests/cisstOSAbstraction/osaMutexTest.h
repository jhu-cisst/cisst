/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: osaMutexTest.h,v 1.1 2008/01/30 22:46:06 anton Exp $
  
  Author(s):  Anton Deguet
  Created on: 2008-01-30
  
  (C) Copyright 2007-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstOSAbstraction/osaMutex.h>

class osaMutexTest: public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(osaMutexTest);
    CPPUNIT_TEST(TestMutexInternalsSize);
    CPPUNIT_TEST(TestLockBlocks);
    CPPUNIT_TEST_SUITE_END();
    
 public:
    void setUp(void) {
    }
    
    void tearDown(void) {
    }
    
    /*! Test that structure for internal size is large enough */
    void TestMutexInternalsSize(void);

    /*! Check that locks do block */
    void TestLockBlocks(void);
};


