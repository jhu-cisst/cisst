/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Min Yang Jung
  Created on: 2009-11-17
  
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

#include <cisstMultiTask/mtsConfig.h>
#include <cisstMultiTask/mtsManagerLocal.h>

class mtsManagerGlobalTest: public CppUnit::TestFixture
{
private:
    mtsManagerLocal *localManager1, *localManager2;

    CPPUNIT_TEST_SUITE(mtsManagerGlobalTest);
    {
        CPPUNIT_TEST(TestConstructor);
        CPPUNIT_TEST(TestDestructor);
        CPPUNIT_TEST(TestCleanup);

        CPPUNIT_TEST(TestConnectedInterfaceInfo);
        CPPUNIT_TEST(TestConnectionElement);

        CPPUNIT_TEST(TestGetConnectionsOfInterfaceProvided);
        CPPUNIT_TEST(TestGetConnectionsOfInterfaceRequired);

        CPPUNIT_TEST(TestAddProcess);
        CPPUNIT_TEST(TestFindProcess);
        CPPUNIT_TEST(TestRemoveProcess);

        //CPPUNIT_TEST(TestGetProcessObject);

        CPPUNIT_TEST(TestAddComponent);
        CPPUNIT_TEST(TestFindComponent);
        CPPUNIT_TEST(TestRemoveComponent);

        CPPUNIT_TEST(TestAddInterfaceProvided);
        CPPUNIT_TEST(TestFindInterfaceProvided);
        CPPUNIT_TEST(TestRemoveInterfaceProvided);

        CPPUNIT_TEST(TestAddInterfaceRequired);
        CPPUNIT_TEST(TestFindInterfaceRequired);
        CPPUNIT_TEST(TestRemoveInterfaceRequired);

        CPPUNIT_TEST(TestConnectLocal);
#if CISST_MTS_HAS_ICE
        //CPPUNIT_TEST(TestConnectRemote);
        //CPPUNIT_TEST(TestDisconnect);
#endif
        CPPUNIT_TEST(TestIsAlreadyConnected);
        CPPUNIT_TEST(TestAddConnectedInterface);
	}
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    void TestConstructor(void);
    void TestDestructor(void);
    void TestCleanup(void);

    void TestConnectedInterfaceInfo(void);
    void TestConnectionElement(void);

    void TestGetConnectionsOfInterfaceProvided(void);
    void TestGetConnectionsOfInterfaceRequired(void);

    void TestAddProcess(void);
    void TestFindProcess(void);
    void TestRemoveProcess(void);

    //void TestGetProcessObject(void);

    void TestAddComponent(void);
    void TestFindComponent(void);
    void TestRemoveComponent(void);

    void TestAddInterfaceProvided(void);
    void TestFindInterfaceProvided(void);
    void TestRemoveInterfaceProvided(void);
         
    void TestAddInterfaceRequired(void);
    void TestFindInterfaceRequired(void);
    void TestRemoveInterfaceRequired(void);

    void TestConnectLocal(void);
#if CISST_MTS_HAS_ICE
    //void TestConnectRemote(void);
    //void TestDisconnect(void);
#endif
    void TestIsAlreadyConnected(void);
    void TestAddConnectedInterface(void);
};
