/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2010

  (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlFilterImageConvolution.h>


/*****************************************/
/*** svlFilterImageConvolution class *****/
/*****************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterImageConvolution, svlFilterBase)

svlFilterImageConvolution::svlFilterImageConvolution() :
    svlFilterBase(),
    OutputImage(0),
    KernelSeparable(true),
    AbsoluteResults(false)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBStereo);
    AddInputType("input", svlTypeImageRGBA);
    AddInputType("input", svlTypeImageRGBAStereo);
    AddInputType("input", svlTypeImageMono8);
    AddInputType("input", svlTypeImageMono8Stereo);
    AddInputType("input", svlTypeImageMono16);
    AddInputType("input", svlTypeImageMono16Stereo);
    AddInputType("input", svlTypeImageMono32);
    AddInputType("input", svlTypeImageMono32Stereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);

    // Default kernel is a separable 5x5 Gaussian blur
    KernelHoriz.SetSize(5);
    KernelVert.SetSize(5);
    KernelHoriz[0] = 1.0 / 16.0;
    KernelHoriz[1] = 4.0 / 16.0;
    KernelHoriz[2] = 6.0 / 16.0;
    KernelHoriz[3] = 4.0 / 16.0;
    KernelHoriz[4] = 1.0 / 16.0;
    KernelVert.Assign(KernelHoriz);
}

void svlFilterImageConvolution::SetKernel(const vctDynamicMatrix<double> & kernel)
{
    Kernel.ForceAssign(kernel);
    KernelSeparable = false;
}

void svlFilterImageConvolution::SetKernel(const vctDynamicVector<double> & kernel_horiz, const vctDynamicVector<double> & kernel_vert)
{
    KernelHoriz.ForceAssign(kernel_horiz);
    KernelVert.ForceAssign(kernel_vert);
    KernelSeparable = true;
}

void svlFilterImageConvolution::SetAbsoluteResults(bool absres)
{
    AbsoluteResults = absres;
}

int svlFilterImageConvolution::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    OutputImage = dynamic_cast<svlSampleImage*>(syncInput->GetNewInstance());
    OutputImage->SetSize(syncInput);
    syncOutput = OutputImage;
    return SVL_OK;
}

int svlFilterImageConvolution::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = OutputImage;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);
    _SkipIfDisabled();

    svlSampleImage* img = dynamic_cast<svlSampleImage*>(syncInput);
    unsigned int videochannels = img->GetVideoChannels();
    unsigned int idx;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        if (KernelSeparable) {
            svlImageProcessing::Convolution(img, idx, OutputImage, idx, KernelHoriz, KernelVert, AbsoluteResults);
        }
        else {
            svlImageProcessing::Convolution(img, idx, OutputImage, idx, Kernel, AbsoluteResults);
        }
    }

    return SVL_OK;
}

