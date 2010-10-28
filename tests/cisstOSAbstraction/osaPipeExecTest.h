/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id:

  Author(s):  Martin Kelly
  Created on: 2010-09-23

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _osaPipeExecTest_h
#define _osaPipeExecTest_h

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

class osaPipeExecTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(osaPipeExecTest);
    {
        CPPUNIT_TEST(TestPipe);
		CPPUNIT_TEST(TestPipeInternalsSize);
	}

    CPPUNIT_TEST_SUITE_END();

public:
    /*! Test that structure for internal size is large enough */
    void TestPipeInternalsSize(void);

    /*! Test communication with cisstOSAbstractionTestsPipeUtility */
    void TestPipe(void);
};
#endif // _osaPipeExecTest_h
