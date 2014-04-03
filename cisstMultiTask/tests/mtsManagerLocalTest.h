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

class mtsManagerLocalTest: public CppUnit::TestFixture
{
private:

    CPPUNIT_TEST_SUITE(mtsManagerLocalTest);
    {
        CPPUNIT_TEST(TestInitialize);
        CPPUNIT_TEST(TestConstructor);
        CPPUNIT_TEST(TestCleanup);

        CPPUNIT_TEST(TestAddComponent);
        CPPUNIT_TEST(TestFindComponent);
        CPPUNIT_TEST(TestRemoveComponent);
        CPPUNIT_TEST(TestRegisterInterfaces);

        CPPUNIT_TEST(TestGetInstance);
        CPPUNIT_TEST(TestGetComponent);
        CPPUNIT_TEST(TestGetNamesOfComponents);
        CPPUNIT_TEST(TestGetProcessName);

        CPPUNIT_TEST(TestConnectLocally);
        CPPUNIT_TEST(TestConnectDisconnect);

#if CISST_MTS_HAS_ICE
        CPPUNIT_TEST(TestGetIPAddressList);
        CPPUNIT_TEST(TestGetName);

        CPPUNIT_TEST(TestConnectServerSideInterface);
        CPPUNIT_TEST(TestCreateInterfaceRequiredProxy);
        CPPUNIT_TEST(TestCreateInterfaceProvidedProxy);
        CPPUNIT_TEST(TestRemoveInterfaceRequiredProxy);
        CPPUNIT_TEST(TestRemoveInterfaceProvidedProxy);
#endif
    }
    CPPUNIT_TEST_SUITE_END();

public:
    mtsManagerLocalTest();

    void setUp(void);
    void tearDown(void);

    void TestInitialize(void);
    void TestConstructor(void);
    void TestCleanup(void);

    void TestAddComponent(void);
    void TestFindComponent(void);
    void TestRemoveComponent(void);
    void TestRegisterInterfaces(void);

    void TestGetInstance(void);
    void TestGetComponent(void);
    void TestGetNamesOfComponents(void);
    void TestGetNamesOfTasks(void);
    void TestGetNamesOfDevices(void);
    void TestGetProcessName(void);

    void TestConnectLocally(void);
    void TestConnectDisconnect(void);

#if CISST_MTS_HAS_ICE
    void TestGetIPAddressList(void);
    void TestGetName(void);

    void TestConnectServerSideInterface(void);
    void TestCreateInterfaceRequiredProxy(void);
    void TestCreateInterfaceProvidedProxy(void);
    void TestRemoveInterfaceRequiredProxy(void);
    void TestRemoveInterfaceProvidedProxy(void);
#endif
};
