/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2014-11-18

  (C) Copyright 2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _nmrIncludesTest_h
#define _nmrIncludesTest_h

#include <cisstCommon/cmnPortability.h>

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

// this is the test, making sure the header file exists and it can be parsed
#include <cisstNumerical.h>

class nmrIncludesTest: public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(nmrIncludesTest);
    {
        CPPUNIT_TEST(TestIncludes);
    }
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp()
    {}

    void tearDown()
    {}

    void TestIncludes(void);
};

#endif // _nmrIncludesTest_h
