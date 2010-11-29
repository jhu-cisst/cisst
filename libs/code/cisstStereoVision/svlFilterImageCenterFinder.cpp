/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $
  
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

#include <cisstStereoVision/svlFilterImageCenterFinder.h>
#include <cisstStereoVision/svlFilterImageCropper.h>
#if CISST_SVL_HAS_OPENCV
    #include <cisstStereoVision/svlFilterImageZoom.h>
#endif // CISST_SVL_HAS_OPENCV


/*****************************************/
/*** svlFilterImageCenterFinder class ****/
/*****************************************/

CMN_IMPLEMENT_SERVICES(svlFilterImageCenterFinder)

svlFilterImageCenterFinder::svlFilterImageCenterFinder() :
    svlFilterBase(),
    Smoothing(0.0),
    MaskEnabled(false),
    ThresholdLevel(10)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBStereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);
}

int svlFilterImageCenterFinder::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    svlSampleImage* image  = dynamic_cast<svlSampleImage*>(syncInput);
    unsigned int i, videochannels = image->GetVideoChannels();
    unsigned int size;

    ProjectionV.SetSize(videochannels);
    ProjectionH.SetSize(videochannels);
    CenterX.SetSize(videochannels);
    CenterY.SetSize(videochannels);

    for (i = 0; i < videochannels; i ++) {
        size = image->GetWidth(i);
        ProjectionV[i].SetSize(size);
        CenterX[i] = static_cast<int>(size) / 2;

        size = image->GetHeight(i);
        ProjectionH[i].SetSize(size);
        CenterY[i] = static_cast<int>(size) / 2;
    }

    syncOutput = syncInput;

    return SVL_OK;
}

int svlFilterImageCenterFinder::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);
    _SkipIfDisabled();

    svlSampleImage* image  = dynamic_cast<svlSampleImage*>(syncInput);
    unsigned int i, j, width, height, vch, videochannels = image->GetVideoChannels();
    unsigned int *h, *v, *v2, sum, pix, stride, x, y, a;
    unsigned char *img;
    const unsigned int thrsh = ThresholdLevel * 3;

    _ParallelLoop(procInfo, vch, videochannels) {
        width = image->GetWidth(vch);
        height = image->GetHeight(vch);
        stride = width * 3 - 2;

        img = image->GetUCharPointer(vch);
        v = ProjectionV[vch].Pointer();
        h = ProjectionH[vch].Pointer();

        memset(v, 0, sizeof(unsigned int) * width);

        // Generate projections
        for (j = 0; j < height; j ++) {
            v2 = v;
            sum = 0;

            for (i = 0; i < width; i ++) {
                pix  = *img; img ++;
                pix += *img; img ++;
                pix += *img; img ++;

                if (pix >= thrsh) {
                    sum += 255;
                    *v2 += 255;
                }
                else if (MaskEnabled) {
                    img[-3] = 0;
                    img[-2] = 0;
                    img[-1] = 0;
                }

                v2 ++;
            }

            *h = sum / width; h ++;
        }

        h = ProjectionH[vch].Pointer();

        // Find center of weight
        x = sum = 0;
        for (i = 0; i < width; i ++) {
            a = *v / height; v ++; sum += a;
            x += a * i;
        }
        if (sum != 0) x /= sum;
        else x = CenterX[vch];

        y = sum = 0;
        for (j = 0; j < height; j ++) {
            a = *h; h ++; sum += a;
            y += a * j;
        }
        if (sum != 0) y /= sum;
        else y = CenterY[vch];

        if (FrameCounter > 0 && Smoothing > 0.0) {
            x = static_cast<int>((Smoothing * CenterX[vch] + x) / (1.0 + Smoothing));
            y = static_cast<int>((Smoothing * CenterY[vch] + y) / (1.0 + Smoothing));
        }

        CenterX[vch] = x;
        CenterY[vch] = y;
    }

    _SynchronizeThreads(procInfo);

    _OnSingleThread(procInfo) {

        x = y = 0;
        for (vch = 0; vch < videochannels; vch ++) {
            x += CenterX[vch];
            y += CenterY[vch];
        }
        x /= videochannels;
        y /= videochannels;

        for (i = 0; i < Receivers.size(); i ++) {
            if (Receivers[i]) {
                for (vch = 0; vch < videochannels; vch ++) Receivers[i]->SetCenter(x, y, vch);
            }
        }
    }

    return SVL_OK;
}

void svlFilterImageCenterFinder::AddReceiver(svlFilterImageCenterFinderInterface* receiver)
{
    if (!receiver) return;

    unsigned int size = Receivers.size();
    Receivers.resize(size + 1);
    Receivers[size] = receiver;
}

void svlFilterImageCenterFinder::SetTrajectorySmoothing(double smoothing)
{
    Smoothing = smoothing;
}

double svlFilterImageCenterFinder::GetTrajectorySmoothing()
{
    return Smoothing;
}

void svlFilterImageCenterFinder::SetMask(bool enablemask)
{
    MaskEnabled = enablemask;
}

bool svlFilterImageCenterFinder::GetMask()
{
    return MaskEnabled;
}

void svlFilterImageCenterFinder::SetThreshold(unsigned char thresholdlevel)
{
    ThresholdLevel = thresholdlevel;
}

unsigned char svlFilterImageCenterFinder::GetThreshold()
{
    return static_cast<unsigned char>(ThresholdLevel);
}

int svlFilterImageCenterFinder::GetCenter(int &x, int &y, unsigned int videoch)
{
    if (!IsRunning()) return SVL_FAIL;
    if (videoch >= CenterX.size()) return SVL_FAIL;

    x = CenterX[videoch];
    y = CenterY[videoch];

    return SVL_OK;
}

