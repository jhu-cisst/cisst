/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
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
//#include <cisstMultiTask/mtsTaskManager.h>

#include "mtsTaskTest.h"

#include <string>

CMN_IMPLEMENT_SERVICES(mtsTaskTestTask);

//-----------------------------------------------------------------------------
mtsTaskTestTask::mtsTaskTestTask(const std::string & collectorName, 
                                 double period) :
	mtsTaskPeriodic(collectorName, period, false, 5000)
{
}

//-----------------------------------------------------------------------------
//	Tests for public variables and methods
//-----------------------------------------------------------------------------
void mtsTaskTest::TestGetStateVectorID(void)
{
	const size_t default_column_count = 3;	// Toc, Tic, Period
	const size_t user_column_count = 2;	// Data1, Data2
	const size_t total_column_count = default_column_count + user_column_count;
	
	const std::string names[total_column_count] = { 
		// added by default
		"Toc", 
		"Tic", 
		"Period", 
		// user items
		"Data1", 
		"Data2" };	
	
	mtsTaskTestTask task("task", 10 * cmn_ms);
    mtsStateTable StateTable(20, "Test");
	
	CPPUNIT_ASSERT(default_column_count == StateTable.StateVectorDataNames.size());
	{
		StateTable.StateVectorDataNames.push_back("Data1");
		StateTable.StateVectorDataNames.push_back("Data2");
	}
	CPPUNIT_ASSERT(total_column_count == StateTable.StateVectorDataNames.size());
}

//-----------------------------------------------------------------------------
//	Tests for private variables and methods
//-----------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION(mtsTaskTest);
