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

#ifndef _svlVidCapSrcV4L2_h
#define _svlVidCapSrcV4L2_h

#include <cisstStereoVision/svlFilterSourceVideoCapture.h>

class svlBufferImage;
class osaThread;
class svlVidCapSrcV4L2Thread;

class svlVidCapSrcV4L2 : public svlVidCapSrcBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

friend class svlVidCapSrcV4L2Thread;

    typedef struct _FrameBufferType {
        void *start;
        int length;
    } FrameBufferType;

public:
    svlVidCapSrcV4L2();
    ~svlVidCapSrcV4L2();

public:
    svlFilterSourceVideoCapture::PlatformType GetPlatformType();
    int SetStreamCount(unsigned int numofstreams);
    int GetDeviceList(svlFilterSourceVideoCapture::DeviceInfo **deviceinfo);
    int Open();
    void Close();
    int Start();
    svlImageRGB* GetLatestFrame(bool waitfornew, unsigned int videoch = 0);
    int Stop();
    bool IsRunning();
    int SetDevice(int devid, int inid, unsigned int videoch = 0);
    int GetWidth(unsigned int videoch = 0);
    int GetHeight(unsigned int videoch = 0);

    int GetFormatList(unsigned int deviceid, svlFilterSourceVideoCapture::ImageFormat **formatlist);
    int GetFormat(svlFilterSourceVideoCapture::ImageFormat& format, unsigned int videoch = 0);
    int SetFormat(svlFilterSourceVideoCapture::ImageFormat& format, unsigned int videoch = 0);

private:
    unsigned int NumOfStreams;
    bool Initialized;
    bool Running;

    svlVidCapSrcV4L2Thread** CaptureProc;
    osaThread** CaptureThread;
    int* DeviceID;
    int* InputID;
    int* CapStride;
    int* CapWidth;
    int* CapHeight;
    int* DeviceHandle;
    int* CapMethod;
    int* ColorSpace;
    int* FrameBufferSize;
    FrameBufferType** FrameBuffer;
    svlBufferImage** OutputBuffer;
    svlFilterSourceVideoCapture::ImageFormat** Format;

    int ReadFrame(unsigned int videoch);

    void Release();
    int GetDeviceInputs(int fd, svlFilterSourceVideoCapture::DeviceInfo *deviceinfo);
    void HM12_de_macro_y(unsigned char* dst, unsigned char* src, int dstride, const int w, const int h);
    void HM12_de_macro_uv(unsigned char* dstu, unsigned char* dstv, unsigned char* src, int dstride, const int w, const int h);
    void YUV420p_to_BGR24(unsigned char* dst, unsigned char* src, int dststride, int srcstride, const int w, const int h);

    int V4L2_color_to_internal_color(int color_in);
    int svlPixelType_to_V4L2_color(svlFilterSourceVideoCapture::PixelType color_in);
    svlFilterSourceVideoCapture::PixelType V4L2_color_to_svlPixelType(int color_in);
};


class svlVidCapSrcV4L2Thread
{
public:
    svlVidCapSrcV4L2Thread(int streamid) { StreamID = streamid; InitSuccess = false; }
    ~svlVidCapSrcV4L2Thread() {}
    void* Proc(svlVidCapSrcV4L2* baseref);

    bool WaitForInit() { InitEvent.Wait(); return InitSuccess; }
    bool IsError() { return Error; }

private:
    int StreamID;
    bool Error;
    osaThreadSignal InitEvent;
    bool InitSuccess;
};

CMN_DECLARE_SERVICES_INSTANTIATION(svlVidCapSrcV4L2)

#endif // _svlVidCapSrcV4L2_h

