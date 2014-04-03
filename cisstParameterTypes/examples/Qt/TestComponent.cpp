/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Praneeth Sadda, Anton Deguet
  Created on: 2011-11-11

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include "TestComponent.h"

#include <sstream>

#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>


CMN_IMPLEMENT_SERVICES(TestComponent);


TestComponent::TestComponent():
    mtsTaskPeriodic("TestComponent", 10.0 * cmn_ms)
{
    CounterInt = 0;
    CounterDouble = 0.0;
    CounterBool = false;
    this->StateTable.AddData(CounterInt);
    this->StateTable.AddData(CounterDouble);
    this->StateTable.AddData(CounterBool);
}


void TestComponent::CommandVoid(void)
{
    CounterDouble = 0.0;
    CounterInt = 0;
    CounterBool = false;
    EventVoid();
}


void TestComponent::CommandQualifiedReadDouble(const mtsDouble& in, mtsStdString& out) const
{
    std::stringstream ss;
    ss << "The number you entered was "
       << in.GetData();

    mtsStdString newStr(ss.str());
    out.Assign(newStr);
}


void TestComponent::CommandWriteReturnBool(const mtsBool & in, mtsStdString & out)
{
    CounterBool = in;
    std::stringstream ss;
    ss << "The value you wrote was "
       << std::boolalpha
       << in.GetData();
    osaSleep(10.0);
    mtsStdString newStr(ss.str());
    out.Assign(newStr);
}


void TestComponent::Configure(const std::string & CMN_UNUSED(file))
{
    mtsInterfaceProvided * interfaceProvided = AddInterfaceProvided("TestInterface1");
    if (interfaceProvided) {
        interfaceProvided->AddCommandVoid<TestComponent>(&TestComponent::CommandVoid, this, "Void");
        interfaceProvided->AddCommandQualifiedRead<TestComponent, mtsDouble, mtsStdString>(&TestComponent::CommandQualifiedReadDouble, this, "QualifiedReadDouble");
        interfaceProvided->AddCommandWriteReturn<TestComponent, mtsBool, mtsStdString>(&TestComponent::CommandWriteReturnBool, this, "WriteReturnBool");
        interfaceProvided->AddCommandReadState(this->StateTable, CounterDouble, "ReadDouble");
        interfaceProvided->AddCommandReadState(this->StateTable, CounterInt, "ReadInt");
        interfaceProvided->AddEventVoid(EventVoid, "EventVoid");
        interfaceProvided->AddEventWrite(EventWriteInt, "EventWriteInt", mtsInt());
    }
}


void TestComponent::Run(void)
{
    ProcessQueuedCommands();
    CounterInt++;
    CounterDouble += 0.1;
    if ((CounterInt % 100) == 0) {
        EventWriteInt(mtsInt(CounterInt));
    }
}
