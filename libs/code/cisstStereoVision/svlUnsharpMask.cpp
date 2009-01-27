/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlUnsharpMask.cpp,v 1.13 2008/10/28 17:38:28 vagvoba Exp $
  
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

#include <cisstStereoVision/svlUnsharpMask.h>

using namespace std;


/******************************************/
/*** svlUnsharpMask class *****************/
/******************************************/

svlUnsharpMask::svlUnsharpMask() : svlFilterBase()
{
    AddSupportedType(svlTypeImageRGB, svlTypeImageRGB);
    AddSupportedType(svlTypeImageRGBStereo, svlTypeImageRGBStereo);

    Amount = 384; // =150%
    Radius = 2;
    Threshold = 0;
}

svlUnsharpMask::~svlUnsharpMask()
{
    Release();
}

int svlUnsharpMask::Initialize(svlSample* inputdata)
{
    Release();

    switch (GetInputType()) {
        case svlTypeImageRGB:
            OutputData = new svlSampleImageRGB;
        break;

        case svlTypeImageRGBStereo:
            OutputData = new svlSampleImageRGBStereo;
        break;

        case svlTypeInvalid:
        case svlTypeStreamSource:
        case svlTypeStreamSink:
        case svlTypeImageMono8:
        case svlTypeImageMono8Stereo:
        case svlTypeImageMono16:
        case svlTypeImageMono16Stereo:
        case svlTypeImageCustom:
        case svlTypeDepthMap:
        case svlTypeRigidXform:
        case svlTypePointCloud:
            return SVL_INVALID_INPUT_TYPE;
    }

    if (OutputData == 0) return SVL_FAIL;

    // Allocating image buffers
    dynamic_cast<svlSampleImageBase*>(OutputData)->SetSize(*(dynamic_cast<svlSampleImageBase*>(inputdata)));

    return SVL_OK;
}

int svlUnsharpMask::ProcessFrame(ProcInfo* procInfo, svlSample* inputdata)
{
    ///////////////////////////////////////////
    // Check if the input sample has changed //
      if (!IsNewSample(inputdata))
          return SVL_ALREADY_PROCESSED;
    ///////////////////////////////////////////

    svlSampleImageBase* input = dynamic_cast<svlSampleImageBase*>(inputdata);
    svlSampleImageBase* output = dynamic_cast<svlSampleImageBase*>(OutputData);
    unsigned int videochannels = input->GetVideoChannels();
    unsigned int idx;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        if (Radius == 0 || Amount == 256) {
            memcpy(output->GetPointer(idx), input->GetPointer(idx), output->GetDataSize(idx));
            continue;
        }

#if (CISST_SVL_HAS_OPENCV == ON)

        cvSmooth(input->IplImageRef(idx), output->IplImageRef(idx), CV_GAUSSIAN, Radius * 2 + 1);

#else // CISST_SVL_HAS_OPENCV

        FilterBlur(reinterpret_cast<unsigned char*>(input->GetPointer(idx)),
                   reinterpret_cast<unsigned char*>(output->GetPointer(idx)),
                   input->GetWidth(idx),
                   input->GetHeight(idx),
                   Radius);

#endif // CISST_SVL_HAS_OPENCV

        Sharpening(reinterpret_cast<unsigned char*>(input->GetPointer(idx)),
                   reinterpret_cast<unsigned char*>(output->GetPointer(idx)),
                   reinterpret_cast<unsigned char*>(output->GetPointer(idx)),
                   input->GetWidth(idx),
                   input->GetHeight(idx),
                   Amount);
    }

    return SVL_OK;
}

int svlUnsharpMask::Release()
{
    if (OutputData) {
        delete OutputData;
        OutputData = 0;
    }

    return SVL_OK;
}

int svlUnsharpMask::SetAmount(int amount)
{
    if (amount < 1 || amount > 2000)
        return SVL_FAIL;

    // Convert to easily computable scale
    Amount = amount * 256 / 100;
    return SVL_OK;
}

int svlUnsharpMask::SetRadius(int radius)
{
    if (radius < 1 || radius > 30)
        return SVL_FAIL;

    Radius = radius;
    return SVL_OK;
}

int svlUnsharpMask::SetThreshold(int threshold)
{
    if (threshold < 0 || threshold > 255)
        return SVL_FAIL;

    Threshold = threshold;
    return SVL_OK;
}

void svlUnsharpMask::FilterBlur(unsigned char* img_in, unsigned char* img_out, const int width, const int height, int radius)
{
    const int rowstride = width * 3;
    const int rs_minus2 = rowstride - 2;

    int i, j, k, l;
    int xstart, xend, ystart, yend;
    int sum_r, sum_g, sum_b, divider;
    unsigned char *input, *output;

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
        output = img_out + j * rowstride;
        *output = sum_r / divider;
        output ++;
        *output = sum_g / divider;
        output ++;
        *output = sum_b / divider;
        output ++;

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
            *output = sum_r / divider;
            output ++;
            *output = sum_g / divider;
            output ++;
            *output = sum_b / divider;
            output ++;
        }
    }
}

void svlUnsharpMask::Sharpening(unsigned char* img_in, unsigned char* img_mask, unsigned char* img_out, const int width, const int height, int strength)
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

