/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Peter Kazanzides, Anton Deguet, Min Yang Jung
  Created on: 2008-11-13

  (C) Copyright 2008-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsInterfaceRequiredOrInput.h>
#include <cisstMultiTask/mtsComponent.h>


mtsInterfaceRequiredOrInput::mtsInterfaceRequiredOrInput(const std::string & interfaceName,
                                                         mtsComponent * component,
                                                         mtsRequiredType required):
    Name(interfaceName),
    Component(component),
    Required(required)
{
}


mtsInterfaceRequiredOrInput::~mtsInterfaceRequiredOrInput()
{
}


const std::string & mtsInterfaceRequiredOrInput::GetName(void) const
{
    return this->Name;
}


const std::string mtsInterfaceRequiredOrInput::GetFullName(void) const
{
    if (this->Component) {
        return this->Component->GetName() + ":" + this->GetName();
    }
    return "[no component]:" + this->GetName();
}


const mtsComponent *  mtsInterfaceRequiredOrInput::GetComponent(void) const
{
    return this->Component;
}


mtsRequiredType mtsInterfaceRequiredOrInput::IsRequired(void) const
{
    return this->Required;
}
