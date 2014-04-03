/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2013-11-01

  (C) Copyright 2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>



class cmnDataGeneratorTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(cmnDataGeneratorTest);
    {
        CPPUNIT_TEST(TestInheritance);
        CPPUNIT_TEST(TestAccessors);
        CPPUNIT_TEST(TestCopy);
        CPPUNIT_TEST(TestBinarySerialization);
        CPPUNIT_TEST(TestTextSerialization);
        CPPUNIT_TEST(TestScalars);
    }
    CPPUNIT_TEST_SUITE_END();

 public:
    void setUp(void) {
    }

    void tearDown(void) {
    }

    void TestInheritance(void);
    void TestAccessors(void);
    void TestCopy(void);
    void TestBinarySerialization(void);
    void TestTextSerialization(void);
    void TestScalars(void);
};


CPPUNIT_TEST_SUITE_REGISTRATION(cmnDataGeneratorTest);
