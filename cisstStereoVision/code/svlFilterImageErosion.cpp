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

#include <cisstStereoVision/svlFilterImageErosion.h>
#include <cisstStereoVision/svlImageProcessing.h>


/**************************************/
/*** svlFilterImageErosion class *****/
/**************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterImageErosion, svlFilterBase)

svlFilterImageErosion::svlFilterImageErosion() :
    svlFilterBase(),
    OutputImage(0),
    TempImage(0),
    Iterations(1)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageMono8);
    AddInputType("input", svlTypeImageMono8Stereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);
}

svlFilterImageErosion::~svlFilterImageErosion()
{
    Release();
}

void svlFilterImageErosion::SetIterations(unsigned int iterations)
{
    Iterations = iterations;
}

unsigned int svlFilterImageErosion::GetIterations() const
{
    return Iterations;
}

int svlFilterImageErosion::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    Release();

    OutputImage = dynamic_cast<svlSampleImage*>(syncInput->GetNewInstance());
    OutputImage->SetSize(syncInput);

    TempImage = dynamic_cast<svlSampleImage*>(syncInput->GetNewInstance());
    TempImage->SetSize(syncInput);

    syncOutput = OutputImage;

    return SVL_OK;
}

int svlFilterImageErosion::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = OutputImage;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);
    _SkipIfDisabled();

    svlSampleImage* img = dynamic_cast<svlSampleImage*>(syncInput);
    unsigned int videochannels = img->GetVideoChannels();
    unsigned int idx, iter;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        iter = Iterations;
        if (iter) {
            if (iter & 1) {
                svlImageProcessing::Erode(img, idx, OutputImage, idx, 1);
                iter --;
            }
            else {
                svlImageProcessing::Erode(img, idx, TempImage, idx, 1);
                svlImageProcessing::Erode(TempImage, idx, OutputImage, idx, 1);
                iter -= 2;
            }
            while (iter) {
                svlImageProcessing::Erode(OutputImage, idx, TempImage, idx, 1);
                svlImageProcessing::Erode(TempImage, idx, OutputImage, idx, 1);
                iter -= 2;
            }
        }
    }

    return SVL_OK;
}

int svlFilterImageErosion::Release()
{
    if (OutputImage) {
        delete OutputImage;
        OutputImage = 0;
    }

    if (TempImage) {
        delete TempImage;
        TempImage = 0;
    }

    return SVL_OK;
}

