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

#include "svlVidCapSrcV4L2.h"
#include <cisstOSAbstraction/osaThread.h>
#include <cisstStereoVision/svlBufferImage.h>

#include <stdlib.h>
#include <iostream>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/types.h>
#include <linux/videodev2.h>

//#define __verbose__

#define MV4LP_METHOD_STREAMING      0
#define MV4LP_METHOD_READ           1
#define MV4LP_BUFFER_SIZE_TARGET    2
#define MV4LP_MIN_BUFFER_SIZE       2
#define MV4LP_FRAME_TIMEOUT         100
#define MV4LP_CS_UNKNOWN            -1
#define MV4LP_CS_BGR24              0
#define MV4LP_CS_UYVY               1
#define MV4LP_CS_HM12               2
#define MV4LP_CS_MPEG               -10


/*************************************/
/*** svlVidCapSrcV4L2 class **********/
/*************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlVidCapSrcV4L2, svlVidCapSrcBase)

svlVidCapSrcV4L2::svlVidCapSrcV4L2() :
    svlVidCapSrcBase(),
    NumOfStreams(0),
    Initialized(false),
    Running(false),
    CaptureProc(0),
    CaptureThread(0),
	DeviceID(0),
	InputID(0),
	CapStride(0),
	CapWidth(0),
	CapHeight(0),
	DeviceHandle(0),
	CapMethod(0),
	ColorSpace(0),
	FrameBufferSize(0),
    FrameBuffer(0),
    OutputBuffer(0)
{
}

svlVidCapSrcV4L2::~svlVidCapSrcV4L2()
{
    Release();
}

svlFilterSourceVideoCapture::PlatformType svlVidCapSrcV4L2::GetPlatformType()
{
    return svlFilterSourceVideoCapture::LinVideo4Linux2;
}

int svlVidCapSrcV4L2::SetStreamCount(unsigned int numofstreams)
{
    if (numofstreams < 1) return SVL_FAIL;

    Release();

    NumOfStreams = numofstreams;

    CaptureProc = new svlVidCapSrcV4L2Thread*[NumOfStreams];
    CaptureThread = new osaThread*[NumOfStreams];
    DeviceID = new int[NumOfStreams];
    InputID = new int[NumOfStreams];
    CapStride = new int[NumOfStreams];
    CapWidth = new int[NumOfStreams];
    CapHeight = new int[NumOfStreams];
    DeviceHandle = new int[NumOfStreams];
    CapMethod = new int[NumOfStreams];
    ColorSpace = new int[NumOfStreams];
    FrameBufferSize = new int[NumOfStreams];
    FrameBuffer = new FrameBufferType*[NumOfStreams];
    OutputBuffer = new svlBufferImage*[NumOfStreams];

    for (unsigned int i = 0; i < NumOfStreams; i ++) {
        CaptureProc[i] = 0;
        CaptureThread[i] = 0;
        DeviceID[i] = -1;
        InputID[i] = -1;
        CapStride[i] = 0;
        CapWidth[i] = 0;
        CapHeight[i] = 0;
        DeviceHandle[i] = -1;
        CapMethod[i] = 0;
        ColorSpace[i] = 0;
        FrameBufferSize[i] = 0;
        FrameBuffer[i] = 0;
        OutputBuffer[i] = 0;
    }

    return SVL_OK;
}

int svlVidCapSrcV4L2::GetDeviceList(svlFilterSourceVideoCapture::DeviceInfo **deviceinfo)
{
    if (deviceinfo == 0 || Initialized) return SVL_FAIL;

    char tempname[64];
    svlFilterSourceVideoCapture::DeviceInfo tempinfo[16];
    v4l2_capability devprops;
    int i, fd, counter;

    memset(tempinfo, 0, sizeof(svlFilterSourceVideoCapture::DeviceInfo) * 16);

#ifdef __verbose__
    cout << "Starting enumeration... " << endl;
#endif

    // Enumerate device names:
    //   V4L device names are in the form of "/dev/video*"
    //   where * can be an integer from 0 to 255 (minor number).
    //   However, uncompressed video devices are listed only
    //   in the range of 32-47.
    counter = 0;
    for (i = 0; i < 16; i ++) {
    
        sprintf(tempname, "/dev/video%d", i);
#ifdef __verbose__
        cout << "-Opening device: " << tempname << endl;
#endif
        fd = open(tempname, O_RDWR);
        if (fd >= 0) {

#ifdef __verbose__
            cout << "-Device opened" << endl;
#endif
            // Getting device properties
            if (ioctl(fd, VIDIOC_QUERYCAP, &devprops) == 0) {

                if ((devprops.capabilities & V4L2_CAP_STREAMING) != 0) {
                    // streaming source
                }

                if ((devprops.capabilities & V4L2_CAP_VIDEO_CAPTURE) != 0 &&
                    (devprops.capabilities & V4L2_CAP_READWRITE) != 0) {

                    // platform
                    tempinfo[counter].platform = svlFilterSourceVideoCapture::LinVideo4Linux2;

                    // id
                    tempinfo[counter].ID = i;

                    // name
                    sprintf(tempinfo[counter].name, "%s (%s)", devprops.card, tempname);

                    // inputs
                    GetDeviceInputs(fd, tempinfo + counter);

                    // id
                    tempinfo[counter].testok = true;

#ifdef __verbose__
                    cout << "-Device supported - Success!" << endl;
#endif
                    counter ++;
                }
            }
            close(fd);
        }
        else {
            switch (errno) {
                case EACCES: // The caller has no permission to access the device.
                case EBUSY:  // The driver does not support multiple opens and the device is already in use.
                case ENXIO:  // No device corresponding to this device special file exists.
                case ENOMEM: // Insufficient kernel memory was available.
                case EMFILE: // The process already has the maximum number of files open.
                case ENFILE: // The limit on the total number of files open on the system has been reached.
                break;
            }
        }
    }

    // Allocate memory for device info array
    // CALLER HAS TO FREE UP THIS ARRAY!!!
    if (counter > 0) {
        deviceinfo[0] = new svlFilterSourceVideoCapture::DeviceInfo[counter];
        memcpy(deviceinfo[0], tempinfo, counter * sizeof(svlFilterSourceVideoCapture::DeviceInfo));
    }
    else {
        deviceinfo[0] = 0;
    }

#ifdef __verbose__
    cout << "Enumeration finished" << endl;
#endif

    return counter;
}

int svlVidCapSrcV4L2::Open()
{
    if (NumOfStreams <= 0) return SVL_FAIL;
    if (Initialized) return SVL_OK;

    Close();
    
    int j;
    unsigned int i;
    v4l2_capability devprops;
    v4l2_std_id standard;
    v4l2_format format;
//    v4l2_requestbuffers reqbuff;
//    v4l2_buffer buffer;
    char tempname[64];

    for (i = 0; i < NumOfStreams; i ++) {

        // Opening device
        sprintf(tempname, "/dev/video%d", DeviceID[i]);
#ifdef __verbose__
        cout << "Trying: " << tempname << endl;
#endif
        DeviceHandle[i] = open(tempname, O_RDWR);
        if (DeviceHandle[i] < 0) goto labError;
#ifdef __verbose__
        cout << "-Open: handle opened" << endl;
#endif

        // QUERYCAP
        if (ioctl(DeviceHandle[i], VIDIOC_QUERYCAP, &devprops) != 0) goto labError;
        if ((devprops.capabilities & V4L2_CAP_STREAMING) != 0) {
            CapMethod[i] = MV4LP_METHOD_STREAMING;
#ifdef __verbose__
            cout << "-Open: QUERYCAP done - Streaming method selected" << endl;
#endif
        }
        else {
            CapMethod[i] = MV4LP_METHOD_READ;
#ifdef __verbose__
            cout << "-Open: QUERYCAP done - Read method selected" << endl;
#endif
        }

        // Setting input
        if (ioctl(DeviceHandle[i], VIDIOC_S_INPUT, &(InputID[i])) != 0) goto labError;
#ifdef __verbose__
        cout << "-Open: input set" << endl;
#endif

        // Setting standard
        memset(&standard, 0, sizeof(v4l2_std_id));
        standard = V4L2_STD_NTSC_M;
        if (ioctl(DeviceHandle[i], VIDIOC_S_STD, &standard) != 0) goto labError;
#ifdef __verbose__
        cout << "-Open: standard set" << endl;
#endif

        // Setting format
        format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        format.fmt.pix.width  = 720;
        format.fmt.pix.height = 480;
        format.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24;
        format.fmt.pix.field = V4L2_FIELD_INTERLACED;
        if (ioctl(DeviceHandle[i], VIDIOC_S_FMT, &format) != 0) goto labError;

#ifdef __verbose__
        cout << "-Open: format set" << endl;
#endif

#ifdef __verbose__
        // Exhaustive list of pixel formats from V4L2.
        switch(format.fmt.pix.pixelformat) {
            case V4L2_PIX_FMT_RGB332:   cout << "-Open: pixel format = RGB332" << endl; break;
            case V4L2_PIX_FMT_RGB555:   cout << "-Open: pixel format = RGB555" << endl; break;
            case V4L2_PIX_FMT_RGB565:   cout << "-Open: pixel format = RGB565" << endl; break;
            case V4L2_PIX_FMT_RGB555X:  cout << "-Open: pixel format = RGB555X" << endl; break;
            case V4L2_PIX_FMT_RGB565X:  cout << "-Open: pixel format = RGB565X" << endl; break;
            case V4L2_PIX_FMT_BGR24:    cout << "-Open: pixel format = BGR24" << endl; break;
            case V4L2_PIX_FMT_RGB24:    cout << "-Open: pixel format = RGB24" << endl; break;
            case V4L2_PIX_FMT_BGR32:    cout << "-Open: pixel format = BGR32" << endl; break;
            case V4L2_PIX_FMT_RGB32:    cout << "-Open: pixel format = RGB32" << endl; break;
            case V4L2_PIX_FMT_GREY:     cout << "-Open: pixel format = GREY" << endl; break;
            case V4L2_PIX_FMT_YVU410:   cout << "-Open: pixel format = YVU410" << endl; break;
            case V4L2_PIX_FMT_YVU420:   cout << "-Open: pixel format = YVU420" << endl; break;
            case V4L2_PIX_FMT_YUYV:     cout << "-Open: pixel format = YUYV" << endl; break;
            case V4L2_PIX_FMT_UYVY:     cout << "-Open: pixel format = UYVY" << endl; break;
            case V4L2_PIX_FMT_YUV422P:  cout << "-Open: pixel format = YUV422P" << endl; break;
            case V4L2_PIX_FMT_YUV411P:  cout << "-Open: pixel format = YUV411P" << endl; break;
            case V4L2_PIX_FMT_Y41P:     cout << "-Open: pixel format = Y41P" << endl; break;
            case V4L2_PIX_FMT_NV12:     cout << "-Open: pixel format = NV12" << endl; break;
            case V4L2_PIX_FMT_NV21:     cout << "-Open: pixel format = NV21" << endl; break;
            case V4L2_PIX_FMT_YUV410:   cout << "-Open: pixel format = YUV410" << endl; break;
            case V4L2_PIX_FMT_YUV420:   cout << "-Open: pixel format = YUV420" << endl; break;
            case V4L2_PIX_FMT_YYUV:     cout << "-Open: pixel format = YYUV" << endl; break;
            case V4L2_PIX_FMT_HI240:    cout << "-Open: pixel format = HI240" << endl; break;
            case V4L2_PIX_FMT_HM12:     cout << "-Open: pixel format = HM12" << endl; break;
            case V4L2_PIX_FMT_SBGGR8:   cout << "-Open: pixel format = SBGGR8" << endl; break;
            case V4L2_PIX_FMT_MJPEG:    cout << "-Open: pixel format = MJPEG" << endl; break;
            case V4L2_PIX_FMT_JPEG:     cout << "-Open: pixel format = JPEG" << endl; break;
            case V4L2_PIX_FMT_DV:       cout << "-Open: pixel format = DV" << endl; break;
            case V4L2_PIX_FMT_MPEG:     cout << "-Open: pixel format = MPEG" << endl; break;
            case V4L2_PIX_FMT_WNVA:     cout << "-Open: pixel format = WNVA" << endl; break;
            case V4L2_PIX_FMT_SN9C10X:  cout << "-Open: pixel format = SN9C10X" << endl; break;
            case V4L2_PIX_FMT_PWC1:     cout << "-Open: pixel format = PWC1" << endl; break;
            case V4L2_PIX_FMT_PWC2:     cout << "-Open: pixel format = PWC2" << endl; break;
            case V4L2_PIX_FMT_ET61X251: cout << "-Open: pixel format = ET61X251" << endl; break;
            default: cout << "-Open: unknown pixel format" <<endl;
        }
#endif

        if (format.fmt.pix.pixelformat == V4L2_PIX_FMT_BGR24) {
            // Using BGR24
            ColorSpace[i] = MV4LP_CS_BGR24;
        }
        else if (format.fmt.pix.pixelformat == V4L2_PIX_FMT_MPEG) {
            // Using MPEG
            ColorSpace[i] = MV4LP_CS_MPEG;
        }
        else if (format.fmt.pix.pixelformat == V4L2_PIX_FMT_HM12) {
            // Using HM12
            ColorSpace[i] = MV4LP_CS_HM12;
        }
        else if (format.fmt.pix.pixelformat == V4L2_PIX_FMT_UYVY) {
            // Using UYVY
            ColorSpace[i] = MV4LP_CS_UYVY;
        }
        else {
            // Unsupported format
            ColorSpace[i] = MV4LP_CS_UNKNOWN;
        }

        // Setting resolution
        CapWidth[i] = format.fmt.pix.width;
        CapHeight[i] = format.fmt.pix.height;

        // Stride
        CapStride[i] = format.fmt.pix.width * 3;

#ifdef __verbose__
        cout << "-Open: Image properties: " << CapWidth[i] << "*" << CapHeight[i];

        switch (ColorSpace[i]) {
            case MV4LP_CS_BGR24:
                cout << " BGR24";
            break;

            case MV4LP_CS_UYVY:
                cout << " UYVY";
            break;

            case MV4LP_CS_HM12:
                cout << " HM12";
            break;

            case MV4LP_CS_MPEG:
                cout << " MPEG";
            break;

            default:
                cout << " Unknown color space: " << format.fmt.pix.pixelformat;
        }

        cout << endl;
#endif

        if (ColorSpace[i] < 0) {
#ifdef __verbose__
            cout << "-Open: Error - Unsupported image format" << endl;
#endif
            goto labError;
        }

        if (CapMethod[i] == MV4LP_METHOD_STREAMING) {
            // Streaming I/O
            // Not yet supported
            //goto labError;
///*
            struct v4l2_requestbuffers reqbuff;

            // Requesting buffer
            memset(&reqbuff, 0, sizeof(v4l2_requestbuffers));
            reqbuff.count = MV4LP_BUFFER_SIZE_TARGET;
            reqbuff.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            reqbuff.memory = V4L2_MEMORY_MMAP;
            if (ioctl(DeviceHandle[i], VIDIOC_REQBUFS, &reqbuff) != 0)  goto labError;
            // Buffer count may be overridden by the driver
            if (reqbuff.count < MV4LP_MIN_BUFFER_SIZE) goto labError;
            FrameBufferSize[i] = reqbuff.count;
#ifdef __verbose__
            cout << "-Open: buffers requested" << endl;
#endif

            // Query buffers
            FrameBuffer[i] = new FrameBufferType[FrameBufferSize[i]];
            memset(FrameBuffer[i], 0, FrameBufferSize[i] * sizeof(FrameBufferType));

            for (j = 0; j < FrameBufferSize[i]; j++) {
                struct v4l2_buffer buffer;

                memset(&buffer, 0, sizeof(v4l2_buffer));

                buffer.index       = j;
                buffer.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buffer.memory      = V4L2_MEMORY_MMAP;

                if (ioctl(DeviceHandle[i], VIDIOC_QUERYBUF, &buffer) != 0) goto labError;

                FrameBuffer[i][j].length = buffer.length;
                FrameBuffer[i][j].start = mmap(0,                       // start anywhere
                                               buffer.length,
                                               PROT_READ | PROT_WRITE,  // required
                                               MAP_SHARED,              // recommended
                                               DeviceHandle[i],
                                               buffer.m.offset);

                if (FrameBuffer[i][j].start == MAP_FAILED) goto labError;
#ifdef __verbose__
                cout << "--Open: buffer " << j << " parameters received" << endl;
#endif
            }
//*/
        }
        else {
            // Read/write I/O
            FrameBufferSize[i] = MV4LP_BUFFER_SIZE_TARGET;
            FrameBuffer[i] = new FrameBufferType[FrameBufferSize[i]];
            memset(FrameBuffer[i], 0, FrameBufferSize[i] * sizeof(FrameBufferType));
            for (j = 0; j < FrameBufferSize[i]; j++) {
                FrameBuffer[i][j].length = CapHeight[i] * CapStride[i];
                FrameBuffer[i][j].start = new unsigned char[FrameBuffer[i][j].length];
            }
#ifdef __verbose__
            cout << "-Open: frame buffers allocated" << endl;
#endif
        }

        // allocate output buffers
        OutputBuffer[i] = new svlBufferImage(CapWidth[i], CapHeight[i]);
    }

    Initialized = true;
    return SVL_OK;

labError:
    Close();
    return SVL_FAIL;
}

void svlVidCapSrcV4L2::Close()
{
    if (NumOfStreams <= 0) return;

    Stop();

    int j;
    unsigned int i;

    Initialized = false;

    for (i = 0; i < NumOfStreams; i ++) {

        if (DeviceHandle[i] >= 0) {
            close(DeviceHandle[i]);
            DeviceHandle[i] = -1;
        }

        if (FrameBuffer[i]) {
			for (j = 0; j < FrameBufferSize[i]; j++) {
                if (CapMethod[i] == MV4LP_METHOD_STREAMING) {
                    munmap(FrameBuffer[i][j].start, FrameBuffer[i][j].length);
                } else {
					delete [] reinterpret_cast<unsigned char*>(FrameBuffer[i][j].start);
                }
			}
            delete [] FrameBuffer[i];
            FrameBuffer[i] = 0;
        }

        // release output buffers
        if (OutputBuffer[i]) delete OutputBuffer[i];
        OutputBuffer[i] = 0;
    }
}

int svlVidCapSrcV4L2::Start()
{
    if (!Initialized) return SVL_FAIL;
    if (Running) return SVL_OK;

    unsigned int i;

    for (i = 0; i < NumOfStreams; i ++) {
        if (DeviceHandle[i] < 0) return SVL_FAIL;
    }

    Running = true;
    for (i = 0; i < NumOfStreams; i ++) {
        CaptureProc[i] = new svlVidCapSrcV4L2Thread(i);
        CaptureThread[i] = new osaThread;
        CaptureThread[i]->Create<svlVidCapSrcV4L2Thread, svlVidCapSrcV4L2*>(CaptureProc[i],
                                                                  &svlVidCapSrcV4L2Thread::Proc,
                                                                  this);
        if (CaptureProc[i]->WaitForInit() == false) break;
    }
    if (i == NumOfStreams) {
        return SVL_OK;
    }

    Running = false;
    Stop();
    return SVL_FAIL;
}

svlImageRGB* svlVidCapSrcV4L2::GetLatestFrame(bool waitfornew, unsigned int videoch)
{
    if (videoch >= NumOfStreams || !Initialized) return 0;
    return OutputBuffer[videoch]->Pull(waitfornew);
}

int svlVidCapSrcV4L2::Stop()
{
    if (!Initialized) return SVL_FAIL;

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

bool svlVidCapSrcV4L2::IsRunning()
{
    return Running;
}

int svlVidCapSrcV4L2::SetDevice(int devid, int inid, unsigned int videoch)
{
    if (videoch >= NumOfStreams) return SVL_FAIL;
    DeviceID[videoch] = devid;
    InputID[videoch] = inid;
    return SVL_OK;
}

int svlVidCapSrcV4L2::GetWidth(unsigned int videoch)
{
    if (videoch >= NumOfStreams) return SVL_FAIL;
    return CapWidth[videoch];
}

int svlVidCapSrcV4L2::GetHeight(unsigned int videoch)
{
    if (videoch >= NumOfStreams) return SVL_FAIL;
    return CapHeight[videoch];
}

int svlVidCapSrcV4L2::GetFormatList(unsigned int CMN_UNUSED(deviceid), svlFilterSourceVideoCapture::ImageFormat **formatlist)
{
    if (formatlist == 0) return SVL_FAIL;

    formatlist[0] = new svlFilterSourceVideoCapture::ImageFormat[1];
    formatlist[0][0].width = 720;
    formatlist[0][0].height = 480;
    formatlist[0][0].colorspace = svlFilterSourceVideoCapture::PixelYUV422;
    formatlist[0][0].rgb_order = true;
    formatlist[0][0].yuyv_order = false;
    formatlist[0][0].framerate = 30.0;
    formatlist[0][0].custom_mode = -1;

    return 1;
}

int svlVidCapSrcV4L2::GetFormat(svlFilterSourceVideoCapture::ImageFormat& format, unsigned int videoch)
{
    if (videoch >= NumOfStreams) return SVL_FAIL;

    format.width = 720;
    format.height = 480;
    format.colorspace = svlFilterSourceVideoCapture::PixelYUV422;
    format.rgb_order = true;
    format.yuyv_order = false;
    format.framerate = 30.0;
    format.custom_mode = -1;

    return SVL_OK;
}

int svlVidCapSrcV4L2::ReadFrame(unsigned int videoch)
{
    if (Running == false) return SVL_FAIL;

    unsigned int imlen;
    unsigned char *imbuf = OutputBuffer[videoch]->GetPushBuffer(imlen);

    const int w = CapWidth[videoch];
    const int h = CapHeight[videoch];
    const int stride = CapStride[videoch];
    const int line = w * 3;

    int error = SVL_OK;

    if (ColorSpace[videoch] == MV4LP_CS_BGR24) {
        // BGR24

        if (line == stride) {
            if (read(DeviceHandle[videoch], imbuf, imlen) < imlen) error = SVL_FAIL;
        }
        else {
            unsigned char *buf = reinterpret_cast<unsigned char*>(FrameBuffer[videoch][0].start);
            int len = FrameBuffer[videoch][0].length;

            if (read(DeviceHandle[videoch], buf, len) < len) error = SVL_FAIL;

            for (int j = 0; j < h; j ++) {
                memcpy(imbuf, buf, line);
                imbuf += line;
                buf += stride;
            }
        }
    }
    else {
        // UYVY or HM12

        unsigned char *buf1 = reinterpret_cast<unsigned char*>(FrameBuffer[videoch][0].start);

        int ystride = stride / 3;
        int framesize = ystride * h * 3 / 2;

        int ret = 0;
        while (ret == 0) {
            ret = read(DeviceHandle[videoch], buf1, framesize);
            if (ret < framesize) {
                if (ret == -1 && errno == EAGAIN) {
                    ret = 0;
#ifdef __verbose__
                    cout << "EAGAIN [" << videoch << "]" << endl;
#endif
                }
                else {
                    error = SVL_FAIL;
#ifdef __verbose__
                    cout << "Capture failed [" << videoch << "]" << endl;
#endif
                }
            }
            else {
#ifdef __verbose__
                cout << "Frame captured [" << videoch << "]" << endl;
#endif
            }
        }

        if (ColorSpace[videoch] == MV4LP_CS_UYVY) {
            // Convert UYVY to BGR24
            YUV420p_to_BGR24(imbuf, buf1, line, ystride, w, h);
        }
        else {
            // Rescramble HM12 to UYVY
            unsigned char *buf2 = reinterpret_cast<unsigned char*>(FrameBuffer[videoch][1].start);
            int planesize = ystride * h;

            HM12_de_macro_y(buf2, buf1, ystride, ystride, h);
            HM12_de_macro_uv(buf2 + planesize,
                             buf2 + planesize + planesize / 4,
                             buf1 + planesize,
                             ystride / 2,
                             ystride / 2,
                             h / 2);

            // Convert UYVY to BGR24
            YUV420p_to_BGR24(imbuf, buf2, line, ystride, w, h);
        }
    }

    // Add image to the output buffer
    OutputBuffer[videoch]->Push();

	return error;
}


void svlVidCapSrcV4L2::Release()
{
	Close();

    if (CaptureProc) delete [] CaptureProc;
    if (CaptureThread) delete [] CaptureThread;
    if (DeviceID) delete [] DeviceID;;
    if (InputID) delete [] InputID;
    if (CapStride) delete [] CapStride;
    if (CapWidth) delete [] CapWidth;
    if (CapHeight) delete [] CapHeight;
    if (DeviceHandle) delete [] DeviceHandle;
    if (CapMethod) delete [] CapMethod;
    if (ColorSpace) delete [] ColorSpace;
    if (FrameBufferSize) delete [] FrameBufferSize;
    if (FrameBuffer) delete [] FrameBuffer;
    if (OutputBuffer) delete [] OutputBuffer;

    NumOfStreams = 0;
    Initialized = false;
    Running = false;
    CaptureProc = 0;
    CaptureThread = 0;
	DeviceID = 0;
	InputID = 0;
	CapStride = 0;
	CapWidth = 0;
	CapHeight = 0;
	DeviceHandle = 0;
	CapMethod = 0;
	ColorSpace = 0;
	FrameBufferSize = 0;
    FrameBuffer = 0;
    OutputBuffer = 0;
}

int svlVidCapSrcV4L2::GetDeviceInputs(int fd, svlFilterSourceVideoCapture::DeviceInfo *deviceinfo)
{
    if (fd < 0 || deviceinfo == 0) return SVL_FAIL;

    v4l2_input input;
    int i;

    for (i = 0; i < SVL_VCS_ARRAY_LENGTH; i ++) {
        input.index = i;
        if (ioctl(fd, VIDIOC_ENUMINPUT, &input) != 0) break;
        memcpy(deviceinfo->inputnames[i], input.name, 32);
    }

    if (i > 0) {
        deviceinfo->inputcount = i;
    }
    else {
        sprintf(deviceinfo->inputnames[0], "Default");
        deviceinfo->inputcount = 1;
    }

    return SVL_OK;
}

void svlVidCapSrcV4L2::HM12_de_macro_y(unsigned char* dst, unsigned char* src, int dstride, const int w, const int h)
{
    int i, x, y;

    // From mplayer source:
    //   descramble Y plane
    for (y = 0; y < h; y += 16) {
        for (x = 0; x < w; x += 16) {
            for (i = 0; i < 16; i ++) {
                memcpy(dst + x + (y + i) * dstride, src, 16);
                src += 16;
            }
        }
    }
}

void svlVidCapSrcV4L2::HM12_de_macro_uv(unsigned char* dstu, unsigned char* dstv, unsigned char* src, int dstride, const int w, const int h)
{
    int i, x, y, idx;

    // From mplayer source:
    //   descramble U/V plane
    for (y = 0; y < h; y += 16) {
        for (x = 0; x < w; x += 8) {
            for (i = 0; i < 16; i ++) {
                idx = x + (y + i) * dstride;
                dstu[idx + 0] = src[0]; dstv[idx + 0] = src[1];
                dstu[idx + 1] = src[2]; dstv[idx + 1] = src[3];
                dstu[idx + 2] = src[4]; dstv[idx + 2] = src[5];
                dstu[idx + 3] = src[6]; dstv[idx + 3] = src[7];
                dstu[idx + 4] = src[8]; dstv[idx + 4] = src[9];
                dstu[idx + 5] = src[10]; dstv[idx + 5] = src[11];
                dstu[idx + 6] = src[12]; dstv[idx + 6] = src[13];
                dstu[idx + 7] = src[14]; dstv[idx + 7] = src[15];
                src += 16;
            }
        }
    }
}

void svlVidCapSrcV4L2::YUV420p_to_BGR24(unsigned char* dst, unsigned char* src, int dststride, int srcstride, const int w, const int h)
{
    int yplsize = srcstride * h;
    int uplsize = yplsize / 4;
    int uvstride = srcstride / 2;

    unsigned char* src_y_line1 = src;
    unsigned char* src_y_line2 = src + srcstride;
    unsigned char* src_u_line = src + yplsize;
    unsigned char* src_v_line = src_u_line + uplsize;
    unsigned char* dst_line1 = dst;
    unsigned char* dst_line2 = dst + dststride;

    unsigned char* dst1;
    unsigned char* dst2;
    unsigned char* src_y1;
    unsigned char* src_y2;
    unsigned char* src_u;
    unsigned char* src_v;
    int yt, y1, y2, y3, y4, u, v;
    int rt, gt, bt, val;
    int i, j;

    dststride *= 2;
    srcstride *= 2;

    for (j = 0; j < h; j += 2) {
        src_y1 = src_y_line1;
        src_y2 = src_y_line2;
        src_u = src_u_line;
        src_v = src_v_line;
        dst1 = dst_line1;
        dst2 = dst_line2;

        for (i = 0; i < w; i += 2) {
            y1 = *src_y1;
            src_y1 ++;
            y2 = *src_y1;
            src_y1 ++;

            y3 = *src_y2;
            src_y2 ++;
            y4 = *src_y2;
            src_y2 ++;

            u = *src_u;
            src_u ++;
            v = *src_v;
            src_v ++;

            // conversion
            y1 -= 16;
            y2 -= 16;
            y3 -= 16;
            y4 -= 16;
            u -= 128;
            v -= 128;

            rt = 409 * v;
            gt = -100 * u - 208 * v;
            bt = 516 * u;

            ////////////////////////////////////
            // UL
            yt = 298 * y1;

            // B
            val = (yt + bt) >> 8;
            if (val < 0) val = 0;
            else if (val > 255) val = 255;
            *dst1 = static_cast<unsigned char>(val);
            dst1 ++;

            // G
            val = (yt + gt) >> 8;
            if (val < 0) val = 0;
            else if (val > 255) val = 255;
            *dst1 = static_cast<unsigned char>(val);
            dst1 ++;

            // R
            val = (yt + rt) >> 8;
            if (val < 0) val = 0;
            else if (val > 255) val = 255;
            *dst1 = static_cast<unsigned char>(val);
            dst1 ++;

            ////////////////////////////////////
            // UR
            yt = 298 * y2;

            // B
            val = (yt + bt) >> 8;
            if (val < 0) val = 0;
            else if (val > 255) val = 255;
            *dst1 = static_cast<unsigned char>(val);
            dst1 ++;

            // G
            val = (yt + gt) >> 8;
            if (val < 0) val = 0;
            else if (val > 255) val = 255;
            *dst1 = static_cast<unsigned char>(val);
            dst1 ++;

            // R
            val = (yt + rt) >> 8;
            if (val < 0) val = 0;
            else if (val > 255) val = 255;
            *dst1 = static_cast<unsigned char>(val);
            dst1 ++;

            ////////////////////////////////////
            // LL
            yt = 298 * y3;

            // B
            val = (yt + bt) >> 8;
            if (val < 0) val = 0;
            else if (val > 255) val = 255;
            *dst2 = static_cast<unsigned char>(val);
            dst2 ++;

            // G
            val = (yt + gt) >> 8;
            if (val < 0) val = 0;
            else if (val > 255) val = 255;
            *dst2 = static_cast<unsigned char>(val);
            dst2 ++;

            // R
            val = (yt + rt) >> 8;
            if (val < 0) val = 0;
            else if (val > 255) val = 255;
            *dst2 = static_cast<unsigned char>(val);
            dst2 ++;

            ////////////////////////////////////
            // LR
            yt = 298 * y4;

            // B
            val = (yt + bt) >> 8;
            if (val < 0) val = 0;
            else if (val > 255) val = 255;
            *dst2 = static_cast<unsigned char>(val);
            dst2 ++;

            // G
            val = (yt + gt) >> 8;
            if (val < 0) val = 0;
            else if (val > 255) val = 255;
            *dst2 = static_cast<unsigned char>(val);
            dst2 ++;

            // R
            val = (yt + rt) >> 8;
            if (val < 0) val = 0;
            else if (val > 255) val = 255;
            *dst2 = static_cast<unsigned char>(val);
            dst2 ++;
        }

        src_y_line1 += srcstride;
        src_y_line2 += srcstride;
        src_u_line += uvstride;
        src_v_line += uvstride;
        dst_line1 += dststride;
        dst_line2 += dststride;
    }
}


/**************************************/
/*** svlVidCapSrcV4L2Thread class *****/
/**************************************/

void* svlVidCapSrcV4L2Thread::Proc(svlVidCapSrcV4L2* baseref)
{
    // signal success to main thread
    Error = false;
    InitSuccess = true;
    InitEvent.Raise();

    while (baseref->Running) {
        if (baseref->ReadFrame(StreamID) != SVL_OK) {
            Error = true;
            break;
        }
    }

	return this;
}

