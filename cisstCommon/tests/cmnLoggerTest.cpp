/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Peter Kazanzides
  Created on: 2018-12-16

  (C) Copyright 2018-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "cmnLoggerTest.h"

#include <cisstCommon/cmnLogger.h>
#include <cisstCommon/cmnPath.h>

void cmnLoggerTest::TestLoggerFileName(void)
{
    static bool setDefaultLogFileName = false;

    // Depending on order that tests are run, logger may already be created. This means that we
    // have to trust that cmnLogger::IsCreated is working. To run "Case 2" (logger not yet created),
    // try running just a single test
    if (cmnLogger::IsCreated()) {
        std::cout << std::endl << "TestLoggerFileName: Case 1: log file created before test" << std::endl;
    }
    else {
        std::cout << std::endl << "TestLoggerFileName: Case 2: log file not created before test" << std::endl;
        if (cmnPath::Exists("testLog.txt")) {
            cmnPath::DeleteFile("testLog.txt");
            CPPUNIT_ASSERT(!cmnPath::Exists("testLog.txt"));
        }
        CPPUNIT_ASSERT_EQUAL(true, cmnLogger::SetDefaultLogFileName("testLog.txt"));
        setDefaultLogFileName = true;

        CPPUNIT_ASSERT_EQUAL(std::string("testLog.txt"), cmnLogger::GetDefaultLogFileName());
        // Following call will create log file
        CMN_LOG_RUN_VERBOSE << "TestLoggerFileName: Creating log file" << std::endl;
        CPPUNIT_ASSERT_EQUAL(true, cmnLogger::IsCreated());
        CPPUNIT_ASSERT_EQUAL(true, cmnPath::Exists("testLog.txt"));
    }

    if (setDefaultLogFileName) {
        // If already set, then setting again should do nothing and return false,
        // regardless of the name passed in:
        CPPUNIT_ASSERT_EQUAL(false, cmnLogger::SetDefaultLogFileName("testLog.txt"));
        CPPUNIT_ASSERT_EQUAL(false, cmnLogger::SetDefaultLogFileName("cisstLog.txt"));
        CPPUNIT_ASSERT_EQUAL(std::string("testLog.txt"), cmnLogger::GetDefaultLogFileName());
    }
    else {
        CPPUNIT_ASSERT_EQUAL(std::string("cisstLog.txt"), cmnLogger::GetDefaultLogFileName());
    }
}
