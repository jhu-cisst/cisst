/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2006 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlFilterImageRectifier.h>
#include "svlImageProcessingHelper.h"


/******************************************/
/*** svlFilterImageRectifier class ********/
/******************************************/

CMN_IMPLEMENT_SERVICES(svlFilterImageRectifier)

svlFilterImageRectifier::svlFilterImageRectifier() :
    svlFilterBase(),
    OutputImage(0),
    InterpolationEnabled(true)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBStereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);
}

svlFilterImageRectifier::~svlFilterImageRectifier()
{
    if (OutputImage) delete OutputImage;
}

int svlFilterImageRectifier::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    svlSampleImage* input = dynamic_cast<svlSampleImage*>(syncInput);

    svlImageProcessingHelper::RectificationInternals* table;
    unsigned int i, channels;

    channels = std::min(SVL_MAX_CHANNELS, input->GetVideoChannels());
    for (i = 0; i < channels; i ++) {
        table = dynamic_cast<svlImageProcessingHelper::RectificationInternals*>(Tables[i].Get());
        if (table) {
            if (table->Width != input->GetWidth(i) ||
                table->Height != input->GetHeight(i))
                return SVL_FAIL;
        }
    }

    // Preparing output sample
    if (OutputImage) delete OutputImage;
    OutputImage = dynamic_cast<svlSampleImage*>(input->GetNewInstance());
    OutputImage->SetSize(*input);

    channels = OutputImage->GetVideoChannels();
    for (i = 0; i < channels; i ++) {
        memset(OutputImage->GetUCharPointer(i), 0, OutputImage->GetDataSize(i));
    }

    syncOutput = OutputImage;

    return SVL_OK;
}

int svlFilterImageRectifier::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = OutputImage;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);

    svlSampleImage* id = dynamic_cast<svlSampleImage*>(syncInput);
    unsigned int videochannels = id->GetVideoChannels();
    svlImageProcessingHelper::RectificationInternals* table;
    unsigned int idx;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        // Processing
        table = dynamic_cast<svlImageProcessingHelper::RectificationInternals*>(Tables[idx].Get());
        if (table) {
            svlImageProcessing::Rectify(id, idx, OutputImage, idx, InterpolationEnabled, Tables[idx]);
        }
        else {
            memcpy(OutputImage->GetUCharPointer(idx), id->GetUCharPointer(idx), id->GetDataSize(idx));
        }
    }

    return SVL_OK;
}

int svlFilterImageRectifier::LoadTable(const std::string &filepath, unsigned int videoch, int exponentlen)
{
    if (IsInitialized() == true) return SVL_ALREADY_INITIALIZED;
    if (videoch >= SVL_MAX_CHANNELS) return SVL_FAIL;

    svlImageProcessingHelper::RectificationInternals* table = new svlImageProcessingHelper::RectificationInternals;
    if (!table->Load(filepath, exponentlen)) {
        delete table;
        return SVL_FAIL;
    }

    Tables[videoch].Set(table);

    return SVL_OK;
}

void svlFilterImageRectifier::EnableInterpolation(bool enable)
{
    InterpolationEnabled = enable;
}
