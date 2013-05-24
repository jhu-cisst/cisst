
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


#include "osaTimeDataTest.h"
#include <iomanip>
#include <cisstOSAbstraction/osaTimeData.h>


/*void osaTimeServerTest::TestInternalsSize(void) {

    CPPUNIT_ASSERT(osaTimeServer::INTERNALS_SIZE >= osaTimeServer::SizeOfInternals());
}*/

void osaTimeDataTest::TestAlgebra(void)
{
	osaTimeData temp;
	osaTimeData temp2;
	osaTimeData result; 
	result.SumOf(temp,temp2);
	CPPUNIT_ASSERT((temp+temp2)==result);

	result.SumOf(temp,temp2);
	result.Subtract(temp);
	CPPUNIT_ASSERT(temp2==result);

	osaTimeData t(200,6913578,true);
	osaTimeData t2(100,3456789,true);
	CPPUNIT_ASSERT(t==t2*2);
	
	CPPUNIT_ASSERT(t/2==t2);

}

void osaTimeDataTest::TestEqual(void)
{
	osaTimeData left = osaTimeData(100,0,true);
	osaTimeData right =osaTimeData(100,0,true);
	CPPUNIT_ASSERT(left == right);
}

void osaTimeDataTest::TestInequalities(void)
{
	osaTimeData before;
    before.Now();
	osaTimeData after;
    after.Now();
	CPPUNIT_ASSERT(before <= after);	


	before.Add( after);
	CPPUNIT_ASSERT(before > after);	
}

void osaTimeDataTest::TestFromTo(void)
{
	osaTimeData now(10,234,true);
	double nowInSeconds = now.ToSeconds();
	osaTimeData shouldBeSame(nowInSeconds);
	CPPUNIT_ASSERT(now == shouldBeSame);
}
/*
// #define OSA_SHARE_SAME_TIME_ORIGIN

#ifndef OSA_SHARE_SAME_TIME_ORIGIN
    // This tests consistency of synchronization (e.g., on Windows)
    for (index = 0;
         index < numberOfServers;
         ++index) {
        servers[index]->SetTimeOrigin();
    }
#else
    servers[0]->SetTimeOrigin();
    for (index = 1;
         index < numberOfServers;
         ++index) {
        servers[index]->SetTimeOriginFrom(servers[0]);
    }
#endif

    stopwatch.Start();

    for (index = 0;
         index < numberOfServers;
         ++index) {
        currentTime[index] = servers[index]->GetAbsoluteTimeInSeconds();
    }
    stopwatch.Stop();

    currentTimeMax = cmnTypeTraits<double>::MinNegativeValue();
    currentTimeMin = cmnTypeTraits<double>::MaxPositiveValue();

    for (index = 0;
         index < numberOfServers;
         ++index) {
        if (currentTime[index] > currentTimeMax) {
            currentTimeMax = currentTime[index];
        }
        if (currentTime[index] < currentTimeMin) {
            currentTimeMin = currentTime[index];
        }
    }

    CPPUNIT_ASSERT((currentTimeMax - currentTimeMin) < (stopwatch.GetElapsedTime() + 5.0 * cmn_ms)); // add 5 ms to be generous
}


struct osaTimeServerTestThreadData
{
    size_t ThreadIndex;
    osaThread * Thread;

    bool * QuitFlag;

    bool SetTimeOriginFlag;
    bool GetTimeFlag;

    double LastTime;
    osaTimeServer TimeServer;
};


void * osaTimeServerTestRun(osaTimeServerTestThreadData * data)
{
    while (!(*(data->QuitFlag))) {
        if (data->GetTimeFlag) {
            data->LastTime = data->TimeServer.GetAbsoluteTimeInSeconds();
            data->GetTimeFlag = false;
        }
        if (data->SetTimeOriginFlag) {
            data->TimeServer.SetTimeOrigin();
            data->SetTimeOriginFlag = false;
        }
        osaSleep(0.1 * cmn_ms);
    }
    return 0;
}


void osaTimeServerTest::TestMultipleServersMultiThreads(void)
{
    const size_t numberOfServers = 20;
    size_t index;
    osaTimeServerTestThreadData * threadsData[numberOfServers];
    osaTimeServerTestThreadData * threadData;
    osaThread * thread;
    bool quitFlag = false;

    osaStopwatch stopwatch;
    double currentTimeMax;
    double currentTimeMin;


    for (index = 0;
         index < numberOfServers;
         ++index) {


        threadData = new osaTimeServerTestThreadData;
        threadsData[index] = threadData;

        threadData->ThreadIndex = index;
        threadData->QuitFlag = &quitFlag;

        threadData->SetTimeOriginFlag = false;
        threadData->GetTimeFlag = false;

        thread = new osaThread;
        threadData->Thread = thread;

        thread->Create<osaTimeServerTestThreadData*>(&osaTimeServerTestRun,
                                                     threadsData[index]);
    }


#ifndef OSA_SHARE_SAME_TIME_ORIGIN
    for (index = 0;
         index < numberOfServers;
         ++index) {
        threadData = threadsData[index];
        threadData->SetTimeOriginFlag = true;
        while (threadData->SetTimeOriginFlag) {
            osaSleep(0.1 * cmn_ms);
        }
    }
#else
    // This works on Windows
    threadData = threadsData[0];
    threadData->SetTimeOriginFlag = true;
    while (threadData->SetTimeOriginFlag) {
        osaSleep(0.1 * cmn_ms);
    }

    for (index = 1;
         index < numberOfServers;
         ++index) {
        threadsData[index]->TimeServer.SetTimeOriginFrom(&(threadsData[0]->TimeServer));
    }
#endif


    stopwatch.Start();

    // trigger read time
    for (index = 0;
         index < numberOfServers;
         ++index) {
        threadData = threadsData[index];
        threadData->GetTimeFlag = true;
    }

    // make sure all time read
    for (index = 0;
         index < numberOfServers;
         ++index) {
        threadData = threadsData[index];
        while (threadData->GetTimeFlag) {
            osaSleep(0.1 * cmn_ms);
        }
    }

    stopwatch.Stop();

    // read all time values
    for (index = 0;
         index < numberOfServers;
         ++index) {
        threadData = threadsData[index];
    }

    currentTimeMax = cmnTypeTraits<double>::MinNegativeValue();
    currentTimeMin = cmnTypeTraits<double>::MaxPositiveValue();

    for (index = 0;
         index < numberOfServers;
         ++index) {
        threadData = threadsData[index];
        if (threadData->LastTime > currentTimeMax) {
            currentTimeMax = threadData->LastTime;
        }
        if (threadData->LastTime < currentTimeMin) {
            currentTimeMin = threadData->LastTime;
        }
    }

    CPPUNIT_ASSERT((currentTimeMax - currentTimeMin) < (stopwatch.GetElapsedTime() + 5.0 * cmn_ms)); // add 5 ms to be generous

    quitFlag = true;

    // to make sure all threads are stopped and data freed
    for (index = 0;
         index < numberOfServers;
         ++index) {
        threadsData[index]->Thread->Wait();
        delete threadsData[index]->Thread;
        delete threadsData[index];
    }
}
*/

CPPUNIT_TEST_SUITE_REGISTRATION(osaTimeDataTest);
