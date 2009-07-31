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

#ifndef _vidV4L2Source_h
#define _vidV4L2Source_h

#include <cisstStereoVision/svlFilterSourceVideoCapture.h>
#include "svlImageBuffer.h"


class CV4L2SourceThread;

class CV4L2Source : public CVideoCaptureSourceBase
{
friend class CV4L2SourceThread;

    typedef struct _FrameBufferType {
        void *start;
        int length;
    } FrameBufferType;

public:
    CV4L2Source();
    ~CV4L2Source();

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

private:
    unsigned int NumOfStreams;
    bool Initialized;
    bool Running;

    CV4L2SourceThread** CaptureProc;
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
    svlImageBuffer** OutputBuffer;

    int ReadFrame(unsigned int videoch);

    void Release();
    int GetDeviceInputs(int fd, svlFilterSourceVideoCapture::DeviceInfo *deviceinfo);
    void HM12_de_macro_y(unsigned char* dst, unsigned char* src, int dstride, const int w, const int h);
    void HM12_de_macro_uv(unsigned char* dstu, unsigned char* dstv, unsigned char* src, int dstride, const int w, const int h);
    void YUV420p_to_BGR24(unsigned char* dst, unsigned char* src, int dststride, int srcstride, const int w, const int h);
};


class CV4L2SourceThread
{
public:
    CV4L2SourceThread(int streamid) { StreamID = streamid; InitSuccess = false; }
    ~CV4L2SourceThread() {}
    void* Proc(CV4L2Source* baseref);

    bool WaitForInit() { InitEvent.Wait(); return InitSuccess; }
    bool IsError() { return Error; }

private:
    int StreamID;
    bool Error;
    osaThreadSignal InitEvent;
    bool InitSuccess;
};


#endif // _vidV4L2Source_h

