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
    int SetFormat(svlFilterSourceVideoCapture::ImageFormat& format, unsigned int videoch = 0);

private:
    unsigned int        NumOfStreams;
    bool                Running;
    bool                Initialized;
    BMDVideoInputFlags	inputFlags;
    BMDDisplayMode		displayMode;
    BMDPixelFormat		pixelFormat;
    int                 width, height;
    double              frameRate;
    bool                debug;

    int	BMDNumberOfDevices;
    int* DeviceID;
    svlBufferImage** ImageBuffer;
    IDeckLink** deckLink;
    IDeckLinkInput** deckLinkInput;
    DeckLinkCaptureDelegate** Delegate;

    std::vector<IDeckLink*> available_decklinks;

    void GetWidthHeightfromBMDDisplayMode(const BMDPixelFormat pixelFormat_in,int  &width_int, int &height_out, double &frameRate_out);
    IDeckLinkIterator* GetIDeckLinkIterator();

    std::vector<BMDDisplayMode> supported_displayModes; /// \todo(dmirota1)  This needs to be expanded to be per device per available inputs.

    class width_height_framerate{
        public:
        width_height_framerate(int width_in, int height_in, double framerate_in){width = width_in;height = height_in;framerate = framerate_in;}
        width_height_framerate(){width = 0;height = 0;framerate = 0;}
        width_height_framerate(const width_height_framerate & rhs_in){width = rhs_in.width;height = rhs_in.height;framerate = rhs_in.framerate;}
        int width;
        int height;
        double framerate;

        bool operator <(const width_height_framerate rhs_in) const {return width < rhs_in.width && height < rhs_in.height && framerate < rhs_in.framerate;}


    };

    //std::map<width_height_framerate, BMDDisplayMode> bmdDisplayMode_lookup; /// \note Used to lookup BMDDisplayMode given a height, width and frameRate
    std::map<BMDDisplayMode, width_height_framerate> width_height_framerate_lookup;

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
    void processVideoFrame(IDeckLinkVideoInputFrame* videoFrame);

    ULONG			    m_refCount;
    osaMutex*           m_mutex;
    BMDTimecodeFormat	g_timecodeFormat;
    int                 frameCount;
    svlBufferImage*		m_buffer;
    bool                debug;
};

CMN_DECLARE_SERVICES_INSTANTIATION(svlVidCapSrcBMD)

#endif // _svlVidCapSrcBMD_h

