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

#include "svlVidCapSrcDC1394.h"
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstStereoVision/svlBufferImage.h>

#include <iostream>

using std::cerr;
using std::endl;


//////////////////////////////////////////////////////////
// Verbose levels:                                      //
//  0 -   quiet                                         //
//  1 -   only error and warning messages               //
//  2 -   1 + configuration setup messages              //
//  3 -   2 + status messages                           //
//  4 -   all, including frame capture status messages  //
//////////////////////////////////////////////////////////
#define __verbose__             1


#define DEFAULT_WIDTH           1024
#define DEFAULT_HEIGHT          768
#define DEFAULT_COLORSPACE      svlFilterSourceVideoCaptureTypes::PixelYUV422
#define DEFAULT_FRAMERATE       30
#define FRAME_BUFFER_SIZE       2
#define MAX_VENDOR_LEN          32
#define MAX_MODEL_LEN           32


/****************************************/
/*** svlVidCapSrcDC1394Context class ****/
/****************************************/

svlVidCapSrcDC1394Context* svlVidCapSrcDC1394Context::Instance()
{
    static svlVidCapSrcDC1394Context instance;
    return &instance;
}

dc1394_t* svlVidCapSrcDC1394Context::GetContext()
{
    return Instance()->Context;
}

int svlVidCapSrcDC1394Context::GetDeviceList(svlFilterSourceVideoCaptureTypes::DeviceInfo **deviceinfo)
{
    if (deviceinfo) {
        // Allocate memory for device info array
        // CALLER HAS TO FREE UP THIS ARRAY!!!
        if (NumberOfCameras > 0) {
            deviceinfo[0] = new svlFilterSourceVideoCaptureTypes::DeviceInfo[NumberOfCameras];
            memcpy(deviceinfo[0], DeviceInfos, NumberOfCameras * sizeof(svlFilterSourceVideoCaptureTypes::DeviceInfo));
        }
        else {
            deviceinfo[0] = 0;
        }
    }
    return NumberOfCameras;
}

dc1394camera_t** svlVidCapSrcDC1394Context::GetCameras()
{
    return Cameras;
}

unsigned int svlVidCapSrcDC1394Context::GetNumberOfCameras()
{
    return NumberOfCameras;
}

dc1394operation_mode_t* svlVidCapSrcDC1394Context::GetBestOpMode()
{
    return BestOpMode;
}

dc1394speed_t* svlVidCapSrcDC1394Context::GetBestISOSpeed()
{
    return BestISOSpeed;
}

unsigned int svlVidCapSrcDC1394Context::GetPixelTypeBitSize(svlFilterSourceVideoCaptureTypes::PixelType type)
{
    return PixelTypeBitSize[type];
}

void svlVidCapSrcDC1394Context::Enumerate()
{
    //////////////////////////////////////////////////////////////////////
    // Please note that the user has to have read+write permissions to: //
    //    /dev/raw1394                                                  //
    //    /dev/video1394                                                //
    //    /dev/video1394/*                                              //
    //////////////////////////////////////////////////////////////////////

    // Re-enumerate camera list
    ReleaseEnumeration();

#if (__verbose__ < 2)
    // Disable libdc1394 warnings print-outs
    dc1394_log_register_handler(DC1394_LOG_WARNING, 0, 0);
#endif

    // Get list of cameras with 'GUID' and 'Unit' identifiers
    if (dc1394_camera_enumerate(Context, &CameraList) != DC1394_SUCCESS || CameraList->num < 1) {
        ReleaseEnumeration();
        return;
    }

    int len;
    unsigned int i;
    char tempname[MAX_MODEL_LEN], tempvendor[MAX_MODEL_LEN];
    svlFilterSourceVideoCaptureTypes::DeviceInfo *tempinfo;

    // Get camera structures
    Cameras = new dc1394camera_t*[CameraList->num];
    for (i = 0; i < CameraList->num; i ++) {
        Cameras[i] = dc1394_camera_new_unit(Context,
                                            CameraList->ids[i].guid,
                                            CameraList->ids[i].unit);
    }

    BestOpMode = new dc1394operation_mode_t[CameraList->num];
    BestISOSpeed = new dc1394speed_t[CameraList->num];
    tempinfo = new svlFilterSourceVideoCaptureTypes::DeviceInfo[CameraList->num];
    memset(tempinfo, 0, CameraList->num * sizeof(svlFilterSourceVideoCaptureTypes::DeviceInfo));

    NumberOfCameras = 0;
    for (i = 0; i < CameraList->num; i ++) {
        if (Cameras[i] == 0) continue;

#if CISST_SVL_DC1394_ENABLE_800Mbps
        // Check if IEEE1394B operation mode is supported
        if (TestIEEE1394Interface(Cameras[i], DC1394_OPERATION_MODE_1394B, DC1394_ISO_SPEED_800) == SVL_OK) {
            BestOpMode[i] = DC1394_OPERATION_MODE_1394B;
            BestISOSpeed[i] = DC1394_ISO_SPEED_800;
        }
        else {
            BestOpMode[i] = DC1394_OPERATION_MODE_LEGACY;
            BestISOSpeed[i] = DC1394_ISO_SPEED_400;
        }
#else // CISST_SVL_DC1394_ENABLE_800Mbps
        BestOpMode[i] = DC1394_OPERATION_MODE_LEGACY;
        BestISOSpeed[i] = DC1394_ISO_SPEED_400;
#endif // CISST_SVL_DC1394_ENABLE_800Mbps

        // Build device model string
        len = strlen(Cameras[i]->model);
        if (len >= MAX_MODEL_LEN) {
            len = MAX_MODEL_LEN - 1;
            Cameras[i]->model[len] = 0;
        }
        if (len > 0) strcpy(tempname, Cameras[i]->model);
        else strcpy(tempname, "Unknown device model");

        // Build device vendor string
        len = strlen(Cameras[i]->vendor);
        if (len >= MAX_VENDOR_LEN) {
            len = MAX_VENDOR_LEN - 1;
            Cameras[i]->vendor[len] = 0;
        }
        if (len > 0) strcpy(tempvendor, Cameras[i]->vendor);
        else strcpy(tempvendor, "Unknown device vendor");

        // Fill device properties structure
        tempinfo[NumberOfCameras].ID = i;
        if (BestOpMode[i] == DC1394_OPERATION_MODE_1394B) {
            sprintf(tempinfo[NumberOfCameras].name, "%s (%s) [1394B]", tempname, tempvendor);
        }
        else {
            sprintf(tempinfo[NumberOfCameras].name, "%s (%s) [1394A]", tempname, tempvendor);
        }
        tempinfo[NumberOfCameras].platform = svlFilterSourceVideoCaptureTypes::LinLibDC1394;
        tempinfo[NumberOfCameras].inputcount = -1;
        tempinfo[NumberOfCameras].activeinput = -1;
        tempinfo[NumberOfCameras].testok = false;

        NumberOfCameras ++;
    }

    if (NumberOfCameras > 0) {
        DeviceInfos = new svlFilterSourceVideoCaptureTypes::DeviceInfo[NumberOfCameras];
        memcpy(DeviceInfos, tempinfo, NumberOfCameras * sizeof(svlFilterSourceVideoCaptureTypes::DeviceInfo));
    }

    if (tempinfo) delete [] tempinfo;
}

int svlVidCapSrcDC1394Context::TestIEEE1394Interface(dc1394camera_t* camera, dc1394operation_mode_t opmode, dc1394speed_t isospeed)
{
    if (camera == 0) return SVL_FAIL;

    int fileno = 0, nfds, ret = SVL_FAIL;
    fd_set fdset;
    dc1394switch_t status;

    if (dc1394_video_set_operation_mode(camera, opmode) != DC1394_SUCCESS) {
#if (__verbose__ >= 2)
        cerr << "CDC1394Context::TestIEEE1394Interface - dc1394_video_set_operation_mode returned error" << endl;
#endif
        goto labError;
    }
    if (dc1394_video_set_iso_speed(camera, isospeed) != DC1394_SUCCESS) {
#if (__verbose__ >= 2)
        cerr << "CDC1394Context::TestIEEE1394Interface - dc1394_video_set_iso_speed returned error" << endl;
#endif
        goto labError;
    }
    if (dc1394_video_set_mode(camera, DC1394_VIDEO_MODE_160x120_YUV444) != DC1394_SUCCESS) {
#if (__verbose__ >= 2)
        cerr << "CDC1394Context::TestIEEE1394Interface - dc1394_video_set_mode returned error" << endl;
#endif
        goto labError;
    }
    if (dc1394_video_set_framerate(camera, DC1394_FRAMERATE_7_5) != DC1394_SUCCESS) {
#if (__verbose__ >= 2)
        cerr << "CDC1394Context::TestIEEE1394Interface - dc1394_video_set_framerate returned error" << endl;
#endif
        goto labError;
    }
    if (dc1394_capture_setup(camera, FRAME_BUFFER_SIZE, DC1394_CAPTURE_FLAGS_DEFAULT) != DC1394_SUCCESS) {
#if (__verbose__ >= 2)
        cerr << "CDC1394Context::TestIEEE1394Interface - dc1394_capture_setup returned error" << endl;
#endif
        goto labError;
    }
    if (dc1394_external_trigger_set_mode(camera, DC1394_TRIGGER_MODE_0) != DC1394_SUCCESS) {
#if (__verbose__ >= 2)
        cerr << "CDC1394Context::TestIEEE1394Interface - dc1394_external_trigger_set_mode returned error" << endl;
#endif
        goto labError;
    }

    // Get file descriptor for camera
    fileno = dc1394_capture_get_fileno(camera);
    nfds = fileno + 1;

#if (__verbose__ >= 3)
        cerr << "CDC1394Context::TestIEEE1394Interface - starting test capture" << endl;
#endif

    // setting transmission ON
    if (dc1394_video_set_transmission(camera, DC1394_ON) != DC1394_SUCCESS) {
#if (__verbose__ >= 2)
        cerr << "CDC1394Context::TestIEEE1394Interface - dc1394_video_set_transmission returned error" << endl;
#endif
        goto labError;
    }
    status = DC1394_OFF;
    for (unsigned int i = 0; i < 30; i ++) {
        // Check whether transmission has started
        if (dc1394_video_get_transmission(camera, &status) != DC1394_SUCCESS ||
            status == DC1394_ON) break;
        osaSleep(0.1 * cmn_s); // = 100 millisec
    }
    if (status != DC1394_ON) {
#if (__verbose__ >= 2)
        cerr << "CDC1394Context::TestIEEE1394Interface - dc1394_video_get_transmission returned error" << endl;
#endif
        goto labError;
    }

#if (__verbose__ >= 3)
        cerr << "CDC1394Context::TestIEEE1394Interface - waiting for video frame" << endl;
#endif

    // Wait 2 seconds for the file descriptor
    FD_SET(fileno, &fdset);
    timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    if (select(nfds, &fdset, 0, 0, &tv ) <= 0) {
#if (__verbose__ >= 3)
        cerr << "CDC1394Context::TestIEEE1394Interface - could not capture frame" << endl;
#endif
        goto labError;
    }

#if (__verbose__ >= 3)
    cerr << "CDC1394Context::TestIEEE1394Interface - video frame successfully received" << endl;
#endif

    ret = SVL_OK;

labError:

    if (status == DC1394_ON) {
        dc1394_video_set_transmission(camera, DC1394_OFF);
    }
    if (fileno) {
        FD_CLR(fileno, &fdset);
    }
    dc1394_capture_stop(camera);

    return ret;
}

void svlVidCapSrcDC1394Context::ReleaseEnumeration()
{
    if (CameraList) dc1394_camera_free_list(CameraList);
    if (Cameras) {
        for (unsigned int i = 0; i < NumberOfCameras; i ++) {
            if (Cameras[i]) dc1394_camera_free(Cameras[i]);
        }
        delete [] Cameras;
    }
    if (DeviceInfos) delete [] DeviceInfos;
    if (BestOpMode) delete [] BestOpMode;
    if (BestISOSpeed) delete [] BestISOSpeed;

    NumberOfCameras = 0;
    CameraList = 0;
    Cameras = 0;
    DeviceInfos = 0;
    BestOpMode = 0;
    BestISOSpeed = 0;
}

svlVidCapSrcDC1394Context::svlVidCapSrcDC1394Context() :
    Context(0),
    CameraList(0),
    Cameras(0),
    DeviceInfos(0),
    NumberOfCameras(0),
    BestOpMode(0),
    BestISOSpeed(0)
{
    Context = dc1394_new();
    Enumerate();

    PixelTypeBitSize[svlFilterSourceVideoCaptureTypes::PixelRAW8]    =  8;
    PixelTypeBitSize[svlFilterSourceVideoCaptureTypes::PixelRAW16]   = 16;
    PixelTypeBitSize[svlFilterSourceVideoCaptureTypes::PixelRGB8]    = 24;
    PixelTypeBitSize[svlFilterSourceVideoCaptureTypes::PixelYUV444]  = 24;
    PixelTypeBitSize[svlFilterSourceVideoCaptureTypes::PixelYUV422]  = 16;
    PixelTypeBitSize[svlFilterSourceVideoCaptureTypes::PixelYUV411]  = 12;
    PixelTypeBitSize[svlFilterSourceVideoCaptureTypes::PixelMONO8]   =  8;
    PixelTypeBitSize[svlFilterSourceVideoCaptureTypes::PixelMONO16]  = 16;
    PixelTypeBitSize[svlFilterSourceVideoCaptureTypes::PixelUnknown] = 32;
}

svlVidCapSrcDC1394Context::~svlVidCapSrcDC1394Context()
{
    ReleaseEnumeration();
    if (Context) dc1394_free(Context);
}


/*************************************/
/*** svlVidCapSrcDC1394 class ********/
/*************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlVidCapSrcDC1394, svlVidCapSrcBase)

svlVidCapSrcDC1394::svlVidCapSrcDC1394() :
    svlVidCapSrcBase(),
    NumOfStreams(0),
    Initialized(false),
    Running(false),
    CaptureProc(0),
    CaptureThread(0),
    CameraFileNo(0),
    CameraFDSet(0),
    Context(0),
    CameraList(0),
    Cameras(0),
    NumberOfCameras(0),
    BestOpMode(0),
    BestISOSpeed(0),
	DeviceID(0),
    Format(0),
    Trigger(0),
    Width(0),
    Height(0),
    ColorCoding(0),
    Frame(0),
    OutputBuffer(0)
{
}

svlVidCapSrcDC1394::~svlVidCapSrcDC1394()
{
    Release();
}

svlFilterSourceVideoCaptureTypes::PlatformType svlVidCapSrcDC1394::GetPlatformType()
{
    return svlFilterSourceVideoCaptureTypes::LinLibDC1394;
}

int svlVidCapSrcDC1394::SetStreamCount(unsigned int numofstreams)
{
    if (numofstreams < 1) return SVL_FAIL;

    Release();

    NumOfStreams = numofstreams;

    CaptureProc = new svlVidCapSrcDC1394Thread*[NumOfStreams];
    CaptureThread = new osaThread*[NumOfStreams];
    CameraFileNo = new int[NumOfStreams];
    CameraFDSet = new fd_set[NumOfStreams];
    DeviceID = new int[NumOfStreams];
    Format = new svlFilterSourceVideoCaptureTypes::ImageFormat*[NumOfStreams];
    Trigger = new svlFilterSourceVideoCaptureTypes::ExternalTrigger[NumOfStreams];
    Width = new int[NumOfStreams];
    Height = new int[NumOfStreams];
    ColorCoding = new unsigned int[NumOfStreams];
    Frame = new dc1394video_frame_t*[NumOfStreams];
    OutputBuffer = new svlBufferImage*[NumOfStreams];

    for (unsigned int i = 0; i < NumOfStreams; i ++) {
        CaptureProc[i] = 0;
        CaptureThread[i] = 0;
        CameraFileNo[i] = 0;
        FD_ZERO(&(CameraFDSet[i]));
        DeviceID[i] = -1;
        Format[i] = 0;
        memset(&(Trigger[i]), 0, sizeof(svlFilterSourceVideoCaptureTypes::ExternalTrigger));
        Width[i] = -1;
        Height[i] = -1;
        ColorCoding[i] = 0;
        Frame[i] = 0;
        OutputBuffer[i] = 0;
    }

    return SVL_OK;
}

int svlVidCapSrcDC1394::GetDeviceList(svlFilterSourceVideoCaptureTypes::DeviceInfo **deviceinfo)
{
    svlVidCapSrcDC1394Context* context = svlVidCapSrcDC1394Context::Instance();
    Context = context->GetContext();
    Cameras = context->GetCameras();
    NumberOfCameras = context->GetNumberOfCameras();
    BestOpMode = context->GetBestOpMode();
    BestISOSpeed = context->GetBestISOSpeed();
    return context->GetDeviceList(deviceinfo);
}

int svlVidCapSrcDC1394::Open()
{
    if (NumOfStreams <= 0) return SVL_FAIL;
    if (Initialized) return SVL_OK;

    // Get cameras
    if (Cameras == 0 && GetDeviceList(0) < 1) return SVL_FAIL;

    Close();

    int f7mode;
    double fps;
    unsigned int i, j;
    unsigned int mode = 0, framerate = 0;
    unsigned int f7roileft = 0, f7roitop = 0, f7framerate = 0, f7framerateunit = 0, f7frameratemax = 0;
    svlFilterSourceVideoCaptureTypes::PixelType  colorspace = svlFilterSourceVideoCaptureTypes::PixelRGB8;
    dc1394video_modes_t modes;
    dc1394framerates_t framerates;
    bool found;

#if (__verbose__ >= 1)
    cerr << endl;
#endif
    for (i = 0; i < NumOfStreams; i ++) {
        if (DeviceID[i] < 0 || DeviceID[i] >= static_cast<int>(NumberOfCameras)) {
#if (__verbose__ >= 1)
            cerr << "svlVidCapSrcDC1394::Open - wrong device ID" << endl;
#endif
            goto labError;
        }

        if (Format[i]->colorspace == svlFilterSourceVideoCaptureTypes::PixelRAW16) {
#if (__verbose__ >= 2)
            cerr << "svlVidCapSrcDC1394::Open - Unsupported color space" << endl;
#endif
            goto labError;
        }

        if (Format[i]) {
            if (Format[i]->custom_mode < 0) {
                    // Finding the best possible camera settings
                    // for the requested capture format
                if (GetModeFromFormat(Format[i]->width, Format[i]->height, Format[i]->colorspace, mode) != SVL_OK) {
#if (__verbose__ >= 1)
                    cerr << "svlVidCapSrcDC1394::Open - GetModeFromFormat returned error" << endl;
#endif
                    goto labError;
                }
                if (GetFramerateFromFPS(Format[i]->framerate, framerate) != SVL_OK) {
#if (__verbose__ >= 1)
                    cerr << "svlVidCapSrcDC1394::Open - GetFramerateFromFPS returned error" << endl;
#endif
                    goto labError;
                }
            }

            Width[i] = static_cast<int>(Format[i]->width);
            Height[i] = static_cast<int>(Format[i]->height);
            colorspace = Format[i]->colorspace;
            fps = Format[i]->framerate;
            f7mode = Format[i]->custom_mode;
            f7roileft = Format[i]->custom_roileft;
            f7roitop = Format[i]->custom_roitop;
            f7framerate = Format[i]->custom_framerate;
        }
        else {
            // Setting default image format
            GetModeFromFormat(DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_COLORSPACE, mode);
            GetFramerateFromFPS(DEFAULT_FRAMERATE, framerate);

            Width[i] = DEFAULT_WIDTH;
            Height[i] = DEFAULT_HEIGHT;
            colorspace = DEFAULT_COLORSPACE;
            fps = DEFAULT_FRAMERATE;
            f7mode = -1;
        }

        // Manually check whether enough ISO bandwidth is
        // available for the requested resolution and framerate
        int camsp, mbps;
        if (BestISOSpeed[DeviceID[i]] == DC1394_ISO_SPEED_800) camsp = 800;
        else camsp = 400;
        mbps = static_cast<int>(fps *
                                Width[i] * Height[i] *
                                svlVidCapSrcDC1394Context::Instance()->GetPixelTypeBitSize(colorspace) /
                                1024 / 1024);
        if (camsp < mbps) {
#if (__verbose__ >= 1)
            cerr << "svlVidCapSrcDC1394::Open - Framerate reduced due to insufficient bus bandwidth" << endl;
#endif
            framerate --;
        }

        OutputBuffer[i] = new svlBufferImage(Width[i], Height[i]);
        ColorCoding[i] = GetColorCodingFromPixelType(colorspace);
        
        if (dc1394_video_set_operation_mode(Cameras[DeviceID[i]],
                                            BestOpMode[DeviceID[i]]) != DC1394_SUCCESS) {
#if (__verbose__ >= 2)
            cerr << "svlVidCapSrcDC1394::Open - dc1394_video_set_operation_mode returned error" << endl;
#endif
            goto labError;
        }
        if (dc1394_video_set_iso_speed(Cameras[DeviceID[i]],
                                       BestISOSpeed[DeviceID[i]]) != DC1394_SUCCESS) {
#if (__verbose__ >= 2)
            cerr << "svlVidCapSrcDC1394::Open - dc1394_video_set_iso_speed returned error" << endl;
#endif
            goto labError;
        }

        if (f7mode < 0) {
            if (dc1394_video_get_supported_modes(Cameras[DeviceID[i]],
                                                 &modes) != DC1394_SUCCESS) {
#if (__verbose__ >= 1)
                cerr << "svlVidCapSrcDC1394::Open - dc1394_video_get_supported_modes returned error" << endl;
#endif
                goto labError;
            }
            // Check if video mode is supported
#if (__verbose__ >= 3)
            cerr << "svlVidCapSrcDC1394::Open - supported video modes: ";
#endif
            found = false;
            for (j = 0; j < modes.num; j ++) {
#if (__verbose__ >= 3)
                if (j > 0) cerr << ", ";
                cerr << modes.modes[j];
                if (j == (modes.num - 1)) cerr << endl;
#endif
                if (modes.modes[j] == static_cast<int>(mode)) found = true;
            }
            if (!found) {
#if (__verbose__ >= 1)
                cerr << "svlVidCapSrcDC1394::Open - requested video mode " << mode << " is not supported" << endl;
#endif
                goto labError;
            }
#if (__verbose__ >= 2)
            cerr << "svlVidCapSrcDC1394::Open - requested video mode " << mode << " is supported" << endl;
#endif
            if (dc1394_video_set_mode(Cameras[DeviceID[i]],
                                      (dc1394video_mode_t)mode) != DC1394_SUCCESS) {
#if (__verbose__ >= 1)
                cerr << "svlVidCapSrcDC1394::Open - dc1394_video_set_mode returned error" << endl;
#endif
                goto labError;
            }
        }
        else if (f7mode <= 7) {
            if (dc1394_video_set_mode(Cameras[DeviceID[i]],
                                      (dc1394video_mode_t)(f7mode + DC1394_VIDEO_MODE_FORMAT7_0)) != DC1394_SUCCESS) {
#if (__verbose__ >= 1)
                cerr << "svlVidCapSrcDC1394::Open - dc1394_video_set_mode returned error" << endl;
#endif
                goto labError;
            }
        }
        else {
#if (__verbose__ >= 1)
            cerr << "svlVidCapSrcDC1394::Open - requested Format7 mode " << f7mode << " is not supported" << endl;
#endif
            goto labError;
        }
#if (__verbose__ >= 3)
        cerr << "svlVidCapSrcDC1394::Open - video mode accepted" << endl;
#endif

        if (f7mode < 0) {
            if (dc1394_video_get_supported_framerates(Cameras[DeviceID[i]],
                                                      (dc1394video_mode_t)mode, &framerates) != DC1394_SUCCESS) {
#if (__verbose__ >= 1)
                cerr << "svlVidCapSrcDC1394::Open - dc1394_video_get_supported_framerates returned error" << endl;
#endif
                goto labError;
            }
            // Check if framerate is supported
#if (__verbose__ >= 3)
            cerr << "svlVidCapSrcDC1394::Open - supported framerates: ";
#endif
            found = false;
            for (j = 0; j < framerates.num; j ++) {
#if (__verbose__ >= 3)
                if (j > 0) cerr << ", ";
                cerr << framerates.framerates[j];
                if (j == (framerates.num - 1)) cerr << endl;
#endif
                if (framerates.framerates[j] == static_cast<int>(framerate)) found = true;
            }
            if (!found) {
#if (__verbose__ >= 2)
                cerr << "svlVidCapSrcDC1394::Open - requested framerate " << framerate << " is not supported" << endl;
#endif
                if (framerates.num <= 1) goto labError;
                framerate = framerates.framerates[framerates.num - 1];
#if (__verbose__ >= 2)
                cerr << "svlVidCapSrcDC1394::Open - setting highest supported framerate: " << framerate << endl;
#endif
            }
            else {
#if (__verbose__ >= 2)
            cerr << "svlVidCapSrcDC1394::Open - requested framerate " << framerate << " is supported" << endl;
#endif
            }
            if (dc1394_video_set_framerate(Cameras[DeviceID[i]],
                                           (dc1394framerate_t)framerate) != DC1394_SUCCESS) {
#if (__verbose__ >= 1)
                cerr << "svlVidCapSrcDC1394::Open - dc1394_video_set_framerate returned error" << endl;
#endif
                goto labError;
            }
#if (__verbose__ >= 3)
            cerr << "svlVidCapSrcDC1394::Open - framerate accepted" << endl;
#endif
        }
        else {
            if (dc1394_format7_set_color_coding(Cameras[DeviceID[i]],
                                                (dc1394video_mode_t)(f7mode + DC1394_VIDEO_MODE_FORMAT7_0),
                                                (dc1394color_coding_t)(ColorCoding[i])) != DC1394_SUCCESS) {
#if (__verbose__ >= 1)
                cerr << "svlVidCapSrcDC1394::Open - dc1394_format7_set_color_coding returned error" << endl;
#endif
                goto labError;
            }
            if (dc1394_format7_get_packet_parameters(Cameras[DeviceID[i]],
                                                     (dc1394video_mode_t)(f7mode + DC1394_VIDEO_MODE_FORMAT7_0),
                                                     &f7framerateunit, &f7frameratemax) != DC1394_SUCCESS) {
#if (__verbose__ >= 1)
                cerr << "svlVidCapSrcDC1394::Open - dc1394_format7_get_packet_parameters returned error" << endl;
#endif
                goto labError;
            }
            if (dc1394_format7_set_roi(Cameras[DeviceID[i]],
                                       (dc1394video_mode_t)(f7mode + DC1394_VIDEO_MODE_FORMAT7_0),
                                       (dc1394color_coding_t)(ColorCoding[i]),
                                       (((f7framerate * f7frameratemax) / f7framerateunit) / 100) * f7framerateunit,
                                       f7roileft, f7roitop,
                                       Width[i], Height[i]) != DC1394_SUCCESS) {
#if (__verbose__ >= 1)
                cerr << "svlVidCapSrcDC1394::Open - dc1394_format7_set_roi returned error" << endl;
#endif
                goto labError;
            }
#if (__verbose__ >= 3)
            cerr << "svlVidCapSrcDC1394::Open - Format7 setup accepted" << endl;
#endif
        }

        if (Trigger[DeviceID[i]].enable) {
            if (dc1394_external_trigger_set_power(Cameras[DeviceID[i]],
                                                  DC1394_ON) == DC1394_SUCCESS) {
#if (__verbose__ >= 3)
                cerr << "svlVidCapSrcDC1394::Open - external trigger enabled" << endl;
#endif
                unsigned int ivalue = Trigger[DeviceID[i]].mode;
                if (ivalue > 15 || (ivalue > 5 && ivalue < 14)) {
                    ivalue = 0;
#if (__verbose__ >= 2)
                    cerr << "svlVidCapSrcDC1394::Open - unsupported trigger mode; using mode 0 instead" << endl;
#endif
                }
                if (ivalue == 14) ivalue = 6;
                if (ivalue == 15) ivalue = 7;
                if (dc1394_external_trigger_set_mode(Cameras[DeviceID[i]],
                                                     static_cast<dc1394trigger_mode_t>(ivalue + DC1394_TRIGGER_MODE_MIN)) == DC1394_SUCCESS) {
#if (__verbose__ >= 3)
                    cerr << "svlVidCapSrcDC1394::Open - external trigger mode accepted" << endl;
#endif
                    ivalue = Trigger[DeviceID[i]].source;
                    if (ivalue > 3) {
                        ivalue = 0;
#if (__verbose__ >= 2)
                        cerr << "svlVidCapSrcDC1394::Open - unsupported trigger source; using source 0 instead" << endl;
#endif
                    }
                    if (dc1394_external_trigger_set_source(Cameras[DeviceID[i]],
                                                           static_cast<dc1394trigger_source_t>(ivalue + DC1394_TRIGGER_SOURCE_MIN)) == DC1394_SUCCESS) {
#if (__verbose__ >= 3)
                        cerr << "svlVidCapSrcDC1394::Open - external trigger source accepted" << endl;
#endif
                        ivalue = Trigger[DeviceID[i]].polarity;
                        if (ivalue > 1) {
                            ivalue = 1;
#if (__verbose__ >= 2)
                            cerr << "svlVidCapSrcDC1394::Open - unsupported trigger polarity; using high polarity instead" << endl;
#endif
                        }
                        if (dc1394_external_trigger_set_polarity(Cameras[DeviceID[i]],
                                                                 static_cast<dc1394trigger_polarity_t>(ivalue)) == DC1394_SUCCESS) {
#if (__verbose__ >= 3)
                            cerr << "svlVidCapSrcDC1394::Open - external trigger polarity accepted" << endl;
#endif
                        }
                        else {
#if (__verbose__ >= 2)
                            cerr << "svlVidCapSrcDC1394::Open - dc1394_external_trigger_set_polarity returned error" << endl;
                            cerr << "svlVidCapSrcDC1394::Open - continuing with default polarity" << endl;
#endif
                        }
                    }
                    else {
#if (__verbose__ >= 2)
                        cerr << "svlVidCapSrcDC1394::Open - dc1394_external_trigger_set_source returned error" << endl;
                        cerr << "svlVidCapSrcDC1394::Open - continuing with default source" << endl;
#endif
                    }
                }
                else {
#if (__verbose__ >= 2)
                    cerr << "svlVidCapSrcDC1394::Open - dc1394_external_trigger_set_mode returned error" << endl;
                    cerr << "svlVidCapSrcDC1394::Open - continuing with default mode" << endl;
#endif
                }
            }
            else {
#if (__verbose__ >= 2)
                cerr << "svlVidCapSrcDC1394::Open - dc1394_external_trigger_set_power returned error" << endl;
                cerr << "svlVidCapSrcDC1394::Open - continuing with internal clock" << endl;
#endif
            }
        }
        else {
            dc1394_external_trigger_set_power(Cameras[DeviceID[i]],
                                              DC1394_OFF);
#if (__verbose__ >= 3)
            cerr << "svlVidCapSrcDC1394::Open - external trigger disabled" << endl;
#endif
        }

        if (dc1394_capture_setup(Cameras[DeviceID[i]],
                                 FRAME_BUFFER_SIZE, DC1394_CAPTURE_FLAGS_DEFAULT) != DC1394_SUCCESS) {
#if (__verbose__ >= 1)
            cerr << "svlVidCapSrcDC1394::Open - dc1394_capture_setup returned error" << endl;
#endif
            goto labError;
        }
        // Get file descriptor for camera
        CameraFileNo[i] = dc1394_capture_get_fileno(Cameras[DeviceID[i]]);
        FD_SET(CameraFileNo[i], &(CameraFDSet[i]));

        if (dc1394_external_trigger_set_mode(Cameras[DeviceID[i]],
                                             DC1394_TRIGGER_MODE_0) != DC1394_SUCCESS) {
#if (__verbose__ >= 1)
            cerr << "svlVidCapSrcDC1394::Open - dc1394_external_trigger_set_mode returned error" << endl;
#endif
            goto labError;
        }

        // Setting defualt image properties
        svlFilterSourceVideoCaptureTypes::ImageProperties properties;
        memset(&properties, 0, sizeof(svlFilterSourceVideoCaptureTypes::ImageProperties));
        properties.mask = svlFilterSourceVideoCapture::propShutter & svlFilterSourceVideoCapture::propGain &
                          svlFilterSourceVideoCapture::propWhiteBalance & svlFilterSourceVideoCapture::propBrightness &
                          svlFilterSourceVideoCapture::propGamma & svlFilterSourceVideoCapture::propSaturation;
        SetImageProperties(properties, i);
    }

#if (__verbose__ >= 3)
    cerr << "svlVidCapSrcDC1394::Open - success" << endl;
#endif

    Initialized = true;
    return SVL_OK;

labError:
    Close();

#if (__verbose__ >= 1)
    cerr << "svlVidCapSrcDC1394::Open - failed" << endl;
#endif

    return SVL_FAIL;
}

void svlVidCapSrcDC1394::Close()
{
    Stop();
    for (unsigned int i = 0; i < NumOfStreams; i ++) {

        if (CameraFileNo[i]) {
            FD_CLR(CameraFileNo[i], &(CameraFDSet[i]));
        }

        if (OutputBuffer[i]) {
            delete OutputBuffer[i];
            OutputBuffer[i] = 0;
        }

        // Not sure if it is needed
        if (DeviceID[i] >= 0 && DeviceID[i] < static_cast<int>(NumberOfCameras) && Cameras[DeviceID[i]]) {
            dc1394_capture_stop(Cameras[DeviceID[i]]);
        }
    }
    Initialized = false;

#if (__verbose__ >= 3)
    cerr << "svlVidCapSrcDC1394::Close - done" << endl;
#endif
}

int svlVidCapSrcDC1394::Start()
{
    if (!Initialized) return SVL_FAIL;
    if (Running) return SVL_OK;

    unsigned int i, j;
    dc1394switch_t status;

    for (i = 0; i < NumOfStreams; i ++) {
        // setting transmission ON
        if (dc1394_video_set_transmission(Cameras[DeviceID[i]], DC1394_ON) != DC1394_SUCCESS) {
#if (__verbose__ >= 1)
            cerr << "svlVidCapSrcDC1394::Start - dc1394_video_set_transmission returned error" << endl;
#endif
            return SVL_FAIL;
        }
    }

    // waiting for transmission to start
#if (__verbose__ >= 3)
    cerr << "svlVidCapSrcDC1394::Start - waiting for transmission to start" << endl;
#endif

    Running = true;
    for (i = 0; i < NumOfStreams; i ++) {
        status = DC1394_OFF;
        for (j = 0; j < 30; j ++) {
            // Check whether transmission has started
            if (dc1394_video_get_transmission(Cameras[DeviceID[i]], &status) != DC1394_SUCCESS) {
#if (__verbose__ >= 1)
                cerr << "svlVidCapSrcDC1394::Start - dc1394_video_get_transmission returned error" << endl;
#endif
                Running = false;
                break;
            }
            if (status == DC1394_ON) {
#if (__verbose__ >= 3)
                cerr << "svlVidCapSrcDC1394::Start - capture started" << endl;
#endif
                break;
            }

            osaSleep(0.1); // = 100 millisec
        }
    }

    if (Running) {
        for (i = 0; i < NumOfStreams; i ++) {
            CaptureProc[i] = new svlVidCapSrcDC1394Thread(i);
            CaptureThread[i] = new osaThread;
            CaptureThread[i]->Create<svlVidCapSrcDC1394Thread, svlVidCapSrcDC1394*>(CaptureProc[i],
                                                                          &svlVidCapSrcDC1394Thread::Proc,
                                                                          this);
#if (__verbose__ >= 3)
            cerr << "svlVidCapSrcDC1394::Start - waiting for thread " << i << " to start" << endl;
#endif
            if (CaptureProc[i]->WaitForInit() == false) break;
#if (__verbose__ >= 3)
            cerr << "svlVidCapSrcDC1394::Start - thread " << i << " started" << endl;
#endif
        }
        if (i == NumOfStreams) return SVL_OK;
    }

#if (__verbose__ >= 1)
    cerr << "svlVidCapSrcDC1394::Start - failed: time out" << endl;
#endif
    Stop();
    return SVL_FAIL;
}

svlImageRGB* svlVidCapSrcDC1394::GetLatestFrame(bool waitfornew, unsigned int videoch)
{
    if (videoch >= NumOfStreams || !Initialized) return 0;
    return OutputBuffer[videoch]->Pull(waitfornew);
}

int svlVidCapSrcDC1394::Stop()
{
    if (!Initialized) return SVL_FAIL;

    unsigned int i;
    bool run = Running;

    Running = false;

    for (i = 0; i < NumOfStreams; i ++) {
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

    if (run) {
        for (i = 0; i < NumOfStreams; i ++) {
            dc1394_video_set_transmission(Cameras[DeviceID[i]], DC1394_OFF);
        }
    }

    return SVL_OK;
}

bool svlVidCapSrcDC1394::IsRunning()
{
    return Running;
}

int svlVidCapSrcDC1394::SetDevice(int devid, int CMN_UNUSED(inid), unsigned int videoch)
{
    if (videoch >= NumOfStreams) return SVL_FAIL;
    DeviceID[videoch] = devid;
    // InputID ignored

    // Reset device settings
    if (Format[videoch]) {
        delete Format[videoch];
        Format[videoch] = 0;
    }
    memset(&(Trigger[videoch]), 0, sizeof(svlFilterSourceVideoCaptureTypes::ExternalTrigger));

    return SVL_OK;
}

int svlVidCapSrcDC1394::GetWidth(unsigned int videoch)
{
    if (videoch >= NumOfStreams) return SVL_FAIL;
    return Width[videoch];
}

int svlVidCapSrcDC1394::GetHeight(unsigned int videoch)
{
    if (videoch >= NumOfStreams) return SVL_FAIL;
    return Height[videoch];
}

int svlVidCapSrcDC1394::CaptureFrame(unsigned int videoch)
{
    if (Running == false) return SVL_FAIL;

    // Wait 2 seconds for the file descriptor
#if (__verbose__ >= 4)
    cerr << "svlVidCapSrcDC1394::CaptureFrame - waiting for frame" << endl;
#endif
    timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    int ret = select(FD_SETSIZE, &(CameraFDSet[videoch]), 0, 0, &tv );
    if (ret == 0) { // timeout
#if (__verbose__ >= 1)
        cerr << "svlVidCapSrcDC1394::CaptureFrame - frame timeout (channel: " << videoch << ")" << endl;
#endif
        return SVL_FAIL;
    }
    else if (ret < 0) { // error
#if (__verbose__ >= 1)
        cerr << "svlVidCapSrcDC1394::CaptureFrame - error while waiting for frame" << endl;
#endif
        return SVL_FAIL;
    }

    // Acquire frame buffer
#if (__verbose__ >= 4)
    cerr << "svlVidCapSrcDC1394::CaptureFrame - dequeueing video frame from buffer..." << endl;
#endif
    if (dc1394_capture_dequeue(Cameras[DeviceID[videoch]],
                               DC1394_CAPTURE_POLICY_POLL,
                               &(Frame[videoch])) != DC1394_SUCCESS || Frame[videoch] == 0) {
#if (__verbose__ >= 1)
            cerr << "svlVidCapSrcDC1394::CaptureFrame - error while dequeuing frame" << endl;
#endif
        return SVL_FAIL;
    }

#if (CISST_OS != CISST_DARWIN)
    if (dc1394_capture_is_frame_corrupt(Cameras[DeviceID[videoch]], Frame[videoch]) == DC1394_TRUE) {
#if (__verbose__ >= 2)
            cerr << "svlVidCapSrcDC1394::CaptureFrame - captured frame is corrupt; skipping to next frame" << endl;
#endif
        dc1394_capture_enqueue(Cameras[DeviceID[videoch]], Frame[videoch]);
        return SVL_OK;
    }
#endif // (CISST_OS != CISST_DARWIN)

#if (__verbose__ >= 4)
    cerr << "svlVidCapSrcDC1394::CaptureFrame - video frame dequeued from buffer" << endl;
#endif

    // Color space conversions
    unsigned int yuvorder, rgborder;
    if (Format[videoch]) {
        rgborder = Format[videoch]->rgb_order;

        if (Format[videoch]->yuyv_order)
            yuvorder = DC1394_BYTE_ORDER_YUYV;
        else
            yuvorder = DC1394_BYTE_ORDER_UYVY;
    }
    else {
        // Default conversion settings
        yuvorder = DC1394_BYTE_ORDER_UYVY;
        rgborder = false;
    }

    if (ColorCoding[videoch] == DC1394_COLOR_CODING_RAW8) {
        dc1394_bayer_decoding_8bit(Frame[videoch]->image,
                                   OutputBuffer[videoch]->GetPushBuffer(),
                                   Width[videoch], Height[videoch],
                                   GetColorFilterFromPatternType(Format[videoch]->custom_pattern),
                                   DC1394_BAYER_METHOD_BILINEAR);
    }
    else {
        dc1394_convert_to_RGB8(Frame[videoch]->image,
                               OutputBuffer[videoch]->GetPushBuffer(),
                               Width[videoch], Height[videoch],
                               yuvorder,
                               (dc1394color_coding_t)ColorCoding[videoch],
                               16);
    }

#if (__verbose__ >= 4)
    cerr << "svlVidCapSrcDC1394::CaptureFrame - releasing frame buffer" << endl;
#endif
    // Release frame buffer
    dc1394_capture_enqueue(Cameras[DeviceID[videoch]], Frame[videoch]);

    if (rgborder) {
#if (__verbose__ >= 4)
        cerr << "svlVidCapSrcDC1394::CaptureFrame - converting between RGB and BGR" << endl;
#endif
        SwapRGBBuffer(OutputBuffer[videoch]->GetPushBuffer(), Width[videoch] * Height[videoch]);
    }

    // Add image to the output buffer
    OutputBuffer[videoch]->Push();

#if (__verbose__ >= 4)
    cerr << "svlVidCapSrcDC1394::CaptureFrame - video frame captured" << endl;
#endif

    return SVL_OK;
}

int svlVidCapSrcDC1394::GetFormatList(unsigned int deviceid, svlFilterSourceVideoCaptureTypes::ImageFormat **formatlist)
{
    if (deviceid >= NumberOfCameras || formatlist == 0 || Cameras == 0) return SVL_FAIL;

    dc1394video_modes_t modes;
    if (dc1394_video_get_supported_modes(Cameras[deviceid], &modes) != DC1394_SUCCESS) return SVL_FAIL;

    // Allocate memory for format array
    // CALLER HAS TO FREE UP THIS ARRAY!!!
    unsigned int listsize = modes.num + DC1394_VIDEO_MODE_FORMAT7_NUM;
    unsigned int validlistsize = listsize;
    svlFilterSourceVideoCaptureTypes::ImageFormat *templist = new svlFilterSourceVideoCaptureTypes::ImageFormat[listsize];
    double *fpslist;
    unsigned int fpslistsize, i, j, k, l;

#if (__verbose__ >= 4)
    cerr << "svlVidCapSrcDC1394::GetFormatList - modes.num = " << modes.num << "; listsize = " << listsize << endl;
#endif

    for (i = 0; i < modes.num; i ++) {
        if (GetFormatFromMode(modes.modes[i], templist[i]) != SVL_OK) {
            templist[i].width = -1;
            templist[i].height = -1;
            templist[i].colorspace = svlFilterSourceVideoCaptureTypes::PixelUnknown;
            validlistsize --;
            continue;
        }
        templist[i].rgb_order = false;
        templist[i].yuyv_order = false;
        templist[i].custom_mode = -1;
        templist[i].custom_roileft = 0;
        templist[i].custom_roitop = 0;
        templist[i].custom_framerate = 0;
        templist[i].custom_pattern = svlFilterSourceVideoCaptureTypes::PatternUnknown;
        templist[i].custom_maxwidth = 0;
        templist[i].custom_maxheight = 0;
        templist[i].custom_unitwidth = 0;
        templist[i].custom_unitheight = 0;
        templist[i].custom_unitleft = 0;
        templist[i].custom_unittop = 0;
        templist[i].custom_colorspaces[0] = svlFilterSourceVideoCaptureTypes::PixelUnknown;

        fpslistsize = 0;
        GetSupportedFrameratesForFormat(deviceid, templist[i], &fpslist, fpslistsize);
        if (fpslistsize > 0) templist[i].framerate = fpslist[fpslistsize - 1];
        else templist[i].framerate = -1.0;
        delete [] fpslist;
    }

    // Get Format7 modes and parameters
    dc1394format7modeset_t f7modes;
    memset(&f7modes, 0, sizeof(dc1394format7modeset_t));
    dc1394_format7_get_modeset(Cameras[deviceid], &f7modes);
    svlFilterSourceVideoCaptureTypes::PixelType cstype;

    for (j = 0; i < listsize; i ++, j ++) {
#if (__verbose__ >= 4)
        cerr << "svlVidCapSrcDC1394::GetFormatList - Format7 mode " << j;
#endif
        if (f7modes.mode[j].present) {
            templist[i].width = f7modes.mode[j].size_x;
            templist[i].height = f7modes.mode[j].size_y;
            templist[i].colorspace = GetPixelTypeFromColorCoding(f7modes.mode[j].color_coding);
            templist[i].rgb_order = false;
            templist[i].yuyv_order = false;
            templist[i].custom_mode = j;
            templist[i].custom_roileft = f7modes.mode[j].pos_x;
            templist[i].custom_roitop = f7modes.mode[j].pos_y;
            templist[i].custom_framerate = 100;
            templist[i].custom_pattern = GetPatternTypeFromColorFilter(f7modes.mode[j].color_filter);
            templist[i].custom_maxwidth = f7modes.mode[j].max_size_x;
            templist[i].custom_maxheight = f7modes.mode[j].max_size_y;
            templist[i].custom_unitwidth = f7modes.mode[j].unit_size_x;
            templist[i].custom_unitheight = f7modes.mode[j].unit_size_y;
            templist[i].custom_unitleft = f7modes.mode[j].unit_pos_x;
            templist[i].custom_unittop = f7modes.mode[j].unit_pos_y;
            for (l = 0, k = 0; k < f7modes.mode[j].color_codings.num; k ++) {
                cstype = GetPixelTypeFromColorCoding(f7modes.mode[j].color_codings.codings[k]);
                if (cstype != svlFilterSourceVideoCaptureTypes::PixelUnknown) {
                    templist[i].custom_colorspaces[l] = cstype;
                    l ++;
                }
            }
            // Set the rest of the list to invalid color space
            for (k = l; k < svlFilterSourceVideoCaptureTypes::PixelTypeCount; k ++) {
                templist[i].custom_colorspaces[k] = svlFilterSourceVideoCaptureTypes::PixelUnknown;
            }
            if (l < 1) {
                templist[i].width = -1;
                templist[i].height = -1;
                validlistsize --;
#if (__verbose__ >= 4)
                cerr << " error: invalid color space" << endl;
#endif
            }
            else {
#if (__verbose__ >= 4)
                cerr << " success" << endl;
#endif
            }
        }
        else {
            templist[i].width = -1;
            templist[i].height = -1;
            templist[i].colorspace = svlFilterSourceVideoCaptureTypes::PixelUnknown;
            validlistsize --;
#if (__verbose__ >= 4)
            cerr << " error: not implemented" << endl;
#endif
        }
    }

#if (__verbose__ >= 4)
    cerr << "svlVidCapSrcDC1394::GetFormatList - validlistsize = " << validlistsize << endl;
#endif

    formatlist[0] = new svlFilterSourceVideoCaptureTypes::ImageFormat[validlistsize];
    for (i = 0, j = 0; i < listsize && j < validlistsize; i ++) {
        if (templist[i].width > 0 && templist[i].height > 0 &&
            templist[i].colorspace != svlFilterSourceVideoCaptureTypes::PixelUnknown) {
            memcpy(formatlist[0] + j, templist + i, sizeof(svlFilterSourceVideoCaptureTypes::ImageFormat));
            j ++;
        }
    }

    delete [] templist;

    return validlistsize;
}

int svlVidCapSrcDC1394::SetFormat(svlFilterSourceVideoCaptureTypes::ImageFormat& format, unsigned int videoch)
{
    if (videoch >= NumOfStreams || Initialized) return SVL_FAIL;

    if (Format[videoch] == 0) Format[videoch] = new svlFilterSourceVideoCaptureTypes::ImageFormat;
    memcpy(Format[videoch], &format, sizeof(svlFilterSourceVideoCaptureTypes::ImageFormat));

    return SVL_OK;
}

int svlVidCapSrcDC1394::GetFormat(svlFilterSourceVideoCaptureTypes::ImageFormat& format, unsigned int videoch)
{
    if (videoch >= NumOfStreams || Initialized || Format[videoch] == 0) return SVL_FAIL;

    memcpy(&format, Format[videoch], sizeof(svlFilterSourceVideoCaptureTypes::ImageFormat));

    return SVL_OK;
}

int svlVidCapSrcDC1394::SetImageProperties(svlFilterSourceVideoCaptureTypes::ImageProperties& properties, unsigned int videoch)
{
    if (videoch >= NumOfStreams) return SVL_FAIL;

    dc1394feature_mode_t mode;

    Mutex.Lock();

    if (Initialized) {
        // Setting modes and values
        if (properties.mask & svlFilterSourceVideoCapture::propShutter) {
            if (properties.manual & svlFilterSourceVideoCapture::propShutter) mode = DC1394_FEATURE_MODE_MANUAL;
            else mode = DC1394_FEATURE_MODE_AUTO;
            dc1394_feature_set_mode(Cameras[DeviceID[videoch]], DC1394_FEATURE_SHUTTER, mode);
            dc1394_feature_set_value(Cameras[DeviceID[videoch]], DC1394_FEATURE_SHUTTER, properties.shutter);
        }
        if (properties.mask & svlFilterSourceVideoCapture::propGain) {
            if (properties.manual & svlFilterSourceVideoCapture::propGain) mode = DC1394_FEATURE_MODE_MANUAL;
            else mode = DC1394_FEATURE_MODE_AUTO;
            dc1394_feature_set_mode(Cameras[DeviceID[videoch]], DC1394_FEATURE_GAIN, mode);
            dc1394_feature_set_value(Cameras[DeviceID[videoch]], DC1394_FEATURE_GAIN, properties.gain);
        }
        if (properties.mask & svlFilterSourceVideoCapture::propWhiteBalance) {
            if (properties.manual & svlFilterSourceVideoCapture::propWhiteBalance) mode = DC1394_FEATURE_MODE_MANUAL;
            else mode = DC1394_FEATURE_MODE_AUTO;
            dc1394_feature_set_mode(Cameras[DeviceID[videoch]], DC1394_FEATURE_WHITE_BALANCE, mode);
            dc1394_feature_whitebalance_set_value(Cameras[DeviceID[videoch]], properties.wb_u_b, properties.wb_v_r);
        }
        if (properties.mask & svlFilterSourceVideoCapture::propGamma) {
            if (properties.manual & svlFilterSourceVideoCapture::propGamma) mode = DC1394_FEATURE_MODE_MANUAL;
            else mode = DC1394_FEATURE_MODE_AUTO;
            dc1394_feature_set_mode(Cameras[DeviceID[videoch]], DC1394_FEATURE_GAMMA, mode);
            dc1394_feature_set_value(Cameras[DeviceID[videoch]], DC1394_FEATURE_GAMMA, properties.gamma);
        }
        if (properties.mask & svlFilterSourceVideoCapture::propBrightness) {
            if (properties.manual & svlFilterSourceVideoCapture::propBrightness) mode = DC1394_FEATURE_MODE_MANUAL;
            else mode = DC1394_FEATURE_MODE_AUTO;
            dc1394_feature_set_mode(Cameras[DeviceID[videoch]], DC1394_FEATURE_BRIGHTNESS, mode);
            dc1394_feature_set_value(Cameras[DeviceID[videoch]], DC1394_FEATURE_BRIGHTNESS, properties.brightness);
        }
        if (properties.mask & svlFilterSourceVideoCapture::propSaturation) {
            if (properties.manual & svlFilterSourceVideoCapture::propSaturation) mode = DC1394_FEATURE_MODE_MANUAL;
            else mode = DC1394_FEATURE_MODE_AUTO;
            dc1394_feature_set_mode(Cameras[DeviceID[videoch]], DC1394_FEATURE_SATURATION, mode);
            dc1394_feature_set_value(Cameras[DeviceID[videoch]], DC1394_FEATURE_SATURATION, properties.saturation);
        }
    }

    Mutex.Unlock();

    return SVL_OK;
}

int svlVidCapSrcDC1394::GetImageProperties(svlFilterSourceVideoCaptureTypes::ImageProperties& properties, unsigned int videoch)
{
    if (videoch >= NumOfStreams) return SVL_FAIL;
    if (!Initialized) return SVL_FAIL;

    dc1394feature_mode_t mode;

    memset(&properties, 0, sizeof(svlFilterSourceVideoCaptureTypes::ImageProperties));
    properties.mask = svlFilterSourceVideoCapture::propShutter & svlFilterSourceVideoCapture::propGain &
                      svlFilterSourceVideoCapture::propWhiteBalance & svlFilterSourceVideoCapture::propBrightness &
                      svlFilterSourceVideoCapture::propGamma & svlFilterSourceVideoCapture::propSaturation;

    Mutex.Lock();

    // Getting AUTO/MANUAL modes
    dc1394_feature_get_mode(Cameras[DeviceID[videoch]], DC1394_FEATURE_SHUTTER, &mode);
    if (mode == DC1394_FEATURE_MODE_MANUAL) properties.manual += svlFilterSourceVideoCapture::propShutter;
    dc1394_feature_get_mode(Cameras[DeviceID[videoch]], DC1394_FEATURE_GAIN, &mode);
    if (mode == DC1394_FEATURE_MODE_MANUAL) properties.manual += svlFilterSourceVideoCapture::propGain;
    dc1394_feature_get_mode(Cameras[DeviceID[videoch]], DC1394_FEATURE_WHITE_BALANCE, &mode);
    if (mode == DC1394_FEATURE_MODE_MANUAL) properties.manual += svlFilterSourceVideoCapture::propWhiteBalance;
    dc1394_feature_get_mode(Cameras[DeviceID[videoch]], DC1394_FEATURE_GAMMA, &mode);
    if (mode == DC1394_FEATURE_MODE_MANUAL) properties.manual += svlFilterSourceVideoCapture::propGamma;
    dc1394_feature_get_mode(Cameras[DeviceID[videoch]], DC1394_FEATURE_BRIGHTNESS, &mode);
    if (mode == DC1394_FEATURE_MODE_MANUAL) properties.manual += svlFilterSourceVideoCapture::propBrightness;
    dc1394_feature_get_mode(Cameras[DeviceID[videoch]], DC1394_FEATURE_SATURATION, &mode);
    if (mode == DC1394_FEATURE_MODE_MANUAL) properties.manual += svlFilterSourceVideoCapture::propSaturation;

    // Getting property values
    dc1394_feature_get_value(Cameras[DeviceID[videoch]], DC1394_FEATURE_SHUTTER, &(properties.shutter));
    dc1394_feature_get_value(Cameras[DeviceID[videoch]], DC1394_FEATURE_GAIN, &(properties.gain));
    dc1394_feature_whitebalance_get_value(Cameras[DeviceID[videoch]], &(properties.wb_u_b), &(properties.wb_v_r));
    dc1394_feature_get_value(Cameras[DeviceID[videoch]], DC1394_FEATURE_GAMMA, &(properties.gamma));
    dc1394_feature_get_value(Cameras[DeviceID[videoch]], DC1394_FEATURE_BRIGHTNESS, &(properties.brightness));
    dc1394_feature_get_value(Cameras[DeviceID[videoch]], DC1394_FEATURE_SATURATION, &(properties.saturation));

    Mutex.Unlock();

    return SVL_OK;
}

int svlVidCapSrcDC1394::SetTrigger(svlFilterSourceVideoCaptureTypes::ExternalTrigger & trigger, unsigned int videoch)
{
    if (videoch >= NumOfStreams || Initialized) return SVL_FAIL;

    memcpy(&(Trigger[videoch]), &trigger, sizeof(svlFilterSourceVideoCaptureTypes::ExternalTrigger));

    return SVL_OK;
}

int svlVidCapSrcDC1394::GetTrigger(svlFilterSourceVideoCaptureTypes::ExternalTrigger & trigger, unsigned int videoch)
{
    if (videoch >= NumOfStreams) return SVL_FAIL;

    memcpy(&trigger, &(Trigger[videoch]), sizeof(svlFilterSourceVideoCaptureTypes::ExternalTrigger));

    return SVL_OK;
}

void svlVidCapSrcDC1394::Release()
{
    Close();

    unsigned int i;
    if (CaptureProc) delete [] CaptureProc;
    if (CaptureThread) delete [] CaptureThread;
    if (CameraFileNo) delete [] CameraFileNo;
    if (CameraFDSet) delete [] CameraFDSet;
    if (DeviceID) delete [] DeviceID;
    if (Format) {
        for (i = 0; i < NumOfStreams; i ++) {
            if (Format[i]) delete Format[i];
        }
        delete [] Format;
    }
    if (Trigger) delete [] Trigger;
    if (ColorCoding) delete [] ColorCoding;
    if (Frame) delete [] Frame;
    if (Width) delete [] Width;
    if (Height) delete [] Height;
    if (OutputBuffer) delete [] OutputBuffer;

    NumOfStreams = 0;
    Initialized = false;
    Running = false;
    CaptureProc = 0;
    CaptureThread = 0;
    NumOfStreams = 0;
    CameraFileNo = 0;
    CameraFDSet = 0;
    DeviceID = 0;
    Format = 0;
    ColorCoding = 0;
    Frame = 0;
    Width = 0;
    Height = 0;
    OutputBuffer = 0;
}

int svlVidCapSrcDC1394::GetModeFromFormat(unsigned int width, unsigned int height, svlFilterSourceVideoCaptureTypes::PixelType colspc, unsigned int& mode)
{
    if (colspc == svlFilterSourceVideoCaptureTypes::PixelRAW8 ||
        colspc == svlFilterSourceVideoCaptureTypes::PixelRAW16) {
        // No corresponding mode exists
        // Supported only in Format7
        return SVL_FAIL;
    }

    if (width == 160 && height == 120) {
        if (colspc == svlFilterSourceVideoCaptureTypes::PixelYUV444) mode = DC1394_VIDEO_MODE_160x120_YUV444;
        else return SVL_FAIL;
    }
    else if (width == 320 && height == 240) {
        if (colspc == svlFilterSourceVideoCaptureTypes::PixelYUV422) mode = DC1394_VIDEO_MODE_320x240_YUV422;
        else return SVL_FAIL;
    }
    else if (width == 640 && height == 480) {
        switch (colspc) {
            case svlFilterSourceVideoCaptureTypes::PixelYUV411:
                mode = DC1394_VIDEO_MODE_640x480_YUV411;
            break;

            case svlFilterSourceVideoCaptureTypes::PixelYUV422:
                mode = DC1394_VIDEO_MODE_640x480_YUV422;
            break;

            case svlFilterSourceVideoCaptureTypes::PixelRGB8:
                mode = DC1394_VIDEO_MODE_640x480_RGB8;
            break;

            case svlFilterSourceVideoCaptureTypes::PixelMONO8:
                mode = DC1394_VIDEO_MODE_640x480_MONO8;
            break;

            case svlFilterSourceVideoCaptureTypes::PixelMONO16:
                mode = DC1394_VIDEO_MODE_640x480_MONO16;
            break;

            case svlFilterSourceVideoCaptureTypes::PixelRAW8:
            case svlFilterSourceVideoCaptureTypes::PixelRAW16:
            case svlFilterSourceVideoCaptureTypes::PixelYUV444:
            case svlFilterSourceVideoCaptureTypes::PixelUnknown:
            default:
                return SVL_FAIL;
        }
    }
    else if (width == 800 && height == 600) {
        switch (colspc) {
            case svlFilterSourceVideoCaptureTypes::PixelYUV422:
                mode = DC1394_VIDEO_MODE_800x600_YUV422;
            break;

            case svlFilterSourceVideoCaptureTypes::PixelRGB8:
                mode = DC1394_VIDEO_MODE_800x600_RGB8;
            break;

            case svlFilterSourceVideoCaptureTypes::PixelMONO8:
                mode = DC1394_VIDEO_MODE_800x600_MONO8;
            break;

            case svlFilterSourceVideoCaptureTypes::PixelMONO16:
                mode = DC1394_VIDEO_MODE_800x600_MONO16;
            break;

            case svlFilterSourceVideoCaptureTypes::PixelRAW8:
            case svlFilterSourceVideoCaptureTypes::PixelRAW16:
            case svlFilterSourceVideoCaptureTypes::PixelYUV411:
            case svlFilterSourceVideoCaptureTypes::PixelYUV444:
            case svlFilterSourceVideoCaptureTypes::PixelUnknown:
            default:
                return SVL_FAIL;
        }
    }
    else if (width == 1024 && height == 768) {
        switch (colspc) {
            case svlFilterSourceVideoCaptureTypes::PixelYUV422:
                mode = DC1394_VIDEO_MODE_1024x768_YUV422;
            break;

            case svlFilterSourceVideoCaptureTypes::PixelRGB8:
                mode = DC1394_VIDEO_MODE_1024x768_RGB8;
            break;

            case svlFilterSourceVideoCaptureTypes::PixelMONO8:
                mode = DC1394_VIDEO_MODE_1024x768_MONO8;
            break;

            case svlFilterSourceVideoCaptureTypes::PixelMONO16:
                mode = DC1394_VIDEO_MODE_1024x768_MONO16;
            break;

            case svlFilterSourceVideoCaptureTypes::PixelRAW8:
            case svlFilterSourceVideoCaptureTypes::PixelRAW16:
            case svlFilterSourceVideoCaptureTypes::PixelYUV411:
            case svlFilterSourceVideoCaptureTypes::PixelYUV444:
            case svlFilterSourceVideoCaptureTypes::PixelUnknown:
            default:
                return SVL_FAIL;
        }
    }
    else if (width == 1280 && height == 960) {
        switch (colspc) {
            case svlFilterSourceVideoCaptureTypes::PixelYUV422:
                mode = DC1394_VIDEO_MODE_1280x960_YUV422;
            break;

            case svlFilterSourceVideoCaptureTypes::PixelRGB8:
                mode = DC1394_VIDEO_MODE_1280x960_RGB8;
            break;

            case svlFilterSourceVideoCaptureTypes::PixelMONO8:
                mode = DC1394_VIDEO_MODE_1280x960_MONO8;
            break;

            case svlFilterSourceVideoCaptureTypes::PixelMONO16:
                mode = DC1394_VIDEO_MODE_1280x960_MONO16;
            break;

            case svlFilterSourceVideoCaptureTypes::PixelRAW8:
            case svlFilterSourceVideoCaptureTypes::PixelRAW16:
            case svlFilterSourceVideoCaptureTypes::PixelYUV411:
            case svlFilterSourceVideoCaptureTypes::PixelYUV444:
            case svlFilterSourceVideoCaptureTypes::PixelUnknown:
            default:
                return SVL_FAIL;
        }
    }
    else if (width == 1600 && height == 1200) {
        switch (colspc) {
            case svlFilterSourceVideoCaptureTypes::PixelYUV422:
                mode = DC1394_VIDEO_MODE_1600x1200_YUV422;
            break;

            case svlFilterSourceVideoCaptureTypes::PixelRGB8:
                mode = DC1394_VIDEO_MODE_1600x1200_RGB8;
            break;

            case svlFilterSourceVideoCaptureTypes::PixelMONO8:
                mode = DC1394_VIDEO_MODE_1600x1200_MONO8;
            break;

            case svlFilterSourceVideoCaptureTypes::PixelMONO16:
                mode = DC1394_VIDEO_MODE_1600x1200_MONO16;
            break;

            case svlFilterSourceVideoCaptureTypes::PixelRAW8:
            case svlFilterSourceVideoCaptureTypes::PixelRAW16:
            case svlFilterSourceVideoCaptureTypes::PixelYUV411:
            case svlFilterSourceVideoCaptureTypes::PixelYUV444:
            case svlFilterSourceVideoCaptureTypes::PixelUnknown:
            default:
                return SVL_FAIL;
        }
    }
    else return SVL_FAIL;

    return SVL_OK;
}

int svlVidCapSrcDC1394::GetSupportedFrameratesForFormat(unsigned int devid, svlFilterSourceVideoCaptureTypes::ImageFormat& format, double **fpslist, unsigned int& listsize)
{
    if (fpslist == 0 || Cameras == 0 || devid >= NumberOfCameras) return SVL_FAIL;

    unsigned int mode;
    if (GetModeFromFormat(format.width, format.height, format.colorspace, mode) != 0) return SVL_FAIL;

    dc1394framerates_t framerates;
    if(dc1394_video_get_supported_framerates(Cameras[devid], (dc1394video_mode_t)mode, &framerates) != DC1394_SUCCESS) return SVL_FAIL;

    // Allocate memory for framerate array
    // CALLER HAS TO FREE UP THIS ARRAY!!!
    listsize = framerates.num;
    fpslist[0] = new double[listsize];

    float fps;
    unsigned int rate;
    for (unsigned int i = 0; i < listsize; i ++) {
        rate = framerates.framerates[i];
        dc1394_framerate_as_float((dc1394framerate_t)rate, &fps);
        fpslist[0][i] = static_cast<double>(fps);
    }

    return SVL_OK;
}

int svlVidCapSrcDC1394::GetFramerateFromFPS(double fps, unsigned int& framerate)
{
    if (fps < 1.0 || fps > 240.0) return SVL_FAIL;

    double rates[] = {1.875, 3.75, 7.5, 15.0, 30.0, 60.0, 120.0, 240.0};
    unsigned int i, bestmatch = 0;
    float diff, mindiff = 1000;

    for (i = 0; i < 8; i ++) {
        diff = fps - rates[i];
        if (diff < 0.0) diff = -diff;
        if (diff < mindiff) {
            bestmatch = i;
            mindiff = diff;
        }
    }
    if (mindiff > 60.0) return SVL_FAIL;

    switch (bestmatch) {
        case 0:
            framerate = DC1394_FRAMERATE_1_875;
        break;

        case 1:
            framerate = DC1394_FRAMERATE_3_75;
        break;

        case 2:
            framerate = DC1394_FRAMERATE_7_5;
        break;

        case 3:
            framerate = DC1394_FRAMERATE_15;
        break;

        case 4:
            framerate = DC1394_FRAMERATE_30;
        break;

        case 5:
            framerate = DC1394_FRAMERATE_60;
        break;

        case 6:
            framerate = DC1394_FRAMERATE_120;
        break;

        case 7:
            framerate = DC1394_FRAMERATE_240;
        break;

        default:
            framerate = DC1394_FRAMERATE_30;
    }

    return SVL_OK;
}

int svlVidCapSrcDC1394::GetFormatFromMode(unsigned int mode, svlFilterSourceVideoCaptureTypes::ImageFormat& format)
{
    switch (mode) {
        case DC1394_VIDEO_MODE_160x120_YUV444:
            format.width = 160;
            format.height = 120;
            format.colorspace = svlFilterSourceVideoCaptureTypes::PixelYUV444;
        break;

        case DC1394_VIDEO_MODE_320x240_YUV422:
            format.width = 320;
            format.height = 240;
            format.colorspace = svlFilterSourceVideoCaptureTypes::PixelYUV422;
        break;

        case DC1394_VIDEO_MODE_640x480_YUV411:
            format.width = 640;
            format.height = 480;
            format.colorspace = svlFilterSourceVideoCaptureTypes::PixelYUV411;
        break;

        case DC1394_VIDEO_MODE_640x480_YUV422:
            format.width = 640;
            format.height = 480;
            format.colorspace = svlFilterSourceVideoCaptureTypes::PixelYUV422;
        break;

        case DC1394_VIDEO_MODE_640x480_RGB8:
            format.width = 640;
            format.height = 480;
            format.colorspace = svlFilterSourceVideoCaptureTypes::PixelRGB8;
        break;

        case DC1394_VIDEO_MODE_640x480_MONO8:
            format.width = 640;
            format.height = 480;
            format.colorspace = svlFilterSourceVideoCaptureTypes::PixelMONO8;
        break;

        case DC1394_VIDEO_MODE_640x480_MONO16:
            format.width = 640;
            format.height = 480;
            format.colorspace = svlFilterSourceVideoCaptureTypes::PixelMONO16;
        break;

        case DC1394_VIDEO_MODE_800x600_YUV422:
            format.width = 800;
            format.height = 600;
            format.colorspace = svlFilterSourceVideoCaptureTypes::PixelYUV422;
        break;

        case DC1394_VIDEO_MODE_800x600_RGB8:
            format.width = 800;
            format.height = 600;
            format.colorspace = svlFilterSourceVideoCaptureTypes::PixelRGB8;
        break;

        case DC1394_VIDEO_MODE_800x600_MONO8:
            format.width = 800;
            format.height = 600;
            format.colorspace = svlFilterSourceVideoCaptureTypes::PixelMONO8;
        break;

        case DC1394_VIDEO_MODE_1024x768_YUV422:
            format.width = 1024;
            format.height = 768;
            format.colorspace = svlFilterSourceVideoCaptureTypes::PixelYUV422;
        break;

        case DC1394_VIDEO_MODE_1024x768_RGB8:
            format.width = 1024;
            format.height = 768;
            format.colorspace = svlFilterSourceVideoCaptureTypes::PixelRGB8;
        break;

        case DC1394_VIDEO_MODE_1024x768_MONO8:
            format.width = 1024;
            format.height = 768;
            format.colorspace = svlFilterSourceVideoCaptureTypes::PixelMONO8;
        break;

        case DC1394_VIDEO_MODE_800x600_MONO16:
            format.width = 800;
            format.height = 600;
            format.colorspace = svlFilterSourceVideoCaptureTypes::PixelMONO16;
        break;

        case DC1394_VIDEO_MODE_1024x768_MONO16:
            format.width = 1024;
            format.height = 768;
            format.colorspace = svlFilterSourceVideoCaptureTypes::PixelMONO16;
        break;

        case DC1394_VIDEO_MODE_1280x960_YUV422:
            format.width = 1280;
            format.height = 960;
            format.colorspace = svlFilterSourceVideoCaptureTypes::PixelYUV422;
        break;

        case DC1394_VIDEO_MODE_1280x960_RGB8:
            format.width = 1280;
            format.height = 960;
            format.colorspace = svlFilterSourceVideoCaptureTypes::PixelRGB8;
        break;

        case DC1394_VIDEO_MODE_1280x960_MONO8:
            format.width = 1280;
            format.height = 960;
            format.colorspace = svlFilterSourceVideoCaptureTypes::PixelMONO8;
        break;

        case DC1394_VIDEO_MODE_1600x1200_YUV422:
            format.width = 1600;
            format.height = 1200;
            format.colorspace = svlFilterSourceVideoCaptureTypes::PixelYUV422;
        break;

        case DC1394_VIDEO_MODE_1600x1200_RGB8:
            format.width = 1600;
            format.height = 1200;
            format.colorspace = svlFilterSourceVideoCaptureTypes::PixelRGB8;
        break;

        case DC1394_VIDEO_MODE_1600x1200_MONO8:
            format.width = 1600;
            format.height = 1200;
            format.colorspace = svlFilterSourceVideoCaptureTypes::PixelMONO8;
        break;

        case DC1394_VIDEO_MODE_1280x960_MONO16:
            format.width = 1280;
            format.height = 960;
            format.colorspace = svlFilterSourceVideoCaptureTypes::PixelMONO16;
        break;

        case DC1394_VIDEO_MODE_1600x1200_MONO16:
            format.width = 1600;
            format.height = 1200;
            format.colorspace = svlFilterSourceVideoCaptureTypes::PixelMONO16;
        break;

        default:
            return SVL_FAIL;
    }

    return SVL_OK;
}

dc1394color_coding_t svlVidCapSrcDC1394::GetColorCodingFromPixelType(svlFilterSourceVideoCaptureTypes::PixelType pixeltype)
{
    switch (pixeltype) {
        case svlFilterSourceVideoCaptureTypes::PixelRAW8:
            return DC1394_COLOR_CODING_RAW8;

        case svlFilterSourceVideoCaptureTypes::PixelRAW16:
            return DC1394_COLOR_CODING_RAW16;

        case svlFilterSourceVideoCaptureTypes::PixelRGB8:
            return DC1394_COLOR_CODING_RGB8;

        case svlFilterSourceVideoCaptureTypes::PixelYUV444:
            return DC1394_COLOR_CODING_YUV444;

        case svlFilterSourceVideoCaptureTypes::PixelYUV422:
            return DC1394_COLOR_CODING_YUV422;

        case svlFilterSourceVideoCaptureTypes::PixelYUV411:
            return DC1394_COLOR_CODING_YUV411;

        case svlFilterSourceVideoCaptureTypes::PixelMONO8:
            return DC1394_COLOR_CODING_MONO8;

        case svlFilterSourceVideoCaptureTypes::PixelMONO16:
            return DC1394_COLOR_CODING_MONO16;

        case svlFilterSourceVideoCaptureTypes::PixelUnknown:
        default:
        break;
    }
    return DC1394_COLOR_CODING_RAW8;
}

svlFilterSourceVideoCaptureTypes::PixelType svlVidCapSrcDC1394::GetPixelTypeFromColorCoding(dc1394color_coding_t colorcoding)
{
    switch (colorcoding) {
        case DC1394_COLOR_CODING_MONO8:
            return svlFilterSourceVideoCaptureTypes::PixelMONO8;
            
        case DC1394_COLOR_CODING_YUV411:
            return svlFilterSourceVideoCaptureTypes::PixelYUV411;
            
        case DC1394_COLOR_CODING_YUV422:
            return svlFilterSourceVideoCaptureTypes::PixelYUV422;
            
        case DC1394_COLOR_CODING_RGB8:
            return svlFilterSourceVideoCaptureTypes::PixelRGB8;
            
        case DC1394_COLOR_CODING_MONO16:
            return svlFilterSourceVideoCaptureTypes::PixelMONO16;
            
        case DC1394_COLOR_CODING_RAW8:
            return svlFilterSourceVideoCaptureTypes::PixelRAW8;
            
        case DC1394_COLOR_CODING_RAW16:
            return svlFilterSourceVideoCaptureTypes::PixelRAW16;
            
        case DC1394_COLOR_CODING_RGB16:
        case DC1394_COLOR_CODING_MONO16S:
        case DC1394_COLOR_CODING_RGB16S:
        default:
        break;
    }
    return svlFilterSourceVideoCaptureTypes::PixelUnknown;
}

dc1394color_filter_t svlVidCapSrcDC1394::GetColorFilterFromPatternType(svlFilterSourceVideoCaptureTypes::PatternType patterntype)
{
    switch (patterntype) {
        case svlFilterSourceVideoCaptureTypes::PatternRGGB:
            return DC1394_COLOR_FILTER_RGGB;

        case svlFilterSourceVideoCaptureTypes::PatternGBRG:
            return DC1394_COLOR_FILTER_GBRG;

        case svlFilterSourceVideoCaptureTypes::PatternGRBG:
            return DC1394_COLOR_FILTER_GRBG;

        case svlFilterSourceVideoCaptureTypes::PatternBGGR:
            return DC1394_COLOR_FILTER_BGGR;
    
        case svlFilterSourceVideoCaptureTypes::PatternUnknown:
        default:
            break;
    }
    return DC1394_COLOR_FILTER_RGGB;
}

svlFilterSourceVideoCaptureTypes::PatternType svlVidCapSrcDC1394::GetPatternTypeFromColorFilter(dc1394color_filter_t colorfilter)
{
    switch (colorfilter) {
        case DC1394_COLOR_FILTER_RGGB:
            return svlFilterSourceVideoCaptureTypes::PatternRGGB;
            
        case DC1394_COLOR_FILTER_GBRG:
            return svlFilterSourceVideoCaptureTypes::PatternGBRG;
            
        case DC1394_COLOR_FILTER_GRBG:
            return svlFilterSourceVideoCaptureTypes::PatternGRBG;
            
        case DC1394_COLOR_FILTER_BGGR:
            return svlFilterSourceVideoCaptureTypes::PatternBGGR;

        default:
            break;
    }
    return svlFilterSourceVideoCaptureTypes::PatternUnknown;
}

void svlVidCapSrcDC1394::SwapRGBBuffer(unsigned char* buffer, const unsigned int numberofpixels)
{
    unsigned char colval;
    unsigned char* r = buffer;
    unsigned char* b = r + 2;
    for (unsigned int i = 0; i < numberofpixels; i ++) {
       colval = *r;
       *r = *b;
       *b = colval;
       r += 3;
       b += 3;
    }
}


/****************************************/
/*** svlVidCapSrcDC1394Thread class *****/
/****************************************/

void* svlVidCapSrcDC1394Thread::Proc(svlVidCapSrcDC1394* baseref)
{
    // signal success to main thread
    Error = false;
    InitSuccess = true;
    InitEvent.Raise();

    while (baseref->Running) {
        if (baseref->CaptureFrame(StreamID) != SVL_OK) {
            Error = true;
            break;
        }
        osaSleep(0.005);
    }

	return this;
}

