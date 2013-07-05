

/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: osaTimeServerTest.cpp 3615 2012-04-25 00:46:07Z adeguet1 $

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

#include "osaDateTest.h"
#include <cisstOSAbstraction/osaDate.h>



void osaDateTest::TestPrint(void)
{
    osaDate date = osaDateNow();
    osaTimeData now = osaTimeNow(0);
    osaDate date2(now);
    std::cout<<"\nosaDate time stamp of current time :";
    std::cout<<date.ToString()<< std::endl;
    std::cout<<"osaDate time stamp of current time (created from osaTimeData) :";
    std::cout<<date2.ToString()<< std::endl;

    osaTimeData shouldBeSame;
    date2.To(shouldBeSame);
	CPPUNIT_ASSERT(now == shouldBeSame);
    
}

CPPUNIT_TEST_SUITE_REGISTRATION(osaDateTest);
