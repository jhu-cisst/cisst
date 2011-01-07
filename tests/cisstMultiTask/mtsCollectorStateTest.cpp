/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsCollectorStateTest.cpp 2169 2011-01-05 22:42:04Z adeguet1 $

  Author(s):  Anton Deguet
  Created on: 2011-01-06

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "mtsCollectorStateTest.h"

#include <cisstMultiTask/mtsManagerGlobal.h>
#include <cisstMultiTask/mtsManagerLocal.h>

#include "mtsTestComponents.h"

#define P1 "P1"
#define P2 "P2"
#define P1_OBJ localManager1
#define P2_OBJ localManager2

#define DEFAULT_PROCESS_NAME "LCM"

const double TransitionDelay = 3.0 * cmn_s;

mtsCollectorStateTest::mtsCollectorStateTest()
{
}


void mtsCollectorStateTest::setUp(void)
{
}


void mtsCollectorStateTest::tearDown(void)
{
}

template <class _clientType>
void mtsCollectorStateTest::TestProvidedInterfaces(_clientType * client)
{
    // execution result used by all functions
    mtsExecutionResult executionResult;

    // we assume both client and servers use the same type
    typedef typename _clientType::value_type value_type;
    mtsComponentManager * manager = mtsComponentManager::GetInstance();

    // add to manager and start all
    CPPUNIT_ASSERT(manager->AddComponent(client));

    mtsCollectorState * stateCollector = new mtsCollectorState("CollectorStateTest");
    CPPUNIT_ASSERT(stateCollector);
    CPPUNIT_ASSERT(stateCollector->SetStateTable(client->GetName(),
                                                 client->InterfaceProvided1.StateTable->GetName()));
    CPPUNIT_ASSERT(stateCollector->AddSignal("StateValue"));
    manager->AddComponent(stateCollector);
    CPPUNIT_ASSERT(stateCollector->Connect());
    stateCollector->SetOutputToDefault();

    // manager->Connect(client->GetName(), "r1", server->GetName(), "p1");
    manager->CreateAll();
    CPPUNIT_ASSERT(manager->WaitForStateAll(mtsComponentState::READY, TransitionDelay));
    manager->StartAll();
    CPPUNIT_ASSERT(manager->WaitForStateAll(mtsComponentState::ACTIVE, TransitionDelay));

    // stop all and cleanup
    manager->KillAll();
    CPPUNIT_ASSERT(manager->WaitForStateAll(mtsComponentState::FINISHED, TransitionDelay));
    // manager->Disconnect(client->GetName(), "r1", server->GetName(), "p1");
    CPPUNIT_ASSERT(stateCollector->Disconnect());
    CPPUNIT_ASSERT(manager->RemoveComponent(client));
    CPPUNIT_ASSERT(manager->RemoveComponent(stateCollector));
    delete stateCollector;
    // the manager singleton needs to be cleaned up, adeguet1
    std::cerr << "temporary hack " << CMN_LOG_DETAILS << std::endl;
    manager->RemoveComponent("LCM_MCC");
    manager->RemoveComponent("MCS");
}


template <class _elementType>
void mtsCollectorStateTest::TestProvidedInterfacesDevice(void)
{
    mtsTestDevice1<_elementType> * client = new mtsTestDevice1<_elementType>;
    TestProvidedInterfaces(client);
    delete client;
}
void mtsCollectorStateTest::TestProvidedInterfacesDevice_mtsInt(void) {
    mtsCollectorStateTest::TestProvidedInterfacesDevice<mtsInt>();
}
void mtsCollectorStateTest::TestProvidedInterfacesDevice_int(void) {
    mtsCollectorStateTest::TestProvidedInterfacesDevice<int>();
}


template <class _elementType>
void mtsCollectorStateTest::TestProvidedInterfacesPeriodic(void)
{
    mtsTestPeriodic1<_elementType> * client = new mtsTestPeriodic1<_elementType>;
    TestProvidedInterfaces(client);
    delete client;
}
void mtsCollectorStateTest::TestProvidedInterfacesPeriodic_mtsInt(void) {
    mtsCollectorStateTest::TestProvidedInterfacesPeriodic<mtsInt>();
}
void mtsCollectorStateTest::TestProvidedInterfacesPeriodic_int(void) {
    mtsCollectorStateTest::TestProvidedInterfacesPeriodic<int>();
}


CPPUNIT_TEST_SUITE_REGISTRATION(mtsCollectorStateTest);
