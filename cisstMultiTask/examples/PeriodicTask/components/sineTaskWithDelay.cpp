/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstCommon/cmnConstants.h>
#include "sineTaskWithDelay.h"

// required to implement the class services, see cisstCommon
CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(sineTaskWithDelay, mtsComponentAddLatency, mtsTaskPeriodicConstructorArg);

sineTaskWithDelay::sineTaskWithDelay(const std::string & componentName, double period):
    mtsComponentAddLatency(componentName, period)
{
    Init();
}

sineTaskWithDelay::sineTaskWithDelay(const mtsTaskPeriodicConstructorArg &arg):
    mtsComponentAddLatency(arg.Name, arg.Period)
{
    Init();
}

void sineTaskWithDelay::Init(void)
{
    mtsInterfaceRequired * interfaceRequired = this->AddInterfaceRequired("MainInterface");
    mtsInterfaceProvided * interfaceProvided = this->AddInterfaceProvided("MainInterface");
    this->AddCommandWriteDelayed<mtsDouble>(interfaceRequired, "SetAmplitude",
                                            interfaceProvided);
    this->AddCommandReadDelayed<mtsDouble>(interfaceRequired, "GetData",
                                           interfaceProvided);
    this->AddCommandWriteDelayed<mtsDouble>(interfaceRequired, "SetTriggerThreshold",
                                            interfaceProvided);
    this->AddCommandVoidDelayed(interfaceRequired, "ResetTrigger",
                                interfaceProvided);
    this->AddEventVoidDelayed(interfaceRequired, "TriggerEvent",
                              interfaceProvided);
}
