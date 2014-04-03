/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2003-08-09

  (C) Copyright 2003-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstCommon/cmnPortability.h>


class cmnPortabilityTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(cmnPortabilityTest);
    {
        CPPUNIT_TEST(Print);
        CPPUNIT_TEST(TestUndefined);
        CPPUNIT_TEST(TestWithCMake);
        CPPUNIT_TEST(TestCMN_ISNAN);
        CPPUNIT_TEST(TestCMN_ISFINITE);
        CPPUNIT_TEST(TestDataModel);
    }
    CPPUNIT_TEST_SUITE_END();

    static double Zero;

 public:
    void setUp(void) {
    }

    void tearDown(void) {
    }

    /*! Print the information, not actual test */
    void Print(void);

    /*! Test if any of CISST_COMPILER or CISST_OS is undefined */
    void TestUndefined(void);

    /*! Compare with the information from CMake */
    void TestWithCMake(void);

    /*! Test the NaN macro CMN_ISNAN */
    void TestCMN_ISNAN(void);

    /*! Test the NaN macro CMN_ISINF */
    void TestCMN_ISFINITE(void);

    /*! Test data model, i.e. size of int, long, long long, pointers, size_t, ... */
    void TestDataModel(void);
};


CPPUNIT_TEST_SUITE_REGISTRATION(cmnPortabilityTest);

