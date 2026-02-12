/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2020-06-25

  (C) Copyright 2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

class mtsTypesTestServer;
class mtsTypesTestClient;

class mtsTypesTest: public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(mtsTypesTest);
	{
        CPPUNIT_TEST(TestSetup);
        CPPUNIT_TEST(TestInt);
        CPPUNIT_TEST(TestDouble);
        CPPUNIT_TEST(TestBool);
		CPPUNIT_TEST(TestStdString);
    }
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

    void TestSetup(void);
    void TestInt(void);
    void TestDouble(void);
    void TestBool(void);
	void TestStdString(void);

protected:
    mtsTypesTestServer * mServer;
    mtsTypesTestClient * mClient;
};
