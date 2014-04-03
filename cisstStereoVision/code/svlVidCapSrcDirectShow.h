/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2006 

  (C) Copyright 2006-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstStereoVision/svlFilterSourceVideoCapture.h>
#include <cisstStereoVision/svlBufferImage.h>
#include "svlVidCapSrcDirectShowInputSelector.h"


//////////////////////////////////////////////////////////////
// qedit.h is missing from the latest Windows SDK versions. //
// Here is a re-definition of the sample grabber interfaces //
// in order to be able to access qedit.dll                  //
//////////////////////////////////////////////////////////////

#ifndef __qedit_h__
#define __qedit_h__

#pragma once

interface ISampleGrabberCB : public IUnknown
{
	virtual STDMETHODIMP SampleCB( double SampleTime, IMediaSample *pSample ) = 0;
	virtual STDMETHODIMP BufferCB( double SampleTime, BYTE *pBuffer, long BufferLen ) = 0;
};

static const IID IID_ISampleGrabberCB          = { 0x0579154A, 0x2B53, 0x4994, { 0xB0, 0xD0, 0xE7, 0x73, 0x14, 0x8E, 0xFF, 0x85 } };

interface ISampleGrabber : public IUnknown
{
	virtual HRESULT STDMETHODCALLTYPE SetOneShot( BOOL OneShot ) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetMediaType( const AM_MEDIA_TYPE *pType ) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetConnectedMediaType( AM_MEDIA_TYPE *pType ) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetBufferSamples( BOOL BufferThem ) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetCurrentBuffer( long *pBufferSize, long *pBuffer ) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetCurrentSample( IMediaSample **ppSample ) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetCallback( ISampleGrabberCB *pCallback, long WhichMethodToCallback ) = 0;
};

static const IID IID_ISampleGrabber            = { 0x6B652FFF, 0x11FE, 0x4fce, { 0x92, 0xAD, 0x02, 0x66, 0xB5, 0xD7, 0xC7, 0x8F } };
static const CLSID CLSID_SampleGrabber         = { 0xC1F400A0, 0x3F08, 0x11d3, { 0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37 } };
static const CLSID CLSID_NullRenderer          = { 0xC1F400A4, 0x3F08, 0x11d3, { 0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37 } };
static const CLSID CLSID_VideoEffects1Category = { 0xcc7bfb42, 0xf175, 0x11d1, { 0xa3, 0x92, 0x0, 0xe0, 0x29, 0x1f, 0x39, 0x59 } };
static const CLSID CLSID_VideoEffects2Category = { 0xcc7bfb43, 0xf175, 0x11d1, { 0xa3, 0x92, 0x0, 0xe0, 0x29, 0x1f, 0x39, 0x59 } };
static const CLSID CLSID_AudioEffects1Category = { 0xcc7bfb44, 0xf175, 0x11d1, { 0xa3, 0x92, 0x0, 0xe0, 0x29, 0x1f, 0x39, 0x59 } };
static const CLSID CLSID_AudioEffects2Category = { 0xcc7bfb45, 0xf175, 0x11d1, { 0xa3, 0x92, 0x0, 0xe0, 0x29, 0x1f, 0x39, 0x59 } };

#endif


#ifndef _svlVidCapSrcDirectShow_h
#define _svlVidCapSrcDirectShow_h

class svlVidCapSrcDirectShow;

class svlVidCapSrcDirectShowCB : public ISampleGrabberCB
{
public:
	STDMETHODIMP_(ULONG) AddRef() { return 1; }

    STDMETHODIMP_(ULONG) Release() { return 2; }

	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject)
	{
		if (NULL == ppvObject) return E_POINTER;
		if (riid == __uuidof(IUnknown)) {
			*ppvObject = static_cast<IUnknown*>(this);
			return S_OK;
		}
		if (riid == IID_ISampleGrabberCB) {
			*ppvObject = static_cast<ISampleGrabberCB*>(this);
			return S_OK;
		}
		return E_NOTIMPL;
	}

    STDMETHODIMP SampleCB(double SampleTime, IMediaSample *pSample);
    STDMETHODIMP BufferCB(double sampletime, unsigned char *buffer, long buffersize);

    svlVidCapSrcDirectShowCB(svlBufferImage *buffer, bool topdown);

protected:
    svlBufferImage *Buffer;
    bool TopDown;
};


class svlVidCapSrcDirectShow : public svlVidCapSrcBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
	svlVidCapSrcDirectShow();
	~svlVidCapSrcDirectShow();

private:
	static int CoInitCounter;

    bool Initialized;
    unsigned int NumOfStreams;
	bool RunState;
	IGraphBuilder *pGraph;
	ICaptureGraphBuilder2 *pGraphBuilder;
	IGraphBuilder *pTestGraph;
	ICaptureGraphBuilder2 *pTestBuilder;
    IBaseFilter *pTestCapFilt;

	int *DeviceID;
	int *InputID;
	int *CapWidth;
	int *CapHeight;
	bool *CapTopDown;
	int *EnableRenderer;
    IBaseFilter **pCaptureFilter;
	IBaseFilter **pIntermediatePreviewFilter;
	IBaseFilter **pAviDecomprFilter;
	IBaseFilter **pColorConvFilter;
	IBaseFilter **pSampleGrabFilter;
	IPin **pCaptureFilterOut;
    GUID *PinCategory;
    ISampleGrabber **pSampleGrabber;
	svlVidCapSrcDirectShowCB **pCallBack;
	unsigned char **pMediaType;
	unsigned int *MediaTypeLength;
    svlBufferImage **OutputBuffer;

private:
    void Release();

    int AssembleGraph();
	void DisassembleGraph();
	void EmptyGraph(IFilterGraph* graph, bool keepsources = false);

    int TestOpen(int devid);
    void TestClose();

	IBaseFilter* GetCaptureFilter(int devid);

	IPin* EnumeratePin(IBaseFilter *filter, LPCTSTR name, GUID *category = NULL);
    int GetPinCategory(IPin *pin, GUID *category);

	int SetDeviceInput(IBaseFilter *capfilter, int input_id = 0);
	int GetDeviceInputs(IBaseFilter *capfilter, svlFilterSourceVideoCapture::DeviceInfo *deviceinfo);

public:
    svlFilterSourceVideoCapture::PlatformType GetPlatformType();
    int SetStreamCount(unsigned int numofstreams);
	int GetDeviceList(svlFilterSourceVideoCapture::DeviceInfo **deviceinfo);
	int Open();
	void Close();
	int Start();
    svlImageRGB* GetLatestFrame(bool waitfornew, unsigned int videoch = 0);
	int Stop();
	bool IsRunning() { return RunState; }
    int SetDevice(int devid, int inid, unsigned int videoch = 0);
	int GetWidth(unsigned int videoch = 0);
	int GetHeight(unsigned int videoch = 0);

    int ShowFormatDialog(HWND hwnd, unsigned int videoch = 0);
	int ShowImageDialog(HWND hwnd, unsigned int videoch = 0);
	int SetupMediaType(unsigned int videoch = 0);
	int RequestMediaType(unsigned int videoch = 0);
	int GetMediaType(unsigned char *&mediabuffer, unsigned int &length, unsigned int videoch = 0);
	int SetMediaType(unsigned char *mediabuffer, unsigned int length, unsigned int videoch = 0);
    int SetRendererOnOff(bool render, unsigned int videoch = 0);
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlVidCapSrcDirectShow)

#endif // _svlVidCapSrcDirectShow_h

