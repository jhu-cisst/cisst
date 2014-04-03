/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung, Anton Deguet
  Created on: 2009-11-17

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cisstMultiTask/mtsConfig.h>

class mtsManagerLocal;

class mtsComponentStateTest: public CppUnit::TestFixture
{
private:

    CPPUNIT_TEST_SUITE(mtsComponentStateTest);
    {
        CPPUNIT_TEST(TestOrder);
        CPPUNIT_TEST(TestTransitions);
    }
    CPPUNIT_TEST_SUITE_END();

public:
    mtsComponentStateTest();

    void setUp(void);
    void tearDown(void);
    
    void TestOrder(void);
    void TestTransitions(void);
};
