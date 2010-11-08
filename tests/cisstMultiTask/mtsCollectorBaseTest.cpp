/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
$Id$

Author(s):  Min Yang Jung
Created on: 2009-03-02

(C) Copyright 2008-2009 Johns Hopkins University (JHU), All Rights
Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "mtsCollectorBaseTest.h"

#include <cisstCommon/cmnUnits.h>
#include <cisstOSAbstraction/osaStopwatch.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsCollectorBase.h>

#include <string.h>

CMN_IMPLEMENT_SERVICES(mtsCollectorBaseTestTask);

//-----------------------------------------------------------------------------
mtsCollectorBaseTestTask::mtsCollectorBaseTestTask(const std::string & collectorName)
    :
    mtsTaskFromSignal(collectorName, 5000)
{
}

void mtsCollectorBaseTestTask::AddDataToStateTable(const std::string & dataName)
{ 
    TestData.AddToStateTable(StateTable, dataName); 
}

//-----------------------------------------------------------------------------
//	Tests for public variables and methods
//-----------------------------------------------------------------------------
//void mtsCollectorBaseTest::TestGetCollectorCount(void)
//{
//    mtsCollectorSample a("collector-1", 10 * cmn_ms);
//    CPPUNIT_ASSERT_EQUAL((unsigned int) 1, mtsCollectorBase::GetCollectorCount());
//
//    mtsCollectorSample * b = new mtsCollectorSample(
//        "collector-2", 10 * cmn_ms);
//    CPPUNIT_ASSERT_EQUAL((unsigned int) 2, mtsCollectorBase::GetCollectorCount());
//
//    delete b;
//    CPPUNIT_ASSERT_EQUAL((unsigned int) 1, mtsCollectorBase::GetCollectorCount());
//}
//
///*
//void mtsCollectorBaseTest::TestAddSignal(void)
//{	
//    const std::string taskName = "Task_TestAddSignal";
//    const std::string signalName = "Data_TestAddSignal";	
//
//    mtsCollectorBase collector("collector", 
//        mtsCollectorBase::COLLECTOR_TYPE_SAMPLE, 10 * cmn_ms);
//        
//    // The following object has to be created in a dynamic way so that the object can
//    // be deleted outside this unit test. (It'll be deleted at
//    // mtsCollectorBaseTest::TestAddSignalCleanUp().)
//    mtsCollectorBaseTestTask * TaskA = new mtsCollectorBaseTestTask(taskName);
//
//    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
//    CPPUNIT_ASSERT(taskManager);
//
//    // In case of a task that are not under the control of the manager
//    CPPUNIT_ASSERT(!collector.AddSignal(taskName, "", ""));
//
//    // Put it under the control of the manager.	
//    CPPUNIT_ASSERT(taskManager->AddTask(TaskA));
//    CPPUNIT_ASSERT(collector.AddSignal(taskName, signalName, ""));
//    
//    // If this collector is of COLLECTOR_TYPE_DUMP type, only one task can be registered.
//    mtsCollectorBase collector2("collector2", 
//        mtsCollectorBase::COLLECTOR_TYPE_DUMP, 10 * cmn_ms);
//        
//    mtsCollectorBaseTestTask TaskB("TaskB");
//    CPPUNIT_ASSERT(taskManager->AddTask(&TaskB));
//    {
//        CPPUNIT_ASSERT(collector2.AddSignal("TaskB", "SignalB-1"));
//        CPPUNIT_ASSERT(!collector2.AddSignal("TaskB", "SignalB-2"));
//    }
//    CPPUNIT_ASSERT(taskManager->RemoveTask(&TaskB));
//
//    // Prevent duplicate signal registration
//    // The following line will throw a mtsCollectorException.
//    CPPUNIT_ASSERT(!collector.AddSignal(taskName, signalName, ""));    
//}
//*/
//
//// This class is not inteded to test mtsCollectorBase::AddSignalCleanup() method. 
//// That is, there is no such method in mtsCollector.
//// This is for cleaning up a temporary task generated and registered at
//// mtsCollectorBaseTest::AddSignal() where mtsTaskManager::RemoveTask() cannot be called
//// because an exception is throwed.
///*
//void mtsCollectorBaseTest::TestAddSignalCleanUp(void)
//{	
//    const std::string taskName = "Task_TestAddSignal";
//    const std::string signalName = "Data_TestAddSignal";	
//
//    mtsCollectorBase collector("collector", 
//        mtsCollectorBase::COLLECTOR_TYPE_SAMPLE, 10 * cmn_ms);
//
//    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
//    CPPUNIT_ASSERT(taskManager);
//
//    mtsCollectorBaseTestTask * task = 
//        dynamic_cast<mtsCollectorBaseTestTask*>(taskManager->GetTask(taskName));
//    CPPUNIT_ASSERT(task);
//
//    // Remove a task not to cause potential side-effect during the unit-test process
//    CPPUNIT_ASSERT(taskManager->RemoveTask(task));
//
//    // Don't forget
//    delete task;
//}
//
//void mtsCollectorBaseTest::TestRemoveSignal(void)
//{
//    const std::string taskName = "Task_TestRemoveSignal";
//    const std::string signalName = "Data_TestRemoveSignal";	
//
//    mtsCollectorBase collector("collector", 
//        mtsCollectorBase::COLLECTOR_TYPE_SAMPLE, 10 * cmn_ms);
//    mtsCollectorBaseTestTask TaskA(taskName);
//
//    // 1. Try removing a signal from the empty signal list
//    CPPUNIT_ASSERT(!collector.RemoveSignal(taskName, signalName));
//
//    // Add a signal
//    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
//    CPPUNIT_ASSERT(taskManager);
//    CPPUNIT_ASSERT(taskManager->AddTask(&TaskA));	
//    CPPUNIT_ASSERT(collector.AddSignal(taskName, signalName, ""));
//
//    // 2. Try removing a signal with incorrect task name
//    CPPUNIT_ASSERT(!collector.RemoveSignal(taskName + "1234", signalName));
//
//    // 3. Try removing a signal with incorrect signal name
//    CPPUNIT_ASSERT(!collector.RemoveSignal(taskName, signalName + "1234"));
//
//    // 4. Try removing a signal in a correct way
//    CPPUNIT_ASSERT(collector.RemoveSignal(taskName, signalName));
//
//    // Remove a task not to cause potential side-effect during the unit-test process
//    CPPUNIT_ASSERT(taskManager->RemoveTask(&TaskA));	
//}
//
//void mtsCollectorBaseTest::TestFindSignal(void)
//{
//    const std::string taskName = "Task_TestFindSignal";
//    const std::string signalName = "Data_TestFindSignal";
//
//    mtsCollectorBase collector("collector", 
//        mtsCollectorBase::COLLECTOR_TYPE_SAMPLE, 10 * cmn_ms);
//    mtsCollectorBaseTestTask TaskA(taskName);
//
//    // return false if the signal list is empty
//    CPPUNIT_ASSERT(!collector.FindSignal(taskName, signalName));
//
//    // Add a signal
//    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
//    CPPUNIT_ASSERT(taskManager);
//    CPPUNIT_ASSERT(taskManager->AddTask(&TaskA));	
//    CPPUNIT_ASSERT(collector.AddSignal(taskName, signalName, ""));
//
//    // return false if finding a nonregistered task or signal
//    CPPUNIT_ASSERT(!collector.FindSignal(taskName, "nonregistered_signal"));
//    CPPUNIT_ASSERT(!collector.FindSignal("nonregistered_task", signalName));
//
//    // return true if finding a correct one
//    CPPUNIT_ASSERT(collector.FindSignal(taskName, signalName));
//
//    // Remove a task not to cause potential side-effect during the unit-test process
//    CPPUNIT_ASSERT(taskManager->RemoveTask(&TaskA));
//
//}
//*/
//
//void mtsCollectorBaseTest::TestCleanup(void)
//{
//    mtsCollectorSample collector("collector", 10 * cmn_ms);	
//
//    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
//    CPPUNIT_ASSERT(taskManager);
//
//    // Put tasks for this test under the control of the taskManager so that
//    // mtsCollectorBase::AddSignal() works correctly.
//    mtsCollectorBaseTestTask task1("task1");
//    mtsCollectorBaseTestTask task2("task2");
//    mtsCollectorBaseTestTask task3("task3");
//    CPPUNIT_ASSERT(taskManager->AddTask(&task1));
//    CPPUNIT_ASSERT(taskManager->AddTask(&task2));
//    CPPUNIT_ASSERT(taskManager->AddTask(&task3));
//    task1.StateTable.StateVectorDataNames.push_back("signal1-1");
//    task1.StateTable.StateVectorDataNames.push_back("signal1-2");
//    task2.StateTable.StateVectorDataNames.push_back("signal2");
//    task3.StateTable.StateVectorDataNames.push_back("signal3");
//    {
//        CPPUNIT_ASSERT(collector.AddSignal("task1", "signal1-1", ""));
//        CPPUNIT_ASSERT(collector.AddSignal("task1", "signal1-2", ""));
//        CPPUNIT_ASSERT(collector.AddSignal("task2", "signal2", ""));
//        CPPUNIT_ASSERT(collector.AddSignal("task3", "signal3", ""));
//
//        collector.ClearTaskMap();
//        CPPUNIT_ASSERT(collector.taskMap.size() == 0);
//    }
//    // Remove a task not to cause potential side-effect during the unit-test process
//    CPPUNIT_ASSERT(taskManager->RemoveTask(&task1));
//    CPPUNIT_ASSERT(taskManager->RemoveTask(&task2));
//    CPPUNIT_ASSERT(taskManager->RemoveTask(&task3));
//}
//
///*
//void mtsCollectorBaseTest::TestSetTimeBaseDouble(void)
//{
//    const double defaultPeriod = (double) 10 * cmn_ms;
//    const double newPeriod = (double) 20 * cmn_ms;
//
//    mtsCollectorSample collector("collector", defaultPeriod);       
//
//    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
//    CPPUNIT_ASSERT(taskManager);
//
//    mtsCollectorBaseTestTask task1("task1", defaultPeriod);
//    CPPUNIT_ASSERT(taskManager->AddTask(&task1));
//    task1.StateTable.StateVectorDataNames.push_back("signal1");
//    {    
//        // If there is no task registered to mtsTaskManager, SetTimeBase does NOP.
//        double currentPeriod = collector.SamplingInterval;
//        collector.SetTimeBase(newPeriod, true);
//        CPPUNIT_ASSERT_EQUAL(currentPeriod, collector.SamplingInterval);
//        CPPUNIT_ASSERT_EQUAL((unsigned int) 0, collector.SamplingStride);
//        CPPUNIT_ASSERT(false == collector.TimeOffsetToZero);
//
//        CPPUNIT_ASSERT(collector.AddSignal("task1", "signal1", ""));
//        CPPUNIT_ASSERT(collector.taskMap.size() == 1);
//        {
//            collector.SetTimeBase(newPeriod, true);
//            CPPUNIT_ASSERT_EQUAL(newPeriod, collector.SamplingInterval);
//            CPPUNIT_ASSERT_EQUAL((unsigned int) 0, collector.SamplingStride);
//            CPPUNIT_ASSERT(true == collector.TimeOffsetToZero);
//
//            collector.SetTimeBase(newPeriod, false);
//            CPPUNIT_ASSERT_EQUAL((unsigned int) 0, collector.SamplingStride);
//            CPPUNIT_ASSERT(false == collector.TimeOffsetToZero);
//        }
//    }
//    CPPUNIT_ASSERT(taskManager->RemoveTask(&task1));
//}
//
//void mtsCollectorBaseTest::TestSetTimeBaseInt(void)
//{
//    const unsigned int newStride = 2;
//
//    mtsCollectorSample collector("collector", 10 * cmn_ms);
//
//    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
//    CPPUNIT_ASSERT(taskManager);
//
//    mtsCollectorBaseTestTask task1("task1", 10 * cmn_ms);
//    CPPUNIT_ASSERT(taskManager->AddTask(&task1));
//    task1.StateTable.StateVectorDataNames.push_back("signal1");
//    {
//        // If there is no task registered to mtsTaskManager, SetTimeBase does NOP.   
//        int currentStride = collector.SamplingStride;
//        collector.SetTimeBase((unsigned int) 1, true);
//        CPPUNIT_ASSERT_EQUAL((unsigned int) currentStride, collector.SamplingStride);
//        CPPUNIT_ASSERT(false == collector.TimeOffsetToZero);
//
//        CPPUNIT_ASSERT(collector.AddSignal("task1", "signal1", ""));
//        CPPUNIT_ASSERT(collector.taskMap.size() == 1);
//        {
//            collector.SetTimeBase(newStride, true);
//            CPPUNIT_ASSERT_EQUAL(0.0, collector.SamplingInterval);
//            CPPUNIT_ASSERT_EQUAL(newStride, collector.SamplingStride);
//            CPPUNIT_ASSERT(true == collector.TimeOffsetToZero);
//
//            collector.SetTimeBase(newStride, false);
//            CPPUNIT_ASSERT_EQUAL(0.0, collector.SamplingInterval);
//            CPPUNIT_ASSERT_EQUAL(newStride, collector.SamplingStride);
//            CPPUNIT_ASSERT(false == collector.TimeOffsetToZero);
//        }
//    }
//    CPPUNIT_ASSERT(taskManager->RemoveTask(&task1));
//}
//*/
//
//void mtsCollectorBaseTest::TestStart(void)
//{
//    const double accuracy = 0.000001;
//    
//    mtsCollectorSample collector("collector", 10 * cmn_ms);    
//    
//    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, collector.DelayedStart, accuracy);
//    CPPUNIT_ASSERT(collector.Status == mtsCollectorBase::COLLECTOR_STOP);
//    CPPUNIT_ASSERT(collector.Stopwatch.IsRunning() == false);
//        
//    collector.Status = mtsCollectorBase::COLLECTOR_WAIT_START;
//    collector.Start(1.0);
//    {
//        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, collector.DelayedStart, accuracy);
//        CPPUNIT_ASSERT(collector.Status == mtsCollectorBase::COLLECTOR_WAIT_START);
//        CPPUNIT_ASSERT(collector.Stopwatch.IsRunning() == false);
//    }
//    
//    collector.Status = mtsCollectorBase::COLLECTOR_WAIT_STOP;
//    collector.Start(1.0);
//    {
//        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, collector.DelayedStart, accuracy);
//        CPPUNIT_ASSERT(collector.Status == mtsCollectorBase::COLLECTOR_WAIT_STOP);
//        CPPUNIT_ASSERT(collector.Stopwatch.IsRunning() == false);
//    }
//    
//    collector.Status = mtsCollectorBase::COLLECTOR_COLLECTING;
//    collector.Start(1.0);
//    {
//        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, collector.DelayedStart, accuracy);
//        CPPUNIT_ASSERT(collector.Status == mtsCollectorBase::COLLECTOR_COLLECTING);
//        CPPUNIT_ASSERT(collector.Stopwatch.IsRunning() == false);
//    }
//    
//    collector.Status = mtsCollectorBase::COLLECTOR_STOP;
//    collector.Start(1.0);
//    {
//        CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, collector.DelayedStart, accuracy);
//        CPPUNIT_ASSERT(collector.Status == mtsCollectorBase::COLLECTOR_WAIT_START);
//        CPPUNIT_ASSERT(collector.Stopwatch.IsRunning() == true);
//    }
//}
//
//void mtsCollectorBaseTest::TestStop(void)
//{
//    const double accuracy = 0.000001;
//    
//    mtsCollectorSample collector("collector", 10 * cmn_ms);    
//    
//    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, collector.DelayedStop, accuracy);
//    CPPUNIT_ASSERT(collector.Status == mtsCollectorBase::COLLECTOR_STOP);
//    CPPUNIT_ASSERT(collector.Stopwatch.IsRunning() == false);
//        
//    collector.Status = mtsCollectorBase::COLLECTOR_WAIT_START;
//    collector.Stop(1.0);
//    {
//        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, collector.DelayedStop, accuracy);
//        CPPUNIT_ASSERT(collector.Status == mtsCollectorBase::COLLECTOR_WAIT_START);
//        CPPUNIT_ASSERT(collector.Stopwatch.IsRunning() == false);
//    }
//    
//    collector.Status = mtsCollectorBase::COLLECTOR_WAIT_STOP;
//    collector.Stop(1.0);
//    {
//        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, collector.DelayedStop, accuracy);
//        CPPUNIT_ASSERT(collector.Status == mtsCollectorBase::COLLECTOR_WAIT_STOP);
//        CPPUNIT_ASSERT(collector.Stopwatch.IsRunning() == false);
//    }
//    
//    collector.Status = mtsCollectorBase::COLLECTOR_STOP;
//    collector.Stop(1.0);
//    {
//        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, collector.DelayedStop, accuracy);
//        CPPUNIT_ASSERT(collector.Status == mtsCollectorBase::COLLECTOR_STOP);
//        CPPUNIT_ASSERT(collector.Stopwatch.IsRunning() == false);
//    }
//    
//    collector.Status = mtsCollectorBase::COLLECTOR_COLLECTING;
//    collector.Stop(1.0);
//    {
//        CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, collector.DelayedStop, accuracy);
//        CPPUNIT_ASSERT(collector.Status == mtsCollectorBase::COLLECTOR_WAIT_STOP);
//        CPPUNIT_ASSERT(collector.Stopwatch.IsRunning() == true);
//    }
//}
//
////-----------------------------------------------------------------------------
////	Tests for private variables and methods
////-----------------------------------------------------------------------------
//void mtsCollectorBaseTest::TestInit(void)
//{
//    /* Enable this block after mtsCollectorSample class is implemented.
//    mtsCollectorSample collector("collector", 10 * cmn_ms);
//
//    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
//    CPPUNIT_ASSERT(taskManager);
//
//    // Put tasks for this test under the control of the taskManager so that
//    // mtsCollectorBase::AddSignal() works correctly.
//    mtsCollectorBaseTestTask task1("taskA");
//    mtsCollectorBaseTestTask task2("taskB");
//    mtsCollectorBaseTestTask task3("taskC");
//    CPPUNIT_ASSERT(taskManager->AddTask(&task1));
//    CPPUNIT_ASSERT(taskManager->AddTask(&task2));
//    CPPUNIT_ASSERT(taskManager->AddTask(&task3));
//    task1.StateTable.StateVectorDataNames.push_back("signal1-1");
//    task1.StateTable.StateVectorDataNames.push_back("signal1-2");
//    task2.StateTable.StateVectorDataNames.push_back("signal2");
//    task3.StateTable.StateVectorDataNames.push_back("signal3");
//    {
//        CPPUNIT_ASSERT(collector.AddSignal("taskA", "signal1-1", ""));
//        CPPUNIT_ASSERT(collector.AddSignal("taskA", "signal1-2", ""));
//        CPPUNIT_ASSERT(collector.AddSignal("taskB", "signal2", ""));
//        CPPUNIT_ASSERT(collector.AddSignal("taskC", "signal3", ""));
//
//        collector.Init();
//
//        // tests for correct variable initialization        
//        CPPUNIT_ASSERT(collector.taskMap.size()     == 0);
//        CPPUNIT_ASSERT(collector.TimeOffsetToZero   == false);
//        CPPUNIT_ASSERT(collector.Status             == mtsCollectorBase::COLLECTOR_STOP);
//        CPPUNIT_ASSERT_EQUAL((unsigned int) 0, collector.SamplingStride);
//
//        const double accuracy = 0.00001;
//        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, collector.DelayedStart, accuracy);
//        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, collector.DelayedStop, accuracy);
//        CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, collector.SamplingInterval, accuracy);
//    }
//    // Remove a task not to cause potential side-effect during the unit-test process
//    CPPUNIT_ASSERT(taskManager->RemoveTask(&task1));
//    CPPUNIT_ASSERT(taskManager->RemoveTask(&task2));
//    CPPUNIT_ASSERT(taskManager->RemoveTask(&task3));
//    */
//}
//
//void mtsCollectorBaseTest::TestClearTaskMap(void)
//{
//    mtsCollectorSample collector("collector", 10 * cmn_ms);
//
//    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
//    CPPUNIT_ASSERT(taskManager);
//
//    // Put tasks for this test under the control of the taskManager so that
//    // mtsCollectorBase::AddSignal() works correctly.
//    mtsCollectorBaseTestTask task1("task1");
//    mtsCollectorBaseTestTask task2("task2");
//    mtsCollectorBaseTestTask task3("task3");
//
//    CPPUNIT_ASSERT(taskManager->AddTask(&task1));
//    CPPUNIT_ASSERT(taskManager->AddTask(&task2));
//    CPPUNIT_ASSERT(taskManager->AddTask(&task3));
//    task1.StateTable.StateVectorDataNames.push_back("signal1-1");
//    task1.StateTable.StateVectorDataNames.push_back("signal1-2");
//    task2.StateTable.StateVectorDataNames.push_back("signal2");
//    task3.StateTable.StateVectorDataNames.push_back("signal3");
//    {
//        CPPUNIT_ASSERT(collector.AddSignal("task1", "signal1-1", ""));
//        CPPUNIT_ASSERT(collector.AddSignal("task1", "signal1-2", ""));
//        CPPUNIT_ASSERT(collector.AddSignal("task2", "signal2", ""));
//        CPPUNIT_ASSERT(collector.AddSignal("task3", "signal3", ""));
//
//        collector.ClearTaskMap();
//        CPPUNIT_ASSERT(collector.taskMap.size() == 0);
//    }
//    // Remove a task not to cause potential side-effect during the unit-test process
//    CPPUNIT_ASSERT(taskManager->RemoveTask(&task1));
//    CPPUNIT_ASSERT(taskManager->RemoveTask(&task2));
//    CPPUNIT_ASSERT(taskManager->RemoveTask(&task3));
//}
//
///*
//void mtsCollectorBaseTest::TestCollect(void)
//{
//    osaStopwatch Stopwatch;
//    Stopwatch.Reset();
//    //Stopwatch.Start();
//    
//    // Test Start() -> Collect()
//    //
//    //  Make unit-tests HERE!!!!!!
//    //
//    
//    // Test Stop() -> Collect()
//    //
//    //  Make unit-tests HERE!!!!!!
//    //
//}
//*/
//
//void mtsCollectorBaseTest::TestIsAnySignalRegistered(void)
//{
//    /* Enable this block after mtsCollectorSample class is implemented.
//    mtsCollectorSample collector("collector", 10 * cmn_ms);
//
//    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
//    CPPUNIT_ASSERT(taskManager);
//
//    mtsCollectorBaseTestTask task1("task1");    
//    task1.StateTable.StateVectorDataNames.push_back("signal1-1");
//    CPPUNIT_ASSERT(taskManager->AddTask(&task1));
//    {
//        CPPUNIT_ASSERT(collector.IsAnySignalRegistered() == false);
//        CPPUNIT_ASSERT(collector.AddSignal("task1", "signal1-1", ""));
//        CPPUNIT_ASSERT(collector.IsAnySignalRegistered() == true);
//    }
//    // Remove a task not to cause potential side-effect during the unit-test process
//    CPPUNIT_ASSERT(taskManager->RemoveTask(&task1));
//    */
//}

CPPUNIT_TEST_SUITE_REGISTRATION(mtsCollectorBaseTest);
