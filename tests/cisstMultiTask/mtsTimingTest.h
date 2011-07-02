/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Martin Kelly
  Created on: 2011-05-13

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cisstMultiTask/mtsManagerLocal.h>

class mtsTimingTest: public CppUnit::TestFixture
{
private:
    CPPUNIT_TEST_SUITE(mtsTimingTest);
    {
        CPPUNIT_TEST(TestContinuous);
        CPPUNIT_TEST(TestPeriodic);
    }
    CPPUNIT_TEST_SUITE_END();

    mtsManagerLocal * Manager;

public:
    mtsTimingTest(void);
    ~mtsTimingTest(void);

    template <class _componentType> void TestExecution(_componentType * component);
    void TestContinuous(void);
    void TestPeriodic(void);
};

CPPUNIT_TEST_SUITE_REGISTRATION(mtsTimingTest);
