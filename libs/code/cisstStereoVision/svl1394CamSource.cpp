/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svl1394CamSource.cpp,v 1.6 2008/09/30 22:12:59 vagvoba Exp $
  
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

#include <cisstStereoVision/svl1394CamSource.h>
#include "vidDC1394Source.h"

using namespace std;

/***************************************/
/*** svl1394CamSource class ************/
/***************************************/

svl1394CamSource::svl1394CamSource(bool stereo) : svlFilterBase()
{
    Libdc1394Grab* cam;
    if (stereo) {
        SetFilterToSource(svlTypeImageRGBStereo);

        OutputData = new svlSampleImageRGBStereo;
        cam = new Libdc1394Grab();
        Camera[SVL_LEFT] = (void*)cam;
        cam = new Libdc1394Grab();
        Camera[SVL_RIGHT] = (void*)cam;

        if (OutputData == 0 || Camera[SVL_LEFT] == 0 || Camera[SVL_RIGHT] == 0) ErrorInConstructor();        
    }
    else {
        SetAlgoToSource(svlTypeImageRGB);

        OutputData = new svlSampleImageRGB;
        cam = new Libdc1394Grab();
        Camera[SVL_LEFT] = (void*)cam;
        Camera[SVL_RIGHT] = 0;

        if (OutputData == 0 || Camera[SVL_LEFT] == 0) ErrorInConstructor();        
    }

    Stereo = stereo;
    CameraID[SVL_LEFT] = 0;
    CameraID[SVL_RIGHT] = 1;
    Width[SVL_LEFT] = Width[SVL_RIGHT] = 640;
    Height[SVL_LEFT] = Height[SVL_RIGHT] = 480;
    Format[SVL_LEFT] = Format[SVL_RIGHT] = svl1394PixelRGB8;
    FPS = 30.0;
    YUYVEnabled[SVL_LEFT] = YUYVEnabled[SVL_RIGHT] = false;
    RGBEnabled[SVL_LEFT] = RGBEnabled[SVL_RIGHT] = false;
}

svl1394CamSource::~svl1394CamSource()
{
    Release();

    if (OutputData) delete(OutputData);

    Libdc1394Grab* cam;
    if (Camera[0]) {
        cam = (Libdc1394Grab*)Camera[0];
        delete(cam);
    }
    if (Camera[1]) {
        cam = (Libdc1394Grab*)Camera[1];
        delete(cam);
    }
}

int svl1394CamSource::GetWidth(int videoch)
{
    if (IsDataValid(GetOutputType(), OutputData) != SVL_OK)
        return SVL_FAIL;
    if (Stereo) {
        svlSampleImageRGBStereo* img = (svlSampleImageRGBStereo*)OutputData;
        if (videoch == SVL_LEFT) return img->pixmap_left.width();
        if (videoch == SVL_RIGHT) return img->pixmap_right.width();
    }
    else {
        svlSampleImageRGB* img = (svlSampleImageRGB*)OutputData;
        return img->pixmap.width();
    }
    return SVL_WRONG_CHANNEL;
}

int svl1394CamSource::GetHeight(int videoch)
{
    if (IsDataValid(GetOutputType(), OutputData) != SVL_OK)
        return SVL_FAIL;
    if (Stereo) {
        svlSampleImageRGBStereo* img = (svlSampleImageRGBStereo*)OutputData;
        if (videoch == SVL_LEFT) return img->pixmap_left.height();
        if (videoch == SVL_RIGHT) return img->pixmap_right.height();
    }
    else {
        svlSampleImageRGB* img = (svlSampleImageRGB*)OutputData;
        return img->pixmap.height();
    }
    return SVL_WRONG_CHANNEL;
}

int svl1394CamSource::SetCamera(unsigned int camid, unsigned int videoch)
{
    if (videoch > 1) return SVL_FAIL;
    CameraID[videoch] = camid;
    return SVL_OK;
}

int svl1394CamSource::SetResolution(unsigned int width, unsigned int height, unsigned int videoch)
{
    if (videoch > 1) return SVL_FAIL;
    Width[videoch] = width;
    Height[videoch] = height;
    return SVL_OK;
}

int svl1394CamSource::SetColorFormat(svl1394ColorFormat format, unsigned int videoch)
{
    if (videoch > 1) return SVL_FAIL;
    Format[videoch] = format;
    return SVL_OK;
}

int svl1394CamSource::SetFramerate(float fps)
{
    FPS = fps;
    return SVL_OK;
}

int svl1394CamSource::EnableYUYVEncoding(bool yuyvenabled, unsigned int videoch)
{
    if (videoch > 1) return SVL_FAIL;
    YUYVEnabled[videoch] = yuyvenabled;
    return SVL_OK;
}

int svl1394CamSource::EnableRGBEncoding(bool rgbenabled, unsigned int videoch)
{
    if (videoch > 1) return SVL_FAIL;
    RGBEnabled[videoch] = rgbenabled;
    return SVL_OK;
}

int svl1394CamSource::Initialize(svlSample* inputdata)
{
    Release();

    if (Stereo) {
        svlSampleImageRGBStereo* img = (svlSampleImageRGBStereo*)OutputData;
        Libdc1394Grab* cam1 = (Libdc1394Grab*)Camera[SVL_LEFT];
        Libdc1394Grab* cam2 = (Libdc1394Grab*)Camera[SVL_RIGHT];
        cam1->Close();
        cam2->Close();

        Libdc1394ModeProperties modeprops1, modeprops2;
        modeprops1.width = Width[SVL_LEFT];
        modeprops1.height = Height[SVL_LEFT];
        modeprops1.colorspace = Format[SVL_LEFT];
        modeprops2.width = Width[SVL_RIGHT];
        modeprops2.height = Height[SVL_RIGHT];
        modeprops2.colorspace = Format[SVL_RIGHT];

        img->pixmap_left.SetSize(modeprops1.height, modeprops1.width);
        img->pixmap_right.SetSize(modeprops2.height, modeprops2.width);

        cam1->EnableYUYVEncoding(YUYVEnabled[SVL_LEFT]);
        cam1->EnableRGBEncoding(RGBEnabled[SVL_LEFT]);
        cam2->EnableYUYVEncoding(YUYVEnabled[SVL_RIGHT]);
        cam2->EnableRGBEncoding(RGBEnabled[SVL_RIGHT]);
        if (cam1->Open(CameraID[SVL_LEFT], &modeprops1, FPS, (unsigned char*)(img->pixmap_left.Pointer()), img->GetDataSize(SVL_LEFT)) != 0 ||
            cam2->Open(CameraID[SVL_RIGHT], &modeprops2, FPS, (unsigned char*)(img->pixmap_right.Pointer()), img->GetDataSize(SVL_RIGHT)) != 0) {
            Release();
            return SVL_1394_UNABLE_TO_OPEN;
        }
        if (cam1->Start() != 0 ||
            cam2->Start() != 0) {
            Release();
            return SVL_1394_UNABLE_TO_START;
        }
    }
    else {
        svlSampleImageRGB* img = (svlSampleImageRGB*)OutputData;
        Libdc1394Grab* cam = (Libdc1394Grab*)Camera[SVL_LEFT];
        cam->Close();

        Libdc1394ModeProperties modeprops;
        modeprops.width = Width[SVL_LEFT];
        modeprops.height = Height[SVL_LEFT];
        modeprops.colorspace = Format[SVL_LEFT];

        img->pixmap.SetSize(modeprops.height, modeprops.width);

        cam->EnableYUYVEncoding(YUYVEnabled[SVL_LEFT]);
        cam->EnableRGBEncoding(RGBEnabled[SVL_LEFT]);
        if (cam->Open(CameraID[SVL_LEFT], &modeprops, FPS, (unsigned char*)(img->pixmap.Pointer()), img->GetDataSize()) != 0) {
            Release();
            return SVL_1394_UNABLE_TO_OPEN;
        }
        if (cam->Start() != 0) {
            Release();
            return SVL_1394_UNABLE_TO_START;
        }
    }

    return SVL_OK;
}

int svl1394CamSource::ProcessFrame(svlSample* inputdata)
{
    Libdc1394Grab* cam = (Libdc1394Grab*)Camera[SVL_LEFT];
    cout << "svl1394CamSource::ProcessFrame - getting new frame" << endl;
    if (cam->GetImageSync() != 0) return SVL_FAIL;
    return SVL_OK;
}

int svl1394CamSource::ProcessFrame2(svlSample* inputdata)
{
    if (Stereo) {
        Libdc1394Grab* cam = (Libdc1394Grab*)Camera[SVL_RIGHT];
        if (cam->GetImageSync() != 0) return SVL_FAIL;
    }
    return SVL_OK;
}

int svl1394CamSource::Release()
{
    if (Camera[SVL_LEFT]) {
        Libdc1394Grab* cam = (Libdc1394Grab*)Camera[SVL_LEFT];
        cam->Close();
    }
    if (Camera[SVL_RIGHT]) {
        Libdc1394Grab* cam = (Libdc1394Grab*)Camera[SVL_RIGHT];
        cam->Close();
    }

    return SVL_OK;
}

