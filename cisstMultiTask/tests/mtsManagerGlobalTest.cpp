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

#include "mtsManagerGlobalTest.h"

#include <cisstMultiTask/mtsManagerGlobal.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstOSAbstraction/osaSleep.h>

#include "mtsTestComponents.h"

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

void mtsManagerGlobalTest::setUp()
{
    /*
    mtsManagerTestC1Device * P1C1 = new mtsManagerTestC1Device;
    mtsManagerTestC2Device * P1C2 = new mtsManagerTestC2Device;
    mtsManagerTestC2Device * P2C2 = new mtsManagerTestC2Device;
    mtsManagerTestC3Device * P2C3 = new mtsManagerTestC3Device;

#if !CISST_MTS_HAS_ICE
    localManager1 = new mtsManagerLocal();
    localManager1->AddComponent(P1C1);
    localManager1->AddComponent(P1C2);
    localManager1->AddComponent(P2C2);
    localManager1->AddComponent(P2C3);
#else
    localManager1 = new mtsManagerLocal();
    localManager1->AddComponent(P1C1);
    localManager1->AddComponent(P1C2);

    localManager2 = new mtsManagerLocal();
    localManager2->AddComponent(P2C2);
    localManager2->AddComponent(P2C3);
#endif
    */
}

void mtsManagerGlobalTest::tearDown()
{
    /*
    delete localManager1;
#if CISST_MTS_HAS_ICE
    delete localManager2;
#endif
    */
}

void mtsManagerGlobalTest::TestConstructor(void)
{
    mtsManagerGlobal GCM;
    CPPUNIT_ASSERT(0 == GCM.ConnectionID);
}

void mtsManagerGlobalTest::TestDestructor(void)
{
    TestCleanup();
}

void mtsManagerGlobalTest::TestConnectedInterfaceInfo(void)
{
    mtsManagerGlobal::ConnectedInterfaceInfo info(P1, C1, p1, false);

    CPPUNIT_ASSERT(info.GetProcessName() == P1);
    CPPUNIT_ASSERT(info.GetComponentName() == C1);
    CPPUNIT_ASSERT(info.GetInterfaceName() == p1);
    CPPUNIT_ASSERT(info.IsRemoteConnection() == false);
#if CISST_MTS_HAS_ICE
    info.SetProxyAccessInfo("endpointInfo");
    CPPUNIT_ASSERT(info.GetEndpointInfo() == "endpointInfo");
#endif
}

void mtsManagerGlobalTest::TestConnectionElement(void)
{
    mtsManagerGlobal::ConnectionElement element(P1, 123, P1, C1, r1, P2, C2, p1);
    
    CPPUNIT_ASSERT(element.Connected == false);
    CPPUNIT_ASSERT(element.RequestProcessName == P1);
    CPPUNIT_ASSERT(element.ConnectionID == 123);
    CPPUNIT_ASSERT(element.ClientProcessName == P1);
    CPPUNIT_ASSERT(element.ClientComponentName == C1);
    CPPUNIT_ASSERT(element.ClientInterfaceRequiredName == r1);
    CPPUNIT_ASSERT(element.ServerProcessName == P2);
    CPPUNIT_ASSERT(element.ServerComponentName == C2);
    CPPUNIT_ASSERT(element.ServerInterfaceProvidedName == p1);

    element.SetConnected();
    CPPUNIT_ASSERT(element.Connected == true);

#if CISST_MTS_HAS_ICE
    CPPUNIT_ASSERT(element.CheckTimeout() == false);
    //const double Timeout = (double) mtsManagerProxyServer::GetGCMConnectTimeout();
    const double Timeout = 5.0;
    osaSleep((Timeout * 1.5) * cmn_s); // 50% margin
    CPPUNIT_ASSERT(element.CheckTimeout() == true);
#endif
}

void mtsManagerGlobalTest::TestCleanup(void)
{
    mtsManagerGlobal GCM;

    GCM.AddProcess(P1);
    GCM.AddProcess(P2);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), GCM.ProcessMap.size());

    mtsManagerGlobal::ConnectionElement * element1 = new mtsManagerGlobal::ConnectionElement("", 0, "", "", "", "", "", "");
    mtsManagerGlobal::ConnectionElement * element2 = new mtsManagerGlobal::ConnectionElement("", 0, "", "", "", "", "", "");
    GCM.ConnectionElementMap[0] = element1;
    GCM.ConnectionElementMap[1] = element2;
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), GCM.ConnectionElementMap.size());

    GCM.Cleanup();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), GCM.ProcessMap.size());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), GCM.ConnectionElementMap.size());
}

void mtsManagerGlobalTest::TestGetConnectionsOfInterfaceProvided(void)
{
    mtsManagerGlobal managerGlobal;

    mtsManagerGlobal::ConnectionMapType * connectionMap; 
    mtsManagerGlobal::InterfaceMapType * interfaceMap;
    mtsManagerGlobal::ConnectedInterfaceInfo * connectedInterfaceInfo;

    // 1. Test first type method (with InterfaceMapType argument)
    CPPUNIT_ASSERT(0 == managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p1, &interfaceMap));

    CPPUNIT_ASSERT(managerGlobal.AddProcess(P2));
    CPPUNIT_ASSERT(0 == managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p1, &interfaceMap));

    CPPUNIT_ASSERT(managerGlobal.AddComponent(P2, C2));
    CPPUNIT_ASSERT(0 == managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p1, &interfaceMap));

    CPPUNIT_ASSERT(managerGlobal.AddInterfaceProvidedOrOutput(P2, C2, p1));
    CPPUNIT_ASSERT(0 == managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p1, &interfaceMap));

    // Add required interface to establish connection
    CPPUNIT_ASSERT(managerGlobal.AddComponent(P2, C3));
    CPPUNIT_ASSERT(managerGlobal.AddInterfaceRequiredOrInput(P2, C3, r1));

    // Connect two interfaces
    CPPUNIT_ASSERT(managerGlobal.Connect(P2, P2, C3, r1, P2, C2, p1) != -1);

    // Check if connection information is correct
    connectionMap = managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p1, &interfaceMap);
    CPPUNIT_ASSERT(connectionMap);
    string interfaceUID = managerGlobal.GetInterfaceUID(P2, C3, r1);
    connectedInterfaceInfo = connectionMap->GetItem(interfaceUID);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetProcessName() == P2);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetComponentName() == C3);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetInterfaceName() == r1);

    // 2. Test second type method (without InterfaceMapType argument)
    managerGlobal.Cleanup();

   CPPUNIT_ASSERT(0 == managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p1));

    CPPUNIT_ASSERT(managerGlobal.AddProcess(P2));
    CPPUNIT_ASSERT(0 == managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p1));

    CPPUNIT_ASSERT(managerGlobal.AddComponent(P2, C2));
    CPPUNIT_ASSERT(0 == managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p1));

    CPPUNIT_ASSERT(managerGlobal.AddInterfaceProvidedOrOutput(P2, C2, p1));
    CPPUNIT_ASSERT(0 == managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p1));

    // Add required interface to establish connection
    CPPUNIT_ASSERT(managerGlobal.AddComponent(P2, C3));
    CPPUNIT_ASSERT(managerGlobal.AddInterfaceRequiredOrInput(P2, C3, r1));

    // Connect two interfaces
    CPPUNIT_ASSERT(managerGlobal.Connect(P2, P2, C3, r1, P2, C2, p1) != -1);

    // Check if connection information is correct
    connectionMap = managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p1);
    CPPUNIT_ASSERT(connectionMap);
    interfaceUID = managerGlobal.GetInterfaceUID(P2, C3, r1);
    connectedInterfaceInfo = connectionMap->GetItem(interfaceUID);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetProcessName() == P2);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetComponentName() == C3);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetInterfaceName() == r1);
}

void mtsManagerGlobalTest::TestGetConnectionsOfInterfaceRequired(void)
{
    mtsManagerGlobal managerGlobal;

    mtsManagerGlobal::ConnectionMapType * connectionMap; 
    mtsManagerGlobal::InterfaceMapType * interfaceMap;
    mtsManagerGlobal::ConnectedInterfaceInfo * connectedInterfaceInfo;

    // 1. Test first method (with InterfaceMapType as argument)
    CPPUNIT_ASSERT(0 == managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P2, C3, r1, &interfaceMap));

    CPPUNIT_ASSERT(managerGlobal.AddProcess(P2));
    CPPUNIT_ASSERT(0 == managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P2, C3, r1, &interfaceMap));

    CPPUNIT_ASSERT(managerGlobal.AddComponent(P2, C3));
    CPPUNIT_ASSERT(0 == managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P2, C3, r1, &interfaceMap));

    CPPUNIT_ASSERT(managerGlobal.AddInterfaceRequiredOrInput(P2, C3, r1));
    CPPUNIT_ASSERT(0 == managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P2, C3, r1, &interfaceMap));

    // Add provided interface to establish connection
    CPPUNIT_ASSERT(managerGlobal.AddComponent(P2, C2));
    CPPUNIT_ASSERT(managerGlobal.AddInterfaceProvidedOrOutput(P2, C2, p1));

    // Connect two interfaces
    CPPUNIT_ASSERT(managerGlobal.Connect(P2, P2, C3, r1, P2, C2, p1) != -1);

    // Check if connection information is correct
    connectionMap = managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P2, C3, r1, &interfaceMap);
    CPPUNIT_ASSERT(connectionMap);
    string interfaceUID = managerGlobal.GetInterfaceUID(P2, C2, p1);
    connectedInterfaceInfo = connectionMap->GetItem(interfaceUID);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetProcessName() == P2);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetComponentName() == C2);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetInterfaceName() == p1);

    // 2. Test second method (without InterfaceMapType as argument)
    managerGlobal.Cleanup();

    CPPUNIT_ASSERT(0 == managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P2, C3, r1));

    CPPUNIT_ASSERT(managerGlobal.AddProcess(P2));
    CPPUNIT_ASSERT(0 == managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P2, C3, r1));

    CPPUNIT_ASSERT(managerGlobal.AddComponent(P2, C3));
    CPPUNIT_ASSERT(0 == managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P2, C3, r1));

    CPPUNIT_ASSERT(managerGlobal.AddInterfaceRequiredOrInput(P2, C3, r1));
    CPPUNIT_ASSERT(0 == managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P2, C3, r1));

    // Add provided interface to establish connection
    CPPUNIT_ASSERT(managerGlobal.AddComponent(P2, C2));
    CPPUNIT_ASSERT(managerGlobal.AddInterfaceProvidedOrOutput(P2, C2, p1));

    // Connect two interfaces
    CPPUNIT_ASSERT(managerGlobal.Connect(P2, P2, C3, r1, P2, C2, p1) != -1);

    // Check if connection information is correct
    connectionMap = managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P2, C3, r1);
    CPPUNIT_ASSERT(connectionMap);
    interfaceUID = managerGlobal.GetInterfaceUID(P2, C2, p1);
    connectedInterfaceInfo = connectionMap->GetItem(interfaceUID);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetProcessName() == P2);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetComponentName() == C2);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetInterfaceName() == p1);
}

void mtsManagerGlobalTest::TestAddProcess(void)
{
    mtsManagerGlobal managerGlobal;

    CPPUNIT_ASSERT(managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(managerGlobal.ProcessMap.FindItem(P1));
    CPPUNIT_ASSERT(managerGlobal.ProcessMap.GetItem(P1) == 0);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), managerGlobal.ProcessMap.size());

    CPPUNIT_ASSERT(!managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(managerGlobal.ProcessMap.FindItem(P1));
    CPPUNIT_ASSERT(managerGlobal.ProcessMap.GetItem(P1) == 0);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), managerGlobal.ProcessMap.size());

#if CISST_MTS_HAS_ICE
    CPPUNIT_ASSERT(managerGlobal.AddProcess(P2));
    CPPUNIT_ASSERT(managerGlobal.ProcessMap.FindItem(P1));
    CPPUNIT_ASSERT(managerGlobal.ProcessMap.FindItem(P2));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), managerGlobal.ProcessMap.size());
#endif
}

void mtsManagerGlobalTest::TestFindProcess(void)
{
    mtsManagerGlobal managerGlobal;

    CPPUNIT_ASSERT(!managerGlobal.FindProcess(P1));
    CPPUNIT_ASSERT(managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(managerGlobal.FindProcess(P1));

    CPPUNIT_ASSERT(managerGlobal.RemoveProcess(P1));
    CPPUNIT_ASSERT(!managerGlobal.FindProcess(P1));
}

void mtsManagerGlobalTest::TestRemoveProcess(void)
{
    mtsManagerGlobal managerGlobal;

    CPPUNIT_ASSERT(managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(managerGlobal.FindProcess(P1));

    CPPUNIT_ASSERT(managerGlobal.RemoveProcess(P1));
    CPPUNIT_ASSERT(!managerGlobal.FindProcess(P1));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), managerGlobal.ProcessMap.size());
}

/*
void mtsManagerGlobalTest::TestGetProcessObject(void)
{
    mtsManagerGlobal managerGlobal;

    CPPUNIT_ASSERT(!managerGlobal.GetProcessObject(P1));

    CPPUNIT_ASSERT(managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(P1 == managerGlobal.GetProcessObject(P1));
}
*/

void mtsManagerGlobalTest::TestAddComponent(void)
{
    mtsManagerGlobal managerGlobal;

    // Test adding a component without adding a process first
    CPPUNIT_ASSERT(!managerGlobal.AddComponent(P1, C1));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), managerGlobal.ProcessMap.GetMap().size());

    // Add a process
    CPPUNIT_ASSERT(managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(managerGlobal.AddComponent(P1, C1));
    {
        // Check changes in the process map        
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), managerGlobal.ProcessMap.GetMap().size());

        // Check changes in the component map
        CPPUNIT_ASSERT(0 != managerGlobal.ProcessMap.GetItem(P1));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), managerGlobal.ProcessMap.GetItem(P1)->size());
        CPPUNIT_ASSERT(0 == managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1));
    }

    // Test if a same component name in the same process is not allowed
    CPPUNIT_ASSERT(!managerGlobal.AddComponent(P1, C1));

    // Test addind another component
    CPPUNIT_ASSERT(managerGlobal.AddComponent(P1, C2));
    {
        // Check changes in the process map
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), managerGlobal.ProcessMap.GetMap().size());

        // Check changes in the component map
        CPPUNIT_ASSERT(0 != managerGlobal.ProcessMap.GetItem(P1));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), managerGlobal.ProcessMap.GetItem(P1)->size());
        CPPUNIT_ASSERT(0 == managerGlobal.ProcessMap.GetItem(P1)->GetItem(C2));
    }
}

void mtsManagerGlobalTest::TestFindComponent(void)
{
    mtsManagerGlobal managerGlobal;

    CPPUNIT_ASSERT(!managerGlobal.FindComponent(P1, C1));

    CPPUNIT_ASSERT(managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(!managerGlobal.FindComponent(P1, C1));

    CPPUNIT_ASSERT(managerGlobal.AddComponent(P1, C1));
    CPPUNIT_ASSERT(managerGlobal.FindComponent(P1, C1));

    CPPUNIT_ASSERT(managerGlobal.RemoveComponent(P1, C1));
    CPPUNIT_ASSERT(!managerGlobal.FindComponent(P1, C1));
}

void mtsManagerGlobalTest::TestRemoveComponent(void)
{
    //
    // TODO:
    //
    // Case 1. When only components are registered

    // Case 2. When components that have interfaces are registered

    // Case 3. When components that have interfaces that have connection
    //         with other interfaces are registered
}

void mtsManagerGlobalTest::TestAddInterfaceProvided(void)
{
    mtsManagerGlobal managerGlobal;

    // Test adding a provided interface before adding a component
    CPPUNIT_ASSERT(!managerGlobal.AddInterfaceProvidedOrOutput(P1, C1, p1));

    // Test adding a provided interface after adding a component
    CPPUNIT_ASSERT(managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(managerGlobal.AddComponent(P1, C1));
    CPPUNIT_ASSERT(managerGlobal.AddInterfaceProvidedOrOutput(P1, C1, p1));
    {
        // Check changes in the process map
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), managerGlobal.ProcessMap.GetMap().size());
        CPPUNIT_ASSERT(0 != managerGlobal.ProcessMap.GetItem(P1));

        // Check changes in the component map
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), managerGlobal.ProcessMap.GetItem(P1)->size());
        CPPUNIT_ASSERT(0 != managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1));

        // Check changes in the interface map
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->InterfaceProvidedOrOutputMap.size());
        CPPUNIT_ASSERT(0 == 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->InterfaceProvidedOrOutputMap.GetItem(p1));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->InterfaceRequiredOrInputMap.size());
        CPPUNIT_ASSERT(0 == 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->InterfaceRequiredOrInputMap.GetItem(p1));
    }

    // Test if a same provided interface name in the same component is not allowed
    CPPUNIT_ASSERT(!managerGlobal.AddInterfaceProvidedOrOutput(P1, C1, p1));

    // Test addind another component
    CPPUNIT_ASSERT(managerGlobal.AddInterfaceProvidedOrOutput(P1, C1, p2));
    {
        // Check changes in the process map
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), managerGlobal.ProcessMap.GetMap().size());
        CPPUNIT_ASSERT(0 != managerGlobal.ProcessMap.GetItem(P1));

        // Check changes in the component map
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), managerGlobal.ProcessMap.GetItem(P1)->size());
        CPPUNIT_ASSERT(0 != managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1));

        // Check changes in the interface map
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->InterfaceProvidedOrOutputMap.size());
        CPPUNIT_ASSERT(0 == 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->InterfaceProvidedOrOutputMap.GetItem(p2));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->InterfaceRequiredOrInputMap.size());
        CPPUNIT_ASSERT(0 == 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->InterfaceRequiredOrInputMap.GetItem(p1));
    }
}

void mtsManagerGlobalTest::TestAddInterfaceRequired(void)
{
    mtsManagerGlobal managerGlobal;

    // Test adding a required interface before adding a component
    CPPUNIT_ASSERT(!managerGlobal.AddInterfaceRequiredOrInput(P1, C1, r1));

    // Test adding a required interface after adding a component
    CPPUNIT_ASSERT(managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(managerGlobal.AddComponent(P1, C1));
    CPPUNIT_ASSERT(managerGlobal.AddInterfaceRequiredOrInput(P1, C1, r1));
    {
        // Check changes in the process map
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), managerGlobal.ProcessMap.GetMap().size());
        CPPUNIT_ASSERT(0 != managerGlobal.ProcessMap.GetItem(P1));

        // Check changes in the component map
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), managerGlobal.ProcessMap.GetItem(P1)->size());
        CPPUNIT_ASSERT(0 != managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1));

        // Check changes in the interface map
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->InterfaceRequiredOrInputMap.size());
        CPPUNIT_ASSERT(0 == 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->InterfaceRequiredOrInputMap.GetItem(r1));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->InterfaceProvidedOrOutputMap.size());
        CPPUNIT_ASSERT(0 == 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->InterfaceProvidedOrOutputMap.GetItem(r1));
    }

    // Test if a same required interface name in the same component is not allowed
    CPPUNIT_ASSERT(!managerGlobal.AddInterfaceRequiredOrInput(P1, C1, r1));

    // Test addind another component
    CPPUNIT_ASSERT(managerGlobal.AddInterfaceRequiredOrInput(P1, C1, r2));
    {
        // Check changes in the process map
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), managerGlobal.ProcessMap.GetMap().size());
        CPPUNIT_ASSERT(0 != managerGlobal.ProcessMap.GetItem(P1));

        // Check changes in the component map
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), managerGlobal.ProcessMap.GetItem(P1)->size());
        CPPUNIT_ASSERT(0 != managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1));

        // Check changes in the interface map
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->InterfaceRequiredOrInputMap.size());
        CPPUNIT_ASSERT(0 == 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->InterfaceRequiredOrInputMap.GetItem(r2));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->InterfaceProvidedOrOutputMap.size());
        CPPUNIT_ASSERT(0 == 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->InterfaceProvidedOrOutputMap.GetItem(r1));
    }
}

void mtsManagerGlobalTest::TestFindInterfaceProvided(void)
{
    mtsManagerGlobal managerGlobal;

    CPPUNIT_ASSERT(!managerGlobal.FindInterfaceProvidedOrOutput(P1, C1, p1));

    CPPUNIT_ASSERT(managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(!managerGlobal.FindInterfaceProvidedOrOutput(P1, C1, p1));

    CPPUNIT_ASSERT(managerGlobal.AddComponent(P1, C1));
    CPPUNIT_ASSERT(!managerGlobal.FindInterfaceProvidedOrOutput(P1, C1, p1));

    CPPUNIT_ASSERT(managerGlobal.AddInterfaceProvidedOrOutput(P1, C1, p1));
    CPPUNIT_ASSERT(managerGlobal.FindInterfaceProvidedOrOutput(P1, C1, p1));

    CPPUNIT_ASSERT(managerGlobal.RemoveInterfaceProvidedOrOutput(P1, C1, p1));
    CPPUNIT_ASSERT(!managerGlobal.FindInterfaceProvidedOrOutput(P1, C1, p1));
}

void mtsManagerGlobalTest::TestFindInterfaceRequired(void)
{
    mtsManagerGlobal managerGlobal;

    CPPUNIT_ASSERT(!managerGlobal.FindInterfaceRequiredOrInput(P1, C1, r1));

    CPPUNIT_ASSERT(managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(!managerGlobal.FindInterfaceRequiredOrInput(P1, C1, r1));

    CPPUNIT_ASSERT(managerGlobal.AddComponent(P1, C1));
    CPPUNIT_ASSERT(!managerGlobal.FindInterfaceRequiredOrInput(P1, C1, r1));

    CPPUNIT_ASSERT(managerGlobal.AddInterfaceRequiredOrInput(P1, C1, r1));
    CPPUNIT_ASSERT(managerGlobal.FindInterfaceRequiredOrInput(P1, C1, r1));

    CPPUNIT_ASSERT(managerGlobal.RemoveInterfaceRequiredOrInput(P1, C1, r1));
    CPPUNIT_ASSERT(!managerGlobal.FindInterfaceRequiredOrInput(P1, C1, r1));
}

void mtsManagerGlobalTest::TestRemoveInterfaceProvided(void)
{
    mtsManagerGlobal managerGlobal;

    // Case 1. When only interfaces that have no connection are registered
    // Test removing a provided interface before adding a component
    CPPUNIT_ASSERT(!managerGlobal.RemoveInterfaceProvidedOrOutput(P1, C1, p1));

    // Test adding a provided interface after adding a component
    CPPUNIT_ASSERT(!managerGlobal.FindInterfaceProvidedOrOutput(P1, C1, p1));
    CPPUNIT_ASSERT(managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(managerGlobal.AddComponent(P1, C1));
    CPPUNIT_ASSERT(managerGlobal.AddInterfaceProvidedOrOutput(P1, C1, p1));

    CPPUNIT_ASSERT(managerGlobal.FindInterfaceProvidedOrOutput(P1, C1, p1));
    CPPUNIT_ASSERT(managerGlobal.RemoveInterfaceProvidedOrOutput(P1, C1, p1));
    CPPUNIT_ASSERT(!managerGlobal.FindInterfaceProvidedOrOutput(P1, C1, p1));
    {
        // Check changes in the process map
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), managerGlobal.ProcessMap.GetMap().size());
        CPPUNIT_ASSERT(0 != managerGlobal.ProcessMap.GetItem(P1));

        // Check changes in the component map
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), managerGlobal.ProcessMap.GetItem(P1)->size());
        CPPUNIT_ASSERT(0 != managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1));

        // Check changes in the interface map
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->InterfaceProvidedOrOutputMap.size());
        CPPUNIT_ASSERT(0 == 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->InterfaceProvidedOrOutputMap.GetItem(p1));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->InterfaceRequiredOrInputMap.size());
        CPPUNIT_ASSERT(0 == 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->InterfaceRequiredOrInputMap.GetItem(p1));
    }

    //
    // TODO:
    // 
    // Case 2. When interfaces have connection with other interfaces
}
         
void mtsManagerGlobalTest::TestRemoveInterfaceRequired(void)
{
    mtsManagerGlobal managerGlobal;

    // Case 1. When only interfaces that have no connection are registered
    // Test removing a provided interface before adding a component
    CPPUNIT_ASSERT(!managerGlobal.RemoveInterfaceRequiredOrInput(P1, C1, r1));

    // Test adding a provided interface after adding a component
    CPPUNIT_ASSERT(!managerGlobal.FindInterfaceRequiredOrInput(P1, C1, r1));
    CPPUNIT_ASSERT(managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(managerGlobal.AddComponent(P1, C1));
    CPPUNIT_ASSERT(managerGlobal.AddInterfaceRequiredOrInput(P1, C1, r1));

    CPPUNIT_ASSERT(managerGlobal.FindInterfaceRequiredOrInput(P1, C1, r1));
    CPPUNIT_ASSERT(managerGlobal.RemoveInterfaceRequiredOrInput(P1, C1, r1));
    CPPUNIT_ASSERT(!managerGlobal.FindInterfaceRequiredOrInput(P1, C1, r1));
    {
        // Check changes in the process map
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), managerGlobal.ProcessMap.GetMap().size());
        CPPUNIT_ASSERT(0 != managerGlobal.ProcessMap.GetItem(P1));

        // Check changes in the component map
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), managerGlobal.ProcessMap.GetItem(P1)->size());
        CPPUNIT_ASSERT(0 != managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1));

        // Check changes in the interface map
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->InterfaceProvidedOrOutputMap.size());
        CPPUNIT_ASSERT(0 == 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->InterfaceProvidedOrOutputMap.GetItem(r1));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->InterfaceRequiredOrInputMap.size());
        CPPUNIT_ASSERT(0 == 
            managerGlobal.ProcessMap.GetItem(P1)->GetItem(C1)->InterfaceRequiredOrInputMap.GetItem(r1));
    }

    //
    // TODO:
    // 
    // Case 2. When interfaces have connection with other interfaces
}

void mtsManagerGlobalTest::TestConnectLocal(void)
{
    mtsManagerGlobal managerGlobal;

    // Test if invalid arguments are handled properly
    CPPUNIT_ASSERT(managerGlobal.Connect(P1, P1, C1, r1, P2, C2, p1) == -1);

    CPPUNIT_ASSERT(managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(managerGlobal.Connect(P1, P1, C1, r1, P2, C2, p1) == -1);

    CPPUNIT_ASSERT(managerGlobal.AddComponent(P1, C1));
    CPPUNIT_ASSERT(managerGlobal.Connect(P1, P1, C1, r1, P2, C2, p1) == -1);

    CPPUNIT_ASSERT(managerGlobal.AddInterfaceRequiredOrInput(P1, C1, r1));
    CPPUNIT_ASSERT(managerGlobal.Connect(P1, P1, C1, r1, P2, C2, p1) == -1);

    CPPUNIT_ASSERT(managerGlobal.AddProcess(P2));
    CPPUNIT_ASSERT(managerGlobal.Connect(P1, P1, C1, r1, P2, C2, p1) == -1);

    CPPUNIT_ASSERT(managerGlobal.AddComponent(P2, C2));
    CPPUNIT_ASSERT(managerGlobal.Connect(P1, P1, C1, r1, P2, C2, p1) == -1);

    CPPUNIT_ASSERT(managerGlobal.AddInterfaceProvidedOrOutput(P2, C2, p1));

    // TODO: Resolve this
    /*
    CPPUNIT_ASSERT(managerGlobal.Connect(P1, P1, C1, r1, P2, C2, p1));

    // Return false if already established connection
    CPPUNIT_ASSERT(!managerGlobal.Connect(P1, P1, C1, r1, P2, C2, p1));

    // Test if connection is established correctly
    // Check required interface's connection information
    connectionMap = managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P1, C1, r1);
    CPPUNIT_ASSERT(connectionMap);

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), connectionMap->size());
    string interfaceUID = managerGlobal.GetInterfaceUID(P2, C2, p1);
    connectedInterfaceInfo = connectionMap->GetItem(interfaceUID);
    CPPUNIT_ASSERT(connectedInterfaceInfo);

    CPPUNIT_ASSERT(connectedInterfaceInfo->GetProcessName() == P2);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetComponentName() == C2);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetInterfaceName() == p1);

    // Check provided interface's connection information
    connectionMap = managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p1);
    CPPUNIT_ASSERT(connectionMap);

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), connectionMap->size());
    interfaceUID = managerGlobal.GetInterfaceUID(P1, C1, r1);
    connectedInterfaceInfo = connectionMap->GetItem(interfaceUID);
    CPPUNIT_ASSERT(connectedInterfaceInfo);

    CPPUNIT_ASSERT(connectedInterfaceInfo->GetProcessName() == P1);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetComponentName() == C1);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetInterfaceName() == r1);
    */
}

#if CISST_MTS_HAS_ICE
/*
void mtsManagerGlobalTest::TestConnectRemote(void)
{
    mtsManagerGlobal managerGlobal;
    mtsManagerGlobal::ConnectionMapType * connectionMap;

    // Prepare local managers for this test
    mtsManagerTestC1Device * P1C1 = new mtsManagerTestC1Device;
    mtsManagerTestC2Device * P1C2 = new mtsManagerTestC2Device;
    mtsManagerTestC2Device * P2C2 = new mtsManagerTestC2Device;
    mtsManagerTestC3Device * P2C3 = new mtsManagerTestC3Device;

    mtsManagerLocalInterface * localManager1 = new mtsManagerLocal(P1);
    mtsManagerLocal * localManager1Object = dynamic_cast<mtsManagerLocal*>(localManager1);
    localManager1Object->ManagerGlobal = &managerGlobal;
    managerGlobal.AddProcess(localManager1);
    localManager1Object->AddComponent(P1C1);
    localManager1Object->AddComponent(P1C2);

    mtsManagerLocalInterface * localManager2 = new mtsManagerLocal(P2);
    mtsManagerLocal * localManager2Object = dynamic_cast<mtsManagerLocal*>(localManager2);
    localManager2Object->ManagerGlobal = &managerGlobal;
    managerGlobal.AddProcess(localManager2);
    localManager2Object->AddComponent(P2C2);
    localManager2Object->AddComponent(P2C3);

    // Test with various configurations to see if 
    // connection  information are managed correctly
    // 
    // All the test cases used here are described in the project wiki page.
    // (see https://trac.lcsr.jhu.edu/cisst/wiki/Private/cisstMultiTaskNetwork)

    // Establish connections
    // Connection: (P1, C1, r1) ~ (P2, C2, p1)
    CPPUNIT_ASSERT(managerGlobal.Connect(P1, P1, C1, r1, P2, C2, p1) != -1);
    // Connection: (P1, C1, r2) ~ (P2, C2, p2)
    CPPUNIT_ASSERT(managerGlobal.Connect(P1, P1, C1, r2, P2, C2, p2) != -1);
    // Connection: (P1, C2, r1) ~ (P2, C2, p2)
    CPPUNIT_ASSERT(managerGlobal.Connect(P1, P1, C2, r1, P2, C2, p2) != -1);
    // Connection: (P2, C3, r1) ~ (P2, C2, p2)
    CPPUNIT_ASSERT(managerGlobal.Connect(P2, C3, r1, P2, C2, p2) != -1);

    // Check if connection information is correct
    mtsManagerGlobal::ConnectionMapType::const_iterator it;

    // P1 : C1 : r1 - P2 : C2 : p1
    connectionMap = managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P1, C1, r1);
    CPPUNIT_ASSERT(connectionMap);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), connectionMap->size());
    for (it = connectionMap->begin(); it != connectionMap->end(); ++it) {
        CPPUNIT_ASSERT(it->second->GetProcessName() == P2);
        CPPUNIT_ASSERT(it->second->GetComponentName() == C2);
        CPPUNIT_ASSERT(it->second->GetInterfaceName() == p1);
    }
    
    // P1 : C1 : r2 - P2 : C2 : p2
    connectionMap = managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P1, C1, r2);
    CPPUNIT_ASSERT(connectionMap);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), connectionMap->size());
    for (it = connectionMap->begin(); it != connectionMap->end(); ++it) {
        CPPUNIT_ASSERT(it->second->GetProcessName() == P2);
        CPPUNIT_ASSERT(it->second->GetComponentName() == C2);
        CPPUNIT_ASSERT(it->second->GetInterfaceName() == p2);
    }

    // P1 : C2 : r1 - P2 : C2 : p2
    connectionMap = managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P1, C2, r1);
    CPPUNIT_ASSERT(connectionMap);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), connectionMap->size());
    for (it = connectionMap->begin(); it != connectionMap->end(); ++it) {
        CPPUNIT_ASSERT(it->second->GetProcessName() == P2);
        CPPUNIT_ASSERT(it->second->GetComponentName() == C2);
        CPPUNIT_ASSERT(it->second->GetInterfaceName() == p2);
    }

    // P2 : C2 : p1 - P1 : C1 : r1
    connectionMap = managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p1);
    CPPUNIT_ASSERT(connectionMap);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), connectionMap->size());
    for (it = connectionMap->begin(); it != connectionMap->end(); ++it) {
        CPPUNIT_ASSERT(it->second->GetProcessName() == P1);
        CPPUNIT_ASSERT(it->second->GetComponentName() == C1);
        CPPUNIT_ASSERT(it->second->GetInterfaceName() == r1);
    }

    // P2 : C2 : p2 - P1 : C1 : r2
    // P2 : C2 : p2 - P1 : C2 : r1
    // P2 : C2 : p2 - P2 : C3 : r1
    connectionMap = managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p2);
    CPPUNIT_ASSERT(connectionMap);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), connectionMap->size());
    for (it = connectionMap->begin(); it != connectionMap->end(); ++it) {
        if (it->second->GetProcessName() == P1) {
            if (it->second->GetComponentName() == C1) {
                if (it->second->GetInterfaceName() != r2) {
                    CPPUNIT_ASSERT(false);
                }
            } else if (it->second->GetComponentName() == C2) {
                if (it->second->GetInterfaceName() != r1) {
                    CPPUNIT_ASSERT(false);
                }
            } else {
                CPPUNIT_ASSERT(false);
            }
        } else if (it->second->GetProcessName() == P2) {
            if (it->second->GetComponentName() == C3) {
                if (it->second->GetInterfaceName() != r1) {
                    CPPUNIT_ASSERT(false);
                }
            } else {
                CPPUNIT_ASSERT(false);
            }
        } else {
            CPPUNIT_ASSERT(false);
        }
    }

    // Test if connection is cleaned up correctly when
    // a required interface is successfully connected to
    // provided interface while a provided interface failed
    // to establish connection with a required interface
    // (for any reason).
    //
}

void mtsManagerGlobalTest::TestDisconnect(void)
{
    mtsManagerGlobal managerGlobal;

    // Prepare local managers for this test
    mtsManagerTestC1Device * P1C1 = new mtsManagerTestC1Device;
    mtsManagerTestC2Device * P1C2 = new mtsManagerTestC2Device;
    mtsManagerTestC2Device * P2C2 = new mtsManagerTestC2Device;
    mtsManagerTestC3Device * P2C3 = new mtsManagerTestC3Device;

    mtsManagerLocalInterface * localManager1 = new mtsManagerLocal(P1);
    mtsManagerLocal * localManager1Object = dynamic_cast<mtsManagerLocal*>(localManager1);
    localManager1Object->ManagerGlobal = &managerGlobal;
    managerGlobal.AddProcess(localManager1);
    localManager1Object->AddComponent(P1C1);
    localManager1Object->AddComponent(P1C2);

    mtsManagerLocalInterface * localManager2 = new mtsManagerLocal(P2);
    mtsManagerLocal * localManager2Object = dynamic_cast<mtsManagerLocal*>(localManager2);
    localManager2Object->ManagerGlobal = &managerGlobal;
    managerGlobal.AddProcess(localManager2);
    localManager2Object->AddComponent(P2C2);
    localManager2Object->AddComponent(P2C3);

    // All the test cases used here are described in the project wiki page.
    // (see https://trac.lcsr.jhu.edu/cisst/wiki/Private/cisstMultiTaskNetwork)

    // Establish connections
    // Connection: (P1, C1, r1) ~ (P2, C2, p1)
    CPPUNIT_ASSERT(managerGlobal.Connect(P1, P1, C1, r1, P2, C2, p1) != -1);
    // Connection: (P1, C1, r2) ~ (P2, C2, p2)
    CPPUNIT_ASSERT(managerGlobal.Connect(P1, P1, C1, r2, P2, C2, p2) != -1);
    // Connection: (P1, C2, r1) ~ (P2, C2, p2)
    CPPUNIT_ASSERT(managerGlobal.Connect(P1, P1, C2, r1, P2, C2, p2) != -1);
    // Connection: (P2, C3, r1) ~ (P2, C2, p2)
    CPPUNIT_ASSERT(managerGlobal.Connect(P2, C3, r1, P2, C2, p2) != -1);

    // Check if connection information is correct while disconnecting a connection one by one
    mtsManagerGlobal::ConnectionMapType * connectionMap;

    // Disconnect (P1, C1, r1) ~ (P2, C2, p1)
    CPPUNIT_ASSERT(managerGlobal.DisConnect(P1, P1, C1, r1, P2, C2, p1));
    {
        // Check connection count
        connectionMap = managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P1, C1, r1);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), connectionMap->size());
        connectionMap = managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P1, C1, r2);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), connectionMap->size());
        connectionMap = managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p1);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), connectionMap->size());
        connectionMap = managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p2);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), connectionMap->size());
    }
    CPPUNIT_ASSERT(!managerGlobal.DisConnect(P1, P1, C1, r1, P2, C2, p1));

    // Disconnect (P1, C1, r2) ~ (P2, C2, p2)
    CPPUNIT_ASSERT(managerGlobal.DisConnect(P1, P1, C1, r2, P2, C2, p2));
    {
        // Check connection count
        connectionMap = managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P1, C1, r1);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), connectionMap->size());
        connectionMap = managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P1, C1, r2);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), connectionMap->size());
        connectionMap = managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p1);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), connectionMap->size());
        connectionMap = managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p2);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), connectionMap->size());
    }
    CPPUNIT_ASSERT(!managerGlobal.DisConnect(P1, P1, C1, r2, P2, C2, p2));

    // Disconnect (P1, C2, r1) ~ (P2, C2, p2)
    CPPUNIT_ASSERT(managerGlobal.DisConnect(P1, P1, C2, r1, P2, C2, p2));
    {
        // Check connection count
        connectionMap = managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P1, C1, r1);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), connectionMap->size());
        connectionMap = managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P1, C1, r2);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), connectionMap->size());
        connectionMap = managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p1);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), connectionMap->size());
        connectionMap = managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p2);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), connectionMap->size());
    }
    CPPUNIT_ASSERT(!managerGlobal.DisConnect(P1, P1, C2, r1, P2, C2, p2));

    // Disconnect (P2, C3, r1) ~ (P2, C2, p2)
    CPPUNIT_ASSERT(managerGlobal.Disconnect(P2, C3, r1, P2, C2, p2));
    {
        // Check connection count
        connectionMap = managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P1, C1, r1);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), connectionMap->size());
        connectionMap = managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P1, C1, r2);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), connectionMap->size());
        connectionMap = managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p1);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), connectionMap->size());
        connectionMap = managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p2);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), connectionMap->size());
    }
    CPPUNIT_ASSERT(!managerGlobal.Disconnect(P2, C3, r1, P2, C2, p2));

    // Prepare components
    CPPUNIT_ASSERT(managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(managerGlobal.AddComponent(P1, C1));
    CPPUNIT_ASSERT(managerGlobal.AddInterfaceRequiredOrInput(P1, C1, r1));
    CPPUNIT_ASSERT(managerGlobal.AddInterfaceRequiredOrInput(P1, C1, r2));

    CPPUNIT_ASSERT(managerGlobal.AddProcess(P2));
    CPPUNIT_ASSERT(managerGlobal.AddComponent(P2, C2));
    CPPUNIT_ASSERT(managerGlobal.AddInterfaceProvidedOrOutput(P2, C2, p1));
    CPPUNIT_ASSERT(managerGlobal.AddInterfaceProvidedOrOutput(P2, C2, p2));

    // Establish connections
    // (P1, C1, r1) ~ (P2, C2, p1)
    CPPUNIT_ASSERT(managerGlobal.Connect(P1, P1, C1, r1, P2, C2, p1) != -1);
    // (P1, C1, r1) ~ (P2, C2, p2)
    CPPUNIT_ASSERT(managerGlobal.Connect(P1, P1, C1, r1, P2, C2, p2) != -1);
    // (P1, C1, r2) ~ (P2, C2, p1)
    CPPUNIT_ASSERT(managerGlobal.Connect(P1, P1, C1, r2, P2, C2, p1) != -1);
    // (P1, C1, r2) ~ (P2, C2, p2)
    CPPUNIT_ASSERT(managerGlobal.Connect(P1, P1, C1, r2, P2, C2, p2) != -1);

    mtsManagerGlobal::ConnectionMapType * connectionMap;
    connectionMap = managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P1, C1, r1);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), connectionMap->size());
    connectionMap = managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P1, C1, r2);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), connectionMap->size());
    connectionMap = managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p1);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), connectionMap->size());
    connectionMap = managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p2);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), connectionMap->size());

    // Check if connection information is correct while disconnecting a connection one by one
    mtsManagerGlobal::ConnectedInterfaceInfo * connectionInfo;

    // Disconnect P1:C1:r1 - P2:C2:p1
    managerGlobal.DisConnect(P1, P1, C1, r1, P2, C2, p1);
    {
        // Check required interface's connection information
        connectionMap = managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P1, C1, r1);
        CPPUNIT_ASSERT(connectionMap->begin()->first == managerGlobal.GetInterfaceUID(P2, C2, p2));
        connectionInfo = connectionMap->begin()->second;
        CPPUNIT_ASSERT(connectionInfo->GetProcessName() == P2);
        CPPUNIT_ASSERT(connectionInfo->GetComponentName() == C2);
        CPPUNIT_ASSERT(connectionInfo->GetInterfaceName() == p2);

        // Check provided interface's connection information
        connectionMap = managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p1);
        CPPUNIT_ASSERT(connectionMap->begin()->first == managerGlobal.GetInterfaceUID(P1, C1, r2));
        connectionInfo = connectionMap->begin()->second;
        CPPUNIT_ASSERT(connectionInfo->GetProcessName() == P1);
        CPPUNIT_ASSERT(connectionInfo->GetComponentName() == C1);
        CPPUNIT_ASSERT(connectionInfo->GetInterfaceName() == r2);

        // Check connection count
        connectionMap = managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P1, C1, r1);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), connectionMap->size());
        connectionMap = managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P1, C1, r2);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), connectionMap->size());
        connectionMap = managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p1);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), connectionMap->size());
        connectionMap = managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p2);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), connectionMap->size());
    }

    // Disconnect P1:C1:r1 - P2:C2:p2
    managerGlobal.DisConnect(P1, P1, C1, r1, P2, C2, p2);
    {
        // Check required interface's connection information
        connectionMap = managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P1, C1, r1);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), connectionMap->size());

        // Check provided interface's connection information
        connectionMap = managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p1);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), connectionMap->size());

        CPPUNIT_ASSERT(connectionMap->begin()->first == managerGlobal.GetInterfaceUID(P1, C1, r2));
        connectionInfo = connectionMap->begin()->second;
        CPPUNIT_ASSERT(connectionInfo->GetProcessName() == P1);
        CPPUNIT_ASSERT(connectionInfo->GetComponentName() == C1);
        CPPUNIT_ASSERT(connectionInfo->GetInterfaceName() == r2);

        // Check connection status
        connectionMap = managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P1, C1, r1);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), connectionMap->size());
        connectionMap = managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P1, C1, r2);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), connectionMap->size());
        connectionMap = managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p1);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), connectionMap->size());
        connectionMap = managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p2);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), connectionMap->size());
    }

    // Disconnect P1:C1:r2 - P2:C2:p1
    managerGlobal.DisConnect(P1, P1, C1, r2, P2, C2, p1);
    {
        // Check required interface's connection information
        connectionMap = managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P1, C1, r2);
        CPPUNIT_ASSERT(connectionMap->begin()->first == managerGlobal.GetInterfaceUID(P2, C2, p2));
        connectionInfo = connectionMap->begin()->second;
        CPPUNIT_ASSERT(connectionInfo->GetProcessName() == P2);
        CPPUNIT_ASSERT(connectionInfo->GetComponentName() == C2);
        CPPUNIT_ASSERT(connectionInfo->GetInterfaceName() == p2);

        // Check provided interface's connection information
        connectionMap = managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p1);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), connectionMap->size());

        // Check connection status
        connectionMap = managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P1, C1, r1);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), connectionMap->size());
        connectionMap = managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P1, C1, r2);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), connectionMap->size());
        connectionMap = managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p1);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), connectionMap->size());
        connectionMap = managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p2);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), connectionMap->size());
    }

    // Disconnect P1:C1:r2 - P2:C2:p2
    managerGlobal.DisConnect(P1, P1, C1, r2, P2, C2, p2);
    {
        // Check required interface's connection information
        connectionMap = managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P1, C1, r2);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), connectionMap->size());

        // Check provided interface's connection information
        connectionMap = managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p2);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), connectionMap->size());

        // Check connection status
        connectionMap = managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P1, C1, r1);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), connectionMap->size());
        connectionMap = managerGlobal.GetConnectionsOfInterfaceRequiredOrInput(P1, C1, r2);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), connectionMap->size());
        connectionMap = managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p1);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), connectionMap->size());
        connectionMap = managerGlobal.GetConnectionsOfInterfaceProvidedOrOutput(P2, C2, p2);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), connectionMap->size());
    }
}
*/
#endif


void mtsManagerGlobalTest::TestIsAlreadyConnected(void)
{
    mtsManagerGlobal managerGlobal;

    // Because it is assumed that the existence of provided and required interface 
    // has already been checked before calling this method, 
    // FindInterfaceRequiredOrInput() and FindInterfaceProvidedOrOutput() should be checked first.

    // Required interface
    CPPUNIT_ASSERT(managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(managerGlobal.AddComponent(P1, C1));
    CPPUNIT_ASSERT(managerGlobal.AddInterfaceRequiredOrInput(P1, C1, r1));
    // Provided interface
    CPPUNIT_ASSERT(managerGlobal.AddProcess(P2));
    CPPUNIT_ASSERT(managerGlobal.AddComponent(P2, C2));
    CPPUNIT_ASSERT(managerGlobal.AddInterfaceProvidedOrOutput(P2, C2, p1));

    // Check if both interfaces exist
    CPPUNIT_ASSERT(managerGlobal.FindInterfaceRequiredOrInput(P1, C1, r1));
    CPPUNIT_ASSERT(managerGlobal.FindInterfaceProvidedOrOutput(P2, C2, p1));

    // Check if this method can detect connection correctly after connection.
    CPPUNIT_ASSERT(!managerGlobal.IsAlreadyConnected(P1, C1, r1, P2, C2, p1));
    /* TODO: Resolve this
    CPPUNIT_ASSERT(managerGlobal.Connect(P1, P1, C1, r1, P2, C2, p1) != -1);
    CPPUNIT_ASSERT(managerGlobal.IsAlreadyConnected(P1, C1, r1, P2, C2, p1) != -1);

    // Check if this method can detect connection correctly after disconnection.
    managerGlobal.Disconnect(P1, C1, r1, P2, C2, p1);
    CPPUNIT_ASSERT(!managerGlobal.IsAlreadyConnected(P1, C1, r1, P2, C2, p1) != -1);
    */
}

void mtsManagerGlobalTest::TestAddConnectedInterface(void)
{
    mtsManagerGlobal managerGlobal;

    // Test if invalid arguments are handled properly
    CPPUNIT_ASSERT(!managerGlobal.AddConnectedInterface(0, P1, C1, r1));

    // Prepare components
    CPPUNIT_ASSERT(managerGlobal.AddProcess(P1));
    CPPUNIT_ASSERT(managerGlobal.AddComponent(P1, C1));
    CPPUNIT_ASSERT(managerGlobal.AddInterfaceRequiredOrInput(P1, C1, r1));

    mtsManagerGlobal::ConnectionMapType connectionMap;
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), connectionMap.size());

    CPPUNIT_ASSERT(managerGlobal.AddConnectedInterface(&connectionMap, P1, C1, r1));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), connectionMap.size());

    string interfaceUID = managerGlobal.GetInterfaceUID(P1, C1, r1);
    CPPUNIT_ASSERT(connectionMap.FindItem(interfaceUID));
    
    mtsManagerGlobal::ConnectionMapType::const_iterator it = connectionMap.begin();
    mtsManagerGlobal::ConnectedInterfaceInfo * connectedInterfaceInfo = it->second;

    CPPUNIT_ASSERT_EQUAL(interfaceUID, it->first);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetProcessName() == P1);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetComponentName() == C1);
    CPPUNIT_ASSERT(connectedInterfaceInfo->GetInterfaceName() == r1);
}

CPPUNIT_TEST_SUITE_REGISTRATION(mtsManagerGlobalTest);
