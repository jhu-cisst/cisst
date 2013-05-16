

/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: osaPerformanceCounterTest.h 3612 2012-04-24 21:19:46Z adeguet1 $

  Author(s):  Tae Soo Kim
  Created on: 2013-04-28

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

class osaPerformanceCounterTest: public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(osaPerformanceCounterTest);
    {
        CPPUNIT_TEST(TestTimer);
 //       CPPUNIT_TEST(TestEqual);
 //       CPPUNIT_TEST(TestInequalities);
 //       CPPUNIT_TEST(TestMultipleServersMultiThreads);
    }
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void) {
    }

    void tearDown(void) {
    }

	/*! Test that the timer works as intended*/
	void TestTimer(void);

};
