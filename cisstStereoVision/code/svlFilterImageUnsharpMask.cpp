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

#include <cisstStereoVision/svlFilterImageUnsharpMask.h>
#include <cisstStereoVision/svlFilterInput.h>

/******************************************/
/*** svlFilterImageUnsharpMask class ******/
/******************************************/

CMN_IMPLEMENT_SERVICES(svlFilterImageUnsharpMask)

svlFilterImageUnsharpMask::svlFilterImageUnsharpMask() :
    svlFilterBase(),
    OutputImage(0),
    Amount(384), // =150%
    Radius(2),
    Threshold(0)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBStereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);
}

svlFilterImageUnsharpMask::~svlFilterImageUnsharpMask()
{
    Release();
}

int svlFilterImageUnsharpMask::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    Release();

    switch (GetInput()->GetType()) {
        case svlTypeImageRGB:
            OutputImage = new svlSampleImageRGB;
        break;

        case svlTypeImageRGBStereo:
            OutputImage = new svlSampleImageRGBStereo;
        break;

        case svlTypeInvalid:
        case svlTypeStreamSource:
        case svlTypeStreamSink:
        case svlTypeImageRGBA:
        case svlTypeImageRGBAStereo:
        case svlTypeImageMono8:
        case svlTypeImageMono8Stereo:
        case svlTypeImageMono16:
        case svlTypeImageMono16Stereo:
        case svlTypeImageMono32:
        case svlTypeImageMono32Stereo:
        case svlTypeImage3DMap:
        case svlTypeCUDAImageRGB:
        case svlTypeCUDAImageRGBA:
        case svlTypeCUDAImageRGBStereo:
        case svlTypeCUDAImageRGBAStereo:
        case svlTypeCUDAImageMono8:
        case svlTypeCUDAImageMono8Stereo:
        case svlTypeCUDAImageMono16:
        case svlTypeCUDAImageMono16Stereo:
        case svlTypeCUDAImageMono32:
        case svlTypeCUDAImageMono32Stereo:
        case svlTypeCUDAImage3DMap:
        case svlTypeMatrixInt8:
        case svlTypeMatrixInt16:
        case svlTypeMatrixInt32:
        case svlTypeMatrixInt64:
        case svlTypeMatrixUInt8:
        case svlTypeMatrixUInt16:
        case svlTypeMatrixUInt32:
        case svlTypeMatrixUInt64:
        case svlTypeMatrixFloat:
        case svlTypeMatrixDouble:
        case svlTypeTransform3D:
        case svlTypeTargets:
        case svlTypeText:
        case svlTypeCameraGeometry:
        case svlTypeBlobs:
            return SVL_INVALID_INPUT_TYPE;
    }

    if (OutputImage == 0) return SVL_FAIL;

    OutputImage->SetSize(*syncInput);

    syncOutput = OutputImage;

    return SVL_OK;
}

int svlFilterImageUnsharpMask::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = OutputImage;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);

    svlSampleImage* input = dynamic_cast<svlSampleImage*>(syncInput);
    unsigned int videochannels = input->GetVideoChannels();
    unsigned int idx;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        if (Radius == 0 || Amount == 256) {
            memcpy(OutputImage->GetUCharPointer(idx), input->GetUCharPointer(idx), OutputImage->GetDataSize(idx));
            continue;
        }

#if CISST_SVL_HAS_OPENCV

        cvSmooth(input->IplImageRef(idx), OutputImage->IplImageRef(idx), CV_GAUSSIAN, Radius * 2 + 1);

#else // CISST_SVL_HAS_OPENCV

        FilterBlur(input->GetUCharPointer(idx),
                   OutputImage->GetUCharPointer(idx),
                   input->GetWidth(idx),
                   input->GetHeight(idx),
                   Radius);

#endif // CISST_SVL_HAS_OPENCV

        Sharpening(input->GetUCharPointer(idx),
                   OutputImage->GetUCharPointer(idx),
                   OutputImage->GetUCharPointer(idx),
                   input->GetWidth(idx),
                   input->GetHeight(idx));
    }

    return SVL_OK;
}

int svlFilterImageUnsharpMask::Release()
{
    if (OutputImage) {
        delete OutputImage;
        OutputImage = 0;
    }

    return SVL_OK;
}

int svlFilterImageUnsharpMask::SetAmount(int amount)
{
    if (amount < 1 || amount > 2000)
        return SVL_FAIL;

    // Convert to easily computable scale
    Amount = amount * 256 / 100;
    return SVL_OK;
}

int svlFilterImageUnsharpMask::SetRadius(int radius)
{
    if (radius < 1 || radius > 30)
        return SVL_FAIL;

    Radius = radius;
    return SVL_OK;
}

int svlFilterImageUnsharpMask::SetThreshold(int threshold)
{
    if (threshold < 0 || threshold > 255)
        return SVL_FAIL;

    Threshold = threshold;
    return SVL_OK;
}

void svlFilterImageUnsharpMask::FilterBlur(unsigned char* img_in, unsigned char* img_out, const int width, const int height, int radius)
{
    const int rowstride = width * 3;
    const int rs_minus2 = rowstride - 2;

    int i, j, k, l;
    int xstart, xend, ystart, yend;
    int sum_r, sum_g, sum_b, divider;
    unsigned char *input, *OutputImage;

    for (j = 0; j < height; j ++) {

        sum_r = 0;
        sum_g = 0;
        sum_b = 0;
        divider = 0;

        // Initializing region of processing
        xstart = 0;
        xend = radius;
        ystart = j - radius;
        if (ystart < 0) ystart = 0;
        yend = j + radius;
        if (yend >= height) yend = height - 1;

        // Computing initial sum
        for (l = ystart; l <= yend; l ++) {
            input = img_in + l * rowstride + xstart * 3;
            for (k = xstart; k <= xend; k ++) {

                sum_r += *input;
                input ++;
                sum_g += *input;
                input ++;
                sum_b += *input;
                input ++;

                divider ++;
            }
        }

        // Setting value of first column
        OutputImage = img_out + j * rowstride;
        *OutputImage = sum_r / divider;
        OutputImage ++;
        *OutputImage = sum_g / divider;
        OutputImage ++;
        *OutputImage = sum_b / divider;
        OutputImage ++;

        // Proceeding on the rest of the line
        for (i = 1; i < width; i ++) {

            // Subtracting previous column
            xstart = i - radius - 1;
            if (xstart >= 0) {
                input = img_in + ystart * rowstride + xstart * 3;
                for (l = ystart; l <= yend; l ++) {

                    sum_r -= *input;
                    input ++;
                    sum_g -= *input;
                    input ++;
                    sum_b -= *input;
                    input += rs_minus2;

                    divider --;
                }
            }

            // Adding next column
            xend = i + radius;
            if (xend < width) {
                input = img_in + ystart * rowstride + xend * 3;
                for (l = ystart; l <= yend; l ++) {

                    sum_r += *input;
                    input ++;
                    sum_g += *input;
                    input ++;
                    sum_b += *input;
                    input += rs_minus2;

                    divider ++;
                }
            }

            // Setting value
            *OutputImage = sum_r / divider;
            OutputImage ++;
            *OutputImage = sum_g / divider;
            OutputImage ++;
            *OutputImage = sum_b / divider;
            OutputImage ++;
        }
    }
}

void svlFilterImageUnsharpMask::Sharpening(unsigned char* img_in, unsigned char* img_mask, unsigned char* img_out, const int width, const int height)
{
    int i, in, mask, diff, out;
    const int size = width * height * 3;

    if (Threshold > 0) {
        for (i = 0; i < size; i ++) {
            in = *img_in; mask = *img_mask;

            if (in < mask) diff = mask - in;
            else diff = in - mask;

            if (diff < Threshold) {
                *img_out = in;
            }
            else {
                out = mask + (((in - mask) * Amount) >> 8);
                if (out < 0) out = 0;
                if (out > 255) out = 255;
                *img_out = out;
            }

            img_in ++;
            img_mask ++;
            img_out ++;
        }
    }
    else {
        for (i = 0; i < size; i ++) {
            in = *img_in; mask = *img_mask;

            out = mask + (((in - mask) * Amount) >> 8);
            if (out < 0) out = 0;
            if (out > 255) out = 255;
            *img_out = out;

            img_in ++;
            img_mask ++;
            img_out ++;
        }
    }
}

