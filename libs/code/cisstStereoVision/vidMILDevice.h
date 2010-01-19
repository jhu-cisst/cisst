/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2009 

  (C) Copyright 2006-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _vidMILDevice_h
#define _vidMILDevice_h

#include <cisstStereoVision/svlFilterSourceVideoCapture.h>
#include <cisstStereoVision/svlRenderTargets.h>
#include "svlImageBuffer.h"

#include <mil.h>


class CMILDeviceRenderTarget : public svlRenderTargetBase
{
friend class svlRenderTargets;

protected:
    CMILDeviceRenderTarget(unsigned int deviceID);
    ~CMILDeviceRenderTarget();

public:
    bool SetImage(unsigned char* buffer, int offsetx, int offsety, bool vflip);
    unsigned int GetWidth();
    unsigned int GetHeight();

    void* ThreadProc(void* param);

private:
    int DeviceID;

    osaThread* Thread;
    osaThreadSignal NewFrameSignal;
    osaThreadSignal ThreadReadySignal;
    bool TransferSuccessful;
    bool KillThread;
    bool ThreadKilled;

    void TranslateImage(unsigned char* src, unsigned char* dest, const int width, const int height, const int trhoriz, const int trvert, bool vflip);
};


class CMILDevice : public CVideoCaptureSourceBase, public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

friend class CMILDeviceRenderTarget;

public:
    typedef struct tagMILCaptureParameters {
        MIL_ID *MilFrames;
        svlImageBuffer *ImageBuffer;
        bool OverlayModified;
        MIL_ID *MilOverlayImage;
        unsigned char *MilOverlayBuffer;
    } MILCaptureParameters;

private:
	CMILDevice();
	~CMILDevice();

public:
    static CMILDevice* GetInstance();
    void ReleaseAll();

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
    void Release();

    bool IsCaptureSupported(int devid);
    bool IsOverlaySupported(int devid);
    bool EnableCapture(int devid, bool enable = true);
    bool EnableOverlay(int devid, bool enable = true);

private:
    unsigned int NumOfStreams;
    bool Initialized;
    bool Running;

    int* DeviceID;
    bool CaptureEnabled[2];
    bool OverlayEnabled[2];
    bool CaptureSupported[2];
    bool OverlaySupported[2];
    int Width[2];
    int Height[2];
    svlImageBuffer** ImageBuffer;

    int MILNumberOfDevices;
    MIL_ID MilApplication;
    MIL_ID MilSystem[2];
    MIL_ID MilDisplay[2];
    MIL_ID MilDigitizer[2];
    MIL_ID MilDisplayImage[2];
    MIL_ID MilOverlayImage[2];
    MIL_INT MilDeviceID[2];
    bool MilDeviceInitialized[2];
    bool MilCaptureEnabled[2];
    bool MilOverlayEnabled[2];
    long MilWidth[2];
    long MilHeight[2];
    long MilBands[2];
    long MilBandBits[2];
    unsigned char *MilOverlayBuffer[2];
    unsigned int MilCaptureBuffers;
    MILCaptureParameters MilCaptureParams[2];

    bool MILInitializeApplication();
    bool MILInitializeDevice(int device, bool capture, bool overlay, int& width, int& height, int& bands);
    bool MILUploadOverlay(int device);
    void MILReleaseDevice(int device);
    void MILReleaseApplication();
};

CMN_DECLARE_SERVICES_INSTANTIATION(CMILDevice)

#endif // _vidMILDevice_h

