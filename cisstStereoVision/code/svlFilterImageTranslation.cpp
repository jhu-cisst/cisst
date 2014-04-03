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

#include <cisstStereoVision/svlFilterImageTranslation.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>


/****************************************/
/*** svlFilterImageTranslation class ****/
/****************************************/

CMN_IMPLEMENT_SERVICES(svlFilterImageTranslation)

svlFilterImageTranslation::svlFilterImageTranslation() :
    svlFilterBase(),
    OutputImage(0)
{
    CreateInterfaces();
    
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBStereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);

    HorizTranslation.SetAll(0);
    VertTranslation.SetAll(0);
}

int svlFilterImageTranslation::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    OutputImage = dynamic_cast<svlSampleImage*>(syncInput->GetNewInstance());
    OutputImage->CopyOf(syncInput);
    syncOutput = OutputImage;
    return SVL_OK;
}

int svlFilterImageTranslation::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = OutputImage;
    _SkipIfAlreadyProcessed(syncInput, OutputImage);

    svlSampleImage* id = dynamic_cast<svlSampleImage*>(syncInput);
    unsigned int videochannels = id->GetVideoChannels();
    unsigned int idx;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        // Processing
        Translate(id->GetUCharPointer(idx),
                  OutputImage->GetUCharPointer(idx),
                  id->GetWidth(idx) * id->GetBPP(),
                  id->GetHeight(idx),
                  HorizTranslation[idx] * id->GetBPP(),
                  VertTranslation[idx]);
    }

    return SVL_OK;
}

int svlFilterImageTranslation::SetTranslation(const int h_translation, const int v_translation, unsigned int videoch)
{
    if (videoch >= SVL_MAX_CHANNELS) return SVL_FAIL;
    HorizTranslation[videoch] = h_translation;
    VertTranslation[videoch] = v_translation;
    return SVL_OK;
}

int svlFilterImageTranslation::SetTranslationHoriz(const int translation, unsigned int videoch)
{
    if (videoch >= SVL_MAX_CHANNELS) return SVL_FAIL;
    HorizTranslation[videoch] = translation;
    return SVL_OK;
}

int svlFilterImageTranslation::SetTranslationVert(const int translation, unsigned int videoch)
{
    if (videoch >= SVL_MAX_CHANNELS) return SVL_FAIL;
    VertTranslation[videoch] = translation;
    return SVL_OK;
}

void svlFilterImageTranslation::Translate(unsigned char* src, unsigned char* dest, const int width, const int height, const int trhoriz, const int trvert)
{
    int abs_h = abs(trhoriz);
    int abs_v = abs(trvert);

    if (width <= abs_h || height <= abs_v) {
        memset(dest, 0, width * height);
        return;
    }

    if (trhoriz == 0) {
        memcpy(dest + std::max(0, trvert) * width,
               src + std::max(0, -trvert) * width,
               width * (height - abs_v));
        return;
    }

    int linecopysize = width - abs_h;
    int xfrom = std::max(0, trhoriz);
    int yfrom = std::max(0, trvert);
    int yto = height + std::min(0, trvert);
    int copyxoffset = std::max(0, -trhoriz);
    int copyyoffset = std::max(0, -trvert);

    src += width * copyyoffset + copyxoffset;
    dest += width * yfrom + xfrom;
    for (int j = yfrom; j < yto; j ++) {
        memcpy(dest, src, linecopysize);
        src += width;
        dest += width;
    }
}

void svlFilterImageTranslation::CreateInterfaces()
{
    // Add NON-QUEUED provided interface for configuration management
    mtsInterfaceProvided* provided = AddInterfaceProvided("Settings", MTS_COMMANDS_SHOULD_NOT_BE_QUEUED);
    if (provided) {
        provided->AddCommandWrite(&svlFilterImageTranslation::SetTranslationLCommand, this, "SetTranslation");
        provided->AddCommandWrite(&svlFilterImageTranslation::SetTranslationLCommand, this, "SetLeftTranslation");
        provided->AddCommandWrite(&svlFilterImageTranslation::SetTranslationRCommand, this, "SetRightTranslation");
        provided->AddCommandRead (&svlFilterImageTranslation::GetTranslationLCommand, this, "GetTranslation");
        provided->AddCommandRead (&svlFilterImageTranslation::GetTranslationLCommand, this, "GetLeftTranslation");
        provided->AddCommandRead (&svlFilterImageTranslation::GetTranslationRCommand, this, "GetRightTranslation");
    }
}

void svlFilterImageTranslation::SetTranslationLCommand(const vctInt2 & translation)
{
    HorizTranslation[SVL_LEFT] = translation[0];
    VertTranslation[SVL_LEFT] = translation[1];
}

void svlFilterImageTranslation::SetTranslationRCommand(const vctInt2 & translation)
{
    HorizTranslation[SVL_RIGHT] = translation[0];
    VertTranslation[SVL_RIGHT] = translation[1];
}

void svlFilterImageTranslation::GetTranslationLCommand(vctInt2 & translation) const
{
    translation[0] = HorizTranslation[SVL_LEFT];
    translation[1] = VertTranslation[SVL_LEFT];
}

void svlFilterImageTranslation::GetTranslationRCommand(vctInt2 & translation) const
{
    translation[0] = HorizTranslation[SVL_RIGHT];
    translation[1] = VertTranslation[SVL_RIGHT];
}

