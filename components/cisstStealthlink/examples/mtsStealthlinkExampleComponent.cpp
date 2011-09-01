/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Peter Kazanzides, Anton Deguet
  Created on: 2011-07-14

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "mtsStealthlinkExampleComponent.h"
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsTransformationTypes.h>
#include <cisstParameterTypes/prmPositionCartesianGet.h>
#include <cisstStealthlink/mtsStealthlinkTypes.h>

mtsStealthlinkExampleComponent::mtsStealthlinkExampleComponent(const std::string & name,
                                                               double periodInSeconds):
    mtsTaskPeriodic(name, periodInSeconds)
{
    mtsInterfaceRequired * required = AddInterfaceRequired("Stealthlink");
    if (required) {
        required->AddFunction("GetTool", Stealthlink.GetTool);
        required->AddFunction("GetFrame", Stealthlink.GetFrame);
    }
    AddToolInterface("Pointer", Pointer);
    AddToolInterface("Frame", Frame);
    required = AddInterfaceRequired("Registration");
    if (required) {
        required->AddFunction("GetTransformation", Registration.GetTransformation);
        required->AddFunction("GetPredictedAccuracy", Registration.GetPredictedAccuracy);
    }
}

void mtsStealthlinkExampleComponent::AddToolInterface(const std::string & toolName,
                                                      mtsStealthlinkExampleComponent::ToolStruct & functionSet)
{
    mtsInterfaceRequired * required = AddInterfaceRequired(toolName, MTS_OPTIONAL);
    if (required) {
        required->AddFunction("GetPositionCartesian", functionSet.GetPositionCartesian);
        required->AddFunction("GetMarkerCartesian", functionSet.GetMarkerCartesian);
    }
}


void mtsStealthlinkExampleComponent::Run(void)
{
    mtsStealthTool StealthTool;
    mtsStealthFrame StealthFrame;
    prmPositionCartesianGet prmPos;
    mtsFrm3 mtsFrm;
    mtsDouble predictedAccuracy;

    bool didOutput = false;
    Stealthlink.GetTool(StealthTool);
    Stealthlink.GetFrame(StealthFrame);
    if (StealthTool.Valid()) {
        std::cout << "Tool " << StealthTool.GetName() << ": " << StealthTool.GetFrame().Translation() << "; ";
        didOutput = true;
    }
    if (StealthFrame.Valid()) {
        std::cout << "Frame " << StealthFrame.GetName() << ": " << StealthFrame.GetFrame().Translation() << "; ";
        didOutput = true;
    }
    if (Pointer.GetPositionCartesian.IsValid()) {
        Pointer.GetPositionCartesian(prmPos);
        if (prmPos.Valid()) {
            std::cout << "Interface Pointer: " << prmPos.Position().Translation() << "; ";
            didOutput = true;
        }
    }
    else if (Pointer.GetMarkerCartesian.IsValid()) {
        Pointer.GetMarkerCartesian(prmPos);
        if (prmPos.Valid()) {
            std::cout << "Interface PointerM: " << prmPos.Position().Translation() << "; ";
            didOutput = true;
        }
    }
    if (Frame.GetPositionCartesian.IsValid()) {
        Frame.GetPositionCartesian(prmPos);
        if (prmPos.Valid()) {
            std::cout << "Interface Frame: " << prmPos.Position().Translation();
            didOutput = true;
        }
    }
    else if (Frame.GetMarkerCartesian.IsValid()) {
        Frame.GetMarkerCartesian(prmPos);
        if (prmPos.Valid()) {
            std::cout << "Interface FrameM: " << prmPos.Position().Translation();
            didOutput = true;
        }
    }
    if (Registration.GetTransformation.IsValid()) {
        Registration.GetTransformation(mtsFrm);
        if (mtsFrm.Valid()) {
            std::cout << "Registration: " << mtsFrm.Translation();
            didOutput = true;
        }
    }
    if (didOutput) {
        std::cout << std::endl;
    }
}
