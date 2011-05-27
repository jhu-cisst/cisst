/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cameraCalibration.cpp 2426 2011-05-21 00:53:58Z wliu25 $

  Author(s):  Wen P. Liu
  Created on: 2011

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "svlVidCapSrcBMD.h"
#include <cisstOSAbstraction/osaThread.h>
#include <cisstStereoVision/svlBufferImage.h>


/*******************************/
/*** svlVidCapSrcBMD class *****/
/*******************************/

CMN_IMPLEMENT_SERVICES(svlVidCapSrcBMD)

svlVidCapSrcBMD::svlVidCapSrcBMD() :
    svlVidCapSrcBase(),
    cmnGenericObject(),
    NumOfStreams(0),
    Running(false),
    CaptureProc(0),
    CaptureThread(0),
    deckLinkObj(0),
    OutputBuffer(0)
{
    BMDNumberOfDevices = 0;
	BMDVideoInputFlags			inputFlags = 0;
	BMDDisplayMode				displayMode = bmdModeNTSC; //bmdModeHD1080p30, bmdModeNTSC
	SetWidthHeightByBMDDisplayMode();
	BMDPixelFormat				pixelFormat = bmdFormat8BitARGB;//bmdFormat8BitYUV
}

svlVidCapSrcBMD::~svlVidCapSrcBMD()
{
    Close();

    if (ImageBuffer) delete [] ImageBuffer;
}

void svlVidCapSrcBMD::SetWidthHeightByBMDDisplayMode()
{
	switch(displayMode)
	{
		case bmdModeNTSC: bmdModeNTSC2398: bmdModePAL: bmdModeNTSCp: bmdModePALp:
			width = 720;
			height = 480; //sometimes 486?
			break;
		case bmdModeHD1080p2398: bmdModeHD1080p24: bmdModeHD1080p25: bmdModeHD1080p2997: bmdModeHD1080p30:
			 bmdModeHD1080i50: bmdModeHD1080i5994:bmdModeHD1080i6000:
			 bmdModeHD1080p50:bmdModeHD1080p5994:bmdModeHD1080p6000:
			width = 1920;
			height = 1080; 
			break;
		case bmdModeHD720p50: bmdModeHD720p5994: bmdModeHD720p60:
			width = 1920;
			height = 720; 
			break;
		case bmdMode2k2398: bmdMode2k24: bmdMode2k25:
			width = 1920; //?
			height = 1080; //?
			break;		
		default:
			printf("Unrecognized display mode: %d\n", displayMode);	
	}
}

IDeckLinkIterator* svlVidCapSrcBMD::GetIDeckLinkIterator()
{
	IDeckLinkIterator*			deckLinkIterator;
	HRESULT						result;
	#ifdef _WIN32
		// Create an IDeckLinkIterator object to enumerate all DeckLink cards in the system
		result = CoCreateInstance(CLSID_CDeckLinkIterator, NULL, CLSCTX_ALL, IID_IDeckLinkIterator, (void**)&deckLinkIterator);
		if (FAILED(result))
		{
			fprintf(stderr, "A DeckLink iterator could not be created.  The DeckLink drivers may not be installed.\n");
			return 1;
		}
	#else
		deckLinkIterator = CreateDeckLinkIteratorInstance();	
	#endif // _WIN32

	return deckLinkIterator;
}

svlFilterSourceVideoCapture::PlatformType svlVidCapSrcBMD::GetPlatformType()
{
    return svlFilterSourceVideoCapture::BlackmagicDeckLink;
}

int svlVidCapSrcBMD::SetStreamCount(unsigned int numofstreams)
{
    if (numofstreams < 1) return SVL_FAIL;

    Release();

    NumOfStreams = numofstreams;

    DeviceID = new int[NumOfStreams];
    ImageBuffer = new svlBufferImage*[NumOfStreams];

    for (unsigned int i = 0; i < NumOfStreams; i ++) {
        DeviceID[i] = -1;
        ImageBuffer[i] = 0;
    }

    return SVL_OK;
}

int svlVidCapSrcBMD::GetDeviceList(svlFilterSourceVideoCapture::DeviceInfo **deviceinfo)
{
    if (deviceinfo == 0 || Initialized) return SVL_FAIL;
	
	IDeckLinkIterator*			deckLinkIterator = GetIDeckLinkIterator();
	IDeckLinkAPIInformation*	deckLinkAPIInformation;
	IDeckLink*					deckLink;
	HRESULT						result;

	// Enumerate all cards in this system
	while (deckLinkIterator->Next(&deckLink) == S_OK)
	{
		numDevices++;
		deckLink->Release();
	}

	// Create an IDeckLinkIterator object to enumerate all DeckLink cards in the system
	deckLinkIterator = GetIDeckLinkIterator();

    // Allocate memory for device info array
    // CALLER HAS TO FREE UP THIS ARRAY!!!
    if (numDevices > 0) {
		int i = 0;
        deviceinfo[0] = new svlFilterSourceVideoCapture::DeviceInfo[numDevices];
		while (deckLinkIterator->Next(&deckLink) == S_OK)
		{
			BSTR		deviceNameBSTR = NULL;
			result = deckLink->GetModelName(&deviceNameBSTR);
			if (result == S_OK)
			{
				_bstr_t		deviceName(deviceNameBSTR, false);
				//printf("=============== %s ===============\n\n", (char*)deviceName);
			}

            // platform
            deviceinfo[0][i].platform = svlFilterSourceVideoCapture::BlackmagicDeckLink;

            // id
            deviceinfo[0][i].id = i;

            // name
            description = (char*)deviceName;

            memset(deviceinfo[0][i].name, 0, SVL_VCS_STRING_LENGTH);
            memcpy(deviceinfo[0][i].name,
                   description.c_str(),
                   std::min(SVL_VCS_STRING_LENGTH - 1, static_cast<int>(description.length())));

            // inputs
            deviceinfo[0][i].inputcount = 0;
            deviceinfo[0][i].activeinput = -1;

            // id
            deviceinfo[0][i].testok = true;
			i++;

			// Release the IDeckLink instance when we've finished with it to prevent leaks
			deckLink->Release();
        }
	}
    else {
        deviceinfo[0] = 0;
    }

    return numDevices;
}

int svlVidCapSrcBMD::Open()
{
    if (NumOfStreams <= 0) return SVL_FAIL;
    if (Initialized) return SVL_OK;

    Close();

	IDeckLinkIterator			*deckLinkIterator = GetIDeckLinkIterator();

	int							displayModeCount = 0;
	int							exitStatus = 1;
	int							ch;
	bool 						foundDisplayMode = false;
	HRESULT						result;

	pthread_mutex_init(&sleepMutex, NULL);
	pthread_cond_init(&sleepCond, NULL);
	
	if (!deckLinkIterator)
	{
		fprintf(stderr, "This application requires the DeckLink drivers installed.\n");
		goto labError;
	}

    for (unsigned int i = 0; i < NumOfStreams; i ++) {		
		/* Connect to a DeckLink instance */
		result = deckLinkIterator->Next(&deckLink);
		if (result != S_OK)
		{
			fprintf(stderr, "No DeckLink PCI cards found.\n");
			goto labError;
		}
    
		if (deckLink->QueryInterface(IID_IDeckLinkInput, (void**)&deckLinkInput) != S_OK)
			goto labError;

		// Opening device
		result = deckLinkInput->EnableVideoInput(selectedDisplayMode, pixelFormat, inputFlags);
	    if(result != S_OK) goto labError;

        // Allocate capture buffers
        ImageBuffer[i] = new svlBufferImage(width, height);
    }

    Initialized = true;
    return SVL_OK;

labError:
    Close();
    return SVL_FAIL;
}

void svlVidCapSrcBMD::Close()
{
    if (NumOfStreams <= 0) return;

    Stop();

	IDeckLinkIterator			*deckLinkIterator = GetIDeckLinkIterator();
	if (!deckLinkIterator)
	{
		fprintf(stderr, "This application requires the DeckLink drivers installed.\n");
	}

    // Release capture buffers
    for (unsigned int i = 0; i < NumOfStreams; i ++) {
		result = deckLinkIterator->Next(&deckLink);
		if (result != S_OK)
		{
			fprintf(stderr, "No DeckLink PCI cards found.\n");
			goto labError;
		}
    
		if (deckLink->QueryInterface(IID_IDeckLinkInput, (void**)&deckLinkInput) != S_OK)
			goto labError;

		// Closing device
		result = deckLinkInput->DisableVideoInput();

        if (ImageBuffer[i]) {
            delete ImageBuffer[i];
            ImageBuffer[i] = 0;
        }
    }
}

int svlVidCapSrcBMD::Start()
{
    if (!Initialized) return SVL_FAIL;
    Running = true;
    return SVL_OK;
}

svlImageRGB* svlVidCapSrcBMD::GetLatestFrame(bool waitfornew, unsigned int videoch)
{
    if (videoch >= NumOfStreams || DeviceID[videoch] < 0) return 0;
    return ImageBuffer[videoch]->Pull(waitfornew);
}

int svlVidCapSrcBMD::CaptureFrame()
{
	//TODO

    return SVL_FAIL;
}

int svlVidCapSrcBMD::Stop()
{
    if (!Running) return SVL_FAIL;
    Running = false;
    return SVL_OK;
}

bool svlVidCapSrcBMD::IsRunning()
{
    return Running;
}

int svlVidCapSrcBMD::SetDevice(int devid, int inid, unsigned int videoch)
{
    if (videoch >= NumOfStreams) return SVL_FAIL;
    DeviceID[videoch] = devid;
    // Input ID is ignored
    return SVL_OK;
}

int svlVidCapSrcBMD::GetWidth(unsigned int videoch)
{
    if (videoch >= NumOfStreams) return SVL_FAIL;
    return ImageBuffer[videoch]->GetWidth();
}

int svlVidCapSrcBMD::GetHeight(unsigned int videoch)
{
    if (videoch >= NumOfStreams) return SVL_FAIL;
    return ImageBuffer[videoch]->GetHeight();
}

int svlVidCapSrcBMD::GetFormatList(unsigned int deviceid, svlFilterSourceVideoCapture::ImageFormat **formatlist)
{
    if (formatlist == 0) return SVL_FAIL;

    formatlist[0] = new svlFilterSourceVideoCapture::ImageFormat[1];
    formatlist[0][0].width = width;
    formatlist[0][0].height = height;
    formatlist[0][0].colorspace = svlFilterSourceVideoCapture::PixelRGB8;
    formatlist[0][0].rgb_order = true;
    formatlist[0][0].yuyv_order = false;
    formatlist[0][0].framerate = 25.0;
    formatlist[0][0].custom_mode = -1;

    return 1;
}

int svlVidCapSrcBMD::GetFormat(svlFilterSourceVideoCapture::ImageFormat& format, unsigned int videoch)
{
    if (videoch >= NumOfStreams) return SVL_FAIL;

    format.width = width;
    format.height = height;
    format.colorspace = svlFilterSourceVideoCapture::PixelRGB8;
    format.rgb_order = true;
    format.yuyv_order = false;
    format.framerate = 25.0;
    format.custom_mode = -1;

    return SVL_OK;
}

void svlVidCapSrcBMD::ConvertRGB32toRGB24(unsigned char* source, unsigned char* dest, const int pixcount)
{
    unsigned char r, g, b;
    for (int i = 0; i < pixcount; i ++) {
        r = *source;
        source ++;
        g = *source;
        source ++;
        b = *source;
        source += 2;

        *dest = b;
        dest ++;
        *dest = g;
        dest ++;
        *dest = r;
        dest ++;
    }
}


/**************************************/
/*** svlVidCapSrcBMDThread class ******/
/**************************************/

void* svlVidCapSrcBMDThread::Proc(svlVidCapSrcBMD* baseref)
{
    // signal success to main thread
    Error = false;
    InitSuccess = true;
    InitEvent.Raise();

    while (baseref->Running) {
        if (baseref->CaptureFrame() != SVL_OK) {
            Error = true;
            break;
        }
    }

	return this;
}

