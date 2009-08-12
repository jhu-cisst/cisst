/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlDummySource.cpp 596 2009-07-27 17:01:59Z bvagvol1 $
  
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

#include <cisstStereoVision/svlFilterSourceDummy.h>
#include <string.h>

#ifdef _MSC_VER
    // Quick fix for Visual Studio Intellisense:
    // The Intellisense parser can't handle the CMN_UNUSED macro
    // correctly if defined in cmnPortability.h, thus
    // we should redefine it here for it.
    // Removing this part of the code will not effect compilation
    // in any way, on any platforms.
    #undef CMN_UNUSED
    #define CMN_UNUSED(argument) argument
#endif

using namespace std;

#include "time.h"

/*************************************/
/*** svlFilterSourceDummy class ******/
/*************************************/

svlFilterSourceDummy::svlFilterSourceDummy(svlStreamType type) :
    svlFilterSourceBase(),
    Disparity(0),
    Noise(false)
{
    // Other types may be added in the future
    if (type == svlTypeImageRGB ||
        type == svlTypeImageRGBStereo) {
        AddSupportedType(type);
        OutputData = svlSample::GetNewFromType(type);
    }

    ImageBuffer[0] = ImageBuffer[1] = 0;
}

svlFilterSourceDummy::~svlFilterSourceDummy()
{
    Release();

    if (OutputData) delete OutputData;
    if (ImageBuffer[0]) delete [] ImageBuffer[0];
    if (ImageBuffer[1]) delete [] ImageBuffer[1];
}

int svlFilterSourceDummy::Initialize()
{
    Release();

    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(OutputData);
    for (unsigned int i = 0; i < img->GetVideoChannels(); i ++) {
        if (img->GetWidth(i) <= 0 || img->GetHeight(i) <= 0)
            return SVL_DMYSRC_DATA_NOT_INITIALIZED;
    }

    srand(time(0));

    return SVL_OK;
}

int svlFilterSourceDummy::ProcessFrame(ProcInfo* procInfo)
{
    // Try to keep TargetFrequency
    _OnSingleThread(procInfo) WaitForTargetTimer();

    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(OutputData);
    const unsigned int videochannels = img->GetVideoChannels();
    unsigned int channel, idx;
    unsigned char* ptr;

    if (Noise) {

        for (channel = 0; channel < videochannels; channel ++) {

            ptr = img->GetUCharPointer(channel);

            _ParallelLoop(procInfo, idx, img->GetDataSize(channel))
            {
                ptr[idx] = static_cast<unsigned char>(rand());
            }
        }
    }
    else {

        _ParallelLoop(procInfo, idx, videochannels)
        {
            if (ImageBuffer[idx]) {
                memcpy(img->GetUCharPointer(idx), ImageBuffer[idx], img->GetDataSize(idx));
            }
            else {
                memset(img->GetUCharPointer(idx), 0, img->GetDataSize(idx));
            }
        }
    }

    return SVL_OK;
}

int svlFilterSourceDummy::SetImage(unsigned char* buffer, unsigned int size)
{
    if (GetOutputType() != svlTypeImageRGB || OutputData == 0) return SVL_FAIL;

    svlSampleImageRGB* img = dynamic_cast<svlSampleImageRGB*>(OutputData);
    if (buffer == 0) {
        if (ImageBuffer[0]) {
            delete [] ImageBuffer[0];
            ImageBuffer[0] = 0;
        }
        memset(img->GetUCharPointer(), 0, img->GetDataSize());
    }
    else {
        if (size != img->GetDataSize()) return SVL_FAIL;
        if (ImageBuffer[0]) delete [] ImageBuffer[0];
        ImageBuffer[0] = new unsigned char[size];
        memcpy(ImageBuffer[0], buffer, size);
    }

    return SVL_OK;
}

int svlFilterSourceDummy::SetImage(unsigned char* buffer_left, unsigned int size_left, unsigned char* buffer_right, unsigned int size_right)
{
    if (GetOutputType() != svlTypeImageRGBStereo || OutputData == 0) return SVL_FAIL;

    svlSampleImageRGBStereo* img = dynamic_cast<svlSampleImageRGBStereo*>(OutputData);
    if (buffer_left == 0) {
        if (ImageBuffer[0]) {
            delete [] ImageBuffer[0];
            ImageBuffer[0] = 0;
        }
        memset(img->GetUCharPointer(SVL_LEFT), 0, img->GetDataSize(SVL_LEFT));
    }
    else {
        if (size_left == img->GetDataSize(SVL_LEFT)) {
            if (ImageBuffer[0]) delete [] ImageBuffer[0];
            ImageBuffer[0] = new unsigned char[size_left];
            memcpy(ImageBuffer[0], buffer_left, size_left);
        }
    }
    if (buffer_right == 0) {
        if (ImageBuffer[1]) {
            delete [] ImageBuffer[1];
            ImageBuffer[1] = 0;
        }
        memset(img->GetUCharPointer(SVL_RIGHT), 0, img->GetDataSize(SVL_RIGHT));
    }
    else {
        if (size_right == img->GetDataSize(SVL_RIGHT)) {
            if (ImageBuffer[1]) delete [] ImageBuffer[1];
            ImageBuffer[1] = new unsigned char[size_right];
            memcpy(ImageBuffer[1], buffer_right, size_right);
        }
    }

    return SVL_OK;
}

int svlFilterSourceDummy::SetDimensions(unsigned int width, unsigned int height)
{
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;
    if (OutputData == 0)
        return SVL_FAIL;

    if (OutputData->IsImage()) {
        dynamic_cast<svlSampleImageBase*>(OutputData)->SetSize(width, height);
    }

    return SVL_OK;
}

void svlFilterSourceDummy::SetStereoNoiseDisparity(int disparity)
{
    if (disparity > SVL_DMYSRC_DISPARITY_CAP) disparity = SVL_DMYSRC_DISPARITY_CAP;
    if (disparity < -SVL_DMYSRC_DISPARITY_CAP) disparity = -SVL_DMYSRC_DISPARITY_CAP;
    Disparity = disparity;
}

void svlFilterSourceDummy::Translate(unsigned char* src, unsigned char* dest, const int width, const int height, const int trhoriz, const int trvert)
{
    int abs_h = abs(trhoriz);
    int abs_v = abs(trvert);

    if (width <= abs_h || height <= abs_v) {
        memset(dest, 0, width * height);
        return;
    }

    if (trhoriz == 0) {
        memcpy(dest + max(0, trvert) * width,
               src + max(0, -trvert) * width,
               width * (height - abs_v));
        return;
    }

    int linecopysize = width - abs_h;
    int xfrom = max(0, trhoriz);
    int yfrom = max(0, trvert);
    int yto = height + min(0, trvert);
    int copyxoffset = max(0, -trhoriz);
    int copyyoffset = max(0, -trvert);

    src += width * copyyoffset + copyxoffset;
    dest += width * yfrom + xfrom;
    for (int j = yfrom; j < yto; j ++) {
        memcpy(dest, src, linecopysize);
        src += width;
        dest += width;
    }
}

