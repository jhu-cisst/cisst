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

#include <cisstStereoVision/svlFilterVideoExposureManager.h>


/*******************************************/
/*** svlFilterVideoExposureManager class ***/
/*******************************************/

CMN_IMPLEMENT_SERVICES(svlFilterVideoExposureManager)

svlFilterVideoExposureManager::svlFilterVideoExposureManager() :
    svlFilterBase(),
    SourceFilter(0),
    Tolerance(0.3),
    Threshold(255),
    MaxGain(1000),
    MaxShutter(1305)
{
    Histogram.SetSize(256);
    Histogram.SetAll(0);

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
#if (CISST_OS != CISST_LINUX_RTAI) && (CISST_OS != CISST_LINUX) && (CISST_OS != CISST_SOLARIS) && (CISST_OS != CISST_QNX)
    // Doesn't seem to be working when (CISST_OS == CISST_DARWIN)
    SourceFilter = 0;
#endif
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
    MaxGain = static_cast<int>(maxgain);
}

unsigned int svlFilterVideoExposureManager::GetMaxGain() const
{
    return static_cast<unsigned int>(MaxGain);
}

void svlFilterVideoExposureManager::SetMaxShutter(unsigned int maxshutter)
{
    MaxShutter = static_cast<int>(maxshutter);
}

unsigned int svlFilterVideoExposureManager::GetMaxShutter() const
{
    return static_cast<unsigned int>(MaxShutter);
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

            if (IsEnabled()) {
                int change, exposure;
                unsigned int maxval;
                double area;

                // Compute size of saturated area
                GetSaturationRatio(img, SVL_LEFT, area, maxval);
                if (area < Tolerance) change = (255 - maxval) / 8;
                else change = std::min(50, static_cast<int>((Tolerance - area) * 25));

                // Get exposure parameters from the capture filter
                svlFilterSourceVideoCapture::ImageProperties properties;
                SourceFilter->GetImageProperties(properties, SVL_LEFT);

                // Make sure auto-shutter and auto-gain are turned off
                properties.manual |= svlFilterSourceVideoCapture::propShutter + svlFilterSourceVideoCapture::propGain;

                exposure = static_cast<int>(properties.shutter + properties.gain) + change;
                properties.shutter = static_cast<unsigned int>(std::max(0, std::min(exposure, MaxShutter)));
                properties.gain = static_cast<unsigned int>(std::max(0, std::min(exposure - MaxShutter, MaxGain)));

                // Set modified exposure parameters in the capture filter
                if (change != 0) {
                    properties.mask = svlFilterSourceVideoCapture::propShutter + svlFilterSourceVideoCapture::propGain;
                    for (unsigned int i = 0; i < videochannels; i ++) {
                        SourceFilter->SetImageProperties(properties, i);
                    }
                }
            }
            else {
                bool modified;
                svlFilterSourceVideoCapture::ImageProperties properties;

                for (unsigned int i = 0; i < videochannels; i ++) {
                    // Get exposure parameters from the capture filter
                    SourceFilter->GetImageProperties(properties, i);

                    // Make sure auto-shutter and auto-gain are turned on
                    modified = false;
                    if (properties.manual & svlFilterSourceVideoCapture::propShutter) {
                        properties.manual -= svlFilterSourceVideoCapture::propShutter;
                        modified = true;
                    }
                    if (properties.manual & svlFilterSourceVideoCapture::propGain) {
                        properties.manual -= svlFilterSourceVideoCapture::propGain;
                        modified = true;
                    }
                    if (!modified) continue;

                    // Set modified exposure parameters in the capture filter
                    properties.mask = svlFilterSourceVideoCapture::propShutter + svlFilterSourceVideoCapture::propGain;
                    SourceFilter->SetImageProperties(properties, i);
                }
            }
        }
    }

    return SVL_OK;
}

void svlFilterVideoExposureManager::GetSaturationRatio(svlSampleImage* image, const unsigned int videoch, double& saturation, unsigned int& maxval)
{
    const unsigned int pixelcount = image->GetWidth(videoch) * image->GetHeight(videoch);
    unsigned char* ptr = image->GetUCharPointer(videoch);
    unsigned char uch;
    int area = 0;

    Histogram.SetAll(0);

    for (unsigned int i = 0; i < pixelcount; i ++) {
        saturation = false;
        uch = *ptr; ptr ++; Histogram[uch] ++; if (uch >= Threshold) saturation = true;
        uch = *ptr; ptr ++; Histogram[uch] ++; if (uch >= Threshold) saturation = true;
        uch = *ptr; ptr ++; Histogram[uch] ++; if (uch >= Threshold) saturation = true;
        if (saturation) area ++;
    }

    saturation = 100.0 * area / pixelcount;

    maxval = 255;
    area = static_cast<int>(Tolerance / 100.0 * pixelcount);
    while (maxval && area > 0) {
        area -= Histogram[maxval];
        maxval --;
    }
}

