/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstCommon/cmnConstants.h>
#include "clockComponent.h"

CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(clockComponent, mtsComponent, std::string);

clockComponent::clockComponent(const std::string & componentName):
    mtsComponent(componentName)
{
    // call generated method to configure this component
    InitComponent();

    Timer.Reset(); // reset the clock
    Timer.Start(); // start the clock
}

void clockComponent::GetTime(mtsDouble & time) const
{
    time = Timer.GetElapsedTime(); // get the time since started
}
