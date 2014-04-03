/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Balazs Vagvolgyi
  Created on: 2009 

  (C) Copyright 2006-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include "svlVidCapSrcMIL.h"
#include <cisstOSAbstraction/osaThread.h>
#include <cisstStereoVision/svlBufferImage.h>
#include <cisstOSAbstraction/osaSleep.h>

#define __VERBOSE__  0


/******************************************/
/*** svlVidCapSrcMILRenderTarget class ****/
/******************************************/

svlVidCapSrcMILRenderTarget::svlVidCapSrcMILRenderTarget(unsigned int deviceID, unsigned int displayID) :
    svlRenderTargetBase(),
    Thread(0),
    TransferSuccessful(true),
    KillThread(false),
    ThreadKilled(true)
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMILRenderTarget::constructor()" << std::endl;
#endif

    SystemID = deviceID;
    DigitizerID = displayID;
    if (SystemID >= MIL_MAX_SYS) SystemID = 0;
    if (DigitizerID >= MIL_MAX_DIG) DigitizerID = 0;

    // Try to initialize overlay module
    svlVidCapSrcMIL *device = svlVidCapSrcMIL::GetInstance();
    if (!device->MILInitializeSystem(SystemID) ||
        !device->MILInitializeDigitizer(SystemID, DigitizerID) ||
        !device->MILInitializeOverlay(SystemID, DigitizerID)) {
        std::cerr << "svlVidCapSrcMILRenderTarget::constructor - could not initialize overlay (System: M_DEV"
                  << deviceID << ", Digitizer: M_DEV" << displayID << ")" << std::endl;
    }

    // Start up overlay thread
    Thread = new osaThread;
    Thread->Create<svlVidCapSrcMILRenderTarget, void*>(this, &svlVidCapSrcMILRenderTarget::ThreadProc, 0);
    if (ThreadReadySignal.Wait(1.0) && ThreadKilled == false) {
        ThreadReadySignal.Raise();
    }
    else {
        // If it takes longer than 1 sec, don't execute
        KillThread = true;
    }
}

svlVidCapSrcMILRenderTarget::~svlVidCapSrcMILRenderTarget()
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMILRenderTarget::destructor()" << std::endl;
#endif

    KillThread = true;
    if (ThreadKilled == false) Thread->Wait();
    delete Thread;

    svlVidCapSrcMIL::GetInstance()->MILReleaseOverlay(SystemID, DigitizerID);
}

bool svlVidCapSrcMILRenderTarget::SetImage(unsigned char* buffer, int offsetx, int offsety, bool vflip)
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMILRenderTarget::SetImage()" << std::endl;
#endif

    if (SystemID < 0) return false;

    // Wait for thread to finish previous transfer
    if (ThreadReadySignal.Wait(2.0) == false || TransferSuccessful == false || KillThread || ThreadKilled) {
        // Something went terribly wrong on the thread
        return false;
    }

    // Copy image to the Matrox buffer with translation and flip...
    svlVidCapSrcMIL *device = svlVidCapSrcMIL::GetInstance();
    TranslateImage(buffer,
                   device->MilOverlayBuffer[SystemID][DigitizerID],
                   device->MilWidth[SystemID][DigitizerID] * 3,
                   device->MilHeight[SystemID][DigitizerID],
                   offsetx * 3,
                   offsety,
                   vflip);

    // Signal Thread that there is a new frame to transfer
    NewFrameSignal.Raise();

    // Frame successfully filed for transfer
    return true;
}

unsigned int svlVidCapSrcMILRenderTarget::GetWidth()
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMILRenderTarget::GetWidth()" << std::endl;
#endif

    if (SystemID < 0) return 0;
    return svlVidCapSrcMIL::GetInstance()->MilWidth[SystemID][DigitizerID];
}

unsigned int svlVidCapSrcMILRenderTarget::GetHeight()
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMILRenderTarget::GetHeight()" << std::endl;
#endif

    if (SystemID < 0) return 0;
    return svlVidCapSrcMIL::GetInstance()->MilHeight[SystemID][DigitizerID];
}

void* svlVidCapSrcMILRenderTarget::ThreadProc(void* CMN_UNUSED(param))
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMILRenderTarget::ThreadProc()" << std::endl;
#endif

    ThreadKilled = false;
    ThreadReadySignal.Raise();

    while (!KillThread) {
        if (NewFrameSignal.Wait(0.5)) {
            TransferSuccessful = svlVidCapSrcMIL::GetInstance()->MILUploadOverlay(SystemID, DigitizerID);
            ThreadReadySignal.Raise();
        }
    }

    // Release waiting threads (if any)
    ThreadReadySignal.Raise();

    ThreadKilled = true;
    return this;
}

void svlVidCapSrcMILRenderTarget::TranslateImage(unsigned char* src, unsigned char* dest, const int width, const int height, const int trhoriz, const int trvert, bool vflip)
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMILRenderTarget::TranslateImage()" << std::endl;
#endif

    int abs_h = std::abs(trhoriz);
    int abs_v = std::abs(trvert);

    if (vflip) {
        if (width <= abs_h || height <= abs_v) {
            src += width * (height - 1);
            for (int j = 0; j < height; j ++) {
                memcpy(dest, src, width);
                src -= width;
                dest += width;
            }
            return;
        }

        int linecopysize = width - abs_h;
        int xfrom = std::max(0, trhoriz);
        int yfrom = std::max(0, trvert);
        int yto = height + std::min(0, trvert);
        int copyxoffset = std::max(0, -trhoriz);
        int copyyoffset = std::max(0, -trvert);

        if (trhoriz == 0) {
            src += width * (height - copyyoffset - 1);
            dest += width * yfrom;
            for (int j = height - abs_v - 1; j >= 0; j --) {
                memcpy(dest, src, width);
                src -= width;
                dest += width;
            }
            return;
        }

        src += width * (height - copyyoffset - 1) + copyxoffset;
        dest += width * yfrom + xfrom;
        for (int j = yfrom; j < yto; j ++) {
            memcpy(dest, src, linecopysize);
            src -= width;
            dest += width;
        }
    }
    else {
        if (width <= abs_h || height <= abs_v) {
            memset(dest, 0, width * height);
            return;
        }

        if (trhoriz == 0) {
            memcpy(dest + std::max(0, trvert) * width,
                   src + std::max(0, -trvert) * width,
                   width * (height - abs_v));
            return;
        }

        int linecopysize = width - abs_h;
        int xfrom = std::max(0, trhoriz);
        int yfrom = std::max(0, trvert);
        int yto = height + std::min(0, trvert);
        int copyxoffset = std::max(0, -trhoriz);
        int copyyoffset = std::max(0, -trvert);

        src += width * copyyoffset + copyxoffset;
        dest += width * yfrom + xfrom;
        for (int j = yfrom; j < yto; j ++) {
            memcpy(dest, src, linecopysize);
            src += width;
            dest += width;
        }
    }
}


/***************************/
/*** Function prototypes ***/
/***************************/

MIL_INT MFTYPE MILProcessingCallback(MIL_INT HookType, MIL_ID HookId, void MPTYPE *HookDataPtr);


/*************************************/
/*** svlVidCapSrcMIL class ***********/
/*************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlVidCapSrcMIL, svlVidCapSrcBase)

svlVidCapSrcMIL::svlVidCapSrcMIL() :
    svlVidCapSrcBase(),
    NumOfStreams(0),
    Initialized(false),
    Running(false),
    MilCaptureBuffers(3)
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::constructor() - MIL version=" << M_MIL_CURRENT_ASCII_VERSION << std::endl;
#endif

    MilApplication = M_NULL;
    MILNumberOfSystems = 0;

    for (unsigned int sys = 0; sys < MIL_MAX_SYS; sys ++) {
        MilSystem[sys] = M_NULL;
        MilNumberOfDigitizers[sys] = 0;

        for (unsigned int dig = 0; dig < MIL_MAX_DIG; dig ++) {
            MilDisplay[sys][dig] = M_NULL;
            MilDigitizer[sys][dig] = M_NULL;
            MilWidth[sys][dig] = 0;
            MilHeight[sys][dig] = 0;
            MilBands[sys][dig] = 0;
            MilBandBits[sys][dig] = 0;
            MilDisplayImage[sys][dig] = M_NULL;
            MilOverlayImage[sys][dig] = M_NULL;
            MilOverlayBuffer[sys][dig] = 0;
            MilCaptureParams[sys][dig].SystemID = 0;
            MilCaptureParams[sys][dig].DigitizerID = 0;
            MilCaptureParams[sys][dig].MilFrames = 0;
            MilCaptureParams[sys][dig].ImageBuffer = 0;
            MilCaptureParams[sys][dig].OverlayModified = false;
            MilCaptureParams[sys][dig].MilOverlayImage = 0;
            MilCaptureParams[sys][dig].MilOverlayBuffer = 0;
            MilCaptureSupported[sys][dig] = false;
            MilOverlaySupported[sys][dig] = false;
        }
    }

    MILInitializeApplication();
/*
    SetStreamCount(1);

    // Enumerate devices
    svlFilterSourceVideoCapture::DeviceInfo *deviceinfolist = 0;
    GetDeviceList(&deviceinfolist);
    delete [] deviceinfolist;
*/
}

svlVidCapSrcMIL::~svlVidCapSrcMIL()
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::destructor()" << std::endl;
#endif

    Release();
    MILReleaseApplication();
}

svlVidCapSrcMIL* svlVidCapSrcMIL::GetInstance()
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::GetInstance()" << std::endl;
#endif

    static svlVidCapSrcMIL instance;
    return &instance;
}

svlFilterSourceVideoCapture::PlatformType svlVidCapSrcMIL::GetPlatformType()
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::GetPlatformType()" << std::endl;
#endif

    return svlFilterSourceVideoCaptureTypes::MatroxImaging;
}

int svlVidCapSrcMIL::SetStreamCount(unsigned int numofstreams)
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::SetStreamCount(" << numofstreams << ")" << std::endl;
#endif

    if (numofstreams < 1) return SVL_FAIL;

    Release();

    NumOfStreams = numofstreams;

    SystemID.SetSize(NumOfStreams);
    DigitizerID.SetSize(NumOfStreams);
    ImageBuffer.SetSize(NumOfStreams);

    for (unsigned int i = 0; i < NumOfStreams; i ++) {
        SystemID[i] = -1;
        DigitizerID[i] = -1;
        ImageBuffer[i] = 0;
    }

    return SVL_OK;
}

int svlVidCapSrcMIL::GetDeviceList(svlFilterSourceVideoCapture::DeviceInfo **deviceinfo)
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::GetDeviceList(...)" << std::endl;
#endif

    if (deviceinfo == 0 || Initialized) return SVL_FAIL;

    unsigned int sys, dig, digitizers;
    bool cap, ovrl;

    MILNumberOfSystems = 0;
    for (sys = 0; sys < MIL_MAX_SYS; sys ++) {
        if (!MILInitializeSystem(sys)) break;
        MILNumberOfSystems ++;

        for (dig = 0; dig < MilNumberOfDigitizers[sys]; dig ++) {
            if (MILInitializeDigitizer(sys, dig)) {
                if (MILInitializeCapture(sys, dig)) MILReleaseCapture(sys, dig);
                if (MILInitializeOverlay(sys, dig)) MILReleaseOverlay(sys, dig);
                MILReleaseDigitizer(sys, dig);
            }
        }
        MILReleaseSystem(sys);
    }

    // Allocate memory for device info array
    // CALLER HAS TO FREE UP THIS ARRAY!!!
    if (MILNumberOfSystems > 0) {

        deviceinfo[0] = new svlFilterSourceVideoCapture::DeviceInfo[MILNumberOfSystems];

        for (sys = 0; sys < MILNumberOfSystems; sys ++) {
            // platform
            deviceinfo[0][sys].platform = svlFilterSourceVideoCaptureTypes::MatroxImaging;

            // id
            deviceinfo[0][sys].ID = sys;

            // name
            std::stringstream dev_name;
            dev_name << "Matrox Imaging Device (" << "M_DEV" << sys << ")";

            memset(deviceinfo[0][sys].name, 0, SVL_VCS_STRING_LENGTH);
            memcpy(deviceinfo[0][sys].name,
                   dev_name.str().c_str(),
                   std::min(SVL_VCS_STRING_LENGTH - 1, static_cast<int>(dev_name.str().length())));

            // digitizers
            digitizers = MilNumberOfDigitizers[sys];
            if (digitizers > SVL_VCS_ARRAY_LENGTH) digitizers = SVL_VCS_ARRAY_LENGTH;
            deviceinfo[0][sys].inputcount = digitizers;
            deviceinfo[0][sys].activeinput = 0;

            for (dig = 0; dig < digitizers; dig ++) {
                std::stringstream in_name;
                in_name << "Digitizer #" << dig << " (M_DEV" << dig << ": ";

                cap = MilCaptureSupported[sys][dig];
                ovrl = MilOverlaySupported[sys][dig];
                if (cap && ovrl) in_name << "Capture+Overlay)";
                else if (cap) in_name << "Capture only)";
                else if (ovrl) in_name << "Overlay only)";
                else in_name << "Not supported)";

                memset(deviceinfo[0][sys].inputnames[dig], 0, SVL_VCS_STRING_LENGTH);
                memcpy(deviceinfo[0][sys].inputnames[dig],
                       in_name.str().c_str(),
                       std::min(SVL_VCS_STRING_LENGTH - 1, static_cast<int>(in_name.str().length())));
            }

            // test
            deviceinfo[0][sys].testok = true;
        }
    }
    else {
        deviceinfo[0] = 0;
    }

    return MILNumberOfSystems;
}

int svlVidCapSrcMIL::Open()
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::Open() - Number of video channels = " << NumOfStreams << std::endl;
#endif

    if (NumOfStreams <= 0) return SVL_FAIL;
    if (Initialized) return SVL_OK;

    Close();

    for (unsigned int i = 0; i < NumOfStreams; i ++) {

        if (MilSystem[SystemID[i]] == M_NULL) {
            if (!MILInitializeSystem(SystemID[i])) {
#if __VERBOSE__ == 1
                std::cerr << "svlVidCapSrcMIL::Open() - Failed to initialize system M_DEV" << SystemID[i] << std::endl;
#endif
                goto labError;
            }
        }

        if (MilDigitizer[SystemID[i]][DigitizerID[i]] == M_NULL) {
            if (!MILInitializeDigitizer(SystemID[i], DigitizerID[i])) {
#if __VERBOSE__ == 1
                std::cerr << "svlVidCapSrcMIL::Open() - Failed to initialize digitizer M_DEV"
                          << DigitizerID[i] << " on system M_DEV" << SystemID[i] << std::endl;
#endif
                goto labError;
            }
        }

        if (!MILInitializeCapture(SystemID[i], DigitizerID[i])) {
#if __VERBOSE__ == 1
            std::cerr << "svlVidCapSrcMIL::Open() - Failed to initialize capture on system M_DEV"
                      << SystemID[i] << ", digitizer M_DEV" << DigitizerID[i] << std::endl;
#endif
            goto labError;
        }

        if (MilBands[SystemID[i]][DigitizerID[i]] != 1 &&
            MilBands[SystemID[i]][DigitizerID[i]] != 3) goto labError;

        // Allocate capture buffers
        const unsigned int width  = MilWidth[SystemID[i]][DigitizerID[i]];
        const unsigned int height = MilHeight[SystemID[i]][DigitizerID[i]];
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::Open - Allocate image buffer (" << width << ", " << height << ")" << std::endl;
#endif
        ImageBuffer[i] = new svlBufferImage(width, height);
        // Set the pointer in the capture structure that will be accessed in the callback
        MilCaptureParams[SystemID[i]][DigitizerID[i]].ImageBuffer = ImageBuffer[i];
        MilCaptureParams[SystemID[i]][DigitizerID[i]].SystemID    = SystemID[i];
        MilCaptureParams[SystemID[i]][DigitizerID[i]].DigitizerID = DigitizerID[i];
    }

    Initialized = true;
    return SVL_OK;

labError:
    Close();
    return SVL_FAIL;
}

void svlVidCapSrcMIL::Close()
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::Close()" << std::endl;
#endif

    if (NumOfStreams == 0) return;

    Stop();

    Initialized = false;

    for (unsigned int sys = 0; sys < MIL_MAX_SYS; sys ++) {
        if (MilSystem[sys] == M_NULL) continue;
        bool no_release = false;

        for (unsigned int dig = 0; dig < MIL_MAX_DIG; dig ++) {
            if (MilDigitizer[dig] == M_NULL) continue;

            // Release capture
            MILReleaseCapture(sys, dig);

            // Do not release system and digitizer if overlay is used
            if (MilDisplay[sys][dig] == M_NULL) {
                MILReleaseDigitizer(sys, dig);
            }
            else {
                no_release = true;
            }
        }

        if (!no_release) MILReleaseSystem(sys);
    }

    for (unsigned int i = 0; i < NumOfStreams; i ++) {
        delete ImageBuffer[i];
        ImageBuffer[i] = 0;
    }
}

int svlVidCapSrcMIL::Start()
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::Start()" << std::endl;
#endif

    if (!Initialized) return SVL_FAIL;
    Running = true;
    return SVL_OK;
}

svlImageRGB* svlVidCapSrcMIL::GetLatestFrame(bool waitfornew, unsigned int videoch)
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::GetLatestFrame(" << waitfornew << ", " << videoch << ")" << std::endl;
#endif

    if (videoch >= NumOfStreams || !Initialized) return 0;
    return ImageBuffer[videoch]->Pull(waitfornew);
}

int svlVidCapSrcMIL::Stop()
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::Stop()" << std::endl;
#endif

    if (!Running) return SVL_FAIL;
    Running = false;
    return SVL_OK;
}

bool svlVidCapSrcMIL::IsRunning()
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::IsRunning()" << std::endl;
#endif

    return Running;
}

int svlVidCapSrcMIL::SetDevice(int devid, int inid, unsigned int videoch)
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::SetDevice(" << devid << ", " << inid << ", " << videoch << ")" << std::endl;
#endif

    if (videoch >= NumOfStreams) return SVL_FAIL;
    SystemID[videoch] = devid;
    DigitizerID[videoch] = inid;
    return SVL_OK;
}

int svlVidCapSrcMIL::GetWidth(unsigned int videoch)
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::GetWidth(" << videoch << ")" << std::endl;
#endif

    if (videoch >= NumOfStreams) return SVL_FAIL;
    return ImageBuffer[videoch]->GetWidth();
}

int svlVidCapSrcMIL::GetHeight(unsigned int videoch)
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::GetHeight(" << videoch << ")" << std::endl;
#endif

    if (videoch >= NumOfStreams) return SVL_FAIL;
    return ImageBuffer[videoch]->GetHeight();
}

int svlVidCapSrcMIL::GetFormatList(unsigned int deviceid, svlFilterSourceVideoCapture::ImageFormat **formatlist)
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::GetFormatList(" << deviceid << ", ...)" << std::endl;
#endif

    if (static_cast<int>(deviceid) >= MILNumberOfSystems || formatlist == 0) return SVL_FAIL;

    formatlist[0] = new svlFilterSourceVideoCapture::ImageFormat[1];
    formatlist[0][0].width = MilWidth[deviceid][0];
    formatlist[0][0].height = MilHeight[deviceid][0];
    formatlist[0][0].colorspace = svlFilterSourceVideoCaptureTypes::PixelRGB8;
    formatlist[0][0].rgb_order = true;
    formatlist[0][0].yuyv_order = false;
    formatlist[0][0].framerate = -1.0;
    formatlist[0][0].custom_mode = -1;

    return 1;
}

int svlVidCapSrcMIL::GetFormat(svlFilterSourceVideoCapture::ImageFormat& format, unsigned int videoch)
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::GetFormat(..., " << videoch << ")" << std::endl;
#endif

    if (SystemID[videoch] > 0 && SystemID[videoch] >= static_cast<int>(MILNumberOfSystems)) return SVL_FAIL;

    format.width = MilWidth[SystemID[videoch]][DigitizerID[videoch]];
    format.height = MilHeight[SystemID[videoch]][DigitizerID[videoch]];
    format.colorspace = svlFilterSourceVideoCaptureTypes::PixelRGB8;
    format.rgb_order = true;
    format.yuyv_order = false;
    format.framerate = -1.0;
    format.custom_mode = -1;

    return SVL_OK;
}

bool svlVidCapSrcMIL::IsCaptureSupported(unsigned int sysid, unsigned int digid)
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::IsCaptureSupported(" << sysid << ", " << digid << ")" << std::endl;
#endif

    if (sysid >= MILNumberOfSystems || digid >= MilNumberOfDigitizers[sysid]) return false;
    return MilCaptureSupported[sysid][digid];
}

bool svlVidCapSrcMIL::IsOverlaySupported(unsigned int sysid, unsigned int digid)
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::IsOverlaySupported(" << sysid << ", " << digid << ")" << std::endl;
#endif

    if (sysid >= MILNumberOfSystems || digid >= MilNumberOfDigitizers[sysid]) return false;
    return MilOverlaySupported[sysid][digid];
}

void svlVidCapSrcMIL::Release()
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::Release()" << std::endl;
#endif

	Close();

    NumOfStreams = 0;
    Initialized = false;
    Running = false;
	SystemID.SetSize(0);
	DigitizerID.SetSize(0);
	ImageBuffer.SetSize(0);

    for (unsigned int sys = 0; sys < MIL_MAX_SYS; sys ++) {
        if (MilSystem[sys] == M_NULL) continue;
        for (unsigned int dig = 0; dig < MIL_MAX_DIG; dig ++) {
            if (MilDigitizer[dig] == M_NULL) continue;
            MILReleaseOverlay(sys, dig);
            MILReleaseCapture(sys, dig);
            MILReleaseDigitizer(sys, dig);
        }
        MILReleaseSystem(sys);
    }
}

bool svlVidCapSrcMIL::MILInitializeApplication()
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::MILInitializeApplication()" << std::endl;
#endif

    if (MilApplication == M_NULL) MappAlloc(M_DEFAULT, &MilApplication);
    if (MilApplication == M_NULL) return false;
    MappControl(M_ERROR, M_PRINT_DISABLE);
//    MappControlMp(MilApplication, M_MP_PRIORITY, M_DEFAULT, M_TIME_CRITICAL, M_NULL);
    return true;
}

bool svlVidCapSrcMIL::MILInitializeSystem(int system)
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::MILInitializeSystem(" << system << ")" << std::endl;
#endif

    if (system < 0 || system >= MIL_MAX_SYS) return false;

    if (MilSystem[system] != M_NULL) {
        MILReleaseSystem(system);
    }

    MsysAlloc(M_SYSTEM_DEFAULT, system, M_SETUP, &(MilSystem[system]));
    if (MilSystem[system] == M_NULL) return false;

    MsysControl(system, M_MODIFIED_BUFFER_HOOK_MODE, M_MULTI_THREAD + 2/*Max number of threads*/);

    MilNumberOfDigitizers[system] = static_cast<unsigned int>(MsysInquire(MilSystem[system], M_DIGITIZER_NUM, M_NULL));
    if (MilNumberOfDigitizers[system] > 0) {
        // No more than 64 inputs
        if (MilNumberOfDigitizers[system] > 64) MilNumberOfDigitizers[system] = 64;
        return true;
    }

    MILReleaseSystem(system);
    return false;
}

bool svlVidCapSrcMIL::MILInitializeDigitizer(int system, int digitizer)
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::MILInitializeDigitizer(" << system << ", " << digitizer << ")" << std::endl;
#endif

    if (system    < 0 || system    >= static_cast<int>(MILNumberOfSystems) ||
        digitizer < 0 || digitizer >= static_cast<int>(MilNumberOfDigitizers[system]) ||
        MilSystem[system] == M_NULL) return false;

    MdigAlloc(MilSystem[system], digitizer, MIL_TEXT("M_DEFAULT"), M_DEFAULT, &(MilDigitizer[system][digitizer]));
    if (MilDigitizer[system][digitizer] == M_NULL) return false;

    MilWidth[system][digitizer] = static_cast<long>(MdigInquire(MilDigitizer[system][digitizer], M_SIZE_X, M_NULL));
    MilHeight[system][digitizer] = static_cast<long>(MdigInquire(MilDigitizer[system][digitizer], M_SIZE_Y, M_NULL));
    MilBands[system][digitizer] = static_cast<long>(MdigInquire(MilDigitizer[system][digitizer], M_SIZE_BAND, M_NULL)); 
    MilBandBits[system][digitizer] = static_cast<long>(MdigInquire(MilDigitizer[system][digitizer], M_SIZE_BIT, M_NULL));

#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::MILInitializeDigitizer - system=" << system
                                                      << ", digitizer=" << digitizer
                                                      << ", width=" << MilWidth[system][digitizer]
                                                      << ", height=" << MilHeight[system][digitizer]
                                                      << ", bands=" << MilBands[system][digitizer]
                                                      << ", bandbits=" << MilBandBits[system][digitizer] << std::endl;
#endif

    return true;
}

bool svlVidCapSrcMIL::MILInitializeCapture(int system, int digitizer)
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::MILInitializeCapture(" << system << ", " << digitizer << ")" << std::endl;
#endif

    if (system    < 0 || system    >= static_cast<int>(MILNumberOfSystems) ||
        digitizer < 0 || digitizer >= static_cast<int>(MilNumberOfDigitizers[system]) ||
        MilSystem[system]               == M_NULL ||
        MilDigitizer[system][digitizer] == M_NULL) {
#if __VERBOSE__ == 1
        std::cerr << "svlVidCapSrcMIL::MILInitializeCapture() - error: system or digitizer not yet initialized" << std::endl;
#endif
        return false;
    }

    if (MilCaptureSupported[system][digitizer] &&
        MilCaptureParams[system][digitizer].MilFrames) {
#if __VERBOSE__ == 1
        std::cerr << "svlVidCapSrcMIL::MILInitializeCapture() - already initialized" << std::endl;
#endif
        return true;
    }

    MilCaptureSupported[system][digitizer] = false;

    MilCaptureParams[system][digitizer].MilFrames = new MIL_ID[MilCaptureBuffers];
	for (unsigned int i = 0; i < MilCaptureBuffers; i ++) {
        MilCaptureParams[system][digitizer].MilFrames[i] = M_NULL;
    }

	for (unsigned int i = 0; i < MilCaptureBuffers; i ++) {
		MbufAllocColor(MilSystem[system],
					   MilBands[system][digitizer],
					   MilWidth[system][digitizer],
					   MilHeight[system][digitizer],
					   8+M_UNSIGNED, M_IMAGE+M_DISP+M_PROC+M_GRAB,
					   &(MilCaptureParams[system][digitizer].MilFrames[i]));
        if (MilCaptureParams[system][digitizer].MilFrames[i] == M_NULL) goto labError;

        MbufClear(MilCaptureParams[system][digitizer].MilFrames[i], 0);
	}

    // Triple buffer will be initialized by the caller upon return
    MilCaptureParams[system][digitizer].ImageBuffer = 0;

	MdigProcess(MilDigitizer[system][digitizer],
				MilCaptureParams[system][digitizer].MilFrames,
				MilCaptureBuffers,
				M_START,
				M_ASYNCHRONOUS,
				MILProcessingCallback,
				&(MilCaptureParams[system][digitizer]));

    MilCaptureSupported[system][digitizer] = true;
    return true;

labError:
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::MILInitializeCapture() - error" << std::endl;
#endif
    MILReleaseCapture(system, digitizer);
    return false;
}

bool svlVidCapSrcMIL::MILInitializeOverlay(int system, int digitizer)
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::MILInitializeOverlay(" << system << ", " << digitizer << ")" << std::endl;
#endif

    if (system    < 0 || system    >= static_cast<int>(MILNumberOfSystems) ||
        digitizer < 0 || digitizer >= static_cast<int>(MilNumberOfDigitizers[system]) ||
        MilSystem[system]               == M_NULL ||
        MilDigitizer[system][digitizer] == M_NULL) {
#if __VERBOSE__ == 1
        std::cerr << "svlVidCapSrcMIL::MILInitializeOverlay() - error: system or digitizer not yet initialized" << std::endl;
#endif
        return false;
    }

    MilOverlaySupported[system][digitizer] = false;

    MdispAlloc(MilSystem[system], M_DEFAULT, MIL_TEXT("M_DEFAULT"), M_DEFAULT, &(MilDisplay[system][digitizer]));
    if (MilDisplay[system][digitizer] == M_NULL) goto labError;

    MbufAllocColor(MilSystem[system],
                   MilBands[system][digitizer],
                   MilWidth[system][digitizer],
                   MilHeight[system][digitizer],
                   8+M_UNSIGNED, M_IMAGE+M_DISP+M_PROC+M_GRAB,
                   &(MilDisplayImage[system][digitizer]));
    if (MilDisplayImage[system][digitizer] == M_NULL) goto labError;

    MbufClear(MilDisplayImage[system][digitizer], 0);
#if M_MIL_CURRENT_INT_VERSION >= 0x0900
    MdispControl(MilDisplay[system][digitizer], M_SELECT_VIDEO_SOURCE, MilDigitizer[system][digitizer]);
#endif
    MdispSelect(MilDisplay[system][digitizer], MilDisplayImage[system][digitizer]);
    MdispControl(MilDisplay[system][digitizer], M_OVERLAY, M_ENABLE);
    MdispControl(MilDisplay[system][digitizer], M_OVERLAY_CLEAR, M_DEFAULT);
    MdispControl(MilDisplay[system][digitizer], M_OVERLAY_SHOW, M_ENABLE);
    MdispControl(MilDisplay[system][digitizer], M_NO_TEARING, M_ENABLE);

    MdispInquire(MilDisplay[system][digitizer], M_OVERLAY_ID, &MilOverlayImage[system][digitizer]);
    if (MilOverlayImage[system][digitizer] == M_NULL) goto labError;

    MdispControl(MilDisplay[system][digitizer], M_TRANSPARENT_COLOR, static_cast<MIL_INT32>(M_BGR888(0,0,0)));

    MilOverlayBuffer[system][digitizer] = new unsigned char[MilWidth[system][digitizer] * MilHeight[system][digitizer] * MilBands[system][digitizer]];

    MilCaptureParams[system][digitizer].MilOverlayImage = &(MilOverlayImage[system][digitizer]);
    MilCaptureParams[system][digitizer].MilOverlayBuffer = MilOverlayBuffer[system][digitizer];
    MilCaptureParams[system][digitizer].OverlayModified = false;

    MilOverlaySupported[system][digitizer] = true;
    return true;

labError:
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::MILInitializeOverlay() - error" << std::endl;
#endif
    MILReleaseOverlay(system, digitizer);
    return false;
}

bool svlVidCapSrcMIL::MILUploadOverlay(int system, int digitizer)
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::MILUploadOverlay(" << system << ", " << digitizer << ")" << std::endl;
#endif

    if (system    < 0 || system    >= static_cast<int>(MILNumberOfSystems) ||
        digitizer < 0 || digitizer >= static_cast<int>(MilNumberOfDigitizers[system]) ||
        MilSystem[system]               == M_NULL ||
        MilDigitizer[system][digitizer] == M_NULL ||
        MilOverlaySupported[system][digitizer] == false) {
#if __VERBOSE__ == 1
        std::cerr << "svlVidCapSrcMIL::MILUploadOverlay() - error: system or digitizer not yet initialized" << std::endl;
#endif
        return false;
    }

    if (MilCaptureParams[system][digitizer].MilFrames) {
        // Capture is active
        MilCaptureParams[system][digitizer].OverlayModified = true;
    }
    else {
        // Overlay only
        MbufPutColor(MilOverlayImage[system][digitizer],
                     M_PACKED+M_RGB24, M_ALL_BANDS,
                     MilOverlayBuffer[system][digitizer]);
    }

    return true;
}

void svlVidCapSrcMIL::MILReleaseOverlay(int system, int digitizer)
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::MILReleaseOverlay(" << system << ", " << digitizer << ")" << std::endl;
#endif

    if (system    < 0 || system    >= static_cast<int>(MILNumberOfSystems) ||
        digitizer < 0 || digitizer >= static_cast<int>(MilNumberOfDigitizers[system]) ||
        MilSystem[system]               == M_NULL ||
        MilDigitizer[system][digitizer] == M_NULL) return;

    if (MilOverlayImage[system][digitizer] != M_NULL) {
        MbufClear(MilOverlayImage[system][digitizer], 0);
    }

    if (MilDisplay[system][digitizer] != M_NULL) {
        MdispControl(MilDisplay[system][digitizer], M_AUXILIARY_KEEP_DISPLAY_ALIVE, M_ENABLE);
    }

    if (MilDisplay[system][digitizer] != M_NULL && MilDisplayImage[system][digitizer] != M_NULL) {
        MdispDeselect(MilDisplay[system][digitizer], MilDisplayImage[system][digitizer]);
    }

    if (MilDisplay[system][digitizer] != M_NULL) {
        MdispFree(MilDisplay[system][digitizer]);
    }

    if (MilDisplayImage[system][digitizer] != M_NULL) {
        MbufFree(MilDisplayImage[system][digitizer]);
    }

    if (MilOverlayBuffer[system][digitizer]) {
        delete [] MilOverlayBuffer[system][digitizer];
    }

    MilDisplay[system][digitizer]       = M_NULL;
    MilDisplayImage[system][digitizer]  = M_NULL;
    MilOverlayImage[system][digitizer]  = M_NULL;
    MilOverlayBuffer[system][digitizer] = 0;
}

void svlVidCapSrcMIL::MILReleaseCapture(int system, int digitizer)
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::MILReleaseCapture(" << system << ", " << digitizer << ")" << std::endl;
#endif

    if (system    < 0 || system    >= static_cast<int>(MILNumberOfSystems) ||
        digitizer < 0 || digitizer >= static_cast<int>(MilNumberOfDigitizers[system]) ||
        MilSystem[system]               == M_NULL ||
        MilDigitizer[system][digitizer] == M_NULL) return;

   	MdigProcess(MilDigitizer[system][digitizer],
				MilCaptureParams[system][digitizer].MilFrames,
				MilCaptureBuffers,
				M_STOP,//+M_WAIT,
				M_DEFAULT,
				MILProcessingCallback,
				&(MilCaptureParams[system][digitizer]));

    if (MilCaptureParams[system][digitizer].MilFrames) {
    	for (unsigned int i = 0; i < MilCaptureBuffers; i ++) {
            if (MilCaptureParams[system][digitizer].MilFrames[i] != M_NULL) {
                MbufFree(MilCaptureParams[system][digitizer].MilFrames[i]);
            }
    	}
        delete [] MilCaptureParams[system][digitizer].MilFrames;
        MilCaptureParams[system][digitizer].MilFrames = 0;
    }
}

void svlVidCapSrcMIL::MILReleaseDigitizer(int system, int digitizer)
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::MILReleaseDigitizer(" << system << ", " << digitizer << ")" << std::endl;
#endif

    if (system    < 0 || system    >= static_cast<int>(MILNumberOfSystems) ||
        digitizer < 0 || digitizer >= static_cast<int>(MilNumberOfDigitizers[system]) ||
        MilSystem[system]               == M_NULL ||
        MilDigitizer[system][digitizer] == M_NULL) return;

    if (MilDigitizer[system][digitizer] != M_NULL) {
        MdigHalt(MilDigitizer[system][digitizer]);
    }

    if (MilDigitizer[system][digitizer] != M_NULL) {
        MdigFree(MilDigitizer[system][digitizer]);
    }

    MilDigitizer[system][digitizer] = M_NULL;
}

void svlVidCapSrcMIL::MILReleaseSystem(int system)
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::MILReleaseSystem(" << system << ")" << std::endl;
#endif

    if (system < 0 || system >= static_cast<int>(MILNumberOfSystems) ||
        MilSystem[system] == M_NULL) return;

    if (MilSystem[system] != M_NULL) {
        MsysFree(MilSystem[system]);
    }

    MilSystem[system] = M_NULL;
}

void svlVidCapSrcMIL::MILReleaseApplication()
{
#if __VERBOSE__ == 1
    std::cerr << "svlVidCapSrcMIL::MILReleaseApplication()" << std::endl;
#endif

    if (MilApplication != M_NULL) MappFree(MilApplication);
    MilApplication = M_NULL;
}


/*************************************/
/*** MILProcessingCallback ***********/
/*************************************/

MIL_INT MFTYPE MILProcessingCallback(MIL_INT CMN_UNUSED(HookType), MIL_ID HookId, void MPTYPE *HookDataPtr)
{
#if __VERBOSE__ == 1
    std::cerr << "MILProcessingCallback()" << std::endl;
#endif

    svlVidCapSrcMIL::MILCaptureParameters *milcaptureparams = (svlVidCapSrcMIL::MILCaptureParameters*)HookDataPtr;

    if (milcaptureparams->ImageBuffer) {
        MIL_INT milbufferindex;
        MdigGetHookInfo(HookId, M_MODIFIED_BUFFER+M_BUFFER_ID, &milbufferindex);
        MbufGetColor(milbufferindex,
    				 M_PACKED+M_RGB24,
    				 M_ALL_BANDS,
                     milcaptureparams->ImageBuffer->GetPushBuffer());
        milcaptureparams->ImageBuffer->Push();
    }
    if (milcaptureparams->OverlayModified) {
        milcaptureparams->OverlayModified = false;
        MbufPutColor((milcaptureparams->MilOverlayImage)[0],
                     M_PACKED+M_RGB24, M_ALL_BANDS,
                     milcaptureparams->MilOverlayBuffer);
    }

	return 0;
}

