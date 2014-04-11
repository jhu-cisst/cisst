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

#include "svlVidCapSrcDirectShow.h"


/*************************************/
/*** svlVidCapSrcDirectShow class ****/
/*************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlVidCapSrcDirectShow, svlVidCapSrcBase)

svlVidCapSrcDirectShow::svlVidCapSrcDirectShow() :
    svlVidCapSrcBase(),
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
    CapTopDown(0),
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
}

svlVidCapSrcDirectShow::~svlVidCapSrcDirectShow()
{
    Release();
}

svlFilterSourceVideoCapture::PlatformType svlVidCapSrcDirectShow::GetPlatformType()
{
    return svlFilterSourceVideoCaptureTypes::WinDirectShow;
}

int svlVidCapSrcDirectShow::SetStreamCount(unsigned int numofstreams)
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
	CapTopDown     = new bool[NumOfStreams];
	EnableRenderer = new int[NumOfStreams];
    pCaptureFilter             = new IBaseFilter*[NumOfStreams];
    pIntermediatePreviewFilter = new IBaseFilter*[NumOfStreams];
    pAviDecomprFilter          = new IBaseFilter*[NumOfStreams];
    pColorConvFilter           = new IBaseFilter*[NumOfStreams];
    pSampleGrabFilter          = new IBaseFilter*[NumOfStreams];
	pCaptureFilterOut = new IPin*[NumOfStreams];
	pSampleGrabber    = new ISampleGrabber*[NumOfStreams];
	pCallBack         = new svlVidCapSrcDirectShowCB*[NumOfStreams];
	pMediaType        = new unsigned char*[NumOfStreams];
	MediaTypeLength   = new unsigned int[NumOfStreams];
	PinCategory       = new GUID[NumOfStreams];
	OutputBuffer      = new svlBufferImage*[NumOfStreams];
    for (unsigned int i = 0; i < NumOfStreams; i ++) {
        DeviceID[i] = -1;
        InputID[i] = -1;
        CapWidth[i] = -1;
        CapHeight[i] = -1;
        CapTopDown[i] = true;
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

void svlVidCapSrcDirectShow::Release()
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
	if (CapTopDown) delete [] CapTopDown;
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
	CapTopDown = 0;
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

int svlVidCapSrcDirectShow::GetDeviceList(svlFilterSourceVideoCapture::DeviceInfo **deviceinfo)
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
        deviceinfo[0] = new svlFilterSourceVideoCapture::DeviceInfo[counter];
        memset(deviceinfo[0], 0, counter * sizeof(svlFilterSourceVideoCapture::DeviceInfo));

        // Get capture device inputs
        for (i = 0; i < counter; i ++) {
            memcpy(deviceinfo[0][i].name, devnames[i], std::min(static_cast<int>(strlen(devnames[i])), static_cast<int>(SVL_VCS_STRING_LENGTH) - 1));
            if (TestOpen(i) == SVL_OK) {
                deviceinfo[0][i].ID = i;
                deviceinfo[0][i].testok = true;
                deviceinfo[0][i].platform = svlFilterSourceVideoCaptureTypes::WinDirectShow;
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

int svlVidCapSrcDirectShow::TestOpen(int devid)
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

void svlVidCapSrcDirectShow::TestClose()
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

int svlVidCapSrcDirectShow::Open()
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

        CMN_LOG_CLASS_INIT_VERBOSE << "Open called for stream " << i << std::endl;
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

        if (pMediaType && (pMediaType[i] != 0)) SetupMediaType(i);
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

int svlVidCapSrcDirectShow::AssembleGraph()
{
    if (!Initialized) return SVL_FAIL;

    HRESULT hr;
	AM_MEDIA_TYPE mediatype;
	VIDEOINFOHEADER *videoinfo;

    for (unsigned int i = 0; i < NumOfStreams; i ++) {

        FILTER_INFO fInfo;
        char fName[MAX_FILTER_NAME];
		pCaptureFilter[i]->QueryFilterInfo(&fInfo);
        WideCharToMultiByte(CP_ACP, 0, fInfo.achName, -1, fName, MAX_FILTER_NAME, 0, 0);
        CMN_LOG_CLASS_INIT_VERBOSE << "AssembleGraph for stream " << i << ": capture device = " << fName << std::endl;

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

        // Try to directly connect source filter to color space converter, then to sample grabber
        hr = pGraphBuilder->RenderStream(&(PinCategory[i]), &MEDIATYPE_Video, pCaptureFilter[i], pColorConvFilter[i], pSampleGrabFilter[i]);
        if (hr == S_OK || hr == VFW_S_NOPREVIEWPIN) {
            if (hr == S_OK) {
                CMN_LOG_CLASS_INIT_VERBOSE << "AssembleGraph: Connected to colour space converter, then to sample grabber." << std::endl;
            }
            else {
                CMN_LOG_CLASS_INIT_VERBOSE << "AssembleGraph: Connected (through a smart tee) to colour space converter, then to sample grabber." << std::endl;
            }
        }
        else {
            CMN_LOG_CLASS_INIT_VERBOSE << "AssembleGraph: Cannot directly connect to colour space converter, then to sample grabber." << std::endl;

            // Try connecting source filter to AVI decompressor
            hr = pGraphBuilder->RenderStream(&(PinCategory[i]), &MEDIATYPE_Video, pCaptureFilter[i], 0, pAviDecomprFilter[i]);
            if (hr == S_OK) {
                CMN_LOG_CLASS_INIT_VERBOSE << "AssembleGraph: Connected to AVI decompressor." << std::endl;

                hr = pGraphBuilder->RenderStream(0, 0, pAviDecomprFilter[i], pColorConvFilter[i], pSampleGrabFilter[i]);
                if (hr != S_OK) goto labError;

                CMN_LOG_CLASS_INIT_VERBOSE << "AssembleGraph: AVI decompressor connected to colour space converter, then to sample grabber filter." << std::endl;
            }
            else {
                CMN_LOG_CLASS_INIT_VERBOSE << "AssembleGraph: Cannot connect to AVI decompression filter." << std::endl;

                // Cannot directly connect Capture Filter to Avi Decompressor Filter or Colour conversion filter,
                // so start searching with brute force for an appropriate intermediate filter

                IFilterMapper2 *mapper = 0;
                IEnumMoniker *enumcat = 0;
                IMoniker *moniker = 0;
                GUID typesarray[2];
                ULONG fetched;

                CMN_LOG_CLASS_INIT_VERBOSE << "AssembleGraph: Searching for intermediate filters." << std::endl;

                // Searching for matching filters
                hr = CoCreateInstance(CLSID_FilterMapper2, 0, CLSCTX_INPROC, IID_IFilterMapper2, reinterpret_cast<void**>(&mapper));
                if (hr != S_OK || !mapper) goto labError;

                typesarray[0] = MEDIATYPE_Video;
                typesarray[1] = GUID_NULL;
                hr = mapper->EnumMatchingFilters(&enumcat,
                                                 0,                     // Reserved.
                                                 TRUE,                  // Use exact match?
                                                 MERIT_DO_NOT_USE + 1,  // Minimum merit.
                                                 TRUE,                  // At least one input pin?
                                                 1,                     // Number of major type/subtype pairs for input.
                                                 typesarray,            // Array of major type/subtype pairs for input.
                                                 0,                     // Input medium.
                                                 0,                     // Input pin category.
                                                 FALSE,                 // Must be a renderer?
                                                 TRUE,                  // At least one output pin?
                                                 0,                     // Number of major type/subtype pairs for output.
                                                 0,                     // Array of major type/subtype pairs for output.
                                                 0,                     // Output medium.
                                                 0);                    // Output pin category.
                if (hr != S_OK || !enumcat) {
                    mapper->Release();
                    goto labError;
                }

                // Enumerating the monikers
                while (enumcat->Next(1, &moniker, &fetched) == S_OK) {

                    // Getting a reference to the enumerated filter
                    if (moniker->BindToObject(0, 0, IID_IBaseFilter, reinterpret_cast<void**>(&(pIntermediatePreviewFilter[i]))) == S_OK) {

                        if (pGraph->AddFilter(pIntermediatePreviewFilter[i], 0) == S_OK) {

                            // First try to connect to color space converter
                            if (pGraphBuilder->RenderStream(&(PinCategory[i]), &MEDIATYPE_Video,
                                                            pCaptureFilter[i], pIntermediatePreviewFilter[i], pColorConvFilter[i]) == S_OK) {
                                hr = pGraphBuilder->RenderStream(0, 0,
                                                                 pColorConvFilter[i], 0, pSampleGrabFilter[i]);
                                if (hr != S_OK) {
                                    // Disconnect source and color space renderer from all other filters
                                    pCaptureFilterOut[i]->Disconnect();
                                    pGraph->RemoveFilter(pColorConvFilter[i]);
                                    pGraph->AddFilter(pColorConvFilter[i], 0);
                                }
                            }
                            // Then try to build through AVI decompressor
                            else if (pGraphBuilder->RenderStream(&(PinCategory[i]), &MEDIATYPE_Video,
                                                                 pCaptureFilter[i], pIntermediatePreviewFilter[i], pAviDecomprFilter[i]) == S_OK) {
                                hr = pGraphBuilder->RenderStream(0, 0,
                                                                 pAviDecomprFilter[i], pColorConvFilter[i], pSampleGrabFilter[i]);
                                if (hr != S_OK) {
                                    // Disconnect source and AVI decompressor from all other filters
                                    pCaptureFilterOut[i]->Disconnect();
                                    pGraph->RemoveFilter(pAviDecomprFilter[i]);
                                    pGraph->AddFilter(pAviDecomprFilter[i], 0);
                                }
                            }
                            // All failed
                            else {
                                hr = S_FALSE;
                            }

                            if (hr == S_OK) {

                                // Display the filter name
                                std::stringstream str;
                                IPropertyBag *pPropBag = NULL;
                                hr = moniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
                                if (hr == S_OK) {
                                    VARIANT varName;
                                    VariantInit(&varName);
                                    hr = pPropBag->Read(L"FriendlyName", &varName, 0);
                                    if (hr == S_OK)
                                        str << varName.bstrVal;
                                    VariantClear(&varName);
                                    pPropBag->Release();
                                }
                                if (hr != S_OK)
                                    str << "Unknown";
                                CMN_LOG_CLASS_INIT_VERBOSE << "AssembleGraph: Using intermediate filter: " << str.str() << std::endl;

                                // Intermediate filter found, so exit from loop
                                break;
                            }
                        }

                        // Mismatch, so remove and release filter
                        if (pIntermediatePreviewFilter[i] != 0) {
                            pGraph->RemoveFilter(pIntermediatePreviewFilter[i]);
                            pIntermediatePreviewFilter[i]->Release();
                            pIntermediatePreviewFilter[i] = 0;
                        }
                    }

                    if (moniker) {
                        moniker->Release();
                        moniker = 0;
                    }
                }

                enumcat->Release();
                mapper->Release();

                // No intermediate filter found
                if (pIntermediatePreviewFilter[i] == 0) goto labError;
            }
        }

        // Get image capture dimensions
        if (pCaptureFilterOut[i]->ConnectionMediaType(&mediatype) != S_OK) goto labError;
        videoinfo = reinterpret_cast<VIDEOINFOHEADER*>(mediatype.pbFormat);
        CapWidth[i] = videoinfo->bmiHeader.biWidth;
        if (videoinfo->bmiHeader.biHeight < 0) {
            CapHeight[i] = -(videoinfo->bmiHeader.biHeight);
            CapTopDown[i] = false;
        }
        else {
            CapHeight[i] = videoinfo->bmiHeader.biHeight;
            CapTopDown[i] = true;
        }
        /*CapWidth[i] = 640;
        CapHeight[i] = 480;
        CapTopDown[i] = true;*/
        CoTaskMemFree(mediatype.pbFormat);

        // Setup output buffer and frame callback
        OutputBuffer[i] = new svlBufferImage(CapWidth[i], CapHeight[i]);
        pCallBack[i] = new svlVidCapSrcDirectShowCB(OutputBuffer[i], CapTopDown[i]);
        if (pSampleGrabber[i]->SetCallback(pCallBack[i], 1) != S_OK) goto labError;

        // Add DirectShow video renderer if requested
        if (EnableRenderer[i] != 0 &&
            pGraphBuilder->RenderStream(0, 0, pSampleGrabFilter[i], 0, 0) != S_OK) goto labError;
    }

    return SVL_OK;

labError:
    CMN_LOG_CLASS_INIT_ERROR << "AssembleGraph returning error (SVL_FAIL)." << std::endl;
    DisassembleGraph();
    return SVL_FAIL;
}

void svlVidCapSrcDirectShow::DisassembleGraph()
{
    if (!Initialized) return;

    Stop();

    for (unsigned int i = 0; i < NumOfStreams; i ++) {

        CapWidth[i] = -1;
        CapHeight[i] = -1;
        CapTopDown[i] = true;

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

    // Get rid of all the leftover inivisible junk,
    // except the capture filter
    if (pGraph != 0) EmptyGraph(pGraph, true);
}

void svlVidCapSrcDirectShow::EmptyGraph(IFilterGraph* graph, bool keepsources)
{
    if (!Initialized) return;

    if (RunState == false && pGraph != 0) {
        HRESULT hr;
        unsigned int i;
        bool issource, morefilters = true;
        IEnumFilters  *enumfilt;
        IBaseFilter *filtref, *prevfiltref = 0;
        ULONG fetched;

        while (morefilters) {
            if (graph->EnumFilters(&enumfilt) == S_OK) {
                hr = enumfilt->Next(1, &filtref, &fetched);
                while (hr == S_OK && keepsources) {
                    for (issource = false, i = 0; i < NumOfStreams; i ++ ) {
                        if (filtref == pCaptureFilter[i]) {
                            issource = true;
                            break;
                        }
                    }
                    if (!issource) break;

                    // jump over source filters
                    filtref->Release();
                    hr = enumfilt->Next(1, &filtref, &fetched);
                }
                if (hr == S_OK) {
                    graph->RemoveFilter(filtref);
                    filtref->Release();
                }
                else {
                    morefilters = false;
                }
                enumfilt->Release();
            }
            else {
                morefilters = false;
            }
        }
    }
}

void svlVidCapSrcDirectShow::Close() 
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

IBaseFilter* svlVidCapSrcDirectShow::GetCaptureFilter(int devid)
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
            filtref->Release();
        }
        moniker->Release();
        filtref = 0;
    }
    enumcat->Release();
    devenum->Release();

    return filtref;
}

IPin* svlVidCapSrcDirectShow::EnumeratePin(IBaseFilter* filter, LPCTSTR name, GUID *category)
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

int svlVidCapSrcDirectShow::GetPinCategory(IPin *pin, GUID *category)
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

int svlVidCapSrcDirectShow::SetDeviceInput(IBaseFilter *capfilter, int input_id)
{
    if (capfilter == 0) return SVL_FAIL;

    HRESULT hr;
    svlVidCapSrcDirectShowInputSelector *crossbar;
    GUID pincategory;
    IPin *pin;

    pincategory = PIN_CATEGORY_ANALOGVIDEOIN;
    pin = EnumeratePin(capfilter, 0, &pincategory);
    if (pin != 0) {
        crossbar = new svlVidCapSrcDirectShowInputSelector(pin);
        hr = crossbar->SetInputIndex((LONG)input_id);

        delete crossbar;
        pin->Release();

        if (hr == S_OK) return SVL_OK;
    }

    return SVL_FAIL;
}

int svlVidCapSrcDirectShow::GetDeviceInputs(IBaseFilter *capfilter, svlFilterSourceVideoCapture::DeviceInfo *deviceinfo)
{
    if (capfilter == 0 ||
        deviceinfo == 0) return SVL_FAIL;

    IPin *pin;
    svlVidCapSrcDirectShowInputSelector *crossbar;
    GUID pincategory;
    LONG count;
    std::string name;
    int i, length;

    pincategory = PIN_CATEGORY_ANALOGVIDEOIN;
    pin = EnumeratePin(capfilter, 0, &pincategory);
    if (pin != 0) {
        crossbar = new svlVidCapSrcDirectShowInputSelector(pin);

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

int svlVidCapSrcDirectShow::Start() 
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

int svlVidCapSrcDirectShow::Stop() 
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

svlImageRGB* svlVidCapSrcDirectShow::GetLatestFrame(bool waitfornew, unsigned int videoch)
{
    if (!Initialized || videoch >= NumOfStreams) return 0;
    return OutputBuffer[videoch]->Pull(waitfornew);
}

int svlVidCapSrcDirectShow::ShowFormatDialog(HWND hwnd, unsigned int videoch)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "ShowFormatDialog called for channel " << videoch << std::endl;
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

int svlVidCapSrcDirectShow::ShowImageDialog(HWND hwnd, unsigned int videoch)
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

int svlVidCapSrcDirectShow::GetMediaType(unsigned char *&mediabuffer, unsigned int &length, unsigned int videoch)
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

int svlVidCapSrcDirectShow::SetMediaType(unsigned char *mediabuffer, unsigned int length, unsigned int videoch)
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

int svlVidCapSrcDirectShow::SetupMediaType(unsigned int videoch)
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

int svlVidCapSrcDirectShow::RequestMediaType(unsigned int videoch)
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

int svlVidCapSrcDirectShow::SetDevice(int devid, int inid, unsigned int videoch)
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

int svlVidCapSrcDirectShow::SetRendererOnOff(bool render, unsigned int videoch)
{
    if (!Initialized ||
        videoch >= NumOfStreams) return SVL_FAIL;

    if (render) EnableRenderer[videoch] = 1;
    else EnableRenderer[videoch] = 0;

    return SVL_OK;
}

int svlVidCapSrcDirectShow::GetWidth(unsigned int videoch)
{
    if (!Initialized ||
        videoch >= NumOfStreams) return SVL_FAIL;

    return CapWidth[videoch];
}

int svlVidCapSrcDirectShow::GetHeight(unsigned int videoch)
{
    if (!Initialized ||
        videoch >= NumOfStreams) return SVL_FAIL;

    return CapHeight[videoch];
}


/***************************************/
/*** svlVidCapSrcDirectShowCB class ****/
/***************************************/

svlVidCapSrcDirectShowCB::svlVidCapSrcDirectShowCB(svlBufferImage *buffer, bool topdown) :
    Buffer(buffer),
    TopDown(topdown)
{
}

STDMETHODIMP svlVidCapSrcDirectShowCB::SampleCB(double SampleTime, IMediaSample *pSample)
{
    return E_NOTIMPL;
}

STDMETHODIMP svlVidCapSrcDirectShowCB::BufferCB(double sampletime, unsigned char *buffer, long buffersize)
{
    if (Buffer) Buffer->Push(buffer, buffersize, TopDown);
    return S_OK;
}

