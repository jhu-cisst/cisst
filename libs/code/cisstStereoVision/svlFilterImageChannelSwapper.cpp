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

#include <cisstStereoVision/svlFilterImageChannelSwapper.h>


/******************************************/
/*** svlFilterImageChannelSwapper class ***/
/******************************************/

CMN_IMPLEMENT_SERVICES(svlFilterImageChannelSwapper)

svlFilterImageChannelSwapper::svlFilterImageChannelSwapper() :
    svlFilterBase()
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBStereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);
}

int svlFilterImageChannelSwapper::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    return SVL_OK;
}

int svlFilterImageChannelSwapper::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);
    _SkipIfDisabled();

    svlSampleImage* img = dynamic_cast<svlSampleImage*>(syncInput);
    unsigned int i, videochannels = img->GetVideoChannels();
    unsigned int pixelcount;
    unsigned char colval, *r, *b;
    unsigned int idx;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        pixelcount = img->GetWidth(idx) * img->GetHeight(idx);
        r = img->GetUCharPointer(idx);
        b = r + 2;

        for (i = 0; i < pixelcount; i ++) {
            colval = *r;
            *r = *b;
            *b = colval;
            r += 3;
            b += 3;
        }
    }

    return SVL_OK;
}

