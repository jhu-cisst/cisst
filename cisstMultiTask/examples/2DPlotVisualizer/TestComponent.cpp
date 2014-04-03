/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Praneeth Sadda, Anton Deguet
  Created on: 2012-05-26

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include "TestComponent.h"

#include <cmath>

#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>


CMN_IMPLEMENT_SERVICES(TestComponent);


TestComponent::TestComponent():
    mtsTaskPeriodic("TestComponent", 10.0 * cmn_ms)
{
    Counter = 0.0;
    ValueSine = 0.0;
    this->StateTable.AddData(Counter);
    this->StateTable.AddData(ValueSine);
}


void TestComponent::Configure(const std::string & CMN_UNUSED(file))
{
    mtsInterfaceProvided * interfaceProvided = AddInterfaceProvided("TestInterface1");
    if (interfaceProvided) {
        interfaceProvided->AddCommandReadState(this->StateTable, ValueSine, "ReadSine");
        interfaceProvided->AddCommandReadState(this->StateTable, ValueSine, "ReadAbsSine");
    }
}


void TestComponent::Run(void)
{
    ProcessQueuedCommands();
    Counter += 0.1;
    ValueSine = std::sin(Counter);
    ValueAbsSine = std::abs(ValueSine);
}
