/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlDeinterlacer.cpp,v 1.8 2008/10/22 14:51:58 vagvoba Exp $
  
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

#include <cisstStereoVision/svlDeinterlacer.h>
#include "ipDeinterlacing.h"

using namespace std;

/*************************************/
/*** svlDeinterlacer class ***********/
/*************************************/

svlDeinterlacer::svlDeinterlacer() : svlFilterBase()
{
    AddSupportedType(svlTypeImageRGB, svlTypeImageRGB);
    AddSupportedType(svlTypeImageRGBStereo, svlTypeImageRGBStereo);
    Method = svlDeinterlacingNone;
}

svlDeinterlacer::~svlDeinterlacer()
{
    Release();
}

int svlDeinterlacer::Initialize(svlSample* inputdata)
{
    Release();

    OutputData = inputdata;

    return SVL_OK;
}

int svlDeinterlacer::ProcessFrame(ProcInfo* procInfo, svlSample* inputdata)
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
        // Processing the input image directly
        switch (Method) {
            case svlDeinterlacingNone:
                // NOP
            break;

            case svlDeinterlacingBlending:
                Blending(reinterpret_cast<unsigned char*>(img->GetPointer(idx)),
                         static_cast<int>(img->GetWidth(idx)), static_cast<int>(img->GetHeight(idx)));
            break;

            case svlDeinterlacingDiscarding:
                Discarding(reinterpret_cast<unsigned char*>(img->GetPointer(idx)),
                           static_cast<int>(img->GetWidth(idx)), static_cast<int>(img->GetHeight(idx)));
            break;

            case svlDeinterlacingAdaptiveBlending:
                AdaptiveBlending(reinterpret_cast<unsigned char*>(img->GetPointer(idx)),
                                 static_cast<int>(img->GetWidth(idx)), static_cast<int>(img->GetHeight(idx)));
            break;

            case svlDeinterlacingAdaptiveDiscarding:
                AdaptiveDiscarding(reinterpret_cast<unsigned char*>(img->GetPointer(idx)),
                                   static_cast<int>(img->GetWidth(idx)), static_cast<int>(img->GetHeight(idx)));
            break;
        }
    }

    return SVL_OK;
}

int svlDeinterlacer::Release()
{
    return SVL_OK;
}

