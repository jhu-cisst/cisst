/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlComputationalStereo.cpp 545 2009-07-14 08:38:33Z adeguet1 $
  
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

#include <cisstStereoVision/svlFilterComputationalStereo.h>
#include <string.h>

#include "svlStereoDP.h"
#include "svlStereoDPMono.h"

using namespace std;

/*******************************************/
/*** svlFilterComputationalStereo class ****/
/*******************************************/

svlFilterComputationalStereo::svlFilterComputationalStereo() : svlFilterBase()
{
    AddSupportedType(svlTypeImageMono8Stereo, svlTypeDepthMap);
    AddSupportedType(svlTypeImageMono16Stereo, svlTypeDepthMap);
    AddSupportedType(svlTypeImageRGBStereo, svlTypeDepthMap);

    svlSampleDepthMap* depth = new svlSampleDepthMap;

    OutputData = depth;

    StereoAlgorithm = 0;
    XCheckStereoAlgorithm = 0;
    XCheckImage = 0;

    XCheckEnabled = false;
    StereoMethod = svlComputationalStereoMethodDP;
    DisparityInterpolationEnabled = false;
    DisparityOutputEnabled = false;
    FocalLength = 600.0f;
    Baseline = 10.0f;
    PPX_Left_flt = 0.0f;
    PPX_Right_flt = 0.0f;
    PPY_flt = 0.0f;
    MinDisparity = 0;
    MaxDisparity = 64;
    ScaleFactor = 2;
    BlockSize = 3;
    NarrowedSearchRadius = 10;
    Smoothness = 5;
    TemporalFilter = 0.0;
    SpatialFilterRadius = 0;
    ValidAreaLeft = 0;
    ValidAreaRight = 0;
    ValidAreaTop = 0;
    ValidAreaBottom = 0;
}

svlFilterComputationalStereo::~svlFilterComputationalStereo()
{
    Release();

    if (OutputData) delete OutputData;
}

int svlFilterComputationalStereo::Initialize(svlSample* inputdata)
{
    svlStreamType inputtype = GetInputType();
    svlSampleImageBase* stimg = dynamic_cast<svlSampleImageBase*>(inputdata);

    Release();

    // Mono input is supported only by Dynamic Programming (so far)
    if (inputtype != svlTypeImageRGBStereo && StereoMethod != svlComputationalStereoMethodDP) return SVL_FAIL;

    int w1, w2, h1, h2;
    w1 = stimg->GetWidth(SVL_LEFT);
    h1 = stimg->GetHeight(SVL_LEFT);
    w2 = stimg->GetWidth(SVL_RIGHT);
    h2 = stimg->GetHeight(SVL_RIGHT);

    if (w1 <= 0 || h1 <= 0 || w1 != w2 || h1 != h2)
        return SVL_STEREO_INPUT_MISMATCH;

    // Check and fix valid area
    int itemp;
    if (ValidAreaLeft > ValidAreaRight) {
        itemp = ValidAreaRight;
        ValidAreaRight = ValidAreaLeft;
        ValidAreaLeft = itemp;
    }
    if (ValidAreaLeft < 0) ValidAreaLeft = 0;
    if (ValidAreaLeft >= w1) ValidAreaLeft = w1 - 1;
    if (ValidAreaRight < 0) ValidAreaRight = 0;
    if (ValidAreaRight >= w1) ValidAreaRight = w1 - 1;
    if (ValidAreaTop > ValidAreaBottom) {
        itemp = ValidAreaBottom;
        ValidAreaBottom = ValidAreaTop;
        ValidAreaTop = itemp;
    }
    if (ValidAreaTop < 0) ValidAreaTop = 0;
    if (ValidAreaTop >= h1) ValidAreaTop = h1 - 1;
    if (ValidAreaBottom < 0) ValidAreaBottom = 0;
    if (ValidAreaBottom >= h1) ValidAreaBottom = h1 - 1;

    // Misc initializations
    PPX_Left = static_cast<int>(PPX_Left_flt);
    PPX_Right = static_cast<int>(PPX_Right_flt);
    PPY = static_cast<int>(PPY_flt);

    svlSampleDepthMap* depth = dynamic_cast<svlSampleDepthMap*>(OutputData);
    depth->SetSize(w1, h1);
    depth->MatrixRef().SetAll(0);

    DisparityBuffer.SetSize(h1, w1);
    SpatialFilterBuffer.SetSize(h1, w1);

    // Creating computational stereo object(s)
    switch (StereoMethod) {
        case svlComputationalStereoMethodDP:
            if (inputtype == svlTypeImageRGBStereo) { // Color input
                StereoAlgorithm = new svlStereoDP(w1, h1,
                                                  ValidAreaLeft,
                                                  ValidAreaTop,
                                                  ValidAreaRight,
                                                  ValidAreaBottom,
                                                  MinDisparity,
                                                  MaxDisparity,
                                                  PPX_Right - PPX_Left,
                                                  ScaleFactor,
                                                  BlockSize,
                                                  NarrowedSearchRadius,
                                                  Smoothness,
                                                  static_cast<int>(TemporalFilter),
                                                  DisparityInterpolationEnabled);
            }
            else { // Mono input
                StereoAlgorithm = new svlStereoDPMono(w1, h1,
                                                      ValidAreaLeft,
                                                      ValidAreaTop,
                                                      ValidAreaRight,
                                                      ValidAreaBottom,
                                                      MinDisparity,
                                                      MaxDisparity,
                                                      PPX_Right - PPX_Left,
                                                      ScaleFactor,
                                                      BlockSize,
                                                      NarrowedSearchRadius,
                                                      Smoothness,
                                                      TemporalFilter,
                                                      DisparityInterpolationEnabled);
            }
        break;

        default:
        break;
    }

    if (StereoAlgorithm == 0 || StereoAlgorithm->Initialize() != 0) {
        Release();
        return SVL_STEREO_INIT_ERROR;
    }

    if (XCheckEnabled) {
        // allocate cress check image sample
        if (inputtype == svlTypeImageRGBStereo) XCheckImage = new svlSampleImageRGBStereo;
        else if (inputtype == svlTypeImageMono8Stereo) XCheckImage = new svlSampleImageMono8Stereo;
        else XCheckImage = new svlSampleImageMono16Stereo;
        XCheckImage->SetSize(w1, h1);
        // allocate cross check disparity buffer
        XCheckDisparityBuffer.SetSize(h1, w1);

        switch (StereoMethod) {
            case svlComputationalStereoMethodDP:
                if (inputtype == svlTypeImageRGBStereo) { // Color input
                    XCheckStereoAlgorithm = new svlStereoDP(w1, h1,
                                                            w1 - ValidAreaRight,
                                                            h1 - ValidAreaBottom,
                                                            w1 - ValidAreaLeft,
                                                            h1 - ValidAreaTop,
                                                            MinDisparity,
                                                            MaxDisparity,
                                                            PPX_Right - PPX_Left,
                                                            ScaleFactor,
                                                            BlockSize,
                                                            NarrowedSearchRadius,
                                                            Smoothness,
                                                            static_cast<int>(TemporalFilter),
                                                            DisparityInterpolationEnabled);
                }
                else { // Mono input
                    XCheckStereoAlgorithm = new svlStereoDPMono(w1, h1,
                                                                w1 - ValidAreaRight,
                                                                h1 - ValidAreaBottom,
                                                                w1 - ValidAreaLeft,
                                                                h1 - ValidAreaTop,
                                                                MinDisparity,
                                                                MaxDisparity,
                                                                PPX_Right - PPX_Left,
                                                                ScaleFactor,
                                                                BlockSize,
                                                                NarrowedSearchRadius,
                                                                Smoothness,
                                                                TemporalFilter,
                                                                DisparityInterpolationEnabled);
                }
            break;

            default:
            break;
        }

        if (XCheckStereoAlgorithm == 0 || XCheckStereoAlgorithm->Initialize() != 0) {
            Release();
            return SVL_STEREO_INIT_ERROR;
        }
    }

    return SVL_OK;
}

int svlFilterComputationalStereo::ProcessFrame(ProcInfo* procInfo, svlSample* inputdata)
{
    ///////////////////////////////////////////
    // Check if the input sample has changed //
      if (!IsNewSample(inputdata))
          return SVL_ALREADY_PROCESSED;
    ///////////////////////////////////////////

    svlSampleImageBase* stimg = dynamic_cast<svlSampleImageBase*>(inputdata);

    // Process data
    if (procInfo->count == 1 || procInfo->id == 1) {
        if (XCheckEnabled) {
            svlStreamType inputtype = GetInputType();

            // Process data
            if (inputtype == svlTypeImageRGBStereo) {
                CreateXCheckImageColor(reinterpret_cast<unsigned char*>(stimg->GetPointer(SVL_LEFT)),
                                       reinterpret_cast<unsigned char*>(XCheckImage->GetPointer(SVL_RIGHT)),
                                       stimg->GetWidth(SVL_LEFT),
                                       stimg->GetHeight(SVL_LEFT));
                CreateXCheckImageColor(reinterpret_cast<unsigned char*>(stimg->GetPointer(SVL_RIGHT)),
                                       reinterpret_cast<unsigned char*>(XCheckImage->GetPointer(SVL_LEFT)),
                                       stimg->GetWidth(SVL_RIGHT),
                                       stimg->GetHeight(SVL_RIGHT));
            }
            else if (inputtype == svlTypeImageMono8Stereo) {
                CreateXCheckImageMono<unsigned char>(reinterpret_cast<unsigned char*>(stimg->GetPointer(SVL_LEFT)),
                                                     reinterpret_cast<unsigned char*>(XCheckImage->GetPointer(SVL_RIGHT)),
                                                     stimg->GetWidth(SVL_LEFT),
                                                     stimg->GetHeight(SVL_LEFT));
                CreateXCheckImageMono<unsigned char>(reinterpret_cast<unsigned char*>(stimg->GetPointer(SVL_RIGHT)),
                                                     reinterpret_cast<unsigned char*>(XCheckImage->GetPointer(SVL_LEFT)),
                                                     stimg->GetWidth(SVL_RIGHT),
                                                     stimg->GetHeight(SVL_RIGHT));
            }
            else if (inputtype == svlTypeImageMono16Stereo) {
                CreateXCheckImageMono<unsigned short>(reinterpret_cast<unsigned short*>(stimg->GetPointer(SVL_LEFT)),
                                                      reinterpret_cast<unsigned short*>(XCheckImage->GetPointer(SVL_RIGHT)),
                                                      stimg->GetWidth(SVL_LEFT),
                                                      stimg->GetHeight(SVL_LEFT));
                CreateXCheckImageMono<unsigned short>(reinterpret_cast<unsigned short*>(stimg->GetPointer(SVL_RIGHT)),
                                                      reinterpret_cast<unsigned short*>(XCheckImage->GetPointer(SVL_LEFT)),
                                                      stimg->GetWidth(SVL_RIGHT),
                                                      stimg->GetHeight(SVL_RIGHT));
            }
            else return SVL_FAIL;

            // Stereo: computing disparity map
            XCheckStereoAlgorithm->Process(XCheckImage, XCheckDisparityBuffer.Pointer());

            if (procInfo->id == 1) {
                _SynchronizeThreads(procInfo);
                return SVL_OK;
            }
        }
    }

    if (procInfo->id == 0) {

        // Stereo: computing disparity map
        StereoAlgorithm->Process(stimg, DisparityBuffer.Pointer());

        if (XCheckEnabled) {
            _SynchronizeThreads(procInfo);

            // Compare results with the cross checked results and update final disparity map
            PerformXCheck();
        }

        // Convert disparity map to depth map
        svlSampleDepthMap* depth = dynamic_cast<svlSampleDepthMap*>(OutputData);
        ConvertDisparityToDistance(DisparityBuffer.Pointer(),
                                   reinterpret_cast<float*>(depth->GetPointer()),
                                   static_cast<int>(depth->GetWidth()),
                                   static_cast<int>(depth->GetHeight()));

        // Apply spatial filter on depth map if enabled
        if (SpatialFilterRadius > 0) ApplySpatialFilter(SpatialFilterRadius,
                                                        reinterpret_cast<float*>(depth->GetPointer(0, ValidAreaLeft, ValidAreaTop)),
                                                        SpatialFilterBuffer.Pointer(ValidAreaTop, ValidAreaLeft),
                                                        ValidAreaRight - ValidAreaLeft,
                                                        ValidAreaBottom - ValidAreaTop,
                                                        static_cast<int>(depth->GetWidth()));
    }

    return SVL_OK;
}

int svlFilterComputationalStereo::Release()
{
    if (StereoAlgorithm) {
        delete StereoAlgorithm;
        StereoAlgorithm = 0;
    }
    if (XCheckStereoAlgorithm) {
        delete XCheckStereoAlgorithm;
        XCheckStereoAlgorithm = 0;
    }
    if (XCheckImage) {
        delete XCheckImage;
        XCheckImage = 0;
    }
    DisparityBuffer.SetSize(0, 0);
    UnitSurfaceVectors.SetSize(0, 0);
    SurfaceImageMap.SetSize(0);
    SpatialFilterBuffer.SetSize(0, 0);
    XCheckDisparityBuffer.SetSize(0, 0);

    return SVL_OK;
}

int svlFilterComputationalStereo::SetCrossCheck(bool enabled)
{
    if (IsInitialized()) return SVL_FAIL;
    XCheckEnabled = enabled;
    return SVL_OK;
}

bool svlFilterComputationalStereo::GetCrossCheck()
{
    return XCheckEnabled;
}

void svlFilterComputationalStereo::SetMethod(svlComputationalStereoMethod method)
{
    StereoMethod = method;
}

svlComputationalStereoMethod svlFilterComputationalStereo::GetMethod()
{
    return StereoMethod;
}

int svlFilterComputationalStereo::SetFocalLength(double focallength)
{
    if (focallength < 1.0) return SVL_FAIL;
    FocalLength = static_cast<float>(focallength);
    return SVL_OK;
}

double svlFilterComputationalStereo::GetFocalLength()
{
    return static_cast<double>(FocalLength);
}

int svlFilterComputationalStereo::SetStereoBaseline(double baseline)
{
    if (baseline <= 0.0001) return SVL_FAIL;
    Baseline = static_cast<float>(baseline);
    return SVL_OK;
}

double svlFilterComputationalStereo::GetStereoBaseline()
{
    return static_cast<double>(Baseline);
}

void svlFilterComputationalStereo::SetPrincipalPoints(double ppx_left, double ppx_right, double ppy)
{
    PPX_Left_flt = static_cast<float>(ppx_left);
    PPX_Right_flt = static_cast<float>(ppx_right);
    PPY_flt = static_cast<float>(ppy);
}

void svlFilterComputationalStereo::GetPrincipalPoints(double &ppx_left, double &ppx_right, double &ppy)
{
    ppx_left = static_cast<double>(PPX_Left_flt);
    ppx_right = static_cast<double>(PPX_Right_flt);
    ppy = static_cast<double>(PPY_flt);
}

void svlFilterComputationalStereo::SetDisparityRange(unsigned int mindisparity, unsigned int maxdisparity)
{
    MinDisparity = mindisparity;
    MaxDisparity = maxdisparity;
}

void svlFilterComputationalStereo::GetDisparityRange(unsigned int& mindisparity, unsigned int& maxdisparity)
{
    mindisparity = MinDisparity;
    maxdisparity = MaxDisparity;
}

void svlFilterComputationalStereo::SetBlockSize(unsigned int blocksize)
{
    BlockSize = blocksize;
}

unsigned int svlFilterComputationalStereo::GetBlockSize()
{
    return BlockSize;
}

void svlFilterComputationalStereo::SetScalingFactor(unsigned int scalefactor)
{
    ScaleFactor = scalefactor;
}

unsigned int svlFilterComputationalStereo::GetScalingFactor()
{
    return ScaleFactor;
}

void svlFilterComputationalStereo::SetQuickSearchRadius(unsigned int searchradius)
{
    NarrowedSearchRadius = searchradius;
}

unsigned int svlFilterComputationalStereo::GetQuickSearchRadius()
{
    return NarrowedSearchRadius;
}

void svlFilterComputationalStereo::SetSmoothnessFactor(unsigned int smoothness)
{
    Smoothness = smoothness;
}

unsigned int svlFilterComputationalStereo::GetSmoothnessFactor()
{
    return Smoothness;
}

void svlFilterComputationalStereo::SetTemporalFiltering(double tempfilt)
{
    TemporalFilter = tempfilt;
}

double svlFilterComputationalStereo::GetTemporalFiltering()
{
    return TemporalFilter;
}

void svlFilterComputationalStereo::SetSpatialFiltering(unsigned int radius)
{
    SpatialFilterRadius = radius;
}

unsigned int svlFilterComputationalStereo::GetSpatialFiltering()
{
    return SpatialFilterRadius;
}

void svlFilterComputationalStereo::SetValidRect(int left, int top, int right, int bottom)
{
    ValidAreaLeft = left;
    ValidAreaTop = top;
    ValidAreaRight = right;
    ValidAreaBottom = bottom;
}
 
void svlFilterComputationalStereo::CreateXCheckImageColor(unsigned char* source, unsigned char* target, const unsigned int width, const unsigned int height)
{
    unsigned int i, j;
    unsigned char* dst = target + width * height * 3 - 3;
    for (j = 0; j < height; j ++) {
        for (i = 0; i < width; i ++) {
            *dst = *source; source ++; dst ++;
            *dst = *source; source ++; dst ++;
            *dst = *source; source ++; dst -= 5;
        }
    }
}

void svlFilterComputationalStereo::PerformXCheck()
{
    const int width = DisparityBuffer.width() - 1;
    const int height = DisparityBuffer.height() - 1;
    int i, j, k, r, l, from, to, dispmin, disp, prevdisp;

    // find occlusions and inconsistencies
    if (DisparityInterpolationEnabled) {
        for (j = 1; j < height; j ++) {
            for (i = 0; i <= width; i ++) {
                r = DisparityBuffer.Element(j, i);
                l = XCheckDisparityBuffer.Element(height - j, width - (i + ((r + 2) >> 2)));
                if (abs(r - l) > 4) DisparityBuffer.Element(j, i) = 0x7FFFFFFF;
            }
        }
    }
    else {
        for (j = 1; j < height; j ++) {
            for (i = 0; i <= width; i ++) {
                r = DisparityBuffer.Element(j, i);
                l = XCheckDisparityBuffer.Element(height - j, width - (i + r));
                if (abs(r - l) > 1) DisparityBuffer.Element(j, i) = 0x7FFFFFFF;
            }
        }
    }
    // fill holes
    for (j = ValidAreaTop - 1; j <= ValidAreaBottom; j ++) {
        from = 0x7FFFFFFF;
        to = -1;
        prevdisp = 0x7FFFFFFF;
        dispmin = 0x7FFFFFFF;
        for (i = ValidAreaLeft - 1; i <= ValidAreaRight; i ++) {
            disp = DisparityBuffer.Element(j, i);
            if (disp == 0x7FFFFFFF) {
                if (from == 0x7FFFFFFF) {
                    from = i;
                    dispmin = prevdisp;
                }
                to = i;
            }
            else {
                if (from != 0x7FFFFFFF) {
                    // selecting the lowest disparity from the two sides of the hole
                    if (dispmin > disp) dispmin = disp;

                    // fill the hole
                    for (k = from; k <= to; k ++) {
                        DisparityBuffer.Element(j, k) = dispmin;
                    }

                    // reset
                    from = 0x7FFFFFFF;
                }
                prevdisp = disp;
            }
        }
        if (from != 0x7FFFFFFF) {
            if (dispmin != 0x7FFFFFFF) {
                // fill the hole
                for (k = from; k <= to; k ++) {
                    DisparityBuffer.Element(j, k) = dispmin;
                }
            }
        }
    }
}

void svlFilterComputationalStereo::ConvertDisparityToDistance(int* disparitymap, float* depthmap, const int mapwidth, const int mapheight)
{
    int i, j, disp;
    float dist, udd, maxdist;
    const float mltplr = 0.25f;

    if (DisparityOutputEnabled) {
        // store disparity in depth buffer
        if (DisparityInterpolationEnabled) {
            for (j = 0; j < mapheight; j ++) {
                for (i = 0; i < mapwidth; i ++) {
                    *depthmap = mltplr * (*disparitymap);
                    disparitymap ++;
                    depthmap ++;
                }
            }
        }
        else {
            for (j = 0; j < mapheight; j ++) {
                for (i = 0; i < mapwidth; i ++) {
                    *depthmap = static_cast<float>(*disparitymap);
                    disparitymap ++;
                    depthmap ++;
                }
            }
        }
    }
    else {
        udd = FocalLength * Baseline;
        if (DisparityInterpolationEnabled) udd /= mltplr;
        maxdist = 100000.0f;

        for (j = 0; j < mapheight; j ++) {
            for (i = 0; i < mapwidth; i ++) {
                // convert disparity to distance
                disp = *disparitymap;
                if (disp > 0) {
                    dist = udd / static_cast<float>(disp);
                    *depthmap = dist;
                }
                else {
                    *depthmap = maxdist;
                }

                disparitymap ++;
                depthmap ++;
            }
        }
    }
}

void svlFilterComputationalStereo::ApplySpatialFilter(const int radius,
                                                float* depthmap, float* tempbuffer,
                                                const int mapwidth, const int mapheight, const int linestride)
{
    int i, j, k, l, divider;
    int xstart, xend, ystart, yend;
    float sum;
    float *input, *output;

    for (j = 0; j < mapheight; j ++) {

        sum = 0.0f;
        divider = 0;

        // Initializing region of processing
        xstart = 0;
        xend = radius;
        ystart = j - radius;
        if (ystart < 0) ystart = 0;
        yend = j + radius;
        if (yend >= mapheight) yend = mapheight - 1;

        // Computing initial sum
        for (l = ystart; l <= yend; l ++) {
            input = depthmap + l * linestride + xstart;
            for (k = xstart; k <= xend; k ++) {
                sum += *input;
                input ++;
                divider ++;
            }
        }

        // Setting value of first column
        output = tempbuffer + j * linestride;
        *output = sum / static_cast<float>(divider);
        output ++;

        // Proceeding on the rest of the line
        for (i = 1; i < mapwidth; i ++) {

            // Subtracting previous column
            xstart = i - radius - 1;
            if (xstart >= 0) {
                input = depthmap + ystart * linestride + xstart;
                for (l = ystart; l <= yend; l ++) {
                    sum -= *input;
                    input += linestride;
                    divider --;
                }
            }

            // Adding next column
            xend = i + radius;
            if (xend < mapwidth) {
                input = depthmap + ystart * linestride + xend;
                for (l = ystart; l <= yend; l ++) {
                    sum += *input;
                    input += linestride;
                    divider ++;
                }
            }

            // Setting value
            *output = sum / static_cast<float>(divider);
            output ++;
        }
    }

    // copy temp buffer back to depth map
    input = tempbuffer;
    output = depthmap;
    for (j = 0; j < mapheight; j ++) {
        memcpy(output, input, mapwidth * sizeof(float));
        input += linestride;
        output += linestride;
    }
}

