/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2010-11-09
  
  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "cmnLogLoDTest.h"

#include <cisstCommon/cmnLogLoD.h>

void cmnLogLoDTest::TestLogLevelToIndex(void)
{
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), cmnLogLevelToIndex(CMN_LOG_LEVEL_NONE));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), cmnLogLevelToIndex(CMN_LOG_LEVEL_INIT_ERROR));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), cmnLogLevelToIndex(CMN_LOG_LEVEL_INIT_WARNING));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), cmnLogLevelToIndex(CMN_LOG_LEVEL_INIT_VERBOSE));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), cmnLogLevelToIndex(CMN_LOG_LEVEL_INIT_DEBUG));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), cmnLogLevelToIndex(CMN_LOG_LEVEL_RUN_ERROR));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), cmnLogLevelToIndex(CMN_LOG_LEVEL_RUN_WARNING));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), cmnLogLevelToIndex(CMN_LOG_LEVEL_RUN_VERBOSE));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), cmnLogLevelToIndex(CMN_LOG_LEVEL_RUN_DEBUG));
}


void cmnLogLoDTest::TestIndexToLogLevel(void)
{
    CPPUNIT_ASSERT_EQUAL(CMN_LOG_LEVEL_NONE,         static_cast<int>(cmnIndexToLogLevel(0)));
    CPPUNIT_ASSERT_EQUAL(CMN_LOG_LEVEL_INIT_ERROR,   static_cast<int>(cmnIndexToLogLevel(1)));
    CPPUNIT_ASSERT_EQUAL(CMN_LOG_LEVEL_INIT_WARNING, static_cast<int>(cmnIndexToLogLevel(2)));
    CPPUNIT_ASSERT_EQUAL(CMN_LOG_LEVEL_INIT_VERBOSE, static_cast<int>(cmnIndexToLogLevel(3)));
    CPPUNIT_ASSERT_EQUAL(CMN_LOG_LEVEL_INIT_DEBUG,   static_cast<int>(cmnIndexToLogLevel(4)));
    CPPUNIT_ASSERT_EQUAL(CMN_LOG_LEVEL_RUN_ERROR,    static_cast<int>(cmnIndexToLogLevel(5)));
    CPPUNIT_ASSERT_EQUAL(CMN_LOG_LEVEL_RUN_WARNING,  static_cast<int>(cmnIndexToLogLevel(6)));
    CPPUNIT_ASSERT_EQUAL(CMN_LOG_LEVEL_RUN_VERBOSE,  static_cast<int>(cmnIndexToLogLevel(7)));
    CPPUNIT_ASSERT_EQUAL(CMN_LOG_LEVEL_RUN_DEBUG,    static_cast<int>(cmnIndexToLogLevel(8)));
}


void cmnLogLoDTest::TestMasks(void)
{
    CPPUNIT_ASSERT(!(CMN_LOG_ALLOW_NONE & CMN_LOG_LEVEL_INIT_ERROR));
    CPPUNIT_ASSERT(!(CMN_LOG_ALLOW_NONE & CMN_LOG_LEVEL_RUN_ERROR));

    CPPUNIT_ASSERT((CMN_LOG_ALLOW_ERRORS & CMN_LOG_LEVEL_INIT_ERROR));
    CPPUNIT_ASSERT((CMN_LOG_ALLOW_ERRORS & CMN_LOG_LEVEL_RUN_ERROR));
    CPPUNIT_ASSERT(!(CMN_LOG_ALLOW_ERRORS & CMN_LOG_LEVEL_INIT_WARNING));
    CPPUNIT_ASSERT(!(CMN_LOG_ALLOW_ERRORS & CMN_LOG_LEVEL_RUN_WARNING));

    CPPUNIT_ASSERT((CMN_LOG_ALLOW_ERRORS_AND_WARNINGS & CMN_LOG_LEVEL_INIT_ERROR));
    CPPUNIT_ASSERT((CMN_LOG_ALLOW_ERRORS_AND_WARNINGS & CMN_LOG_LEVEL_RUN_ERROR));
    CPPUNIT_ASSERT((CMN_LOG_ALLOW_ERRORS_AND_WARNINGS & CMN_LOG_LEVEL_INIT_WARNING));
    CPPUNIT_ASSERT((CMN_LOG_ALLOW_ERRORS_AND_WARNINGS & CMN_LOG_LEVEL_RUN_WARNING));
    CPPUNIT_ASSERT(!(CMN_LOG_ALLOW_ERRORS_AND_WARNINGS & CMN_LOG_LEVEL_INIT_VERBOSE));
    CPPUNIT_ASSERT(!(CMN_LOG_ALLOW_ERRORS_AND_WARNINGS & CMN_LOG_LEVEL_RUN_VERBOSE));

    CPPUNIT_ASSERT((CMN_LOG_ALLOW_VERBOSE & CMN_LOG_LEVEL_INIT_ERROR));
    CPPUNIT_ASSERT((CMN_LOG_ALLOW_VERBOSE & CMN_LOG_LEVEL_RUN_ERROR));
    CPPUNIT_ASSERT((CMN_LOG_ALLOW_VERBOSE & CMN_LOG_LEVEL_INIT_WARNING));
    CPPUNIT_ASSERT((CMN_LOG_ALLOW_VERBOSE & CMN_LOG_LEVEL_RUN_WARNING));
    CPPUNIT_ASSERT((CMN_LOG_ALLOW_VERBOSE & CMN_LOG_LEVEL_INIT_VERBOSE));
    CPPUNIT_ASSERT((CMN_LOG_ALLOW_VERBOSE & CMN_LOG_LEVEL_RUN_VERBOSE));
    CPPUNIT_ASSERT(!(CMN_LOG_ALLOW_VERBOSE & CMN_LOG_LEVEL_INIT_DEBUG));
    CPPUNIT_ASSERT(!(CMN_LOG_ALLOW_VERBOSE & CMN_LOG_LEVEL_RUN_DEBUG));

    CPPUNIT_ASSERT((CMN_LOG_ALLOW_ALL & CMN_LOG_LEVEL_INIT_ERROR));
    CPPUNIT_ASSERT((CMN_LOG_ALLOW_ALL & CMN_LOG_LEVEL_RUN_ERROR));
    CPPUNIT_ASSERT((CMN_LOG_ALLOW_ALL & CMN_LOG_LEVEL_INIT_WARNING));
    CPPUNIT_ASSERT((CMN_LOG_ALLOW_ALL & CMN_LOG_LEVEL_RUN_WARNING));
    CPPUNIT_ASSERT((CMN_LOG_ALLOW_ALL & CMN_LOG_LEVEL_INIT_VERBOSE));
    CPPUNIT_ASSERT((CMN_LOG_ALLOW_ALL & CMN_LOG_LEVEL_RUN_VERBOSE));
    CPPUNIT_ASSERT((CMN_LOG_ALLOW_ALL & CMN_LOG_LEVEL_INIT_DEBUG));
    CPPUNIT_ASSERT((CMN_LOG_ALLOW_ALL & CMN_LOG_LEVEL_RUN_DEBUG));
}

#include <cisstCommon/cmnLogger.h>

void cmnLogLoDTest::TestIfElse(void)
{
    bool beenThere;
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    beenThere = false;
    if (false)
        CMN_LOG_RUN_ERROR << "cmnLogLoDTest::TestIfElse" << std::endl;
    else
        beenThere = true;
    CPPUNIT_ASSERT(beenThere);
}
