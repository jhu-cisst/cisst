/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
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
#include <cisstStereoVision/svlConverters.h>

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

// #define __verbose__
#ifdef __verbose__
using namespace std;
#endif

#define MV4LP_METHOD_STREAMING      0
#define MV4LP_METHOD_READ           1
#define MV4LP_BUFFER_SIZE_TARGET    2
#define MV4LP_MIN_BUFFER_SIZE       2
#define MV4LP_FRAME_TIMEOUT         100
#define MV4LP_CS_UNKNOWN            -1
#define MV4LP_CS_BGR24              0
#define MV4LP_CS_UYVY               1
#define MV4LP_CS_HM12               2
#define MV4LP_CS_YUYV               3
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
    OutputBuffer(0),
    Format(0)
{
}

svlVidCapSrcV4L2::~svlVidCapSrcV4L2()
{
    Release();
}

svlFilterSourceVideoCapture::PlatformType svlVidCapSrcV4L2::GetPlatformType()
{
    return svlFilterSourceVideoCaptureTypes::LinVideo4Linux2;
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
    Format = new svlFilterSourceVideoCapture::ImageFormat*[NumOfStreams];

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
        Format[i] = 0;
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
    /// \todo(dmirota1)  Consider replacing with glob call
    /** glob_t globbuf;
        glob("/dev/video[0-9]*", NULL, NULL, &globbuf);

        for (i = 0; i < globbuf.gl_pathc; i ++) {
            memcopy(tempname,globbuf.gl_pathv[i],strlen(globbuf.gl_pathv[i]));
            ...
        }
    **/
    counter = 0;
    for (i = 0; i < 16; i ++) {
    
        sprintf(tempname, "/dev/video%d", i /*+ 32*/);
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
                    tempinfo[counter].platform = svlFilterSourceVideoCaptureTypes::LinVideo4Linux2;

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
    memset(&format,0,sizeof(v4l2_format));
//    v4l2_requestbuffers reqbuff;
//    v4l2_buffer buffer;
    char tempname[64];

    for (i = 0; i < NumOfStreams; i ++) {

        // Opening device
        sprintf(tempname, "/dev/video%d", DeviceID[i] /*+ 32*/);
#ifdef __verbose__
        cout << "Trying: " << tempname << endl;
#endif
        DeviceHandle[i] = open(tempname, O_RDWR);
        if (DeviceHandle[i] < 0) goto labError;
#ifdef __verbose__
        cout << "-Open: handle opened" << endl;
#endif

        // QUERYCAP
        if (ioctl(DeviceHandle[i], VIDIOC_QUERYCAP, &devprops) != 0) {
            CMN_LOG_CLASS_INIT_ERROR << "Open: failed to get ioctl VIDIOC_QUERYCAP" << std::endl;
            goto labError;
        }
        if ((devprops.capabilities & V4L2_CAP_STREAMING) != 0) {
            CapMethod[i] = MV4LP_METHOD_STREAMING;
#ifdef __verbose__
            cout << "-Open: QUERYCAP done - Streaming method selected" << endl;
#endif
        }
        /// Prefer Read method when available
        if ((devprops.capabilities & V4L2_CAP_READWRITE) != 0){
            CapMethod[i] = MV4LP_METHOD_READ;
#ifdef __verbose__
            cout << "-Open: QUERYCAP done - Read method selected" << endl;
#endif
        }

        // Setting input
        if (ioctl(DeviceHandle[i], VIDIOC_S_INPUT, &(InputID[i])) != 0) {
            CMN_LOG_CLASS_INIT_ERROR << "Open: failed to set ioctl VIDIOC_S_INPUT" << std::endl;
            goto labError;
        }
#ifdef __verbose__
        cout << "-Open: input set" << endl;
#endif

        // Setting standard
        memset(&standard, 0, sizeof(v4l2_std_id));
        standard = V4L2_STD_NTSC_M;
        if((devprops.capabilities & V4L2_CAP_TUNER) != 0)
            /// \todo(dmirota1)  PAL and NTSC should be options that exposed
            if (ioctl(DeviceHandle[i], VIDIOC_S_STD, &standard) != 0) {
                CMN_LOG_CLASS_INIT_ERROR << "Open: failed to get ioctl VIDIOC_S_STD for NTSC" << std::endl;
                goto labError;
            }
#ifdef __verbose__
        cout << "-Open: standard set" << endl;
#endif

        // Setting format
        format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (ioctl(DeviceHandle[i], VIDIOC_G_FMT, &format) != 0) {
            CMN_LOG_CLASS_INIT_ERROR << "Open: failed to set ioctl VIDIOC_G_FMT" << std::endl;
            goto labError;
        }
        format.fmt.pix.width  = Format[i]->width;
        format.fmt.pix.height = Format[i]->height;
        format.fmt.pix.pixelformat = svlPixelType_to_V4L2_color(Format[i]->colorspace);

        if (Format[i]->colorspace == svlFilterSourceVideoCaptureTypes::PixelYUV422 && Format[i]->yuyv_order) {
            format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
            format.fmt.pix.field = V4L2_FIELD_INTERLACED;
        }
        if (ioctl(DeviceHandle[i], VIDIOC_S_FMT, &format) != 0) {
            CMN_LOG_CLASS_INIT_ERROR << "Open: failed to set ioctl VIDIOC_S_FMT" << std::endl;
            goto labError;
        }

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

        ColorSpace[i] = V4L2_color_to_internal_color(format.fmt.pix.pixelformat);

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

            case MV4LP_CS_YUYV:
                cout << " YUYV";
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
            CMN_LOG_CLASS_INIT_ERROR << "Open: Unsupported image format" << std::endl;
            goto labError;
        }

        if (CapMethod[i] == MV4LP_METHOD_STREAMING) {
            // Streaming I/O
            // Not yet supported, works for Hauppauge cards, but not XtremeRGB cards
            //goto labError;
///*
            struct v4l2_requestbuffers reqbuff;

            // Requesting buffer
            memset(&reqbuff, 0, sizeof(v4l2_requestbuffers));
            reqbuff.count = MV4LP_BUFFER_SIZE_TARGET;
            reqbuff.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            reqbuff.memory = V4L2_MEMORY_MMAP;
            if (ioctl(DeviceHandle[i], VIDIOC_REQBUFS, &reqbuff) != 0) {
                CMN_LOG_CLASS_INIT_ERROR << "Open: failed to set ioctl VIDIOC_REQBUFS" << std::endl;
                goto labError;
            }
            // Buffer count may be overridden by the driver
            if (reqbuff.count < MV4LP_MIN_BUFFER_SIZE) {
                CMN_LOG_CLASS_INIT_ERROR << "Open: invalid required buffer count" << std::endl;
                goto labError;
            }
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

                if (ioctl(DeviceHandle[i], VIDIOC_QUERYBUF, &buffer) != 0) {
                    CMN_LOG_CLASS_INIT_ERROR << "Open: failed to set ioctl VIDIOC_QUERYBUF" << std::endl;
                    goto labError;
                }

                FrameBuffer[i][j].length = buffer.length;
                FrameBuffer[i][j].start = mmap(0,                       // start anywhere
                                               buffer.length,
                                               PROT_READ | PROT_WRITE,  // required
                                               MAP_SHARED,              // recommended
                                               DeviceHandle[i],
                                               buffer.m.offset);

                if (FrameBuffer[i][j].start == MAP_FAILED) {
                    CMN_LOG_CLASS_INIT_ERROR << "Open: frame buffer start failed" << std::endl;
                    goto labError;
                }
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

int svlVidCapSrcV4L2::GetFormatList(unsigned int deviceid, svlFilterSourceVideoCapture::ImageFormat **formatlist)
{
    if (formatlist == 0) return SVL_FAIL;

    /*formatlist[0][0].width = 720;
    formatlist[0][0].height = 480;
    formatlist[0][0].colorspace = svlFilterSourceVideoCapture::PixelYUV422;
    formatlist[0][0].rgb_order = true;
    formatlist[0][0].yuyv_order = false;
    formatlist[0][0].framerate = 30.0;
    formatlist[0][0].custom_mode = -1;*/

    std::vector<svlFilterSourceVideoCapture::ImageFormat *> formats;

    struct v4l2_input input;
    //struct v4l2_standard standard;
    char tempname[64];
    double framerate = 30.0;

    memset (&input, 0, sizeof (input));

    sprintf(tempname, "/dev/video%d", deviceid);
    int fd = open(tempname, O_RDWR);


    if (-1 == ioctl(fd, VIDIOC_G_INPUT, &input.index)) {
        perror ("VIDIOC_G_INPUT");
        exit (EXIT_FAILURE);
    }

    if (-1 == ioctl(fd, VIDIOC_ENUMINPUT, &input)) {
        perror ("VIDIOC_ENUM_INPUT");
        exit (EXIT_FAILURE);
    }

    //printf ("Current input %s supports:\n", input.name);

    v4l2_std_id std_id = 0;
    ioctl(fd, VIDIOC_G_STD, &std_id);
    //memset (&standard, 0, sizeof (standard));

    /*standard.index = 0;
    while (0 == ioctl (fd, VIDIOC_ENUMSTD, &standard)) {
        if (standard.id & input.std){

            printf ("%10s %g", standard.name, standard.frameperiod.denominator/(double)standard.frameperiod.numerator);
            if (standard.id == std_id){
                framerate = standard.frameperiod.denominator/(double)standard.frameperiod.numerator;
                printf (" (Current Setting)");
                //break;
            }

            printf("\n");
        }
        standard.index++;
    }*/

    struct v4l2_format current_format;
    memset (&current_format, 0, sizeof (current_format));
    current_format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl (fd, VIDIOC_G_FMT, &current_format);
    /*fprintf(stdout, "\nCurrent video format:\n");
    fprintf(stdout, "  h:%d x w:%d\n",current_format.fmt.pix.height ,current_format.fmt.pix.width);
    fprintf(stdout, "  field %d\n",current_format.fmt.pix.field);
    fprintf(stdout, "  pixelformat  :%c%c%c%c\n",
            current_format.fmt.pix.pixelformat & 0xFF, (current_format.fmt.pix.pixelformat >> 8) & 0xFF,
            (current_format.fmt.pix.pixelformat >> 16) & 0xFF, (current_format.fmt.pix.pixelformat >> 24) & 0xFF);
    */
    formats.push_back(new svlFilterSourceVideoCapture::ImageFormat);
    memset(formats.back(),0,sizeof(svlFilterSourceVideoCapture::ImageFormat));
    formats.back()->width = current_format.fmt.pix.width;
    formats.back()->height = current_format.fmt.pix.height;
    formats.back()->colorspace = V4L2_color_to_svlPixelType(current_format.fmt.pix.pixelformat);
    formats.back()->rgb_order = true;
    formats.back()->yuyv_order = false;
    if (current_format.fmt.pix.pixelformat == V4L2_PIX_FMT_YUYV) {
        formats.back()->yuyv_order = true;
    }
    formats.back()->framerate = framerate;
    formats.back()->custom_mode = -1;

    struct v4l2_fmtdesc format;
    memset(&format, 0, sizeof (format));
    format.index = 0;

    //char *buf_types[] = {"VIDEO_CAPTURE","VIDEO_OUTPUT", "VIDEO_OVERLAY"}; /* Conversion between enumerated type & english \*/
    //char *flags[] = {"uncompressed", "compressed"};
    //fprintf(stdout, "\nDiscovering supported video formats:\n");

    struct v4l2_frmsizeenum framesize;
    memset(&framesize, 0, sizeof (framesize));
    framesize.index = 0;

    /* For each of the supported v4l2_buf_type buffer types */
    //for (int i = V4L2_BUF_TYPE_VIDEO_CAPTURE; i < V4L2_BUF_TYPE_VIDEO_OVERLAY; i++)
    //{
        format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;//v4l2_buf_type(i);
        /* Send the VIDIOC_ENUM_FM ioctl and print the results */
        while (ioctl(fd, VIDIOC_ENUM_FMT, &format) == 0)
        {

            /* We got a video format/codec back */
            //fprintf(stdout,"VIDIOC_ENUM_FMT(%d, %s)\n", format.index, buf_types[format.type-1]);
            //fprintf(stdout, "  index        :%d\n", format.index);
            //fprintf(stdout, "  type         :%s\n", buf_types[format.type-1]);
            //fprintf(stdout, "  flags        :%s\n", flags[format.flags]);
            //fprintf(stdout, "  description  :%s\n", format.description);

            /* Convert the pixelformat attributes from FourCC into 'human readable' format */
            //fprintf(stdout, "  pixelformat  :%c%c%c%c\n",
            //       format.pixelformat & 0xFF, (format.pixelformat >> 8) & 0xFF,
            //        (format.pixelformat >> 16) & 0xFF, (format.pixelformat >> 24) & 0xFF);

            /* Increment the index */
            format.index++;

            if (V4L2_color_to_svlPixelType(format.pixelformat) != svlFilterSourceVideoCaptureTypes::PixelUnknown) {
                formats.push_back(new svlFilterSourceVideoCapture::ImageFormat);
                memset(formats.back(),0,sizeof(svlFilterSourceVideoCapture::ImageFormat));
                formats.back()->width = current_format.fmt.pix.width;
                formats.back()->height = current_format.fmt.pix.height;
                formats.back()->colorspace = V4L2_color_to_svlPixelType(format.pixelformat);
                formats.back()->rgb_order = true;
                formats.back()->yuyv_order = false;
                if (current_format.fmt.pix.pixelformat == V4L2_PIX_FMT_YUYV) {
                    formats.back()->yuyv_order = true;
                }
                formats.back()->framerate = framerate;
                formats.back()->custom_mode = -1;
            }

            /*framesize.index = 0;
            framesize.pixel_format = format.pixelformat;
            fprintf(stdout, "  sizes  :\n");
            while( ioctl( fd, VIDIOC_ENUM_FRAMESIZES, &framesize ) == 0 )
            {
                if (framesize.type == V4L2_FRMSIZE_TYPE_DISCRETE){
                 fprintf(stdout, "  h:%dxw:%d\n",framesize.discrete.height ,framesize.discrete.width);
                 framesize.index++;
                }else if(framesize.type == V4L2_FRMSIZE_TYPE_STEPWISE || framesize.type == V4L2_FRMSIZE_TYPE_CONTINUOUS){
                    fprintf(stdout, "  max h:%d x max w:%d\n",framesize.stepwise.max_height ,framesize.stepwise.max_width);
                    break;
                }else{
                    break;
                }
            }*/

        }
    //}

    /* EINVAL indicates the end of the enumeration, which cannot be
       empty unless this device falls under the USB exception. */

    /*if (errno != EINVAL || standard.index == 0) {
        perror ("VIDIOC_ENUMSTD");
        exit (EXIT_FAILURE);
    }*/

    close(fd);


    formatlist[0] = new svlFilterSourceVideoCapture::ImageFormat[0+formats.size()];

    memset(&formatlist[0][0], 0, sizeof(svlFilterSourceVideoCapture::ImageFormat));

    GetFormat(formatlist[0][0]);

    for (unsigned int i = 0; i < formats.size();i++) {
        memcpy(&formatlist[0][i+0], formats.at(i), sizeof(svlFilterSourceVideoCapture::ImageFormat));
        delete formats.at(i);
        formats.at(i) = NULL;
    }


    return 0+formats.size();
}

int svlVidCapSrcV4L2::GetFormat(svlFilterSourceVideoCapture::ImageFormat& format, unsigned int CMN_UNUSED(videoch))
{
    //if (videoch >= NumOfStreams) return SVL_FAIL;

    format.width = 720;
    format.height = 480;
    format.colorspace = svlFilterSourceVideoCaptureTypes::PixelRGB8;
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
    unsigned char *imbuf = NULL;

    //if(OutputBuffer[videoch]->GetTimestamp() < 0)
    //   return SVL_FAIL;

    try {
        imbuf = OutputBuffer[videoch]->GetPushBuffer(imlen);
    } catch( ... ) {
        try {
            if (OutputBuffer) delete [] OutputBuffer;
        } catch ( ... ) {

        }
        OutputBuffer = new svlBufferImage*[NumOfStreams];
        for (unsigned int i = 0; i < NumOfStreams; i ++) {
            OutputBuffer[i] = new svlBufferImage(CapWidth[i], CapHeight[i]);
        }
        imbuf = OutputBuffer[videoch]->GetPushBuffer(imlen);
    }
    if (imbuf == NULL) return SVL_FAIL;

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
        if (ColorSpace[videoch] == MV4LP_CS_YUYV) {
            framesize = ystride * h * 2;
        } // 16 bpp

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
        if (ColorSpace[videoch] == MV4LP_CS_YUYV) {
            // Convert YUYV to BGR24
            svlConverter::YUV422toRGB24(buf1, imbuf, w*h, true, true, true);
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

    if (Format) {
        for (unsigned int i = 0; i < NumOfStreams; i ++) {
            if (Format[i]) delete Format[i];
        }
        delete [] Format;
    }

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
    memset(&input,0,sizeof(v4l2_input));
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


int svlVidCapSrcV4L2::SetFormat(svlFilterSourceVideoCapture::ImageFormat& format, unsigned int videoch)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "SetFormat: " << format.colorspace << " " << format.yuyv_order << std::endl;
    if (videoch >= NumOfStreams || Initialized) return SVL_FAIL;
    if (Format[videoch] == 0) Format[videoch] = new svlFilterSourceVideoCapture::ImageFormat;
    memcpy(Format[videoch], &format, sizeof(svlFilterSourceVideoCapture::ImageFormat));

    return SVL_OK;
}


int svlVidCapSrcV4L2::V4L2_color_to_internal_color(int color_in){

    if (color_in == V4L2_PIX_FMT_BGR24) {
        // Using BGR24
        return MV4LP_CS_BGR24;
    }
    /*else if (color_in == V4L2_PIX_FMT_MPEG) {
        // Using MPEG
        return MV4LP_CS_MPEG;
    }*/
    else if (color_in == V4L2_PIX_FMT_HM12) {
        // Using HM12
        return MV4LP_CS_HM12;
    }
    else if (color_in == V4L2_PIX_FMT_UYVY) {
        // Using UYVY
        return MV4LP_CS_UYVY;
    }
    else if (color_in == V4L2_PIX_FMT_YUYV) {
        // Using YUYV                                                                                                                                          
        return MV4LP_CS_YUYV;
    }
    else {
        // Unsupported format
        return MV4LP_CS_UNKNOWN;
    }
}


int svlVidCapSrcV4L2::svlPixelType_to_V4L2_color(svlFilterSourceVideoCapture::PixelType color_in)
{
    if (color_in == svlFilterSourceVideoCaptureTypes::PixelRGB8) {
        // Using BGR24
        return V4L2_PIX_FMT_BGR24;
    }
    else if (color_in == svlFilterSourceVideoCaptureTypes::PixelYUV422) {
        // Using UYVY
        return V4L2_PIX_FMT_UYVY;
    }
    else {
        // Unsupported format
        return -1;
    }

}

svlFilterSourceVideoCapture::PixelType svlVidCapSrcV4L2::V4L2_color_to_svlPixelType(int color_in)
{
    if (color_in == V4L2_PIX_FMT_BGR24) {
        // Using BGR24
        return svlFilterSourceVideoCaptureTypes::PixelRGB8;
    }
    else if (color_in == V4L2_PIX_FMT_UYVY) {
        // Using UYVY
        return svlFilterSourceVideoCaptureTypes::PixelYUV422;
    }
    else if (color_in == V4L2_PIX_FMT_YUYV) {
        // Using UYVY
        return svlFilterSourceVideoCaptureTypes::PixelYUV422;
    }
    else {
        // Unsupported format
        return svlFilterSourceVideoCaptureTypes::PixelUnknown;
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
