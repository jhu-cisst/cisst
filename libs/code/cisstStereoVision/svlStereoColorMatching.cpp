/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlStereoColorMatching.cpp,v 1.8 2008/10/22 14:51:58 vagvoba Exp $
  
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

#include <cisstStereoVision/svlStereoColorMatching.h>

using namespace std;

/*************************************/
/*** svlStereoColorMatching class ***********/
/*************************************/

svlStereoColorMatching::svlStereoColorMatching() : svlFilterBase()
{
    AddSupportedType(svlTypeImageRGBStereo, svlTypeImageRGBStereo);

    ROI_Target.Assign(0, 0, 0x7FFFFFFF, 0x7FFFFFFF);
    ColBal_Red = ColBal_Green = ColBal_Blue = 256;
    RecomputeRatios = 1;
}

svlStereoColorMatching::~svlStereoColorMatching()
{
    Release();
}

int svlStereoColorMatching::SetRegionOfInterest(svlRect roi)
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

int svlStereoColorMatching::SetRegionOfInterest(unsigned int left, unsigned int top, unsigned int right, unsigned int bottom)
{
    if (IsInitialized()) return SVL_FAIL;
    ROI_Target.Assign(static_cast<int>(left), static_cast<int>(top), static_cast<int>(right), static_cast<int>(bottom));
    return SVL_OK;
}

int svlStereoColorMatching::Initialize(svlSample* inputdata)
{
    Release();

    svlSampleImageRGBStereo* stimg = dynamic_cast<svlSampleImageRGBStereo*>(inputdata);
    int w0 = stimg->GetWidth(SVL_LEFT);
    int w1 = stimg->GetWidth(SVL_RIGHT);
    int h0 = stimg->GetHeight(SVL_LEFT);
    int h1 = stimg->GetHeight(SVL_RIGHT);

    if (w0 != w1 || h0 != h1) return SVL_FAIL;

    ROI_Actual = ROI_Target;
    // make sure it's not larger than the image
    if (ROI_Actual.left >= w0) ROI_Actual.left = w0 - 1;
    if (ROI_Actual.right >= w0) ROI_Actual.right = w0 - 1;
    if (ROI_Actual.top >= h0) ROI_Actual.top = h0 - 1;
    if (ROI_Actual.bottom >= h0) ROI_Actual.bottom = h0 - 1;

    OutputData = inputdata;

    return SVL_OK;
}

int svlStereoColorMatching::ProcessFrame(ProcInfo* procInfo, svlSample* inputdata)
{
    ///////////////////////////////////////////
    // Check if the input sample has changed //
      if (!IsNewSample(inputdata))
          return SVL_ALREADY_PROCESSED;
    ///////////////////////////////////////////

    // Passing the same image for the next filter
    OutputData = inputdata;

    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(inputdata);

    if (FrameCounter == 0 || RecomputeRatios > 0) {
        // Compute color balance between left and right image

        _OnSingleThread(procInfo)
        {
            // Recomputing ratios when requested

            svlRGB *leftimg, *rightimg, *leftpix, *rightpix;
            int roi_l, roi_r, roi_t, roi_b;
            int l_r, l_g, l_b, r_r, r_g, r_b;
            int i, j, k, width;

            width = img->GetWidth(SVL_LEFT);
            roi_l = ROI_Actual.left;
            roi_r = ROI_Actual.right;
            roi_t = ROI_Actual.top;
            roi_b = ROI_Actual.bottom;

            l_r = l_g = l_b = r_r = r_g = r_b = 0;

            leftimg = reinterpret_cast<svlRGB*>(img->GetPointer(SVL_LEFT));
            rightimg = reinterpret_cast<svlRGB*>(img->GetPointer(SVL_RIGHT));

            for (j = roi_t; j < roi_b; j ++) {
                k = j * width + roi_l;
                for (i = roi_l; i < roi_r; i ++) {
                    leftpix = leftimg + k;
                    rightpix = rightimg + k;

                    l_r += leftpix->R;
                    l_g += leftpix->G;
                    l_b += leftpix->B;
                    r_r += rightpix->R;
                    r_g += rightpix->G;
                    r_b += rightpix->B;

                    k ++;
                }
            }

            k = (roi_r - roi_l) * (roi_b - roi_t);
            l_r /= k; l_g /= k; l_b /= k;
            r_r /= k; r_g /= k; r_b /= k;

            if (l_r > 0) ColBal_Red = 256 * r_r / l_r;
            if (l_g > 0) ColBal_Green = 256 * r_g / l_g;
            if (l_b > 0) ColBal_Blue = 256 * r_b / l_b;

            if (RecomputeRatios == 1) RecomputeRatios = 0;
        }

        _SynchronizeThreads(procInfo);
    }

    // Apply correction on the left image
    unsigned int pixelcount = img->GetWidth(SVL_LEFT) * img->GetHeight(SVL_LEFT);
    unsigned char *pImg = reinterpret_cast<unsigned char*>(img->GetPointer(SVL_LEFT));
    unsigned char *ptr;
    unsigned int idx;
    int k;

    _ParallelInterleavedLoop(procInfo, idx, pixelcount)
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

int svlStereoColorMatching::Release()
{
    ColBal_Red = ColBal_Green = ColBal_Blue = 256;
    return SVL_OK;
}

void svlStereoColorMatching::RecomputeColorBalance(bool always)
{
    if (always) RecomputeRatios = 2;
    else RecomputeRatios = 1;
}

