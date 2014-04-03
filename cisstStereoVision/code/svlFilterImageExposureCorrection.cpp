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

#include <cisstStereoVision/svlFilterImageExposureCorrection.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include "svlImageProcessingHelper.h"


/**********************************************/
/*** svlFilterImageExposureCorrection class ***/
/**********************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterImageExposureCorrection, svlFilterBase)

svlFilterImageExposureCorrection::svlFilterImageExposureCorrection() :
    svlFilterBase(),
    Brightness(0.0),
    Contrast(0.0),
    Gamma(0.0)
{
    CreateInterfaces();
    
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBStereo);
    AddInputType("input", svlTypeImageRGBA);
    AddInputType("input", svlTypeImageRGBAStereo);
    AddInputType("input", svlTypeImageMono8);
    AddInputType("input", svlTypeImageMono8Stereo);

// Might be added in the future
//    AddInputType("input", svlTypeImageMono16);
//    AddInputType("input", svlTypeImageMono16Stereo);
//    AddInputType("input", svlTypeImageMono32);
//    AddInputType("input", svlTypeImageMono32Stereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);
}

void svlFilterImageExposureCorrection::SetBrightness(const double & brightness)
{
    Brightness = brightness;
    if (Brightness > 100.0) Brightness = 100.0;
    else if (Brightness < -100.0) Brightness = -100.0;
}

void svlFilterImageExposureCorrection::GetBrightness(double & brightness) const
{
    brightness = Brightness;
}

void svlFilterImageExposureCorrection::SetContrast(const double & contrast)
{
    Contrast = contrast;
    if (Contrast > 100.0) Contrast = 100.0;
    else if (Contrast < -100.0) Contrast = -100.0;
}

void svlFilterImageExposureCorrection::GetContrast(double & contrast) const
{
    contrast = Contrast;
}

void svlFilterImageExposureCorrection::SetGamma(const double & gamma)
{
    Gamma = gamma;
    if (Gamma > 100.0) Gamma = 100.0;
    else if (Gamma < -100.0) Gamma = -100.0;
}

void svlFilterImageExposureCorrection::GetGamma(double & gamma) const
{
    gamma = Gamma;
}

int svlFilterImageExposureCorrection::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    svlSampleImage* img = dynamic_cast<svlSampleImage*>(syncInput);
    if (!img) return SVL_FAIL;
    Exposure.SetSize(img->GetVideoChannels());
    return SVL_OK;
}

int svlFilterImageExposureCorrection::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);
    _SkipIfDisabled();

    svlSampleImage* img = dynamic_cast<svlSampleImage*>(syncInput);
    const unsigned int videochannels = img->GetVideoChannels();
    unsigned int vch;

    _ParallelLoop(procInfo, vch, videochannels)
    {
        svlImageProcessing::SetExposure(img, vch, Brightness, Contrast, Gamma, Exposure[vch]);
    }

    return SVL_OK;
}

void svlFilterImageExposureCorrection::CreateInterfaces()
{
    // Add NON-QUEUED provided interface for configuration management
    mtsInterfaceProvided* provided = AddInterfaceProvided("Settings", MTS_COMMANDS_SHOULD_NOT_BE_QUEUED);
    if (provided) {
        provided->AddCommandWrite(&svlFilterImageExposureCorrection::SetBrightness, this, "SetBrightness");
        provided->AddCommandWrite(&svlFilterImageExposureCorrection::SetContrast,   this, "SetContrast");
        provided->AddCommandWrite(&svlFilterImageExposureCorrection::SetGamma,      this, "SetGamma");
        provided->AddCommandRead (&svlFilterImageExposureCorrection::GetBrightness, this, "GetBrightness");
        provided->AddCommandRead (&svlFilterImageExposureCorrection::GetContrast,   this, "GetContrast");
        provided->AddCommandRead (&svlFilterImageExposureCorrection::GetGamma,      this, "GetGamma");
    }
}

