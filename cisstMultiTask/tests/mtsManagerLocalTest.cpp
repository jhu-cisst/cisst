/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung, Anton Deguet
  Created on: 2009-11-17

  (C) Copyright 2009-2026 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "mtsManagerLocalTest.h"

#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsStateTable.h>

#include "mtsTestComponents.h"

#define P1 "P1"
#define P2 "P2"
#define P1_OBJ localManager1
#define P2_OBJ localManager2

#define DEFAULT_PROCESS_NAME "LCM"

using namespace std;

mtsManagerLocalTest::mtsManagerLocalTest()
{
}

void mtsManagerLocalTest::setUp(void)
{
    //localManager1 = new mtsManagerLocal();
    //localManager2 = new mtsManagerLocal();

    //localManager1->ProcessName = P1;
    //localManager2->ProcessName = P2;
}

void mtsManagerLocalTest::tearDown(void)
{
    //delete localManager1;
    //delete localManager2;
}

void mtsManagerLocalTest::TestInitialize(void)
{
    // Add __os_init() test if needed.
}

void mtsManagerLocalTest::TestConstructor(void)
{
    mtsManagerLocal * localManager = mtsManagerLocal::GetInstance();

    //CPPUNIT_ASSERT_EQUAL(localManager->ProcessName, string(DEFAULT_PROCESS_NAME));
    CPPUNIT_ASSERT_EQUAL(localManager->ProcessName, string());

    mtsManagerLocal::DeleteInstance();
}

void mtsManagerLocalTest::TestGetInstance(void)
{
    mtsManagerLocal * localManager = mtsManagerLocal::GetInstance();

    CPPUNIT_ASSERT(localManager);
    CPPUNIT_ASSERT_EQUAL(localManager, mtsManagerLocal::Instance);

    mtsManagerLocal::DeleteInstance();
}

void mtsManagerLocalTest::TestAddComponent(void)
{
    mtsManagerLocal * localManager = mtsManagerLocal::GetInstance();

    // Test with mtsComponent type components
    mtsTestDevice2<mtsInt> * device2 = new mtsTestDevice2<mtsInt>;

    // Invalid argument test
    CPPUNIT_ASSERT(!localManager->AddComponent(NULL));

    // Should succeed if a component is new
    CPPUNIT_ASSERT(localManager->AddComponent(device2));
    CPPUNIT_ASSERT(localManager->FindComponent(device2->GetName()));

    // Test with mtsTask type components
    mtsTestContinuous1<mtsInt> * continuous1 = new mtsTestContinuous1<mtsInt>;

    // Invalid argument test
    CPPUNIT_ASSERT(!localManager->AddComponent(NULL));

    // Should succeed if a component is new
    CPPUNIT_ASSERT(localManager->AddComponent(continuous1));
    CPPUNIT_ASSERT(localManager->FindComponent(continuous1->GetName()));

    mtsManagerLocal::DeleteInstance();
}

void mtsManagerLocalTest::TestFindComponent(void)
{
    mtsManagerLocal *localManager = mtsManagerLocal::GetInstance();

    mtsTestDevice1<mtsInt> * device1 = new mtsTestDevice1<mtsInt>;
    const std::string componentName = device1->GetName();

    CPPUNIT_ASSERT(!localManager->FindComponent(componentName));
    CPPUNIT_ASSERT(localManager->AddComponent(device1));
    CPPUNIT_ASSERT(localManager->FindComponent(componentName));

    CPPUNIT_ASSERT(localManager->RemoveComponent(componentName));
    CPPUNIT_ASSERT(!localManager->FindComponent(componentName));

    mtsManagerLocal::DeleteInstance();
}

void mtsManagerLocalTest::TestRemoveComponent(void)
{
    mtsManagerLocal *localManager = mtsManagerLocal::GetInstance();

    mtsTestDevice1<mtsInt> * device1 = new mtsTestDevice1<mtsInt>;
    const std::string componentName1 = device1->GetName();

    // Invalid argument test
    CPPUNIT_ASSERT(!localManager->RemoveComponent(NULL));
    CPPUNIT_ASSERT(!localManager->RemoveComponent("dummy"));

    CPPUNIT_ASSERT(localManager->AddComponent(device1));
    CPPUNIT_ASSERT(localManager->FindComponent(componentName1));
    CPPUNIT_ASSERT(localManager->RemoveComponent(componentName1));
    CPPUNIT_ASSERT(!localManager->FindComponent(componentName1));

    device1 = new mtsTestDevice1<mtsInt>;
    CPPUNIT_ASSERT(localManager->AddComponent(device1));
    CPPUNIT_ASSERT(localManager->FindComponent(componentName1));
    CPPUNIT_ASSERT(localManager->RemoveComponent(device1));
    CPPUNIT_ASSERT(!localManager->FindComponent(componentName1));

    mtsTestPeriodic1<mtsInt> * periodic1 = new mtsTestPeriodic1<mtsInt>;
    const std::string componentName2 = periodic1->GetName();

    CPPUNIT_ASSERT(localManager->AddComponent(periodic1));
    CPPUNIT_ASSERT(localManager->FindComponent(componentName2));
    CPPUNIT_ASSERT(localManager->RemoveComponent(componentName2));
    CPPUNIT_ASSERT(!localManager->FindComponent(componentName2));

    periodic1 = new mtsTestPeriodic1<mtsInt>;
    CPPUNIT_ASSERT(localManager->AddComponent(periodic1));
    CPPUNIT_ASSERT(localManager->FindComponent(componentName2));
    CPPUNIT_ASSERT(localManager->RemoveComponent(periodic1));
    CPPUNIT_ASSERT(!localManager->FindComponent(componentName2));

    mtsManagerLocal::DeleteInstance();
}

void mtsManagerLocalTest::TestGetComponent(void)
{
    mtsManagerLocal * localManager = mtsManagerLocal::GetInstance();

    mtsTestDevice1<mtsInt> * device1 = new mtsTestDevice1<mtsInt>;
    mtsTestDevice2<mtsInt> * device2 = new mtsTestDevice2<mtsInt>;
    mtsTestDevice3<mtsInt> * device3 = new mtsTestDevice3<mtsInt>;
    mtsComponent * nullComponent = 0;

    CPPUNIT_ASSERT_EQUAL(nullComponent, localManager->GetComponent(device1->GetName()));
    CPPUNIT_ASSERT_EQUAL(nullComponent, localManager->GetComponent(device2->GetName()));
    CPPUNIT_ASSERT_EQUAL(nullComponent, localManager->GetComponent(device3->GetName()));

    CPPUNIT_ASSERT(localManager->AddComponent(device1));
    CPPUNIT_ASSERT(localManager->AddComponent(device2));
    CPPUNIT_ASSERT(localManager->AddComponent(device3));

    CPPUNIT_ASSERT(device1 == localManager->GetComponent(device1->GetName()));
    CPPUNIT_ASSERT(device2 == localManager->GetComponent(device2->GetName()));
    CPPUNIT_ASSERT(device3 == localManager->GetComponent(device3->GetName()));

    mtsManagerLocal::DeleteInstance();
}


void mtsManagerLocalTest::TestGetNamesOfComponents(void)
{
    mtsManagerLocal * localManager = mtsManagerLocal::GetInstance();

    mtsTestDevice1<mtsInt> * device1 = new mtsTestDevice1<mtsInt>;
    mtsTestDevice2<mtsInt> * device2 = new mtsTestDevice2<mtsInt>;
    mtsTestDevice3<mtsInt> * device3 = new mtsTestDevice3<mtsInt>;

    CPPUNIT_ASSERT(localManager->AddComponent(device1));
    CPPUNIT_ASSERT(localManager->AddComponent(device2));
    CPPUNIT_ASSERT(localManager->AddComponent(device3));

    // return value
    std::vector<std::string> namesOfComponents1 = localManager->GetNamesOfComponents();
    bool found1 = false;
    bool found2 = false;
    bool found3 = false;
    CPPUNIT_ASSERT(namesOfComponents1.size() >= static_cast<size_t>(3));
    for (size_t i = 0; i < namesOfComponents1.size(); ++i) {
        if (namesOfComponents1[i] == device1->GetName()) {
            found1 = true;
        }
        else if (namesOfComponents1[i] == device2->GetName()) {
            found2 = true;
        }
        else if (namesOfComponents1[i] == device3->GetName()) {
            found3 = true;
        }
    }
    CPPUNIT_ASSERT(found1);
    CPPUNIT_ASSERT(found2);
    CPPUNIT_ASSERT(found3);

    // using placeholder
    std::vector<std::string> namesOfComponents2;
    localManager->GetNamesOfComponents(namesOfComponents2);
    found1 = false;
    found2 = false;
    found3 = false;
    CPPUNIT_ASSERT(namesOfComponents2.size() >= static_cast<size_t>(3));
    for (size_t i = 0; i < namesOfComponents2.size(); ++i) {
        if (namesOfComponents2[i] == device1->GetName()) {
            found1 = true;
        }
        else if (namesOfComponents2[i] == device2->GetName()) {
            found2 = true;
        }
        else if (namesOfComponents2[i] == device3->GetName()) {
            found3 = true;
        }
    }
    CPPUNIT_ASSERT(found1);
    CPPUNIT_ASSERT(found2);
    CPPUNIT_ASSERT(found3);

    mtsManagerLocal::DeleteInstance();
}


void mtsManagerLocalTest::TestGetProcessName(void)
{
    mtsManagerLocal * localManager = mtsManagerLocal::GetInstance();

    CPPUNIT_ASSERT_EQUAL(localManager->GetProcessName(), std::string(DEFAULT_PROCESS_NAME));

    mtsManagerLocal::DeleteInstance();
}


void mtsManagerLocalTest::TestConnectDisconnect(void)
{
    // Local connection test
    mtsManagerLocal * localManager = mtsManagerLocal::GetInstance();

    mtsTestPeriodic1<mtsInt> * periodic1 = new mtsTestPeriodic1<mtsInt>;
    mtsTestContinuous1<mtsInt> * continuous1 = new mtsTestContinuous1<mtsInt>;
    mtsTestFromCallback1<mtsInt> * fromCallback1 = new mtsTestFromCallback1<mtsInt>;
    mtsTestDevice2<mtsInt> * device2 = new mtsTestDevice2<mtsInt>;

    CPPUNIT_ASSERT(localManager->AddComponent(periodic1));
    CPPUNIT_ASSERT(localManager->AddComponent(continuous1));
    CPPUNIT_ASSERT(localManager->AddComponent(fromCallback1));
    CPPUNIT_ASSERT(localManager->AddComponent(device2));

    // Establish connections between the three components of mtsTask type
    CPPUNIT_ASSERT(localManager->Connect(periodic1->GetName(), "r1", continuous1->GetName(), "p1"));
    CPPUNIT_ASSERT(localManager->Connect(periodic1->GetName(), "r2", continuous1->GetName(), "p2"));
    CPPUNIT_ASSERT(localManager->Connect(device2->GetName(), "r1", continuous1->GetName(), "p2"));
    CPPUNIT_ASSERT(localManager->Connect(fromCallback1->GetName(), "r1", continuous1->GetName(), "p2"));

    // Should fail: already established connections
    CPPUNIT_ASSERT(!localManager->Connect(periodic1->GetName(), "r1", continuous1->GetName(), "p1"));
    CPPUNIT_ASSERT(!localManager->Connect(periodic1->GetName(), "r2", continuous1->GetName(), "p2"));
    CPPUNIT_ASSERT(!localManager->Connect(device2->GetName(), "r1", continuous1->GetName(), "p2"));
    CPPUNIT_ASSERT(!localManager->Connect(fromCallback1->GetName(), "r1", continuous1->GetName(), "p2"));

    // Disconnect all current connections
    CPPUNIT_ASSERT(localManager->Disconnect(periodic1->GetName(), "r1", continuous1->GetName(), "p1"));
    CPPUNIT_ASSERT(localManager->Disconnect(periodic1->GetName(), "r2", continuous1->GetName(), "p2"));
    CPPUNIT_ASSERT(localManager->Disconnect(device2->GetName(), "r1", continuous1->GetName(), "p2"));
    CPPUNIT_ASSERT(localManager->Disconnect(fromCallback1->GetName(), "r1", continuous1->GetName(), "p2"));

    // PK HACK: Disconnect currently does not wait for completion, so we need a rather long wait
    osaSleep(4.0);

    // Should succeed: new connections
    CPPUNIT_ASSERT(localManager->Connect(periodic1->GetName(), "r1", continuous1->GetName(), "p1"));
    CPPUNIT_ASSERT(localManager->Connect(periodic1->GetName(), "r2", continuous1->GetName(), "p2"));
    CPPUNIT_ASSERT(localManager->Connect(device2->GetName(), "r1", continuous1->GetName(), "p2"));
    CPPUNIT_ASSERT(localManager->Connect(fromCallback1->GetName(), "r1", continuous1->GetName(), "p2"));

    mtsManagerLocal::DeleteInstance();
}

void mtsManagerLocalTest::TestConnectLocally(void)
{
    mtsManagerLocal * localManager = mtsManagerLocal::GetInstance();

    mtsTestDevice1<mtsInt> * client = new mtsTestDevice1<mtsInt>;
    mtsTestDevice2<mtsInt> * server = new mtsTestDevice2<mtsInt>;

#define FAIL    false
#define SUCCESS true
    // test with invalid arguments
    CPPUNIT_ASSERT_EQUAL(FAIL, localManager->Connect("", "", "", ""));

    CPPUNIT_ASSERT(localManager->AddComponent(client));
    CPPUNIT_ASSERT_EQUAL(FAIL, localManager->Connect(client->GetName(), "", "", ""));

    CPPUNIT_ASSERT(localManager->AddComponent(server));
    CPPUNIT_ASSERT_EQUAL(FAIL, localManager->Connect(client->GetName(), "", server->GetName(), ""));

    CPPUNIT_ASSERT_EQUAL(FAIL, localManager->Connect(client->GetName(), "", server->GetName(), "p1"));

    CPPUNIT_ASSERT(client->GetInterfaceRequired("r1")->GetConnectedInterface() == 0);
    CPPUNIT_ASSERT_EQUAL(SUCCESS, localManager->Connect(client->GetName(), "r1", server->GetName(), "p1"));

    // Following CPPUNIT_ASSERT is not correct, since the provided interface name is specialized based on
    // the connected required interface; for example, it is called "p1[r1]" rather than just "p1".
    // CPPUNIT_ASSERT(client->GetInterfaceRequired("r1")->GetConnectedInterface() == server->GetInterfaceProvided("p1"));
    // Following is the correct implementation.
    const mtsInterfaceProvided *clientPtr = client->GetInterfaceRequired("r1")->GetConnectedInterface();
    CPPUNIT_ASSERT(clientPtr);
    const mtsInterfaceProvided *serverPtr = server->GetInterfaceProvided("p1");
    CPPUNIT_ASSERT(serverPtr);
    CPPUNIT_ASSERT_EQUAL(clientPtr->GetName(), mtsInterfaceProvided::GenerateEndUserInterfaceName(serverPtr, "r1"));

    mtsManagerLocal::DeleteInstance();
}

void mtsManagerLocalTest::TestGetIPAddressList(void)
{
    vector<string> ipList1, ipList2;
    ipList1 = mtsManagerLocal::GetIPAddressList();
    mtsManagerLocal::GetIPAddressList(ipList2);

    CPPUNIT_ASSERT_EQUAL(ipList1.size(), ipList2.size());
    for (size_t i = 0; i < ipList1.size(); ++i) {
        CPPUNIT_ASSERT_EQUAL(ipList1[i], ipList2[i]);
    }
}

CPPUNIT_TEST_SUITE_REGISTRATION(mtsManagerLocalTest);
