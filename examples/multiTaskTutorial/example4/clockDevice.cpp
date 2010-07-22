/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
// $Id$

#include <cisstCommon/cmnConstants.h>
#include "clockDevice.h"

CMN_IMPLEMENT_SERVICES(clockDevice);

clockDevice::clockDevice(const std::string & deviceName):
    mtsDevice(deviceName) {
    mtsInterfaceProvided * mainInterface = AddInterfaceProvided("MainInterface");
    mainInterface->AddCommandRead(&clockDevice::GetTime, this, "GetTime");
    Timer.Reset();   // reset the clock
    Timer.Start();   // start the clock 
}

void clockDevice::GetTime(mtsDouble & time) const
{
    time = Timer.GetElapsedTime(); // get the time since started
}

/*
  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/
