/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2012-02-02

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstCommon/cmnConstants.h>
#include "serverWithDelay.h"

// required to implement the class services, see cisstCommon
CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(serverWithDelay, mtsTaskPeriodic, mtsTaskPeriodicConstructorArg);

serverWithDelay::serverWithDelay(const std::string & componentName, double period):
    mtsComponentAddLatency(componentName, period)
{
    mtsInterfaceRequired * interfaceRequired = this->AddInterfaceRequired("Required");
    mtsInterfaceProvided * interfaceProvided = this->AddInterfaceProvided("Provided");
    this->AddCommandVoidDelayed(interfaceRequired, "Void",
                                interfaceProvided);
    this->AddCommandWriteDelayed<mtsInt>(interfaceRequired, "Write",
                                         interfaceProvided);
    this->AddCommandReadDelayed<mtsInt>(interfaceRequired, "Read",
                                        interfaceProvided);
    this->AddCommandQualifiedReadDelayed<mtsInt, mtsInt>(interfaceRequired, "QualifiedRead",
                                                         interfaceProvided);
    this->AddEventVoidDelayed(interfaceRequired, "EventVoid",
                              interfaceProvided);
    this->AddEventWriteDelayed<mtsInt>(interfaceRequired, "EventWrite",
                                       interfaceProvided);
}
