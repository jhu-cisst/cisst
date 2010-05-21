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

//#include <cisstCommon/cmnUnits.h>
#include <cisstMultiTask/mtsStateTable.h>
//#include <cisstMultiTask/mtsTaskManager.h>

#include "mtsStateTableTest.h"

#include <string>

/*
CMN_IMPLEMENT_SERVICES(mtsTaskTestTask);

//-----------------------------------------------------------------------------
mtsTaskTestTask::mtsTaskTestTask(const std::string & collectorName, 
										   double period) :
	mtsTaskPeriodic(collectorName, period, false, 5000)
{
}

void mtsTaskTestTask::AddDataToStateTable(const std::string & dataName)
{ 
	TestData.AddToStateTable(StateTable, dataName); 
}

//-----------------------------------------------------------------------------
//	Tests for public variables and methods
//-----------------------------------------------------------------------------
void mtsTaskTest::TestGetCollectorCount(void)
{
	mtsCollector a("collector-1", 10 * cmn_ms);
	CPPUNIT_ASSERT_EQUAL((unsigned int) 1, mtsCollector::GetCollectorCount());

	mtsCollector b("collector-2", 10 * cmn_ms);
    CPPUNIT_ASSERT_EQUAL((unsigned int) 2, mtsCollector::GetCollectorCount());    
}

void mtsTaskTest::TestAddSignal(void)
{	
	const std::string taskName = "Task_TestAddSignal";
	const std::string signalName = "Data_TestAddSignal";	

	mtsCollector collector("collector", 10 * cmn_ms);
	mtsTaskTestTask TaskA(taskName, 10 * cmn_ms );

	mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
	CPPUNIT_ASSERT(taskManager);

	// In case of a task that are not under the control of the manager	
	CPPUNIT_ASSERT(!collector.AddSignal(taskName, "", ""));

	// Put it under the control of the manager.	
	CPPUNIT_ASSERT(taskManager->AddTask(&TaskA));

	//TaskA.AddDataToStateTable(signalName);
	CPPUNIT_ASSERT(collector.AddSignal(taskName, signalName, ""));
	
	// Prevent duplicate signal registration
	CPPUNIT_ASSERT(!collector.AddSignal(taskName, signalName, ""));

	// Remove a task not to cause potential side-effect when using mtsTaskManager
	//taskManager->
		
	// Throw an exception if already collecting
	//
	// TODO: IMPLEMENT ME~~~~!!!!!
	//
}

void mtsTaskTest::TestRemoveSignal(void)
{
	const std::string taskName = "Task_TestRemoveSignal";
	const std::string signalName = "Data_TestRemoveSignal";	

	mtsCollector collector("collector", 10 * cmn_ms);
	mtsTaskTestTask TaskA(taskName, 10 * cmn_ms );

	// 1. Try removing a signal from the empty signal list
	//
	//	TODO: need a method such as mtsTaskManager::RemoveTask() to test the following code.
	//
	//CPPUNIT_ASSERT(!collector.RemoveSignal(taskName, signalName));

	// Add a signal
	mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
	CPPUNIT_ASSERT(taskManager);
	CPPUNIT_ASSERT(taskManager->AddTask(&TaskA));
	//TaskA.AddDataToStateTable(signalName);
	CPPUNIT_ASSERT(collector.AddSignal(taskName, signalName, ""));

	// 2. Try removing a signal with incorrect task name
	CPPUNIT_ASSERT(!collector.RemoveSignal(taskName + "1234", signalName));

	// 3. Try removing a signal with incorrect signal name
	CPPUNIT_ASSERT(!collector.RemoveSignal(taskName, signalName + "1234"));

	// 4. Try removing a signal in a correct way
	CPPUNIT_ASSERT(collector.RemoveSignal(taskName, signalName));
}

void mtsTaskTest::TestFindSignal(void)
{
	const std::string taskName = "Task_TestFindSignal";
	const std::string signalName = "Data_TestFindSignal";
	
	mtsCollector collector("collector", 10 * cmn_ms);
	mtsTaskTestTask TaskA(taskName, 10 * cmn_ms );
	
	// return false if the signal list is empty
	CPPUNIT_ASSERT(!collector.FindSignal(taskName, signalName));
	
	// Add a signal
	mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
	CPPUNIT_ASSERT(taskManager);
	CPPUNIT_ASSERT(taskManager->AddTask(&TaskA));
	//TaskA.AddDataToStateTable(signalName);
	CPPUNIT_ASSERT(collector.AddSignal(taskName, signalName, ""));
	
	// return false if finding a nonregistered task or signal
	CPPUNIT_ASSERT(!collector.FindSignal(taskName, "nonregistered_signal"));
	CPPUNIT_ASSERT(!collector.FindSignal("nonregistered_task", signalName));
	
	// return true if finding a correct one
	CPPUNIT_ASSERT(collector.FindSignal(taskName, signalName));
	
}

void mtsTaskTest::TestGetSignalCount(void)
{
	CPPUNIT_ASSERT_EQUAL((unsigned int) 0, mtsCollector::GetCollectorCount());
	
	mtsCollector collector1("collector1", 10 * cmn_ms);
	CPPUNIT_ASSERT_EQUAL((unsigned int) 1, mtsCollector::GetCollectorCount());
	
	mtsCollector collector2("collector2", 10 * cmn_ms);
	CPPUNIT_ASSERT_EQUAL((unsigned int) 2, mtsCollector::GetCollectorCount());
}
*/

//-----------------------------------------------------------------------------
//	Tests for private variables and methods
//-----------------------------------------------------------------------------
void mtsStateTableTest::TestGetStateVectorID(void)
{
    mtsStateTable StateTable(20, "Test");

    const int default_column_count = mtsStateTable::StateVectorBaseIDForUser;
	const int user_column_count = 2;	// Data1, Data2
	const int total_column_count = default_column_count + user_column_count;
	
	const std::string names[10] = {
		// added by default
		"Toc", 
		"Tic", 
		"Period", 
        "PeriodStatistics",
		// user items
		"Data1", 
		"Data2" };	
	
	CPPUNIT_ASSERT_EQUAL(default_column_count, (int) StateTable.StateVectorDataNames.size());
	{
		StateTable.StateVectorDataNames.push_back(names[4]);
		StateTable.StateVectorDataNames.push_back(names[5]);
	}
	CPPUNIT_ASSERT_EQUAL(total_column_count, (int) StateTable.StateVectorDataNames.size());

    for (int i = 0; i < total_column_count; ++i) {
        CPPUNIT_ASSERT(i == StateTable.GetStateVectorID(names[i]));
    }
}

CPPUNIT_TEST_SUITE_REGISTRATION(mtsStateTableTest);
