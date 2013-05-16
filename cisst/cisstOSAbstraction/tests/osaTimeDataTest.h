
/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: osaTimeServerTest.h 3612 2012-04-24 21:19:46Z adeguet1 $

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

class osaTimeDataTest: public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(osaTimeDataTest);
    {
        CPPUNIT_TEST(TestAlgebra);
        CPPUNIT_TEST(TestEqual);
        CPPUNIT_TEST(TestInequalities);
       CPPUNIT_TEST(TestFromTo);
    }
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void) {
    }

    void tearDown(void) {
    }


	/*! Test that the addition to time works as intended */
	void TestAlgebra(void);

	/*! Test that the equality check works as intended*/
	void TestEqual(void);
	/*! Test that the inequality checks work as intended*/
	void TestInequalities(void);
	/*! Test that the from/to function work as intended*/
	void TestFromTo(void);

};
