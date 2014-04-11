/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Peter Kazanzides, Anton Deguet
  Created on: 2008-11-13

  (C) Copyright 2008-2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsInterfaceInput.h>
#include <cisstMultiTask/mtsInterfaceOutput.h>

mtsInterfaceInput::mtsInterfaceInput(const std::string & interfaceName, mtsComponent * component) :
    mtsInterface(interfaceName, component)
{
}


mtsInterfaceInput::~mtsInterfaceInput()
{
}

const mtsInterfaceOutput * mtsInterfaceInput::GetConnectedInterface(void) const
{
    return InterfaceOutput;
}
