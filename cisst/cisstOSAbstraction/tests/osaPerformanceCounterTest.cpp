/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Tae Soo Kim
  Created on: 2013-04-28

  (C) Copyright 2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "osaPerformanceCounterTest.h"
#include <cisstOSAbstraction/osaPerformanceCounter.h>
#include <unistd.h>

void osaPerformanceCounterTest::TestTimer(void)
{
    osaPerformanceCounter pf;
    pf.Start();
    pf.Delay(2.00000000); //sleep for 2 seconds
    pf.Stop();
    osaTimeData diff = pf.GetElapsedTime();

    osaTimeData acceptableRangeUpper(2,005000000,true);
    osaTimeData acceptableRangeLower(1,995000000,true);
    CPPUNIT_ASSERT(diff<=acceptableRangeUpper && diff >= acceptableRangeLower);
}

CPPUNIT_TEST_SUITE_REGISTRATION(osaPerformanceCounterTest);
