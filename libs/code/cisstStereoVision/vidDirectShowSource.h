/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
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

#ifndef _vidDirectShowSource_h
#define _vidDirectShowSource_h

#include <cisstStereoVision/svlFilterSourceVideoCapture.h>
#include "svlImageBuffer.h"
#include "vidDirectShowInputSelector.h"


class CDirectShowSource;

class CDirectShowSourceCB : public ISampleGrabberCB
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
		if (riid == __uuidof(ISampleGrabberCB)) {
			*ppvObject = static_cast<ISampleGrabberCB*>(this);
			return S_OK;
		}
		return E_NOTIMPL;
	}

    STDMETHODIMP SampleCB(double SampleTime, IMediaSample *pSample);
    STDMETHODIMP BufferCB(double sampletime, unsigned char *buffer, long buffersize);

    CDirectShowSourceCB(svlImageBuffer *buffer);

protected:
    svlImageBuffer *Buffer;
};


class CDirectShowSource : public CVideoCaptureSourceBase
{
public:
	CDirectShowSource();
	~CDirectShowSource();

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
	int *EnableRenderer;
    IBaseFilter **pCaptureFilter;
	IBaseFilter **pIntermediatePreviewFilter;
	IBaseFilter **pAviDecomprFilter;
	IBaseFilter **pColorConvFilter;
	IBaseFilter **pSampleGrabFilter;
	IPin **pCaptureFilterOut;
    GUID *PinCategory;
    ISampleGrabber **pSampleGrabber;
	CDirectShowSourceCB **pCallBack;
	unsigned char **pMediaType;
	unsigned int *MediaTypeLength;
    svlImageBuffer **OutputBuffer;

private:
    void Release();

    int AssembleGraph();
	void DisassembleGraph();
	void EmptyGraph(IFilterGraph* graph);

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

#endif // _vidDirectShowSource_h

