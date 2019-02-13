/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung, Anton Deguet
  Created on: 2009-11-17

  (C) Copyright 2009-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "mtsManagerLocalTest.h"

#include <cisstMultiTask/mtsManagerGlobal.h>
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
#if !CISST_MTS_HAS_ICE
    mtsManagerLocal::UnitTestNetworkProxyEnabled = false;
#else
    mtsManagerLocal::UnitTestNetworkProxyEnabled = true;
#endif
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
    mtsManagerLocal localManager;
    CPPUNIT_ASSERT_EQUAL(localManager.ProcessName, string(DEFAULT_PROCESS_NAME));
    CPPUNIT_ASSERT(localManager.ManagerGlobal);

    mtsManagerGlobal * GCM = dynamic_cast<mtsManagerGlobal*>(localManager.ManagerGlobal);
    CPPUNIT_ASSERT(GCM);

    CPPUNIT_ASSERT(GCM->FindProcess(localManager.ProcessName));
    CPPUNIT_ASSERT(GCM->GetProcessObject(localManager.ProcessName) == &localManager);
}

void mtsManagerLocalTest::TestCleanup(void)
{
    mtsManagerLocal managerLocal;

    CPPUNIT_ASSERT(managerLocal.ManagerGlobal);
    mtsTestDevice1<mtsInt> * dummy = new mtsTestDevice1<mtsInt>;
    CPPUNIT_ASSERT(managerLocal.ComponentMap.AddItem("dummy", dummy));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), managerLocal.ComponentMap.size());

    managerLocal.Cleanup();

    CPPUNIT_ASSERT(managerLocal.ManagerGlobal == 0);
    // Changed to 1 because size()==1, Cleanup does not remove items from ComponentMap...
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), managerLocal.ComponentMap.size());

    // Add __os_exit() test if needed.
}

void mtsManagerLocalTest::TestGetInstance(void)
{
    mtsManagerLocal * managerLocal = mtsManagerLocal::GetInstance();

    CPPUNIT_ASSERT(managerLocal);
    CPPUNIT_ASSERT(managerLocal->ManagerGlobal);
    CPPUNIT_ASSERT_EQUAL(managerLocal, mtsManagerLocal::Instance);
    CPPUNIT_ASSERT(managerLocal->ManagerGlobal->FindProcess(DEFAULT_PROCESS_NAME));
}

void mtsManagerLocalTest::TestAddComponent(void)
{
    mtsManagerLocal * localManager = mtsManagerLocal::GetInstance();
    localManager->RemoveAllUserComponents();  // Clean up from previous tests

    // Test with mtsComponent type components
    mtsTestDevice2<mtsInt> * device2 = new mtsTestDevice2<mtsInt>;

    // Invalid argument test
    CPPUNIT_ASSERT(!localManager->AddComponent(NULL));

    // Check with the global component manager.
    // Should fail if a component has already been registered before
    CPPUNIT_ASSERT(localManager->ManagerGlobal->AddComponent(DEFAULT_PROCESS_NAME, device2->GetName()));
    CPPUNIT_ASSERT(!localManager->AddComponent(device2));

    // Should succeed if a component is new
    CPPUNIT_ASSERT(localManager->ManagerGlobal->RemoveComponent(DEFAULT_PROCESS_NAME, device2->GetName()));
    CPPUNIT_ASSERT(localManager->AddComponent(device2));
    CPPUNIT_ASSERT(localManager->ComponentMap.FindItem(device2->GetName()));

    // Check if all the existing required interfaces and provided interfaces are
    // added to the global component manager.
    CPPUNIT_ASSERT(localManager->ManagerGlobal->FindInterfaceRequiredOrInput(DEFAULT_PROCESS_NAME, device2->GetName(), "r1"));
    CPPUNIT_ASSERT(localManager->ManagerGlobal->FindInterfaceProvidedOrOutput(DEFAULT_PROCESS_NAME, device2->GetName(), "p1"));
    CPPUNIT_ASSERT(localManager->ManagerGlobal->FindInterfaceProvidedOrOutput(DEFAULT_PROCESS_NAME, device2->GetName(), "p2"));

    // Not really necessary, but start with a clean slate.
    localManager->RemoveAllUserComponents();

    // Test with mtsTask type components
    mtsTestContinuous1<mtsInt> * continuous1 = new mtsTestContinuous1<mtsInt>;

    // Invalid argument test
    CPPUNIT_ASSERT(!localManager->AddComponent(NULL));

    // Check with the global component manager.
    // Should fail if a component to be added has already been registered before
    CPPUNIT_ASSERT(localManager->ManagerGlobal->AddComponent(DEFAULT_PROCESS_NAME, continuous1->GetName()));
    CPPUNIT_ASSERT(!localManager->AddComponent(continuous1));

    // Should succeed if a component is new
    CPPUNIT_ASSERT(localManager->ManagerGlobal->RemoveComponent(DEFAULT_PROCESS_NAME, continuous1->GetName()));
    CPPUNIT_ASSERT(localManager->AddComponent(continuous1));
    CPPUNIT_ASSERT(localManager->ComponentMap.FindItem(continuous1->GetName()));

    // Check if all the existing required interfaces and provided interfaces are
    // added to the global component manager.
    CPPUNIT_ASSERT(localManager->ManagerGlobal->FindInterfaceRequiredOrInput(DEFAULT_PROCESS_NAME, continuous1->GetName(), "r1"));
    CPPUNIT_ASSERT(localManager->ManagerGlobal->FindInterfaceProvidedOrOutput(DEFAULT_PROCESS_NAME, continuous1->GetName(), "p1"));
    CPPUNIT_ASSERT(localManager->ManagerGlobal->FindInterfaceProvidedOrOutput(DEFAULT_PROCESS_NAME, continuous1->GetName(), "p2"));
}

void mtsManagerLocalTest::TestFindComponent(void)
{
    mtsManagerLocal *localManager = mtsManagerLocal::GetInstance();
    localManager->RemoveAllUserComponents();  // Clean up from previous tests

    mtsTestDevice1<mtsInt> * device1 = new mtsTestDevice1<mtsInt>;
    const std::string componentName = device1->GetName();

    CPPUNIT_ASSERT(!localManager->FindComponent(componentName));
    CPPUNIT_ASSERT(localManager->AddComponent(device1));
    CPPUNIT_ASSERT(localManager->FindComponent(componentName));

    CPPUNIT_ASSERT(localManager->RemoveComponent(componentName));
    CPPUNIT_ASSERT(!localManager->FindComponent(componentName));
}

void mtsManagerLocalTest::TestRemoveComponent(void)
{
    mtsManagerLocal *localManager = mtsManagerLocal::GetInstance();
    localManager->RemoveAllUserComponents();  // Clean up from previous tests

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

    // Not really necessary, but start with a clean slate.
    localManager->RemoveAllUserComponents();

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
}

void mtsManagerLocalTest::TestRegisterInterfaces(void)
{
    mtsManagerLocal * localManager = mtsManagerLocal::GetInstance();
    localManager->RemoveAllUserComponents();  // Clean up from previous tests

    mtsManagerGlobal * globalManager = dynamic_cast<mtsManagerGlobal *>(localManager->ManagerGlobal);
    CPPUNIT_ASSERT(globalManager);

    mtsTestDevice2<mtsInt> * component = new mtsTestDevice2<mtsInt>;
    const std::string componentName = component->GetName();

    // Check initial values of GCM
    CPPUNIT_ASSERT(!globalManager->FindInterfaceRequiredOrInput("LCM", componentName, "r1"));
    CPPUNIT_ASSERT(!globalManager->FindInterfaceProvidedOrOutput("LCM", componentName, "p1"));
    CPPUNIT_ASSERT(!globalManager->FindInterfaceProvidedOrOutput("LCM", componentName, "p2"));
    // This should fail because no component is registered yet
    CPPUNIT_ASSERT(!localManager->RegisterInterfaces(component));

    // Add the component. This includes registration of interfaces that have been added so far.
    CPPUNIT_ASSERT(localManager->AddComponent(component));

    // Check updated values of GCM
    CPPUNIT_ASSERT(globalManager->FindInterfaceRequiredOrInput("LCM", componentName, "r1"));
    CPPUNIT_ASSERT(globalManager->FindInterfaceProvidedOrOutput("LCM", componentName, "p1"));
    CPPUNIT_ASSERT(globalManager->FindInterfaceProvidedOrOutput("LCM", componentName, "p2"));

    // Now, create a new required and provided interface which have not been added.
    mtsInterfaceRequired * requiredInterface = component->AddInterfaceRequired("newRequiredInterface");
    CPPUNIT_ASSERT(requiredInterface);
    mtsInterfaceProvided * providedInterface = component->AddInterfaceProvided("newProvidedInterface");
    CPPUNIT_ASSERT(providedInterface);

    // Check initial values of GCM
    CPPUNIT_ASSERT(!globalManager->FindInterfaceRequiredOrInput("LCM", componentName, requiredInterface->GetName()));
    CPPUNIT_ASSERT(!globalManager->FindInterfaceProvidedOrOutput("LCM", componentName, providedInterface->GetName()));

    // Register the new interfaces
    CPPUNIT_ASSERT(localManager->RegisterInterfaces(component));

    // Check updated values of GCM
    CPPUNIT_ASSERT(globalManager->FindInterfaceRequiredOrInput("LCM", componentName, requiredInterface->GetName()));
    CPPUNIT_ASSERT(globalManager->FindInterfaceProvidedOrOutput("LCM", componentName, providedInterface->GetName()));
}


void mtsManagerLocalTest::TestGetComponent(void)
{
    mtsManagerLocal * localManager = mtsManagerLocal::GetInstance();
    localManager->RemoveAllUserComponents();  // Clean up from previous tests

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
}


void mtsManagerLocalTest::TestGetNamesOfComponents(void)
{
    mtsManagerLocal * localManager = mtsManagerLocal::GetInstance();
    localManager->RemoveAllUserComponents();  // Clean up from previous tests

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
}


void mtsManagerLocalTest::TestGetProcessName(void)
{
    mtsManagerLocal localManager;
    CPPUNIT_ASSERT_EQUAL(localManager.GetProcessName(), std::string(DEFAULT_PROCESS_NAME));
}


void mtsManagerLocalTest::TestConnectDisconnect(void)
{
    // Local connection test
    mtsManagerLocal * localManager = mtsManagerLocal::GetInstance();
    localManager->RemoveAllUserComponents();  // Clean up from previous tests

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
}

void mtsManagerLocalTest::TestConnectLocally(void)
{
    mtsManagerLocal * localManager = mtsManagerLocal::GetInstance();
    localManager->RemoveAllUserComponents();  // Clean up from previous tests

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
}

#if CISST_MTS_HAS_ICE
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

void mtsManagerLocalTest::TestGetName(void)
{
}

void mtsManagerLocalTest::TestConnectServerSideInterface(void)
{
}

void mtsManagerLocalTest::TestCreateInterfaceRequiredProxy(void)
{
}

void mtsManagerLocalTest::TestCreateInterfaceProvidedProxy(void)
{
}

void mtsManagerLocalTest::TestRemoveInterfaceRequiredProxy(void)
{
}

void mtsManagerLocalTest::TestRemoveInterfaceProvidedProxy(void)
{
}
#endif

/*
    //
    // Remote connection test
    //

    // Test with invalid arguments.
    managerLocal.UnitTestEnabled = true; // run in unit test mode
    managerLocal.UnitTestNetworkProxyEnabled = false; // but disable network proxy processings
    CPPUNIT_ASSERT(!managerLocal.Connect(P1, C1, r1, P2, C2, p1));

    mtsManagerGlobal managerGlobal;

    // Prepare local managers for this test
    mtsTestDevice1<mtsInt> * P1C1 = new mtsTestDevice1<mtsInt>;
    mtsTestDevice2<mtsInt> * P1C2 = new mtsTestDevice2<mtsInt>;
    mtsTestDevice2<mtsInt> * P2C2 = new mtsTestDevice2<mtsInt>;
    mtsTestDevice3<mtsInt> * P2C3 = new mtsTestDevice3<mtsInt>;

    mtsManagerLocalInterface * managerLocal1 = new mtsManagerLocal(P1);
    mtsManagerLocal * managerLocal1Object = dynamic_cast<mtsManagerLocal*>(managerLocal1);
    managerLocal1Object->ManagerGlobal = &managerGlobal;
    managerGlobal.AddProcess(managerLocal1->GetProcessName());
    managerLocal1Object->AddComponent(P1C1);
    managerLocal1Object->AddComponent(P1C2);
    managerLocal1Object->UnitTestEnabled = true; // run in unit test mode
    managerLocal1Object->UnitTestNetworkProxyEnabled = true; // but disable network proxy processings

    mtsManagerLocalInterface * managerLocal2 = new mtsManagerLocal(P2);
    mtsManagerLocal * managerLocal2Object = dynamic_cast<mtsManagerLocal*>(managerLocal2);
    managerLocal2Object->ManagerGlobal = &managerGlobal;
    managerGlobal.AddProcess(managerLocal2->GetProcessName());
    managerLocal2Object->AddComponent(P2C2);
    managerLocal2Object->AddComponent(P2C3);
    managerLocal2Object->UnitTestEnabled = true; // run in unit test mode
    managerLocal2Object->UnitTestNetworkProxyEnabled = true; // but disable network proxy processings

    // Connecting two interfaces for the first time should success.
    CPPUNIT_ASSERT(managerLocal1Object->Connect(P1, C1, r1, P2, C2, p1));
    CPPUNIT_ASSERT(managerLocal1Object->Connect(P1, C1, r2, P2, C2, p2));
    CPPUNIT_ASSERT(managerLocal1Object->Connect(P1, C2, r1, P2, C2, p2));
    CPPUNIT_ASSERT(managerLocal2Object->Connect(P2, C3, r1, P2, C2, p2));

    // Connecting two interfaces that are already connected should fail.
    CPPUNIT_ASSERT(!managerLocal1Object->Connect(P1, C1, r1, P2, C2, p1));
    CPPUNIT_ASSERT(!managerLocal1Object->Connect(P1, C1, r2, P2, C2, p2));
    CPPUNIT_ASSERT(!managerLocal1Object->Connect(P1, C2, r1, P2, C2, p2));
    CPPUNIT_ASSERT(!managerLocal2Object->Connect(P2, C3, r1, P2, C2, p2));

    // Disconnect all the connections for the next tests
    CPPUNIT_ASSERT(managerLocal1Object->Disconnect(P1, C1, r1, P2, C2, p1));
    CPPUNIT_ASSERT(managerLocal1Object->Disconnect(P1, C1, r2, P2, C2, p2));
    CPPUNIT_ASSERT(managerLocal1Object->Disconnect(P1, C2, r1, P2, C2, p2));
    CPPUNIT_ASSERT(managerLocal2Object->Disconnect(P2, C3, r1, P2, C2, p2));

    // Disconnect should fail if disconnecting non-connected interfaces.
    CPPUNIT_ASSERT(!managerLocal1Object->Disconnect(P1, C1, r1, P2, C2, p1));
    CPPUNIT_ASSERT(!managerLocal1Object->Disconnect(P1, C1, r2, P2, C2, p2));
    CPPUNIT_ASSERT(!managerLocal1Object->Disconnect(P1, C2, r1, P2, C2, p2));
    CPPUNIT_ASSERT(!managerLocal2Object->Disconnect(P2, C3, r1, P2, C2, p2));

    //
    // TODO: After implementing proxy clean-up codes (WHEN DISCONNECT() IS CALLED),
    // enable the following tests!!!
    //
    return;

    // Connection should be established correctly regardless whoever calls Connect() method.
    CPPUNIT_ASSERT(managerLocal2Object->Connect(P1, C1, r1, P2, C2, p1));
    CPPUNIT_ASSERT(managerLocal2Object->Connect(P1, C1, r2, P2, C2, p2));
    CPPUNIT_ASSERT(managerLocal2Object->Connect(P1, C2, r1, P2, C2, p2));
*/

CPPUNIT_TEST_SUITE_REGISTRATION(mtsManagerLocalTest);
