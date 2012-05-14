/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsStealthlinkControllerComponent.cpp 2805 2011-08-03 19:17:49Z wliu25 $

  Author(s):  Wen P. Liu
  Created on: 2011-08-12

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstStealthlink/mtsStealthlinkControllerComponent.h>

CMN_IMPLEMENT_SERVICES(mtsStealthlinkControllerComponent);


mtsStealthlinkControllerComponent::mtsStealthlinkControllerComponent(const std::string &name) :
        mtsComponent(name)
{
    mtsInterfaceRequired *required = AddInterfaceRequired("Stealthlink");
    if (required) {
        required->AddFunction("GetTool", GetTool);
        required->AddFunction("GetFrame", GetFrame);
    }

    // AddToolInterface("Pointer", Pointer);
    mtsInterfaceRequired* requiredPointer = AddInterfaceRequired("Pointer", MTS_OPTIONAL);
    if (requiredPointer) {
        requiredPointer->AddFunction("GetPositionCartesian", GetPositionCartesian);
        requiredPointer->AddFunction("GetMarkerCartesian", GetMarkerCartesian);
    }

    // AddToolInterface("Frame", Frame);
    mtsInterfaceRequired *requiredFrame = AddInterfaceRequired("Frame", MTS_OPTIONAL);
    if (requiredFrame) {
        requiredFrame->AddFunction("GetPositionCartesian", GetPositionCartesian);
        requiredFrame->AddFunction("GetMarkerCartesian", GetMarkerCartesian);
    }

    // AddToolInterface("Endoscope", Endoscope);
    requiredPointer = AddInterfaceRequired("Endoscope", MTS_OPTIONAL);
    if (requiredPointer) {
        requiredPointer->AddFunction("GetPositionCartesian", GetPositionCartesian);
        requiredPointer->AddFunction("GetMarkerCartesian", GetMarkerCartesian);
    }

}
