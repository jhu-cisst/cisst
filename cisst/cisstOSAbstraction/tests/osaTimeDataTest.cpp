/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2008-01-30

  (C) Copyright 2008-2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "osaTimeDataTest.h"
#include <iomanip>
#include <cisstOSAbstraction/osaTimeData.h>

void osaTimeDataTest::TestAlgebra(void)
{
    osaTimeData temp;
    osaTimeData temp2;
    osaTimeData result, result2;
    result.SumOf(temp, temp2);
    result2 = temp + temp2;
    CPPUNIT_ASSERT_EQUAL(result2, result);

    result.SumOf(temp, temp2);
    result.Subtract(temp);
    CPPUNIT_ASSERT_EQUAL(temp2, result);

    osaTimeData t(200, 6913578);
    osaTimeData t2(100, 3456789);
    CPPUNIT_ASSERT_EQUAL(t, t2 * 2);
    CPPUNIT_ASSERT_EQUAL(t/2, t2);
}

void osaTimeDataTest::TestEqual(void)
{
    osaTimeData left = osaTimeData(100, 0, true);
    osaTimeData right = osaTimeData(100, 0, true);
    CPPUNIT_ASSERT(left == right);
}

void osaTimeDataTest::TestInequalities(void)
{
    osaTimeData before = osaTimeNow(0);
    osaTimeData after = osaTimeNow(0);
    CPPUNIT_ASSERT(before <= after);

    before.Add(after);
    CPPUNIT_ASSERT(before > after);

    osaTimeData negative(-100.0);
    osaTimeData positive(100.0);
    CPPUNIT_ASSERT(negative < positive);
    CPPUNIT_ASSERT(positive > negative);
}

void osaTimeDataTest::TestFromTo(void)
{
    osaTimeData originalTimeData(10, 234, true);
    double timeInSeconds = originalTimeData.ToSeconds();
    osaTimeData newTimeData(timeInSeconds);
    CPPUNIT_ASSERT_EQUAL(originalTimeData, newTimeData);
}

CPPUNIT_TEST_SUITE_REGISTRATION(osaTimeDataTest);
