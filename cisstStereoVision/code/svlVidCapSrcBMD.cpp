/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

Author(s):  Wen P. Liu
Created on: 2011

(C) Copyright 2006-2013 Johns Hopkins University (JHU), All Rights
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
    BMDNumberOfInputDevices(-1),
    Initialized(false),
    DeviceID(0),
    DisplayMode(0),
    NumberOfFormats(0),
    ImageBuffer(0),
    SelectedDeckLink(0),
    SelectedDeckLinkInput(0),
    Delegate(0),
    PixelFormat(bmdFormat8BitYUV), // not sure if this is an option for the user?
    InputFlags(0),
    Debug(false)
{
    // initialize vector containing flags for cards having input interfaces
    DeckLinkHasInputInterface.SetSize(10);
    DeckLinkHasInputInterface.SetAll(false);
}

svlVidCapSrcBMD::~svlVidCapSrcBMD()
{
    Close();

    if (ImageBuffer) delete [] ImageBuffer;
    if (SelectedDeckLink) delete [] SelectedDeckLink;
    if (SelectedDeckLinkInput) delete [] SelectedDeckLinkInput;
    if (Delegate) delete [] Delegate;
}

IDeckLinkIterator* CreateDeckLinkIteratorInstance(void);

IDeckLinkIterator* svlVidCapSrcBMD::GetIDeckLinkIterator()
{
    IDeckLinkIterator* deck_link_iterator;
#if (CISST_OS == CISST_WINDOWS)
    HRESULT	 result;
    // Create an IDeckLinkIterator object to enumerate all DeckLink cards in the system
    result = CoCreateInstance(CLSID_CDeckLinkIterator, 0, CLSCTX_ALL, IID_IDeckLinkIterator, (void**)&deck_link_iterator);
    if (FAILED(result)) {
        fprintf(stderr, "A DeckLink iterator could not be created.  The DeckLink drivers may not be installed.\n");
        return 0;
    }
#else
    deck_link_iterator = CreateDeckLinkIteratorInstance();
#endif // (CISST_OS == CISST_WINDOWS)

    return deck_link_iterator;
}

svlFilterSourceVideoCapture::PlatformType svlVidCapSrcBMD::GetPlatformType()
{
    return svlFilterSourceVideoCaptureTypes::BlackMagicDeckLink;
}

int svlVidCapSrcBMD::SetStreamCount(unsigned int numofstreams)
{
    if (numofstreams < 1) return SVL_FAIL;

    NumOfStreams = numofstreams;

    DeviceID = new int[NumOfStreams];
    DisplayMode = new BMDDisplayMode[NumOfStreams];
    NumberOfFormats = new int[NumOfStreams];
    ImageBuffer = new svlBufferImage*[NumOfStreams];
    SelectedDeckLink = new IDeckLink*[NumOfStreams];
    SelectedDeckLinkInput = new IDeckLinkInput*[NumOfStreams];
    Delegate = new DeckLinkCaptureDelegate*[NumOfStreams];

    for (unsigned int i = 0; i < NumOfStreams; i ++) {
        DeviceID[i] = -1;
        DisplayMode[i] = bmdModeUnknown;
        NumberOfFormats[i] = -1;
        ImageBuffer[i] = 0;
        SelectedDeckLink[i] = 0;
        SelectedDeckLinkInput[i] = 0;
        Delegate[i] = 0;
    }

    return SVL_OK;
}

int svlVidCapSrcBMD::EnumerateDevices()
{
    if (!DeckLinkHasInputInterface.empty()) {
        DeckLinkHasInputInterface.resize(10);
        DeckLinkHasInputInterface.SetAll(false);
    }
    BMDNumberOfInputDevices = 0;

    IDeckLinkIterator * deck_link_iterator = GetIDeckLinkIterator();
    IDeckLink * deck_link;
    IDeckLinkInput* deck_link_input;

    // Enumerate all cards in this system
    int i = 0;
    while (deck_link_iterator->Next(&deck_link) == S_OK)
    {
        // check if the card has an input interface else ignore e.g. Mini Monitor 
        if (deck_link->QueryInterface(IID_IDeckLinkInput, (void**)&deck_link_input) == S_OK) {
            DeckLinkHasInputInterface[i] = true;
            BMDNumberOfInputDevices++;
        }
        deck_link->Release();
        i++;
    }
    DeckLinkHasInputInterface.resize(i);
    
    if(deck_link_iterator != NULL) deck_link_iterator->Release();
    if (deck_link_input != NULL) deck_link_input->Release();

    return BMDNumberOfInputDevices;
}

int svlVidCapSrcBMD::GetDeviceList(svlFilterSourceVideoCapture::DeviceInfo **deviceinfo)
{
    if(Debug)
        printf("svlVidCapSrcBMD::GetDeviceList() called\n");
    if (deviceinfo == 0 || Initialized) return SVL_FAIL;

    if (BMDNumberOfInputDevices < 0) EnumerateDevices();
    
    // get the deck link iterator
    IDeckLinkIterator * deck_link_iterator = GetIDeckLinkIterator();
    IDeckLink * deck_link;
    HRESULT result;

    // Allocate memory for device info array
    if (BMDNumberOfInputDevices > 0) {
        deviceinfo[0] = new svlFilterSourceVideoCapture::DeviceInfo[BMDNumberOfInputDevices];
        int d = 0; // indexing through the all decklink devices
        int i = 0; // indexing through decklink devices with inputs
        while (deck_link_iterator->Next(&deck_link) == S_OK)
        {
            // only list the devices that have input interfaces
            if (DeckLinkHasInputInterface[d])
            {
                std::string description;
#if (CISST_OS == CISST_WINDOWS)
                BSTR deviceNameBSTR = NULL;
                result = deck_link->GetModelName(&deviceNameBSTR);
                // name
                if(result!= S_OK)
                    description = "BlackMagic DeckLink";
                else {
                    std::wstring name(deviceNameBSTR);
                    description = std::string(name.begin(), name.end());
                    //std::cerr << "svlVidCapBMD: " << description << std::endl;
                }
#else
#if (CISST_OS == CISST_LINUX)
                char * deviceNameString = NULL;
                result = deck_link->GetModelName((const char **)(&deviceNameString));
                // name
                if (result != S_OK)
                    description = "BlackMagic DeckLink";
                else
                    description = deviceNameString;
#else // Mac OS
                CFStringRef deviceNameCFString = 0;
                result = deck_link->GetModelName(&deviceNameCFString);
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
                deviceinfo[0][i].platform = svlFilterSourceVideoCaptureTypes::BlackMagicDeckLink;

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
            }
            d++;
        }
    }
    else {
        deviceinfo[0] = 0;
    }
    return BMDNumberOfInputDevices;
}

int svlVidCapSrcBMD::SetDevice(int device_id, int CMN_UNUSED(input_id), unsigned int videoch)
{
    if (videoch >= NumOfStreams) return SVL_FAIL;
    if (BMDNumberOfInputDevices < 0) EnumerateDevices();
    if (device_id >= BMDNumberOfInputDevices) return SVL_FAIL;
    DeviceID[videoch] = device_id;
    // Input ID is ignored, currently not suppporting multiple input formats
    return SVL_OK;
}

int svlVidCapSrcBMD::EnumerateFormats(int device_id, vctDynamicVector<BMDDisplayMode> &display_modes)
{
    // check if devices have been enumerated
    if (BMDNumberOfInputDevices < 0) EnumerateDevices();
    // check device_id
    if (device_id >= BMDNumberOfInputDevices) return SVL_FAIL;

    // get provided device_id
    IDeckLinkIterator * deck_link_iterator = GetIDeckLinkIterator();
    IDeckLink * deck_link = 0;
    unsigned int d = 0; // number of devices
    int i = 0; // number of input devices
    while (deck_link_iterator->Next(&deck_link) == S_OK) {
        if (DeckLinkHasInputInterface[d]) {
            if (i == device_id) break;
            i++;
        }
        d++;
    }
    deck_link_iterator->Release();

    IDeckLinkInput * deck_link_input;
    deck_link->QueryInterface(IID_IDeckLinkInput,(void **)&deck_link_input);
    deck_link->Release();
    
    IDeckLinkDisplayModeIterator * display_mode_iterator = NULL;
    deck_link_input->GetDisplayModeIterator(&display_mode_iterator);
    deck_link_input->Release();

    IDeckLinkDisplayMode * temp_displaymode = NULL;
    display_modes.resize(100);
    display_modes.SetAll(bmdModeUnknown);
    WidthHeightFramerateLookup.clear();

    int f = 0;
    while(display_mode_iterator->Next(&temp_displaymode) == S_OK) {
        BMDTimeValue frame_duration, time_scale;
        temp_displaymode->GetFrameRate(&frame_duration, &time_scale);
        WidthHeightFramerateLookup[temp_displaymode->GetDisplayMode()] = 
            widthHeightFramerate(temp_displaymode->GetWidth(), temp_displaymode->GetHeight(), 
                double(time_scale)/double(frame_duration));
        display_modes[f] = temp_displaymode->GetDisplayMode();
        f++;
    }
    display_modes.resize(f);
    return f;
}

int svlVidCapSrcBMD::GetFormatList(unsigned int device_id, svlFilterSourceVideoCapture::ImageFormat **format_list)
{
    if (format_list == 0) return SVL_FAIL;
    vctDynamicVector<BMDDisplayMode> display_modes;
    if (EnumerateFormats(device_id, display_modes) < 0) return SVL_FAIL;

    format_list[0] = new svlFilterSourceVideoCapture::ImageFormat[display_modes.size()];
    
    for (int f = 0; f < display_modes.size(); f++) {
        widthHeightFramerate temp_whf = WidthHeightFramerateLookup[display_modes[f]];
        format_list[0][f].width = temp_whf.Width;
        format_list[0][f].height = temp_whf.Height;
        format_list[0][f].colorspace = svlFilterSourceVideoCaptureTypes::PixelRGB8;
        format_list[0][f].rgb_order = true;
        format_list[0][f].yuyv_order = false;
        format_list[0][f].framerate = temp_whf.Framerate;
        format_list[0][f].custom_mode = -1;
        format_list[0][f].custom_framerate = display_modes[f]; /// \todo(dmirota1)  Ask Balazs about where to storage extra information
    }
    return display_modes.size();
}

int svlVidCapSrcBMD::GetFormat(svlFilterSourceVideoCapture::ImageFormat& format, unsigned int videoch)
{
    if (videoch >= NumOfStreams) return SVL_FAIL;

    widthHeightFramerate temp_whf = WidthHeightFramerateLookup[DisplayMode[videoch]];
    format.width = temp_whf.Width;
    format.height = temp_whf.Height;
    format.colorspace = svlFilterSourceVideoCaptureTypes::PixelRGB8;
    format.rgb_order = true;
    format.yuyv_order = false;
    format.framerate = temp_whf.Framerate;
    format.custom_mode = -1;
    format.custom_framerate = DisplayMode[videoch];

    return SVL_OK;
}

int svlVidCapSrcBMD::SetFormat(svlFilterSourceVideoCapture::ImageFormat& format, unsigned int videoch)
{
    if (videoch >= NumOfStreams || Initialized) return SVL_FAIL;
    // device would be always set before the format gets set, so lets enumerate format list
    if (NumberOfFormats[videoch] < 0) EnumerateFormats(DeviceID[videoch]);
    // set display mode for current channel/stream
    DisplayMode[videoch] = static_cast<BMDDisplayMode> (format.custom_framerate);

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

int svlVidCapSrcBMD::Open()
{
    if(Debug) {
        std::cerr << "svlVidCapSrcBMD::Open() called" << std::endl;
    }

    if (NumOfStreams <= 0) return SVL_FAIL;
    if (Initialized) return SVL_OK;

    Close();
    
    HRESULT result;
    for (unsigned int i = 0; i < NumOfStreams; i ++)
    {
        IDeckLinkIterator * deck_link_iterator = GetIDeckLinkIterator();
        // Connect to the decklink set by the user stored in DeviceID[streamid]
        int count = 0;
        for (int d = 0; d <= DeviceID[i]; d++) { 
            result = deck_link_iterator->Next(&SelectedDeckLink[i]);
            if (result != S_OK) {
                std::cerr << "svlVidCapSrcBMD::Open - deck_link_iterator->Next() returned error: " << result << std::endl;
                goto labError;
            }
            count++;
        }
        deck_link_iterator->Release();
        // get the input interface for the selected device
        if (SelectedDeckLink[i]->QueryInterface(IID_IDeckLinkInput, reinterpret_cast<void**>(&SelectedDeckLinkInput[i])) != S_OK) {
            std::cerr << "svlVidCapSrcBMD::Open - deck_link_iterator->QueryInterface() returned error" << std::endl;
            goto labError;
        }

        // Set callback delegate
        // Allocate capture buffers
        ImageBuffer[i] = new svlBufferImage(WidthHeightFramerateLookup[DisplayMode[i]].Width,
            WidthHeightFramerateLookup[DisplayMode[i]].Height);
        Delegate[i] = new DeckLinkCaptureDelegate(ImageBuffer[i]);
        SelectedDeckLinkInput[i]->SetCallback(Delegate[i]);

        // Opening device
        if(SelectedDeckLinkInput[i]->EnableVideoInput(DisplayMode[i], PixelFormat, InputFlags) != S_OK) {
            std::cerr << "svlVidCapSrcBMD::Open - SelectedDeckLinkInput[" << i << "]->EnableVideoInput() returned error: "; //<< result << std::endl;
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
        if (SelectedDeckLinkInput[i]->StartStreams() != S_OK) {
            std::cerr << "svlVidCapSrcBMD::Open - deckLinkInput[" << i << "]->StartStreams() returned error: " << result << std::endl;
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
    if(Debug) {
        std::cerr << "svlVidCapSrcBMD::Close() called" << std::endl;
    }

    Stop();

    for (unsigned int i = 0; i < NumOfStreams; i ++) {
        if(SelectedDeckLinkInput[i] != 0) {
            SelectedDeckLinkInput[i]->StopStreams();
            SelectedDeckLinkInput[i]->Release();
            SelectedDeckLinkInput[i] = 0;
        }
        if(SelectedDeckLink[i] != 0) {
            SelectedDeckLink[i]->Release();
            SelectedDeckLink[i] = 0;
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
    if (Debug) {
        std::cerr << "svlVidCapSrcBMD::Start() called" << std::endl;
    }

    if (!Initialized) return SVL_FAIL;
    Running = true;

    return SVL_OK;
}

svlImageRGB* svlVidCapSrcBMD::GetLatestFrame(bool waitfornew, unsigned int videoch)
{
    if (videoch >= NumOfStreams || !ImageBuffer[videoch]) return 0;
    if (Debug) {
        std::cerr << "svlVidCapSrcBMD::GetLatestFrame(" << waitfornew << ", " << videoch << ") called" << std::endl;
    }
    return ImageBuffer[videoch]->Pull(waitfornew);
}

int svlVidCapSrcBMD::Stop()
{
    if(Debug) {
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



/*************************************/
/*** DeckLinkCaptureDelegate class ***/
/*************************************/

DeckLinkCaptureDelegate::DeckLinkCaptureDelegate(svlBufferImage* buffer)
    :MRefCount(0),
    MBuffer(buffer),
    GTimecodeFormat(static_cast<BMDTimecodeFormat>(0)),
    FrameCounter(0),
    Debug(false)
{
    MMutex = new osaMutex();
}

DeckLinkCaptureDelegate::~DeckLinkCaptureDelegate()
{
    if (MMutex) delete MMutex;
}

HRESULT DeckLinkCaptureDelegate::QueryInterface(REFIID CMN_UNUSED(iid), LPVOID* CMN_UNUSED(ppv))
{
    return E_NOINTERFACE;
}

ULONG DeckLinkCaptureDelegate::AddRef(void)
{
    MMutex->Lock();
    MRefCount ++;
    MMutex->Unlock();
    return static_cast<ULONG>(MRefCount);
}

ULONG DeckLinkCaptureDelegate::Release(void)
{
    MMutex->Lock();
    MRefCount --;
    MMutex->Unlock();
    return static_cast<ULONG>(MRefCount);
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
            std::cerr << "Frame received (" << FrameCounter << ") - No input signal detected" << std::endl;
        }
        else {
            ProcessVideoFrame(videoFrame);
        }

        if (rightEyeFrame) rightEyeFrame->Release();

        FrameCounter++;
    }

    if (audioFrame) {
        // NOP
    }

    return S_OK;
}

void DeckLinkCaptureDelegate::ProcessVideoFrame(IDeckLinkVideoInputFrame* videoFrame)
{
    if (videoFrame) {
        void* source = 0;
        videoFrame->GetBytes(&source);

        unsigned char* dest = MBuffer->GetPushBuffer();
        svlConverter::UYVYtoRGB24(reinterpret_cast<unsigned char*>(source),
                                  dest,
                                  videoFrame->GetWidth() * videoFrame->GetHeight(),
                                  true, true, true);
        MBuffer->Push();
    }
}

