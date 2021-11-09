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

#include "mtsComponentStateTest.h"

#include "mtsTestComponents.h"
#include <cisstMultiTask/mtsManagerLocal.h>

mtsComponentStateTest::mtsComponentStateTest(void)
{}


void mtsComponentStateTest::setUp(void)
{}


void mtsComponentStateTest::tearDown(void)
{}


void mtsComponentStateTest::TestOrder(void)
{

}


void mtsComponentStateTest::TestTransitions(void)
{
    const double maxTimeToChangeState = 3.0 * cmn_s; // this is an upper limit across OSs, raise it if needed
    mtsManagerLocal * manager = mtsManagerLocal::GetInstance();
    manager->RemoveAllUserComponents();

    mtsTestPeriodic1<mtsInt> * periodic1 = new mtsTestPeriodic1<mtsInt>;
    mtsTestContinuous1<mtsInt> * continuous1 = new mtsTestContinuous1<mtsInt>;
    mtsTestFromCallback1<mtsInt> * fromCallback1 = new mtsTestFromCallback1<mtsInt>;
    mtsTestCallbackTrigger * callbackTrigger = new mtsTestCallbackTrigger(fromCallback1);
    mtsTestFromSignal1<mtsInt> * fromSignal1 = new mtsTestFromSignal1<mtsInt>;
    mtsTestDevice2<mtsInt> * device2 = new mtsTestDevice2<mtsInt>;

    CPPUNIT_ASSERT(periodic1->GetState() == mtsComponentState::CONSTRUCTED);
    CPPUNIT_ASSERT(continuous1->GetState() == mtsComponentState::CONSTRUCTED);
    CPPUNIT_ASSERT(fromCallback1->GetState() == mtsComponentState::CONSTRUCTED);
    CPPUNIT_ASSERT(fromSignal1->GetState() == mtsComponentState::CONSTRUCTED);

    CPPUNIT_ASSERT(manager->AddComponent(periodic1));
    CPPUNIT_ASSERT(manager->AddComponent(continuous1));
    CPPUNIT_ASSERT(manager->AddComponent(fromCallback1));
    CPPUNIT_ASSERT(manager->AddComponent(fromSignal1));
    CPPUNIT_ASSERT(manager->AddComponent(device2));

    // Establish connections between the three components of mtsTask type
    CPPUNIT_ASSERT(manager->Connect(periodic1->GetName(), "r1", continuous1->GetName(), "p1"));
    CPPUNIT_ASSERT(manager->Connect(periodic1->GetName(), "r2", continuous1->GetName(), "p2"));
    CPPUNIT_ASSERT(manager->Connect(continuous1->GetName(), "r1", device2->GetName(), "p1"));
    CPPUNIT_ASSERT(manager->Connect(fromCallback1->GetName(), "r1", continuous1->GetName(), "p2"));
    CPPUNIT_ASSERT(manager->Connect(fromSignal1->GetName(), "r1", continuous1->GetName(), "p2"));
    CPPUNIT_ASSERT(manager->Connect(device2->GetName(), "r1", continuous1->GetName(), "p2"));

    manager->CreateAll();
    CPPUNIT_ASSERT((periodic1->GetState() == mtsComponentState::INITIALIZING) ||
                   (periodic1->GetState() == mtsComponentState::READY));
    CPPUNIT_ASSERT((continuous1->GetState() == mtsComponentState::INITIALIZING) ||
                   (continuous1->GetState() == mtsComponentState::READY));
    CPPUNIT_ASSERT((fromCallback1->GetState() == mtsComponentState::INITIALIZING) ||
                   (fromCallback1->GetState() == mtsComponentState::READY));
    CPPUNIT_ASSERT((fromSignal1->GetState() == mtsComponentState::INITIALIZING) ||
                   (fromSignal1->GetState() == mtsComponentState::READY));

    // let all tasks get initialized
    CPPUNIT_ASSERT(manager->WaitForStateAll(mtsComponentState::READY,
                                                maxTimeToChangeState));
    CPPUNIT_ASSERT(periodic1->GetState() == mtsComponentState::READY);
    CPPUNIT_ASSERT(continuous1->GetState() == mtsComponentState::READY);
    CPPUNIT_ASSERT(fromCallback1->GetState() == mtsComponentState::READY);
    CPPUNIT_ASSERT(fromSignal1->GetState() == mtsComponentState::READY);

    manager->StartAll();
    // let all tasks start
    CPPUNIT_ASSERT(manager->WaitForStateAll(mtsComponentState::ACTIVE,
                                                maxTimeToChangeState));
    CPPUNIT_ASSERT(periodic1->GetState() == mtsComponentState::ACTIVE);
    CPPUNIT_ASSERT(continuous1->GetState() == mtsComponentState::ACTIVE);
    CPPUNIT_ASSERT(fromCallback1->GetState() == mtsComponentState::ACTIVE);
    CPPUNIT_ASSERT(fromSignal1->GetState() == mtsComponentState::ACTIVE);

    manager->KillAll();
    CPPUNIT_ASSERT(periodic1->GetState() == mtsComponentState::FINISHING ||
                   periodic1->GetState() == mtsComponentState::FINISHED);
    CPPUNIT_ASSERT(continuous1->GetState() == mtsComponentState::FINISHING ||
                   continuous1->GetState() == mtsComponentState::FINISHED);
    CPPUNIT_ASSERT(fromCallback1->GetState() == mtsComponentState::FINISHING ||
                   fromCallback1->GetState() == mtsComponentState::FINISHED);
    CPPUNIT_ASSERT(fromSignal1->GetState() == mtsComponentState::FINISHING ||
                   fromSignal1->GetState() == mtsComponentState::FINISHED);

    // let all tasks stop
    CPPUNIT_ASSERT(manager->WaitForStateAll(mtsComponentState::FINISHED,
                                            maxTimeToChangeState));
    CPPUNIT_ASSERT(periodic1->GetState() == mtsComponentState::FINISHED);
    CPPUNIT_ASSERT(continuous1->GetState() == mtsComponentState::FINISHED);
    CPPUNIT_ASSERT(fromCallback1->GetState() == mtsComponentState::FINISHED);
    CPPUNIT_ASSERT(fromSignal1->GetState() == mtsComponentState::FINISHED);

    CPPUNIT_ASSERT(manager->RemoveComponent(periodic1));
    CPPUNIT_ASSERT(manager->RemoveComponent(continuous1));
    CPPUNIT_ASSERT(manager->RemoveComponent(fromCallback1));
    CPPUNIT_ASSERT(manager->RemoveComponent(fromSignal1));
    CPPUNIT_ASSERT(manager->RemoveComponent(device2));

    delete periodic1;
    delete continuous1;
    callbackTrigger->Stop();
    delete callbackTrigger;
    delete fromCallback1;
    delete fromSignal1;
    delete device2;
}


CPPUNIT_TEST_SUITE_REGISTRATION(mtsComponentStateTest);
