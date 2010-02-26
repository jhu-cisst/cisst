/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2008 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlFilterImageFlipRotate.h>
#include <cisstImage/imgUCharRGB.h>

using namespace std;

/***************************************/
/*** svlFilterImageFlipRotate class ****/
/***************************************/

CMN_IMPLEMENT_SERVICES(svlFilterImageFlipRotate)

svlFilterImageFlipRotate::svlFilterImageFlipRotate() :
    svlFilterBase(),
    cmnGenericObject()
{
    AddSupportedType(svlTypeImageRGB, svlTypeImageRGB);
    AddSupportedType(svlTypeImageMono8, svlTypeImageMono8);
    AddSupportedType(svlTypeImageMono16, svlTypeImageMono16);
    AddSupportedType(svlTypeImageRGBStereo, svlTypeImageRGBStereo);
    AddSupportedType(svlTypeImageMono8Stereo, svlTypeImageMono8Stereo);
    AddSupportedType(svlTypeImageMono16Stereo, svlTypeImageMono16Stereo);

    CWQuarters[0] = CWQuarters[1] = 0;
    FlipHorizontal[0] = FlipHorizontal[1] = false;
    FlipVertical[0] = FlipVertical[1] = false;
}

svlFilterImageFlipRotate::~svlFilterImageFlipRotate()
{
    Release();
}

int svlFilterImageFlipRotate::Initialize(svlSample* inputdata)
{
    Release();

    svlSampleImageBase* output = 0;
    svlSampleImageBase* input = dynamic_cast<svlSampleImageBase*>(inputdata);
    const unsigned int channels = input->GetVideoChannels();

    switch (GetInputType()) {
        case svlTypeImageRGB:
            output = new svlSampleImageRGB;
        break;

        case svlTypeImageMono8:
            output = new svlSampleImageMono8;
        break;

        case svlTypeImageMono16:
            output = new svlSampleImageMono16;
        break;

        case svlTypeImageRGBStereo:
            output = new svlSampleImageRGBStereo;
        break;

        case svlTypeImageMono8Stereo:
            output = new svlSampleImageMono8Stereo;
        break;

        case svlTypeImageMono16Stereo:
            output = new svlSampleImageMono16Stereo;
        break;

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
        return SVL_FAIL;
    }
    if (output == 0)
        return SVL_ALLOCATION_ERROR;
    OutputData = output;

    // Initializing image sizes and processing parameters
    for (unsigned int i = 0; i < channels; i ++) {
        // Consolidating rotation
        // after which the rotation will be either -1 or 0 or +1
        if (CWQuarters[i] < 0) CWQuarters[i] = 2 - CWQuarters[i];
        CWQuarters[i] = CWQuarters[i] % 4;
        if (CWQuarters[i] == 2) {
            CWQuarters[i] = 0;
            FlipHorizontal[i] = !FlipHorizontal[i];
            FlipVertical[i] = !FlipVertical[i];
        }
        if (CWQuarters[i] == 3) CWQuarters[i] = -1;

        // Computing rotation & flip parameters
        QuickCopy[i] = 0;
        if (CWQuarters[i] == 0) {
            if (FlipHorizontal[i]) {
                if (FlipVertical[i]) {
                    // rotate 180 degrees
                    StartOffset[i] = input->GetWidth(i) * input->GetHeight(i) - 1;
                    Stride[i] = -1;
                    LineStride[i] = -1 * input->GetWidth(i);
                }
                else {
                    // flip horizontally
                    StartOffset[i] = input->GetWidth(i) - 1;
                    Stride[i] = -1;
                    LineStride[i] = input->GetWidth(i);
                }
            }
            else {
                if (FlipVertical[i]) {
                    // upside down
                    QuickCopy[i] = 2;
                }
                else {
                    // same as input
                    QuickCopy[i] = 1;
                }
            }
        }
        else if (CWQuarters[i] == 1) {
            if (FlipHorizontal[i]) {
                if (FlipVertical[i]) {
                    // rotate -90 degrees
                    StartOffset[i] = input->GetHeight(i) * (input->GetWidth(i) - 1);
                    Stride[i] = -1 * input->GetHeight(i);
                    LineStride[i] = 1;
                }
                else {
                    // rotate 90 degrees and flip horizontally
                    StartOffset[i] = 0;
                    Stride[i] = input->GetHeight(i);
                    LineStride[i] = 1;
                }
            }
            else {
                if (FlipVertical[i]) {
                    // rotate 90 degrees and flip vertically
                    StartOffset[i] = input->GetWidth(i) * input->GetHeight(i) - 1;
                    Stride[i] = -1 * input->GetHeight(i);
                    LineStride[i] = -1;
                }
                else {
                    // rotate 90 degrees
                    StartOffset[i] = input->GetHeight(i) - 1;
                    Stride[i] = input->GetHeight(i);
                    LineStride[i] = -1;
                }
            }
        }
        else { // CWQuarters[i] == -1
            if (FlipHorizontal[i]) {
                if (FlipVertical[i]) {
                    // rotate 90 degrees
                    StartOffset[i] = input->GetHeight(i) - 1;
                    Stride[i] = input->GetHeight(i);
                    LineStride[i] = -1;
                }
                else {
                    // rotate -90 degrees and flip horizontally
                    StartOffset[i] = input->GetWidth(i) * input->GetHeight(i) - 1;
                    Stride[i] = -1 * input->GetHeight(i);
                    LineStride[i] = -1;
                }
            }
            else {
                if (FlipVertical[i]) {
                    // rotate -90 degrees and flip vertically
                    StartOffset[i] = 0;
                    Stride[i] = input->GetHeight(i);
                    LineStride[i] = 1;
                }
                else {
                    // rotate -90 degrees
                    StartOffset[i] = input->GetHeight(i) * (input->GetWidth(i) - 1);
                    Stride[i] = -1 * input->GetHeight(i);
                    LineStride[i] = 1;
                }
            }
        }

        if (CWQuarters[i] == 0) output->SetSize(i, input->GetWidth(i), input->GetHeight(i));
        else output->SetSize(i, input->GetHeight(i), input->GetWidth(i));
    }

    return SVL_OK;
}

int svlFilterImageFlipRotate::ProcessFrame(svlProcInfo* procInfo, svlSample* inputdata)
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
        switch (GetInputType()) {
            case svlTypeImageRGB:
            case svlTypeImageRGBStereo:
                FlipRotate<imgUCharRGB::Pixel>(reinterpret_cast<imgUCharRGB::Pixel*>(input->GetUCharPointer(idx)),
                                               reinterpret_cast<imgUCharRGB::Pixel*>(output->GetUCharPointer(idx)),
                                               input->GetWidth(idx), input->GetHeight(idx),
                                               QuickCopy[idx], StartOffset[idx], Stride[idx], LineStride[idx]);
            break;

            case svlTypeImageMono8:
            case svlTypeImageMono8Stereo:
                FlipRotate<unsigned char>(input->GetUCharPointer(idx),
                                          output->GetUCharPointer(idx),
                                          input->GetWidth(idx), input->GetHeight(idx),
                                          QuickCopy[idx], StartOffset[idx], Stride[idx], LineStride[idx]);
            break;

            case svlTypeImageMono16:
            case svlTypeImageMono16Stereo:
                FlipRotate<unsigned short>(reinterpret_cast<unsigned short*>(input->GetUCharPointer(idx)),
                                           reinterpret_cast<unsigned short*>(output->GetUCharPointer(idx)),
                                           input->GetWidth(idx), input->GetHeight(idx),
                                           QuickCopy[idx], StartOffset[idx], Stride[idx], LineStride[idx]);
            break;

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
            return SVL_FAIL;
        }
    }

    return SVL_OK;
}

int svlFilterImageFlipRotate::Release()
{
    if (OutputData) {
        delete OutputData;
        OutputData = 0;
    }
    return SVL_OK;
}

int svlFilterImageFlipRotate::SetHorizontalFlip(bool flip)
{
    if (IsInitialized()) return SVL_FAIL;
    FlipHorizontal[0] = FlipHorizontal[1] = flip;
    return SVL_OK;
}

int svlFilterImageFlipRotate::SetHorizontalFlip(unsigned int videoch, bool flip)
{
    if (IsInitialized()) return SVL_FAIL;
    if (videoch <= 1) FlipHorizontal[videoch] = flip;
    return SVL_OK;
}

int svlFilterImageFlipRotate::SetVerticalFlip(bool flip)
{
    if (IsInitialized()) return SVL_FAIL;
    FlipVertical[0] = FlipVertical[1] = flip;
    return SVL_OK;
}

int svlFilterImageFlipRotate::SetVerticalFlip(unsigned int videoch, bool flip)
{
    if (IsInitialized()) return SVL_FAIL;
    if (videoch <= 1) FlipVertical[videoch] = flip;
    return SVL_OK;
}

int svlFilterImageFlipRotate::SetRotation(int cw_quarters)
{
    if (IsInitialized()) return SVL_FAIL;
    CWQuarters[0] = CWQuarters[1] = cw_quarters;
    return SVL_OK;
}

int svlFilterImageFlipRotate::SetRotation(unsigned int videoch, int cw_quarters)
{
    if (IsInitialized()) return SVL_FAIL;
    if (videoch <= 1) CWQuarters[videoch] = cw_quarters;
    return SVL_OK;
}

