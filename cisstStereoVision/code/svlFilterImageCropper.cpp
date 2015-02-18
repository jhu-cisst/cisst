/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2007 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlFilterImageCropper.h>
#include <cisstStereoVision/svlImageProcessing.h>


/******************************************/
/*** svlFilterImageCropper class **********/
/******************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterImageCropper, svlFilterBase)

svlFilterImageCropper::svlFilterImageCropper() :
    svlFilterBase(),
    OutputImage(0)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBA);
    AddInputType("input", svlTypeImageMono8);
    AddInputType("input", svlTypeImageMono16);
    AddInputType("input", svlTypeImageMono32);
    AddInputType("input", svlTypeImageRGBStereo);
    AddInputType("input", svlTypeImageRGBAStereo);
    AddInputType("input", svlTypeImageMono8Stereo);
    AddInputType("input", svlTypeImageMono16Stereo);
    AddInputType("input", svlTypeImageMono32Stereo);
    AddInputType("input", svlTypeImage3DMap);

    AddOutput("output", true);
    SetAutomaticOutputType(true);

    Enabled.SetAll(false);
}

svlFilterImageCropper::~svlFilterImageCropper()
{
    Release();
}

int svlFilterImageCropper::SetRectangle(int left, int top, int right, int bottom, unsigned int videoch)
{
    return SetRectangle(svlRect(left, top, right, bottom), videoch);
}

int svlFilterImageCropper::SetRectangle(const svlRect & rect, unsigned int videoch)
{
    if (IsInitialized() || videoch >= Rectangles.size()) return SVL_FAIL;

    svlRect r(rect);
    r.Normalize();
    if ((r.right - r.left) < 1 || (r.right - r.left) > 4096 ||
        (r.bottom - r.top) < 1 || (r.bottom - r.top) > 4096) return SVL_FAIL;

    Rectangles[videoch].Assign(r);
    Enabled[videoch] = true;

    return SVL_OK;
}

int svlFilterImageCropper::SetCorner(int x, int y, unsigned int videoch)
{
    if (videoch >= Rectangles.size() || !Enabled[videoch]) return SVL_FAIL;
    Rectangles[videoch].right = x + Rectangles[videoch].right - Rectangles[videoch].left;
    Rectangles[videoch].bottom = y + Rectangles[videoch].bottom - Rectangles[videoch].top;
    Rectangles[videoch].left = x;
    Rectangles[videoch].top = y;
    return SVL_OK;
}

int svlFilterImageCropper::SetCenter(int x, int y, unsigned int videoch)
{
    if (videoch >= Rectangles.size() || !Enabled[videoch]) return SVL_FAIL;
    return SetCorner(x - (Rectangles[videoch].right - Rectangles[videoch].left) / 2,
                     y - (Rectangles[videoch].bottom - Rectangles[videoch].top) / 2,
                     videoch);
}

int svlFilterImageCropper::OnChangeCenter(int x, int y, unsigned int videoch)
{
    return SetCenter(x, y, videoch);
}

int svlFilterImageCropper::OnChangeCenterRect(const svlRect & rect, unsigned int videoch)
{
    if (IsInitialized()) {
        return SetCenter((rect.left + rect.right + 1) / 2, (rect.top + rect.bottom + 1) / 2, videoch);
    }
    else {
        return SetRectangle(rect, videoch);
    }
}

svlRect svlFilterImageCropper::GetRectangle(unsigned int videoch) {

    if (videoch >= Rectangles.size()) {
        CMN_LOG_CLASS_RUN_ERROR << "Requested channel does not exist"<<std::endl;
        return svlRect();
    }

    return Rectangles[videoch];
}


int svlFilterImageCropper::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    Release();

    OutputImage = dynamic_cast<svlSampleImage*>(syncInput->GetNewInstance());

    svlSampleImage* input = dynamic_cast<svlSampleImage*>(syncInput);

    for (unsigned int i = 0; i < input->GetVideoChannels(); i ++) {
        if (Enabled[i]) {
            OutputImage->SetSize(i, Rectangles[i].right - Rectangles[i].left, Rectangles[i].bottom - Rectangles[i].top); 
        }
        else {
            OutputImage->SetSize(i, input->GetWidth(i), input->GetHeight(i));
        }
    }

    syncOutput = OutputImage;

    return SVL_OK;
}

int svlFilterImageCropper::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = OutputImage;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);

    svlSampleImage* in_image = dynamic_cast<svlSampleImage*>(syncInput);
    unsigned int videochannels = in_image->GetVideoChannels();
    unsigned int idx;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        svlImageProcessing::Crop(in_image, idx, OutputImage, idx, Rectangles[idx].left, Rectangles[idx].top);
    }

    return SVL_OK;
}

int svlFilterImageCropper::Release()
{
    if (OutputImage) {
        delete OutputImage;
        OutputImage = 0;
    }

    return SVL_OK;
}

