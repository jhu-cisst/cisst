/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlImageResizer.cpp 545 2009-07-14 08:38:33Z adeguet1 $
  
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

#include <cisstStereoVision/svlFilterImageResizer.h>

#include <stdio.h>
#include <string.h>

using namespace std;


/******************************************/
/*** svlFilterImageResizer class **********/
/******************************************/

CMN_IMPLEMENT_SERVICES(svlFilterImageResizer)

svlFilterImageResizer::svlFilterImageResizer() :
    svlFilterBase(),
    cmnGenericObject()
{
    AddSupportedType(svlTypeImageRGB, svlTypeImageRGB);
    AddSupportedType(svlTypeImageMono8, svlTypeImageMono8);
    AddSupportedType(svlTypeImageRGBStereo, svlTypeImageRGBStereo);
    AddSupportedType(svlTypeImageMono8Stereo, svlTypeImageMono8Stereo);

//  To be added:
//    AddSupportedType(svlTypeImageMono16, svlTypeImageMono16);
//    AddSupportedType(svlTypeImageMono16Stereo, svlTypeImageMono16Stereo);

    for (unsigned int i = 0; i < 2; i ++) {
        WidthRatio[i] = HeightRatio[i] = 1.0;
        Width[i] = Height[i] = 0;
        TempBuffer[i] = 0;
    }
    InterpolationEnabled = false;
}

svlFilterImageResizer::~svlFilterImageResizer()
{
    Release();
}

int svlFilterImageResizer::SetOutputSize(unsigned int width, unsigned int height, unsigned int videoch)
{
    if (IsInitialized() == true || videoch > 1 || width < 1 || height < 1) return SVL_FAIL;
    Width[videoch] = width;
    Height[videoch] = height;
    return SVL_OK;
}

int svlFilterImageResizer::SetOutputRatio(double widthratio, double heightratio, unsigned int videoch)
{
    if (IsInitialized() == true || videoch > 1 || widthratio <= 0.0 || heightratio <= 0.0) return SVL_FAIL;
    WidthRatio[videoch] = widthratio;
    HeightRatio[videoch] = heightratio;
    Width[videoch] = 0;
    Height[videoch] = 0;
    return SVL_OK;
}

int svlFilterImageResizer::Initialize(svlSample* inputdata)
{
    Release();

    switch (GetInputType()) {
        case svlTypeImageRGB:
            OutputData = new svlSampleImageRGB;
        break;

        case svlTypeImageRGBStereo:
            OutputData = new svlSampleImageRGBStereo;
        break;

        case svlTypeImageMono8:
            OutputData = new svlSampleImageMono8;
        break;

        case svlTypeImageMono8Stereo:
            OutputData = new svlSampleImageMono8Stereo;
        break;

        case svlTypeImageMono16:        // To be added
        case svlTypeImageMono16Stereo:  // To be added

        // Other types may be added in the future
        case svlTypeImageRGBA:
        case svlTypeImageRGBAStereo:
        case svlTypeImageMonoFloat:
        case svlTypeImage3DMap:
        case svlTypeInvalid:
        case svlTypeStreamSource:
        case svlTypeStreamSink:
        case svlTypeImageCustom:
        case svlTypeRigidXform:
        case svlTypePointCloud:
        break;
    }

    svlSampleImageBase* inputimage = dynamic_cast<svlSampleImageBase*>(inputdata);
    const unsigned int numofchannels = inputimage->GetVideoChannels();

    for (unsigned int i = 0; i < numofchannels; i ++) {

        if (Width[i] == 0 || Height[i] == 0) {
            Width[i] = static_cast<unsigned int>(WidthRatio[i] * inputimage->GetWidth(i));
            Height[i] = static_cast<unsigned int>(HeightRatio[i] * inputimage->GetHeight(i));
            if (Width[i] < 1) Width[i] = 1;
            if (Height[i] < 1) Height[i] = 1;
        }

        dynamic_cast<svlSampleImageBase*>(OutputData)->SetSize(i, Width[i], Height[i]);

#if (CISST_SVL_HAS_OPENCV == OFF)
        TempBuffer[i] = new unsigned char[Width[i] * inputimage->GetHeight(i) * inputimage->GetDataChannels()];
#endif // CISST_SVL_HAS_OPENCV OFF
    }

    return SVL_OK;
}

int svlFilterImageResizer::ProcessFrame(svlProcInfo* procInfo, svlSample* inputdata)
{
    ///////////////////////////////////////////
    // Check if the input sample has changed //
      if (!IsNewSample(inputdata))
          return SVL_ALREADY_PROCESSED;
    ///////////////////////////////////////////

    svlSampleImageBase* id = dynamic_cast<svlSampleImageBase*>(inputdata);
    svlSampleImageBase* od = dynamic_cast<svlSampleImageBase*>(OutputData);
    unsigned int videochannels = id->GetVideoChannels();
    unsigned int idx;
    bool weq, heq;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        weq = (Width[idx] == id->GetWidth(idx));
        heq = (Height[idx] == id->GetHeight(idx));

        if (weq && heq) {
            memcpy(od->GetUCharPointer(idx), id->GetUCharPointer(idx), id->GetDataSize(idx));
            return SVL_OK;
        }

#if (CISST_SVL_HAS_OPENCV == ON)

        if (InterpolationEnabled) cvResize(id->IplImageRef(idx), od->IplImageRef(idx), CV_INTER_LINEAR);
        else cvResize(id->IplImageRef(idx), od->IplImageRef(idx), CV_INTER_NN);

#else // CISST_SVL_HAS_OPENCV

        if (id->GetDataChannels() == 3) { // RGB
            if (InterpolationEnabled) {
                if (weq) {
                    ResampleAndInterpolateVRGB24(id->GetUCharPointer(idx), id->GetHeight(idx),
                                                 od->GetUCharPointer(idx), od->GetHeight(idx),
                                                 od->GetWidth(idx));
                }
                else if (heq) {
                    ResampleAndInterpolateHRGB24(id->GetUCharPointer(idx), id->GetWidth(idx),
                                                 od->GetUCharPointer(idx), od->GetWidth(idx),
                                                 id->GetHeight(idx));
                }
                else {
                    ResampleAndInterpolateHRGB24(id->GetUCharPointer(idx), id->GetWidth(idx),
                                                 TempBuffer[idx], od->GetWidth(idx),
                                                 id->GetHeight(idx));
                    ResampleAndInterpolateVRGB24(TempBuffer[idx], id->GetHeight(idx),
                                                 od->GetUCharPointer(idx), od->GetHeight(idx),
                                                 od->GetWidth(idx));
                }
            }
            else {
                ResampleRGB24(id->GetUCharPointer(idx), id->GetWidth(idx), id->GetHeight(idx),
                              od->GetUCharPointer(idx), od->GetWidth(idx), od->GetHeight(idx));
            }
        }
        else { // Mono8
            if (InterpolationEnabled) {
                if (weq) {
                    ResampleAndInterpolateVMono8(id->GetUCharPointer(idx), id->GetHeight(idx),
                                                 od->GetUCharPointer(idx), od->GetHeight(idx),
                                                 od->GetWidth(idx));
                }
                else if (heq) {
                    ResampleAndInterpolateHMono8(id->GetUCharPointer(idx), id->GetWidth(idx),
                                                 od->GetUCharPointer(idx), od->GetWidth(idx),
                                                 id->GetHeight(idx));
                }
                else {
                    ResampleAndInterpolateHMono8(id->GetUCharPointer(idx), id->GetWidth(idx),
                                                 TempBuffer[idx], od->GetWidth(idx),
                                                 id->GetHeight(idx));
                    ResampleAndInterpolateVMono8(TempBuffer[idx], id->GetHeight(idx),
                                                 od->GetUCharPointer(idx), od->GetHeight(idx),
                                                 od->GetWidth(idx));
                }
            }
            else {
                ResampleMono8(id->GetUCharPointer(idx), id->GetWidth(idx), id->GetHeight(idx),
                              od->GetUCharPointer(idx), od->GetWidth(idx), od->GetHeight(idx));
            }
        }

#endif // CISST_SVL_HAS_OPENCV
    }

    return SVL_OK;
}

int svlFilterImageResizer::Release()
{
    if (OutputData) {
        delete OutputData;
        OutputData = 0;
    }
    if (TempBuffer[SVL_LEFT]) {
        delete [] TempBuffer[SVL_LEFT];
        TempBuffer[SVL_LEFT] = 0;
    }
    if (TempBuffer[SVL_RIGHT]) {
        delete [] TempBuffer[SVL_RIGHT];
        TempBuffer[SVL_RIGHT] = 0;
    }
    return SVL_OK;
}

int svlFilterImageResizer::ResampleMono8(unsigned char* src, const unsigned int srcwidth, const unsigned int srcheight,
                                   unsigned char* dst, const unsigned int dstwidth, const unsigned int dstheight)
{
    unsigned int i, j;
    unsigned int x1, y1, x2, y2;
    unsigned char *psrc, *plsrc, *pdst;

    // vertical sampling loop
    plsrc = src;
    pdst = dst;
    y1 = 0;
    y2 = dstheight;
    for (j = 0; j < dstheight; j ++) {

        // horizontal sampling loop
        psrc = plsrc;
        x1 = 0;
        x2 = dstwidth;
        for (i = 0; i < dstwidth; i ++) {
            *pdst = *psrc;
            pdst ++;

            x1 += srcwidth;
            while (x1 >= x2) {
                x2 += dstwidth;
                psrc ++;
            }
        }

        y1 += srcheight;
        while (y1 >= y2) {
            y2 += dstheight;
            plsrc += srcwidth;
        }
    }

    return SVL_OK;
}

int svlFilterImageResizer::ResampleAndInterpolateHMono8(unsigned char* src, const unsigned int srcwidth,
                                                  unsigned char* dst, const unsigned int dstwidth,
                                                  const unsigned int height)
{
    unsigned int i, j;
    unsigned int x1, x2;
    int wx1, wx2;
    unsigned char *psrc, *plsrc, *pdst;
    unsigned char prev_col, this_col;

    // eliminating division by using integral powers of 2
    const unsigned int fast_dstwidth = 256;   // 2^8
    const unsigned int fast_srcwidth = fast_dstwidth * srcwidth / dstwidth;

    plsrc = src;
    pdst = dst;
    for (j = 0; j < height; j ++) {

        // horizontal sampling loop
        psrc = plsrc;
        x1 = 0;
        x2 = 128;
        prev_col = this_col = *psrc;
        wx1 = 0;
        wx2 = fast_dstwidth;

        for (i = 0; i < dstwidth; i ++) {
            *pdst = (wx1 * prev_col + wx2 * this_col) >> 8;
            pdst ++;

            x1 += fast_srcwidth;
            while (x1 > x2) {
                x2 += fast_dstwidth;
                prev_col = this_col; this_col = *psrc; psrc ++;
            }

            wx1 = x2 - x1;
            wx2 = fast_dstwidth - wx1;
        }
        plsrc += srcwidth;
   }

   return SVL_OK;
}

int svlFilterImageResizer::ResampleAndInterpolateVMono8(unsigned char* src, const unsigned int srcheight,
                                                  unsigned char* dst, const unsigned int dstheight,
                                                  const unsigned int width)
{
    unsigned int i, j;
    unsigned int y1, y2;
    int wy1, wy2;
    unsigned char *psrc, *pcsrc, *pdst, *pcdst;
    unsigned char prev_col, this_col;

    // eliminating division by using integral powers of 2
    const unsigned int fast_dstheight = 256;   // 2^8
    const unsigned int fast_srcheight = fast_dstheight * srcheight / dstheight;

    pcsrc = src;
    pcdst = dst;
    for (j = 0; j < width; j ++) {

        // vertical sampling loop
        psrc = pcsrc;
        pdst = pcdst;
        y1 = 0;
        y2 = 128;
        prev_col = this_col = *psrc;
        wy1 = 0;
        wy2 = fast_dstheight;

        for (i = 0; i < dstheight; i ++) {
            *pdst = (wy1 * prev_col + wy2 * this_col) >> 8;
            pdst += width;

            y1 += fast_srcheight;
            while (y1 > y2) {
                y2 += fast_dstheight;
                prev_col = this_col; this_col = *psrc; psrc += width;
            }

            wy1 = y2 - y1;
            wy2 = fast_dstheight - wy1;
        }
        pcsrc ++;
        pcdst ++;
   }

   return SVL_OK;
}

int svlFilterImageResizer::ResampleRGB24(unsigned char* src, const unsigned int srcwidth, const unsigned int srcheight,
                                   unsigned char* dst, const unsigned int dstwidth, const unsigned int dstheight)
{
    unsigned int i, j;
    unsigned int x1, y1, x2, y2;
    unsigned char *psrc, *plsrc, *pdst;
    const unsigned int srcstride = srcwidth * 3;

    // vertical sampling loop
    plsrc = src;
    pdst = dst;
    y1 = 0;
    y2 = dstheight;
    for (j = 0; j < dstheight; j ++) {

        // horizontal sampling loop
        psrc = plsrc;
        x1 = 0;
        x2 = dstwidth;
        for (i = 0; i < dstwidth; i ++) {
            *pdst = psrc[0]; pdst ++;
            *pdst = psrc[1]; pdst ++;
            *pdst = psrc[2]; pdst ++;

            x1 += srcwidth;
            while (x1 >= x2) {
                x2 += dstwidth;
                psrc += 3;
            }
        }

        y1 += srcheight;
        while (y1 >= y2) {
            y2 += dstheight;
            plsrc += srcstride;
        }
    }

    return SVL_OK;
}

int svlFilterImageResizer::ResampleAndInterpolateHRGB24(unsigned char* src, const unsigned int srcwidth,
                                                  unsigned char* dst, const unsigned int dstwidth,
                                                  const unsigned int height)
{
    unsigned int i, j;
    unsigned int x1, x2;
    int wx1, wx2;
    unsigned char *psrc, *plsrc, *pdst;
    unsigned char prev_r, prev_g, prev_b, this_r, this_g, this_b;
    const unsigned int srcstride = srcwidth * 3;

    // eliminating division by using integral powers of 2
    const unsigned int fast_dstwidth = 256;   // 2^8
    const unsigned int fast_srcwidth = fast_dstwidth * srcwidth / dstwidth;

    plsrc = src;
    pdst = dst;
    for (j = 0; j < height; j ++) {

        // horizontal sampling loop
        psrc = plsrc;
        x1 = 0;
        x2 = 128;
        prev_r = this_r = psrc[0];
        prev_g = this_g = psrc[1];
        prev_b = this_b = psrc[2];
        wx1 = 0;
        wx2 = fast_dstwidth;

        for (i = 0; i < dstwidth; i ++) {
            *pdst = (wx1 * prev_r + wx2 * this_r) >> 8;
            pdst ++;
            *pdst = (wx1 * prev_g + wx2 * this_g) >> 8;
            pdst ++;
            *pdst = (wx1 * prev_b + wx2 * this_b) >> 8;
            pdst ++;

            x1 += fast_srcwidth;
            while (x1 > x2) {
                x2 += fast_dstwidth;
                prev_r = this_r; this_r = *psrc; psrc ++;
                prev_g = this_g; this_g = *psrc; psrc ++;
                prev_b = this_b; this_b = *psrc; psrc ++;
            }

            wx1 = x2 - x1;
            wx2 = fast_dstwidth - wx1;
        }
        plsrc += srcstride;
   }

   return SVL_OK;
}

int svlFilterImageResizer::ResampleAndInterpolateVRGB24(unsigned char* src, const unsigned int srcheight,
                                                  unsigned char* dst, const unsigned int dstheight,
                                                  const unsigned int width)
{
    unsigned int i, j;
    unsigned int y1, y2;
    int wy1, wy2;
    unsigned char *psrc, *pcsrc, *pdst, *pcdst;
    unsigned char prev_r, prev_g, prev_b, this_r, this_g, this_b;
    const unsigned int stride = width * 3 - 2;

    // eliminating division by using integral powers of 2
    const unsigned int fast_dstheight = 256;   // 2^8
    const unsigned int fast_srcheight = fast_dstheight * srcheight / dstheight;

    pcsrc = src;
    pcdst = dst;
    for (j = 0; j < width; j ++) {

        // vertical sampling loop
        psrc = pcsrc;
        pdst = pcdst;
        y1 = 0;
        y2 = 128;
        prev_r = this_r = psrc[0];
        prev_g = this_g = psrc[1];
        prev_b = this_b = psrc[2];
        wy1 = 0;
        wy2 = fast_dstheight;

        for (i = 0; i < dstheight; i ++) {
            *pdst = (wy1 * prev_r + wy2 * this_r) >> 8;
            pdst ++;
            *pdst = (wy1 * prev_g + wy2 * this_g) >> 8;
            pdst ++;
            *pdst = (wy1 * prev_b + wy2 * this_b) >> 8;
            pdst += stride;

            y1 += fast_srcheight;
            while (y1 > y2) {
                y2 += fast_dstheight;
                prev_r = this_r; this_r = *psrc; psrc ++;
                prev_g = this_g; this_g = *psrc; psrc ++;
                prev_b = this_b; this_b = *psrc; psrc += stride;
            }

            wy1 = y2 - y1;
            wy2 = fast_dstheight - wy1;
        }
        pcsrc += 3;
        pcdst += 3;
   }

   return SVL_OK;
}

