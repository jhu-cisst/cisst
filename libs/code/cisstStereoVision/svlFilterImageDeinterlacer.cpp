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

#include <cisstStereoVision/svlFilterImageDeinterlacer.h>


/******************************************/
/*** svlFilterImageDeinterlacer class *****/
/******************************************/

CMN_IMPLEMENT_SERVICES(svlFilterImageDeinterlacer)

svlFilterImageDeinterlacer::svlFilterImageDeinterlacer() :
    svlFilterBase()
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBStereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);

    Algorithm = svlImageProcessing::DI_Blending;
}

int svlFilterImageDeinterlacer::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    return SVL_OK;
}

int svlFilterImageDeinterlacer::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);
    _SkipIfDisabled();

    svlSampleImage* img = dynamic_cast<svlSampleImage*>(syncInput);
    unsigned int videochannels = img->GetVideoChannels();
    unsigned int idx;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        svlImageProcessing::Deinterlace(img, idx, Algorithm);
    }

    return SVL_OK;
}

void svlFilterImageDeinterlacer::SetAlgorithm(svlImageProcessing::DI_Algorithm algorithm)
{
    Algorithm = algorithm;
}

svlImageProcessing::DI_Algorithm svlFilterImageDeinterlacer::GetAlgorithm()
{
    return Algorithm;
}

