/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $
  
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

#include <cisstStereoVision/svlFilterVideoExposureManager.h>


/*******************************************/
/*** svlFilterVideoExposureManager class ***/
/*******************************************/

CMN_IMPLEMENT_SERVICES(svlFilterVideoExposureManager)

svlFilterVideoExposureManager::svlFilterVideoExposureManager() :
    svlFilterBase(),
    SourceFilter(0),
    Tolerance(0.5),
    Threshold(250),
    MaxGain(1024),
    MaxShutter(1300),
    ChangeStep(10)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBStereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);
}

int svlFilterVideoExposureManager::SetVideoCaptureFilter(svlFilterSourceVideoCapture* sourcefilter)
{
    if (IsInitialized()) return SVL_FAIL;
    SourceFilter = sourcefilter;
    return SVL_OK;
}

svlFilterSourceVideoCapture* svlFilterVideoExposureManager::GetVideoCaptureFilter() const
{
    return SourceFilter;
}

void svlFilterVideoExposureManager::SetSaturationTolerance(double tolerance)
{
    if (Tolerance < 0.0) Tolerance = 0.0;
    else if (Tolerance > 100.0) Tolerance = 100.0;
    Tolerance = tolerance;
}

double svlFilterVideoExposureManager::GetSaturationTolerance() const
{
    return Tolerance;
}

void svlFilterVideoExposureManager::SetSaturationThreshold(unsigned char threshold)
{
    Threshold = threshold;
}

unsigned char svlFilterVideoExposureManager::GetSaturationThreshold() const
{
    return Threshold;
}

void svlFilterVideoExposureManager::SetMaxGain(unsigned int maxgain)
{
    MaxGain = maxgain;
}

unsigned int svlFilterVideoExposureManager::GetMaxGain() const
{
    return MaxGain;
}

void svlFilterVideoExposureManager::SetMaxShutter(unsigned int maxshutter)
{
    MaxShutter = maxshutter;
}

unsigned int svlFilterVideoExposureManager::GetMaxShutter() const
{
    return MaxShutter;
}

void svlFilterVideoExposureManager::SetChangeStep(unsigned int step)
{
    ChangeStep = step;
}

unsigned int svlFilterVideoExposureManager::GetChangeStep() const
{
    return ChangeStep;
}

int svlFilterVideoExposureManager::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    return SVL_OK;
}

int svlFilterVideoExposureManager::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);

    if (SourceFilter) {
        _OnSingleThread(procInfo)
        {
            svlSampleImage* img = dynamic_cast<svlSampleImage*>(syncInput);
            const unsigned int videochannels = img->GetVideoChannels();
            double saturation;
            int value, change = 0;

            for (unsigned int i = 0; i < videochannels; i ++) {

                // Compute size of saturated area
                saturation = GetSaturationRatio(img, i);

                if (saturation > Tolerance) change = -ChangeStep;
                else change = ChangeStep;

                // Get exposure parameters from the capture filter
                svlFilterSourceVideoCapture::ImageProperties properties;
                SourceFilter->GetImageProperties(properties, i);

                // Make sure auto-shutter and auto-gain are turned off
                properties.manual &= svlFilterSourceVideoCapture::propWhiteBalance &
                                     svlFilterSourceVideoCapture::propBrightness &
                                     svlFilterSourceVideoCapture::propGamma &
                                     svlFilterSourceVideoCapture::propSaturation;

                // First try to change gain
                value = static_cast<int>(properties.gain) + change;
                if (value < 0) value = 0;
                else if (value > static_cast<int>(MaxGain)) value = static_cast<int>(MaxGain);

                if (properties.gain != static_cast<unsigned int>(value)) {
                    properties.gain = static_cast<unsigned int>(value);
                }
                else {

                    // If gain has reached its limits, try adjusting shutter
                    value = static_cast<int>(properties.shutter) + change;
                    if (value < 0) value = 0;
                    else if (value > static_cast<int>(MaxShutter)) value = static_cast<int>(MaxShutter);
                    properties.gain = static_cast<unsigned int>(value);
                }

                // Set modified exposure parameters in the capture filter
                SourceFilter->SetImageProperties(properties, i);
            }
        }
    }

    return SVL_OK;
}

double svlFilterVideoExposureManager::GetSaturationRatio(svlSampleImage* image, const unsigned int videoch)
{
    if (!image) return -1.0;

    const unsigned int pixelcount = image->GetWidth(videoch) * image->GetHeight(videoch);
    unsigned char* ptr = image->GetUCharPointer(videoch);
    unsigned int area = 0;
    double saturation;

    for (unsigned int i = 0; i < pixelcount; i ++) {
        saturation = false;
        if (*ptr >= Threshold) saturation = true; ptr ++;
        if (*ptr >= Threshold) saturation = true; ptr ++;
        if (*ptr >= Threshold) saturation = true; ptr ++;
        if (saturation) area ++;
    }

    return static_cast<double>(area) / pixelcount;
}

