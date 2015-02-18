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

#include <cisstStereoVision/svlFilterComputationalStereo.h>
#include <cisstStereoVision/svlFilterInput.h>
#include <cisstStereoVision/svlFilterOutput.h>

#include "svlStereoDP.h"
#include "svlStereoDPMono.h"


/*******************************************/
/*** svlFilterComputationalStereo class ****/
/*******************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterComputationalStereo, svlFilterBase)


svlFilterComputationalStereo::svlFilterComputationalStereo() :
    svlFilterBase(),
    StereoAlgorithm(0),
    XCheckStereoAlgorithm(0),
    XCheckImage(0),
    ROI(0, 0, 0, 0),
    MinDisparity(0),
    MaxDisparity(64),
    ScaleFactor(2),
    BlockSize(3),
    NarrowedSearchRadius(10),
    Smoothness(5),
    TemporalFilter(0.0),
    SpatialFilterRadius(0),
    SubpixelPrecision(false),
    XCheckEnabled(false),
    Method(DynamicProgramming)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageMono8Stereo);
    AddInputType("input", svlTypeImageMono16Stereo);
    AddInputType("input", svlTypeImageRGBStereo);

    AddOutput("output", true);
    SetAutomaticOutputType(false);
    GetOutput()->SetType(svlTypeMatrixFloat);

    OutputMatrix = new svlSampleMatrixFloat;
}

svlFilterComputationalStereo::~svlFilterComputationalStereo()
{
    Release();

    if (OutputMatrix) delete OutputMatrix;
}

int svlFilterComputationalStereo::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = OutputMatrix;

    svlStreamType inputtype = GetInput()->GetType();
    svlSampleImage* stimg = dynamic_cast<svlSampleImage*>(syncInput);

    Release();

    int w1, w2, h1, h2;
    w1 = stimg->GetWidth(SVL_LEFT);
    h1 = stimg->GetHeight(SVL_LEFT);
    w2 = stimg->GetWidth(SVL_RIGHT);
    h2 = stimg->GetHeight(SVL_RIGHT);

    if (w1 <= 0 || h1 <= 0 || w1 != w2 || h1 != h2)
        return SVL_STEREO_INPUT_MISMATCH;

    // allocate buffers
    OutputMatrix->SetSize(w1, h1);
    memset(OutputMatrix->GetUCharPointer(), 0, OutputMatrix->GetDataSize());
    DisparityBuffer.SetSize(h1, w1);
    SpatialFilterBuffer.SetSize(h1, w1);

    // normalize and trim ROI
    ROI.Normalize();
    ROI.Trim(0, w1 - 1, 0, h1 - 1);

    // Creating computational stereo object(s)
    switch (Method) {
        case DynamicProgramming:
            if (inputtype == svlTypeImageRGBStereo) { // Color input
                StereoAlgorithm = new svlStereoDP(w1, h1,
                                                  ROI,
                                                  MinDisparity,
                                                  MaxDisparity,
                                                  static_cast<int>(Geometry.GetIntrinsics(SVL_RIGHT).cc[0] -
                                                                   Geometry.GetIntrinsics(SVL_LEFT ).cc[0]),
                                                  ScaleFactor,
                                                  BlockSize,
                                                  NarrowedSearchRadius,
                                                  Smoothness,
                                                  TemporalFilter,
                                                  SubpixelPrecision);
            }
            else { // Mono input
                StereoAlgorithm = new svlStereoDPMono(w1, h1,
                                                      ROI,
                                                      MinDisparity,
                                                      MaxDisparity,
                                                      static_cast<int>(Geometry.GetIntrinsics(SVL_RIGHT).cc[0] -
                                                                       Geometry.GetIntrinsics(SVL_LEFT ).cc[0]),
                                                      ScaleFactor,
                                                      BlockSize,
                                                      NarrowedSearchRadius,
                                                      Smoothness,
                                                      TemporalFilter,
                                                      SubpixelPrecision);
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
        // calculate reverse ROI
        svlRect xroi(w1 - ROI.right, h1 - ROI.bottom, w1 - ROI.left, h1 - ROI.top);

        switch (Method) {
            case DynamicProgramming:
                if (inputtype == svlTypeImageRGBStereo) { // Color input
                    XCheckStereoAlgorithm = new svlStereoDP(w1, h1,
                                                            xroi,
                                                            MinDisparity,
                                                            MaxDisparity,
                                                            static_cast<int>(Geometry.GetIntrinsics(SVL_RIGHT).cc[0] -
                                                                             Geometry.GetIntrinsics(SVL_LEFT ).cc[0]),
                                                            ScaleFactor,
                                                            BlockSize,
                                                            NarrowedSearchRadius,
                                                            Smoothness,
                                                            TemporalFilter,
                                                            SubpixelPrecision);
                }
                else { // Mono input
                    XCheckStereoAlgorithm = new svlStereoDPMono(w1, h1,
                                                                xroi,
                                                                MinDisparity,
                                                                MaxDisparity,
                                                                static_cast<int>(Geometry.GetIntrinsics(SVL_RIGHT).cc[0] -
                                                                                 Geometry.GetIntrinsics(SVL_LEFT ).cc[0]),
                                                                ScaleFactor,
                                                                BlockSize,
                                                                NarrowedSearchRadius,
                                                                Smoothness,
                                                                TemporalFilter,
                                                                SubpixelPrecision);
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

int svlFilterComputationalStereo::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = OutputMatrix;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);

    svlSampleImage* stimg = dynamic_cast<svlSampleImage*>(syncInput);

    // Process data
    if (procInfo->count == 1 || procInfo->ID == 1) {
        if (XCheckEnabled) {
            svlStreamType inputtype = GetInput()->GetType();

            // Process data
            if (inputtype == svlTypeImageRGBStereo) {
                CreateXCheckImageColor(stimg->GetUCharPointer(SVL_LEFT),
                                       XCheckImage->GetUCharPointer(SVL_RIGHT),
                                       stimg->GetWidth(SVL_LEFT),
                                       stimg->GetHeight(SVL_LEFT));
                CreateXCheckImageColor(stimg->GetUCharPointer(SVL_RIGHT),
                                       XCheckImage->GetUCharPointer(SVL_LEFT),
                                       stimg->GetWidth(SVL_RIGHT),
                                       stimg->GetHeight(SVL_RIGHT));
            }
            else if (inputtype == svlTypeImageMono8Stereo) {
                CreateXCheckImageMono<unsigned char>(stimg->GetUCharPointer(SVL_LEFT),
                                                     XCheckImage->GetUCharPointer(SVL_RIGHT),
                                                     stimg->GetWidth(SVL_LEFT),
                                                     stimg->GetHeight(SVL_LEFT));
                CreateXCheckImageMono<unsigned char>(stimg->GetUCharPointer(SVL_RIGHT),
                                                     XCheckImage->GetUCharPointer(SVL_LEFT),
                                                     stimg->GetWidth(SVL_RIGHT),
                                                     stimg->GetHeight(SVL_RIGHT));
            }
            else if (inputtype == svlTypeImageMono16Stereo) {
                CreateXCheckImageMono<unsigned short>(reinterpret_cast<unsigned short*>(stimg->GetUCharPointer(SVL_LEFT)),
                                                      reinterpret_cast<unsigned short*>(XCheckImage->GetUCharPointer(SVL_RIGHT)),
                                                      stimg->GetWidth(SVL_LEFT),
                                                      stimg->GetHeight(SVL_LEFT));
                CreateXCheckImageMono<unsigned short>(reinterpret_cast<unsigned short*>(stimg->GetUCharPointer(SVL_RIGHT)),
                                                      reinterpret_cast<unsigned short*>(XCheckImage->GetUCharPointer(SVL_LEFT)),
                                                      stimg->GetWidth(SVL_RIGHT),
                                                      stimg->GetHeight(SVL_RIGHT));
            }
            else return SVL_FAIL;

            // Stereo: computing disparity map
            XCheckStereoAlgorithm->Process(XCheckImage, XCheckDisparityBuffer.Pointer());

            if (procInfo->ID == 1) {
                _SynchronizeThreads(procInfo);
                return SVL_OK;
            }
        }
    }

    if (procInfo->ID == 0) {

        // Stereo: computing disparity map
        StereoAlgorithm->Process(stimg, DisparityBuffer.Pointer());

        if (XCheckEnabled) {
            _SynchronizeThreads(procInfo);

            // Compare results with the cross checked results and update final disparity map
            PerformXCheck();
        }

        // Store disparity map
        ConvertDisparitiesToFloat(DisparityBuffer.Pointer(),
                                  OutputMatrix->GetPointer(),
                                  static_cast<int>(OutputMatrix->GetCols()),
                                  static_cast<int>(OutputMatrix->GetRows()));

        // Apply spatial filter if enabled
        if (SpatialFilterRadius > 0) ApplySpatialFilter(SpatialFilterRadius,
                                                        OutputMatrix->GetPointer(ROI.left, ROI.top),
                                                        SpatialFilterBuffer.Pointer(ROI.top, ROI.left),
                                                        ROI.right - ROI.left,
                                                        ROI.bottom - ROI.top,
                                                        static_cast<int>(OutputMatrix->GetCols()));
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

int svlFilterComputationalStereo::SetCameraGeometry(const svlCameraGeometry & geometry)
{
    svlCameraGeometry::Intrinsics intrinsics[2];
    svlCameraGeometry::Extrinsics extrinsics[2];
    if (geometry.GetIntrinsics(intrinsics[SVL_LEFT],  SVL_LEFT)  != SVL_OK ||
        geometry.GetIntrinsics(intrinsics[SVL_RIGHT], SVL_RIGHT) != SVL_OK ||
        geometry.GetExtrinsics(extrinsics[SVL_LEFT],  SVL_LEFT)  != SVL_OK ||
        geometry.GetExtrinsics(extrinsics[SVL_RIGHT], SVL_RIGHT) != SVL_OK) return SVL_FAIL;
    if (geometry.IsCameraPairRectified(SVL_LEFT, SVL_RIGHT) != SVL_YES) return SVL_FAIL;
    Geometry = geometry;
    return SVL_OK;
}

void svlFilterComputationalStereo::SetROI(const svlRect & rect)
{
    ROI = rect;
}

void svlFilterComputationalStereo::SetROI(int left, int top, int right, int bottom)
{
    SetROI(svlRect(left, top, right, bottom));
}

int svlFilterComputationalStereo::SetSubpixelPrecision(bool enabled)
{
    if (IsInitialized()) return SVL_FAIL;
    SubpixelPrecision = enabled;
    return SVL_OK;
}

int svlFilterComputationalStereo::SetCrossCheck(bool enabled)
{
    if (IsInitialized()) return SVL_FAIL;
    XCheckEnabled = enabled;
    return SVL_OK;
}

void svlFilterComputationalStereo::SetDisparityRange(unsigned int mindisparity, unsigned int maxdisparity)
{
    MinDisparity = mindisparity;
    MaxDisparity = maxdisparity;
}

void svlFilterComputationalStereo::SetBlockSize(unsigned int blocksize)
{
    BlockSize = blocksize;
}

void svlFilterComputationalStereo::SetScalingFactor(unsigned int scalefactor)
{
    ScaleFactor = scalefactor;
}

void svlFilterComputationalStereo::SetQuickSearchRadius(unsigned int searchradius)
{
    NarrowedSearchRadius = searchradius;
}

void svlFilterComputationalStereo::SetSmoothnessFactor(unsigned int smoothness)
{
    Smoothness = smoothness;
}

void svlFilterComputationalStereo::SetTemporalFiltering(double tempfilt)
{
    TemporalFilter = tempfilt;
}

void svlFilterComputationalStereo::SetSpatialFiltering(unsigned int radius)
{
    SpatialFilterRadius = radius;
}

bool svlFilterComputationalStereo::GetSubpixelPrecision()
{
    return SubpixelPrecision;
}

bool svlFilterComputationalStereo::GetCrossCheck()
{
    return XCheckEnabled;
}

void svlFilterComputationalStereo::GetDisparityRange(unsigned int& mindisparity, unsigned int& maxdisparity)
{
    mindisparity = MinDisparity;
    maxdisparity = MaxDisparity;
}

unsigned int svlFilterComputationalStereo::GetBlockSize()
{
    return BlockSize;
}

unsigned int svlFilterComputationalStereo::GetScalingFactor()
{
    return ScaleFactor;
}

unsigned int svlFilterComputationalStereo::GetQuickSearchRadius()
{
    return NarrowedSearchRadius;
}

unsigned int svlFilterComputationalStereo::GetSmoothnessFactor()
{
    return Smoothness;
}

double svlFilterComputationalStereo::GetTemporalFiltering()
{
    return TemporalFilter;
}

unsigned int svlFilterComputationalStereo::GetSpatialFiltering()
{
    return SpatialFilterRadius;
}

void svlFilterComputationalStereo::SetMethod(svlFilterComputationalStereo::StereoMethod method)
{
    Method = method;
}

svlFilterComputationalStereo::StereoMethod svlFilterComputationalStereo::GetMethod()
{
    return Method;
}

void svlFilterComputationalStereo::CreateXCheckImageColor(unsigned char* source, unsigned char* target,
                                                          const unsigned int width, const unsigned int height)
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
    const int width = static_cast<int>(DisparityBuffer.width()) - 1;
    const int height = static_cast<int>(DisparityBuffer.height()) - 1;
    int i, j, k, r, l, from, to, dispmin, disp, prevdisp;

    // find occlusions and inconsistencies
    if (SubpixelPrecision) {
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
    for (j = ROI.top - 1; j <= ROI.bottom; j ++) {
        from = 0x7FFFFFFF;
        to = -1;
        prevdisp = 0x7FFFFFFF;
        dispmin = 0x7FFFFFFF;
        for (i = ROI.left - 1; i <= ROI.right; i ++) {
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

void svlFilterComputationalStereo::ConvertDisparitiesToFloat(int* input, float* output, const int width, const int height)
{
    int i, j;
    const float mltplr = 0.25f;

    if (SubpixelPrecision) {
        for (j = 0; j < height; j ++) {
            for (i = 0; i < width; i ++) {
                *output = mltplr * (*input);
                input ++;
                output ++;
            }
        }
    }
    else {
        for (j = 0; j < height; j ++) {
            for (i = 0; i < width; i ++) {
                *output = static_cast<float>(*input);
                input ++;
                output ++;
            }
        }
    }
}

void svlFilterComputationalStereo::ApplySpatialFilter(const int radius,
                                                      float* disparitymap, float* tempbuffer,
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
            input = disparitymap + l * linestride + xstart;
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
                input = disparitymap + ystart * linestride + xstart;
                for (l = ystart; l <= yend; l ++) {
                    sum -= *input;
                    input += linestride;
                    divider --;
                }
            }

            // Adding next column
            xend = i + radius;
            if (xend < mapwidth) {
                input = disparitymap + ystart * linestride + xend;
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

    // copy temp buffer back to input buffer
    input = tempbuffer;
    output = disparitymap;
    for (j = 0; j < mapheight; j ++) {
        memcpy(output, input, mapwidth * sizeof(float));
        input += linestride;
        output += linestride;
    }
}

