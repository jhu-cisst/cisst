/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Balazs Vagvolgyi
  Created on: 2007

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlFilterImageResizer.h>
#include <cisstStereoVision/svlImageProcessing.h>
#include <cisstStereoVision/svlFilterInput.h>


/******************************************/
/*** svlFilterImageResizer class **********/
/******************************************/

CMN_IMPLEMENT_SERVICES(svlFilterImageResizer)

svlFilterImageResizer::svlFilterImageResizer() :
    svlFilterBase(),
    OutputImage(0)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageMono8);
    AddInputType("input", svlTypeImageRGBStereo);
    AddInputType("input", svlTypeImageMono8Stereo);
//  TO DO:
//    svlTypeImageMono16 and svlTypeImageMono16Stereo

    AddOutput("output", true);
    SetAutomaticOutputType(true);

    for (unsigned int i = 0; i < 2; i ++) {
        WidthRatio[i] = HeightRatio[i] = 1.0;
        Width[i] = Height[i] = 0;
    }
    InterpolationEnabled = false;
}

svlFilterImageResizer::~svlFilterImageResizer()
{
    Release();
}

int svlFilterImageResizer::SetOutputSize(unsigned int width, unsigned int height, unsigned int videoch)
{
    if (IsInitialized() == true || videoch > 1 || width < 1 || height < 1) return SVL_FAIL;
    Width[videoch] = width;
    Height[videoch] = height;
    return SVL_OK;
}

void svlFilterImageResizer::SetInterpolation(bool enable)
{
    InterpolationEnabled = enable;
}

int svlFilterImageResizer::SetOutputRatio(double widthratio, double heightratio, unsigned int videoch)
{
    if (IsInitialized() == true || videoch > 1 || widthratio <= 0.0 || heightratio <= 0.0) return SVL_FAIL;
    WidthRatio[videoch] = widthratio;
    HeightRatio[videoch] = heightratio;
    Width[videoch] = 0;
    Height[videoch] = 0;
    return SVL_OK;
}

int svlFilterImageResizer::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    Release();

    svlStreamType type = GetInput()->GetType();

    switch (type) {
        case svlTypeImageRGB:
        case svlTypeImageRGBStereo:
        case svlTypeImageMono8:
        case svlTypeImageMono8Stereo:
            OutputImage = dynamic_cast<svlSampleImage*>(svlSample::GetNewFromType(type));
        break;

        case svlTypeImageMono16:        // To be added
        case svlTypeImageMono16Stereo:  // To be added

        // Other types may be added in the future
        case svlTypeImageRGBA:
        case svlTypeImageRGBAStereo:
        case svlTypeMatrixInt8:
        case svlTypeMatrixInt16:
        case svlTypeMatrixInt32:
        case svlTypeMatrixInt64:
        case svlTypeMatrixUInt8:
        case svlTypeMatrixUInt16:
        case svlTypeMatrixUInt32:
        case svlTypeMatrixUInt64:
        case svlTypeMatrixFloat:
        case svlTypeMatrixDouble:
        case svlTypeImage3DMap:
        case svlTypeInvalid:
        case svlTypeStreamSource:
        case svlTypeStreamSink:
        case svlTypeTransform3D:
        case svlTypeTargets:
        case svlTypeText:
        break;
    }

    svlSampleImage* inputimage = dynamic_cast<svlSampleImage*>(syncInput);
    const unsigned int numofchannels = inputimage->GetVideoChannels();

    for (unsigned int i = 0; i < numofchannels; i ++) {

        if (Width[i] == 0 || Height[i] == 0) {
            Width[i] = static_cast<unsigned int>(WidthRatio[i] * inputimage->GetWidth(i));
            Height[i] = static_cast<unsigned int>(HeightRatio[i] * inputimage->GetHeight(i));
            if (Width[i] < 1) Width[i] = 1;
            if (Height[i] < 1) Height[i] = 1;
        }

        OutputImage->SetSize(i, Width[i], Height[i]);
    }

    syncOutput = OutputImage;

    return SVL_OK;
}

int svlFilterImageResizer::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = OutputImage;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);

    svlSampleImage* id = dynamic_cast<svlSampleImage*>(syncInput);
    unsigned int videochannels = id->GetVideoChannels();
    unsigned int idx;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        svlImageProcessing::Resize(id, idx, OutputImage, idx, InterpolationEnabled, Internals[idx]);
    }

    return SVL_OK;
}

int svlFilterImageResizer::Release()
{
    if (OutputImage) {
        delete OutputImage;
        OutputImage = 0;
    }
    return SVL_OK;
}

