/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2009-04-29
  
  (C) Copyright 2009-2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

class prmPositionJointGetTest: public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(prmPositionJointGetTest);
    {
        CPPUNIT_TEST(TestConstructors);
        CPPUNIT_TEST(TestSerialize);
        CPPUNIT_TEST(TestTimestamps);
    }
    CPPUNIT_TEST_SUITE_END();
    
public:
    void setUp(void) {}
    
    void tearDown(void) {}

    void TestConstructors(void);
    void TestSerialize(void);
    void TestTimestamps(void);
};


CPPUNIT_TEST_SUITE_REGISTRATION(prmPositionJointGetTest);

