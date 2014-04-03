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

#ifndef _svlVidCapSrcSVS_h
#define _svlVidCapSrcSVS_h

#include <cisstStereoVision/svlFilterSourceVideoCapture.h>
#include <cisstStereoVision/svlBufferImage.h>
#include <svsclass.h>


class svlBufferImage;
class osaThread;
class svlVidCapSrcSVSThread;

class svlVidCapSrcSVS : public svlVidCapSrcBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

friend class svlVidCapSrcSVSThread;

public:
    svlVidCapSrcSVS();
    ~svlVidCapSrcSVS();

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
	bool Running;

    svlVidCapSrcSVSThread* CaptureProc;
    osaThread* CaptureThread;
    svsVideoImages* SVSObj;
    svsStereoImage* SVSImage;
	int DeviceID[2];
    svlBufferImage** OutputBuffer;

    int CaptureFrame();
    void ConvertRGB32toRGB24(unsigned char* source, unsigned char* dest, const int pixcount);
};


class svlVidCapSrcSVSThread
{
public:
    svlVidCapSrcSVSThread() { InitSuccess = false; }
    ~svlVidCapSrcSVSThread() {}
    void* Proc(svlVidCapSrcSVS* baseref);

    bool WaitForInit() { InitEvent.Wait(); return InitSuccess; }
    bool IsError() { return Error; }

private:
    bool Error;
    osaThreadSignal InitEvent;
    bool InitSuccess;
};

CMN_DECLARE_SERVICES_INSTANTIATION(svlVidCapSrcSVS)

#endif // _svlVidCapSrcSVS_h

