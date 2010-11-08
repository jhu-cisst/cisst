/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: CounterOddComponent.h 

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

#include "CounterOddComponent.h"
#include "CounterEvenComponent.h"

std::string CounterOddComponent::NameCounterOddInterfaceProvided = "CounterOddInterfaceProvided";
std::string CounterOddComponent::NameCounterOddInterfaceRequired = "CounterOddInterfaceRequired";
std::string CounterOddComponent::NameGetPeerCounter  = "GetPeerCounter";
std::string CounterOddComponent::NameReturnMyCounter = "ReturnMyCounter";

CMN_IMPLEMENT_SERVICES(CounterOddComponent);

CounterOddComponent::CounterOddComponent()
    : mtsTaskPeriodic("CounterOddObject", 1 * cmn_s, false, 1000), 
      CounterMine(-1)
{
    UseSeparateLogFileDefault();

    CMN_LOG_CLASS_INIT_VERBOSE << "Started" << std::endl;

    AddInterface();
}

CounterOddComponent::CounterOddComponent(const std::string & componentName, double period)
    : mtsTaskPeriodic(componentName, period, false, 1000), 
      CounterMine(-1)
{
    AddInterface();
}

void CounterOddComponent::AddInterface(void)
{
    mtsInterfaceRequired * required = AddInterfaceRequired(NameCounterOddInterfaceRequired);
    if (!required) {
        cmnThrow("CounterOddComponent - failed to create required interface");
    } else {
        required->AddFunction(CounterEvenComponent::NameReturnMyCounter, this->GetPeerCounter);
    }

    mtsInterfaceProvided * provided = AddInterfaceProvided(NameCounterOddInterfaceProvided);
    if (!provided) {
        cmnThrow("CounterOddComponent - failed to create provided interface");
    } else {
        provided->AddCommandRead(&CounterOddComponent::ReturnMyCounter, 
                                this, CounterOddComponent::NameReturnMyCounter);
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "Interface created" << std::endl;
}

void CounterOddComponent::Startup(void)
{
    CounterMine = 1;

    CMN_LOG_CLASS_INIT_VERBOSE << "Startup called" << std::endl;
}

void CounterOddComponent::Run(void) 
{
    ProcessQueuedCommands();

    CounterMine += 2;

    std::cout << this->GetName() << " : " << CounterMine;

    if (!GetPeerCounter.IsValid()) {
        std::cout << "Skipped fetching peer counter: function has not been bound to command" << std::endl;
        return;
    }

    mtsInt peerCounter;
    GetPeerCounter(peerCounter);
    std::cout << ", Peer: " << peerCounter.Data << std::endl;
}

void CounterOddComponent::ReturnMyCounter(mtsInt & myCounter) const
{
    myCounter = CounterMine;
}
