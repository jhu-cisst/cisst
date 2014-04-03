/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
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

#ifndef _svlVidCapSrcMIL_h
#define _svlVidCapSrcMIL_h

#include <cisstStereoVision/svlFilterSourceVideoCapture.h>
#include <cisstStereoVision/svlRenderTargets.h>
#include <mil.h>

// For backwards compatibility
#if M_MIL_CURRENT_INT_VERSION < 0x0900
    typedef long    MIL_INT;
    typedef long    MIL_INT32;
#endif

#define MIL_MAX_SYS     2
#define MIL_MAX_DIG     64

class osaThread;
class svlBufferImage;

class svlVidCapSrcMILRenderTarget : public svlRenderTargetBase
{
friend class svlRenderTargets;

protected:
    svlVidCapSrcMILRenderTarget(unsigned int deviceID, unsigned int displayID = 0);
    ~svlVidCapSrcMILRenderTarget();

public:
    bool SetImage(unsigned char* buffer, int offsetx, int offsety, bool vflip);
    unsigned int GetWidth();
    unsigned int GetHeight();

    void* ThreadProc(void* param);

private:
    int SystemID;
    int DigitizerID;

    osaThread* Thread;
    osaThreadSignal NewFrameSignal;
    osaThreadSignal ThreadReadySignal;
    bool TransferSuccessful;
    bool KillThread;
    bool ThreadKilled;

    void TranslateImage(unsigned char* src, unsigned char* dest, const int width, const int height, const int trhoriz, const int trvert, bool vflip);
};


class svlVidCapSrcMIL : public svlVidCapSrcBase
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

friend class svlVidCapSrcMILRenderTarget;

public:
    typedef struct tagMILCaptureParameters {
        int SystemID;
        int DigitizerID;
        MIL_ID *MilFrames;
        svlBufferImage *ImageBuffer;
        bool OverlayModified;
        MIL_ID *MilOverlayImage;
        unsigned char *MilOverlayBuffer;
    } MILCaptureParameters;

private:
	svlVidCapSrcMIL();
	~svlVidCapSrcMIL();

public:
    static svlVidCapSrcMIL* GetInstance();

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

    bool IsCaptureSupported(unsigned int sysid, unsigned int digid = 0);
    bool IsOverlaySupported(unsigned int sysid, unsigned int digid = 0);

private:
    unsigned int NumOfStreams;
    bool Initialized;
    bool Running;

    vctDynamicVector<int> SystemID;
    vctDynamicVector<int> DigitizerID;
    vctDynamicVector<svlBufferImage*> ImageBuffer;

    unsigned int MILNumberOfSystems;
    unsigned int MilNumberOfDigitizers[MIL_MAX_SYS];
    unsigned int MilCaptureBuffers;

    MIL_ID MilApplication;
    MIL_ID MilSystem[MIL_MAX_SYS];
    MIL_ID MilDigitizer[MIL_MAX_SYS][MIL_MAX_DIG];
    MIL_ID MilDisplay[MIL_MAX_SYS][MIL_MAX_DIG];
    long MilWidth[MIL_MAX_SYS][MIL_MAX_DIG];
    long MilHeight[MIL_MAX_SYS][MIL_MAX_DIG];
    long MilBands[MIL_MAX_SYS][MIL_MAX_DIG];
    long MilBandBits[MIL_MAX_SYS][MIL_MAX_DIG];
    MIL_ID MilDisplayImage[MIL_MAX_SYS][MIL_MAX_DIG];
    MIL_ID MilOverlayImage[MIL_MAX_SYS][MIL_MAX_DIG];
    unsigned char* MilOverlayBuffer[MIL_MAX_SYS][MIL_MAX_DIG];
    MILCaptureParameters MilCaptureParams[MIL_MAX_SYS][MIL_MAX_DIG];
    bool MilCaptureSupported[MIL_MAX_SYS][MIL_MAX_DIG];
    bool MilOverlaySupported[MIL_MAX_SYS][MIL_MAX_DIG];

    bool MILInitializeApplication();
    bool MILInitializeSystem(int system);
    bool MILInitializeDigitizer(int system, int digitizer);
    bool MILInitializeCapture(int system, int digitizer);
    bool MILInitializeOverlay(int system, int digitizer);
    bool MILUploadOverlay(int system, int digitizer);
    void MILReleaseOverlay(int system, int digitizer);
    void MILReleaseCapture(int system, int digitizer);
    void MILReleaseDigitizer(int system, int digitizer);
    void MILReleaseSystem(int system);
    void MILReleaseApplication();
};

CMN_DECLARE_SERVICES_INSTANTIATION(svlVidCapSrcMIL)

#endif // _svlVidCapSrcMIL_h

