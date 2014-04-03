/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2010

  (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlFilterImageZoom.h>


/***************************************/
/*** svlFilterImageZoom class **********/
/***************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterImageZoom, svlFilterBase)

svlFilterImageZoom::svlFilterImageZoom() :
    svlFilterBase(),
    OutputImage(0),
    InterpolationEnabled(true)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageMono8);
    AddInputType("input", svlTypeImageRGBStereo);
    AddInputType("input", svlTypeImageMono8Stereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);

    Center.SetAll(svlPoint2D(0, 0));
    Zoom.SetAll(1.0);
}

svlFilterImageZoom::~svlFilterImageZoom()
{
    Release();
}

int svlFilterImageZoom::SetZoom(const double zoom, unsigned int videoch)
{
    if (videoch >= Zoom.size()) return SVL_FAIL;
    Zoom[videoch] = std::max(0.05, zoom);
    return SVL_OK;
}

double svlFilterImageZoom::GetZoom(unsigned int videoch)
{
    if (videoch >= Zoom.size()) return -1.0;
    return Zoom[videoch];
}

int svlFilterImageZoom::SetCenter(int x, int y, unsigned int videoch)
{
    if (videoch >= Center.size()) return SVL_FAIL;
    Center[videoch].Assign(x, y);
    return SVL_OK;
}

int svlFilterImageZoom::OnChangeCenter(int x, int y, unsigned int videoch)
{
    return SetCenter(x, y, videoch);
}

int svlFilterImageZoom::GetCenter(int & x, int & y, unsigned int videoch)
{
    if (videoch >= Center.size()) return SVL_FAIL;
    x = Center[videoch].x;
    y = Center[videoch].y;
    return SVL_OK;
}

void svlFilterImageZoom::SetInterpolation(bool enable)
{
    InterpolationEnabled = enable;
}

int svlFilterImageZoom::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    Release();

    OutputImage = dynamic_cast<svlSampleImage*>(syncInput->GetNewInstance());
    OutputImage->SetSize(*syncInput); 

    syncOutput = OutputImage;

    return SVL_OK;
}

int svlFilterImageZoom::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = OutputImage;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);

    if (IsDisabled()) {
        syncOutput = syncInput;
        return SVL_OK;
    }

    svlSampleImage* input = dynamic_cast<svlSampleImage*>(syncInput);
    const unsigned int videochannels = input->GetVideoChannels();
    unsigned int idx;

    int width, height, in_width, in_height, lr, rr, tr, br;
    CvRect in_cvrect, out_cvrect;
    svlRect in_rect, out_rect;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        width   = static_cast<int>(input->GetWidth(idx));
        height  = static_cast<int>(input->GetHeight(idx));

        if (Zoom[idx] == 1.0) {
            if (Center[idx].x == width  / 2 &&
                Center[idx].y == height / 2) {
                memcpy(OutputImage->GetUCharPointer(idx), input->GetUCharPointer(idx), OutputImage->GetDataSize(idx));
                continue;
            }
            else {
                // TODO: implement image translation
            }
        }

        in_width   = static_cast<int>(width  / Zoom[idx]);
        in_height  = static_cast<int>(height / Zoom[idx]);

        memset(OutputImage->GetUCharPointer(idx), 0, OutputImage->GetDataSize(idx));

        out_rect.left   = width  / 2 - width  / 2;
        out_rect.top    = height / 2 - height / 2;
        out_rect.right  = out_rect.left + width;
        out_rect.bottom = out_rect.top  + height;

        lr = (width / 2 - out_rect.left) * 10000 / (width  / 2);
        tr = (height / 2 - out_rect.top) * 10000 / (height / 2);
        rr = (out_rect.right  - width  / 2) * 10000 / (width  / 2);
        br = (out_rect.bottom - height / 2) * 10000 / (height / 2);

        out_rect.Trim(0, width, 0, height);

        in_rect.left   = Center[idx].x - lr * in_width  / 20000;
        in_rect.right  = Center[idx].x + rr * in_width  / 20000;
        in_rect.top    = Center[idx].y - tr * in_height / 20000;
        in_rect.bottom = Center[idx].y + br * in_height / 20000;

        lr = Center[idx].x * 10000 / (Center[idx].x - in_rect.left);
        tr = Center[idx].y * 10000 / (Center[idx].y - in_rect.top);
        rr = (width  - Center[idx].x) * 10000 / (in_rect.right  - Center[idx].x);
        br = (height - Center[idx].y) * 10000 / (in_rect.bottom - Center[idx].y);
        if (lr > 10000) lr = 10000;
        if (tr > 10000) tr = 10000;
        if (tr > 10000) rr = 10000;
        if (br > 10000) br = 10000;

        in_rect.Trim(0, width, 0, height);
        if (in_rect.left >= width  || in_rect.right  < 0 ||
            in_rect.top  >= height || in_rect.bottom < 0) continue;

        out_rect.left   = width  / 2 - lr * (width  / 2 - out_rect.left)   / 10000;
        out_rect.right  = width  / 2 - rr * (width  / 2 - out_rect.right)  / 10000;
        out_rect.top    = height / 2 - tr * (height / 2 - out_rect.top)    / 10000;
        out_rect.bottom = height / 2 - br * (height / 2 - out_rect.bottom) / 10000;

        if (out_rect.left >= width  || out_rect.right  < 0 ||
            out_rect.top  >= height || out_rect.bottom < 0) continue;

        in_cvrect.x      = in_rect.left;
        in_cvrect.width  = in_rect.right - in_rect.left;
        in_cvrect.y      = in_rect.top;
        in_cvrect.height = in_rect.bottom - in_rect.top;

        out_cvrect.x      = out_rect.left;
        out_cvrect.width  = out_rect.right - out_rect.left;
        out_cvrect.y      = out_rect.top;
        out_cvrect.height = out_rect.bottom - out_rect.top;
        
        cvSetImageROI(input->IplImageRef(idx), in_cvrect);
        cvSetImageROI(OutputImage->IplImageRef(idx), out_cvrect);

        if (InterpolationEnabled) cvResize(input->IplImageRef(idx), OutputImage->IplImageRef(idx), CV_INTER_LINEAR);
        else cvResize(input->IplImageRef(idx), OutputImage->IplImageRef(idx), CV_INTER_NN);

        cvResetImageROI(input->IplImageRef(idx));
        cvResetImageROI(OutputImage->IplImageRef(idx));
    }

    return SVL_OK;
}

int svlFilterImageZoom::Release()
{
    if (OutputImage) {
        delete OutputImage;
        OutputImage = 0;
    }

    return SVL_OK;
}

