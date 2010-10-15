/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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

#include "mtsCommandAndEventTest.h"

#include <cisstMultiTask/mtsManagerGlobal.h>
#include <cisstMultiTask/mtsManagerLocal.h>

#include "mtsTestComponents.h"

#define P1 "P1"
#define P2 "P2"
#define P1_OBJ localManager1
#define P2_OBJ localManager2

#define DEFAULT_PROCESS_NAME "LCM"

const double TransitionDelay = 3.0 * cmn_s;

mtsCommandAndEventTest::mtsCommandAndEventTest()
{
    mtsManagerLocal::UnitTestEnabled = true;
#if !CISST_MTS_HAS_ICE
    mtsManagerLocal::UnitTestNetworkProxyEnabled = false;
#else
    mtsManagerLocal::UnitTestNetworkProxyEnabled = true;
#endif
}


void mtsCommandAndEventTest::setUp(void)
{
}


void mtsCommandAndEventTest::tearDown(void)
{
}

template <class _clientType, class _serverType>
void mtsCommandAndEventTest::TestExecution(_clientType * client, _serverType * server,
                                           double clientExecutionDelay, double serverExecutionDelay,
                                           double blockingDelay)
{
    mtsComponentManager * manager = mtsComponentManager::GetInstance();

    // we assume both client and servers use the same type
    typedef typename _serverType::value_type value_type;

    // add to manager and start all
    manager->AddComponent(client);
    manager->AddComponent(server);
    manager->Connect(client->GetName(), "r1", server->GetName(), "p1");
    manager->CreateAll();
    CPPUNIT_ASSERT(manager->WaitForStateAll(mtsComponentState::READY, TransitionDelay));
    manager->StartAll();
    CPPUNIT_ASSERT(manager->WaitForStateAll(mtsComponentState::ACTIVE, TransitionDelay));

    // test commands and timing
    const double queueingDelay = 10.0 * cmn_ms;
    const osaTimeServer & timeServer = manager->GetTimeServer();
    double startTime, stopTime;

    // check initial values
    CPPUNIT_ASSERT_EQUAL(-1, server->InterfaceProvided1.GetValue()); // initial value
    CPPUNIT_ASSERT_EQUAL(-1, client->InterfaceRequired1.GetValue()); // initial value

    // value we used to make sure commands are processed, default is
    // -1, void command set to 0
    value_type valueWrite, valueWritePlusOne, valueRead;
    valueWrite = 4;

    // loop over void and write commands to alternate blocking and non
    // blocking commands
    unsigned int index;
    for (index = 0; index < 3; index++) {
        // test void command non blocking
        startTime = timeServer.GetRelativeTime();
        client->InterfaceRequired1.FunctionVoid();
        stopTime = timeServer.GetRelativeTime();
        CPPUNIT_ASSERT((stopTime - startTime) <= queueingDelay); // make sure execution is fast
        osaSleep(serverExecutionDelay + blockingDelay); // time to dequeue and let command execute
        CPPUNIT_ASSERT_EQUAL(0,  server->InterfaceProvided1.GetValue()); // reset
        CPPUNIT_ASSERT_EQUAL(-1, client->InterfaceRequired1.GetValue()); // unchanged

        // test write command
        startTime = timeServer.GetRelativeTime();
        client->InterfaceRequired1.FunctionWrite(valueWrite);
        stopTime = timeServer.GetRelativeTime();
        CPPUNIT_ASSERT((stopTime - startTime) <= queueingDelay); // make sure execution is fast
        osaSleep(serverExecutionDelay + blockingDelay);  // time to dequeue and let command execute
        CPPUNIT_ASSERT(valueWrite == server->InterfaceProvided1.GetValue()); // set to new value
        CPPUNIT_ASSERT(-1 == client->InterfaceRequired1.GetValue()); // unchanged

        // test filtered write command
        startTime = timeServer.GetRelativeTime();
        client->InterfaceRequired1.FunctionFilteredWrite(valueWrite);
        stopTime = timeServer.GetRelativeTime();
        CPPUNIT_ASSERT((stopTime - startTime) <= queueingDelay); // make sure execution is fast
        osaSleep(serverExecutionDelay + blockingDelay);  // time to dequeue and let command execute
        CPPUNIT_ASSERT((valueWrite + 1) == server->InterfaceProvided1.GetValue()); // set to new value + 1 (filter)
        CPPUNIT_ASSERT(-1 == client->InterfaceRequired1.GetValue()); // unchanged

        // test void command blocking
        if (blockingDelay > 0.0) {
            startTime = timeServer.GetRelativeTime();
            client->InterfaceRequired1.FunctionVoid.ExecuteBlocking();
            stopTime = timeServer.GetRelativeTime();
            std::stringstream message;
            message << "Actual: " << (stopTime - startTime) << " >= " << (blockingDelay * 0.9);
            CPPUNIT_ASSERT_MESSAGE(message.str(), (stopTime - startTime) >= (blockingDelay * 0.9));
        } else {
            // no significant delay but result should be garanteed without sleep
            client->InterfaceRequired1.FunctionVoid.ExecuteBlocking();
        }
        CPPUNIT_ASSERT(0 == server->InterfaceProvided1.GetValue()); // reset
        CPPUNIT_ASSERT(-1 == client->InterfaceRequired1.GetValue()); // unchanged

        // test write command blocking
        if (blockingDelay > 0.0) {
            startTime = timeServer.GetRelativeTime();
            client->InterfaceRequired1.FunctionWrite.ExecuteBlocking(valueWrite);
            stopTime = timeServer.GetRelativeTime();
            std::stringstream message;
            message << "Actual: " << (stopTime - startTime) << " >= " << (blockingDelay * 0.9);
            CPPUNIT_ASSERT_MESSAGE(message.str(), (stopTime - startTime) >= (blockingDelay * 0.9));
        } else {
            // no significant delay but result should be garanteed without sleep
            client->InterfaceRequired1.FunctionWrite.ExecuteBlocking(valueWrite);
        }
        CPPUNIT_ASSERT(valueWrite == server->InterfaceProvided1.GetValue()); // set to new value
        CPPUNIT_ASSERT(-1 == client->InterfaceRequired1.GetValue()); // unchanged

        // test filtered write command blocking
        if (blockingDelay > 0.0) {
            startTime = timeServer.GetRelativeTime();
            client->InterfaceRequired1.FunctionFilteredWrite.ExecuteBlocking(valueWrite);
            stopTime = timeServer.GetRelativeTime();
            std::stringstream message;
            message << "Actual: " << (stopTime - startTime) << " >= " << (blockingDelay * 0.9);
            CPPUNIT_ASSERT_MESSAGE(message.str(), (stopTime - startTime) >= (blockingDelay * 0.9));
        } else {
            // no significant delay but result should be garanteed without sleep
            client->InterfaceRequired1.FunctionFilteredWrite.ExecuteBlocking(valueWrite);
        }
        CPPUNIT_ASSERT((valueWrite + 1) == server->InterfaceProvided1.GetValue()); // set to new value + 1 (filter)
        CPPUNIT_ASSERT(-1 == client->InterfaceRequired1.GetValue()); // unchanged

        // test void return command (always blocking)
        value_type result;
        if (blockingDelay > 0.0) {
            startTime = timeServer.GetRelativeTime();
            client->InterfaceRequired1.FunctionVoidReturn(result);
            stopTime = timeServer.GetRelativeTime();
            std::stringstream message;
            message << "Actual: " << (stopTime - startTime) << " >= " << (blockingDelay * 0.9);
            CPPUNIT_ASSERT_MESSAGE(message.str(), (stopTime - startTime) >= (blockingDelay * 0.9));
        } else {
            // no significant delay but result should be garanteed without sleep
            client->InterfaceRequired1.FunctionVoidReturn(result);
        }
        CPPUNIT_ASSERT(result == 1); // number was positive
        CPPUNIT_ASSERT((-(valueWrite + 1)) == server->InterfaceProvided1.GetValue()); // negated
        CPPUNIT_ASSERT(-1 == client->InterfaceRequired1.GetValue()); // unchanged

        // call write return to change sign of value back
        if (blockingDelay > 0.0) {
            startTime = timeServer.GetRelativeTime();
            valueWritePlusOne = valueWrite + 1;
            client->InterfaceRequired1.FunctionWriteReturn(valueWritePlusOne, valueRead);
            result = valueRead;
            stopTime = timeServer.GetRelativeTime();
            std::stringstream message;
            message << "Actual: " << (stopTime - startTime) << " >= " << (blockingDelay * 0.9);
            CPPUNIT_ASSERT_MESSAGE(message.str(), (stopTime - startTime) >= (blockingDelay * 0.9));
        } else {
            // no significant delay but result should be garanteed without sleep
            valueWritePlusOne = valueWrite + 1;
            client->InterfaceRequired1.FunctionWriteReturn(valueWritePlusOne, valueRead);
            result = valueRead;
        }
        CPPUNIT_ASSERT(result == -1); // number was negative
        CPPUNIT_ASSERT((valueWrite + 1) == server->InterfaceProvided1.GetValue()); // negated back
        CPPUNIT_ASSERT(-1 == client->InterfaceRequired1.GetValue()); // unchanged

    }

    // test read command
    valueRead = 0;
    client->InterfaceRequired1.FunctionRead(valueRead);
    CPPUNIT_ASSERT((valueWrite + 1) == valueRead);
    CPPUNIT_ASSERT((valueWrite + 1) == server->InterfaceProvided1.GetValue()); // unchanged
    CPPUNIT_ASSERT(-1 == client->InterfaceRequired1.GetValue()); // unchanged

    // test qualified read command
    valueRead = 0;
    client->InterfaceRequired1.FunctionQualifiedRead(valueWrite, valueRead);
    CPPUNIT_ASSERT((valueWrite + 1) == valueRead);
    CPPUNIT_ASSERT((valueWrite + 1) == server->InterfaceProvided1.GetValue()); // unchanged
    CPPUNIT_ASSERT(-1 == client->InterfaceRequired1.GetValue()); // unchanged

    // test void event
    server->InterfaceProvided1.EventVoid();
    osaSleep(clientExecutionDelay);
    CPPUNIT_ASSERT((valueWrite + 1) == server->InterfaceProvided1.GetValue()); // unchanged
    CPPUNIT_ASSERT(0 == client->InterfaceRequired1.GetValue()); // reset by void event

    // test write event
    server->InterfaceProvided1.EventWrite(valueWrite);
    osaSleep(clientExecutionDelay);
    CPPUNIT_ASSERT((valueWrite + 1) == server->InterfaceProvided1.GetValue()); // unchanged
    CPPUNIT_ASSERT(valueWrite == client->InterfaceRequired1.GetValue()); // set by write event

    // stop all and cleanup
    manager->KillAll();
    CPPUNIT_ASSERT(manager->WaitForStateAll(mtsComponentState::FINISHED, TransitionDelay));
    manager->Disconnect(client->GetName(), "r1", server->GetName(), "p1");
    manager->RemoveComponent(client);
    manager->RemoveComponent(server);
}


template <class _elementType>
void mtsCommandAndEventTest::TestLocalDeviceDevice(void)
{
    mtsTestDevice2<_elementType> * client = new mtsTestDevice2<_elementType>;
    mtsTestDevice3<_elementType> * server = new mtsTestDevice3<_elementType>;
    TestExecution(client, server, 0.0, 0.0);
    delete client;
    delete server;
}
void mtsCommandAndEventTest::TestLocalDeviceDevice_mtsInt(void) {
    mtsCommandAndEventTest::TestLocalDeviceDevice<mtsInt>();
}
void mtsCommandAndEventTest::TestLocalDeviceDevice_int(void) {
    mtsCommandAndEventTest::TestLocalDeviceDevice<int>();
}


template <class _elementType>
void mtsCommandAndEventTest::TestLocalPeriodicPeriodic(void)
{
    mtsTestPeriodic1<_elementType> * client = new mtsTestPeriodic1<_elementType>("mtsTestPeriodic1Client");
    mtsTestPeriodic1<_elementType> * server = new mtsTestPeriodic1<_elementType>("mtsTestPeriodic1Server");
    // these delays are OS dependent, we might need to increase them later
    const double clientExecutionDelay = 0.1 * cmn_s;
    const double serverExecutionDelay = 0.1 * cmn_s;
    TestExecution(client, server, clientExecutionDelay, serverExecutionDelay);
    delete client;
    delete server;
}
void mtsCommandAndEventTest::TestLocalPeriodicPeriodic_mtsInt(void) {
    mtsCommandAndEventTest::TestLocalPeriodicPeriodic<mtsInt>();
}
void mtsCommandAndEventTest::TestLocalPeriodicPeriodic_int(void) {
    mtsCommandAndEventTest::TestLocalPeriodicPeriodic<int>();
}


template <class _elementType>
void mtsCommandAndEventTest::TestLocalContinuousContinuous(void)
{
    mtsTestContinuous1<_elementType> * client = new mtsTestContinuous1<_elementType>("mtsTestContinuous1Client");
    mtsTestContinuous1<_elementType> * server = new mtsTestContinuous1<_elementType>("mtsTestContinuous1Server");
    // these delays are OS dependent, we might need to increase them later
    const double clientExecutionDelay = 0.1 * cmn_s;
    const double serverExecutionDelay = 0.1 * cmn_s;
    TestExecution(client, server, clientExecutionDelay, serverExecutionDelay);
    delete client;
    delete server;
}
void mtsCommandAndEventTest::TestLocalContinuousContinuous_mtsInt(void) {
    mtsCommandAndEventTest::TestLocalContinuousContinuous<mtsInt>();
}
void mtsCommandAndEventTest::TestLocalContinuousContinuous_int(void) {
    mtsCommandAndEventTest::TestLocalContinuousContinuous<int>();
}


template <class _elementType>
void mtsCommandAndEventTest::TestLocalFromCallbackFromCallback(void)
{
    mtsTestFromCallback1<_elementType> * client = new mtsTestFromCallback1<_elementType>("mtsTestFromCallback1Client");
    mtsTestCallbackTrigger * clientTrigger = new mtsTestCallbackTrigger(client);
    mtsTestFromCallback1<_elementType> * server = new mtsTestFromCallback1<_elementType>("mtsTestFromCallback1Server");
    mtsTestCallbackTrigger * serverTrigger = new mtsTestCallbackTrigger(server);
    // these delays are OS dependent, we might need to increase them later
    const double clientExecutionDelay = 0.1 * cmn_s;
    const double serverExecutionDelay = 0.1 * cmn_s;
    TestExecution(client, server, clientExecutionDelay, serverExecutionDelay);
    clientTrigger->Stop();
    delete clientTrigger;
    delete client;
    serverTrigger->Stop();
    delete serverTrigger;
    delete server;
}
void mtsCommandAndEventTest::TestLocalFromCallbackFromCallback_mtsInt(void) {
    mtsCommandAndEventTest::TestLocalFromCallbackFromCallback<mtsInt>();
}
void mtsCommandAndEventTest::TestLocalFromCallbackFromCallback_int(void) {
    mtsCommandAndEventTest::TestLocalFromCallbackFromCallback<int>();
}


template <class _elementType>
void mtsCommandAndEventTest::TestLocalFromSignalFromSignal(void)
{
    mtsTestFromSignal1<_elementType> * client = new mtsTestFromSignal1<_elementType>("mtsTestFromSignal1Client");
    mtsTestFromSignal1<_elementType> * server = new mtsTestFromSignal1<_elementType>("mtsTestFromSignal1Server");
    // these delays are OS dependent, we might need to increase them later
    const double clientExecutionDelay = 0.1 * cmn_s;
    const double serverExecutionDelay = 0.1 * cmn_s;
    TestExecution(client, server, clientExecutionDelay, serverExecutionDelay);
    delete client;
    delete server;
}
void mtsCommandAndEventTest::TestLocalFromSignalFromSignal_mtsInt(void) {
    mtsCommandAndEventTest::TestLocalFromSignalFromSignal<mtsInt>();
}
void mtsCommandAndEventTest::TestLocalFromSignalFromSignal_int(void) {
    mtsCommandAndEventTest::TestLocalFromSignalFromSignal<int>();
}


template <class _elementType>
void mtsCommandAndEventTest::TestLocalPeriodicPeriodicBlocking(void)
{
    const double blockingDelay = 0.5 * cmn_s;
    mtsTestPeriodic1<_elementType> * client = new mtsTestPeriodic1<_elementType>("mtsTestPeriodic1Client");
    mtsTestPeriodic1<_elementType> * server = new mtsTestPeriodic1<_elementType>("mtsTestPeriodic1Server", blockingDelay);
    // these delays are OS dependent, we might need to increase them later
    const double clientExecutionDelay = 0.1 * cmn_s;
    const double serverExecutionDelay = 0.1 * cmn_s;
    TestExecution(client, server, clientExecutionDelay, serverExecutionDelay, blockingDelay);
    delete client;
    delete server;
}
void mtsCommandAndEventTest::TestLocalPeriodicPeriodicBlocking_mtsInt(void) {
    mtsCommandAndEventTest::TestLocalPeriodicPeriodicBlocking<mtsInt>();
}
void mtsCommandAndEventTest::TestLocalPeriodicPeriodicBlocking_int(void) {
    mtsCommandAndEventTest::TestLocalPeriodicPeriodicBlocking<int>();
}


template <class _elementType>
void mtsCommandAndEventTest::TestLocalContinuousContinuousBlocking(void)
{
    const double blockingDelay = 0.5 * cmn_s;
    mtsTestContinuous1<_elementType> * client = new mtsTestContinuous1<_elementType>("mtsTestContinuous1Client");
    mtsTestContinuous1<_elementType> * server = new mtsTestContinuous1<_elementType>("mtsTestContinuous1Server", blockingDelay);
    // these delays are OS dependent, we might need to increase them later
    const double clientExecutionDelay = 0.1 * cmn_s;
    const double serverExecutionDelay = 0.1 * cmn_s;
    TestExecution(client, server, clientExecutionDelay, serverExecutionDelay, blockingDelay);
    delete client;
    delete server;
}
void mtsCommandAndEventTest::TestLocalContinuousContinuousBlocking_mtsInt(void) {
    mtsCommandAndEventTest::TestLocalContinuousContinuousBlocking<mtsInt>();
}
void mtsCommandAndEventTest::TestLocalContinuousContinuousBlocking_int(void) {
    mtsCommandAndEventTest::TestLocalContinuousContinuousBlocking<int>();
}


template <class _elementType>
void mtsCommandAndEventTest::TestLocalFromCallbackFromCallbackBlocking(void)
{
    const double blockingDelay = 0.5 * cmn_s;
    mtsTestFromCallback1<_elementType> * client = new mtsTestFromCallback1<_elementType>("mtsTestFromCallback1Client");
    mtsTestCallbackTrigger * clientTrigger = new mtsTestCallbackTrigger(client);
    mtsTestFromCallback1<_elementType> * server = new mtsTestFromCallback1<_elementType>("mtsTestFromCallback1Server", blockingDelay);
    mtsTestCallbackTrigger * serverTrigger = new mtsTestCallbackTrigger(server);
    // these delays are OS dependent, we might need to increase them later
    const double clientExecutionDelay = 0.1 * cmn_s;
    const double serverExecutionDelay = 0.1 * cmn_s;
    TestExecution(client, server, clientExecutionDelay, serverExecutionDelay, blockingDelay);
    clientTrigger->Stop();
    delete clientTrigger;
    delete client;
    serverTrigger->Stop();
    delete serverTrigger;
    delete server;
}
void mtsCommandAndEventTest::TestLocalFromCallbackFromCallbackBlocking_mtsInt(void) {
    mtsCommandAndEventTest::TestLocalFromCallbackFromCallbackBlocking<mtsInt>();
}
void mtsCommandAndEventTest::TestLocalFromCallbackFromCallbackBlocking_int(void) {
    mtsCommandAndEventTest::TestLocalFromCallbackFromCallbackBlocking<int>();
}


template <class _elementType>
void mtsCommandAndEventTest::TestLocalFromSignalFromSignalBlocking(void)
{
    const double blockingDelay = 0.5 * cmn_s;
    mtsTestFromSignal1<_elementType> * client = new mtsTestFromSignal1<_elementType>("mtsTestFromSignal1Client");
    mtsTestFromSignal1<_elementType> * server = new mtsTestFromSignal1<_elementType>("mtsTestFromSignal1Server", blockingDelay);
    // these delays are OS dependent, we might need to increase them later
    const double clientExecutionDelay = 0.1 * cmn_s;
    const double serverExecutionDelay = 0.1 * cmn_s;
    TestExecution(client, server, clientExecutionDelay, serverExecutionDelay, blockingDelay);
    delete client;
    delete server;
}
void mtsCommandAndEventTest::TestLocalFromSignalFromSignalBlocking_mtsInt(void) {
    mtsCommandAndEventTest::TestLocalFromSignalFromSignalBlocking<mtsInt>();
}
void mtsCommandAndEventTest::TestLocalFromSignalFromSignalBlocking_int(void) {
    mtsCommandAndEventTest::TestLocalFromSignalFromSignalBlocking<int>();
}


template <class _elementType>
void mtsCommandAndEventTest::TestArgumentPrototypes(void)
{
    typedef _elementType value_type;

    mtsTestContinuous1<value_type> * client = new mtsTestContinuous1<value_type>("mtsTestContinuous1Client");
    mtsTestContinuous1<value_type> * server = new mtsTestContinuous1<value_type>("mtsTestContinuous1Server");

    mtsComponentManager * manager = mtsComponentManager::GetInstance();

    // add to manager and start all
    manager->AddComponent(client);
    manager->AddComponent(server);
    manager->Connect(client->GetName(), "r1", server->GetName(), "p1");
    manager->CreateAll();
    CPPUNIT_ASSERT(manager->WaitForStateAll(mtsComponentState::READY, TransitionDelay));
    manager->StartAll();
    CPPUNIT_ASSERT(manager->WaitForStateAll(mtsComponentState::ACTIVE, TransitionDelay));

    // test that values are set properly for all argument prototypes
    const mtsGenericObject * argumentPrototypeGeneric;
    const mtsInt * argumentPrototype;
    int expected, actual;

    // test argument prototype for void with result command
    argumentPrototypeGeneric = client->InterfaceRequired1.FunctionVoidReturn.GetResultPrototype();
    CPPUNIT_ASSERT(argumentPrototypeGeneric != 0);
    argumentPrototype = dynamic_cast<const mtsInt *>(argumentPrototypeGeneric);
    CPPUNIT_ASSERT(argumentPrototype != 0);
    expected = mtsTestInterfaceProvided<mtsInt>::Argument2PrototypeDefault;
    actual = *argumentPrototype;
    CPPUNIT_ASSERT_EQUAL(expected, actual);

    // test argument prototype for write command
    argumentPrototypeGeneric = client->InterfaceRequired1.FunctionWrite.GetArgumentPrototype();
    CPPUNIT_ASSERT(argumentPrototypeGeneric != 0);
    argumentPrototype = dynamic_cast<const mtsInt *>(argumentPrototypeGeneric);
    CPPUNIT_ASSERT(argumentPrototype != 0);
    expected = mtsTestInterfaceProvided<mtsInt>::Argument1PrototypeDefault;
    actual = *argumentPrototype;
    CPPUNIT_ASSERT_EQUAL(expected, actual);

    // test argument prototype for read command
    argumentPrototypeGeneric = client->InterfaceRequired1.FunctionRead.GetArgumentPrototype();
    CPPUNIT_ASSERT(argumentPrototypeGeneric != 0);
    argumentPrototype = dynamic_cast<const mtsInt *>(argumentPrototypeGeneric);
    CPPUNIT_ASSERT(argumentPrototype != 0);
    expected = mtsTestInterfaceProvided<mtsInt>::Argument1PrototypeDefault;
    actual = *argumentPrototype;
    CPPUNIT_ASSERT_EQUAL(expected, actual);

    // test argument prototype for qualified read command
    argumentPrototypeGeneric = client->InterfaceRequired1.FunctionQualifiedRead.GetArgument1Prototype();
    CPPUNIT_ASSERT(argumentPrototypeGeneric != 0);
    argumentPrototype = dynamic_cast<const mtsInt *>(argumentPrototypeGeneric);
    CPPUNIT_ASSERT(argumentPrototype != 0);
    expected = mtsTestInterfaceProvided<mtsInt>::Argument1PrototypeDefault;
    actual = *argumentPrototype;
    CPPUNIT_ASSERT_EQUAL(expected, actual);

    argumentPrototypeGeneric = client->InterfaceRequired1.FunctionQualifiedRead.GetArgument2Prototype();
    CPPUNIT_ASSERT(argumentPrototypeGeneric != 0);
    argumentPrototype = dynamic_cast<const mtsInt *>(argumentPrototypeGeneric);
    CPPUNIT_ASSERT(argumentPrototype != 0);
    expected = mtsTestInterfaceProvided<mtsInt>::Argument2PrototypeDefault;
    actual = *argumentPrototype;
    CPPUNIT_ASSERT_EQUAL(expected, actual);

    // stop all and cleanup
    manager->KillAll();
    CPPUNIT_ASSERT(manager->WaitForStateAll(mtsComponentState::FINISHED, TransitionDelay));
    manager->Disconnect(client->GetName(), "r1", server->GetName(), "p1");
    manager->RemoveComponent(client);
    manager->RemoveComponent(server);

    delete client;
    delete server;
}
void mtsCommandAndEventTest::TestArgumentPrototypes_mtsInt(void) {
    mtsCommandAndEventTest::TestArgumentPrototypes<mtsInt>();
}
void mtsCommandAndEventTest::TestArgumentPrototypes_int(void) {
    mtsCommandAndEventTest::TestArgumentPrototypes<int>();
}


/*
void mtsCommandAndEventTest::TestRemoteDeviceDevice(void)
{
    mtsManagerGlobal managerGlobal;

    // Prepare local managers for this test
    mtsTestDevice1 * P1C1 = new mtsTestDevice1;
    mtsTestDevice2 * P1C2 = new mtsTestDevice2;
    mtsTestDevice2 * P2C2 = new mtsTestDevice2;
    mtsTestDevice3 * P2C3 = new mtsTestDevice3;

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
    CPPUNIT_ASSERT_EQUAL(-1, P2C2->InterfaceRequired1.GetValue());
    CPPUNIT_ASSERT_EQUAL(-1, P2C2->InterfaceProvided1.GetValue());
    CPPUNIT_ASSERT_EQUAL(-1, P2C2->InterfaceProvided2.GetValue());
    CPPUNIT_ASSERT_EQUAL(-1, P2C3->InterfaceRequired1.GetValue());

    // Test void command
    P2C3->InterfaceRequired1.CommandVoid();
    //CPPUNIT_ASSERT_EQUAL(-1, P2C2->InterfaceRequired1.GetValue());
    //CPPUNIT_ASSERT_EQUAL(-1, P2C2->InterfaceProvided1.GetValue());
    //CPPUNIT_ASSERT_EQUAL(0,  P2C2->InterfaceProvided2.GetValue());
    //CPPUNIT_ASSERT_EQUAL(-1, P2C3->InterfaceRequired1.GetValue());

    // Test write command
    //mtsInt valueWrite;
    //valueWrite.Data = 2;
    //P2C3->InterfaceRequired1.CommandWrite(valueWrite);
    //CPPUNIT_ASSERT_EQUAL(-1, P2C2->InterfaceRequired1.GetValue());
    //CPPUNIT_ASSERT_EQUAL(-1, P2C2->InterfaceProvided1.GetValue());
    //CPPUNIT_ASSERT_EQUAL(valueWrite.Data,  P2C2->InterfaceProvided2.GetValue());
    //CPPUNIT_ASSERT_EQUAL(-1, P2C3->InterfaceRequired1.GetValue());

    //// Test read command
    //mtsInt valueRead;
    //valueRead.Data = 0;
    //P2C3->InterfaceRequired1.CommandRead(valueRead);
    //CPPUNIT_ASSERT_EQUAL(valueWrite.Data, valueRead.Data);
    //CPPUNIT_ASSERT_EQUAL(-1, P2C2->InterfaceRequired1.GetValue());
    //CPPUNIT_ASSERT_EQUAL(-1, P2C2->InterfaceProvided1.GetValue());
    //CPPUNIT_ASSERT_EQUAL(valueWrite.Data,  P2C2->InterfaceProvided2.GetValue());
    //CPPUNIT_ASSERT_EQUAL(-1, P2C3->InterfaceRequired1.GetValue());

    //// Test qualified read command
    //valueWrite.Data = 3;
    //valueRead.Data = 0;
    //P2C3->InterfaceRequired1.CommandQualifiedRead(valueWrite, valueRead);
    //CPPUNIT_ASSERT_EQUAL(-1, P2C2->InterfaceRequired1.GetValue());
    //CPPUNIT_ASSERT_EQUAL(-1, P2C2->InterfaceProvided1.GetValue());
    //CPPUNIT_ASSERT_EQUAL(-1, P2C3->InterfaceRequired1.GetValue());

    //// Test void event
    //P2C2->InterfaceProvided2.EventVoid();
    //CPPUNIT_ASSERT_EQUAL(-1, P2C2->InterfaceRequired1.GetValue());
    //CPPUNIT_ASSERT_EQUAL(-1, P2C2->InterfaceProvided1.GetValue());
    //CPPUNIT_ASSERT_EQUAL(0, P2C3->InterfaceRequired1.GetValue());

    //// Test write event
    //valueWrite.Data = 4;
    //P2C2->InterfaceProvided2.EventWrite(valueWrite);
    //CPPUNIT_ASSERT_EQUAL(-1, P2C2->InterfaceRequired1.GetValue());
    //CPPUNIT_ASSERT_EQUAL(-1, P2C2->InterfaceProvided1.GetValue());
    //CPPUNIT_ASSERT_EQUAL(valueWrite.Data, P2C3->InterfaceRequired1.GetValue());
}
*/

CPPUNIT_TEST_SUITE_REGISTRATION(mtsCommandAndEventTest);
