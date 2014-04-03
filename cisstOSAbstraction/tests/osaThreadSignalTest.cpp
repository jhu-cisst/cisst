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

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnPrintf.h>
#include <cisstOSAbstraction/osaStopwatch.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstOSAbstraction/osaThread.h>

#include "osaThreadSignalTest.h"

#include <cisstOSAbstraction/osaThreadSignal.h>

#include <string.h>


class ThreadSignalMethodArguments {
public:
    osaThreadSignal * ThreadSignal;
    osaStopwatch TimerWait;
    osaStopwatch TimerBlock;
    unsigned int NumberOfIterations;
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
        unsigned int index;
        osaSleep(argument->DelayToStart);
        for (index = 0;
             index < argument->NumberOfIterations;
             index++) {
            argument->TimerWait.Start();
            argument->ThreadSignal->Wait();
            argument->TimerWait.Stop();
            argument->TimerBlock.Start();
            osaSleep(argument->DelayToUnlock);
            argument->TimerBlock.Stop();
            argument->ThreadSignal->Raise();
            osaSleep(argument->DelayToLock);
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
    osaThread thread1;
    ThreadSignalMethodHolder methodHolder1;
    ThreadSignalMethodArguments arguments1;
    arguments1.ThreadSignal = &threadSignal;
    arguments1.TimerWait.Reset();
    arguments1.TimerBlock.Reset();
    arguments1.NumberOfIterations = nbIterations;
    arguments1.DelayToStart = 0 * cmn_ms;
    arguments1.DelayToUnlock = delayToUnlock;
    arguments1.DelayToLock = 20 * cmn_ms;
    arguments1.Name = "1";

    osaThread thread2;
    ThreadSignalMethodHolder methodHolder2;
    ThreadSignalMethodArguments arguments2;
    arguments2.ThreadSignal = &threadSignal;
    arguments2.TimerWait.Reset();
    arguments2.TimerBlock.Reset();
    arguments2.NumberOfIterations = nbIterations;
    arguments2.DelayToStart = 20 * cmn_ms;
    arguments2.DelayToUnlock = delayToUnlock;
    arguments2.DelayToLock = 10 * cmn_ms;
    arguments2.Name = "2";

    // both threads will be starting with Wait, so we release first to
    // make sure one thread's wait will not block
    threadSignal.Raise();

    thread1.Create<ThreadSignalMethodHolder, ThreadSignalMethodArguments *>(&methodHolder1, &ThreadSignalMethodHolder::Method, &arguments1, arguments1.Name.c_str());
    thread2.Create<ThreadSignalMethodHolder, ThreadSignalMethodArguments *>(&methodHolder2, &ThreadSignalMethodHolder::Method, &arguments2, arguments2.Name.c_str());

    thread1.Wait();
    thread2.Wait();

    overallTimer.Stop();

    double totalTimeMeasured = overallTimer.GetElapsedTime();
    double margeOfError = totalTimeMeasured / 2.0; // large as we can't trust all OSs to be accurate

    double totalTimeExpected = 2 * nbIterations * delayToUnlock; // 2 threads
    CPPUNIT_ASSERT_DOUBLES_EQUAL(totalTimeExpected, totalTimeMeasured, margeOfError);

    double timeSpent1 = arguments1.TimerWait.GetElapsedTime() + arguments1.TimerBlock.GetElapsedTime();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(timeSpent1, totalTimeMeasured, margeOfError);

    double timeSpent2 = arguments2.TimerWait.GetElapsedTime() + arguments2.TimerBlock.GetElapsedTime();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(timeSpent2, totalTimeMeasured, margeOfError);
}


CPPUNIT_TEST_SUITE_REGISTRATION(osaThreadSignalTest);
