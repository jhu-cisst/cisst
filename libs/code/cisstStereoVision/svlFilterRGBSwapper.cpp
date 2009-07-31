/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlRGBSwapper.cpp 75 2009-02-24 16:47:20Z adeguet1 $
  
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

#include <cisstStereoVision/svlFilterRGBSwapper.h>

using namespace std;

/*************************************/
/*** svlFilterRGBSwapper class *******/
/*************************************/

svlFilterRGBSwapper::svlFilterRGBSwapper() : svlFilterBase()
{
    AddSupportedType(svlTypeImageRGB, svlTypeImageRGB);
    AddSupportedType(svlTypeImageRGBStereo, svlTypeImageRGBStereo);
}

svlFilterRGBSwapper::~svlFilterRGBSwapper()
{
    Release();
}

int svlFilterRGBSwapper::Initialize(svlSample* inputdata)
{
    Release();

    OutputData = inputdata;

    return SVL_OK;
}

int svlFilterRGBSwapper::ProcessFrame(ProcInfo* procInfo, svlSample* inputdata)
{
    ///////////////////////////////////////////
    // Check if the input sample has changed //
      if (!IsNewSample(inputdata))
          return SVL_ALREADY_PROCESSED;
    ///////////////////////////////////////////

    // Passing the same image for the next filter
    OutputData = inputdata;

    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(inputdata);
    unsigned int videochannels = img->GetVideoChannels();
    unsigned int idx;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        SwapChannels(reinterpret_cast<unsigned char*>(img->GetPointer(idx)), img->GetWidth(idx) * img->GetHeight(idx));
    }

    return SVL_OK;
}

int svlFilterRGBSwapper::Release()
{
    return SVL_OK;
}

void svlFilterRGBSwapper::SwapChannels(unsigned char* buffer, unsigned int pixelcount)
{
    unsigned char colval;
    unsigned char* r = buffer;
    unsigned char* b = r + 2;
    for (unsigned int i = 0; i < pixelcount; i ++) {
       colval = *r;
       *r = *b;
       *b = colval;
       r += 3;
       b += 3;
    }
}

