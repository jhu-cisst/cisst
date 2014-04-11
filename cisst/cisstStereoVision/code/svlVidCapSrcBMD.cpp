/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
$Id$

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


/*****************************/
/*** svlVidCapSrcBMD class ***/
/*****************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlVidCapSrcBMD, svlVidCapSrcBase)

svlVidCapSrcBMD::svlVidCapSrcBMD() :
    svlVidCapSrcBase(),
    NumOfStreams(0),
    Running(false),
    Initialized(false),
    ImageBuffer(0),
    deckLink(0),
    deckLinkInput(0),
    Delegate(0),
    BMDNumberOfDevices(0),
    inputFlags(0),
    displayMode(bmdModeNTSC),
    width(0),
    height(0),
    frameRate(0),
    pixelFormat(bmdFormat8BitYUV),
    debug(false)
{
    /// \todo(dmirota1)  This should not be changing at runtime
    /*supported_displayModes.push_back(bmdModeNTSC);
    supported_displayModes.push_back(bmdModeNTSC2398);
    supported_displayModes.push_back(bmdModeNTSCp);
    supported_displayModes.push_back(bmdModePAL);
    supported_displayModes.push_back(bmdModePALp);
    supported_displayModes.push_back(bmdModeHD1080p2398);
    supported_displayModes.push_back(bmdModeHD1080p24);
    supported_displayModes.push_back(bmdModeHD1080p25);
    supported_displayModes.push_back(bmdModeHD1080p2997);
    supported_displayModes.push_back(bmdModeHD1080p30);
    supported_displayModes.push_back(bmdModeHD1080p50);
    supported_displayModes.push_back(bmdModeHD1080p5994);
    supported_displayModes.push_back(bmdModeHD1080p6000);
    supported_displayModes.push_back(bmdModeHD1080i50);
    supported_displayModes.push_back(bmdModeHD1080i5994);
    supported_displayModes.push_back(bmdModeHD1080i6000);
    supported_displayModes.push_back(bmdModeHD720p50);
    supported_displayModes.push_back(bmdModeHD720p5994);
    supported_displayModes.push_back(bmdModeHD720p60);
    supported_displayModes.push_back(bmdMode2k2398);
    supported_displayModes.push_back(bmdMode2k24);
    supported_displayModes.push_back(bmdMode2k25);*/


    /// \todo(dmirota1) Should be a better way to do this.
    /*bmdDisplayMode_lookup[width_height_framerate(720,486,30)] = bmdModeNTSC;
    bmdDisplayMode_lookup[width_height_framerate(720,486,23.98)] = bmdModeNTSC2398;
    bmdDisplayMode_lookup[width_height_framerate(720,486,60)] = bmdModeNTSCp;
    bmdDisplayMode_lookup[width_height_framerate(720,576,30)] = bmdModePAL;
    bmdDisplayMode_lookup[width_height_framerate(720,576,60)] = bmdModePALp;
    bmdDisplayMode_lookup[width_height_framerate(1920,1080,23.98)] = bmdModeHD1080p2398;
    bmdDisplayMode_lookup[width_height_framerate(1920,1080,24)] = bmdModeHD1080p24;
    bmdDisplayMode_lookup[width_height_framerate(1920,1080,25)] = bmdModeHD1080p25;
    bmdDisplayMode_lookup[width_height_framerate(1920,1080,29.97)] = bmdModeHD1080p2997;
    bmdDisplayMode_lookup[width_height_framerate(1920,1080,30)] = bmdModeHD1080p30;
    bmdDisplayMode_lookup[width_height_framerate(1920,1080,50)] = bmdModeHD1080p50;
    bmdDisplayMode_lookup[width_height_framerate(1920,1080,59.94)] = bmdModeHD1080p5994;
    bmdDisplayMode_lookup[width_height_framerate(1920,1080,60)] = bmdModeHD1080p6000;
    bmdDisplayMode_lookup[width_height_framerate(1280,720,50)] = bmdModeHD720p50;
    bmdDisplayMode_lookup[width_height_framerate(1280,720,59.94)] = bmdModeHD720p5994;
    bmdDisplayMode_lookup[width_height_framerate(1280,720,60)] = bmdModeHD720p60;
    bmdDisplayMode_lookup[width_height_framerate(2048,1556,23.98)] = bmdMode2k2398;
    bmdDisplayMode_lookup[width_height_framerate(2048,1556,24)] = bmdMode2k24;
    bmdDisplayMode_lookup[width_height_framerate(2048,1556,25)] = bmdMode2k25;*/

}

svlVidCapSrcBMD::~svlVidCapSrcBMD()
{
    Close();

    if (ImageBuffer) delete [] ImageBuffer;
    if (deckLink) delete [] deckLink;
    if (deckLinkInput) delete [] deckLinkInput;
    if (Delegate) delete [] Delegate;
}

void svlVidCapSrcBMD::GetWidthHeightfromBMDDisplayMode(const BMDPixelFormat displayMode_in,int  &width_out, int &height_out, double &frameRate_out)
{
    // default settings
    width_out = 1920;
    height_out = 1080;
    frameRate_out = 30;

    // width & height
    if (displayMode_in == bmdModeNTSC ||
        displayMode_in == bmdModeNTSC2398 ||
        displayMode_in == bmdModeNTSCp){
        width_out = 720;
        height_out = 486;
    }
    else if (displayMode_in == bmdModePAL ||
             displayMode_in == bmdModePALp ) {
             width_out = 720;
             height_out = 576;
    }
    else if (displayMode_in == bmdModeHD1080p2398 ||
             displayMode_in == bmdModeHD1080p24 ||
             displayMode_in == bmdModeHD1080p25 ||
             displayMode_in == bmdModeHD1080p2997 ||
             displayMode_in == bmdModeHD1080p30 ||
             displayMode_in == bmdModeHD1080i50 ||
             displayMode_in == bmdModeHD1080i5994 ||
             displayMode_in == bmdModeHD1080i6000 ||
             displayMode_in == bmdModeHD1080p50 ||
             displayMode_in == bmdModeHD1080p5994 ||
             displayMode_in == bmdModeHD1080p6000) {
        width_out = 1920;
        height_out = 1080;
    }
    else if (displayMode_in == bmdModeHD720p50 ||
             displayMode_in == bmdModeHD720p5994 ||
             displayMode_in == bmdModeHD720p60) {
        width_out = 1280;
        height_out = 720;
    }
    else if (displayMode_in == bmdMode2k2398 ||
             displayMode_in == bmdMode2k24 ||
             displayMode_in == bmdMode2k25) {
        width_out = 2048;
        height_out = 1556;
    }

    // frame rate
    switch(displayMode_in)
    {
        case bmdModeNTSC2398:
        case bmdModeHD1080p2398:
        case bmdMode2k2398:	
            frameRate_out = 23.98;
        break;

        case bmdModeHD1080p24:	
        case bmdMode2k24:	
            frameRate_out = 24.0;
        break;

        case bmdModeHD1080p25:	
        case bmdMode2k25:	
            frameRate_out = 25.0;
        break;

        case bmdModeHD1080p2997:
            frameRate_out = 29.97;
        break;

        case bmdModeNTSC:
        case bmdModePAL:
        case bmdModeHD1080p30:
            frameRate_out = 30.0;
        break;

        case bmdModeHD1080i50:
        case bmdModeHD1080p50:	
        case bmdModeHD720p50:	
        case bmdModePALp:
            frameRate_out = 50.0;
        break;

        case bmdModeHD1080i5994:
        case bmdModeHD1080p5994:	
        case bmdModeHD720p5994:
            frameRate_out = 59.94;
        break;

        case bmdModeHD1080i6000:
        case bmdModeHD1080p6000:	
        case bmdModeHD720p60:
        case bmdModeNTSCp:
            frameRate_out = 60;
        break;

        default:
            printf("Unrecognized display mode: %d\n", displayMode_in);
        break;
    }

    //printf("Width:%d, Height:%d at %4.2fHz\n", width_out, height_out, frameRate_out);
}

IDeckLinkIterator* CreateDeckLinkIteratorInstance(void);

IDeckLinkIterator* svlVidCapSrcBMD::GetIDeckLinkIterator()
{
    IDeckLinkIterator* deckLinkIterator;
#if (CISST_OS == CISST_WINDOWS)
    HRESULT	 result;
    // Create an IDeckLinkIterator object to enumerate all DeckLink cards in the system
    result = CoCreateInstance(CLSID_CDeckLinkIterator, 0, CLSCTX_ALL, IID_IDeckLinkIterator, (void**)&deckLinkIterator);
    if (FAILED(result)) {
        fprintf(stderr, "A DeckLink iterator could not be created.  The DeckLink drivers may not be installed.\n");
        return 0;
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
    deckLink = new IDeckLink*[NumOfStreams];
    deckLinkInput = new IDeckLinkInput*[NumOfStreams];
    Delegate = new DeckLinkCaptureDelegate*[NumOfStreams];

    for (unsigned int i = 0; i < NumOfStreams; i ++) {
        DeviceID[i] = -1;
        ImageBuffer[i] = 0;
        deckLink[i] = 0;
        deckLinkInput[i] = 0;
        Delegate[i] = 0;
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
        available_decklinks.push_back(deckLink);
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
            else {
                std::wstring name(deviceNameBSTR);
                description = std::string(name.begin(), name.end());
            }

#else
#if (CISST_OS == CISST_LINUX)
            char *		deviceNameString = NULL;
            result = deckLink->GetModelName((const char **)(&deviceNameString));
            // name
            if (result != S_OK)
                description = "BlackMagic DeckLink";
            else
                description = deviceNameString;
#else // Mac OS
            CFStringRef deviceNameCFString = 0;
            result = deckLink->GetModelName(&deviceNameCFString);
            if (result != S_OK) {
                description = "BlackMagic DeckLink";
            } else {
                char deviceName[64];
                CFStringGetCString(deviceNameCFString, deviceName, sizeof(deviceName), kCFStringEncodingMacRoman);
                description = deviceName;
            }

#endif // LINUX
#endif // WIN32

            // platform
            deviceinfo[0][i].platform = svlFilterSourceVideoCapture::BlackMagicDeckLink;

            // id
            deviceinfo[0][i].ID = i;

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
    if(debug) {
        std::cerr << "svlVidCapSrcBMD::Open() called" << std::endl;
    }

    if (NumOfStreams <= 0) return SVL_FAIL;
    if (Initialized) return SVL_OK;

    Close();

    HRESULT result;

    IDeckLinkIterator* deckLinkIterator = GetIDeckLinkIterator();
    if (!deckLinkIterator) {
        std::cerr << "svlVidCapSrcBMD::Open - GetIDeckLinkIterator() returned error" << std::endl;
        goto labError;
    }

    for (unsigned int i = 0; i < NumOfStreams; i ++) {
        // Connect to a DeckLink instance
        result = deckLinkIterator->Next(&deckLink[i]);
        if (result != S_OK) {
            std::cerr << "svlVidCapSrcBMD::Open - deckLinkIterator->Next() returned error: " << result << std::endl;
            goto labError;
        }

        if (deckLink[i]->QueryInterface(IID_IDeckLinkInput, reinterpret_cast<void**>(&deckLinkInput[i])) != S_OK) {
            std::cerr << "svlVidCapSrcBMD::Open - deckLinkIterator->QueryInterface() returned error" << std::endl;
            goto labError;
        }

        // Set callback delgate
        // Allocate capture buffers
        ImageBuffer[i] = new svlBufferImage(width, height);
        Delegate[i] = new DeckLinkCaptureDelegate(ImageBuffer[i]);
        deckLinkInput[i]->SetCallback(Delegate[i]);

        // Opening device
        result = deckLinkInput[i]->EnableVideoInput(displayMode, pixelFormat, inputFlags);
        if(result != S_OK) {
            std::cerr << "svlVidCapSrcBMD::Open - deckLinkInput[" << i << "]->EnableVideoInput() returned error: "; //<< result << std::endl;
            switch(result){
                case E_FAIL:
                    std::cerr << "Failed" << std::endl;
                    break;
                case E_INVALIDARG:
                    std::cerr << "Invalid Argument" << std::endl;
                    break;
                case E_ACCESSDENIED:
                    std::cerr << "Access Denied" << std::endl;
                    break;
                case E_OUTOFMEMORY:
                    std::cerr << "Out of Memory" << std::endl;
                    break;
                default:
                    break;
            }
            goto labError;
        }

        // Starting Stream
        result = deckLinkInput[i]->StartStreams();
        if (result != S_OK) {
            std::cerr << "svlVidCapSrcBMD::Open - deckLinkInput[" << i << "]->StartStreams() returned error: " << result << std::endl;
            goto labError;
        }
    }

    deckLinkIterator->Release();

    Initialized = true;

    return SVL_OK;

labError:
    Close();
    return SVL_FAIL;
}

void svlVidCapSrcBMD::Close()
{
    if(debug) {
        std::cerr << "svlVidCapSrcBMD::Close() called" << std::endl;
    }

    Stop();

    for (unsigned int i = 0; i < NumOfStreams; i ++) {
        if(deckLinkInput[i] != 0) {
            deckLinkInput[i]->StopStreams();
            deckLinkInput[i]->Release();
            deckLinkInput[i] = 0;
        }
        if(deckLink[i] != 0) {
            deckLink[i]->Release();
            deckLink[i] = 0;
        }
        if(Delegate[i] != 0) {
            delete Delegate[i];
            Delegate[i] = 0;
        }
        if(ImageBuffer[i] != 0) {
            delete ImageBuffer[i];
            ImageBuffer[i] = 0;
        }
    }
}

int svlVidCapSrcBMD::Start()
{
    if (debug) {
        std::cerr << "svlVidCapSrcBMD::Start() called" << std::endl;
    }

    if (!Initialized) return SVL_FAIL;
    Running = true;

    return SVL_OK;
}

svlImageRGB* svlVidCapSrcBMD::GetLatestFrame(bool waitfornew, unsigned int videoch)
{
    if (videoch >= NumOfStreams || !ImageBuffer[videoch]) return 0;
    if (debug) {
        std::cerr << "svlVidCapSrcBMD::GetLatestFrame(" << waitfornew << ", " << videoch << ") called" << std::endl;
    }
    return ImageBuffer[videoch]->Pull(waitfornew);
}

int svlVidCapSrcBMD::Stop()
{
    if(debug) {
        std::cerr << "svlVidCapSrcBMD::Stop() called" << std::endl;
    }

    if (!Running) return SVL_FAIL;
    Running = false;

    return SVL_OK;
}

bool svlVidCapSrcBMD::IsRunning()
{
    return Running;
}

int svlVidCapSrcBMD::SetDevice(int devid, int CMN_UNUSED(inid), unsigned int videoch)
{
    if (videoch >= NumOfStreams) return SVL_FAIL;
    DeviceID[videoch] = devid;
    // Input ID is ignored

    if(supported_displayModes.empty()){
        svlFilterSourceVideoCapture::ImageFormat *formatlist;
        GetFormatList(devid, &formatlist);
        delete formatlist;
    }

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

/// \todo(dmirota1) GetFormatList should not be changing the state of the class
int svlVidCapSrcBMD::GetFormatList(unsigned int deviceid, svlFilterSourceVideoCapture::ImageFormat **formatlist)
{
    if (formatlist == 0) return SVL_FAIL;


    IDeckLinkIterator*			deckLinkIterator = GetIDeckLinkIterator();

    IDeckLink * current_decklink;

    unsigned int i = 0;
    do{
        deckLinkIterator->Next(&current_decklink);
        i++;
    }while(i < deviceid);


    IDeckLinkInput * current_input;

    current_decklink->QueryInterface(IID_IDeckLinkInput,(void **)&current_input);


    IDeckLinkDisplayModeIterator * iterator = NULL;

    current_input->GetDisplayModeIterator(&iterator);

    IDeckLinkDisplayMode * temp_displaymode = NULL;

    supported_displayModes.clear();
    //bmdDisplayMode_lookup.clear();
    width_height_framerate_lookup.clear();

    do{

        iterator->Next(&temp_displaymode);

        if(temp_displaymode != NULL){
            supported_displayModes.push_back(temp_displaymode->GetDisplayMode());

            BMDTimeValue numerator = 0;
            BMDTimeValue denominator = 0;

            temp_displaymode->GetFrameRate(&denominator,&numerator);

            //bmdDisplayMode_lookup[width_height_framerate(temp_displaymode->GetWidth(),temp_displaymode->GetHeight(),(double)numerator/(double)denominator)] = temp_displaymode->GetDisplayMode();
            width_height_framerate_lookup[temp_displaymode->GetDisplayMode()] = width_height_framerate(temp_displaymode->GetWidth(),temp_displaymode->GetHeight(),(double)numerator/(double)denominator);
        }

    }while(temp_displaymode != NULL);



    formatlist[0] = new svlFilterSourceVideoCapture::ImageFormat[supported_displayModes.size()];

    //int current_width = 0;
    //int current_height = 0;
    //double current_frameRate = 0;

    for(int i = 0; i <  supported_displayModes.size();i++){
        //GetWidthHeightfromBMDDisplayMode(supported_displayModes.at(i),current_width,current_height,current_frameRate);
        width_height_framerate temp_width_height_framerate = width_height_framerate_lookup[supported_displayModes.at(i)];
        formatlist[0][i].width = temp_width_height_framerate.width;
        formatlist[0][i].height = temp_width_height_framerate.height;
        formatlist[0][i].colorspace = svlFilterSourceVideoCapture::PixelRGB8;
        formatlist[0][i].rgb_order = true;
        formatlist[0][i].yuyv_order = false;
        formatlist[0][i].framerate = temp_width_height_framerate.framerate;
        formatlist[0][i].custom_mode = -1;
        formatlist[0][i].custom_framerate = supported_displayModes.at(i); /// \todo(dmirota1)  Ask Balazs about where to storage extra information
    }

    return supported_displayModes.size();
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
    format.custom_framerate = displayMode;

    return SVL_OK;
}


int svlVidCapSrcBMD::SetFormat(svlFilterSourceVideoCapture::ImageFormat& format, unsigned int videoch)
{
    if (videoch >= NumOfStreams || Initialized) return SVL_FAIL;

    width = format.width;
    height = format.height;
    frameRate = format.framerate;

    displayMode = static_cast<BMDDisplayMode> (format.custom_framerate);//bmdDisplayMode_lookup[width_height_framerate(width,height,frameRate)];

    return SVL_OK;
}

/*************************************/
/*** DeckLinkCaptureDelegate class ***/
/*************************************/

DeckLinkCaptureDelegate::DeckLinkCaptureDelegate(svlBufferImage* buffer) :
    m_refCount(0)
{
    m_mutex = new osaMutex();
    g_timecodeFormat = static_cast<BMDTimecodeFormat>(0);
    m_buffer = buffer;
    debug = false;
}

DeckLinkCaptureDelegate::~DeckLinkCaptureDelegate()
{
    if (m_mutex) delete m_mutex;
}

HRESULT DeckLinkCaptureDelegate::QueryInterface(REFIID CMN_UNUSED(iid), LPVOID* CMN_UNUSED(ppv))
{
    return E_NOINTERFACE;
}

ULONG DeckLinkCaptureDelegate::AddRef(void)
{
    m_mutex->Lock();
    m_refCount ++;
    m_mutex->Unlock();
    return static_cast<ULONG>(m_refCount);
}

ULONG DeckLinkCaptureDelegate::Release(void)
{
    m_mutex->Lock();
    m_refCount --;
    m_mutex->Unlock();

    return static_cast<ULONG>(m_refCount);
}

HRESULT DeckLinkCaptureDelegate::VideoInputFrameArrived(IDeckLinkVideoInputFrame* videoFrame, IDeckLinkAudioInputPacket* audioFrame)
{
    if (videoFrame) {

        IDeckLinkVideoFrame* rightEyeFrame = 0;
        IDeckLinkVideoFrame3DExtensions* threeDExtensions = 0;

        // If 3D mode is enabled we retreive the 3D extensions interface which gives.
        // us access to the right eye frame by calling GetFrameForRightEye() .
        if (videoFrame->QueryInterface(IID_IDeckLinkVideoFrame3DExtensions, reinterpret_cast<void**>(&threeDExtensions)) != S_OK ||
            threeDExtensions->GetFrameForRightEye(&rightEyeFrame) != S_OK) {
            rightEyeFrame = 0;
        }

        if (threeDExtensions) threeDExtensions->Release();

        if (videoFrame->GetFlags() & bmdFrameHasNoInputSource) {
            std::cerr << "Frame received (" << frameCount << ") - No input signal detected" << std::endl;
        }
        else {
            processVideoFrame(videoFrame);

        }

        if (rightEyeFrame) rightEyeFrame->Release();

        frameCount ++;
    }

    if (audioFrame) {
        // NOP
    }

    return S_OK;
}

void DeckLinkCaptureDelegate::processVideoFrame(IDeckLinkVideoInputFrame* videoFrame)
{
    if (videoFrame) {
        void* source = 0;
        videoFrame->GetBytes(&source);

        unsigned char* dest = m_buffer->GetPushBuffer();
        svlConverter::UYVYtoRGB24(reinterpret_cast<unsigned char*>(source),
                                  dest,
                                  videoFrame->GetWidth() * videoFrame->GetHeight(),
                                  true, true, true);
        m_buffer->Push();
    }
}


