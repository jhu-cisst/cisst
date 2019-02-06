/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Martin Kelly
  Created on: 2010-09-23

  (C) Copyright 2010-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnPortability.h>
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

class osaPipeExecTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(osaPipeExecTest);
    {
#if (CISST_OS == CISST_WINDOWS)
        CPPUNIT_TEST(TestPipeWindow);
        CPPUNIT_TEST(TestPipeNoWindow);
#else
        CPPUNIT_TEST(TestPipe);
#endif
		CPPUNIT_TEST(TestPipeInternalsSize);
	}

    CPPUNIT_TEST_SUITE_END();

    /* Keep attempting to read until length characters have been read or
    stopChar is read. If neither of these conditions occur or if line
    buffering is enabled, this can cause an infinite loop */
    void readUntil(osaPipeExec & pipe, char * buffer, int length, char stopChar);

public:
    /*! Test that structure for internal size is large enough */
    void TestPipeInternalsSize(void);

    /*! Test communication with cisstOSAbstractionTestsPipeUtility */
    void TestPipeCommon(bool noWindow);
#if (CISST_OS == CISST_WINDOWS)
    void TestPipeWindow(void);
    void TestPipeNoWindow(void);
#else
    void TestPipe(void);
#endif
};
