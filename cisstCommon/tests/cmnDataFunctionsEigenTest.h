/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Brendan Burkhart
  Created on: 2026-02-15

  (C) Copyright 2026 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>


class cmnDataFunctionsEigenTest: public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(cmnDataFunctionsEigenTest);
    {
        CPPUNIT_TEST(TestDynamicSizedCopy);
        CPPUNIT_TEST(TestFixedSizedCopy);

        CPPUNIT_TEST(TestDynamicSizedBinarySerializationStream);
        CPPUNIT_TEST(TestFixedSizedBinarySerializationStream);

        CPPUNIT_TEST(TestDynamicSizedTextSerializationStream);
        CPPUNIT_TEST(TestFixedSizedTextSerializationStream);

        CPPUNIT_TEST(TestDynamicSizedScalars);
        CPPUNIT_TEST(TestFixedSizedScalars);
    }
    CPPUNIT_TEST_SUITE_END();

 public:
    void setUp() { }

    void tearDown() { }

    void TestDynamicSizedCopy();
    void TestFixedSizedCopy();

    void TestDynamicSizedTextSerializationStream(void);
    void TestFixedSizedTextSerializationStream(void);

    void TestDynamicSizedBinarySerializationStream(void);
    void TestFixedSizedBinarySerializationStream(void);

    void TestDynamicSizedScalars(void);
    void TestFixedSizedScalars(void);
};

CPPUNIT_TEST_SUITE_REGISTRATION(cmnDataFunctionsEigenTest);
