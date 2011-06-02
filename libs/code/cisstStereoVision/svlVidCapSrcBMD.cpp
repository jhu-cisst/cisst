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
    debug = false;
}

svlVidCapSrcBMD::~svlVidCapSrcBMD()
{
    Close();

    if (ImageBuffer) delete [] ImageBuffer;
}

void svlVidCapSrcBMD::SetWidthHeightByBMDDisplayMode()
{
    // default settings
    width = 1920;
    height = 1080; 
    frameRate = 30;

    // width & height
    if(displayMode == bmdModeNTSC||	displayMode == bmdModeNTSC2398|| displayMode == bmdModePAL|| displayMode == bmdModeNTSCp || displayMode == bmdModePALp)
    {
        width = 720;
        height = 480;
    }else if(displayMode == bmdModeHD1080p2398 || displayMode ==bmdModeHD1080p24 || displayMode == bmdModeHD1080p25	|| displayMode == bmdModeHD1080p2997	
	        || displayMode == bmdModeHD1080p30 || displayMode == bmdModeHD1080i50 || displayMode == bmdModeHD1080i5994 || displayMode == bmdModeHD1080i6000
	        || displayMode == bmdModeHD1080p50 || displayMode == bmdModeHD1080p5994	|| displayMode == bmdModeHD1080p6000)
    {
        width = 1920;
        height = 1080;
    }else if(displayMode == bmdModeHD720p50	|| displayMode == bmdModeHD720p5994	|| displayMode == bmdModeHD720p60)
    {
        width = 1920;
        height = 720;
    }else if(displayMode == bmdMode2k2398 || displayMode == bmdMode2k24	|| displayMode == bmdMode2k25)
    {
        width = 2048; 
        height = 1080;    
    }

    // frame rate
    switch(displayMode)
    {

    case bmdModeNTSC2398:
    case bmdModeHD1080p2398:
    case bmdMode2k2398:	
        frameRate = 23.98;
        break;
    case bmdModeHD1080p24:	
    case bmdMode2k24:	
        frameRate = 24.0;
        break;
    case bmdModeHD1080p25:	
    case bmdMode2k25:	
        frameRate = 25.0;
        break;
    case bmdModeHD1080p2997:	
    case bmdModeNTSC:
    case bmdModePAL:
    case bmdModeNTSCp:
    case bmdModePALp:	
    case bmdModeHD1080p30:
        frameRate = 30.0;
        break;
    case bmdModeHD1080i50:
    case bmdModeHD1080p50:	
    case bmdModeHD720p50:	
        frameRate = 50.0;
        break;
    case bmdModeHD1080i5994:
    case bmdModeHD1080p5994:	
    case bmdModeHD720p5994:
        frameRate = 59.94;
        break;
    case bmdModeHD1080i6000:
    case bmdModeHD1080p6000:	
    case bmdModeHD720p60:
        frameRate = 60;
        break;
    default:
        printf("Unrecognized display mode: %d\n", displayMode);	
        break;
    }

    printf("Width:%d, Height:%d at %4.2fHz\n", width, height, frameRate);
}

IDeckLinkIterator* CreateDeckLinkIteratorInstance(void);

IDeckLinkIterator* svlVidCapSrcBMD::GetIDeckLinkIterator()
{
    IDeckLinkIterator*			deckLinkIterator;
#if (CISST_OS == CISST_WINDOWS)
    HRESULT						result;
    // Create an IDeckLinkIterator object to enumerate all DeckLink cards in the system
    result = CoCreateInstance(CLSID_CDeckLinkIterator, NULL, CLSCTX_ALL, IID_IDeckLinkIterator, (void**)&deckLinkIterator);
    if (FAILED(result))
    {
        fprintf(stderr, "A DeckLink iterator could not be created.  The DeckLink drivers may not be installed.\n");
        return NULL;
    }
#else
    deckLinkIterator = CreateDeckLinkIteratorInstance();	
#endif // (CISST_OS == CISST_WINDOWS)

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
    if(debug)
        printf("svlVidCapSrcBMD::GetDeviceList() called\n");
    if (deviceinfo == 0 || Initialized) return SVL_FAIL;

    IDeckLinkIterator*			deckLinkIterator = GetIDeckLinkIterator();
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
            std::string description;
#if (CISST_OS == CISST_WINDOWS)
            BSTR deviceNameBSTR = NULL;
            result = deckLink->GetModelName(&deviceNameBSTR);
            // name
            if(result!= S_OK)
                description = "BlackMagic DeckLink";
            else
                description = deviceNameBSTR;

#else
            char *		deviceNameString = NULL;
            result = deckLink->GetModelName((const char **)(&deviceNameString));
            // name
            if (result != S_OK)
                description = "BlackMagic DeckLink";
            else
                description = deviceNameString;

#endif // _WIN32

            // platform
            deviceinfo[0][i].platform = svlFilterSourceVideoCapture::BlackMagicDeckLink;

            // id
            deviceinfo[0][i].id = i;

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
            if(deckLink != NULL)
                deckLink->Release();
        }
    }
    else {
        deviceinfo[0] = 0;
    }

    if(deckLinkIterator != NULL)
        deckLinkIterator->Release();
    return BMDNumberOfDevices;
}

int svlVidCapSrcBMD::Open()
{
    if(debug)
        printf("svlVidCapSrcBMD::Open() called\n");

    if (NumOfStreams <= 0) return SVL_FAIL;
    if (Initialized) return SVL_OK;

    Close();

    IDeckLinkIterator			*deckLinkIterator = GetIDeckLinkIterator();
    IDeckLink*					deckLink;
    IDeckLinkInput*				deckLinkInput;
    DeckLinkCaptureDelegate 	*delegate;
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

    return SVL_OK;

labError:
    Close();
    return SVL_FAIL;
}

void svlVidCapSrcBMD::Close()
{
    if(debug)
        printf("svlVidCapSrcBMD::Close() called\n");

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
        if(deckLinkInput != NULL)
        {
            deckLinkInput->Release();
            deckLinkInput = NULL;
        }
        if(deckLink != NULL)
        {
            deckLink->Release();
            deckLink = NULL;
        }
    }

    if(deckLinkInput != NULL)
        deckLinkInput->Release();
    if(deckLink != NULL)
        deckLink->Release();
    if(deckLinkIterator != NULL)
        deckLinkIterator->Release();

labError:
    return;
}

int svlVidCapSrcBMD::Start()
{
    if(debug)
        printf("svlVidCapSrcBMD::Start() called\n");

    if (!Initialized) return SVL_FAIL;
    Running = true;

    return SVL_OK;
}

svlImageRGB* svlVidCapSrcBMD::GetLatestFrame(bool waitfornew, unsigned int videoch)
{
    if (videoch >= NumOfStreams || DeviceID[videoch] < 0) return 0;
    // if(debug)
    //   printf("Getting latest frame, channel %d \n",videoch);
    return ImageBuffer[videoch]->Pull(waitfornew);
}

int svlVidCapSrcBMD::Stop()
{
    if(debug)
        printf("svlVidCapSrcBMD::Stop() called\n");

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
    formatlist[0][0].framerate = frameRate;
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
    format.framerate = frameRate;
    format.custom_mode = -1;

    return SVL_OK;
}

/**************************************/
/*** DeckLinkCaptureDelegate class ******/
/**************************************/

DeckLinkCaptureDelegate::DeckLinkCaptureDelegate(svlBufferImage* buffer) : m_refCount(0)
{
    m_mutex = new osaMutex();
    g_timecodeFormat = (BMDTimecodeFormat)0;
    m_buffer = buffer;
    debug = false;
}

DeckLinkCaptureDelegate::~DeckLinkCaptureDelegate()
{
    if(m_mutex)
        m_mutex->~osaMutex();
    return;
}

ULONG DeckLinkCaptureDelegate::AddRef(void)
{
    m_mutex->Lock();
    m_refCount++;
    m_mutex->Unlock();
    return (ULONG)m_refCount;
}

ULONG DeckLinkCaptureDelegate::Release(void)
{
    m_mutex->Lock();
    m_refCount--;
    m_mutex->Unlock();

    if(m_buffer)
    {
        delete m_buffer;
        m_buffer = 0;
    }
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
            //if(debug)
            //   printf("Processing Video # %d \n",frameCount);
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
    if(videoFrame)
    {
        void* source;;
        unsigned char* dest = m_buffer->GetPushBuffer();

        videoFrame->GetBytes(&source);

        svlConverter::UYVYtoRGB24(static_cast<unsigned char *>(source), dest,videoFrame->GetHeight()*videoFrame->GetWidth(),true,true,true);
        m_buffer->Push();
    }
}



