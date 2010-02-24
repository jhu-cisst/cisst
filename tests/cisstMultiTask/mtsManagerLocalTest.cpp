/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsManagerLocalTest.cpp 2009-03-05 mjung5 $
  
  Author(s):  Min Yang Jung
  Created on: 2009-11-17
  
  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

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

#include "mtsManagerTestClasses.h"

#define P1 "P1"
#define P2 "P2"
#define C1 "C1"
#define C2 "C2"
#define C3 "C3"
#define C4 "C4"
#define C1Task "C1Task"
#define C2Task "C2Task"
#define C3Task "C3Task"
#define C4Task "C4Task"
#define p1 "p1"
#define p2 "p2"
#define r1 "r1"
#define r2 "r2"
#define P1_OBJ localManager1
#define P2_OBJ localManager2

#define DEFAULT_PROCESS_NAME "LCM"

using namespace std;

mtsManagerLocalTest::mtsManagerLocalTest()
{
    mtsManagerLocal::UnitTestEnabled = true;
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
    mtsManagerTestC1Device * dummy = new mtsManagerTestC1Device;
    CPPUNIT_ASSERT(managerLocal.ComponentMap.AddItem("dummy", dummy));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), managerLocal.ComponentMap.size());
    
    managerLocal.Cleanup();

    CPPUNIT_ASSERT(managerLocal.ManagerGlobal == NULL);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), managerLocal.ComponentMap.size());

    // Add __os_exit() test if needed.
}

void mtsManagerLocalTest::TestGetInstance(void)
{
    mtsManagerLocal * managerLocal = mtsManagerLocal::GetInstance();

    CPPUNIT_ASSERT(managerLocal);
    CPPUNIT_ASSERT(managerLocal->ManagerGlobal);
    CPPUNIT_ASSERT(managerLocal == mtsManagerLocal::Instance);
    CPPUNIT_ASSERT(managerLocal->ManagerGlobal->FindProcess(DEFAULT_PROCESS_NAME));
}

void mtsManagerLocalTest::TestAddComponent(void)
{
    mtsManagerLocal localManager1;
    
    // Test with mtsDevice type components
    mtsManagerTestC2Device * c2Device = new mtsManagerTestC2Device;

    // Invalid argument test
    CPPUNIT_ASSERT(!localManager1.AddComponent(NULL));

    // Check with the global component manager.    
    // Should fail if a component has already been registered before
    CPPUNIT_ASSERT(localManager1.ManagerGlobal->AddComponent(DEFAULT_PROCESS_NAME, C2));
    CPPUNIT_ASSERT(!localManager1.AddComponent(c2Device));

    // Should succeed if a component is new
    CPPUNIT_ASSERT(localManager1.ManagerGlobal->RemoveComponent(DEFAULT_PROCESS_NAME, C2));
    CPPUNIT_ASSERT(localManager1.AddComponent(c2Device));
    CPPUNIT_ASSERT(localManager1.ComponentMap.FindItem(C2));
    
    // Check if all the existing required interfaces and provided interfaces are 
    // added to the global component manager.
    CPPUNIT_ASSERT(localManager1.ManagerGlobal->FindRequiredInterface(DEFAULT_PROCESS_NAME, C2, r1));
    CPPUNIT_ASSERT(localManager1.ManagerGlobal->FindProvidedInterface(DEFAULT_PROCESS_NAME, C2, p1));
    CPPUNIT_ASSERT(localManager1.ManagerGlobal->FindProvidedInterface(DEFAULT_PROCESS_NAME, C2, p2));

    mtsManagerLocal localManager2;

    // Test with mtsTask type components
    mtsManagerTestC2 * c2Task = new mtsManagerTestC2;

    // Invalid argument test
    CPPUNIT_ASSERT(!localManager2.AddComponent(NULL));

    // Check with the global component manager.    
    // Should fail if a component to be added has already been registered before
    CPPUNIT_ASSERT(localManager2.ManagerGlobal->AddComponent(DEFAULT_PROCESS_NAME, C2Task));
    CPPUNIT_ASSERT(!localManager2.AddComponent(c2Task));

    // Should succeed if a component is new
    CPPUNIT_ASSERT(localManager2.ManagerGlobal->RemoveComponent(DEFAULT_PROCESS_NAME, C2Task));
    CPPUNIT_ASSERT(localManager2.AddComponent(c2Task));
    CPPUNIT_ASSERT(localManager2.ComponentMap.FindItem(C2Task));
    
    // Check if all the existing required interfaces and provided interfaces are 
    // added to the global component manager.
    CPPUNIT_ASSERT(localManager2.ManagerGlobal->FindRequiredInterface(DEFAULT_PROCESS_NAME, C2Task, r1));
    CPPUNIT_ASSERT(localManager2.ManagerGlobal->FindProvidedInterface(DEFAULT_PROCESS_NAME, C2Task, p1));
    CPPUNIT_ASSERT(localManager2.ManagerGlobal->FindProvidedInterface(DEFAULT_PROCESS_NAME, C2Task, p2));
}

void mtsManagerLocalTest::TestFindComponent(void)
{
    mtsManagerLocal localManager1;
    mtsManagerTestC1Device * c1Device = new mtsManagerTestC1Device;
    const std::string componentName = c1Device->GetName();

    CPPUNIT_ASSERT(!localManager1.FindComponent(componentName));
    CPPUNIT_ASSERT(localManager1.AddComponent(c1Device));
    CPPUNIT_ASSERT(localManager1.FindComponent(componentName));

    CPPUNIT_ASSERT(localManager1.RemoveComponent(componentName));
    CPPUNIT_ASSERT(!localManager1.FindComponent(componentName));
}

void mtsManagerLocalTest::TestRemoveComponent(void)
{
    // Test with mtsDevice type components
    mtsManagerLocal localManager1;
    mtsManagerTestC1Device * c1Device = new mtsManagerTestC1Device;
    const std::string componentName1 = c1Device->GetName();

    // Invalid argument test
    CPPUNIT_ASSERT(!localManager1.RemoveComponent(NULL));
    CPPUNIT_ASSERT(!localManager1.RemoveComponent("dummy"));

    CPPUNIT_ASSERT(localManager1.AddComponent(c1Device));
    CPPUNIT_ASSERT(localManager1.FindComponent(componentName1));
    CPPUNIT_ASSERT(localManager1.RemoveComponent(componentName1));
    CPPUNIT_ASSERT(!localManager1.FindComponent(componentName1));

    c1Device = new mtsManagerTestC1Device;
    CPPUNIT_ASSERT(localManager1.AddComponent(c1Device));
    CPPUNIT_ASSERT(localManager1.FindComponent(componentName1));
    CPPUNIT_ASSERT(localManager1.RemoveComponent(c1Device));
    CPPUNIT_ASSERT(!localManager1.FindComponent(componentName1));

    // Test with mtsDevice type components
    mtsManagerLocal localManager2;
    mtsManagerTestC1 * c1Task = new mtsManagerTestC1;
    const std::string componentName2 = c1Task->GetName();

    CPPUNIT_ASSERT(localManager2.AddComponent(c1Task));
    CPPUNIT_ASSERT(localManager2.FindComponent(componentName2));
    CPPUNIT_ASSERT(localManager2.RemoveComponent(componentName2));
    CPPUNIT_ASSERT(!localManager2.FindComponent(componentName2));

    c1Task = new mtsManagerTestC1;
    CPPUNIT_ASSERT(localManager2.AddComponent(c1Task));
    CPPUNIT_ASSERT(localManager2.FindComponent(componentName2));
    CPPUNIT_ASSERT(localManager2.RemoveComponent(c1Task));
    CPPUNIT_ASSERT(!localManager2.FindComponent(componentName2));
}

void mtsManagerLocalTest::TestGetComponent(void)
{
    mtsManagerLocal localManager;
    mtsManagerTestC1Device * c1Device = new mtsManagerTestC1Device;
    mtsManagerTestC2Device * c2Device = new mtsManagerTestC2Device;
    mtsManagerTestC3Device * c3Device = new mtsManagerTestC3Device;

    CPPUNIT_ASSERT(NULL == localManager.GetComponent(C1));
    CPPUNIT_ASSERT(NULL == localManager.GetComponent(C2));
    CPPUNIT_ASSERT(NULL == localManager.GetComponent(C3));

    CPPUNIT_ASSERT(localManager.AddComponent(c1Device));
    CPPUNIT_ASSERT(localManager.AddComponent(c2Device));
    CPPUNIT_ASSERT(localManager.AddComponent(c3Device));

    CPPUNIT_ASSERT(c1Device == localManager.GetComponent(C1));
    CPPUNIT_ASSERT(c2Device == localManager.GetComponent(C2));
    CPPUNIT_ASSERT(c3Device == localManager.GetComponent(C3));
}

void mtsManagerLocalTest::TestGetNamesOfComponents(void)
{
    mtsManagerLocal localManager;
    mtsManagerTestC1Device * c1Device = new mtsManagerTestC1Device;
    mtsManagerTestC2Device * c2Device = new mtsManagerTestC2Device;
    mtsManagerTestC3Device * c3Device = new mtsManagerTestC3Device;

    CPPUNIT_ASSERT(localManager.AddComponent(c1Device));
    CPPUNIT_ASSERT(localManager.AddComponent(c2Device));
    CPPUNIT_ASSERT(localManager.AddComponent(c3Device));

    std::vector<std::string> namesOfComponents1 = localManager.GetNamesOfComponents();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), namesOfComponents1.size());
    for (int i = 0; i < 3; ++i) {
        CPPUNIT_ASSERT(namesOfComponents1[i] == c1Device->GetName() ||
                       namesOfComponents1[i] == c2Device->GetName() ||
                       namesOfComponents1[i] == c3Device->GetName());
    }

    std::vector<std::string> namesOfComponents2;
    localManager.GetNamesOfComponents(namesOfComponents2);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), namesOfComponents2.size());
    for (int i = 0; i < 3; ++i) {
        CPPUNIT_ASSERT(namesOfComponents2[i] == c1Device->GetName() ||
                       namesOfComponents2[i] == c2Device->GetName() ||
                       namesOfComponents2[i] == c3Device->GetName());
    }
}

void mtsManagerLocalTest::TestGetNamesOfTasks(void)
{
    mtsManagerLocal localManager;
    mtsManagerTestC1Device * c1Device = new mtsManagerTestC1Device;
    mtsManagerTestC2Device * c2Device = new mtsManagerTestC2Device;
    mtsManagerTestC1 * c1Task = new mtsManagerTestC1;
    mtsManagerTestC2 * c2Task = new mtsManagerTestC2;

    CPPUNIT_ASSERT(localManager.AddComponent(c1Device));
    CPPUNIT_ASSERT(localManager.AddComponent(c2Device));
    CPPUNIT_ASSERT(localManager.AddComponent(c1Task));
    CPPUNIT_ASSERT(localManager.AddComponent(c2Task));

    std::vector<std::string> namesOfTasks1 = localManager.GetNamesOfTasks();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), namesOfTasks1.size());
    for (int i = 0; i < 2; ++i) {
        CPPUNIT_ASSERT(namesOfTasks1[i] == c1Task->GetName() ||
                       namesOfTasks1[i] == c2Task->GetName());
    }

    std::vector<std::string> namesOfTasks2;
    localManager.GetNamesOfTasks(namesOfTasks2);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), namesOfTasks2.size());
    for (int i = 0; i < 2; ++i) {
        CPPUNIT_ASSERT(namesOfTasks2[i] == c1Task->GetName() ||
                       namesOfTasks2[i] == c2Task->GetName());
    }
}

void mtsManagerLocalTest::TestGetNamesOfDevices(void)
{
    mtsManagerLocal localManager;
    mtsManagerTestC1Device * c1Device = new mtsManagerTestC1Device;
    mtsManagerTestC2Device * c2Device = new mtsManagerTestC2Device;
    mtsManagerTestC1 * c1Task = new mtsManagerTestC1;
    mtsManagerTestC2 * c2Task = new mtsManagerTestC2;

    CPPUNIT_ASSERT(localManager.AddComponent(c1Device));
    CPPUNIT_ASSERT(localManager.AddComponent(c2Device));
    CPPUNIT_ASSERT(localManager.AddComponent(c1Task));
    CPPUNIT_ASSERT(localManager.AddComponent(c2Task));

    std::vector<std::string> namesOfDevices1 = localManager.GetNamesOfDevices();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), namesOfDevices1.size());
    for (int i = 0; i < 2; ++i) {
        CPPUNIT_ASSERT(namesOfDevices1[i] == c1Device->GetName() ||
                       namesOfDevices1[i] == c2Device->GetName());
    }

    std::vector<std::string> namesOfDevices2;
    localManager.GetNamesOfDevices(namesOfDevices2);
                         CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), namesOfDevices2.size());
    for (int i = 0; i < 2; ++i) {
        CPPUNIT_ASSERT(namesOfDevices2[i] == c1Device->GetName() ||
                       namesOfDevices2[i] == c2Device->GetName());
    }
}

void mtsManagerLocalTest::TestGetTimeServer(void)
{
    mtsManagerLocal localManager;
    CPPUNIT_ASSERT(&localManager.GetTimeServer() == &localManager.TimeServer);
}

void mtsManagerLocalTest::TestGetProcessName(void)
{
    mtsManagerLocal localManager;
    CPPUNIT_ASSERT(localManager.GetProcessName() == DEFAULT_PROCESS_NAME);
}


void mtsManagerLocalTest::TestCreateAll(void)
{
    mtsManagerLocal localManager;
    mtsManagerTestC1 * c1Task = new mtsManagerTestC1;    // C1 is of mtsTaskPeriodic type
    mtsManagerTestC2 * c2Task = new mtsManagerTestC2;    // C2 is of mtsTaskContinuous type
    mtsManagerTestC3 * c3Task = new mtsManagerTestC3;    // C3 is of mtsTaskCallback type

    CPPUNIT_ASSERT(localManager.AddComponent(c1Task));
    CPPUNIT_ASSERT(localManager.AddComponent(c2Task));
    CPPUNIT_ASSERT(localManager.AddComponent(c3Task));
    
    // Check internal states before calling CreateAll()
    CPPUNIT_ASSERT_EQUAL(mtsTask::CONSTRUCTED, c1Task->TaskState);
    CPPUNIT_ASSERT_EQUAL(mtsTask::CONSTRUCTED, c2Task->TaskState);
    CPPUNIT_ASSERT_EQUAL(mtsTask::CONSTRUCTED, c3Task->TaskState);

    localManager.CreateAll();

    //CPPUNIT_ASSERT_EQUAL(mtsTask::INITIALIZING, c1Task->TaskState); // TODO: Resolve this test
    CPPUNIT_ASSERT_EQUAL(mtsTask::INITIALIZING, c2Task->TaskState);
    CPPUNIT_ASSERT_EQUAL(mtsTask::INITIALIZING, c3Task->TaskState);

    localManager.KillAll();
}

void mtsManagerLocalTest::TestStartAll(void)
{
    mtsManagerLocal localManager;
    mtsManagerTestC1 * c1Task = new mtsManagerTestC1;    // C1 is of mtsTaskPeriodic type
    mtsManagerTestC2 * c2Task = new mtsManagerTestC2;    // C2 is of mtsTaskContinuous type
    mtsManagerTestC3 * c3Task = new mtsManagerTestC3;    // C3 is of mtsTaskCallback type
    mtsManagerTestC2Device * c2Device = new mtsManagerTestC2Device;

    CPPUNIT_ASSERT(localManager.AddComponent(c1Task));
    CPPUNIT_ASSERT(localManager.AddComponent(c2Task));
    CPPUNIT_ASSERT(localManager.AddComponent(c3Task));
    CPPUNIT_ASSERT(localManager.AddComponent(c2Device));

    // Establish connections between the three components of mtsTask type
    // Connection: (C1, r1) ~ (C2, p1)
    CPPUNIT_ASSERT(localManager.Connect(C1Task, r1, C2Task, p1));
    // Connection: (C1, r2) ~ (C2, p2)
    CPPUNIT_ASSERT(localManager.Connect(C1Task, r2, C2Task, p2));
    // Connection: (C2, r1) ~ (C2, p2)
    CPPUNIT_ASSERT(localManager.Connect(C2, r1, C2Task, p2));
    // Connection: (C3, r1) ~ (C2, p2)
    CPPUNIT_ASSERT(localManager.Connect(C3Task, r1, C2Task, p2));
    
    localManager.CreateAll();

    //CPPUNIT_ASSERT_EQUAL(mtsTask::INITIALIZING, c1Task->TaskState); // TODO: Resolve this test
    CPPUNIT_ASSERT_EQUAL(mtsTask::INITIALIZING, c2Task->TaskState);
    CPPUNIT_ASSERT_EQUAL(mtsTask::INITIALIZING, c3Task->TaskState);

    localManager.StartAll();
    osaSleep(1 * cmn_s);

    CPPUNIT_ASSERT_EQUAL(mtsTask::ACTIVE, c1Task->TaskState);
    //CPPUNIT_ASSERT_EQUAL(mtsTask::ACTIVE, c2Task->TaskState); // TODO: Resolve this test
    CPPUNIT_ASSERT_EQUAL(mtsTask::INITIALIZING, c3Task->TaskState);

    localManager.KillAll();
    osaSleep(1 * cmn_ms);
}

void mtsManagerLocalTest::TestKillAll(void)
{
    mtsManagerLocal localManager;
    mtsManagerTestC1 * c1Task = new mtsManagerTestC1;    // C1 is of mtsTaskPeriodic type
    mtsManagerTestC2 * c2Task = new mtsManagerTestC2;    // C2 is of mtsTaskContinuous type
    mtsManagerTestC3 * c3Task = new mtsManagerTestC3;    // C3 is of mtsTaskCallback type
    mtsManagerTestC2Device * c2Device = new mtsManagerTestC2Device;

    CPPUNIT_ASSERT(localManager.AddComponent(c1Task));
    CPPUNIT_ASSERT(localManager.AddComponent(c2Task));
    CPPUNIT_ASSERT(localManager.AddComponent(c3Task));
    CPPUNIT_ASSERT(localManager.AddComponent(c2Device));

    // Establish connections between the three components of mtsTask type
    // Connection: (C1, r1) ~ (C2, p1)
    CPPUNIT_ASSERT(localManager.Connect(C1Task, r1, C2Task, p1));
    // Connection: (C1, r2) ~ (C2, p2)
    CPPUNIT_ASSERT(localManager.Connect(C1Task, r2, C2Task, p2));
    // Connection: (C2, r1) ~ (C2, p2)
    CPPUNIT_ASSERT(localManager.Connect(C2, r1, C2Task, p2));
    // Connection: (C3, r1) ~ (C2, p2)
    CPPUNIT_ASSERT(localManager.Connect(C3Task, r1, C2Task, p2));
    
    localManager.CreateAll();
    localManager.StartAll();
    osaSleep(1 * cmn_ms);

    localManager.KillAll();
    osaSleep(1 * cmn_ms);

    CPPUNIT_ASSERT(c1Task->TaskState == mtsTask::FINISHING || 
                   c1Task->TaskState == mtsTask::FINISHED);
    CPPUNIT_ASSERT(c2Task->TaskState == mtsTask::FINISHING || 
                   c2Task->TaskState == mtsTask::FINISHED);
    CPPUNIT_ASSERT(c3Task->TaskState == mtsTask::FINISHING || 
                   c3Task->TaskState == mtsTask::FINISHED);
}

void mtsManagerLocalTest::TestConnectDisconnect(void)
{
    // Local connection test
    mtsManagerLocal localManager;
    mtsManagerTestC1 * c1Task = new mtsManagerTestC1;    // C1 is of mtsTaskPeriodic type
    mtsManagerTestC2 * c2Task = new mtsManagerTestC2;    // C2 is of mtsTaskContinuous type
    mtsManagerTestC3 * c3Task = new mtsManagerTestC3;    // C3 is of mtsTaskCallback type
    mtsManagerTestC2Device * c2Device = new mtsManagerTestC2Device;

    CPPUNIT_ASSERT(localManager.AddComponent(c1Task));
    CPPUNIT_ASSERT(localManager.AddComponent(c2Task));
    CPPUNIT_ASSERT(localManager.AddComponent(c3Task));
    CPPUNIT_ASSERT(localManager.AddComponent(c2Device));

    // Establish connections between the three components of mtsTask type
    // Connection: (C1, r1) ~ (C2, p1)
    CPPUNIT_ASSERT(localManager.Connect(C1Task, r1, C2Task, p1));
    // Connection: (C1, r2) ~ (C2, p2)
    CPPUNIT_ASSERT(localManager.Connect(C1Task, r2, C2Task, p2));
    // Connection: (C2, r1) ~ (C2, p2)
    CPPUNIT_ASSERT(localManager.Connect(C2, r1, C2Task, p2));
    // Connection: (C3, r1) ~ (C2, p2)
    CPPUNIT_ASSERT(localManager.Connect(C3Task, r1, C2Task, p2));

    // Should fail: already established connections
    CPPUNIT_ASSERT(!localManager.Connect(C1Task, r1, C2Task, p1));
    CPPUNIT_ASSERT(!localManager.Connect(C1Task, r2, C2Task, p2));
    CPPUNIT_ASSERT(!localManager.Connect(C2, r1, C2Task, p2));
    CPPUNIT_ASSERT(!localManager.Connect(C3Task, r1, C2Task, p2));

    // Disconnect all current connections
    CPPUNIT_ASSERT(localManager.Disconnect(C1Task, r1, C2Task, p1));
    CPPUNIT_ASSERT(localManager.Disconnect(C1Task, r2, C2Task, p2));
    CPPUNIT_ASSERT(localManager.Disconnect(C2, r1, C2Task, p2));
    CPPUNIT_ASSERT(localManager.Disconnect(C3Task, r1, C2Task, p2));

    // Should success: new connections
    CPPUNIT_ASSERT(localManager.Connect(C1Task, r1, C2Task, p1));
    CPPUNIT_ASSERT(localManager.Connect(C1Task, r2, C2Task, p2));
    CPPUNIT_ASSERT(localManager.Connect(C2, r1, C2Task, p2));
    CPPUNIT_ASSERT(localManager.Connect(C3Task, r1, C2Task, p2));
}

void mtsManagerLocalTest::TestConnectLocally(void)
{
    mtsManagerLocal localManager;
    mtsManagerTestC1Device * client = new mtsManagerTestC1Device;
    mtsManagerTestC2Device * server = new mtsManagerTestC2Device;

#define FAIL -1
#define SUCCESS 0
    // test with invalid arguments
    CPPUNIT_ASSERT_EQUAL(FAIL, localManager.ConnectLocally("", "", "", ""));
    
    CPPUNIT_ASSERT(localManager.AddComponent(client));
    CPPUNIT_ASSERT_EQUAL(FAIL, localManager.ConnectLocally(client->GetName(), "", "", ""));

    CPPUNIT_ASSERT(localManager.AddComponent(server));
    CPPUNIT_ASSERT_EQUAL(FAIL, localManager.ConnectLocally(client->GetName(), "", server->GetName(), ""));

    CPPUNIT_ASSERT_EQUAL(FAIL, localManager.ConnectLocally(client->GetName(), "", server->GetName(), p1));

    CPPUNIT_ASSERT(client->GetRequiredInterface(r1)->OtherInterface == NULL);
    CPPUNIT_ASSERT_EQUAL(SUCCESS, localManager.ConnectLocally(client->GetName(), r1, server->GetName(), p1));
    CPPUNIT_ASSERT(client->GetRequiredInterface(r1)->OtherInterface == server->GetProvidedInterface(p1));
}

#if CISST_MTS_HAS_ICE
void mtsManagerLocalTest::TestGetIPAddressList(void)
{
    vector<string> ipList1, ipList2;
    ipList1 = mtsManagerLocal::GetIPAddressList();
    mtsManagerLocal::GetIPAddressList(ipList2);

    CPPUNIT_ASSERT(ipList1.size() == ipList2.size());
    for (unsigned int i = 0; i < ipList1.size(); ++i)
        CPPUNIT_ASSERT(ipList1[i] == ipList2[i]);
}

void mtsManagerLocalTest::TestGetName(void)
{
}

void mtsManagerLocalTest::TestConnectServerSideInterface(void)
{
}

void mtsManagerLocalTest::TestCreateRequiredInterfaceProxy(void)
{
}

void mtsManagerLocalTest::TestCreateProvidedInterfaceProxy(void)
{
}

void mtsManagerLocalTest::TestRemoveRequiredInterfaceProxy(void)
{
}

void mtsManagerLocalTest::TestRemoveProvidedInterfaceProxy(void)
{
}

void mtsManagerLocalTest::TestRemoteCommandsAndEvents(void)
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
    mtsManagerTestC1Device * P1C1 = new mtsManagerTestC1Device;
    mtsManagerTestC2Device * P1C2 = new mtsManagerTestC2Device;
    mtsManagerTestC2Device * P2C2 = new mtsManagerTestC2Device;
    mtsManagerTestC3Device * P2C3 = new mtsManagerTestC3Device;

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

void mtsManagerLocalTest::TestLocalCommandsAndEvents(void)
{
    mtsManagerLocal localManager;

    mtsManagerTestC2Device * P2C2 = new mtsManagerTestC2Device;
    mtsManagerTestC3Device * P2C3 = new mtsManagerTestC3Device;
    CPPUNIT_ASSERT(localManager.AddComponent(P2C2));
    CPPUNIT_ASSERT(localManager.AddComponent(P2C3));
    
    // Connect two interfaces (establish local connection) and test if commands 
    // and events work correctly.
    CPPUNIT_ASSERT(localManager.Connect(C3, r1, C2, p2));

    // Check initial values
    CPPUNIT_ASSERT_EQUAL(-1, P2C2->RequiredInterface1.GetValue());
    CPPUNIT_ASSERT_EQUAL(-1, P2C2->ProvidedInterface1.GetValue());
    CPPUNIT_ASSERT_EQUAL(-1, P2C2->ProvidedInterface2.GetValue());
    CPPUNIT_ASSERT_EQUAL(-1, P2C3->RequiredInterface1.GetValue());

    // Test void command
    P2C3->RequiredInterface1.CommandVoid();
    CPPUNIT_ASSERT_EQUAL(-1, P2C2->RequiredInterface1.GetValue());
    CPPUNIT_ASSERT_EQUAL(-1, P2C2->ProvidedInterface1.GetValue());
    CPPUNIT_ASSERT_EQUAL(0,  P2C2->ProvidedInterface2.GetValue());
    CPPUNIT_ASSERT_EQUAL(-1, P2C3->RequiredInterface1.GetValue());

    // Test write command
    mtsInt valueWrite;
    valueWrite.Data = 2;
    P2C3->RequiredInterface1.CommandWrite(valueWrite);
    CPPUNIT_ASSERT_EQUAL(-1, P2C2->RequiredInterface1.GetValue());
    CPPUNIT_ASSERT_EQUAL(-1, P2C2->ProvidedInterface1.GetValue());
    CPPUNIT_ASSERT_EQUAL(valueWrite.Data,  P2C2->ProvidedInterface2.GetValue());
    CPPUNIT_ASSERT_EQUAL(-1, P2C3->RequiredInterface1.GetValue());

    // Test read command
    mtsInt valueRead;
    valueRead.Data = 0;
    P2C3->RequiredInterface1.CommandRead(valueRead);
    CPPUNIT_ASSERT_EQUAL(valueWrite.Data, valueRead.Data);
    CPPUNIT_ASSERT_EQUAL(-1, P2C2->RequiredInterface1.GetValue());
    CPPUNIT_ASSERT_EQUAL(-1, P2C2->ProvidedInterface1.GetValue());
    CPPUNIT_ASSERT_EQUAL(valueWrite.Data,  P2C2->ProvidedInterface2.GetValue());
    CPPUNIT_ASSERT_EQUAL(-1, P2C3->RequiredInterface1.GetValue());    

    // Test qualified read command
    valueWrite.Data = 3;
    valueRead.Data = 0;
    P2C3->RequiredInterface1.CommandQualifiedRead(valueWrite, valueRead);
    CPPUNIT_ASSERT_EQUAL(-1, P2C2->RequiredInterface1.GetValue());
    CPPUNIT_ASSERT_EQUAL(-1, P2C2->ProvidedInterface1.GetValue());
    CPPUNIT_ASSERT_EQUAL(-1, P2C3->RequiredInterface1.GetValue());    

    // Test void event
    P2C2->ProvidedInterface2.EventVoid();
    CPPUNIT_ASSERT_EQUAL(-1, P2C2->RequiredInterface1.GetValue());
    CPPUNIT_ASSERT_EQUAL(-1, P2C2->ProvidedInterface1.GetValue());
    CPPUNIT_ASSERT_EQUAL(0, P2C3->RequiredInterface1.GetValue());

    // Test write event
    valueWrite.Data = 4;
    P2C2->ProvidedInterface2.EventWrite(valueWrite);
    CPPUNIT_ASSERT_EQUAL(-1, P2C2->RequiredInterface1.GetValue());
    CPPUNIT_ASSERT_EQUAL(-1, P2C2->ProvidedInterface1.GetValue());
    CPPUNIT_ASSERT_EQUAL(valueWrite.Data, P2C3->RequiredInterface1.GetValue());
}

/*
void mtsManagerLocalTest::TestRemoteCommandsAndEvents(void)
{
    mtsManagerGlobal managerGlobal;

    // Prepare local managers for this test
    mtsManagerTestC1Device * P1C1 = new mtsManagerTestC1Device;
    mtsManagerTestC2Device * P1C2 = new mtsManagerTestC2Device;
    mtsManagerTestC2Device * P2C2 = new mtsManagerTestC2Device;
    mtsManagerTestC3Device * P2C3 = new mtsManagerTestC3Device;

    mtsManagerLocalInterface * managerLocal1 = new mtsManagerLocal(P1);
    mtsManagerLocal * managerLocal1Object = dynamic_cast<mtsManagerLocal*>(managerLocal1);
    managerLocal1Object->ManagerGlobal = &managerGlobal;
    managerGlobal.AddProcess(managerLocal1->GetProcessName());
    managerLocal1Object->AddComponent(P1C1);
    managerLocal1Object->AddComponent(P1C2);
    managerLocal1Object->UnitTestEnabled = true; // run in unit test mode
    managerLocal1Object->UnitTestNetworkProxyEnabled = false; // but disable network proxy processings

    mtsManagerLocalInterface * managerLocal2 = new mtsManagerLocal(P2);
    mtsManagerLocal * managerLocal2Object = dynamic_cast<mtsManagerLocal*>(managerLocal2);
    managerLocal2Object->ManagerGlobal = &managerGlobal;
    managerGlobal.AddProcess(managerLocal2->GetProcessName());
    managerLocal2Object->AddComponent(P2C2);
    managerLocal2Object->AddComponent(P2C3);
    managerLocal2Object->UnitTestEnabled = true; // run in unit test mode
    managerLocal2Object->UnitTestNetworkProxyEnabled = false; // but disable network proxy processings

    // Connect two interfaces (establish remote connection) and test if commands
    // and events work correctly.
    CPPUNIT_ASSERT(managerLocal1Object->Connect(P1, C1, r1, P2, C2, p1));

    // Check initial values
    CPPUNIT_ASSERT_EQUAL(-1, P2C2->RequiredInterface1.GetValue());
    CPPUNIT_ASSERT_EQUAL(-1, P2C2->ProvidedInterface1.GetValue());
    CPPUNIT_ASSERT_EQUAL(-1, P2C2->ProvidedInterface2.GetValue());
    CPPUNIT_ASSERT_EQUAL(-1, P2C3->RequiredInterface1.GetValue());

    // Test void command
    P2C3->RequiredInterface1.CommandVoid();
    //CPPUNIT_ASSERT_EQUAL(-1, P2C2->RequiredInterface1.GetValue());
    //CPPUNIT_ASSERT_EQUAL(-1, P2C2->ProvidedInterface1.GetValue());
    //CPPUNIT_ASSERT_EQUAL(0,  P2C2->ProvidedInterface2.GetValue());
    //CPPUNIT_ASSERT_EQUAL(-1, P2C3->RequiredInterface1.GetValue());

    // Test write command
    //mtsInt valueWrite;
    //valueWrite.Data = 2;
    //P2C3->RequiredInterface1.CommandWrite(valueWrite);
    //CPPUNIT_ASSERT_EQUAL(-1, P2C2->RequiredInterface1.GetValue());
    //CPPUNIT_ASSERT_EQUAL(-1, P2C2->ProvidedInterface1.GetValue());
    //CPPUNIT_ASSERT_EQUAL(valueWrite.Data,  P2C2->ProvidedInterface2.GetValue());
    //CPPUNIT_ASSERT_EQUAL(-1, P2C3->RequiredInterface1.GetValue());

    //// Test read command
    //mtsInt valueRead;
    //valueRead.Data = 0;
    //P2C3->RequiredInterface1.CommandRead(valueRead);
    //CPPUNIT_ASSERT_EQUAL(valueWrite.Data, valueRead.Data);
    //CPPUNIT_ASSERT_EQUAL(-1, P2C2->RequiredInterface1.GetValue());
    //CPPUNIT_ASSERT_EQUAL(-1, P2C2->ProvidedInterface1.GetValue());
    //CPPUNIT_ASSERT_EQUAL(valueWrite.Data,  P2C2->ProvidedInterface2.GetValue());
    //CPPUNIT_ASSERT_EQUAL(-1, P2C3->RequiredInterface1.GetValue());    

    //// Test qualified read command
    //valueWrite.Data = 3;
    //valueRead.Data = 0;
    //P2C3->RequiredInterface1.CommandQualifiedRead(valueWrite, valueRead);
    //CPPUNIT_ASSERT_EQUAL(-1, P2C2->RequiredInterface1.GetValue());
    //CPPUNIT_ASSERT_EQUAL(-1, P2C2->ProvidedInterface1.GetValue());
    //CPPUNIT_ASSERT_EQUAL(-1, P2C3->RequiredInterface1.GetValue());    

    //// Test void event
    //P2C2->ProvidedInterface2.EventVoid();
    //CPPUNIT_ASSERT_EQUAL(-1, P2C2->RequiredInterface1.GetValue());
    //CPPUNIT_ASSERT_EQUAL(-1, P2C2->ProvidedInterface1.GetValue());
    //CPPUNIT_ASSERT_EQUAL(0, P2C3->RequiredInterface1.GetValue());

    //// Test write event
    //valueWrite.Data = 4;
    //P2C2->ProvidedInterface2.EventWrite(valueWrite);
    //CPPUNIT_ASSERT_EQUAL(-1, P2C2->RequiredInterface1.GetValue());
    //CPPUNIT_ASSERT_EQUAL(-1, P2C2->ProvidedInterface1.GetValue());
    //CPPUNIT_ASSERT_EQUAL(valueWrite.Data, P2C3->RequiredInterface1.GetValue());
}
*/

CPPUNIT_TEST_SUITE_REGISTRATION(mtsManagerLocalTest);
