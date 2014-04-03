/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2010-01-21

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnConstants.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include "mtsPeriodicTaskTest.h"


// required to implement the class services, see cisstCommon
CMN_IMPLEMENT_SERVICES(mtsPeriodicTaskTest);

mtsPeriodicTaskTest::mtsPeriodicTaskTest(double periodInSeconds):
    mtsTaskPeriodic("mtsPeriodicTaskTest", periodInSeconds, false, 500)
{
    this->Double = 0.0;
    this->Vector.SetSize(5);
    this->Vector.SetAll(0.0);
    StateTable.AddData(this->Double, "Double");
    StateTable.AddData(this->Vector, "Vector");
    // add one interface, this will create an mtsTaskInterface
    mtsInterfaceProvided * interfaceProvided = AddInterfaceProvided("MainInterface");
    if (interfaceProvided) {
        // add command to access state table values to the interface
        interfaceProvided->AddCommandReadState(StateTable, this->Double, "GetDouble");
        interfaceProvided->AddCommandReadState(StateTable, this->Vector, "GetVector");
        // add command to add to vector
        interfaceProvided->AddCommandWrite(&mtsPeriodicTaskTest::AddDouble, this, "AddDouble");
        // add command to zero all
        interfaceProvided->AddCommandVoid(&mtsPeriodicTaskTest::ZeroAll, this, "ZeroAll");
    }
    // add a second (empty) interface, this will create an mtsTaskInterface
    interfaceProvided = AddInterfaceProvided("Empty Interface");
}


void mtsPeriodicTaskTest::AddDouble(const mtsDouble & addend)
{
    this->Double = addend;
    this->Vector += addend.Data;
}


void mtsPeriodicTaskTest::ZeroAll(void)
{
    this->Double = 0.0;
    this->Vector.SetAll(0.0);
}


void mtsPeriodicTaskTest::Run(void) {
    ProcessQueuedCommands();
}
