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

#include <cisstStereoVision/svlFilterCUDATest.h>


/*******************************/
/*** svlFilterCUDATest class ***/
/*******************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterCUDATest, svlFilterBase)

svlFilterCUDATest::svlFilterCUDATest() :
    svlFilterBase(),
    OutputImage(0)
{
    AddInput("input", true);
    AddInputType("input", svlTypeCUDAImageRGB);
    AddInputType("input", svlTypeCUDAImageRGBStereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);
}

int svlFilterCUDATest::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    Release();

    OutputImage = dynamic_cast<svlSampleCUDAImage*>(syncInput->GetNewInstance());
    OutputImage->SetSize(syncInput);
    syncOutput = OutputImage;

    return SVL_OK;
}

int svlFilterCUDATest::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);
    _SkipIfDisabled();

    _OnSingleThread(procInfo)
    {
        svlSampleCUDAImage* inimg = dynamic_cast<svlSampleCUDAImage*>(syncInput);
        unsigned int videochannels = inimg->GetVideoChannels();

        for (unsigned int i = 0; i < videochannels; i ++) {
            OutputImage->CopyOf(inimg);
            // TO DO: call some CUDA routines
            // inimg->GetCUDAPointer(i);
            // OutputImage->GetCUDAPointer(i);
        }
    }

    return SVL_OK;
}

int svlFilterCUDATest::Release()
{
    if (OutputImage) {
        delete OutputImage;
        OutputImage = 0;
    }
    return SVL_OK;
}

