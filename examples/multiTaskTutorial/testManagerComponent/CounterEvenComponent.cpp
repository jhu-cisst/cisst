/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: CounterEvenComponent.h 

  Author(s):  Min Yang Jung
  Created on: 2010-09-01

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnConstants.h>
#include <cisstOSAbstraction/osaGetTime.h>

#include "CounterEvenComponent.h"
#include "CounterOddComponent.h"

std::string CounterEvenComponent::NameCounterEvenInterfaceProvided = "CounterEvenInterfaceProvided";
std::string CounterEvenComponent::NameCounterEvenInterfaceRequired = "CounterEvenInterfaceRequired";
std::string CounterEvenComponent::NameGetPeerCounter  = "GetPeerCounter";
std::string CounterEvenComponent::NameReturnMyCounter = "ReturnMyCounter";

CMN_IMPLEMENT_SERVICES(CounterEvenComponent);

CounterEvenComponent::CounterEvenComponent()
    : mtsTaskPeriodic("CounterEvenObject", 1 * cmn_s, false, 1000), 
      CounterMine(-1)
{
    UseSeparateLogFileDefault();

    CMN_LOG_CLASS_INIT_VERBOSE << "Started" << std::endl;

    AddInterface();
}

CounterEvenComponent::CounterEvenComponent(const std::string & componentName, double period)
    : mtsTaskPeriodic(componentName, period, false, 1000), 
      CounterMine(-1)
{
    AddInterface();
}

void CounterEvenComponent::AddInterface(void)
{
    mtsInterfaceRequired * required = AddInterfaceRequired(NameCounterEvenInterfaceRequired);
    if (!required) {
        cmnThrow("CounterEvenComponent - failed to create required interface");
    } else {
        required->AddFunction(CounterOddComponent::NameReturnMyCounter, this->GetPeerCounter);
    }

    mtsInterfaceProvided * provided = AddInterfaceProvided(NameCounterEvenInterfaceProvided);
    if (!provided) {
        cmnThrow("CounterEvenComponent - failed to create provided interface");
    } else {
        provided->AddCommandRead(&CounterEvenComponent::ReturnMyCounter, 
                                this, CounterEvenComponent::NameReturnMyCounter);
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "Interface created" << std::endl;
}

void CounterEvenComponent::Startup(void)
{
    CounterMine = 0;

    CMN_LOG_CLASS_INIT_VERBOSE << "Startup called" << std::endl;
}

void CounterEvenComponent::Run(void) 
{
    ProcessQueuedCommands();

    CounterMine += 2;

    std::cout << this->GetName() << ": " << CounterMine;

    if (!GetPeerCounter.IsValid()) {
        std::cout << "Skipped fetching peer counter: function has not been bound to command" << std::endl;
        return;
    }

    mtsInt peerCounter;
    GetPeerCounter(peerCounter);
    std::cout << ", Peer: " << peerCounter.Data << std::endl;
}

void CounterEvenComponent::ReturnMyCounter(mtsInt & myCounter) const
{
    myCounter = CounterMine;
}