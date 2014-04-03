/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2011

  (C) Copyright 2006-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlFilterImageThresholding.h>


/******************************************/
/*** svlFilterImageThresholding class *****/
/******************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterImageThresholding, svlFilterBase)

svlFilterImageThresholding::svlFilterImageThresholding() :
    svlFilterBase(),
    Threshold(128)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageMono8);
    AddInputType("input", svlTypeImageMono16);
    AddInputType("input", svlTypeImageMono32);
    AddInputType("input", svlTypeImageMono8Stereo);
    AddInputType("input", svlTypeImageMono16Stereo);
    AddInputType("input", svlTypeImageMono32Stereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);
}

void svlFilterImageThresholding::SetThreshold(unsigned int threshold)
{
    Threshold = threshold;
}

unsigned int svlFilterImageThresholding::GetThreshold() const
{
    return Threshold;
}

int svlFilterImageThresholding::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    return SVL_OK;
}

int svlFilterImageThresholding::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);
    _SkipIfDisabled();

    svlSampleImage* img = dynamic_cast<svlSampleImage*>(syncInput);
    unsigned int videochannels = img->GetVideoChannels();
    svlPixelType type = img->GetPixelType();
    unsigned int idx;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        if (type == svlPixelMono8) {
            PerformThresholding(img->GetUCharPointer(idx), img->GetDataSize(idx), static_cast<unsigned char>(Threshold), static_cast<unsigned char>(0xFF));
        }
        else if (type == svlPixelMono16) {
            PerformThresholding(reinterpret_cast<unsigned short*>(img->GetUCharPointer(idx)), img->GetDataSize(idx) >> 1, static_cast<unsigned short>(Threshold), static_cast<unsigned short>(0xFFFF));
        }
        else if (type == svlPixelMono32) {
            PerformThresholding(reinterpret_cast<unsigned int*>(img->GetUCharPointer(idx)), img->GetDataSize(idx) >> 2, static_cast<unsigned int>(Threshold), static_cast<unsigned int>(0xFFFFFFFF));
        }
    }

    return SVL_OK;
}

template<class _ValueType>
void svlFilterImageThresholding::PerformThresholding(_ValueType* image, unsigned int pixelcount, const _ValueType threshold, const _ValueType max_value)
{
    while (pixelcount) {
        if (*image < threshold) *image = 0;
        else *image = max_value;
        image ++;
        pixelcount --;
    }
}

