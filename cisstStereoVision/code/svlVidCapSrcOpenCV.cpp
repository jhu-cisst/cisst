/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Balazs Vagvolgyi
  Created on: 2008 

  (C) Copyright 2006-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include "svlVidCapSrcOpenCV.h"
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstStereoVision/svlBufferImage.h>


/*************************************/
/*** svlVidCapSrcOpenCV class ********/
/*************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlVidCapSrcOpenCV, svlVidCapSrcBase)

svlVidCapSrcOpenCV::svlVidCapSrcOpenCV() :
    svlVidCapSrcBase(),
    NumOfStreams(0),
    Initialized(false),
    Running(false),
    CaptureProc(0),
    CaptureThread(0),
	DeviceID(0),
    OCVDeviceID(0),
    OCVNumberOfDevices(0),
    OCVWidth(0),
    OCVHeight(0),
    OCVCapture(0),
    ImageBuffer(0)
{
}

svlVidCapSrcOpenCV::~svlVidCapSrcOpenCV()
{
    Release();
    if (OCVDeviceID) delete [] OCVDeviceID;
    if (OCVWidth) delete [] OCVWidth;
    if (OCVHeight) delete [] OCVHeight;
}

svlFilterSourceVideoCapture::PlatformType svlVidCapSrcOpenCV::GetPlatformType()
{
    return svlFilterSourceVideoCaptureTypes::OpenCV;
}

int svlVidCapSrcOpenCV::SetStreamCount(unsigned int numofstreams)
{
    if (numofstreams < 1) return SVL_FAIL;

    Release();

    NumOfStreams = numofstreams;

    CaptureProc = new svlVidCapSrcOpenCVThread*[NumOfStreams];
    CaptureThread = new osaThread*[NumOfStreams];
    DeviceID = new int[NumOfStreams];
    ImageBuffer = new svlBufferImage*[NumOfStreams];
    OCVCapture = new cv::VideoCapture*[NumOfStreams];

    for (unsigned int i = 0; i < NumOfStreams; i ++) {
        CaptureProc[i] = 0;
        CaptureThread[i] = 0;
        DeviceID[i] = -1;
        ImageBuffer[i] = 0;
        OCVCapture[i] = 0;
    }

    return SVL_OK;
}

int svlVidCapSrcOpenCV::GetDeviceList(svlFilterSourceVideoCapture::DeviceInfo **deviceinfo)
{
    if (deviceinfo == 0 || Initialized) return SVL_FAIL;

    int i;
    int devid[800], width[800], height[800];

    OCVNumberOfDevices = 0;
    while (1) {
        cv::VideoCapture capture2(OCVNumberOfDevices);
        if (!capture2.isOpened()) break;

        // Capture frame to determine image size
        cv::Mat mat;
        for (i = 0; i < 50; i ++) {
            capture2 >> mat;
            if (mat.cols >= 1 && mat.rows >= 1) break;
            osaSleep(0.1);
        }
        if (i >= 50) break;

        width[OCVNumberOfDevices] = mat.cols;
        height[OCVNumberOfDevices] = mat.rows;
        devid[OCVNumberOfDevices] = OCVNumberOfDevices;

        OCVNumberOfDevices ++;
    }

    if (OCVDeviceID) {
        delete [] OCVDeviceID;
        OCVDeviceID = 0;
    }
    if (OCVWidth) {
        delete [] OCVWidth;
        OCVWidth = 0;
    }
    if (OCVHeight) {
        delete [] OCVHeight;
        OCVHeight = 0;
    }

    // Allocate memory for device info array
    // CALLER HAS TO FREE UP THIS ARRAY!!!
    if (OCVNumberOfDevices > 0) {

        OCVDeviceID = new int[OCVNumberOfDevices];
        OCVWidth = new int[OCVNumberOfDevices];
        OCVHeight = new int[OCVNumberOfDevices];
        memcpy(OCVDeviceID, devid, OCVNumberOfDevices * sizeof(int));
        memcpy(OCVWidth, width, OCVNumberOfDevices * sizeof(int));
        memcpy(OCVHeight, height, OCVNumberOfDevices * sizeof(int));

        deviceinfo[0] = new svlFilterSourceVideoCapture::DeviceInfo[OCVNumberOfDevices];

        for (i = 0; i < OCVNumberOfDevices; i ++) {
            // platform
            deviceinfo[0][i].platform = svlFilterSourceVideoCaptureTypes::OpenCV;

            // id
            deviceinfo[0][i].ID = OCVDeviceID[i];

            // name
            std::stringstream strstr;
            strstr << "OpenCV video capture device #" << OCVDeviceID[i];
            memset(deviceinfo[0][i].name, 0, SVL_VCS_STRING_LENGTH);
            memcpy(deviceinfo[0][i].name, strstr.str().c_str(), std::min(static_cast<int>(strstr.str().length()), SVL_VCS_STRING_LENGTH - 1));

            // inputs
            deviceinfo[0][i].inputcount = 0;
            deviceinfo[0][i].activeinput = -1;

            // id
            deviceinfo[0][i].testok = true;
        }
    }
    else {
        deviceinfo[0] = 0;
    }

    return OCVNumberOfDevices;
}

int svlVidCapSrcOpenCV::Open()
{
    if (NumOfStreams <= 0) return SVL_FAIL;
    if (Initialized) return SVL_OK;

    Close();

    for (unsigned int i = 0; i < NumOfStreams; i ++) {

        // Opening device
        OCVCapture[i] = new cv::VideoCapture(DeviceID[i]);
        if (!OCVCapture[i]) goto labError;

        // Check if device is open
        if (!OCVCapture[i]->isOpened()) goto labError;
        // Trying to capture a frame
        cv::Mat mat;
        int j;
        for (j = 0; j < 50; j ++) {
            OCVCapture[i][0] >> mat;
            if (mat.cols >= 1 && mat.rows >= 1) break;
            osaSleep(0.1);
        }
        if (j >= 50) goto labError;;

        // Allocate capture buffers
        ImageBuffer[i] = new svlBufferImage(mat.cols, mat.rows);
    }

    Initialized = true;
    return SVL_OK;

labError:
    Close();
    return SVL_FAIL;
}

void svlVidCapSrcOpenCV::Close()
{
    if (NumOfStreams <= 0) return;

    Stop();

    Initialized = false;

    for (unsigned int i = 0; i < NumOfStreams; i ++) {
        if (OCVCapture[i]) {
            delete OCVCapture[i];
            OCVCapture[i] = 0;
        }

        // Release capture buffers
        if (ImageBuffer[i]) {
            delete ImageBuffer[i];
            ImageBuffer[i] = 0;
        }
    }
}

int svlVidCapSrcOpenCV::Start()
{
    if (!Initialized) return SVL_FAIL;
    if (Running) return SVL_OK;

    for (unsigned int i = 0; i < NumOfStreams; i ++) {
        CaptureProc[i] = new svlVidCapSrcOpenCVThread(i);
        CaptureThread[i] = new osaThread;
        Running = true;
        CaptureThread[i]->Create<svlVidCapSrcOpenCVThread, svlVidCapSrcOpenCV*>(CaptureProc[i],
                                                                                &svlVidCapSrcOpenCVThread::Proc,
                                                                                this);
        if (CaptureProc[i]->WaitForInit() == false) return SVL_FAIL;
    }

    return SVL_OK;
}

svlImageRGB* svlVidCapSrcOpenCV::GetLatestFrame(bool waitfornew, unsigned int videoch)
{
    if (videoch >= NumOfStreams || !Initialized) return 0;
    return ImageBuffer[videoch]->Pull(waitfornew);
}

int svlVidCapSrcOpenCV::Stop()
{
    if (!Running) return SVL_FAIL;

    Running = false;

    for (unsigned int i = 0; i < NumOfStreams; i ++) {
        if (CaptureThread[i]) {
            CaptureThread[i]->Wait();
            delete(CaptureThread[i]);
            CaptureThread[i] = 0;
        }
        if (CaptureProc[i]) {
            delete(CaptureProc[i]);
            CaptureProc[i] = 0;
        }
    }

    return SVL_OK;
}

bool svlVidCapSrcOpenCV::IsRunning()
{
    return Running;
}

int svlVidCapSrcOpenCV::SetDevice(int devid, int CMN_UNUSED(inid), unsigned int videoch)
{
    if (videoch >= NumOfStreams) return SVL_FAIL;
    DeviceID[videoch] = devid;
    // Input ID is ignored
    return SVL_OK;
}

int svlVidCapSrcOpenCV::GetWidth(unsigned int videoch)
{
    if (videoch >= NumOfStreams) return SVL_FAIL;
    return ImageBuffer[videoch]->GetWidth();
}

int svlVidCapSrcOpenCV::GetHeight(unsigned int videoch)
{
    if (videoch >= NumOfStreams) return SVL_FAIL;
    return ImageBuffer[videoch]->GetHeight();
}

int svlVidCapSrcOpenCV::GetFormatList(unsigned int deviceid, svlFilterSourceVideoCapture::ImageFormat **formatlist)
{
    if (static_cast<int>(deviceid) >= OCVNumberOfDevices || formatlist == 0) return SVL_FAIL;

    formatlist[0] = new svlFilterSourceVideoCapture::ImageFormat[1];
    formatlist[0][0].width = OCVWidth[deviceid];
    formatlist[0][0].height = OCVHeight[deviceid];
    formatlist[0][0].colorspace = svlFilterSourceVideoCaptureTypes::PixelRGB8;
    formatlist[0][0].rgb_order = true;
    formatlist[0][0].yuyv_order = false;
    formatlist[0][0].framerate = -1.0;
    formatlist[0][0].custom_mode = -1;

    return 1;
}

int svlVidCapSrcOpenCV::GetFormat(svlFilterSourceVideoCapture::ImageFormat& format, unsigned int videoch)
{
    if (DeviceID[videoch] >= OCVNumberOfDevices) return SVL_FAIL;

    format.width = OCVWidth[DeviceID[videoch]];
    format.height = OCVHeight[DeviceID[videoch]];
    format.colorspace = svlFilterSourceVideoCaptureTypes::PixelRGB8;
    format.rgb_order = true;
    format.yuyv_order = false;
    format.framerate = -1.0;
    format.custom_mode = -1;

    return SVL_OK;
}

void svlVidCapSrcOpenCV::Release()
{
	Close();

    if (CaptureProc) delete [] CaptureProc;
    if (CaptureThread) delete [] CaptureThread;
    if (DeviceID) delete [] DeviceID;;
    if (OCVCapture) delete [] OCVCapture;
    if (ImageBuffer) delete [] ImageBuffer;

    NumOfStreams = 0;
    Initialized = false;
    Running = false;
    CaptureProc = 0;
    CaptureThread = 0;
	DeviceID = 0;
    OCVCapture = 0;
    ImageBuffer = 0;
}


/****************************************/
/*** svlVidCapSrcOpenCVThread class *****/
/****************************************/

void* svlVidCapSrcOpenCVThread::Proc(svlVidCapSrcOpenCV* baseref)
{
    // signal success to main thread
    Error = false;
    InitSuccess = true;
    InitEvent.Raise();

    cv::Mat captured_mat, rgb_mat;
    IplImage ipl_img;
    int i;

    while (baseref->Running) {
        for (i = 0; i < 50; i ++) {
            baseref->OCVCapture[StreamID][0] >> captured_mat;
            if (captured_mat.cols >= 1 && captured_mat.rows >= 1) break;
            osaSleep(0.01);
        }
        if (i >= 50) {
            Error = true;
            break;
        }
        if (captured_mat.elemSize() == 1) {
            cv::cvtColor(captured_mat, rgb_mat, CV_GRAY2RGB);
            ipl_img = rgb_mat;
        }
        else if (captured_mat.elemSize() == 3) {
            ipl_img = captured_mat;
        }
        else {
            Error = true;
            break;
        }
        if (baseref->ImageBuffer[StreamID]->PushIplImage(&ipl_img) == false) {
            Error = true;
            break;
        }
    }

	return this;
}

