/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
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

#include <cisstStereoVision/svlFilterImageChannelSwapper.h>


/******************************************/
/*** svlFilterImageChannelSwapper class ***/
/******************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterImageChannelSwapper, svlFilterBase)

svlFilterImageChannelSwapper::svlFilterImageChannelSwapper() :
    svlFilterBase(),
    OutputImage(0)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBStereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);
}

int svlFilterImageChannelSwapper::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    Release();

    OutputImage = dynamic_cast<svlSampleImage*>(syncInput->GetNewInstance());
    if (!OutputImage) return SVL_FAIL;
    OutputImage->SetSize(syncInput);
    syncOutput = OutputImage;

    return SVL_OK;
}

int svlFilterImageChannelSwapper::Release()
{
    if (OutputImage) {
        delete OutputImage;
        OutputImage = 0;
    }
    return SVL_OK;
}

int svlFilterImageChannelSwapper::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = OutputImage;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);

    svlSampleImage* in_img = dynamic_cast<svlSampleImage*>(syncInput);
    unsigned int i, videochannels = in_img->GetVideoChannels();
    unsigned int pixelcount;
    unsigned char r, g, b, *in_ptr, *out_ptr;
    unsigned int idx;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        pixelcount = in_img->GetWidth(idx) * in_img->GetHeight(idx);
        in_ptr  = in_img->GetUCharPointer(idx);
        out_ptr = OutputImage->GetUCharPointer(idx);

        if (IsDisabled()) {
            memcpy(out_ptr, in_ptr, pixelcount * 3);
            continue;
        }

        for (i = 0; i < pixelcount; i ++) {
            r = *in_ptr; in_ptr ++;
            g = *in_ptr; in_ptr ++;
            b = *in_ptr; in_ptr ++;
            *out_ptr = b; out_ptr ++;
            *out_ptr = g; out_ptr ++;
            *out_ptr = r; out_ptr ++;
        }
    }

    return SVL_OK;
}

