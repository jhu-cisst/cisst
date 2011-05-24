/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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
#include <cisstStereoVision/svlBufferImage.h>

// For compatibility with earlier OpenCV versions
#ifndef CV_CAP_UNICAP
    #define CV_CAP_UNICAP   600
#endif
#ifndef CV_CAP_DSHOW
    #define CV_CAP_DSHOW    700
#endif


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
    return svlFilterSourceVideoCapture::OpenCV;
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

int svlVidCapSrcOpenCV::GetDeviceList(svlFilterSourceVideoCapture::DeviceInfo **deviceinfo)
{
    if (deviceinfo == 0 || Initialized) return SVL_FAIL;

    int i, j, maxdevices;
    int devid[800], width[800], height[800];
    CvCapture *capture;
    IplImage *frame;
    char* imgdata;

    OCVNumberOfDevices = 0;
    for (j = 1; j <= 7; j ++) {
    // The first 7 APIs are supported right now

        // Bug in OpenCV's Mac OS X QuickTime implementation.
        // At the moment only the first device can be used.
        // To be reviewed once OpenCV is fixed.
        if (j*100 == CV_CAP_QT) maxdevices = 1;
        else maxdevices = 100;

#if CISST_SVL_HAS_VIDEO4LINUX2
        // OpenCV's Video4Linux implementation interferes
        // with SVL's native Video4Linux2 implementation.
        // The two cannot be used in the same time.
        // Skipping OpenCV Video4Linux devices.
        if (j*100 == CV_CAP_VFW) continue;
#endif // CISST_HAS_VIDEO4LINUX2

#if CISST_SVL_HAS_DC1394
        // OpenCV's DC1394 implementation may interfere
        // with SVL's native DC1394 implementation.
        // The two should be used in the same time.
        // Skipping OpenCV DC1394 devices.
        if (j*100 == CV_CAP_IEEE1394) continue;
#endif // CISST_SVL_HAS_DC1394

        imgdata = 0;
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

            // Check if the capture device have already been initialized
            if (frame->imageData == imgdata) {
                cvReleaseCapture(&capture);
                break;
            }
            imgdata = frame->imageData;

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

        deviceinfo[0] = new svlFilterSourceVideoCapture::DeviceInfo[OCVNumberOfDevices];

        for (i = 0; i < OCVNumberOfDevices; i ++) {
            // platform
            deviceinfo[0][i].platform = svlFilterSourceVideoCapture::OpenCV;

            // id
            deviceinfo[0][i].ID = OCVDeviceID[i];

            // name
            std::stringstream strstr;
            switch ((OCVDeviceID[i] / 100) * 100) {
                case CV_CAP_IEEE1394:
#if (CISST_OS == CISST_WINDOWS)
                    strstr << "CMU IEEE1394 Device (OpenCV: " << OCVDeviceID[i] << ")";
#else
                    strstr << "DC1394 Device (OpenCV: " << OCVDeviceID[i] << ")";
#endif
                break;

                case CV_CAP_STEREO:
                    strstr << "TYZX Stereo Device (OpenCV: " << OCVDeviceID[i] << ")";
                break;

                case CV_CAP_VFW:
#if (CISST_OS == CISST_WINDOWS)
                    strstr << "Video for Windows Device (OpenCV: " << OCVDeviceID[i] << ")";
#else
                    strstr << "Video4Linux Device (OpenCV: " << OCVDeviceID[i] << ")";
#endif
                break;

                case CV_CAP_MIL:
                    strstr << "Matrox Imaging Device (OpenCV: " << OCVDeviceID[i] << ")";
                break;

                case CV_CAP_QT:
                    strstr << "QuickTime Device (OpenCV: " << OCVDeviceID[i] << ")";
                break;

                case CV_CAP_UNICAP:
                    strstr << "Unicap Device (OpenCV: " << OCVDeviceID[i] << ")";
                break;

                case CV_CAP_DSHOW:
                    strstr << "DirectShow Device (OpenCV: " << OCVDeviceID[i] << ")";
                break;

                default:
                    strstr << "Unknown Device (OpenCV: " << OCVDeviceID[i] << ")";
            }

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
        ImageBuffer[i] = new svlBufferImage(frame->width, frame->height);
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
    formatlist[0][0].colorspace = svlFilterSourceVideoCapture::PixelRGB8;
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
    format.colorspace = svlFilterSourceVideoCapture::PixelRGB8;
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

    while (baseref->Running) {
        Frame = cvQueryFrame(baseref->OCVCapture[StreamID]);

        if (baseref->ImageBuffer[StreamID]->PushIplImage(Frame) == false) {
            Error = true;
            break;
        }
    }

	return this;
}

