/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2008-01-30

  (C) Copyright 2007-2008 Johns Hopkins University (JHU), All Rights
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
#include <cisstOSAbstraction/osaCPUAffinity.h>

#include "osaTimeServerTest.h"

#include <cisstOSAbstraction/osaTimeServer.h>

#include <string.h>


void osaTimeServerTest::TestInternalsSize(void) {
    CPPUNIT_ASSERT(osaTimeServer::INTERNALS_SIZE >= osaTimeServer::SizeOfInternals());
}


void osaTimeServerTest::TestMultipleServersSingleThread(void)
{
    osaCPUSetAffinity(OSA_CPU1);

    const size_t numberOfServers = 10;
    osaTimeServer * servers[numberOfServers];

    size_t index;
    osaStopwatch stopwatch;
    double currentTime[numberOfServers];

    double currentTimeMax;
    double currentTimeMin;

    for (index = 0;
         index < numberOfServers;
         ++index) {
        servers[index] = new osaTimeServer();
        CPPUNIT_ASSERT(servers[index]);
    }

    for (index = 0;
         index < numberOfServers;
         ++index) {
        servers[index]->SetTimeOrigin();
    }

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

    for (index = 0;
         index < numberOfServers;
         ++index) {
        std::cerr << cmnInternalTo_ms(currentTimeMax - currentTime[index]) << " ";
    }

    std::cerr << std::endl
              << "delta in ms: " << cmnInternalTo_ms(currentTimeMax - currentTimeMin) << std::endl
              << "time to get time: " << cmnInternalTo_ms(stopwatch.GetElapsedTime()) << std::endl;
    CPPUNIT_ASSERT((currentTimeMax - currentTimeMin) < (stopwatch.GetElapsedTime() + 5.0 * cmn_ms)); // add 5 ms to be generous
}


CPPUNIT_TEST_SUITE_REGISTRATION(osaTimeServerTest);
