/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: osaTimeServerTest.cpp 3615 2012-04-25 00:46:07Z adeguet1 $

  Author(s): Tae Soo Kim
  Created on: 2013-06-01

  (C) Copyright 2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "osaDateTest.h"
#include <cisstOSAbstraction/osaDate.h>

void osaDateTest::TestPrint(void)
{
    osaDate date = osaDateNow();
    osaTimeData timeData = osaTimeNow();
    osaDate dateFromTimeData(timeData);

    std::cout << std::endl
	      << "osaDate now: "
	      << date.ToString() << " (from osaDateNow)" << std::endl
	      << "osaDate now: "
	      << dateFromTimeData.ToString() << " (from osaTimeNow)" << std::endl;

    osaTimeData timeDataFromDate;
    dateFromTimeData.To(timeDataFromDate);
    CPPUNIT_ASSERT_EQUAL(timeData, timeDataFromDate);
}

CPPUNIT_TEST_SUITE_REGISTRATION(osaDateTest);
