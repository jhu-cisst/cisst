/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2011-01-06

  (C) Copyright 2011-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "mtsCollectorStateTest.h"

#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsCollectorState.h>

#include "mtsTestComponents.h"


mtsCollectorStateTest::mtsCollectorStateTest()
{
}


void mtsCollectorStateTest::setUp(void)
{
}


void mtsCollectorStateTest::tearDown(void)
{
}

template <class _serverType>
void mtsCollectorStateTest::TestExecution(_serverType * server,
                                          double serverExecutionDelay)
{
    const std::string filename = "StateDataCollectionUnitTest";

    // execution result used by all functions
    mtsExecutionResult executionResult;

    // we assume both server and servers use the same type
    mtsManagerLocal * manager = mtsManagerLocal::GetInstance();
    manager->RemoveAllUserComponents();

    // add to manager and start all
    CPPUNIT_ASSERT(manager->AddComponent(server));

    mtsCollectorState * stateCollector = new mtsCollectorState("CollectorStateTest");
    stateCollector->SetOutput(filename, mtsCollectorBase::COLLECTOR_FILE_FORMAT_CSV);
    CPPUNIT_ASSERT(stateCollector);
    CPPUNIT_ASSERT(stateCollector->SetStateTable(server->GetName(),
                                                 server->InterfaceProvided1.StateTable->GetName()));
    CPPUNIT_ASSERT(stateCollector->AddSignal(""));
    CPPUNIT_ASSERT(manager->AddComponent(stateCollector));
    CPPUNIT_ASSERT(stateCollector->Connect());

    // add test device to control execution
    mtsCollectorStateTestDevice * stateCollectorTestDevice = new mtsCollectorStateTestDevice();
    std::string stateTableName = server->InterfaceProvided1.StateTable->GetName();
    std::string stateTableInterfaceName = "StateTable" + server->InterfaceProvided1.StateTable->GetName();
    CPPUNIT_ASSERT(stateCollectorTestDevice);
    CPPUNIT_ASSERT(manager->AddComponent(stateCollectorTestDevice));
    CPPUNIT_ASSERT(manager->Connect(stateCollectorTestDevice->GetName(), "TestComponent",
                                    server->GetName(), "p1"));
    CPPUNIT_ASSERT(manager->Connect(stateCollectorTestDevice->GetName(), "StateTable",
                                    server->GetName(), stateTableInterfaceName));
    CPPUNIT_ASSERT(manager->Connect(stateCollectorTestDevice->GetName(), "CollectorState",
                                    stateCollector->GetName(), "Control"));
    manager->CreateAll();
    CPPUNIT_ASSERT(manager->WaitForStateAll(mtsComponentState::READY, StateTransitionMaximumDelay));
    manager->StartAll();
    CPPUNIT_ASSERT(manager->WaitForStateAll(mtsComponentState::ACTIVE, StateTransitionMaximumDelay));

    // test that functions are connected
    CPPUNIT_ASSERT(stateCollectorTestDevice->TestComponent.StateTableAdvance.IsValid());
    CPPUNIT_ASSERT(stateCollectorTestDevice->CollectorState.StartCollection.IsValid());
    CPPUNIT_ASSERT(stateCollectorTestDevice->CollectorState.StopCollection.IsValid());

    // actual testing, preliminary conditions
    size_t numberOfRows = 0;
    mtsStateTable::IndexRange range;
    CPPUNIT_ASSERT(!stateCollectorTestDevice->CollectionRunning);
    CPPUNIT_ASSERT_EQUAL(0u, stateCollectorTestDevice->BatchReadyEventCounter);
    CPPUNIT_ASSERT_EQUAL(0u, stateCollectorTestDevice->SamplesCollected);

    // try a start/stop without any state table advance
    executionResult = stateCollectorTestDevice->CollectorState.StartCollection.ExecuteBlocking();
    CPPUNIT_ASSERT(executionResult.IsOK());
    // commands are sent to the collector which then sends to the
    // server which owns the state table.  the later command being
    // queued we need to wait a bit
    osaSleep(serverExecutionDelay);
    // collection will not send the start event until the next state table advance
    CPPUNIT_ASSERT(!stateCollectorTestDevice->CollectionRunning);
    // advance state table, this will trigger the start event
    executionResult = stateCollectorTestDevice->TestComponent.StateTableAdvance.ExecuteBlocking();
    numberOfRows++;
    CPPUNIT_ASSERT(executionResult.IsOK());
    CPPUNIT_ASSERT(stateCollectorTestDevice->CollectionRunning);
    // to make sure, not collection should have been performed so far
    CPPUNIT_ASSERT_EQUAL(0u, stateCollectorTestDevice->BatchReadyEventCounter);
    CPPUNIT_ASSERT_EQUAL(0u, stateCollectorTestDevice->SamplesCollected);
    // now try to stop
    executionResult = stateCollectorTestDevice->CollectorState.StopCollection.ExecuteBlocking();
    CPPUNIT_ASSERT(executionResult.IsOK());
    // see previous comment re. osaSleep
    osaSleep(serverExecutionDelay);
    // collection will not send the stop event until the next state table advance
    executionResult = stateCollectorTestDevice->TestComponent.StateTableAdvance.ExecuteBlocking();
    CPPUNIT_ASSERT(executionResult.IsOK());
    CPPUNIT_ASSERT(!stateCollectorTestDevice->CollectionRunning);
    // since we had to do one advance, the collector should send a range event
    CPPUNIT_ASSERT_EQUAL(1u, stateCollectorTestDevice->BatchReadyEventCounter);
    CPPUNIT_ASSERT_EQUAL(1u, stateCollectorTestDevice->SamplesCollected);

#if 0

    // now collect 300 elements so we can test replay
    executionResult = stateCollectorTestDevice->CollectorState.StartCollection.ExecuteBlocking();
    CPPUNIT_ASSERT(executionResult.IsOK());
    // see previous comment re. osaSleep
    osaSleep(serverExecutionDelay);
    for (size_t index = 0;
         index < 300;
         index++) {
        executionResult = stateCollectorTestDevice->TestComponent.StateTableAdvance.ExecuteBlocking();
        numberOfRows++;
        CPPUNIT_ASSERT(executionResult.IsOK());
        CPPUNIT_ASSERT(stateCollectorTestDevice->CollectionRunning);
    }
    // stop collection
    executionResult = stateCollectorTestDevice->CollectorState.StopCollection.ExecuteBlocking();
    CPPUNIT_ASSERT(executionResult.IsOK());
    // see previous comment re. osaSleep
    osaSleep(serverExecutionDelay);
    // collection will not send the stop event until the next state table advance
    executionResult = stateCollectorTestDevice->TestComponent.StateTableAdvance.ExecuteBlocking();
    CPPUNIT_ASSERT(executionResult.IsOK());
    // since we had to do one advance, the collector should send a range event
    CPPUNIT_ASSERT_EQUAL(11u, stateCollectorTestDevice->BatchReadyEventCounter);
    CPPUNIT_ASSERT_EQUAL(300u, stateCollectorTestDevice->SamplesCollected);

    // create a new component of the same type for replay
    _serverType * replayComponent = new _serverType("replay-component");
    CPPUNIT_ASSERT(manager->AddComponent(replayComponent));
    // should fail since we are not in replay mode
    CPPUNIT_ASSERT(!replayComponent->SetReplayData(stateTableName, filename));
    CPPUNIT_ASSERT(replayComponent->SetReplayMode());
    // shouldn't find that "dummy" state table
    CPPUNIT_ASSERT(!replayComponent->SetReplayData("dummy", filename));
    std::string dataReplayFile = filename + ".csv";
    CPPUNIT_ASSERT(replayComponent->SetReplayData(stateTableName, dataReplayFile));

    // create and connect component to read from replay state table
    typedef typename _serverType::value_type value_type;
    mtsTestDevice1<value_type> * replayReader = new mtsTestDevice1<value_type>("replay-reader");
    CPPUNIT_ASSERT(manager->AddComponent(replayReader));
    CPPUNIT_ASSERT(manager->Connect(replayReader->GetName(), "r1",
                                    replayComponent->GetName(), "p1"));

    replayComponent->Start();
    replayReader->Start();
    replayComponent->Create();
    replayReader->Create();

    value_type data;
    for (size_t index = 0;
         index < 300;
         index++) {
        CPPUNIT_ASSERT(replayComponent->InterfaceProvided1.StateTable->ReplayAdvance());
        replayReader->InterfaceRequired1.FunctionStateTableRead(data);
        std::cerr << index << " -> " << data << std::endl;
    }

#endif

    // stop all and cleanup
    manager->KillAll();
    CPPUNIT_ASSERT(manager->WaitForStateAll(mtsComponentState::FINISHED, StateTransitionMaximumDelay));

    CPPUNIT_ASSERT(stateCollector->Disconnect());
    CPPUNIT_ASSERT(manager->Disconnect(stateCollectorTestDevice->GetName(), "TestComponent",
                                       server->GetName(), "p1"));
    CPPUNIT_ASSERT(manager->Disconnect(stateCollectorTestDevice->GetName(), "StateTable",
                                       server->GetName(), stateTableInterfaceName));
    CPPUNIT_ASSERT(manager->Disconnect(stateCollectorTestDevice->GetName(), "CollectorState",
                                       stateCollector->GetName(), "Control"));
    CPPUNIT_ASSERT(manager->RemoveComponent(server));
    CPPUNIT_ASSERT(manager->RemoveComponent(stateCollector));
    CPPUNIT_ASSERT(manager->RemoveComponent(stateCollectorTestDevice));
    delete stateCollector;
    delete stateCollectorTestDevice;
}


template <class _elementType>
void mtsCollectorStateTest::TestDevice(void)
{
    mtsTestDevice1<_elementType> * server = new mtsTestDevice1<_elementType>;
    TestExecution(server, 0.0 /* no execution delay for a device */);
    delete server;
}
void mtsCollectorStateTest::TestDevice_mtsInt(void) {
    mtsCollectorStateTest::TestDevice<mtsInt>();
}
void mtsCollectorStateTest::TestDevice_int(void) {
    mtsCollectorStateTest::TestDevice<int>();
}


template <class _elementType>
void mtsCollectorStateTest::TestPeriodic(void)
{
    mtsTestPeriodic1<_elementType> * server = new mtsTestPeriodic1<_elementType>;
    const double serverExecutionDelay = 0.1 * cmn_s;
    TestExecution(server, serverExecutionDelay);
    delete server;
}
void mtsCollectorStateTest::TestPeriodic_mtsInt(void) {
    mtsCollectorStateTest::TestPeriodic<mtsInt>();
}
void mtsCollectorStateTest::TestPeriodic_int(void) {
    mtsCollectorStateTest::TestPeriodic<int>();
}


template <class _elementType>
void mtsCollectorStateTest::TestContinuous(void)
{
    mtsTestContinuous1<_elementType> * server = new mtsTestContinuous1<_elementType>;
    const double serverExecutionDelay = 0.1 * cmn_s;
    TestExecution(server, serverExecutionDelay);
    delete server;
}
void mtsCollectorStateTest::TestContinuous_mtsInt(void) {
    mtsCollectorStateTest::TestContinuous<mtsInt>();
}
void mtsCollectorStateTest::TestContinuous_int(void) {
    mtsCollectorStateTest::TestContinuous<int>();
}


template <class _elementType>
void mtsCollectorStateTest::TestFromCallback(void)
{
    mtsTestFromCallback1<_elementType> * server = new mtsTestFromCallback1<_elementType>;
    mtsTestCallbackTrigger * serverTrigger = new mtsTestCallbackTrigger(server);
    const double serverExecutionDelay = 0.1 * cmn_s;
    TestExecution(server, serverExecutionDelay);
    serverTrigger->Stop();
    delete serverTrigger;
    delete server;
}
void mtsCollectorStateTest::TestFromCallback_mtsInt(void) {
    mtsCollectorStateTest::TestFromCallback<mtsInt>();
}
void mtsCollectorStateTest::TestFromCallback_int(void) {
    mtsCollectorStateTest::TestFromCallback<int>();
}


template <class _elementType>
void mtsCollectorStateTest::TestFromSignal(void)
{
    mtsTestFromSignal1<_elementType> * server = new mtsTestFromSignal1<_elementType>;
    const double serverExecutionDelay = 0.1 * cmn_s;
    TestExecution(server, serverExecutionDelay);
    delete server;
}
void mtsCollectorStateTest::TestFromSignal_mtsInt(void) {
    mtsCollectorStateTest::TestFromSignal<mtsInt>();
}
void mtsCollectorStateTest::TestFromSignal_int(void) {
    mtsCollectorStateTest::TestFromSignal<int>();
}

CPPUNIT_TEST_SUITE_REGISTRATION(mtsCollectorStateTest);
