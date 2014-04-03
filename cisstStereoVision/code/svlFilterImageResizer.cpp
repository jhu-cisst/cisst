/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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
#include <cisstMultiTask/mtsInterfaceProvided.h>


/******************************************/
/*** svlFilterImageResizer class **********/
/******************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterImageResizer, svlFilterBase)

svlFilterImageResizer::svlFilterImageResizer() :
    svlFilterBase(),
    OutputImage(0)
{
    CreateInterfaces();

    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageMono8);
    AddInputType("input", svlTypeImageRGBStereo);
    AddInputType("input", svlTypeImageMono8Stereo);
//  TO DO:
//    svlTypeImageMono16 and svlTypeImageMono16Stereo
//    svlTypeImageMono32 and svlTypeImageMono32Stereo

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

void svlFilterImageResizer::SetInterpolation(const bool & enable)
{
    InterpolationEnabled = enable;
}

void svlFilterImageResizer::GetInterpolation(bool & enable) const
{
    enable = InterpolationEnabled;
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

    svlSampleImage* inputimage = dynamic_cast<svlSampleImage*>(syncInput);
    const unsigned int numofchannels = inputimage->GetVideoChannels();
    unsigned int i;

    EqualSize = true;
    for (i = 0; i < numofchannels; i ++) {
        if (Width[i] == 0 && Height[i] == 0) {
            if (WidthRatio[i]  != 1.0 ||
                HeightRatio[i] != 1.0) EqualSize = false;
        }
        else {
            if (Width[i]  != inputimage->GetWidth(i) ||
                Height[i] != inputimage->GetHeight(i)) EqualSize = false;
        }
    }

    if (!EqualSize) {
        svlStreamType type = GetInput()->GetType();

        switch (type) {
            case svlTypeImageRGB:
            case svlTypeImageRGBStereo:
            case svlTypeImageMono8:
            case svlTypeImageMono8Stereo:
                OutputImage = dynamic_cast<svlSampleImage*>(svlSample::GetNewFromType(type));
            break;

            case svlTypeImageRGBA:          // To be added
            case svlTypeImageRGBAStereo:    // To be added
            case svlTypeImageMono16:        // To be added
            case svlTypeImageMono16Stereo:  // To be added
            case svlTypeImageMono32:        // To be added
            case svlTypeImageMono32Stereo:  // To be added

                // Other types may be added in the future
            case svlTypeImage3DMap:
            case svlTypeCUDAImageRGB:
            case svlTypeCUDAImageRGBA:
            case svlTypeCUDAImageRGBStereo:
            case svlTypeCUDAImageRGBAStereo:
            case svlTypeCUDAImageMono8:
            case svlTypeCUDAImageMono8Stereo:
            case svlTypeCUDAImageMono16:
            case svlTypeCUDAImageMono16Stereo:
            case svlTypeCUDAImageMono32:
            case svlTypeCUDAImageMono32Stereo:
            case svlTypeCUDAImage3DMap:
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
            case svlTypeInvalid:
            case svlTypeStreamSource:
            case svlTypeStreamSink:
            case svlTypeTransform3D:
            case svlTypeTargets:
            case svlTypeText:
            case svlTypeCameraGeometry:
            case svlTypeBlobs:
            break;
        }

        for (i = 0; i < numofchannels; i ++) {

            if (Width[i] == 0 || Height[i] == 0) {
                Width[i] = static_cast<unsigned int>(WidthRatio[i] * inputimage->GetWidth(i));
                Height[i] = static_cast<unsigned int>(HeightRatio[i] * inputimage->GetHeight(i));
                if (Width[i] < 1) Width[i] = 1;
                if (Height[i] < 1) Height[i] = 1;
            }
            else {
                WidthRatio[i] = static_cast<double>(Width[i]) / inputimage->GetWidth(i);
                HeightRatio[i] = static_cast<double>(Height[i]) / inputimage->GetHeight(i);
            }

            OutputImage->SetSize(i, Width[i], Height[i]);
        }

        syncOutput = OutputImage;
    }
    else {
        syncOutput = syncInput;
    }

    return SVL_OK;
}

int svlFilterImageResizer::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    if (EqualSize) {
        syncOutput = syncInput;
        return SVL_OK;
    }

    syncOutput = OutputImage;

    if (IsDisabled()) {
        // Do not process; resend the last processed output sample
        return SVL_OK;
    }

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

void svlFilterImageResizer::CreateInterfaces()
{
    // Add NON-QUEUED provided interface for configuration management
    mtsInterfaceProvided* provided = AddInterfaceProvided("Settings", MTS_COMMANDS_SHOULD_NOT_BE_QUEUED);
    if (provided) {
        provided->AddCommandWrite(&svlFilterImageResizer::SetInterpolation,           this, "SetInterpolation");
        provided->AddCommandWrite(&svlFilterImageResizer::SetOutputDimensionLCommand, this, "SetOutputDimension");
        provided->AddCommandWrite(&svlFilterImageResizer::SetOutputDimensionLCommand, this, "SetLeftOutputDimension");
        provided->AddCommandWrite(&svlFilterImageResizer::SetOutputDimensionRCommand, this, "SetRightOutputDimension");
        provided->AddCommandWrite(&svlFilterImageResizer::SetOutputRatioLCommand,     this, "SetOutputRatio");
        provided->AddCommandWrite(&svlFilterImageResizer::SetOutputRatioLCommand,     this, "SetLeftOutputRatio");
        provided->AddCommandWrite(&svlFilterImageResizer::SetOutputRatioRCommand,     this, "SetRightOutputRatio");
        provided->AddCommandRead (&svlFilterImageResizer::GetInterpolation,           this, "GetInterpolation");
        provided->AddCommandRead (&svlFilterImageResizer::GetOutputDimensionLCommand, this, "GetOutputDimension");
        provided->AddCommandRead (&svlFilterImageResizer::GetOutputDimensionLCommand, this, "GetLeftOutputDimension");
        provided->AddCommandRead (&svlFilterImageResizer::GetOutputDimensionRCommand, this, "GetRightOutputDimension");
        provided->AddCommandRead (&svlFilterImageResizer::GetOutputRatioLCommand,     this, "GetOutputRatio");
        provided->AddCommandRead (&svlFilterImageResizer::GetOutputRatioLCommand,     this, "GetLeftOutputRatio");
        provided->AddCommandRead (&svlFilterImageResizer::GetOutputRatioRCommand,     this, "GetRightOutputRatio");
    }
}

void svlFilterImageResizer::SetOutputDimensionLCommand(const vctInt2 & dimension)
{
    if (IsInitialized()) {
        CMN_LOG_CLASS_INIT_ERROR << "SetOutputDimensionLCommand: failed to set dimension; filter is already initialized" << std::endl;
        return;
    }
    if (SetOutputSize(dimension[0], dimension[1], SVL_LEFT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "SetOutputDimensionLCommand: invalid image size (" << dimension[0] << ", " << dimension[1] << ")" << std::endl;
    }
}

void svlFilterImageResizer::SetOutputDimensionRCommand(const vctInt2 & dimension)
{
    if (IsInitialized()) {
        CMN_LOG_CLASS_INIT_ERROR << "SetOutputDimensionRCommand: failed to set dimension; filter is already initialized" << std::endl;
        return;
    }
    if (SetOutputSize(dimension[0], dimension[1], SVL_RIGHT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "SetOutputDimensionRCommand: invalid image size (" << dimension[0] << ", " << dimension[1] << ")" << std::endl;
    }
}

void svlFilterImageResizer::SetOutputRatioLCommand(const vctDouble2 & ratio)
{
    if (IsInitialized()) {
        CMN_LOG_CLASS_INIT_ERROR << "SetOutputRatioLCommand: failed to set dimension; filter is already initialized" << std::endl;
        return;
    }
    if (SetOutputRatio(ratio[0], ratio[1], SVL_LEFT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "SetOutputRatioLCommand: invalid image size (" << ratio[0] << ", " << ratio[1] << ")" << std::endl;
    }
}

void svlFilterImageResizer::SetOutputRatioRCommand(const vctDouble2 & ratio)
{
    if (IsInitialized()) {
        CMN_LOG_CLASS_INIT_ERROR << "SetOutputRatioRCommand: failed to set dimension; filter is already initialized" << std::endl;
        return;
    }
    if (SetOutputRatio(ratio[0], ratio[1], SVL_RIGHT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "SetOutputRatioRCommand: invalid image size (" << ratio[0] << ", " << ratio[1] << ")" << std::endl;
    }
}

void svlFilterImageResizer::GetOutputDimensionLCommand(vctInt2 & dimension) const
{
    dimension[0] = Width[SVL_LEFT];
    dimension[1] = Height[SVL_LEFT];
}

void svlFilterImageResizer::GetOutputDimensionRCommand(vctInt2 & dimension) const
{
    dimension[0] = Width[SVL_RIGHT];
    dimension[1] = Height[SVL_RIGHT];
}

void svlFilterImageResizer::GetOutputRatioLCommand(vctDouble2 & ratio) const
{
    ratio[0] = WidthRatio[SVL_LEFT];
    ratio[1] = HeightRatio[SVL_LEFT];
}

void svlFilterImageResizer::GetOutputRatioRCommand(vctDouble2 & ratio) const
{
    ratio[0] = WidthRatio[SVL_RIGHT];
    ratio[1] = HeightRatio[SVL_RIGHT];
}

