/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2012-04-02

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

class osaTimeServerTest: public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(osaTimeServerTest);
    {
        CPPUNIT_TEST(TestInternalsSize);
        CPPUNIT_TEST(TestMultipleServersSingleThread);
        CPPUNIT_TEST(TestMultipleServersMultiThreads);
    }
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void) {
    }

    void tearDown(void) {
    }

    /*! Test that structure for internal size is large enough */
    void TestInternalsSize(void);

    /*! Check that multiple server on a single thread have more or
      less the same origin */
    void TestMultipleServersSingleThread(void);

    /*! Check that multiple server on multiple threads have more or
      less the same origin */
    void TestMultipleServersMultiThreads(void);
};
