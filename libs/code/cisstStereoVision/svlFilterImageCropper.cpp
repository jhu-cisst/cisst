/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlImageCropper.cpp 545 2009-07-14 08:38:33Z adeguet1 $
  
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

#include <cisstStereoVision/svlFilterImageCropper.h>
#include <string.h>

using namespace std;

/******************************************/
/*** svlFilterImageCropper class **********/
/******************************************/

CMN_IMPLEMENT_SERVICES(svlFilterImageCropper)

svlFilterImageCropper::svlFilterImageCropper() :
    svlFilterBase(),
    cmnGenericObject()
{
    AddSupportedType(svlTypeImageRGB, svlTypeImageRGB);
    AddSupportedType(svlTypeImageMono8, svlTypeImageMono8);
    AddSupportedType(svlTypeImageMono16, svlTypeImageMono16);
    AddSupportedType(svlTypeImageRGBStereo, svlTypeImageRGBStereo);
    AddSupportedType(svlTypeImageMono8Stereo, svlTypeImageMono8Stereo);
    AddSupportedType(svlTypeImageMono16Stereo, svlTypeImageMono16Stereo);
    AddSupportedType(svlTypeImageMonoFloat, svlTypeImageMonoFloat);
    AddSupportedType(svlTypeImage3DMap, svlTypeImage3DMap);

    SetLeft[0] = SetLeft[1] = 0;
    SetRight[0] = SetRight[1] = 639;
    SetTop[0] = SetTop[1] = 0;
    SetBottom[0] = SetBottom[1] = 479;
}

svlFilterImageCropper::~svlFilterImageCropper()
{
    Release();
}

void svlFilterImageCropper::SetRectangle(unsigned int left, unsigned int top, unsigned int right, unsigned int bottom, unsigned int videoch)
{
    if (videoch > 1) return;
    SetLeft[videoch] = left;
    SetRight[videoch] = right;
    SetTop[videoch] = top;
    SetBottom[videoch] = bottom;
}

void svlFilterImageCropper::SetRectangle(const svlRect & rect, unsigned int videoch)
{
    SetRectangle(rect.left, rect.top, rect.right, rect.bottom, videoch);
}

int svlFilterImageCropper::Initialize(svlSample* inputdata)
{
    Release();

    switch (GetInputType()) {
        case svlTypeImageRGB:
        {
            svlSampleImageRGB* input = dynamic_cast<svlSampleImageRGB*>(inputdata);

            svlSampleImageRGB* output = new svlSampleImageRGB;
            if (output == 0)
                return SVL_ALLOCATION_ERROR;
            OutputData = output;

            CheckAndFixRectangle(SVL_LEFT, input->GetWidth(), input->GetHeight());
            output->SetSize(SVL_LEFT, Right[SVL_LEFT] - Left[SVL_LEFT] + 1, Bottom[SVL_LEFT] - Top[SVL_LEFT] + 1);
        }
        break;

        case svlTypeImageRGBStereo:
        {
            svlSampleImageRGBStereo* input = dynamic_cast<svlSampleImageRGBStereo*>(inputdata);

            svlSampleImageRGBStereo* output = new svlSampleImageRGBStereo;
            if (output == 0)
                return SVL_ALLOCATION_ERROR;
            OutputData = output;

            // Left channel
            CheckAndFixRectangle(SVL_LEFT, input->GetWidth(SVL_LEFT), input->GetHeight(SVL_LEFT));
            output->SetSize(SVL_LEFT, Right[SVL_LEFT] - Left[SVL_LEFT] + 1, Bottom[SVL_LEFT] - Top[SVL_LEFT] + 1);

            // Right channel
            CheckAndFixRectangle(SVL_RIGHT, input->GetWidth(SVL_RIGHT), input->GetHeight(SVL_RIGHT));
            output->SetSize(SVL_RIGHT, Right[SVL_RIGHT] - Left[SVL_RIGHT] + 1, Bottom[SVL_RIGHT] - Top[SVL_RIGHT] + 1);
        }
        break;

        case svlTypeImageMono8:
        {
            svlSampleImageMono8* input = dynamic_cast<svlSampleImageMono8*>(inputdata);

            svlSampleImageMono8* output = new svlSampleImageMono8;
            if (output == 0)
                return SVL_ALLOCATION_ERROR;
            OutputData = output;

            CheckAndFixRectangle(SVL_LEFT, input->GetWidth(), input->GetHeight());
            output->SetSize(SVL_LEFT, Right[SVL_LEFT] - Left[SVL_LEFT] + 1, Bottom[SVL_LEFT] - Top[SVL_LEFT] + 1);
        }
        break;

        case svlTypeImageMono8Stereo:
        {
            svlSampleImageMono8Stereo* input = dynamic_cast<svlSampleImageMono8Stereo*>(inputdata);

            svlSampleImageMono8Stereo* output = new svlSampleImageMono8Stereo;
            if (output == 0)
                return SVL_ALLOCATION_ERROR;
            OutputData = output;

            // Left channel
            CheckAndFixRectangle(SVL_LEFT, input->GetWidth(SVL_LEFT), input->GetHeight(SVL_LEFT));
            output->SetSize(SVL_LEFT, Right[SVL_LEFT] - Left[SVL_LEFT] + 1, Bottom[SVL_LEFT] - Top[SVL_LEFT] + 1);

            // Right channel
            CheckAndFixRectangle(SVL_RIGHT, input->GetWidth(SVL_RIGHT), input->GetHeight(SVL_RIGHT));
            output->SetSize(SVL_RIGHT, Right[SVL_RIGHT] - Left[SVL_RIGHT] + 1, Bottom[SVL_RIGHT] - Top[SVL_RIGHT] + 1);
        }
        break;

        case svlTypeImageMono16:
        {
            svlSampleImageMono16* input = dynamic_cast<svlSampleImageMono16*>(inputdata);

            svlSampleImageMono16* output = new svlSampleImageMono16;
            if (output == 0)
                return SVL_ALLOCATION_ERROR;
            OutputData = output;

            CheckAndFixRectangle(SVL_LEFT, input->GetWidth(), input->GetHeight());
            output->SetSize(SVL_LEFT, Right[SVL_LEFT] - Left[SVL_LEFT] + 1, Bottom[SVL_LEFT] - Top[SVL_LEFT] + 1);
        }
        break;

        case svlTypeImageMono16Stereo:
        {
            svlSampleImageMono16Stereo* input = dynamic_cast<svlSampleImageMono16Stereo*>(inputdata);

            svlSampleImageMono16Stereo* output = new svlSampleImageMono16Stereo;
            if (output == 0)
                return SVL_ALLOCATION_ERROR;
            OutputData = output;

            // Left channel
            CheckAndFixRectangle(SVL_LEFT, input->GetWidth(SVL_LEFT), input->GetHeight(SVL_LEFT));
            output->SetSize(SVL_LEFT, Right[SVL_LEFT] - Left[SVL_LEFT] + 1, Bottom[SVL_LEFT] - Top[SVL_LEFT] + 1);

            // Right channel
            CheckAndFixRectangle(SVL_RIGHT, input->GetWidth(SVL_RIGHT), input->GetHeight(SVL_RIGHT));
            output->SetSize(SVL_RIGHT, Right[SVL_RIGHT] - Left[SVL_RIGHT] + 1, Bottom[SVL_RIGHT] - Top[SVL_RIGHT] + 1);
        }
        break;

        case svlTypeImageMonoFloat:
        {
            svlSampleImageMonoFloat* input = dynamic_cast<svlSampleImageMonoFloat*>(inputdata);

            svlSampleImageMonoFloat* output = new svlSampleImageMonoFloat;
            if (output == 0)
                return SVL_ALLOCATION_ERROR;
            OutputData = output;

            CheckAndFixRectangle(SVL_LEFT, input->GetWidth(), input->GetHeight());
            output->SetSize(Right[SVL_LEFT] - Left[SVL_LEFT] + 1, Bottom[SVL_LEFT] - Top[SVL_LEFT] + 1);
        }

        case svlTypeImage3DMap:
        {
            svlSampleImage3DMap* input = dynamic_cast<svlSampleImage3DMap*>(inputdata);

            svlSampleImage3DMap* output = new svlSampleImage3DMap;
            if (output == 0)
                return SVL_ALLOCATION_ERROR;
            OutputData = output;

            CheckAndFixRectangle(SVL_LEFT, input->GetWidth(), input->GetHeight());
            output->SetSize(Right[SVL_LEFT] - Left[SVL_LEFT] + 1, Bottom[SVL_LEFT] - Top[SVL_LEFT] + 1);
        }

        // Other types may be added in the future
        case svlTypeImageRGBA:
        case svlTypeImageRGBAStereo:
        case svlTypeInvalid:
        case svlTypeStreamSource:
        case svlTypeStreamSink:
        case svlTypeImageCustom:
        case svlTypeRigidXform:
        case svlTypePointCloud:
        break;
    }

    return SVL_OK;
}

int svlFilterImageCropper::ProcessFrame(svlProcInfo* procInfo, svlSample* inputdata)
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
    unsigned char *input = 0, *output = 0;
    unsigned int j, width = 0, height = 0, stride = 0;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        stride = id->GetWidth(idx) * id->GetBPP();
        width = od->GetWidth(idx) * od->GetBPP();
        height = od->GetHeight(idx);
        input = id->GetUCharPointer(idx, Left[idx], Top[idx]);
        output = od->GetUCharPointer(idx);

        // copy data
        for (j = 0; j < height; j ++) {
            memcpy(output, input, width);
            input += stride;
            output += width;
        }
    }

    return SVL_OK;
}

int svlFilterImageCropper::Release()
{
    if (OutputData) {
        delete OutputData;
        OutputData = 0;
    }

    return SVL_OK;
}

void svlFilterImageCropper::CheckAndFixRectangle(unsigned int videoch, unsigned int width, unsigned int height)
{
    unsigned int ti;

    // check rectangle corners and fix them if needed
    if (SetLeft[videoch] >= width) Left[videoch] = width - 1;
    else Left[videoch] = SetLeft[videoch];
    if (SetRight[videoch] >= width) Right[videoch] = width - 1;
    else Right[videoch] = SetRight[videoch];
    ti = Left[videoch];
    if (ti > Right[videoch]) {
        Left[videoch] = Right[videoch];
        Right[videoch] = ti;
    }
    if (SetTop[videoch] >= height) Top[videoch] = height - 1;
    else Top[videoch] = SetTop[videoch];
    if (SetBottom[videoch] >= height) Bottom[videoch] = height - 1;
    else Bottom[videoch] = SetBottom[videoch];
    ti = Top[videoch];
    if (ti > Bottom[videoch]) {
        Top[videoch] = Bottom[videoch];
        Bottom[videoch] = ti;
    }
}

