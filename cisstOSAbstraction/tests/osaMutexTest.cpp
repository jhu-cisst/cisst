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

#include "osaMutexTest.h"

#include <cisstOSAbstraction/osaMutex.h>
#include <cisstOSAbstraction/osaCriticalSection.h>

#include <string.h>

// abstract class to test either a mutex or a critical section
class osaTestMutexOrCriticalSection
{
public:
    virtual void LockOrEnter(void) = 0;
    virtual void UnlockOrLeave(void) = 0;
};

// abstract class derived for mutex
class osaTestMutex: public osaTestMutexOrCriticalSection
{
    osaMutex Mutex;
public:
    inline void LockOrEnter(void) {
        Mutex.Lock();
    }
    inline void UnlockOrLeave(void) {
        Mutex.Unlock();
    }
};

// abstract class derived for criticalsection
class osaTestCriticalSection: public osaTestMutexOrCriticalSection
{
    osaCriticalSection CriticalSection;
public:
    inline void LockOrEnter(void) {
        CriticalSection.Enter();
    }
    inline void UnlockOrLeave(void) {
        CriticalSection.Leave();
    }
};


class MethodArguments {
public:
    osaTestMutexOrCriticalSection * MutexOrCriticalSection;
    osaStopwatch TimerWait;
    osaStopwatch TimerBlock;
    unsigned int NumberOfIterations;
    double DelayToStart;
    double DelayToUnlock;
    double DelayToLock;
    std::string Name;
};

class MethodHolder {
public:
	MethodHolder(void) {}
	~MethodHolder(void) {}
	void * Method(MethodArguments * argument) {
        unsigned int index;
        osaSleep(argument->DelayToStart);
        for (index = 0;
             index < argument->NumberOfIterations;
             index++) {
            argument->TimerWait.Start();
            argument->MutexOrCriticalSection->LockOrEnter();
            argument->TimerWait.Stop();
            argument->TimerBlock.Start();
            osaSleep(argument->DelayToUnlock);
            argument->TimerBlock.Stop();
            argument->MutexOrCriticalSection->UnlockOrLeave();
            osaSleep(argument->DelayToLock);
        }
        return 0;
    }
};


void osaMutexTestTestLockBlocks(osaTestMutexOrCriticalSection & objectTested)
{
    // all time intervals are in milli seconds!
    osaStopwatch overallTimer;
    overallTimer.Reset();
    overallTimer.Start();

    const unsigned int nbIterations = 10;
    const double delayToUnlock = 300 * cmn_ms; // ms
    osaThread thread1;
    MethodHolder methodHolder1;
    MethodArguments arguments1;
    arguments1.MutexOrCriticalSection = &objectTested;
    arguments1.TimerWait.Reset();
    arguments1.TimerBlock.Reset();
    arguments1.NumberOfIterations = nbIterations;
    arguments1.DelayToStart = 0 * cmn_ms;
    arguments1.DelayToUnlock = delayToUnlock;
    arguments1.DelayToLock = 20 * cmn_ms;
    arguments1.Name = "1";

    osaThread thread2;
    MethodHolder methodHolder2;
    MethodArguments arguments2;
    arguments2.MutexOrCriticalSection = &objectTested;
    arguments2.TimerWait.Reset();
    arguments2.TimerBlock.Reset();
    arguments2.NumberOfIterations = nbIterations;
    arguments2.DelayToStart = 20 * cmn_ms;
    arguments2.DelayToUnlock = delayToUnlock;
    arguments2.DelayToLock = 10 * cmn_ms;
    arguments2.Name = "2";

    osaThread thread3;
    MethodHolder methodHolder3;
    MethodArguments arguments3;
    arguments3.MutexOrCriticalSection = &objectTested;
    arguments3.TimerWait.Reset();
    arguments3.TimerBlock.Reset();
    arguments3.NumberOfIterations = nbIterations;
    arguments3.DelayToStart = 10 * cmn_ms;
    arguments3.DelayToUnlock = delayToUnlock;
    arguments3.DelayToLock = 10 * cmn_ms;
    arguments3.Name = "3";

    thread1.Create<MethodHolder, MethodArguments *>(&methodHolder1, &MethodHolder::Method, &arguments1, arguments1.Name.c_str());
    thread2.Create<MethodHolder, MethodArguments *>(&methodHolder2, &MethodHolder::Method, &arguments2, arguments2.Name.c_str());
    thread3.Create<MethodHolder, MethodArguments *>(&methodHolder3, &MethodHolder::Method, &arguments3, arguments3.Name.c_str());

    thread1.Wait();
    thread2.Wait();
    thread3.Wait();

    overallTimer.Stop();

    double totalTimeMeasured = overallTimer.GetElapsedTime();
    double margeOfError = totalTimeMeasured / 2.0; // large as we can't trust all OSs to be accurate

    double totalTimeExpected = 3 * nbIterations * delayToUnlock; // 3 threads
    CPPUNIT_ASSERT_DOUBLES_EQUAL(totalTimeExpected, totalTimeMeasured, margeOfError);

    double timeSpent1 = arguments1.TimerWait.GetElapsedTime() + arguments1.TimerBlock.GetElapsedTime();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(timeSpent1, totalTimeMeasured, margeOfError);

    double timeSpent2 = arguments2.TimerWait.GetElapsedTime() + arguments2.TimerBlock.GetElapsedTime();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(timeSpent2, totalTimeMeasured, margeOfError);

    double timeSpent3 = arguments3.TimerWait.GetElapsedTime() + arguments3.TimerBlock.GetElapsedTime();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(timeSpent3, totalTimeMeasured, margeOfError);
}


void osaMutexTest::TestMutexLockBlocks(void) {
    // all time intervals are in milli seconds!
    osaTestMutex mutex;
    osaMutexTestTestLockBlocks(mutex);
}


void osaMutexTest::TestCriticalSectionLockBlocks(void) {
    // all time intervals are in milli seconds!
    osaTestCriticalSection criticalSection;
    osaMutexTestTestLockBlocks(criticalSection);
}


CPPUNIT_TEST_SUITE_REGISTRATION(osaMutexTest);

