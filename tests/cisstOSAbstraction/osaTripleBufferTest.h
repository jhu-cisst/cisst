/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id:

  Author(s):  Martin Kelly, Anton Deguet
  Created on: 2011-03-15

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _osaTripleBufferTest_h
#define _osaTripleBufferTest_h

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

class osaTripleBufferTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(osaTripleBufferTest);
    {
        CPPUNIT_TEST(TestLogic);
        CPPUNIT_TEST(TestMultiThreading);
	}
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);

    /*! Test logic */
    void TestLogic(void);

    /*! Test multi threading */
    void TestMultiThreading(void);
};

#endif // _osaTripleBufferTest_h
