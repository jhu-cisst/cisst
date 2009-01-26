/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vidOCVSource.cpp,v 1.6 2008/11/04 18:09:04 vagvoba Exp $

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

#include "vidOCVSource.h"

using namespace std;


/*************************************/
/*** COpenCVSource class *************/
/*************************************/

COpenCVSource::COpenCVSource() :
    CVideoCaptureSourceBase(),
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

COpenCVSource::~COpenCVSource()
{
    Release();
    if (OCVDeviceID) delete [] OCVDeviceID;
    if (OCVWidth) delete [] OCVWidth;
    if (OCVHeight) delete [] OCVHeight;
}

svlVideoCaptureSource::PlatformType COpenCVSource::GetPlatformType()
{
    return svlVideoCaptureSource::OpenCV;
}

int COpenCVSource::SetStreamCount(unsigned int numofstreams)
{
    if (numofstreams < 1) return SVL_FAIL;

    Release();

    NumOfStreams = numofstreams;

    CaptureProc = new COpenCVSourceThread*[NumOfStreams];
    CaptureThread = new osaThread*[NumOfStreams];
    DeviceID = new int[NumOfStreams];
    ImageBuffer = new svlImageBuffer*[NumOfStreams];
    OCVCapture = new CvCapture*[NumOfStreams];

    for (unsigned int i = 0; i < NumOfStreams; i ++) {
        CaptureProc[i] = 0;
        CaptureThread[i] = 0;
        DeviceID[i] = -1;
        ImageBuffer[i] = 0;
        OCVCapture[i] = 0;
    }

    return SVL_OK;
}

int COpenCVSource::GetDeviceList(svlVideoCaptureSource::DeviceInfo **deviceinfo)
{
    if (deviceinfo == 0 || Initialized) return SVL_FAIL;

    int i, j, maxdevices;
    int devid[600], width[600], height[600];
    CvCapture *capture;
    IplImage *frame;

    OCVNumberOfDevices = 0;
    for (j = 1; j <= 5; j ++) {
    // The first 5 APIs are supported right now

        // Bug in OpenCV's Mac OS X QuickTime implementation.
        // At the moment only the first device can be used.
        // To be reviewed once OpenCV is fixed.
        if (j*100 == CV_CAP_QT) maxdevices = 1;
        else maxdevices = 100;

#if (CISST_HAS_VIDEO4LINUX2 == ON)
        // OpenCV's Video4Linux implementation interferes
        // with SVL's native Video4Linux2 implementation.
        // The two cannot be used in the same time.
        // Skipping OpenCV Video4Linux devices.
        if (j*100 == CV_CAP_VFW) continue;
#endif // CISST_HAS_VIDEO4LINUX2

#if (CISST_HAS_LIBDC1394 == ON)
        // OpenCV's DC1394 implementation may interfere
        // with SVL's native DC1394 implementation.
        // The two should be used in the same time.
        // Skipping OpenCV DC1394 devices.
        if (j*100 == CV_CAP_IEEE1394) continue;
#endif // CISST_HAS_LIBDC1394

        for (i = 0; i < maxdevices; i ++) {
        // Find cameras
            capture = cvCaptureFromCAM(j * 100 + i);
            if (!capture) break;

            // Trying to capture a frame
            // to determine image dimensions
            frame = cvQueryFrame(capture);
            if(!frame) {
                cvReleaseCapture(&capture);
                continue;
            }
            width[OCVNumberOfDevices] = frame->width;
            height[OCVNumberOfDevices] = frame->height;

            cvReleaseCapture(&capture);
            devid[OCVNumberOfDevices] = j * 100 + i;
            OCVNumberOfDevices ++;
        }
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

        deviceinfo[0] = new svlVideoCaptureSource::DeviceInfo[OCVNumberOfDevices];

        for (i = 0; i < OCVNumberOfDevices; i ++) {
            // platform
            deviceinfo[0][i].platform = svlVideoCaptureSource::OpenCV;

            // id
            deviceinfo[0][i].id = OCVDeviceID[i];

            // name
            switch ((OCVDeviceID[i] / 100) * 100) {
                case CV_CAP_IEEE1394:
#if (CISST_OS == CISST_WINDOWS)
                    sprintf(deviceinfo[0][i].name, "CMU IEEE1394 Device (OpenCV: %d)", OCVDeviceID[i]);
#else
                    sprintf(deviceinfo[0][i].name, "DC1394 Device (OpenCV: %d)", OCVDeviceID[i]);
#endif
                break;

                case CV_CAP_STEREO:
                    sprintf(deviceinfo[0][i].name, "TYZX Stereo Device (OpenCV: %d)", OCVDeviceID[i]);
                break;

                case CV_CAP_VFW:
#if (CISST_OS == CISST_WINDOWS)
                    sprintf(deviceinfo[0][i].name, "Video for Windows Device (OpenCV: %d)", OCVDeviceID[i]);
#else
                    sprintf(deviceinfo[0][i].name, "Video4Linux Device (OpenCV: %d)", OCVDeviceID[i]);
#endif
                break;

                case CV_CAP_MIL:
                    sprintf(deviceinfo[0][i].name, "Matrox Imaging Device (OpenCV: %d)", OCVDeviceID[i]);
                break;

                case CV_CAP_QT:
                    sprintf(deviceinfo[0][i].name, "QuickTime Device (OpenCV: %d)", OCVDeviceID[i]);
                break;

                default:
                    sprintf(deviceinfo[0][i].name, "Unknown Device (OpenCV: %d)", OCVDeviceID[i]);
            }

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

int COpenCVSource::Open()
{
    if (NumOfStreams <= 0) return SVL_FAIL;
    if (Initialized) return SVL_OK;

    Close();

    IplImage *frame;

    for (unsigned int i = 0; i < NumOfStreams; i ++) {

        // Opening device
        OCVCapture[i] = cvCaptureFromCAM(DeviceID[i]);
        if (!OCVCapture[i]) goto labError;

        // Trying to capture a frame
        if (!cvGrabFrame(OCVCapture[i])) goto labError;

        frame = cvRetrieveFrame(OCVCapture[i]);
        if(!frame) goto labError;

        // Allocate capture buffers
        ImageBuffer[i] = new svlImageBuffer(frame->width, frame->height);
    }

    Initialized = true;
    return SVL_OK;

labError:
    Close();
    return SVL_FAIL;
}

void COpenCVSource::Close()
{
    if (NumOfStreams <= 0) return;

    Stop();

    Initialized = false;

    for (unsigned int i = 0; i < NumOfStreams; i ++) {
        if (OCVCapture[i]) {
            cvReleaseCapture(&(OCVCapture[i]));
            OCVCapture[i] = 0;
        }

        // Release capture buffers
        if (ImageBuffer[i]) {
            delete ImageBuffer[i];
            ImageBuffer[i] = 0;
        }
    }
}

int COpenCVSource::Start()
{
    if (!Initialized) return SVL_FAIL;
    if (Running) return SVL_OK;

    for (unsigned int i = 0; i < NumOfStreams; i ++) {
        CaptureProc[i] = new COpenCVSourceThread(i);
        CaptureThread[i] = new osaThread;
        Running = true;
        CaptureThread[i]->Create<COpenCVSourceThread, COpenCVSource*>(CaptureProc[i],
                                                                      &COpenCVSourceThread::Proc,
                                                                      this);
        if (CaptureProc[i]->WaitForInit() == false) return SVL_FAIL;
    }

    return SVL_OK;
}

svlImageRGB* COpenCVSource::GetLatestFrame(bool waitfornew, unsigned int videoch)
{
    if (videoch >= NumOfStreams || !Initialized) return 0;
    return ImageBuffer[videoch]->Pull(waitfornew);
}

int COpenCVSource::Stop()
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

bool COpenCVSource::IsRunning()
{
    return Running;
}

int COpenCVSource::SetDevice(int devid, int inid, unsigned int videoch)
{
    if (videoch >= NumOfStreams) return SVL_FAIL;
    DeviceID[videoch] = devid;
    // Input ID is ignored
    return SVL_OK;
}

int COpenCVSource::GetWidth(unsigned int videoch)
{
    if (videoch >= NumOfStreams) return SVL_FAIL;
    return ImageBuffer[videoch]->GetWidth();
}

int COpenCVSource::GetHeight(unsigned int videoch)
{
    if (videoch >= NumOfStreams) return SVL_FAIL;
    return ImageBuffer[videoch]->GetHeight();
}

int COpenCVSource::GetFormatList(unsigned int deviceid, svlVideoCaptureSource::ImageFormat **formatlist)
{
    if (static_cast<int>(deviceid) >= OCVNumberOfDevices || formatlist == 0) return SVL_FAIL;

    formatlist[0] = new svlVideoCaptureSource::ImageFormat[1];
    formatlist[0][0].width = OCVWidth[deviceid];
    formatlist[0][0].height = OCVHeight[deviceid];
    formatlist[0][0].colorspace = svlVideoCaptureSource::PixelRGB8;
    formatlist[0][0].rgb_order = true;
    formatlist[0][0].yuyv_order = false;
    formatlist[0][0].framerate = -1.0;

    return 1;
}

int COpenCVSource::GetFormat(svlVideoCaptureSource::ImageFormat& format, unsigned int videoch)
{
    if (DeviceID[videoch] >= OCVNumberOfDevices) return SVL_FAIL;

    format.width = OCVWidth[DeviceID[videoch]];
    format.height = OCVHeight[DeviceID[videoch]];
    format.colorspace = svlVideoCaptureSource::PixelRGB8;
    format.rgb_order = true;
    format.yuyv_order = false;
    format.framerate = -1.0;

    return SVL_OK;
}

void COpenCVSource::Release()
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


/**************************************/
/*** COpenCVSourceThread class ********/
/**************************************/

void* COpenCVSourceThread::Proc(COpenCVSource* baseref)
{
    // signal success to main thread
    Error = false;
    InitSuccess = true;
    InitEvent.Raise();

    while (baseref->Running) {
        Frame = cvQueryFrame(baseref->OCVCapture[StreamID]);

        if (baseref->ImageBuffer[StreamID]->PushIplImage(Frame) == false) {
            Error = true;
            break;
        }
    }

	return this;
}

