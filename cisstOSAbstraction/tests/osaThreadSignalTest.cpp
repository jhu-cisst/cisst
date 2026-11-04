/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2008-01-30

  (C) Copyright 2007-2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <thread>

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnPrintf.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstOSAbstraction/osaStopwatch.h>
#include <cisstOSAbstraction/osaThreadSignal.h>

#include "osaThreadSignalTest.h"


using namespace std::chrono_literals;

class ThreadSignalMethodArguments {
public:
    osaThreadSignal * ThreadSignal;
    osaStopwatch TimerWait;
    osaStopwatch TimerBlock;
    int iterations;
    double DelayToStart;
    double DelayToUnlock;
    double DelayToLock;
    std::string Name;
};

class ThreadSignalMethodHolder {
public:
	ThreadSignalMethodHolder(void) {}
	~ThreadSignalMethodHolder(void) {}

	void * Method(ThreadSignalMethodArguments * argument) {
        osaSleep(argument->DelayToStart);

        for (int iter = 0; iter < argument->iterations; iter++) {
            argument->TimerWait.Start();
            argument->ThreadSignal->Wait();
            argument->TimerWait.Stop();

            argument->TimerBlock.Start();
            std::this_thread::sleep_for(std::chrono::duration<double>(argument->DelayToUnlock));
            argument->TimerBlock.Stop();

            argument->ThreadSignal->Raise();
            std::this_thread::sleep_for(std::chrono::duration<double>(argument->DelayToLock));
        }

        return 0;
    }
};


void osaThreadSignalTest::TestWaitBlocks(void) {
    // all time intervals are in milli seconds!
    osaStopwatch overallTimer;
    overallTimer.Reset();
    overallTimer.Start();

    osaThreadSignal threadSignal;
    const unsigned int nbIterations = 10;
    const double delayToUnlock = 300 * cmn_ms; // ms
    ThreadSignalMethodHolder methodHolder1;
    ThreadSignalMethodArguments arguments1;
    arguments1.ThreadSignal = &threadSignal;
    arguments1.TimerWait.Reset();
    arguments1.TimerBlock.Reset();
    arguments1.iterations = nbIterations;
    arguments1.DelayToStart = 0 * cmn_ms;
    arguments1.DelayToUnlock = delayToUnlock;
    arguments1.DelayToLock = 20 * cmn_ms;
    arguments1.Name = "1";

    ThreadSignalMethodHolder methodHolder2;
    ThreadSignalMethodArguments arguments2;
    arguments2.ThreadSignal = &threadSignal;
    arguments2.TimerWait.Reset();
    arguments2.TimerBlock.Reset();
    arguments2.iterations = nbIterations;
    arguments2.DelayToStart = 20 * cmn_ms;
    arguments2.DelayToUnlock = delayToUnlock;
    arguments2.DelayToLock = 10 * cmn_ms;
    arguments2.Name = "2";

    // both threads will be starting with Wait, so we release first to
    // make sure one thread's wait will not block
    threadSignal.Raise();

    std::thread thread1([&]() { methodHolder1.Method(&arguments1); });
    std::thread thread2([&]() { methodHolder2.Method(&arguments2); });

    thread1.join();
    thread2.join();

    overallTimer.Stop();

    double totalTimeMeasured = overallTimer.GetElapsedTime();
    double marginOfError = totalTimeMeasured / 2.0; // large as we can't trust all OSs to be accurate

    double totalTimeExpected = 2 * nbIterations * delayToUnlock; // 2 threads
    CPPUNIT_ASSERT_DOUBLES_EQUAL(totalTimeExpected, totalTimeMeasured, marginOfError);

    double timeSpent1 = arguments1.TimerWait.GetElapsedTime() + arguments1.TimerBlock.GetElapsedTime();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(timeSpent1, totalTimeMeasured, marginOfError);

    double timeSpent2 = arguments2.TimerWait.GetElapsedTime() + arguments2.TimerBlock.GetElapsedTime();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(timeSpent2, totalTimeMeasured, marginOfError);
}

CPPUNIT_TEST_SUITE_REGISTRATION(osaThreadSignalTest);
