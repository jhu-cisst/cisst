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

#include <cisstStereoVision/svlFilterImageBorder.h>


/**********************************/
/*** svlFilterImageBorder class ***/
/**********************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterImageBorder, svlFilterBase)

svlFilterImageBorder::svlFilterImageBorder() :
    svlFilterBase(),
    OutputImage(0)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBStereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);
}

int svlFilterImageBorder::SetBorders(unsigned int left, unsigned int top, unsigned int right, unsigned int bottom, unsigned int videoch)
{
    if (IsInitialized()) return SVL_FAIL;

    if (Borders.size() <= videoch) Borders.resize(videoch + 1);

    Borders[videoch][0] = left;
    Borders[videoch][1] = top;
    Borders[videoch][2] = right;
    Borders[videoch][3] = bottom;

    return SVL_OK;
}

int svlFilterImageBorder::GetBorders(unsigned int& left, unsigned int& top, unsigned int& right, unsigned int& bottom, unsigned int videoch)
{
    if (Borders.size() <= videoch) return SVL_FAIL;

    left   = Borders[videoch][0];
    top    = Borders[videoch][1];
    right  = Borders[videoch][2];
    bottom = Borders[videoch][3];

    return SVL_OK;
}

int svlFilterImageBorder::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    Release();

    svlSampleImage* inimg = dynamic_cast<svlSampleImage*>(syncInput);
    OutputImage = dynamic_cast<svlSampleImage*>(syncInput->GetNewInstance());
    for (unsigned int vch = 0; vch < OutputImage->GetVideoChannels(); vch ++) {
        if (Borders.size() > vch) {
            OutputImage->SetSize(vch,
                                 inimg->GetWidth(vch)  + Borders[vch][0] + Borders[vch][2],
                                 inimg->GetHeight(vch) + Borders[vch][1] + Borders[vch][3]);
        }
        else {
            OutputImage->SetSize(vch, inimg->GetWidth(vch), inimg->GetHeight(vch));
        }
    }

    syncOutput = OutputImage;
    return SVL_OK;
}

int svlFilterImageBorder::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = OutputImage;
    
    svlSampleImage* inimg = dynamic_cast<svlSampleImage*>(syncInput);
    const unsigned int videochannels = inimg->GetVideoChannels();
    int i, ws, hs, wo, ho, xs, ys, xo, yo, copylen, linecount;
    unsigned char *bgdata, *ovrldata;
    const int bpp = inimg->GetBPP();
    unsigned int idx;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        // Prepare for data copy
        ws = static_cast<int>(OutputImage->GetWidth(idx) * bpp);
        hs = static_cast<int>(OutputImage->GetHeight(idx));
        wo = static_cast<int>(inimg->GetWidth(idx) * bpp);
        ho = static_cast<int>(inimg->GetHeight(idx));

        copylen = wo;
        linecount = ho;
        if (Borders.size() > idx) {
            xs = Borders[idx][0] * bpp;
            ys = Borders[idx][1];
        }
        else {
            xs = ys = 0;
        }
        xo = yo = 0;

        // If overlay position reaches out of the background on the left
        if (xs < 0) {
            copylen += xs;
            xo -= xs;
            xs = 0;
        }
        // If overlay position reaches out of the background on the right
        if ((xs + copylen) > ws) {
            copylen += ws - (xs + copylen);
        }
        // If overlay is outside the background boundaries
        if (copylen <= 0) continue;

        // If overlay position reaches out of the background on the top
        if (ys < 0) {
            linecount += ys;
            yo -= ys;
            ys = 0;
        }
        // If overlay position reaches out of the background on the bottom
        if ((ys + linecount) > hs) {
            linecount += hs - (ys + linecount);
        }
        // If overlay is outside the background boundaries
        if (linecount <= 0) continue;

        bgdata = OutputImage->GetUCharPointer(idx) + (ys * ws) + xs;
        ovrldata = inimg->GetUCharPointer(idx) + (yo * wo) + xo;

        for (i = 0; i < linecount; i ++) {
            memcpy(bgdata, ovrldata, copylen);
            bgdata += ws;
            ovrldata += wo;
        }
    }

    return SVL_OK;
}

int svlFilterImageBorder::Release()
{
    delete OutputImage;
    OutputImage = 0;

    return SVL_OK;
}

