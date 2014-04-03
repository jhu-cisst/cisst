/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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

#ifndef _svlVidCapSrcBMD_h
#define _svlVidCapSrcBMD_h

#include <stdio.h>
#include <stdlib.h>

#include <cisstStereoVision/svlFilterSourceVideoCapture.h>
#include <cisstStereoVision/svlBufferImage.h>
#include <cisstOSAbstraction/osaMutex.h>

#if _MSC_VER >= 1500
    // Balazs: seems to be needed on Visual Studio 2008
    #define interface __interface
#endif // _MSC_VER

#include "DeckLinkAPI.h"


// Forward declarations
class svlBufferImage;
class DeckLinkCaptureDelegate;

class svlVidCapSrcBMD : public svlVidCapSrcBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    svlVidCapSrcBMD();
    ~svlVidCapSrcBMD();

    svlFilterSourceVideoCapture::PlatformType GetPlatformType();
    int SetStreamCount(unsigned int numofstreams);
    int GetDeviceList(svlFilterSourceVideoCapture::DeviceInfo **deviceinfo);
    int SetDevice(int device_id, int input_id, unsigned int videoch = 0);
    int GetFormatList(unsigned int device_id, svlFilterSourceVideoCapture::ImageFormat **format_list);
    int GetFormat(svlFilterSourceVideoCapture::ImageFormat& format, unsigned int videoch = 0);
    int SetFormat(svlFilterSourceVideoCapture::ImageFormat& format, unsigned int videoch = 0);
    int GetWidth(unsigned int videoch = 0);
    int GetHeight(unsigned int videoch = 0);
    int Open();
    void Close();
    int Start();
    svlImageRGB* GetLatestFrame(bool waitfornew, unsigned int videoch = 0);
    int Stop();
    bool IsRunning();
    

private:
    IDeckLinkIterator* GetIDeckLinkIterator();
    int EnumerateDevices();
    int EnumerateFormats(int device_id,
        vctDynamicVector<BMDDisplayMode> &display_modes = vctDynamicVector<BMDDisplayMode>());

private:
    unsigned int NumOfStreams;
    bool Running;
    bool Initialized;
    bool Debug;
    int	BMDNumberOfInputDevices;
    vctBoolVec DeckLinkHasInputInterface;

    // arrays to store device and format properties per stream (channel)
    int* DeviceID;
    BMDDisplayMode* DisplayMode;
    int* NumberOfFormats;
    svlBufferImage** ImageBuffer;
    IDeckLink** SelectedDeckLink;
    IDeckLinkInput** SelectedDeckLinkInput;
    DeckLinkCaptureDelegate** Delegate;
    // these are currently constants across different streams (channels)
    BMDVideoInputFlags InputFlags;
    BMDPixelFormat PixelFormat;

    class widthHeightFramerate
    {
    public:
        widthHeightFramerate(int _width, int _height, double _framerate)
            : Width(_width),
            Height(_height),
            Framerate(_framerate) 
        {}
        widthHeightFramerate()
            :Width(0),
            Height(0),
            Framerate(0)
        {}
        widthHeightFramerate(const widthHeightFramerate & _whf)
            :Width(_whf.Width),
            Height(_whf.Height),
            Framerate(_whf.Framerate)
        {}
        int Width;
        int Height;
        double Framerate;
    };

    std::map<BMDDisplayMode, widthHeightFramerate> WidthHeightFramerateLookup;
};

class DeckLinkCaptureDelegate : public IDeckLinkInputCallback
{
public:
    DeckLinkCaptureDelegate(svlBufferImage* buffer);
    ~DeckLinkCaptureDelegate();

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv);
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE  Release(void);
    virtual HRESULT STDMETHODCALLTYPE VideoInputFormatChanged(BMDVideoInputFormatChangedEvents, IDeckLinkDisplayMode*, BMDDetectedVideoInputFormatFlags){return S_OK;}

    virtual HRESULT STDMETHODCALLTYPE VideoInputFrameArrived(IDeckLinkVideoInputFrame*, IDeckLinkAudioInputPacket*); 
private:
    void ProcessVideoFrame(IDeckLinkVideoInputFrame* videoFrame);

    ULONG			    MRefCount;
    osaMutex*           MMutex;
    BMDTimecodeFormat	GTimecodeFormat;
    int                 FrameCounter;
    svlBufferImage*		MBuffer;
    bool                Debug;
};

CMN_DECLARE_SERVICES_INSTANTIATION(svlVidCapSrcBMD)

#endif // _svlVidCapSrcBMD_h

