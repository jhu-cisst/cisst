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

    // the following two tools are using names normally defined in config.xml
    AddToolInterface("Pointer", Pointer);
    AddToolInterface("Frame", Frame);

     // get registration information
    required = AddInterfaceRequired("Registration");
    if (required) {
        required->AddFunction("GetTransformation", Registration.GetTransformation);
        required->AddFunction("GetPredictedAccuracy", Registration.GetPredictedAccuracy);
        required->AddFunction("GetValid", Registration.GetValid);
    }

    // get exam information
    required = AddInterfaceRequired("ExamInformation");
    if (required) {
        required->AddFunction("RequestExamInformation", ExamInformation.RequestExamInformation);
        required->AddFunction("GetVoxelScale", ExamInformation.GetVoxelScale);
        required->AddFunction("GetSize", ExamInformation.GetSize);
        required->AddFunction("GetValid", ExamInformation.GetValid);
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
    vctFrm3 vctFrm;
    mtsFrm3 mtsFrm;
    mtsDouble predictedAccuracy;

    bool didOutput = false;
    mtsExecutionResult result;
    result = Stealthlink.GetTool(StealthTool);
    if (!result.IsOK()) {
        std::cout << "Stealthlink.GetTool() failed: " << result << std::endl;
    }
    if (StealthTool.Valid()) {
        std::cout << "Tool " << StealthTool.GetName() << ": " << StealthTool.GetFrame().Translation() << "; ";
        didOutput = true;
    }

    result = Stealthlink.GetFrame(StealthFrame);
    if (!result.IsOK()) {
        std::cout << "Stealthlink.GetFrame() failed: " << result << std::endl;
    }
    if (StealthFrame.Valid()) {
        std::cout << "Frame " << StealthFrame.GetName() << ": " << StealthFrame.GetFrame().Translation() << "; ";
        didOutput = true;
    }

    if (Pointer.GetPositionCartesian.IsValid()) {
        result = Pointer.GetPositionCartesian(prmPos);
        if (!result.IsOK()) {
            std::cout << "Pointer.GetPositionCartesian() failed: " << result << std::endl;
        }
        if (prmPos.Valid()) {
            std::cout << "Interface Pointer: " << prmPos.Position().Translation() << "; ";
            didOutput = true;
        }
    }
    if (Pointer.GetMarkerCartesian.IsValid()) {
        result = Pointer.GetMarkerCartesian(prmPos);
        if (!result.IsOK()) {
            std::cout << "Pointer.GetMarkerCartesian() failed: " << result << std::endl;
        }
        if (prmPos.Valid()) {
            std::cout << "Interface PointerM: " << prmPos.Position().Translation() << "; ";
            didOutput = true;
        }
    }

    if (Frame.GetPositionCartesian.IsValid()) {
        result = Frame.GetPositionCartesian(prmPos);
        if (!result.IsOK()) {
            std::cout << "Frame.GetPositionCartesian() failed: " << result << std::endl;
        }
        if (prmPos.Valid()) {
            std::cout << "Interface Frame: " << prmPos.Position().Translation() << "; ";
            didOutput = true;
        } else {
            std::cerr << "Interface Frame, invalid position" << std::endl;
            didOutput = true;
        }
    }
    if (Frame.GetMarkerCartesian.IsValid()) {
        result = Frame.GetMarkerCartesian(prmPos);
        if (!result.IsOK()) {
            std::cout << "Frame.GetMarkerCartesian() failed: " << result << std::endl;
        }
        if (prmPos.Valid()) {
            std::cout << "Interface FrameM: " << prmPos.Position().Translation() << "; ";;
            didOutput = true;
        } else {
            std::cerr << "Interface Frame, invalid position" << std::endl;
            didOutput = true;
        }
    }

    mtsBool valid;
    if (Registration.GetValid.IsValid()) {
        result = Registration.GetValid(valid);
        if (!result.IsOK()) {
            std::cout << "Registration.GetValid() failed: " << result << std::endl;
        }
        if (valid) {
            Registration.GetTransformation(vctFrm);
            std::cout << "Registration: " << vctFrm.Translation() << "; ";;
            didOutput = true;
        } else {
            std::cout << "Registration: invalid" << std::endl;
            didOutput = true;
        }
    }

    if (ExamInformation.RequestExamInformation.IsValid()) {
        result = ExamInformation.RequestExamInformation.ExecuteBlocking();
        if (!result.IsOK()) {
            std::cout << "ExamInformation.RequestExamInformation() failed: " << result << std::endl;
        } else {
            result = ExamInformation.GetValid(valid);
            if (!result.IsOK()) {
                std::cout << "ExamInformation.GetValid() failed: " << result << std::endl;
            } else {
                if (valid) {
                    vctDouble3 voxelScale;
                    result = ExamInformation.GetVoxelScale(voxelScale);
                    if (!result.IsOK()) {
                        std::cout << "ExamInformation.GetVoxelScale() failed: " << result << std::endl;
                    } else {
                        std::cout << "Voxel scale: " << voxelScale << "; ";
                        didOutput = true;
                    }
                    vctInt3 sizes;
                    result = ExamInformation.GetSize(sizes);
                    if (!result.IsOK()) {
                        std::cout << "ExamInformation.GetSize() failed: " << result << std::endl;
                    } else {
                        std::cout << "Size: " << sizes << "; ";
                    }
                } else {
                    std::cout << "ExamInformation is not valid" << std::endl;
                    didOutput = true;
                }
            }
        }
    }

    if (didOutput) {
        std::cout << std::endl;
    } else {
        std::cout << "." << std::flush;
    }
}
