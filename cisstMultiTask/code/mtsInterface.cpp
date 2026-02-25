/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Peter Kazanzides, Anton Deguet, Min Yang Jung
  Created on: 2008-11-13

  (C) Copyright 2008-2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsInterface.h>
#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsManagerLocal.h>


static const std::string mtsInterfaceNoComponentName = "NoName";


mtsInterface::mtsInterface(const std::string & interfaceName,
                           mtsComponent * component):
    Name(interfaceName),
    Component(component)
{
    Initialize();
    AddTag("Generic");
}


void mtsInterface::Initialize(void)
{
}


mtsInterface::~mtsInterface()
{
}


const std::string & mtsInterface::GetName(void) const
{
    return this->Name;
}


const std::string mtsInterface::GetFullName(void) const
{
    if (this->Component) {
        return this->Component->GetName() + ":" + this->GetName();
    }
    return "[no component]:" + this->GetName();
}


const mtsComponent *  mtsInterface::GetComponent(void) const
{
    return this->Component;
}


const std::string & mtsInterface::GetComponentName(void) const
{
    // This shouldn't happen
    if (!Component) {
        return mtsInterfaceNoComponentName;
    }
    return Component->GetName();
}


void mtsInterface::AddTag(const std::string & tag)
{
    if (mtsManagerLocal::GetInstance()->IsValidInterfaceTag(tag)) {
        this->mTags.insert(tag);
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "AddTag: " << tag << " is not a valid interface tag. "
                                 << "Use mtsManagerLocal::AddValidInterfaceTag to register it." << std::endl;
    }
}


const std::set<std::string> & mtsInterface::GetTags(void) const
{
    return this->mTags;
}
