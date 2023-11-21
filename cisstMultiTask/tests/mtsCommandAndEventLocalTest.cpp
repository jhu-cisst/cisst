/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung, Anton Deguet
  Created on: 2009-11-17

  (C) Copyright 2009-2022 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "mtsCommandAndEventLocalTest.h"

#include <cisstMultiTask/mtsManagerLocal.h>

#include "mtsTestComponents.h"


mtsCommandAndEventLocalTest::mtsCommandAndEventLocalTest()
{
}


void mtsCommandAndEventLocalTest::setUp(void)
{
}


void mtsCommandAndEventLocalTest::tearDown(void)
{
}

template <class _clientType, class _serverType>
void mtsCommandAndEventLocalTest::TestExecution(_clientType * client, _serverType * server,
                                                double clientExecutionDelay, double serverExecutionDelay,
                                                double blockingDelay)
{
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClassAll(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);

    // execution result used by all functions
    mtsExecutionResult executionResult;

    // we assume both client and servers use the same type
    typedef typename _serverType::value_type value_type;

    // value we used to make sure commands are processed, default is
    // -1, void command set to 0
    value_type valueWrite, valueWritePlusOne, valueRead;
    valueWrite = 4;

    // test functions not connected
    CPPUNIT_ASSERT(!client->InterfaceRequired1.FunctionVoid.IsValid());
    executionResult = client->InterfaceRequired1.FunctionVoid();
    CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::FUNCTION_NOT_BOUND, executionResult.GetResult());
    executionResult = client->InterfaceRequired1.FunctionVoid.ExecuteBlocking();
    CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::FUNCTION_NOT_BOUND, executionResult.GetResult());

    CPPUNIT_ASSERT(!client->InterfaceRequired1.FunctionWrite.IsValid());
    executionResult = client->InterfaceRequired1.FunctionWrite(valueWrite);
    CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::FUNCTION_NOT_BOUND, executionResult.GetResult());
    executionResult = client->InterfaceRequired1.FunctionWrite.ExecuteBlocking(valueWrite);
    CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::FUNCTION_NOT_BOUND, executionResult.GetResult());

    CPPUNIT_ASSERT(!client->InterfaceRequired1.FunctionFilteredWrite.IsValid());
    executionResult = client->InterfaceRequired1.FunctionFilteredWrite(valueWrite);
    CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::FUNCTION_NOT_BOUND, executionResult.GetResult());
    executionResult = client->InterfaceRequired1.FunctionFilteredWrite.ExecuteBlocking(valueWrite);
    CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::FUNCTION_NOT_BOUND, executionResult.GetResult());

    CPPUNIT_ASSERT(!client->InterfaceRequired1.FunctionVoidReturn.IsValid());
    executionResult = client->InterfaceRequired1.FunctionVoidReturn(valueRead);
    CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::FUNCTION_NOT_BOUND, executionResult.GetResult());

    CPPUNIT_ASSERT(!client->InterfaceRequired1.FunctionWriteReturn.IsValid());
    executionResult = client->InterfaceRequired1.FunctionWriteReturn(valueWritePlusOne, valueRead);
    CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::FUNCTION_NOT_BOUND, executionResult.GetResult());

    CPPUNIT_ASSERT(!client->InterfaceRequired1.FunctionRead.IsValid());
    executionResult = client->InterfaceRequired1.FunctionRead(valueRead);
    CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::FUNCTION_NOT_BOUND, executionResult.GetResult());

    CPPUNIT_ASSERT(!client->InterfaceRequired1.FunctionQualifiedRead.IsValid());
    executionResult = client->InterfaceRequired1.FunctionQualifiedRead(valueWrite, valueRead);
    CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::FUNCTION_NOT_BOUND, executionResult.GetResult());

    CPPUNIT_ASSERT(!client->InterfaceRequired1.FunctionStateTableRead.IsValid());
    executionResult = client->InterfaceRequired1.FunctionStateTableRead(valueRead);
    CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::FUNCTION_NOT_BOUND, executionResult.GetResult());

    CPPUNIT_ASSERT(!client->InterfaceRequired1.FunctionStateTableFilteredReadGenericR.IsValid());
    executionResult = client->InterfaceRequired1.FunctionStateTableFilteredReadGenericR(valueRead);
    CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::FUNCTION_NOT_BOUND, executionResult.GetResult());

    CPPUNIT_ASSERT(!client->InterfaceRequired1.FunctionStateTableFilteredReadGenericVR.IsValid());
    executionResult = client->InterfaceRequired1.FunctionStateTableFilteredReadGenericVR(valueRead);
    CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::FUNCTION_NOT_BOUND, executionResult.GetResult());

    CPPUNIT_ASSERT(!client->InterfaceRequired1.FunctionStateTableFilteredReadGenericCF.IsValid());
    executionResult = client->InterfaceRequired1.FunctionStateTableFilteredReadGenericCF(valueRead);
    CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::FUNCTION_NOT_BOUND, executionResult.GetResult());

#if 0
    CPPUNIT_ASSERT(!client->InterfaceRequired1.FunctionStateTableFilteredReadV3VR.IsValid());
    executionResult = client->InterfaceRequired1.FunctionStateTableFilteredReadV3VR(valueRead);
    CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::FUNCTION_NOT_BOUND, executionResult.GetResult());
#endif

    CPPUNIT_ASSERT(!client->InterfaceRequired1.FunctionStateTableFilteredReadV3CF.IsValid());
    executionResult = client->InterfaceRequired1.FunctionStateTableFilteredReadV3CF(valueRead);
    CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::FUNCTION_NOT_BOUND, executionResult.GetResult());

    CPPUNIT_ASSERT(!client->InterfaceRequired1.FunctionStateTableAdvance.IsValid());
    executionResult = client->InterfaceRequired1.FunctionStateTableAdvance();
    CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::FUNCTION_NOT_BOUND, executionResult.GetResult());

    mtsManagerLocal * manager = mtsManagerLocal::GetInstance();
    manager->RemoveAllUserComponents();

    // add to manager and start all
    CPPUNIT_ASSERT(manager->AddComponent(client));
    CPPUNIT_ASSERT(manager->AddComponent(server));
    CPPUNIT_ASSERT(manager->Connect(client->GetName(), "r1", server->GetName(), "p1"));
    manager->CreateAll();
    CPPUNIT_ASSERT(manager->WaitForStateAll(mtsComponentState::READY, StateTransitionMaximumDelay));
    manager->StartAll();
    CPPUNIT_ASSERT(manager->WaitForStateAll(mtsComponentState::ACTIVE, StateTransitionMaximumDelay));

    // test commands and timing
    const double queueingDelay = 10.0 * cmn_ms;
    const osaTimeServer & timeServer = manager->GetTimeServer();
    double startTime, stopTime;

    // check initial values
    CPPUNIT_ASSERT_EQUAL(-1, server->InterfaceProvided1.GetValue()); // initial value
    CPPUNIT_ASSERT_EQUAL(-1, client->InterfaceRequired1.GetValue()); // initial value

    // check that all functions are bound
    CPPUNIT_ASSERT(client->InterfaceRequired1.FunctionVoid.IsValid());
    CPPUNIT_ASSERT(client->InterfaceRequired1.FunctionWrite.IsValid());
    CPPUNIT_ASSERT(client->InterfaceRequired1.FunctionFilteredWrite.IsValid());
    CPPUNIT_ASSERT(client->InterfaceRequired1.FunctionVoidReturn.IsValid());
    CPPUNIT_ASSERT(client->InterfaceRequired1.FunctionWriteReturn.IsValid());
    CPPUNIT_ASSERT(client->InterfaceRequired1.FunctionRead.IsValid());
    CPPUNIT_ASSERT(client->InterfaceRequired1.FunctionQualifiedRead.IsValid());

    // loop over void and write commands to alternate blocking and non
    // blocking commands
    unsigned int index;
    for (index = 0; index < 3; index++) {
        // test void command non blocking
        startTime = timeServer.GetRelativeTime();
        executionResult = client->InterfaceRequired1.FunctionVoid();
        CPPUNIT_ASSERT(executionResult.IsOK()); // queued or succeeded
        stopTime = timeServer.GetRelativeTime();
        CPPUNIT_ASSERT((stopTime - startTime) <= queueingDelay); // make sure execution is fast
        osaSleep(serverExecutionDelay + blockingDelay); // time to dequeue and let command execute
        CPPUNIT_ASSERT_EQUAL(0,  server->InterfaceProvided1.GetValue()); // reset
        CPPUNIT_ASSERT_EQUAL(-1, client->InterfaceRequired1.GetValue()); // unchanged

        // test write command
        startTime = timeServer.GetRelativeTime();
        executionResult = client->InterfaceRequired1.FunctionWrite(valueWrite);
        CPPUNIT_ASSERT(executionResult.IsOK()); // queued or succeeded
        stopTime = timeServer.GetRelativeTime();
        CPPUNIT_ASSERT((stopTime - startTime) <= queueingDelay); // make sure execution is fast
        osaSleep(serverExecutionDelay + blockingDelay);  // time to dequeue and let command execute
        CPPUNIT_ASSERT(valueWrite == server->InterfaceProvided1.GetValue()); // set to new value
        CPPUNIT_ASSERT(-1 == client->InterfaceRequired1.GetValue()); // unchanged

        // test filtered write command
        startTime = timeServer.GetRelativeTime();
        executionResult = client->InterfaceRequired1.FunctionFilteredWrite(valueWrite);
        CPPUNIT_ASSERT(executionResult.IsOK()); // queued or succeeded
        stopTime = timeServer.GetRelativeTime();
        CPPUNIT_ASSERT((stopTime - startTime) <= queueingDelay); // make sure execution is fast
        osaSleep(serverExecutionDelay + blockingDelay);  // time to dequeue and let command execute
        CPPUNIT_ASSERT((valueWrite + 1) == server->InterfaceProvided1.GetValue()); // set to new value + 1 (filter)
        CPPUNIT_ASSERT(-1 == client->InterfaceRequired1.GetValue()); // unchanged

        // test void command blocking
        if (blockingDelay > 0.0) {
            startTime = timeServer.GetRelativeTime();
            executionResult = client->InterfaceRequired1.FunctionVoid.ExecuteBlocking();
            stopTime = timeServer.GetRelativeTime();
            CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::COMMAND_SUCCEEDED, executionResult.GetResult());
            std::stringstream message;
            message << "Actual: " << (stopTime - startTime) << " >= " << (blockingDelay * 0.9);
            CPPUNIT_ASSERT_MESSAGE(message.str(), (stopTime - startTime) >= (blockingDelay * 0.9));
        } else {
            // no significant delay but result should be guaranteed without sleep
            executionResult = client->InterfaceRequired1.FunctionVoid.ExecuteBlocking();
            CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::COMMAND_SUCCEEDED, executionResult.GetResult());
        }
        CPPUNIT_ASSERT(0 == server->InterfaceProvided1.GetValue()); // reset
        CPPUNIT_ASSERT(-1 == client->InterfaceRequired1.GetValue()); // unchanged

        // test write command blocking
        if (blockingDelay > 0.0) {
            startTime = timeServer.GetRelativeTime();
            executionResult = client->InterfaceRequired1.FunctionWrite.ExecuteBlocking(valueWrite);
            stopTime = timeServer.GetRelativeTime();
            CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::COMMAND_SUCCEEDED, executionResult.GetResult());
            std::stringstream message;
            message << "Actual: " << (stopTime - startTime) << " >= " << (blockingDelay * 0.9);
            CPPUNIT_ASSERT_MESSAGE(message.str(), (stopTime - startTime) >= (blockingDelay * 0.9));
        } else {
            // no significant delay but result should be guaranteed without sleep
            executionResult = client->InterfaceRequired1.FunctionWrite.ExecuteBlocking(valueWrite);
            CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::COMMAND_SUCCEEDED, executionResult.GetResult());
        }
        CPPUNIT_ASSERT(valueWrite == server->InterfaceProvided1.GetValue()); // set to new value
        CPPUNIT_ASSERT(-1 == client->InterfaceRequired1.GetValue()); // unchanged

        // test filtered write command blocking
        if (blockingDelay > 0.0) {
            startTime = timeServer.GetRelativeTime();
            executionResult = client->InterfaceRequired1.FunctionFilteredWrite.ExecuteBlocking(valueWrite);
            stopTime = timeServer.GetRelativeTime();
            CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::COMMAND_SUCCEEDED, executionResult.GetResult());
            std::stringstream message;
            message << "Actual: " << (stopTime - startTime) << " >= " << (blockingDelay * 0.9);
            CPPUNIT_ASSERT_MESSAGE(message.str(), (stopTime - startTime) >= (blockingDelay * 0.9));
        } else {
            // no significant delay but result should be guaranteed without sleep
            executionResult = client->InterfaceRequired1.FunctionFilteredWrite.ExecuteBlocking(valueWrite);
            CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::COMMAND_SUCCEEDED, executionResult.GetResult());
        }
        CPPUNIT_ASSERT((valueWrite + 1) == server->InterfaceProvided1.GetValue()); // set to new value + 1 (filter)
        CPPUNIT_ASSERT(-1 == client->InterfaceRequired1.GetValue()); // unchanged

        // test void return command (always blocking)
        if (blockingDelay > 0.0) {
            startTime = timeServer.GetRelativeTime();
            executionResult = client->InterfaceRequired1.FunctionVoidReturn(valueRead);
            stopTime = timeServer.GetRelativeTime();
            CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::COMMAND_SUCCEEDED, executionResult.GetResult());
            std::stringstream message;
            message << "Actual: " << (stopTime - startTime) << " >= " << (blockingDelay * 0.9);
            CPPUNIT_ASSERT_MESSAGE(message.str(), (stopTime - startTime) >= (blockingDelay * 0.9));
        } else {
            // no significant delay but result should be guaranteed without sleep
            executionResult = client->InterfaceRequired1.FunctionVoidReturn(valueRead);
            CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::COMMAND_SUCCEEDED, executionResult.GetResult());
        }
        CPPUNIT_ASSERT(valueRead == 1); // number was positive
        CPPUNIT_ASSERT((-(valueWrite + 1)) == server->InterfaceProvided1.GetValue()); // negated
        CPPUNIT_ASSERT(-1 == client->InterfaceRequired1.GetValue()); // unchanged

        // call write return to change sign of value back
        valueWritePlusOne = valueWrite + 1;
        if (blockingDelay > 0.0) {
            startTime = timeServer.GetRelativeTime();
            executionResult = client->InterfaceRequired1.FunctionWriteReturn(valueWritePlusOne, valueRead);
            stopTime = timeServer.GetRelativeTime();
            CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::COMMAND_SUCCEEDED, executionResult.GetResult());
            std::stringstream message;
            message << "Actual: " << (stopTime - startTime) << " >= " << (blockingDelay * 0.9);
            CPPUNIT_ASSERT_MESSAGE(message.str(), (stopTime - startTime) >= (blockingDelay * 0.9));
        } else {
            // no significant delay but result should be guaranteed without sleep
            executionResult = client->InterfaceRequired1.FunctionWriteReturn(valueWritePlusOne, valueRead);
            CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::COMMAND_SUCCEEDED, executionResult.GetResult());
        }
        CPPUNIT_ASSERT(valueRead == -1); // number was negative
        CPPUNIT_ASSERT((valueWrite + 1) == server->InterfaceProvided1.GetValue()); // negated back
        CPPUNIT_ASSERT(-1 == client->InterfaceRequired1.GetValue()); // unchanged

        // test the state table read, first advance (blocking to make sure it happened)
        client->InterfaceRequired1.FunctionStateTableAdvance.ExecuteBlocking();
        client->InterfaceRequired1.FunctionStateTableRead(valueRead);
        CPPUNIT_ASSERT(valueRead == static_cast<int>(index + 1));
        valueRead = 0;
        client->InterfaceRequired1.FunctionStateTableFilteredReadGenericR(valueRead);
        CPPUNIT_ASSERT(valueRead == static_cast<int>(index + 1));
        valueRead = 0;
        client->InterfaceRequired1.FunctionStateTableFilteredReadGenericVR(valueRead);
        CPPUNIT_ASSERT(valueRead == static_cast<int>(index + 1));
        valueRead = 0;
        client->InterfaceRequired1.FunctionStateTableFilteredReadGenericCF(valueRead);
        CPPUNIT_ASSERT(valueRead == static_cast<int>(index + 1));
#if 0
        // Not currently supported
        valueRead = 0;
        client->InterfaceRequired1.FunctionStateTableFilteredReadV3VR(valueRead);
        CPPUNIT_ASSERT(valueRead == static_cast<int>(3*(index + 1)));
#endif
        valueRead = 0;
        client->InterfaceRequired1.FunctionStateTableFilteredReadV3CF(valueRead);
        CPPUNIT_ASSERT(valueRead == static_cast<int>(3*(index + 1)));
    }

    // test read command
    valueRead = 0;
    executionResult = client->InterfaceRequired1.FunctionRead(valueRead);
    CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::COMMAND_SUCCEEDED, executionResult.GetResult());
    CPPUNIT_ASSERT((valueWrite + 1) == valueRead);
    CPPUNIT_ASSERT((valueWrite + 1) == server->InterfaceProvided1.GetValue()); // unchanged
    CPPUNIT_ASSERT(-1 == client->InterfaceRequired1.GetValue()); // unchanged
    // test read command with downcast to bool for execution result
    CPPUNIT_ASSERT(client->InterfaceRequired1.FunctionRead(valueRead));

    // test qualified read command
    valueRead = 0;
    executionResult = client->InterfaceRequired1.FunctionQualifiedRead(valueWrite, valueRead);
    CPPUNIT_ASSERT_EQUAL(mtsExecutionResult::COMMAND_SUCCEEDED, executionResult.GetResult());
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
    CPPUNIT_ASSERT(manager->WaitForStateAll(mtsComponentState::FINISHED, StateTransitionMaximumDelay));
    CPPUNIT_ASSERT(manager->Disconnect(client->GetName(), "r1", server->GetName(), "p1"));
    CPPUNIT_ASSERT(manager->RemoveComponent(client));
    CPPUNIT_ASSERT(manager->RemoveComponent(server));
}


template <class _elementType>
void mtsCommandAndEventLocalTest::TestDeviceDevice(void)
{
    mtsTestDevice2<_elementType> * client = new mtsTestDevice2<_elementType>;
    mtsTestDevice3<_elementType> * server = new mtsTestDevice3<_elementType>;
    TestExecution(client, server, 0.0, 0.0);
    delete client;
    delete server;
}
void mtsCommandAndEventLocalTest::TestDeviceDevice_mtsInt(void) {
    mtsCommandAndEventLocalTest::TestDeviceDevice<mtsInt>();
}
void mtsCommandAndEventLocalTest::TestDeviceDevice_int(void) {
    mtsCommandAndEventLocalTest::TestDeviceDevice<int>();
}


template <class _elementType>
void mtsCommandAndEventLocalTest::TestPeriodicPeriodic(void)
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
void mtsCommandAndEventLocalTest::TestPeriodicPeriodic_mtsInt(void) {
    mtsCommandAndEventLocalTest::TestPeriodicPeriodic<mtsInt>();
}
void mtsCommandAndEventLocalTest::TestPeriodicPeriodic_int(void) {
    mtsCommandAndEventLocalTest::TestPeriodicPeriodic<int>();
}


template <class _elementType>
void mtsCommandAndEventLocalTest::TestContinuousContinuous(void)
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
void mtsCommandAndEventLocalTest::TestContinuousContinuous_mtsInt(void) {
    mtsCommandAndEventLocalTest::TestContinuousContinuous<mtsInt>();
}
void mtsCommandAndEventLocalTest::TestContinuousContinuous_int(void) {
    mtsCommandAndEventLocalTest::TestContinuousContinuous<int>();
}


template <class _elementType>
void mtsCommandAndEventLocalTest::TestFromCallbackFromCallback(void)
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
void mtsCommandAndEventLocalTest::TestFromCallbackFromCallback_mtsInt(void) {
    mtsCommandAndEventLocalTest::TestFromCallbackFromCallback<mtsInt>();
}
void mtsCommandAndEventLocalTest::TestFromCallbackFromCallback_int(void) {
    mtsCommandAndEventLocalTest::TestFromCallbackFromCallback<int>();
}


template <class _elementType>
void mtsCommandAndEventLocalTest::TestFromSignalFromSignal(void)
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
void mtsCommandAndEventLocalTest::TestFromSignalFromSignal_mtsInt(void) {
    mtsCommandAndEventLocalTest::TestFromSignalFromSignal<mtsInt>();
}
void mtsCommandAndEventLocalTest::TestFromSignalFromSignal_int(void) {
    mtsCommandAndEventLocalTest::TestFromSignalFromSignal<int>();
}


template <class _elementType>
void mtsCommandAndEventLocalTest::TestPeriodicPeriodicBlocking(void)
{
    const double blockingDelay = 0.25 * cmn_s;
    mtsTestPeriodic1<_elementType> * client = new mtsTestPeriodic1<_elementType>("mtsTestPeriodic1Client");
    mtsTestPeriodic1<_elementType> * server = new mtsTestPeriodic1<_elementType>("mtsTestPeriodic1Server", blockingDelay);
    // these delays are OS dependent, we might need to increase them later
    const double clientExecutionDelay = 0.1 * cmn_s;
    const double serverExecutionDelay = 0.1 * cmn_s;
    TestExecution(client, server, clientExecutionDelay, serverExecutionDelay, blockingDelay);
    delete client;
    delete server;
}
void mtsCommandAndEventLocalTest::TestPeriodicPeriodicBlocking_mtsInt(void) {
    mtsCommandAndEventLocalTest::TestPeriodicPeriodicBlocking<mtsInt>();
}
void mtsCommandAndEventLocalTest::TestPeriodicPeriodicBlocking_int(void) {
    mtsCommandAndEventLocalTest::TestPeriodicPeriodicBlocking<int>();
}


template <class _elementType>
void mtsCommandAndEventLocalTest::TestContinuousContinuousBlocking(void)
{
    const double blockingDelay = 0.25 * cmn_s;
    mtsTestContinuous1<_elementType> * client = new mtsTestContinuous1<_elementType>("mtsTestContinuous1Client");
    mtsTestContinuous1<_elementType> * server = new mtsTestContinuous1<_elementType>("mtsTestContinuous1Server", blockingDelay);
    // these delays are OS dependent, we might need to increase them later
    const double clientExecutionDelay = 0.1 * cmn_s;
    const double serverExecutionDelay = 0.1 * cmn_s;
    TestExecution(client, server, clientExecutionDelay, serverExecutionDelay, blockingDelay);
    delete client;
    delete server;
}
void mtsCommandAndEventLocalTest::TestContinuousContinuousBlocking_mtsInt(void) {
    mtsCommandAndEventLocalTest::TestContinuousContinuousBlocking<mtsInt>();
}
void mtsCommandAndEventLocalTest::TestContinuousContinuousBlocking_int(void) {
    mtsCommandAndEventLocalTest::TestContinuousContinuousBlocking<int>();
}


template <class _elementType>
void mtsCommandAndEventLocalTest::TestFromCallbackFromCallbackBlocking(void)
{
    const double blockingDelay = 0.25 * cmn_s;
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
void mtsCommandAndEventLocalTest::TestFromCallbackFromCallbackBlocking_mtsInt(void) {
    mtsCommandAndEventLocalTest::TestFromCallbackFromCallbackBlocking<mtsInt>();
}
void mtsCommandAndEventLocalTest::TestFromCallbackFromCallbackBlocking_int(void) {
    mtsCommandAndEventLocalTest::TestFromCallbackFromCallbackBlocking<int>();
}


template <class _elementType>
void mtsCommandAndEventLocalTest::TestFromSignalFromSignalBlocking(void)
{
    const double blockingDelay = 0.25 * cmn_s;
    mtsTestFromSignal1<_elementType> * client = new mtsTestFromSignal1<_elementType>("mtsTestFromSignal1Client");
    mtsTestFromSignal1<_elementType> * server = new mtsTestFromSignal1<_elementType>("mtsTestFromSignal1Server", blockingDelay);
    // these delays are OS dependent, we might need to increase them later
    const double clientExecutionDelay = 0.1 * cmn_s;
    const double serverExecutionDelay = 0.1 * cmn_s;
    TestExecution(client, server, clientExecutionDelay, serverExecutionDelay, blockingDelay);
    delete client;
    delete server;
}
void mtsCommandAndEventLocalTest::TestFromSignalFromSignalBlocking_mtsInt(void) {
    mtsCommandAndEventLocalTest::TestFromSignalFromSignalBlocking<mtsInt>();
}
void mtsCommandAndEventLocalTest::TestFromSignalFromSignalBlocking_int(void) {
    mtsCommandAndEventLocalTest::TestFromSignalFromSignalBlocking<int>();
}


template <class _elementType>
void mtsCommandAndEventLocalTest::TestArgumentPrototypes(void)
{
    typedef _elementType value_type;

    mtsTestContinuous1<value_type> * client = new mtsTestContinuous1<value_type>("mtsTestContinuous1Client");
    mtsTestContinuous1<value_type> * server = new mtsTestContinuous1<value_type>("mtsTestContinuous1Server");

    mtsManagerLocal * manager = mtsManagerLocal::GetInstance();
    manager->RemoveAllUserComponents();

    // add to manager and start all
    CPPUNIT_ASSERT(manager->AddComponent(client));
    CPPUNIT_ASSERT(manager->AddComponent(server));
    CPPUNIT_ASSERT(manager->Connect(client->GetName(), "r1", server->GetName(), "p1"));
    manager->CreateAll();
    CPPUNIT_ASSERT(manager->WaitForStateAll(mtsComponentState::READY, StateTransitionMaximumDelay));
    manager->StartAll();
    CPPUNIT_ASSERT(manager->WaitForStateAll(mtsComponentState::ACTIVE, StateTransitionMaximumDelay));

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
    CPPUNIT_ASSERT(manager->WaitForStateAll(mtsComponentState::FINISHED, StateTransitionMaximumDelay));
    CPPUNIT_ASSERT(manager->Disconnect(client->GetName(), "r1", server->GetName(), "p1"));
    CPPUNIT_ASSERT(manager->RemoveComponent(client));
    CPPUNIT_ASSERT(manager->RemoveComponent(server));
    delete client;
    delete server;
}
void mtsCommandAndEventLocalTest::TestArgumentPrototypes_mtsInt(void) {
    mtsCommandAndEventLocalTest::TestArgumentPrototypes<mtsInt>();
}
void mtsCommandAndEventLocalTest::TestArgumentPrototypes_int(void) {
    mtsCommandAndEventLocalTest::TestArgumentPrototypes<int>();
}


CPPUNIT_TEST_SUITE_REGISTRATION(mtsCommandAndEventLocalTest);
