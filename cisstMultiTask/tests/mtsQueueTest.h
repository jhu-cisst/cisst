/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2009-04-29
  
  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstMultiTask/mtsQueue.h>
#include <cisstMultiTask/mtsGenericObjectProxy.h>


class mtsQueueTest: public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(mtsQueueTest);

    CPPUNIT_TEST(TestQueue_mtsDouble);
    CPPUNIT_TEST(TestConstructorDestructorCalls);

    CPPUNIT_TEST_SUITE_END();
    
public:
    void setUp(void) {}
    
    void tearDown(void) {}
    
    /*! Using a proxy class */
    void TestQueue_mtsDouble(void);

    /*! Tests calls to constructors and detructors */
    void TestConstructorDestructorCalls(void);
};


CPPUNIT_TEST_SUITE_REGISTRATION(mtsQueueTest);

