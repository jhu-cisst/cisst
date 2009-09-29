/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vidOCVSource.h 618 2009-07-31 16:39:42Z bvagvol1 $
  
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

#ifndef _vidOCVSource_h
#define _vidOCVSource_h

#include <cisstStereoVision/svlFilterSourceVideoCapture.h>
#include "svlImageBuffer.h"


class COpenCVSourceThread;

class COpenCVSource : public CVideoCaptureSourceBase
{
friend class COpenCVSourceThread;

public:
	COpenCVSource();
	~COpenCVSource();

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

    COpenCVSourceThread** CaptureProc;
    osaThread** CaptureThread;
	int* DeviceID;
    int* OCVDeviceID;
    int OCVNumberOfDevices;
    int* OCVWidth;
    int* OCVHeight;
    CvCapture** OCVCapture;
    svlImageBuffer** ImageBuffer;

    void Release();
};


class COpenCVSourceThread
{
public:
    COpenCVSourceThread(int streamid) { StreamID = streamid; InitSuccess = false; }
    ~COpenCVSourceThread() {}
    void* Proc(COpenCVSource* baseref);

    bool WaitForInit() { InitEvent.Wait(); return InitSuccess; }
    bool IsError() { return Error; }

private:
    int StreamID;
    bool Error;
    osaThreadSignal InitEvent;
    bool InitSuccess;
    IplImage *Frame;
};


#endif // _vidOCVSource_h

