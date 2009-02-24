/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
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

#include <cisstStereoVision/svlDummySource.h>
#include <cisstOSAbstraction/osaSleep.h>

using namespace std;

#include "time.h"

/*************************************/
/*** svlDummySource class ************/
/*************************************/

svlDummySource::svlDummySource(svlStreamType type) : svlFilterBase()
{
    switch (type) {
        case svlTypeImageRGB:
            SetFilterToSource(svlTypeImageRGB);
            OutputData = new svlSampleImageRGB;
        break;

        case svlTypeImageRGBStereo:
            SetFilterToSource(svlTypeImageRGBStereo);
            OutputData = new svlSampleImageRGBStereo;
        break;

        // Other types may be added in the future
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
        break;
    }

    ImageBuffer[0] = ImageBuffer[1] = 0;

    Disparity = 0;
    Hertz = 30.0;
    Noise = false;
}

svlDummySource::~svlDummySource()
{
    Release();

    if (OutputData) delete OutputData;
    if (ImageBuffer[0]) delete [] ImageBuffer[0];
    if (ImageBuffer[1]) delete [] ImageBuffer[1];
}

int svlDummySource::Initialize(svlSample* inputdata)
{
    Release();

    switch (GetOutputType()) {
        case svlTypeImageRGB:
        {
            svlSampleImageRGB* img = dynamic_cast<svlSampleImageRGB*>(OutputData);
            if (img->GetWidth() <= 0 ||
                img->GetHeight() <= 0)
                return SVL_DMYSRC_DATA_NOT_INITIALIZED;
        }
        break;

        case svlTypeImageRGBStereo:
        {
            svlSampleImageRGBStereo* img = dynamic_cast<svlSampleImageRGBStereo*>(OutputData);
            if (img->GetWidth(SVL_LEFT) <= 0 ||
                img->GetHeight(SVL_LEFT) <= 0 ||
                img->GetWidth(SVL_RIGHT) <= 0 ||
                img->GetHeight(SVL_RIGHT) <= 0)
                return SVL_DMYSRC_DATA_NOT_INITIALIZED;
        }
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
            return SVL_FAIL;
    }

    srand(time(0));

    Timer.Reset();
    Timer.Start();
    ulFrameTime = 1.0 / Hertz;

    return SVL_OK;
}

int svlDummySource::ProcessFrame(ProcInfo* procInfo, svlSample* inputdata)
{
    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(OutputData);
    const unsigned int videochannels = img->GetVideoChannels();
    unsigned int channel, idx;
    unsigned char* ptr;

    _OnSingleThread(procInfo)
    {
        if (FrameCounter > 0) {
            double time = Timer.GetElapsedTime();
            double t1 = ulFrameTime * FrameCounter;
            double t2 = time - ulStartTime;
            if (t1 > t2) osaSleep(t1 - t2);
        }
        else {
            ulStartTime = Timer.GetElapsedTime();
        }
    }

    if (Noise) {

        for (channel = 0; channel < videochannels; channel ++) {

            ptr = reinterpret_cast<unsigned char*>(img->GetPointer(channel));

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
                memcpy(img->GetPointer(idx), ImageBuffer[idx], img->GetDataSize(idx));
            }
            else {
                memset(img->GetPointer(idx), 0, img->GetDataSize(idx));
            }
        }
    }

    return SVL_OK;
}

int svlDummySource::Release()
{
    Timer.Stop();
    return SVL_OK;
}

int svlDummySource::SetImage(unsigned char* buffer, unsigned int size)
{
    if (GetOutputType() != svlTypeImageRGB || OutputData == 0) return SVL_FAIL;

    svlSampleImageRGB* img = dynamic_cast<svlSampleImageRGB*>(OutputData);
    if (buffer == 0) {
        if (ImageBuffer[0]) {
            delete [] ImageBuffer[0];
            ImageBuffer[0] = 0;
        }
        memset(reinterpret_cast<unsigned char*>(img->GetPointer()), 0, img->GetDataSize());
    }
    else {
        if (size != img->GetDataSize()) return SVL_FAIL;
        if (ImageBuffer[0]) delete [] ImageBuffer[0];
        ImageBuffer[0] = new unsigned char[size];
        memcpy(ImageBuffer[0], buffer, size);
    }

    return SVL_OK;
}

int svlDummySource::SetImage(unsigned char* buffer_left, unsigned int size_left, unsigned char* buffer_right, unsigned int size_right)
{
    if (GetOutputType() != svlTypeImageRGBStereo || OutputData == 0) return SVL_FAIL;

    svlSampleImageRGBStereo* img = dynamic_cast<svlSampleImageRGBStereo*>(OutputData);
    if (buffer_left == 0) {
        if (ImageBuffer[0]) {
            delete [] ImageBuffer[0];
            ImageBuffer[0] = 0;
        }
        memset(reinterpret_cast<unsigned char*>(img->GetPointer(SVL_LEFT)), 0, img->GetDataSize(SVL_LEFT));
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
        memset(reinterpret_cast<unsigned char*>(img->GetPointer(SVL_RIGHT)), 0, img->GetDataSize(SVL_RIGHT));
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

int svlDummySource::SetDimensions(unsigned int width, unsigned int height)
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

int svlDummySource::GetWidth(int videoch)
{
    if (videoch != 0 && videoch != 1) return SVL_FAIL;
    if (IsDataValid(GetOutputType(), OutputData) != SVL_OK)
        return SVL_FAIL;

    switch (GetOutputType()) {
        case svlTypeImageRGB:
            return dynamic_cast<svlSampleImageBase*>(OutputData)->GetWidth();
        break;

        case svlTypeImageRGBStereo:
            return dynamic_cast<svlSampleImageBase*>(OutputData)->GetWidth(videoch);
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
            return SVL_FAIL;
    }

    return SVL_OK;
}

int svlDummySource::GetHeight(int videoch)
{
    if (videoch != 0 && videoch != 1) return SVL_FAIL;
    if (IsDataValid(GetOutputType(), OutputData) != SVL_OK)
        return SVL_FAIL;

    switch (GetOutputType()) {
        case svlTypeImageRGB:
            return dynamic_cast<svlSampleImageBase*>(OutputData)->GetHeight();
        break;

        case svlTypeImageRGBStereo:
            return dynamic_cast<svlSampleImageBase*>(OutputData)->GetHeight(videoch);
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
            return SVL_FAIL;
    }

    return SVL_OK;
}

void svlDummySource::SetStereoNoiseDisparity(int disparity)
{
    if (disparity > SVL_DMYSRC_DISPARITY_CAP) disparity = SVL_DMYSRC_DISPARITY_CAP;
    if (disparity < -SVL_DMYSRC_DISPARITY_CAP) disparity = -SVL_DMYSRC_DISPARITY_CAP;
    Disparity = disparity;
}

void svlDummySource::Translate(unsigned char* src, unsigned char* dest, const int width, const int height, const int trhoriz, const int trvert)
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

