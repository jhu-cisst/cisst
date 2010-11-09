/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnLogLoDTest.cpp 1236 2010-02-26 20:38:21Z adeguet1 $
  
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

void cmnLogLoDTest::TestLogBitsetToIndex(void)
{
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), cmnLogBitsetToIndex(CMN_LOG_BIT_NONE));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), cmnLogBitsetToIndex(CMN_LOG_BIT_INIT_ERROR));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), cmnLogBitsetToIndex(CMN_LOG_BIT_INIT_WARNING));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), cmnLogBitsetToIndex(CMN_LOG_BIT_INIT_VERBOSE));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), cmnLogBitsetToIndex(CMN_LOG_BIT_INIT_DEBUG));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), cmnLogBitsetToIndex(CMN_LOG_BIT_RUN_ERROR));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), cmnLogBitsetToIndex(CMN_LOG_BIT_RUN_WARNING));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), cmnLogBitsetToIndex(CMN_LOG_BIT_RUN_VERBOSE));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), cmnLogBitsetToIndex(CMN_LOG_BIT_RUN_DEBUG));
}


void cmnLogLoDTest::TestIndexToLogBitset(void)
{
    CPPUNIT_ASSERT_EQUAL(CMN_LOG_BIT_NONE,         static_cast<int>(cmnIndexToLogBitset(0)));
    CPPUNIT_ASSERT_EQUAL(CMN_LOG_BIT_INIT_ERROR,   static_cast<int>(cmnIndexToLogBitset(1)));
    CPPUNIT_ASSERT_EQUAL(CMN_LOG_BIT_INIT_WARNING, static_cast<int>(cmnIndexToLogBitset(2)));
    CPPUNIT_ASSERT_EQUAL(CMN_LOG_BIT_INIT_VERBOSE, static_cast<int>(cmnIndexToLogBitset(3)));
    CPPUNIT_ASSERT_EQUAL(CMN_LOG_BIT_INIT_DEBUG,   static_cast<int>(cmnIndexToLogBitset(4)));
    CPPUNIT_ASSERT_EQUAL(CMN_LOG_BIT_RUN_ERROR,    static_cast<int>(cmnIndexToLogBitset(5)));
    CPPUNIT_ASSERT_EQUAL(CMN_LOG_BIT_RUN_WARNING,  static_cast<int>(cmnIndexToLogBitset(6)));
    CPPUNIT_ASSERT_EQUAL(CMN_LOG_BIT_RUN_VERBOSE,  static_cast<int>(cmnIndexToLogBitset(7)));
    CPPUNIT_ASSERT_EQUAL(CMN_LOG_BIT_RUN_DEBUG,    static_cast<int>(cmnIndexToLogBitset(8)));
}
