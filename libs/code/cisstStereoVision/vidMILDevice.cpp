/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

  Author(s):  Balazs Vagvolgyi
  Created on: 2009 

  (C) Copyright 2006-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include "vidMILDevice.h"

using namespace std;


/***************************/
/*** Function prototypes ***/
/***************************/

MIL_INT MFTYPE MILProcessingCallback(MIL_INT HookType, MIL_ID HookId, void MPTYPE *HookDataPtr);


/*************************************/
/*** CMILDevice class ****************/
/*************************************/

CMILDevice::CMILDevice() :
    CVideoCaptureSourceBase(),
    NumOfStreams(0),
    Initialized(false),
    Running(false),
    DeviceID(0),
    ImageBuffer(0)
{
    MILNumberOfDevices = 0;
    MilCaptureBuffers = 3;
    MilApplication = M_NULL;
    MilSystem[0] = MilSystem[1] = M_NULL;
    MilDisplay[0] = MilDisplay[1] = M_NULL;
    MilDigitizer[0] = MilDigitizer[1] = M_NULL;
    MilDisplayImage[0] = MilDisplayImage[1] = M_NULL;
    MilOverlayImage[0] = MilOverlayImage[1] = M_NULL;
    MilCaptureEnabled[0] = MilCaptureEnabled[0] = true;
    MilOverlayEnabled[0] = MilOverlayEnabled[0] = false;
    MilDeviceID[0] = M_DEV0;
    MilDeviceID[1] = M_DEV1;
    MilDeviceInitialized[0] = MilDeviceInitialized[1] = false;
    MilOverlayBuffer[0] = MilOverlayBuffer[1] = 0;
    MilCaptureParams[0].MilFrames = MilCaptureParams[1].MilFrames = 0;

    MILInitializeApplication();
}

CMILDevice::~CMILDevice()
{
    Release();
    MILReleaseApplication();
}

svlVideoCaptureSource::PlatformType CMILDevice::GetPlatformType()
{
    return svlVideoCaptureSource::MatroxImaging;
}

int CMILDevice::SetStreamCount(unsigned int numofstreams)
{
    if (numofstreams < 1) return SVL_FAIL;

    Release();

    NumOfStreams = numofstreams;

    DeviceID = new int[NumOfStreams];
    ImageBuffer = new svlImageBuffer*[NumOfStreams];

    for (unsigned int i = 0; i < NumOfStreams; i ++) {
        DeviceID[i] = -1;
        ImageBuffer[i] = 0;
    }

    return SVL_OK;
}

int CMILDevice::GetDeviceList(svlVideoCaptureSource::DeviceInfo **deviceinfo)
{
    if (deviceinfo == 0 || Initialized) return SVL_FAIL;

    int i, w, h, b;
    bool cap, ovrl;
    int devid[2];
    bool capture[2], overlay[2];
    string description;

    MILNumberOfDevices = 0;

    // Only M_DEV0 and M_DEV1 are supported
    for (i = 0; i < 2; i ++) {
        cap = MILInitializeDevice(i, true, false, w, h, b);
        ovrl = MILInitializeDevice(i, false, true, w, h, b);
        MILReleaseDevice(i);
        if (cap || ovrl) {
            devid[MILNumberOfDevices] = i;
            capture[MILNumberOfDevices] = cap;
            overlay[MILNumberOfDevices] = ovrl;
            Width[MILNumberOfDevices] = w;
            Height[MILNumberOfDevices] = h;
        }
        MILNumberOfDevices ++;
    }

    // Allocate memory for device info array
    // CALLER HAS TO FREE UP THIS ARRAY!!!
    if (MILNumberOfDevices > 0) {

        deviceinfo[0] = new svlVideoCaptureSource::DeviceInfo[MILNumberOfDevices];

        for (i = 0; i < MILNumberOfDevices; i ++) {
            // platform
            deviceinfo[0][i].platform = svlVideoCaptureSource::MatroxImaging;

            // id
            deviceinfo[0][i].id = devid[i];

            // name
            if (devid[i]) description = "M_DEV1: ";
            else description = "M_DEV0: ";
            if (capture[i] && overlay[i]) description += "Capture+Overlay";
            else if (capture[i]) description += "Capture only";
            else if (overlay[i]) description += "Overlay only";
            sprintf(deviceinfo[0][i].name, "Matrox Imaging Device (%s)", description.c_str());

            // inputs
            deviceinfo[0][i].inputcount = 0;
            deviceinfo[0][i].activeinput = -1;

            // id
            deviceinfo[0][i].testok = true;
        }
    }
    else {
        deviceinfo[0] = 0;
    }

    return MILNumberOfDevices;
}

int CMILDevice::Open()
{
    if (NumOfStreams <= 0) return SVL_FAIL;
    if (Initialized) return SVL_OK;

    Close();

    int w, h, b;

    for (unsigned int i = 0; i < NumOfStreams; i ++) {

        // Opening device
        if (!MILInitializeDevice(DeviceID[i], MilCaptureEnabled[DeviceID[i]], MilOverlayEnabled[DeviceID[i]], w, h, b)) goto labError;
        if (b != 3) goto labError;

        // Allocate capture buffers
        ImageBuffer[i] = new svlImageBuffer(w, h);
        // Set the pointer in the capture structure that will be accessed in the callback
        MilCaptureParams[DeviceID[i]].ImageBuffer = ImageBuffer[i];
    }

    Initialized = true;
    return SVL_OK;

labError:
    Close();
    return SVL_FAIL;
}

void CMILDevice::Close()
{
    if (NumOfStreams <= 0) return;

    Stop();

    Initialized = false;

    MILReleaseDevice(0);
    MILReleaseDevice(1);
}

int CMILDevice::Start()
{
    if (!Initialized) return SVL_FAIL;
    Running = true;
    return SVL_OK;
}

svlImageRGB* CMILDevice::GetLatestFrame(bool waitfornew, unsigned int videoch)
{
    if (videoch >= NumOfStreams || !Initialized) return 0;
    return ImageBuffer[videoch]->Pull(waitfornew);
}

int CMILDevice::Stop()
{
    if (!Running) return SVL_FAIL;
    Running = false;
    return SVL_OK;
}

bool CMILDevice::IsRunning()
{
    return Running;
}

int CMILDevice::SetDevice(int devid, int inid, unsigned int videoch)
{
    if (videoch >= NumOfStreams) return SVL_FAIL;
    DeviceID[videoch] = devid;
    // Input ID is ignored
    return SVL_OK;
}

int CMILDevice::GetWidth(unsigned int videoch)
{
    if (videoch >= NumOfStreams) return SVL_FAIL;
    return ImageBuffer[videoch]->GetWidth();
}

int CMILDevice::GetHeight(unsigned int videoch)
{
    if (videoch >= NumOfStreams) return SVL_FAIL;
    return ImageBuffer[videoch]->GetHeight();
}

int CMILDevice::GetFormatList(unsigned int deviceid, svlVideoCaptureSource::ImageFormat **formatlist)
{
    if (static_cast<int>(deviceid) >= MILNumberOfDevices || formatlist == 0) return SVL_FAIL;

    formatlist[0] = new svlVideoCaptureSource::ImageFormat[1];
    formatlist[0][0].width = Width[deviceid];
    formatlist[0][0].height = Height[deviceid];
    formatlist[0][0].colorspace = svlVideoCaptureSource::PixelRGB8;
    formatlist[0][0].rgb_order = true;
    formatlist[0][0].yuyv_order = false;
    formatlist[0][0].framerate = -1.0;

    return 1;
}

int CMILDevice::GetFormat(svlVideoCaptureSource::ImageFormat& format, unsigned int videoch)
{
    if (DeviceID[videoch] >= MILNumberOfDevices) return SVL_FAIL;

    format.width = Width[DeviceID[videoch]];
    format.height = Height[DeviceID[videoch]];
    format.colorspace = svlVideoCaptureSource::PixelRGB8;
    format.rgb_order = true;
    format.yuyv_order = false;
    format.framerate = -1.0;

    return SVL_OK;
}

void CMILDevice::Release()
{
	Close();

    if (DeviceID) delete [] DeviceID;
    if (ImageBuffer) delete [] ImageBuffer;

    NumOfStreams = 0;
    Initialized = false;
    Running = false;
	DeviceID = 0;
    ImageBuffer = 0;
}

bool CMILDevice::MILInitializeApplication()
{
    if (MilApplication == M_NULL) MappAlloc(M_DEFAULT, &MilApplication);
    if (MilApplication == M_NULL) return false;
    return true;
}

bool CMILDevice::MILInitializeDevice(int device, bool capture, bool overlay, int& width, int& height, int& bands)
{
    if (device < 0 || device > 1) return false;

    if (MilDeviceInitialized[device]) {
        if (MilCaptureEnabled[device] == capture &&
            MilOverlayEnabled[device] == overlay) return true;
        // Reinitialize if needed
        MILReleaseDevice(device);
    }

    MsysAlloc(M_SYSTEM_VIO, MilDeviceID[device], M_SETUP, &(MilSystem[device]));
    if (MilSystem[device] == M_NULL) goto labError;

    if (MsysInquire(MilSystem[device], M_DIGITIZER_NUM, M_NULL) == 0) goto labError;

    MdigAlloc(MilSystem[device], M_DEV0, MIL_TEXT("M_DEFAULT"), M_DEFAULT, &(MilDigitizer[device]));
    if (MilDigitizer[device] == M_NULL) goto labError;

    MilWidth[device] = MdigInquire(MilDigitizer[device], M_SIZE_X, M_NULL);
    MilHeight[device] = MdigInquire(MilDigitizer[device], M_SIZE_Y, M_NULL);
    MilBands[device] = MdigInquire(MilDigitizer[device], M_SIZE_BAND, M_NULL); 
    MilBandBits[device] = MdigInquire(MilDigitizer[device], M_SIZE_BIT, M_NULL);

    MbufAllocColor(MilSystem[device],
                   MilBands[device],
                   MilWidth[device],
                   MilHeight[device],
                   8+M_UNSIGNED, M_IMAGE+M_DISP+M_PROC+M_GRAB,
                   &(MilDisplayImage[device]));
    if (MilDisplayImage[device] == M_NULL) goto labError;

    MbufClear(MilDisplayImage[device], 0);

    MdispAlloc(MilSystem[device], M_DEFAULT, MIL_TEXT("M_DEFAULT"), M_AUXILIARY, &(MilDisplay[device]));
    if (MilDisplay[device] == M_NULL) goto labError;

    MdispControl(MilDisplay[device], M_SELECT_VIDEO_SOURCE, MilDigitizer[device]);

    MdispSelect(MilDisplay[device], MilDisplayImage[device]);

    MilOverlayEnabled[device] = overlay;
    if (overlay) {
        MdispControl(MilDisplay[device], M_OVERLAY, M_ENABLE);
        MdispControl(MilDisplay[device], M_OVERLAY_CLEAR, M_DEFAULT);
        MdispControl(MilDisplay[device], M_OVERLAY_SHOW, M_ENABLE);
        MdispControl(MilDisplay[device], M_NO_TEARING, M_ENABLE);

        MdispInquire(MilDisplay[device], M_OVERLAY_ID, &MilOverlayImage[device]);
        if (MilOverlayImage[device] == M_NULL) goto labError;

        MdispControl(MilDisplay[device], M_TRANSPARENT_COLOR, M_BGR888(0,0,0));

        MilOverlayBuffer[device] = new unsigned char[MilWidth[device] * MilHeight[device] * MilBands[device]];
    }

    MilCaptureEnabled[device] = capture;
    if (capture) {
        unsigned int i;

        MilCaptureParams[device].MilFrames = new MIL_ID[MilCaptureBuffers];
    	for (i = 0; i < MilCaptureBuffers; i ++) {
            MilCaptureParams[device].MilFrames[i] = M_NULL;
        }

    	for (i = 0; i < MilCaptureBuffers; i ++) {
    		MbufAllocColor(MilSystem[device],
    					   MilBands[device],
    					   MilWidth[device],
    					   MilHeight[device],
    					   8+M_UNSIGNED, M_IMAGE+M_DISP+M_PROC+M_GRAB,
    					   &(MilCaptureParams[device].MilFrames[i]));
            if (MilCaptureParams[device].MilFrames[i] == M_NULL) goto labError;

    	    MbufClear(MilCaptureParams[device].MilFrames[i], 0);
    	}

        // Triple buffer will be initialized by the caller upon return
        MilCaptureParams[device].ImageBuffer = 0;

    	MdigProcess(MilDigitizer[device],
    				MilCaptureParams[device].MilFrames,
    				MilCaptureBuffers,
    				M_START,
    				M_ASYNCHRONOUS,
    				MILProcessingCallback,
    				&(MilCaptureParams[device]));
    }

    width = MilWidth[device];
    height = MilHeight[device];
    bands = MilBands[device];

    MilDeviceInitialized[device] = true;
    return true;

labError:
    MILReleaseDevice(device);
    return false;
}

bool CMILDevice::MILUploadOverlay(int device)
{
    if (device < 0 || device > 1) return false;
    if (MilDeviceInitialized[device] && MilOverlayEnabled[device]) {
        MbufPutColor(MilOverlayImage[device],
                     M_PACKED+M_RGB24, M_ALL_BANDS,
                     MilOverlayBuffer[device]);
        return true;
    }
    return false;
}

void CMILDevice::MILReleaseDevice(int device)
{
    if (device < 0 || device > 1) return;

    if (MilDeviceInitialized[device] && MilCaptureEnabled[device]) {
       	MdigProcess(MilDigitizer[device],
    				MilCaptureParams[device].MilFrames,
    				MilCaptureBuffers,
    				M_STOP+M_WAIT,
    				M_DEFAULT,
    				MILProcessingCallback,
    				&(MilCaptureParams[device]));
    }

	if (MilOverlayImage[device] != M_NULL) MbufClear(MilOverlayImage[device], 0);

    if (MilDisplay[device] != M_NULL) MdispControl(MilDisplay[device], M_AUXILIARY_KEEP_DISPLAY_ALIVE, M_ENABLE);

    if (MilCaptureParams[device].MilFrames) {
    	for (unsigned int i = 0; i < MilCaptureBuffers; i ++) {
    	    if (MilCaptureParams[device].MilFrames[i] != M_NULL) MbufFree(MilCaptureParams[device].MilFrames[i]);
    	}
        delete [] MilCaptureParams[device].MilFrames;
        MilCaptureParams[device].MilFrames = 0;
    }

    if (MilDisplayImage[device] != M_NULL) MbufFree(MilDisplayImage[device]);
    if (MilDisplay[device] != M_NULL) MdispFree(MilDisplay[device]);
    if (MilDigitizer[device] != M_NULL) MdigFree(MilDigitizer[device]);
    if (MilSystem[device] != M_NULL) MsysFree(MilSystem[device]);

	if (MilOverlayBuffer[device]) delete [] MilOverlayBuffer[device];
    MilOverlayBuffer[device] = 0;

    MilSystem[device] = M_NULL;
    MilDisplay[device] = M_NULL;
    MilDigitizer[device] = M_NULL;
    MilDisplayImage[device] = M_NULL;
    MilOverlayImage[device] = M_NULL;
    MilDeviceInitialized[device] = false;
}

void CMILDevice::MILReleaseApplication()
{
    if (MilApplication != M_NULL) MappFree(MilApplication);
    MilApplication = M_NULL;
}


/*************************************/
/*** MILProcessingCallback ***********/
/*************************************/

MIL_INT MFTYPE MILProcessingCallback(MIL_INT HookType, MIL_ID HookId, void MPTYPE *HookDataPtr)
{
	MIL_INT milbufferindex;
	CMILDevice::MILCaptureParameters *milcaptureparams = (CMILDevice::MILCaptureParameters*)HookDataPtr;

	// Get modified buffer index
	MdigGetHookInfo(HookId, M_MODIFIED_BUFFER+M_BUFFER_INDEX, &milbufferindex);

    if (milcaptureparams->ImageBuffer) {
    	MbufGetColor(milcaptureparams->MilFrames[milbufferindex],
    				 M_PACKED+M_RGB24,
    				 M_ALL_BANDS,
                     milcaptureparams->ImageBuffer->GetPushBuffer());
        milcaptureparams->ImageBuffer->Push();
    }

	return 0;
}
