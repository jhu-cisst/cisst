/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2012-07-09

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

class vctDataFunctionsTransformationsTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(vctDataFunctionsTransformationsTest);
    {
        CPPUNIT_TEST(TestFrm3DataCopy);
        CPPUNIT_TEST(TestFrm3BinarySerializationStream);
        CPPUNIT_TEST(TestFrm3Scalar);
        CPPUNIT_TEST(TestFrm4x4DataCopy);
        CPPUNIT_TEST(TestFrm4x4BinarySerializationStream);
        CPPUNIT_TEST(TestFrm4x4Scalar);

    }
    CPPUNIT_TEST_SUITE_END();

 public:
    void setUp(void) {
    }

    void tearDown(void) {
    }

    void TestFrm3DataCopy(void);
    void TestFrm3BinarySerializationStream(void);
    void TestFrm3Scalar(void);
    void TestFrm4x4DataCopy(void);
    void TestFrm4x4BinarySerializationStream(void);
    void TestFrm4x4Scalar(void);
};
