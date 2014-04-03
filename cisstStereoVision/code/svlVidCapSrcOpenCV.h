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

#ifndef _svlVidCapSrcOpenCV_h
#define _svlVidCapSrcOpenCV_h

#include <cisstStereoVision/svlFilterSourceVideoCapture.h>


class svlBufferImage;
class osaThread;
class svlVidCapSrcOpenCVThread;

class svlVidCapSrcOpenCV : public svlVidCapSrcBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

friend class svlVidCapSrcOpenCVThread;

public:
	svlVidCapSrcOpenCV();
	~svlVidCapSrcOpenCV();

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

    svlVidCapSrcOpenCVThread** CaptureProc;
    osaThread** CaptureThread;
	int* DeviceID;
    int* OCVDeviceID;
    int OCVNumberOfDevices;
    int* OCVWidth;
    int* OCVHeight;
    cv::VideoCapture** OCVCapture;
    svlBufferImage** ImageBuffer;

    void Release();
};


class svlVidCapSrcOpenCVThread
{
public:
    svlVidCapSrcOpenCVThread(int streamid) { StreamID = streamid; InitSuccess = false; }
    ~svlVidCapSrcOpenCVThread() {}
    void* Proc(svlVidCapSrcOpenCV* baseref);

    bool WaitForInit() { InitEvent.Wait(); return InitSuccess; }
    bool IsError() { return Error; }

private:
    int StreamID;
    bool Error;
    osaThreadSignal InitEvent;
    bool InitSuccess;
    IplImage *Frame;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlVidCapSrcOpenCV)

#endif // _svlVidCapSrcOpenCV_h

