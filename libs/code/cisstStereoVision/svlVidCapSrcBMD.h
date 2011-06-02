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

#ifndef _svlVidCapSrcBMD_h
#define _svlVidCapSrcBMD_h

#include <stdio.h>
#include <stdlib.h>

#include <cisstStereoVision/svlFilterSourceVideoCapture.h>
#include <cisstStereoVision/svlBufferImage.h>
#include <cisstStereoVision.h>

#include "DeckLinkAPI.h"

class svlBufferImage;

class svlVidCapSrcBMD : public svlVidCapSrcBase, public cmnGenericObject
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

private:
    unsigned int NumOfStreams;
	bool Running;
	bool Initialized;
	BMDVideoInputFlags			inputFlags;
	BMDDisplayMode				displayMode;
	BMDPixelFormat				pixelFormat;
	int width, height;

	int	BMDNumberOfDevices;
	int* DeviceID;
    svlBufferImage** ImageBuffer;

	void SetWidthHeightByBMDDisplayMode();
	IDeckLinkIterator* GetIDeckLinkIterator();
  
};

class DeckLinkCaptureDelegate : public IDeckLinkInputCallback
{
public:
	DeckLinkCaptureDelegate(svlBufferImage* buffer);
	~DeckLinkCaptureDelegate();

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv) { return E_NOINTERFACE; }
	virtual ULONG STDMETHODCALLTYPE AddRef(void);
	virtual ULONG STDMETHODCALLTYPE  Release(void);
	virtual HRESULT STDMETHODCALLTYPE VideoInputFormatChanged(BMDVideoInputFormatChangedEvents, IDeckLinkDisplayMode*, BMDDetectedVideoInputFormatFlags){return S_OK;}

	virtual HRESULT STDMETHODCALLTYPE VideoInputFrameArrived(IDeckLinkVideoInputFrame*, IDeckLinkAudioInputPacket*); 
private:
	void processVideoFrame(IDeckLinkVideoInputFrame* videoFrame);

	ULONG				m_refCount;
	//pthread_mutex_t		m_mutex;
	BMDTimecodeFormat		g_timecodeFormat;
	int frameCount;
	svlBufferImage*		m_buffer;
};

CMN_DECLARE_SERVICES_INSTANTIATION(svlVidCapSrcBMD)

#endif // _svlVidCapSrcBMD_h

