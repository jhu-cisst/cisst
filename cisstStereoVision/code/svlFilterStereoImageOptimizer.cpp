/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
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

#include <cisstStereoVision/svlFilterStereoImageOptimizer.h>


/********************************************/
/*** svlFilterStereoImageOptimizer class ****/
/********************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterStereoImageOptimizer, svlFilterBase)

svlFilterStereoImageOptimizer::svlFilterStereoImageOptimizer() :
    svlFilterBase(),
    Disparity_Target(0),
    ROI_Target(0, 0, 0x7FFFFFFF, 0x7FFFFFFF),
    ColBal_Red(256),
    ColBal_Green(256),
    ColBal_Blue(256),
    RecomputeRatios(1)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGBStereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);
}

svlFilterStereoImageOptimizer::~svlFilterStereoImageOptimizer()
{
    Release();
}

int svlFilterStereoImageOptimizer::SetDisparity(int disparity)
{
    if (IsInitialized()) return SVL_FAIL;
    Disparity_Target = disparity;
    return SVL_OK;
}

int svlFilterStereoImageOptimizer::SetRegionOfInterest(const svlRect & roi)
{
    if (IsInitialized()) return SVL_FAIL;
    ROI_Target = roi;
    // make sure none of them are less than 0
    if (ROI_Target.left < 0) ROI_Target.left = 0;
    if (ROI_Target.right < 0) ROI_Target.right = 0;
    if (ROI_Target.top < 0) ROI_Target.top = 0;
    if (ROI_Target.bottom < 0) ROI_Target.bottom = 0;
    return SVL_OK;
}

int svlFilterStereoImageOptimizer::SetRegionOfInterest(unsigned int left, unsigned int top, unsigned int right, unsigned int bottom)
{
    if (IsInitialized()) return SVL_FAIL;
    ROI_Target.Assign(static_cast<int>(left), static_cast<int>(top), static_cast<int>(right), static_cast<int>(bottom));
    return SVL_OK;
}

int svlFilterStereoImageOptimizer::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    Release();

    svlSampleImageRGBStereo* stimg = dynamic_cast<svlSampleImageRGBStereo*>(syncInput);
    int w0 = stimg->GetWidth(SVL_LEFT);
    int w1 = stimg->GetWidth(SVL_RIGHT);
    int h0 = stimg->GetHeight(SVL_LEFT);
    int h1 = stimg->GetHeight(SVL_RIGHT);

    if (w0 != w1 || h0 != h1) return SVL_FAIL;

    // store left ROI size
    ROI[SVL_LEFT].left   = std::min(ROI_Target.left, ROI_Target.right);
    ROI[SVL_LEFT].right  = std::max(ROI_Target.left, ROI_Target.right);
    ROI[SVL_LEFT].top    = std::min(ROI_Target.top,  ROI_Target.bottom);
    ROI[SVL_LEFT].bottom = std::max(ROI_Target.top,  ROI_Target.bottom);

    // make sure it's not reaching out of the image boundaries
    if (ROI[SVL_LEFT].left   <   0) ROI[SVL_LEFT].left   = 0;
    if (ROI[SVL_LEFT].right  <   0) ROI[SVL_LEFT].right  = 0;
    if (ROI[SVL_LEFT].top    <   0) ROI[SVL_LEFT].top    = 0;
    if (ROI[SVL_LEFT].bottom <   0) ROI[SVL_LEFT].bottom = 0;
    if (ROI[SVL_LEFT].left   >= w0) ROI[SVL_LEFT].left   = w0 - 1;
    if (ROI[SVL_LEFT].right  >= w0) ROI[SVL_LEFT].right  = w0 - 1;
    if (ROI[SVL_LEFT].top    >= h0) ROI[SVL_LEFT].top    = h0 - 1;
    if (ROI[SVL_LEFT].bottom >= h0) ROI[SVL_LEFT].bottom = h0 - 1;

    // calculate right ROI size
    Disparity = Disparity_Target;
    ROI[SVL_RIGHT].left   = ROI[SVL_LEFT].left  + Disparity;
    ROI[SVL_RIGHT].right  = ROI[SVL_LEFT].right + Disparity;
    ROI[SVL_RIGHT].top    = ROI[SVL_LEFT].top;
    ROI[SVL_RIGHT].bottom = ROI[SVL_LEFT].bottom;

    // make sure it's not reaching out of the image boundaries
    if (ROI[SVL_RIGHT].left   <   0) ROI[SVL_RIGHT].left   = 0;
    if (ROI[SVL_RIGHT].right  <   0) ROI[SVL_RIGHT].right  = 0;
    if (ROI[SVL_RIGHT].left   >= w0) ROI[SVL_RIGHT].left   = w0 - 1;
    if (ROI[SVL_RIGHT].right  >= w0) ROI[SVL_RIGHT].right  = w0 - 1;

    syncOutput = syncInput;

    return SVL_OK;
}

int svlFilterStereoImageOptimizer::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);
    _SkipIfDisabled();

    unsigned int idx;
    svlSampleImage* img = dynamic_cast<svlSampleImage*>(syncInput);

    if (FrameCounter == 0 || RecomputeRatios > 0) {
        // Compute color balance between left and right image

        const unsigned int videochannels = img->GetVideoChannels();
        svlRGB *buffer, *pix;
        int roi_l, roi_r, roi_t, roi_b, r, g, b;
        int i, j, k, width;

        // Recomputing ratios when requested
        _ParallelLoop(procInfo, idx, videochannels)
        {
            width = img->GetWidth(idx);
            r = g = b = 0;

            // Computing left colors
            buffer = reinterpret_cast<svlRGB*>(img->GetUCharPointer(idx));
            roi_l = ROI[idx].left;
            roi_r = ROI[idx].right;
            roi_t = ROI[idx].top;
            roi_b = ROI[idx].bottom;
            for (j = roi_t; j < roi_b; j ++) {
                k = j * width + roi_l;
                for (i = roi_l; i < roi_r; i ++) {
                    pix = buffer + k;
                    r += pix->r;
                    g += pix->g;
                    b += pix->b;
                    k ++;
                }
            }
            k = (roi_r - roi_l) * (roi_b - roi_t);
            R[idx] = r / k;
            G[idx] = g / k;
            B[idx] = b / k;
        }

        _SynchronizeThreads(procInfo);

        _OnSingleThread(procInfo)
        {
            if (R[SVL_LEFT] > 0) ColBal_Red   = 256 * R[SVL_RIGHT] / R[SVL_LEFT];
            if (G[SVL_LEFT] > 0) ColBal_Green = 256 * G[SVL_RIGHT] / G[SVL_LEFT];
            if (B[SVL_LEFT] > 0) ColBal_Blue  = 256 * B[SVL_RIGHT] / B[SVL_LEFT];
            if (RecomputeRatios == 1) RecomputeRatios = 0;
        }

        _SynchronizeThreads(procInfo);
    }

    // Apply correction on the left image
    unsigned int pixelcount = img->GetWidth(SVL_LEFT) * img->GetHeight(SVL_LEFT);
    unsigned char *pImg = img->GetUCharPointer(SVL_LEFT);
    unsigned char *ptr;
    int k;

    _ParallelLoop(procInfo, idx, pixelcount)
    {
        ptr = pImg + idx * 3;

        k = *ptr * ColBal_Red >> 8;
        if (k > 255) k = 255;
        *ptr = k; ptr ++;

        k = *ptr * ColBal_Green >> 8;
        if (k > 255) k = 255;
        *ptr = k; ptr ++;

        k = *ptr * ColBal_Blue >> 8;
        if (k > 255) k = 255;
        *ptr = k; ptr ++;
    }

    return SVL_OK;
}

int svlFilterStereoImageOptimizer::Release()
{
    ColBal_Red = ColBal_Green = ColBal_Blue = 256;
    return SVL_OK;
}

void svlFilterStereoImageOptimizer::RecomputeColorBalance(bool always)
{
    if (always) RecomputeRatios = 2;
    else RecomputeRatios = 1;
}

