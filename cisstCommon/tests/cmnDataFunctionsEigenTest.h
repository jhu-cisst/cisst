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

        CPPUNIT_TEST(TestTransformBinarySerializationStream);
        CPPUNIT_TEST(TestTransformTextSerializationStream);

        CPPUNIT_TEST(TestQuaternionBinarySerializationStream);
        CPPUNIT_TEST(TestQuaternionTextSerializationStream);

        CPPUNIT_TEST(TestRotation2DBinarySerializationStream);
        CPPUNIT_TEST(TestRotation2DTextSerializationStream);
    }
    CPPUNIT_TEST_SUITE_END();

 public:
    void setUp() { }

    void tearDown() { }

    void TestDynamicSizedCopy();
    void TestFixedSizedCopy();

    void TestDynamicSizedTextSerializationStream();
    void TestFixedSizedTextSerializationStream();

    void TestDynamicSizedBinarySerializationStream();
    void TestFixedSizedBinarySerializationStream();

    void TestDynamicSizedScalars();
    void TestFixedSizedScalars();

    void TestTransformBinarySerializationStream();
    void TestTransformTextSerializationStream();

    void TestQuaternionBinarySerializationStream();
    void TestQuaternionTextSerializationStream();

    void TestRotation2DTextSerializationStream();
    void TestRotation2DBinarySerializationStream();
};

CPPUNIT_TEST_SUITE_REGISTRATION(cmnDataFunctionsEigenTest);
