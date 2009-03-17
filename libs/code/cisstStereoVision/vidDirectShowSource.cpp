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

#include "vidDirectShowSource.h"

using namespace std;

static int OleInitCounter = 0;

#define DS_INIT_TIMEOUT_INTV                500
#define INITIAL_TOLERANCE_WAIT_LENGTH       100     // [frames]


/*************************************/
/*** CDirectShowSource class *********/
/*************************************/

CDirectShowSource::CDirectShowSource() :
    CVideoCaptureSourceBase(),
    Initialized(false),
    NumOfStreams(0),
    RunState(false),
    pGraph(0),
    pGraphBuilder(0),
    pTestGraph(0),
    pTestBuilder(0),
    pTestCapFilt(0),
	DeviceID(0),
	InputID(0),
	CapWidth(0),
	CapHeight(0),
	EnableRenderer(0),
    pCaptureFilter(0),
    pIntermediatePreviewFilter(0),
    pAviDecomprFilter(0),
    pColorConvFilter(0),
    pSampleGrabFilter(0),
	pCaptureFilterOut(0),
	pSampleGrabber(0),
	pCallBack(0),
	pMediaType(0),
	MediaTypeLength(0),
	PinCategory(0),
	OutputBuffer(0)
{
    if (OleInitCounter < 1) {
        CoInitialize(NULL);
        OleInitCounter = 1;
    }
}

CDirectShowSource::~CDirectShowSource()
{
    Release();

	if (OleInitCounter > 0) {
        CoUninitialize();
        OleInitCounter = 0;
    }
}

svlVideoCaptureSource::PlatformType CDirectShowSource::GetPlatformType()
{
    return svlVideoCaptureSource::WinDirectShow;
}

int CDirectShowSource::SetStreamCount(unsigned int numofstreams)
{
    Release();

    if (numofstreams < 1) return SVL_FAIL;

    NumOfStreams = numofstreams;
    RunState = false;
    pGraph = 0;
    pGraphBuilder = 0;
    pTestGraph = 0;
    pTestBuilder = 0;
    pTestCapFilt = 0;
	DeviceID       = new int[NumOfStreams];
	InputID        = new int[NumOfStreams];
	CapWidth       = new int[NumOfStreams];
	CapHeight      = new int[NumOfStreams];
	EnableRenderer = new int[NumOfStreams];
    pCaptureFilter             = new IBaseFilter*[NumOfStreams];
    pIntermediatePreviewFilter = new IBaseFilter*[NumOfStreams];
    pAviDecomprFilter          = new IBaseFilter*[NumOfStreams];
    pColorConvFilter           = new IBaseFilter*[NumOfStreams];
    pSampleGrabFilter          = new IBaseFilter*[NumOfStreams];
	pCaptureFilterOut = new IPin*[NumOfStreams];
	pSampleGrabber    = new ISampleGrabber*[NumOfStreams];
	pCallBack         = new CDirectShowSourceCB*[NumOfStreams];
	pMediaType        = new unsigned char*[NumOfStreams];
	MediaTypeLength   = new unsigned int[NumOfStreams];
	PinCategory       = new GUID[NumOfStreams];
	OutputBuffer      = new svlImageBuffer*[NumOfStreams];
    for (unsigned int i = 0; i < NumOfStreams; i ++) {
        DeviceID[i] = -1;
        InputID[i] = -1;
        CapWidth[i] = -1;
        CapHeight[i] = -1;
        EnableRenderer[i] = 0;
        pCaptureFilter[i] = 0;
        pIntermediatePreviewFilter[i] = 0;
        pAviDecomprFilter[i] = 0;
        pColorConvFilter[i] = 0;
        pSampleGrabFilter[i] = 0;
        pCaptureFilterOut[i] = 0;
        pSampleGrabber[i] = 0;
        pCallBack[i] = 0;
        pMediaType[i] = 0;
        MediaTypeLength[i] = 0;
        OutputBuffer[i] = 0;
    }

    Initialized = true;
    return SVL_OK;
}

void CDirectShowSource::Release()
{
    if (!Initialized) return;

	Close();

    unsigned int i;

    NumOfStreams = 0;
    RunState = false;
    pGraph = 0;
    pGraphBuilder = 0;
    pTestGraph = 0;
    pTestBuilder = 0;
    pTestCapFilt = 0;

	if (DeviceID) delete [] DeviceID;
	if (InputID) delete [] InputID;
	if (CapWidth) delete [] CapWidth;
	if (CapHeight) delete [] CapHeight;
	if (EnableRenderer) delete [] EnableRenderer;
    if (pCaptureFilter) delete [] pCaptureFilter;
    if (pIntermediatePreviewFilter) delete [] pIntermediatePreviewFilter;
    if (pAviDecomprFilter) delete [] pAviDecomprFilter;
    if (pColorConvFilter) delete [] pColorConvFilter;
    if (pSampleGrabFilter) delete [] pSampleGrabFilter;
	if (pCaptureFilterOut) delete [] pCaptureFilterOut;
	if (pSampleGrabber) delete [] pSampleGrabber;
	if (pCallBack) delete [] pCallBack;
    for (i = 0; i < NumOfStreams; i ++) SetMediaType(0, i);
	if (pMediaType) delete [] pMediaType;
	if (MediaTypeLength) delete [] MediaTypeLength;
	if (PinCategory) delete [] PinCategory;
	if (OutputBuffer) delete [] OutputBuffer;

	DeviceID = 0;
	InputID = 0;
	CapWidth = 0;
	CapHeight = 0;
	EnableRenderer = 0;
    pCaptureFilter = 0;
    pIntermediatePreviewFilter = 0;
    pAviDecomprFilter = 0;
    pColorConvFilter = 0;
    pSampleGrabFilter = 0;
	pCaptureFilterOut = 0;
	pSampleGrabber = 0;
	pCallBack = 0;
	pMediaType = 0;
	MediaTypeLength = 0;
	PinCategory = 0;
	OutputBuffer = 0;

    Initialized = false;
}

int CDirectShowSource::GetDeviceList(svlVideoCaptureSource::DeviceInfo **deviceinfo)
{
    if (deviceinfo == 0) return SVL_FAIL;

    char devnames[SVL_VCS_ARRAY_LENGTH][SVL_VCS_STRING_LENGTH];
    ICreateDevEnum *devenum;
    IEnumMoniker *enumcat;
    IMoniker *moniker;
    ULONG fetched;
    IPropertyBag *propbag;
    VARIANT variant;
    variant.vt = VT_BSTR;
    int i, counter = 0;

    for (i = 0; i < SVL_VCS_ARRAY_LENGTH; i ++) devnames[i][0] = 0;

    // Enumerate device names
    if (CoCreateInstance(CLSID_SystemDeviceEnum, 0, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, reinterpret_cast<void**>(&devenum)) == S_OK) {
        if (devenum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &enumcat, 0) == S_OK) {

            counter = 0;
            while (counter < SVL_VCS_ARRAY_LENGTH &&
                   enumcat->Next(1, &moniker, &fetched) == S_OK) {
                if (moniker->BindToStorage(0, 0, IID_IPropertyBag, reinterpret_cast<void**>(&propbag)) == S_OK) {
                    if (propbag->Read(L"FriendlyName", &variant, 0) == S_OK) {
                        WideCharToMultiByte(CP_ACP,
                                            0,
                                            variant.bstrVal,
                                            -1,
                                            devnames[counter],
                                            SVL_VCS_STRING_LENGTH,
                                            0,
                                            0);
                        SysFreeString(variant.bstrVal);
                    }
                    propbag->Release();
                }
                moniker->Release();
                counter ++;
            }

            enumcat->Release();
        }
        devenum->Release();
    }

    // Allocate memory for device info array
    // CALLER HAS TO FREE UP THIS ARRAY!!!
    if (counter > 0) {
        deviceinfo[0] = new svlVideoCaptureSource::DeviceInfo[counter];
        memset(deviceinfo[0], 0, counter * sizeof(svlVideoCaptureSource::DeviceInfo));

        // Get capture device inputs
        for (i = 0; i < counter; i ++) {
            memcpy(deviceinfo[0][i].name, devnames[i], min(static_cast<int>(strlen(devnames[i])), static_cast<int>(SVL_VCS_STRING_LENGTH) - 1));
            if (TestOpen(i) == SVL_OK) {
                deviceinfo[0][i].id = i;
                deviceinfo[0][i].testok = true;
                deviceinfo[0][i].platform = svlVideoCaptureSource::WinDirectShow;
                GetDeviceInputs(pTestCapFilt, deviceinfo[0] + i);
                TestClose();
            }
        }
    }
    else {
        deviceinfo[0] = 0;
    }

    return counter;
}

int CDirectShowSource::TestOpen(int devid)
{
    TestClose();

    HRESULT hr;
    GUID pincategory;

    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, 0, CLSCTX_INPROC, IID_ICaptureGraphBuilder2, reinterpret_cast<void**>(&pTestBuilder));
    if (hr != S_OK) goto labError;
    hr = CoCreateInstance(CLSID_FilterGraph, 0, CLSCTX_INPROC, IID_IGraphBuilder, reinterpret_cast<void**>(&pTestGraph));
    if (hr != S_OK) goto labError;
    hr = pTestBuilder->SetFiltergraph(pTestGraph);
    if (hr != S_OK) goto labError;

    pincategory = PIN_CATEGORY_CAPTURE;

    pTestCapFilt = GetCaptureFilter(devid);
    if (pTestCapFilt == 0) goto labError;

    hr = pTestGraph->AddFilter(pTestCapFilt, 0);
    if (hr != S_OK) goto labError;

    hr = pTestBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pTestCapFilt, 0, 0);
    if (hr != S_OK) goto labError;

    return SVL_OK;

labError:
    TestClose();
    return SVL_FAIL;
}

void CDirectShowSource::TestClose()
{
    if (pTestBuilder != 0) {
        pTestBuilder->Release();
        pTestBuilder = 0;
    }
    if (pTestGraph != 0) {
        EmptyGraph(pTestGraph);
        pTestGraph->Release();
        pTestGraph = 0;
    }
    if (pTestCapFilt != 0) {
        pTestCapFilt->Release();
        pTestCapFilt = 0;
    }
}

int CDirectShowSource::Open()
{
    // Return if already successfully initialized
    if (pGraph) return SVL_OK;

    if (!Initialized) return SVL_FAIL;

    Close();

    unsigned int i;
    HRESULT hr;

    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, 0, CLSCTX_INPROC, IID_ICaptureGraphBuilder2, reinterpret_cast<void**>(&pGraphBuilder));
    if (hr != S_OK) goto labError;
    hr = CoCreateInstance(CLSID_FilterGraph, 0, CLSCTX_INPROC, IID_IGraphBuilder, reinterpret_cast<void**>(&pGraph));
    if (hr != S_OK) goto labError;
    hr = pGraphBuilder->SetFiltergraph(pGraph);
    if (hr != S_OK) goto labError;

    for (i = 0; i < NumOfStreams; i ++) {

        pCaptureFilter[i] = GetCaptureFilter(DeviceID[i]);
        if (pCaptureFilter[i] == 0) goto labError;

        hr = pGraph->AddFilter(pCaptureFilter[i], 0);
        if (hr != S_OK) goto labError;

        PinCategory[i] = PIN_CATEGORY_CAPTURE;
        pCaptureFilterOut[i] = EnumeratePin(pCaptureFilter[i], 0, &(PinCategory[i]));
        if (pCaptureFilterOut[i] == 0) {
            PinCategory[i] = PIN_CATEGORY_PREVIEW;
            pCaptureFilterOut[i] = EnumeratePin(pCaptureFilter[i], 0, &(PinCategory[i]));
            if (pCaptureFilterOut[i] == 0) goto labError;
        }

        if (pMediaType[i] != 0) SetupMediaType(i);
    }

    if (AssembleGraph() != SVL_OK) goto labError;

    for (i = 0; i < NumOfStreams; i ++) {

        if (InputID[i] >= 0) {
            // Trying to select the requested input
            // Return value:
            //   < 0  -> crossbar device is OK but input is unavailable
            //     0  -> no crossbar device is available
            //   > 0  -> crossbar device is OK and input is selected properly
            // Ignore errors, open the input that is available.
            SetDeviceInput(pCaptureFilter[i], InputID[i]);
        }
    }

    return SVL_OK;

labError:
    Close();
    return SVL_FAIL;
}

int CDirectShowSource::AssembleGraph()
{
    if (!Initialized) return SVL_FAIL;

    HRESULT hr;
	AM_MEDIA_TYPE mediatype;
	VIDEOINFOHEADER *videoinfo;

    for (unsigned int i = 0; i < NumOfStreams; i ++) {

        hr = CoCreateInstance(CLSID_AVIDec, 0, CLSCTX_INPROC, IID_IBaseFilter, reinterpret_cast<void**>(&(pAviDecomprFilter[i])));
        if (hr != S_OK) goto labError;

        hr = CoCreateInstance(CLSID_Colour, 0, CLSCTX_INPROC, IID_IBaseFilter, reinterpret_cast<void**>(&(pColorConvFilter[i])));
        if (hr != S_OK) goto labError;

        hr = CoCreateInstance(CLSID_SampleGrabber, 0, CLSCTX_INPROC, IID_IBaseFilter, reinterpret_cast<void**>(&(pSampleGrabFilter[i])));
        if (hr != S_OK) goto labError;
        hr = pSampleGrabFilter[i]->QueryInterface(IID_ISampleGrabber, reinterpret_cast<void**>(&(pSampleGrabber[i])));
        if (hr != S_OK) goto labError;
        hr = pSampleGrabber[i]->SetOneShot(FALSE);
        if (hr != S_OK) goto labError;
        hr = pSampleGrabber[i]->SetBufferSamples(TRUE);
        if (hr != S_OK) goto labError;
        memset(&mediatype, 0, sizeof(mediatype));
        mediatype.majortype = MEDIATYPE_Video;
        mediatype.subtype = MEDIASUBTYPE_RGB24;
        hr = pSampleGrabber[i]->SetMediaType(&mediatype);
        if (hr != S_OK) goto labError;

        hr = pGraph->AddFilter(pAviDecomprFilter[i], 0);
        if (hr != S_OK) goto labError;
        hr = pGraph->AddFilter(pColorConvFilter[i], 0);
        if (hr != S_OK) goto labError;
        hr = pGraph->AddFilter(pSampleGrabFilter[i], 0);
        if (hr != S_OK) goto labError;

        hr = pGraphBuilder->RenderStream(&(PinCategory[i]), &MEDIATYPE_Video, pCaptureFilter[i], 0, pAviDecomprFilter[i]);
        if (hr != S_OK) {
        // Cannot directly connect Capture Filter to Avi Decompressor Filter
        // so start searching with brute force for an appropriate intermediate filter

            IFilterMapper2 *mapper;
            GUID typesarray[2];
            IEnumMoniker *enumcat;
            IMoniker *moniker;
            ULONG fetched;

            // Searching for matching filters
            hr = CoCreateInstance(CLSID_FilterMapper2, 0, CLSCTX_INPROC, IID_IFilterMapper2, reinterpret_cast<void**>(&mapper));
            if (hr != S_OK) goto labError;

            typesarray[0] = MEDIATYPE_Video;
            typesarray[1] = GUID_NULL;
            hr = mapper->EnumMatchingFilters(&enumcat,
                                             0,                     // Reserved.
                                             TRUE,                  // Use exact match?
                                             MERIT_DO_NOT_USE + 1,  // Minimum merit.
                                             TRUE,                  // At least one input pin?
                                             1,                     // Number of major type/subtype pairs for input.
                                             typesarray,            // Array of major type/subtype pairs for input.
                                             0,                  // Input medium.
                                             0,                  // Input pin category.
                                             FALSE,                 // Must be a renderer?
                                             TRUE,                  // At least one output pin?
                                             0,                     // Number of major type/subtype pairs for output.
                                             0,                  // Array of major type/subtype pairs for output.
                                             0,                  // Output medium.
                                             0);                 // Output pin category.
            if (hr != S_OK) {
                mapper->Release();
                goto labError;
            }

            // Enumerating the monikers
            while (enumcat->Next(1, &moniker, &fetched) == S_OK) {

                // Getting a reference to the enumerated filter
                hr = moniker->BindToObject(0, 0, IID_IBaseFilter, reinterpret_cast<void**>(&(pIntermediatePreviewFilter[i])));
                if (hr == S_OK) {
                    hr = pGraph->AddFilter(pIntermediatePreviewFilter[i], 0);
                    if (hr == S_OK) {
                        hr = pGraphBuilder->RenderStream(&(PinCategory[i]),
                                                         &MEDIATYPE_Video,
                                                         pCaptureFilter[i],
                                                         pIntermediatePreviewFilter[i],
                                                         pAviDecomprFilter[i]);
                        if (hr == S_OK) {
                            // FOUND, so exit from the loop
                            moniker->Release();
                            break;
                        }
                    }
                }

                // Mismatch, so remove and release filter
                if (pIntermediatePreviewFilter[i] != 0) {
                    pGraph->RemoveFilter(pIntermediatePreviewFilter[i]);
                    pIntermediatePreviewFilter[i]->Release();
                    pIntermediatePreviewFilter[i] = 0;
                }

                moniker->Release();
            }

            enumcat->Release();
            mapper->Release();

            // No intermediate filter found
            if (pIntermediatePreviewFilter[i] == 0) goto labError;
        }

        hr = pGraphBuilder->RenderStream(0, 0, pAviDecomprFilter[i], pColorConvFilter[i], pSampleGrabFilter[i]);
        if (hr != S_OK) goto labError;

        hr = pCaptureFilterOut[i]->ConnectionMediaType(&mediatype);
        if (hr != S_OK) goto labError;

        videoinfo = reinterpret_cast<VIDEOINFOHEADER*>(mediatype.pbFormat);
        CapWidth[i] = videoinfo->bmiHeader.biWidth;
        CapHeight[i] = videoinfo->bmiHeader.biHeight;
        CoTaskMemFree(mediatype.pbFormat);

        OutputBuffer[i] = new svlImageBuffer(CapWidth[i], CapHeight[i]);
        pCallBack[i] = new CDirectShowSourceCB(OutputBuffer[i]);
        hr = pSampleGrabber[i]->SetCallback(pCallBack[i], 1);
        if (hr != S_OK) goto labError;

        if (EnableRenderer[i] != 0) {
            hr = pGraphBuilder->RenderStream(0, 0, pSampleGrabFilter[i], 0, 0);
            if (hr != S_OK) goto labError;
        }
    }

    return SVL_OK;

labError:
    DisassembleGraph();
    return SVL_FAIL;
}

void CDirectShowSource::DisassembleGraph()
{
    if (!Initialized) return;

    Stop();

    for (unsigned int i = 0; i < NumOfStreams; i ++) {

        CapWidth[i] = -1;
        CapHeight[i] = -1;

        if (pGraph != 0) {
            if (pSampleGrabFilter[i] != 0) pGraph->RemoveFilter(pSampleGrabFilter[i]);
            if (pColorConvFilter[i] != 0) pGraph->RemoveFilter(pColorConvFilter[i]);
            if (pAviDecomprFilter[i] != 0) pGraph->RemoveFilter(pAviDecomprFilter[i]);
            if (pIntermediatePreviewFilter[i] != 0) pGraph->RemoveFilter(pIntermediatePreviewFilter[i]);
            if (pCaptureFilterOut[i] != 0) pCaptureFilterOut[i]->Disconnect();
        }
        if (pSampleGrabber[i] != 0) {
            pSampleGrabber[i]->SetCallback(0, 1);
            pSampleGrabber[i]->Release();
            pSampleGrabber[i] = 0;
        }
        if (pCallBack[i] != 0) {
            delete pCallBack[i];
            pCallBack[i] = 0;
        }
        if (OutputBuffer[i] != 0) {
            delete OutputBuffer[i];
            OutputBuffer[i] = 0;
        }
        if (pSampleGrabFilter[i] != 0) {
            pSampleGrabFilter[i]->Release();
            pSampleGrabFilter[i] = 0;
        }
        if (pColorConvFilter[i] != 0) {
            pColorConvFilter[i]->Release();
            pColorConvFilter[i] = 0;
        }
        if (pAviDecomprFilter[i] != 0) {
            pAviDecomprFilter[i]->Release();
            pAviDecomprFilter[i] = 0;
        }
        if (pIntermediatePreviewFilter[i] != 0) {
            pIntermediatePreviewFilter[i]->Release();
            pIntermediatePreviewFilter[i] = 0;
        }
    }
}

void CDirectShowSource::EmptyGraph(IFilterGraph* graph)
{
    if (!Initialized) return;

    if (RunState == false && pGraph != 0) {
        bool morefilters = true;
        IEnumFilters  *enumfilt;
        IBaseFilter *filtref;
        ULONG fetched;

        while (morefilters) {
            if (graph->EnumFilters(&enumfilt) == S_OK) {
                if (enumfilt->Next(1, &filtref, &fetched) == S_OK) {
                    graph->RemoveFilter(filtref);
                    filtref->Release();
                }
                else {
                    morefilters = false;
                }
                enumfilt->Release();
            }
        }
    }
}

void CDirectShowSource::Close() 
{
    if (!Initialized) return;

    DisassembleGraph();

	unsigned int i;

    if (pGraphBuilder != 0) {
        pGraphBuilder->Release();
        pGraphBuilder = 0;
    }
    if (pGraph != 0) {
        EmptyGraph(pGraph);
        pGraph->Release();
        pGraph = 0;
    }
	if (pCaptureFilterOut) {
		for (i = 0; i < NumOfStreams; i ++) {
			if (pCaptureFilterOut[i] != 0) {
				pCaptureFilterOut[i]->Release();
				pCaptureFilterOut[i] = 0;
			}
		}
	}
	if (pCaptureFilter) {
		for (i = 0; i < NumOfStreams; i ++) {
			if (pCaptureFilter[i] != 0) {
				pCaptureFilter[i]->Release();
				pCaptureFilter[i] = 0;
			}
		}
	}
}

IBaseFilter* CDirectShowSource::GetCaptureFilter(int devid)
{
    HRESULT hr;
    IBaseFilter* filtref;
    ICreateDevEnum *devenum;
    IEnumMoniker *enumcat;
    IMoniker *moniker;
    ULONG fetched;
    int counter;

    hr = CoCreateInstance(CLSID_SystemDeviceEnum, 0, CLSCTX_INPROC, IID_ICreateDevEnum, reinterpret_cast<void**>(&devenum));
    if (hr != S_OK) return 0;
    hr = devenum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &enumcat, 0);
    if (hr != S_OK) {
        devenum->Release();
        return 0;
    }

    counter = 0;
    while (enumcat->Next(1, &moniker, &fetched) == S_OK) {
        if (moniker->BindToObject(0, 0, IID_IBaseFilter, reinterpret_cast<void**>(&filtref)) == S_OK) {
            if (counter == devid) {
                moniker->Release();
                break;
            }
            counter ++;
        }
        moniker->Release();
        filtref->Release();
        filtref = 0;
    }
    enumcat->Release();
    devenum->Release();

    return filtref;
}

IPin* CDirectShowSource::EnumeratePin(IBaseFilter* filter, LPCTSTR name, GUID *category)
{
    if (filter == 0) return 0;

    char mbstr[256];
    IEnumPins *enumpin;
    ULONG fetched;
    IPin *pinref = 0;
    PIN_INFO pininfo;
    GUID tcat;

    if (filter->EnumPins(&enumpin) == S_OK) {
        while (enumpin->Next(1, &pinref, &fetched) == S_OK) {

            if (category == 0) {
            // Searching by name
                if (pinref->QueryPinInfo(&pininfo) == S_OK) {
                    if (pininfo.pFilter != 0) pininfo.pFilter->Release();

                    memset(mbstr, 0, 256);
                    WideCharToMultiByte(CP_ACP, 0, pininfo.achName, -1, mbstr, 256, 0, 0);
                    if (strcmp(mbstr, name) == 0) break;

                    pinref->Release();
                    pinref = 0;
                }
            }
            else {
            // Searching by category
                if (GetPinCategory(pinref, &tcat) == SVL_OK && tcat == category[0]) break;

                pinref->Release();
                pinref = 0;
            }
        }
        enumpin->Release();
    }

    return pinref;
}

int CDirectShowSource::GetPinCategory(IPin *pin, GUID *category)
{
    if (pin == 0 ||
        category == 0) return SVL_FAIL;

    HRESULT hr;
    DWORD dwret;
    IKsPropertySet *pks;

    hr = pin->QueryInterface(IID_IKsPropertySet, reinterpret_cast<void**>(&pks));
    if (hr != S_OK) return SVL_FAIL;

    hr = pks->Get(AMPROPSETID_Pin, AMPROPERTY_PIN_CATEGORY, 0, 0, category, sizeof(GUID), &dwret);

    pks->Release();

    if (hr != S_OK) return SVL_FAIL;
    return SVL_OK;
}

int CDirectShowSource::SetDeviceInput(IBaseFilter *capfilter, int input_id)
{
    if (capfilter == 0) return SVL_FAIL;

    HRESULT hr;
    CDirectShowInputSelector *crossbar;
    GUID pincategory;
    IPin *pin;

    pincategory = PIN_CATEGORY_ANALOGVIDEOIN;
    pin = EnumeratePin(capfilter, 0, &pincategory);
    if (pin != 0) {
        crossbar = new CDirectShowInputSelector(pin);
        hr = crossbar->SetInputIndex((LONG)input_id);

        delete crossbar;
        pin->Release();

        if (hr == S_OK) return SVL_OK;
    }

    return SVL_FAIL;
}

int CDirectShowSource::GetDeviceInputs(IBaseFilter *capfilter, svlVideoCaptureSource::DeviceInfo *deviceinfo)
{
    if (capfilter == 0 ||
        deviceinfo == 0) return SVL_FAIL;

    IPin *pin;
    CDirectShowInputSelector *crossbar;
    GUID pincategory;
    LONG count;
    std::string name;
    int i, length;

    pincategory = PIN_CATEGORY_ANALOGVIDEOIN;
    pin = EnumeratePin(capfilter, 0, &pincategory);
    if (pin != 0) {
        crossbar = new CDirectShowInputSelector(pin);

        count = crossbar->GetInputCount();
        if (count > SVL_VCS_ARRAY_LENGTH) count = SVL_VCS_ARRAY_LENGTH;
        deviceinfo->inputcount = count;

        if (count > 0) {
        // Crossbar device found
            for (i = 0; i < count; i ++) {
                crossbar->GetInputName(i, name);
                length = std::min((int)SVL_VCS_STRING_LENGTH - 1, (int)name.length());
                memcpy(deviceinfo->inputnames[i], name.c_str(), length);
                deviceinfo->inputnames[i][length] = 0;
            }

            count = crossbar->GetInputIndex();
            deviceinfo->activeinput = count;
        }
        else {
        // No appropriate crossbar
            deviceinfo->inputcount = 0;
            deviceinfo->activeinput = -1;
        }

        delete crossbar;
        pin->Release();
    }
    else {
        deviceinfo->inputcount = 0;
        deviceinfo->activeinput = -1;
    }

    return SVL_OK;
}

int CDirectShowSource::Start() 
{
    // Return if already running
    if (RunState) return SVL_OK;

    if (!Initialized ||
        pGraph == 0) return SVL_FAIL;

    HRESULT hr;
    IMediaControl *mediactrl;

    hr = pGraph->QueryInterface(IID_IMediaControl, reinterpret_cast<void**>(&mediactrl));
    if (hr != S_OK) return SVL_FAIL;

    hr = mediactrl->Run();
    if (hr == S_FALSE) {
        OAFilterState pfs;
        do {
            hr = mediactrl->GetState(DS_INIT_TIMEOUT_INTV, &pfs);
        } while (hr == VFW_S_STATE_INTERMEDIATE);
    }
    mediactrl->Release();

    if (hr != S_OK)	return SVL_FAIL;

    RunState = true;
    return SVL_OK;
}

int CDirectShowSource::Stop() 
{
    if (!Initialized ||
        pGraph == 0) return SVL_FAIL;

    RunState = false;

    HRESULT hr;
    IMediaControl *mediactrl;

    hr = pGraph->QueryInterface(IID_IMediaControl, reinterpret_cast<void**>(&mediactrl));
    if (hr == S_OK) {
        hr = mediactrl->Stop();
        mediactrl->Release();
    }

    if (hr != S_OK)	return SVL_FAIL;
    return SVL_OK;
}

svlImageRGB* CDirectShowSource::GetLatestFrame(bool waitfornew, unsigned int videoch)
{
    if (!Initialized || videoch >= NumOfStreams) return 0;
    return OutputBuffer[videoch]->Pull(waitfornew);
}

int CDirectShowSource::ShowFormatDialog(HWND hwnd, unsigned int videoch)
{
    if (!Initialized ||
        videoch >= NumOfStreams ||
        pCaptureFilterOut[videoch] == 0) return SVL_FAIL;

    bool run = RunState;

    DisassembleGraph();

    CAUUID caguid;
    ISpecifyPropertyPages *properties;

    if (pCaptureFilterOut[videoch]->QueryInterface(IID_ISpecifyPropertyPages, reinterpret_cast<void**>(&properties)) != S_OK) return SVL_FAIL;
    properties->GetPages(&caguid);
    properties->Release();

    OleCreatePropertyFrame(hwnd,
                           0,
                           0,
                           L"Capture Format",
                           1,
                           reinterpret_cast<IUnknown**>(&(pCaptureFilterOut[videoch])),
                           caguid.cElems,
                           caguid.pElems,
                           0,
                           0,
                           0);
    CoTaskMemFree(reinterpret_cast<void*>(caguid.pElems));

    if (AssembleGraph() != SVL_OK ||
        (run == true && Start() != SVL_OK)) return SVL_FAIL;
    return SVL_OK;
}

int CDirectShowSource::ShowImageDialog(HWND hwnd, unsigned int videoch)
{
    if (!Initialized ||
        videoch >= NumOfStreams ||
        pCaptureFilter[videoch] == 0) return SVL_FAIL;

    CAUUID caguid;
    ISpecifyPropertyPages *properties;

    if (pCaptureFilter[videoch]->QueryInterface(IID_ISpecifyPropertyPages, reinterpret_cast<void**>(&properties)) != S_OK) return SVL_FAIL;
    properties->GetPages(&caguid);
    properties->Release();

    OleCreatePropertyFrame(hwnd,
                           0,
                           0,
                           L"Image Properties",
                           1,
                           reinterpret_cast<IUnknown**>(&(pCaptureFilter[videoch])),
                           caguid.cElems,
                           caguid.pElems,
                           0,
                           0,
                           0);
    CoTaskMemFree(reinterpret_cast<void*>(caguid.pElems));

    return SVL_OK;
}

int CDirectShowSource::GetMediaType(unsigned char *&mediabuffer, unsigned int &length, unsigned int videoch)
{
    if (!Initialized ||
        videoch >= NumOfStreams) return SVL_FAIL;

    if (pMediaType[videoch] == 0 || MediaTypeLength[videoch] == 0) {
        mediabuffer = 0;
        length = 0;
    }
    else {
        length = MediaTypeLength[videoch];
        mediabuffer = new unsigned char[length];
        memcpy(mediabuffer, pMediaType[videoch], length);
    }

    return SVL_OK;
}

int CDirectShowSource::SetMediaType(unsigned char *mediabuffer, unsigned int length, unsigned int videoch)
{
    if (!Initialized ||
        videoch >= NumOfStreams) return SVL_FAIL;

    if (pMediaType[videoch]) {
        delete [] pMediaType[videoch];
        pMediaType[videoch] = 0;
        MediaTypeLength[videoch] = 0;
    }

    if (mediabuffer != 0 && length > 0) {
        pMediaType[videoch] = new unsigned char[length];
        MediaTypeLength[videoch] = length;
        memcpy(pMediaType[videoch], mediabuffer, length);
    }

    return SVL_OK;
}

int CDirectShowSource::SetupMediaType(unsigned int videoch)
{
    if (!Initialized ||
        videoch >= NumOfStreams ||
        pCaptureFilterOut[videoch] == 0 ||
        pMediaType[videoch] == 0 ||
        MediaTypeLength[videoch] < sizeof(AM_MEDIA_TYPE)) return SVL_FAIL;

    HRESULT hr;
    int therest;
    AM_MEDIA_TYPE *mediatype;
    VIDEOINFOHEADER *videoinfo;
    IAMStreamConfig *streamconf;
    
    mediatype = new AM_MEDIA_TYPE;
    memcpy(mediatype, pMediaType[videoch], sizeof(AM_MEDIA_TYPE));
    mediatype->pbFormat = 0;

    therest = MediaTypeLength[videoch] - sizeof(AM_MEDIA_TYPE);
    if (therest >= sizeof(VIDEOINFOHEADER)) {
        videoinfo = new VIDEOINFOHEADER;
        memcpy(videoinfo, (pMediaType[videoch] + sizeof(AM_MEDIA_TYPE)), sizeof(VIDEOINFOHEADER));
        mediatype->pbFormat = reinterpret_cast<unsigned char*>(videoinfo);
    }

    hr = pGraphBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
                                      &MEDIATYPE_Video,
                                      pCaptureFilter[videoch],
                                      IID_IAMStreamConfig,
                                      reinterpret_cast<void**>(&streamconf));
    if (hr == S_OK) {
        hr = streamconf->SetFormat(mediatype);

        streamconf->Release();
        if (mediatype->pbFormat != 0) delete mediatype->pbFormat;
        delete mediatype;

        if (hr == S_OK) return SVL_OK;
    }

    return SVL_FAIL;
}

int CDirectShowSource::RequestMediaType(unsigned int videoch)
{
    if (!Initialized ||
        videoch >= NumOfStreams ||
        pCaptureFilterOut[videoch] == 0) return SVL_FAIL;

    HRESULT hr;
    AM_MEDIA_TYPE mediatype;

    memset(&mediatype, 0, sizeof(AM_MEDIA_TYPE));

    hr = pCaptureFilterOut[videoch]->ConnectionMediaType(&mediatype);
    if (hr != S_OK) return SVL_FAIL;

    if (pMediaType[videoch]) {
        delete [] pMediaType[videoch];
        pMediaType[videoch] = 0;
        MediaTypeLength[videoch] = 0;
    }

    MediaTypeLength[videoch] = sizeof(AM_MEDIA_TYPE);
    if (mediatype.pbFormat != 0) {
        MediaTypeLength[videoch] += sizeof(VIDEOINFOHEADER);
    }

    pMediaType[videoch] = new unsigned char[MediaTypeLength[videoch]];
    memcpy(pMediaType[videoch], &mediatype, sizeof(AM_MEDIA_TYPE));

    if (mediatype.pbFormat != 0) {
        memcpy(pMediaType[videoch] + sizeof(AM_MEDIA_TYPE), mediatype.pbFormat, sizeof(VIDEOINFOHEADER));
    }

    return SVL_OK;
}

int CDirectShowSource::SetDevice(int devid, int inid, unsigned int videoch)
{
    if (!Initialized ||
        videoch >= NumOfStreams ||
        pCaptureFilterOut[videoch]) return SVL_FAIL;

    DeviceID[videoch] = devid;
    InputID[videoch] = inid;

    if (pMediaType[videoch]) {
        delete [] pMediaType[videoch];
        pMediaType[videoch] = 0;
        MediaTypeLength[videoch] = 0;
    }

    return SVL_OK;
}

int CDirectShowSource::SetRendererOnOff(bool render, unsigned int videoch)
{
    if (!Initialized ||
        videoch >= NumOfStreams) return SVL_FAIL;

    if (render) EnableRenderer[videoch] = 1;
    else EnableRenderer[videoch] = 0;

    return SVL_OK;
}

int CDirectShowSource::GetWidth(unsigned int videoch)
{
    if (!Initialized ||
        videoch >= NumOfStreams) return SVL_FAIL;

    return CapWidth[videoch];
}

int CDirectShowSource::GetHeight(unsigned int videoch)
{
    if (!Initialized ||
        videoch >= NumOfStreams) return SVL_FAIL;

    return CapHeight[videoch];
}


/*************************************/
/*** CDirectShowSourceCB class *******/
/*************************************/

CDirectShowSourceCB::CDirectShowSourceCB(svlImageBuffer *buffer)
{
    Buffer = buffer;
}

STDMETHODIMP CDirectShowSourceCB::SampleCB(double SampleTime, IMediaSample *pSample)
{
    return E_NOTIMPL;
}

STDMETHODIMP CDirectShowSourceCB::BufferCB(double sampletime, unsigned char *buffer, long buffersize)
{
    if (Buffer) Buffer->Push(buffer, buffersize, true);
    return S_OK;
}

