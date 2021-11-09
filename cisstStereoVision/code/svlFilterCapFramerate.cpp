/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Balazs Vagvolgyi, Anton Deguet
  Created on: 2011

  (C) Copyright 2011-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlFilterCapFramerate.h>
#include <cisstStereoVision/svlFilterInput.h>
#include <cisstStereoVision/svlFilterOutput.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsManagerLocal.h>

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterCapFramerate, svlFilterBase)

svlFilterCapFramerate::svlFilterCapFramerate() :
svlFilterBase(),
    TimeForLastFrame(0.0)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBA);
    AddInputType("input", svlTypeImageRGBStereo);
    AddInputType("input", svlTypeImageRGBAStereo);
    AddInputType("input", svlTypeImageMono8);
    AddInputType("input", svlTypeImageMono8Stereo);
    AddInputType("input", svlTypeImageMono16);
    AddInputType("input", svlTypeImageMono16Stereo);
    AddInputType("input", svlTypeImageMono32);
    AddInputType("input", svlTypeImageMono32Stereo);
    AddInputType("input", svlTypeImage3DMap);
    AddInputType("input", svlTypeCUDAImageRGB);
    AddInputType("input", svlTypeCUDAImageRGBA);
    AddInputType("input", svlTypeCUDAImageRGBStereo);
    AddInputType("input", svlTypeCUDAImageRGBAStereo);
    AddInputType("input", svlTypeCUDAImageMono8);
    AddInputType("input", svlTypeCUDAImageMono8Stereo);
    AddInputType("input", svlTypeCUDAImageMono16);
    AddInputType("input", svlTypeCUDAImageMono16Stereo);
    AddInputType("input", svlTypeCUDAImageMono32);
    AddInputType("input", svlTypeCUDAImageMono32Stereo);
    AddInputType("input", svlTypeCUDAImage3DMap);
    AddInputType("input", svlTypeMatrixInt8);
    AddInputType("input", svlTypeMatrixInt16);
    AddInputType("input", svlTypeMatrixInt32);
    AddInputType("input", svlTypeMatrixInt64);
    AddInputType("input", svlTypeMatrixUInt8);
    AddInputType("input", svlTypeMatrixUInt16);
    AddInputType("input", svlTypeMatrixUInt32);
    AddInputType("input", svlTypeMatrixUInt64);
    AddInputType("input", svlTypeMatrixFloat);
    AddInputType("input", svlTypeMatrixDouble);
    AddInputType("input", svlTypeTransform3D);
    AddInputType("input", svlTypeTargets);
    AddInputType("input", svlTypeText);
    AddInputType("input", svlTypeBlobs);

    AddOutput("output", true);
    SetAutomaticOutputType(true);
}


double svlFilterCapFramerate::SetTimeInterval(double maxFrameIntervalInSeconds)
{
    double previousValue = this->DesiredFrameInterval;
    this->DesiredFrameInterval = maxFrameIntervalInSeconds;
    return previousValue;
}


int svlFilterCapFramerate::Initialize(svlSample * syncInput, svlSample * & syncOutput)
{
    syncOutput = syncInput;
    return SVL_OK;
}


int svlFilterCapFramerate::Process(svlProcInfo * procInfo, svlSample * syncInput, svlSample * & syncOutput)
{
    syncOutput = syncInput;
    _OnSingleThread(procInfo)
    {
        const double currentTime = mtsManagerLocal::GetInstance()->GetTimeServer().GetRelativeTime();
        const double timeSinceLastFrame = currentTime - this->TimeForLastFrame;
        this->TimeForLastFrame = currentTime;
        if (timeSinceLastFrame < this->DesiredFrameInterval) {
            osaSleep(this->DesiredFrameInterval - timeSinceLastFrame);
        }
    }
    return SVL_OK;
}
