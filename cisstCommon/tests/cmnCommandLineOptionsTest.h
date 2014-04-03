/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2012-08-28

  (C) Copyright 2012-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "cisstCommon/cmnCommandLineOptions.h"

class cmnCommandLineOptionsTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(cmnCommandLineOptionsTest);
    {
        CPPUNIT_TEST(TestNoOption);
        CPPUNIT_TEST(TestNoParameter);
        CPPUNIT_TEST(TestOneRequiredString);
        CPPUNIT_TEST(TestOneRequiredStringFail);
        CPPUNIT_TEST(TestOneRequiredStringIntDouble);
        CPPUNIT_TEST(TestRepeatedOptions);
        CPPUNIT_TEST(TestIsSet);
        CPPUNIT_TEST(TestMultipleValues);
    }
    CPPUNIT_TEST_SUITE_END();

 public:
    void setUp(void) {
    }

    void tearDown(void) {
    }

    void TestNoOption(void);
    void TestNoParameter(void);
    void TestOneRequiredString(void);
    void TestOneRequiredStringFail(void);
    void TestOneRequiredStringIntDouble(void);
    void TestRepeatedOptions(void);
    void TestIsSet(void);
    void TestMultipleValues(void);
};


CPPUNIT_TEST_SUITE_REGISTRATION(cmnCommandLineOptionsTest);

