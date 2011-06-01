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
#include <cisstStereoVision/svlBufferImage.h>
#include <cisstStereoVision/svlConverters.h>


/*******************************/
/*** svlVidCapSrcBMD class *****/
/*******************************/

CMN_IMPLEMENT_SERVICES(svlVidCapSrcBMD)

svlVidCapSrcBMD::svlVidCapSrcBMD() :
    svlVidCapSrcBase(),
    cmnGenericObject(),
    NumOfStreams(0),
    Running(false),
	Initialized(false),
    ImageBuffer(0)
{
    BMDNumberOfDevices = 0;
	inputFlags = 0;
	displayMode = bmdModeHD1080p30;
	SetWidthHeightByBMDDisplayMode();
        pixelFormat = bmdFormat8BitYUV;//bmdFormat8BitARGB;
}

svlVidCapSrcBMD::~svlVidCapSrcBMD()
{
    Close();

    if (ImageBuffer) delete [] ImageBuffer;
}

void svlVidCapSrcBMD::SetWidthHeightByBMDDisplayMode()
{
	width = 1920;
	height = 1080; 
	switch(displayMode)
	{
		case bmdModeNTSC: bmdModeNTSC2398: bmdModePAL: bmdModeNTSCp: bmdModePALp:
			width = 720;
			height = 480; //sometimes 486?
			break;
                case bmdModeHD1080p2398: bmdModeHD1080p24: bmdModeHD1080p25: bmdModeHD1080p2997:
			 bmdModeHD1080i50: bmdModeHD1080i5994:bmdModeHD1080i6000:
			 bmdModeHD1080p50:bmdModeHD1080p5994:bmdModeHD1080p6000:
                case bmdModeHD1080p30:
			width = 1920;
			height = 1080; 
			printf("Width:%d, Height:%d\n", width, height);	
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
            break;
	}

	printf("Width:%d, Height:%d\n", width, height);	
}

IDeckLinkIterator* CreateDeckLinkIteratorInstance(void);

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
			return NULL;
		}
	#else
		deckLinkIterator = CreateDeckLinkIteratorInstance();	
	#endif // _WIN32

	return deckLinkIterator;
}

svlFilterSourceVideoCapture::PlatformType svlVidCapSrcBMD::GetPlatformType()
{
    return svlFilterSourceVideoCapture::BlackMagicDeckLink;
}

int svlVidCapSrcBMD::SetStreamCount(unsigned int numofstreams)
{
    if (numofstreams < 1) return SVL_FAIL;

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
		BMDNumberOfDevices++;
		deckLink->Release();
	}

	if(deckLinkIterator != NULL)
		deckLinkIterator->Release();

	// Create an IDeckLinkIterator object to enumerate all DeckLink cards in the system
	deckLinkIterator = GetIDeckLinkIterator();

    // Allocate memory for device info array
    // CALLER HAS TO FREE UP THIS ARRAY!!!
    if (BMDNumberOfDevices > 0) {
		int i = 0;
        deviceinfo[0] = new svlFilterSourceVideoCapture::DeviceInfo[BMDNumberOfDevices];
		while (deckLinkIterator->Next(&deckLink) == S_OK)
		{
                        char *		deviceNameString = NULL;
                        result = deckLink->GetModelName((const char **)(&deviceNameString));
                        if (result == S_OK)
                        {
				//_bstr_t		deviceName(deviceNameString, false);
				//printf("=============== %s ===============\n\n", (char*)deviceName);
                        }

            // platform
            deviceinfo[0][i].platform = svlFilterSourceVideoCapture::BlackMagicDeckLink;

            // id
            deviceinfo[0][i].id = i;

            // name
            std::string description = deviceNameString;

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

	if(deckLinkIterator != NULL)
		deckLinkIterator->Release();
	if(deckLink != NULL)
		deckLink->Release();
    return BMDNumberOfDevices;
}

int svlVidCapSrcBMD::Open()
{
    if (NumOfStreams <= 0) return SVL_FAIL;
    if (Initialized) return SVL_OK;

    Close();

	IDeckLinkIterator			*deckLinkIterator = GetIDeckLinkIterator();
	IDeckLink*					deckLink;
	IDeckLinkInput*				deckLinkInput;
	DeckLinkCaptureDelegate 	*delegate;
	int							displayModeCount = 0;
	int							exitStatus = 1;
	int							ch;
	bool 						foundDisplayMode = false;
	HRESULT						result;
	
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
		
		// Set callback delgate
        // Allocate capture buffers
        ImageBuffer[i] = new svlBufferImage(width, height);
		delegate = new DeckLinkCaptureDelegate(ImageBuffer[i]);
		deckLinkInput->SetCallback(delegate);
		
		// Opening device
		result = deckLinkInput->EnableVideoInput(displayMode, pixelFormat, inputFlags);
	    if(result != S_OK) goto labError;

		// Starting Stream
		result = deckLinkInput->StartStreams();
		if(result != S_OK)
		{
			goto labError;
		}
    }

    Initialized = true;
	if(deckLinkIterator != NULL)
		deckLinkIterator->Release();
	if(deckLinkInput != NULL)
		deckLinkInput->Release();
	if(deckLink != NULL)
		deckLink->Release();
    return SVL_OK;

labError:
    Close();
	if(deckLinkIterator != NULL)
		deckLinkIterator->Release();
	if(deckLinkInput != NULL)
		deckLinkInput->Release();
	if(deckLink != NULL)
		deckLink->Release();
    return SVL_FAIL;
}

void svlVidCapSrcBMD::Close()
{
    if (NumOfStreams <= 0) return;

    Stop();
	IDeckLink*					deckLink;
	IDeckLinkInput*				deckLinkInput;
	IDeckLinkIterator			*deckLinkIterator = GetIDeckLinkIterator();
	HRESULT result;
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
		deckLinkInput->StopStreams();
		deckLinkInput->DisableVideoInput();

        if (ImageBuffer[i]) {
            delete ImageBuffer[i];
            ImageBuffer[i] = 0;
        }
    }

labError:
	if(deckLinkIterator != NULL)
		deckLinkIterator->Release();
	if(deckLinkInput != NULL)
		deckLinkInput->Release();
	if(deckLink != NULL)
		deckLink->Release();
    return;
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
    format.framerate = 30.0;
    format.custom_mode = -1;

    return SVL_OK;
}

/**************************************/
/*** DeckLinkCaptureDelegate class ******/
/**************************************/

DeckLinkCaptureDelegate::DeckLinkCaptureDelegate(svlBufferImage* buffer) : m_refCount(0)
{
	//pthread_mutex_init(&m_mutex, NULL);
	g_timecodeFormat = (BMDTimecodeFormat)0;
        m_buffer = buffer;
}

DeckLinkCaptureDelegate::~DeckLinkCaptureDelegate()
{
	//pthread_mutex_destroy(&m_mutex);
	return;
}

ULONG DeckLinkCaptureDelegate::AddRef(void)
{
	//pthread_mutex_lock(&m_mutex);
		m_refCount++;
	//pthread_mutex_unlock(&m_mutex);

	return (ULONG)m_refCount;
}

ULONG DeckLinkCaptureDelegate::Release(void)
{
	//pthread_mutex_lock(&m_mutex);
        m_refCount--;
	//pthread_mutex_unlock(&m_mutex);

	if (m_refCount == 0)
	{
		delete this;
		return 0;
	}

	return (ULONG)m_refCount;
}

HRESULT DeckLinkCaptureDelegate::VideoInputFrameArrived(IDeckLinkVideoInputFrame* videoFrame, IDeckLinkAudioInputPacket* audioFrame)
{
	IDeckLinkVideoFrame*	                rightEyeFrame = NULL;
	IDeckLinkVideoFrame3DExtensions*        threeDExtensions = NULL;
	
	// Handle Video Frame
	if(videoFrame)
	{	
		// If 3D mode is enabled we retreive the 3D extensions interface which gives.
		// us access to the right eye frame by calling GetFrameForRightEye() .
		if ( (videoFrame->QueryInterface(IID_IDeckLinkVideoFrame3DExtensions, (void **) &threeDExtensions) != S_OK) ||
			(threeDExtensions->GetFrameForRightEye(&rightEyeFrame) != S_OK))
		{
			rightEyeFrame = NULL;
		}
		
		if (threeDExtensions)
			threeDExtensions->Release();

		if (videoFrame->GetFlags() & bmdFrameHasNoInputSource)
		{
			fprintf(stderr, "Frame received (#%lu) - No input signal detected\n", frameCount);
		}
                else
                //process standard SDI input
		{
			processVideoFrame(videoFrame);
			
		}
		
		if (rightEyeFrame)
			rightEyeFrame->Release();

		frameCount++;

	}

	// Audio Frame NOT HANDLED
	//if (audioFrame)

    
	return S_OK;
}

void DeckLinkCaptureDelegate::processVideoFrame(IDeckLinkVideoInputFrame* videoFrame)
{
	void* source;;
	unsigned char* dest = m_buffer->GetPushBuffer();

	videoFrame->GetBytes(&source);


        svlConverter::YUV422toRGB24(static_cast<unsigned char *>(source), dest,videoFrame->GetHeight()*videoFrame->GetWidth(),true,true,true);

}



