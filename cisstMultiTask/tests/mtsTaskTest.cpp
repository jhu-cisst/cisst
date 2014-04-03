/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Min Yang Jung
  Created on: 2009-03-05
  
  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnUnits.h>
#include <cisstMultiTask/mtsStateTable.h>

#include "mtsTaskTest.h"

#include <string>

CMN_IMPLEMENT_SERVICES(mtsTaskTestTask);

mtsTaskTestTask::mtsTaskTestTask(const std::string & name, 
                                 double period) :
	mtsTaskPeriodic(name, period, false, 50)
{
}

void mtsTaskTestTask::TestGetStateVectorID(void)
{
    mtsDouble data1, data2;
	const size_t default_column_count = StateTable.GetNumberOfElements();
    CPPUNIT_ASSERT_EQUAL(default_column_count, static_cast<size_t>(4));
	const size_t user_column_count = 2;	// Data1, Data2
	const size_t total_column_count = default_column_count + user_column_count;
    StateTable.AddData(data1, "Data1");
    StateTable.AddData(data2, "Data2");
	CPPUNIT_ASSERT(total_column_count == StateTable.GetNumberOfElements());
}

void mtsTaskTest::TestGetStateVectorID(void)
{
    mtsTaskTestTask task("testingTask", 10 * cmn_ms);
    task.TestGetStateVectorID();
}

CPPUNIT_TEST_SUITE_REGISTRATION(mtsTaskTest);
