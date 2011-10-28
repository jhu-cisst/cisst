/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
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


/*****************************************/
/*** svlFilterImageCenterFinder class ****/
/*****************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterImageCenterFinder, svlFilterBase)

svlFilterImageCenterFinder::svlFilterImageCenterFinder() :
    svlFilterBase(),
    Smoothing(0.0),
    ThresholdLevel(10),
    MassRatio(50),
    MaskEnabled(false),
    LinkHorizontally(false),
    LinkVertically(true)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBStereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);
}

int svlFilterImageCenterFinder::GetCenter(int &x, int &y, unsigned int videoch) const
{
    if (!IsRunning()) return SVL_FAIL;
    if (videoch >= CenterX.size()) return SVL_FAIL;

    x = CenterX[videoch];
    y = CenterY[videoch];

    return SVL_OK;
}

int svlFilterImageCenterFinder::GetRadius(int &x, int &y, unsigned int videoch) const
{
    if (!IsRunning()) return SVL_FAIL;
    if (videoch >= RadiusX.size()) return SVL_FAIL;

    x = RadiusX[videoch];
    y = RadiusY[videoch];

    return SVL_OK;
}

void svlFilterImageCenterFinder::SetTrajectorySmoothing(double smoothing)
{
    Smoothing = smoothing;
}

void svlFilterImageCenterFinder::SetThreshold(unsigned char thresholdlevel)
{
    ThresholdLevel = thresholdlevel;
}

void svlFilterImageCenterFinder::SetMassRatio(unsigned int ratio)
{
    if (ratio > 100) ratio = 100;
    MassRatio = ratio;
}

void svlFilterImageCenterFinder::SetMask(bool enablemask)
{
    MaskEnabled = enablemask;
}

void svlFilterImageCenterFinder::SetHorizontalLink(bool enable)
{
    LinkHorizontally = enable;
}

void svlFilterImageCenterFinder::SetVerticalLink(bool enable)
{
    LinkVertically = enable;
}

double svlFilterImageCenterFinder::GetTrajectorySmoothing() const
{
    return Smoothing;
}

unsigned char svlFilterImageCenterFinder::GetThreshold() const
{
    return static_cast<unsigned char>(ThresholdLevel);
}

unsigned int svlFilterImageCenterFinder::GetMassRatio() const
{
    return MassRatio;
}

bool svlFilterImageCenterFinder::GetMask() const
{
    return MaskEnabled;
}

bool svlFilterImageCenterFinder::GetHorizontalLink() const
{
    return LinkHorizontally;
}

bool svlFilterImageCenterFinder::GetVerticalLink() const
{
    return LinkVertically;
}

void svlFilterImageCenterFinder::AddReceiver(svlFilterImageCenterFinderInterface* receiver)
{
    if (!receiver) return;

    unsigned int size = static_cast<unsigned int>(Receivers.size());
    Receivers.resize(size + 1);
    Receivers[size] = receiver;
}

int svlFilterImageCenterFinder::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    svlSampleImage* image  = dynamic_cast<svlSampleImage*>(syncInput);
    unsigned int i, videochannels = image->GetVideoChannels();
    unsigned int size;

    ProjectionV.SetSize(videochannels);
    ProjectionH.SetSize(videochannels);
    CenterXInternal.SetSize(videochannels);
    CenterYInternal.SetSize(videochannels);
    RadiusXInternal.SetSize(videochannels);
    RadiusYInternal.SetSize(videochannels);
    CenterX.SetSize(videochannels);
    CenterY.SetSize(videochannels);
    RadiusX.SetSize(videochannels);
    RadiusY.SetSize(videochannels);

    for (i = 0; i < videochannels; i ++) {
        size = image->GetWidth(i);
        ProjectionV[i].SetSize(size);
        RadiusXInternal[i] = CenterXInternal[i] = RadiusX[i] = CenterX[i] = static_cast<int>(size) / 2;

        size = image->GetHeight(i);
        ProjectionH[i].SetSize(size);
        RadiusYInternal[i] = CenterYInternal[i] = RadiusY[i] = CenterY[i] = static_cast<int>(size) / 2;
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
    const unsigned int videochannels = image->GetVideoChannels();
    unsigned int a, i, j, x, y, rx, ry, width, height, vch, hsum, vsum, pix, stride;
    unsigned int *h, *h2, *v, *v2;
    unsigned char *img;
    const unsigned int thrsh = ThresholdLevel * 3;

    _ParallelLoop(procInfo, vch, videochannels)
    {
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
            hsum = 0;

            for (i = 0; i < width; i ++) {
                pix  = *img; img ++;
                pix += *img; img ++;
                pix += *img; img ++;

                if (pix >= thrsh) {
                    hsum += 255;
                    *v2 += 255;
                }
                else if (MaskEnabled) {
                    img[-3] = 0;
                    img[-2] = 0;
                    img[-1] = 0;
                }

                v2 ++;
            }

            *h = hsum / width; h ++;
        }
        for (i = 0; i < width; i ++) {
            *v /= height; v ++;
        }


        // Find center of weight
        v = ProjectionV[vch].Pointer();
        h = ProjectionH[vch].Pointer();

        x = vsum = 0;
        for (i = 0; i < width; i ++) {
            a = *v; v ++; vsum += a;
            x += a * i;
        }
        if (vsum != 0) x /= vsum;
        else x = CenterXInternal[vch];

        y = hsum = 0;
        for (j = 0; j < height; j ++) {
            a = *h; h ++; hsum += a;
            y += a * j;
        }
        if (hsum != 0) y /= hsum;
        else y = CenterYInternal[vch];


        // Find radii
        v = v2 = ProjectionV[vch].Pointer() + x;
        h = h2 = ProjectionH[vch].Pointer() + y;

        // Set threshold to 60% of the whole mass
        hsum = hsum * MassRatio / 100;
        vsum = vsum * MassRatio / 100;

        a = rx = 0;
        while (a < vsum) {
            a += *v + *v2;
            v ++; v2 --;
            rx ++;
        }

        a = ry = 0;
        while (a < hsum) {
            a += *h + *h2;
            h ++; h2 --;
            ry ++;
        }


        // Smoothing results
        if (FrameCounter > 0 && Smoothing > 0.0) {
            x  = static_cast<int>((Smoothing * CenterXInternal[vch] + x ) / (1.0 + Smoothing));
            y  = static_cast<int>((Smoothing * CenterYInternal[vch] + y ) / (1.0 + Smoothing));
            rx = static_cast<int>((Smoothing * RadiusXInternal[vch] + rx) / (1.0 + Smoothing));
            ry = static_cast<int>((Smoothing * RadiusYInternal[vch] + ry) / (1.0 + Smoothing));
        }

        // Storing results
        CenterXInternal[vch] = x;
        CenterYInternal[vch] = y;
        RadiusXInternal[vch] = rx;
        RadiusYInternal[vch] = ry;
    }

    _SynchronizeThreads(procInfo);

    _OnSingleThread(procInfo)
    {
        x = y = rx = ry = 0;

        if (LinkHorizontally) {
            for (vch = 0; vch < videochannels; vch ++) {
                x  += CenterXInternal[vch];
                rx += RadiusXInternal[vch];
            }
        }
        if (LinkVertically) {
            for (vch = 0; vch < videochannels; vch ++) {
                y  += CenterYInternal[vch];
                ry += RadiusYInternal[vch];
            }
        }

        for (vch = 0; vch < videochannels; vch ++) {
            if (LinkHorizontally) {
                CenterX[vch] = x  / videochannels;
                RadiusX[vch] = rx / videochannels;
            }
            else {
                CenterX[vch] = CenterXInternal[vch];
                RadiusX[vch] = RadiusXInternal[vch];
            }
            if (LinkVertically) {
                CenterY[vch] = y  / videochannels;
                RadiusY[vch] = ry / videochannels;
            }
            else {
                CenterY[vch] = CenterYInternal[vch];
                RadiusY[vch] = RadiusYInternal[vch];
            }
        }

        for (i = 0; i < Receivers.size(); i ++) {
            if (Receivers[i]) {
                for (vch = 0; vch < videochannels; vch ++) {
                    Receivers[i]->SetCenter(CenterX[vch], CenterY[vch], RadiusX[vch], RadiusY[vch], vch);
                }
            }
        }
    }

    return SVL_OK;
}

