/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2011

  (C) Copyright 2006-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlFilterImageBlobDetector.h>
#include <cisstStereoVision/svlFilterInput.h>


/****************************************/
/*** svlFilterImageBlobDetector class ***/
/****************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterImageBlobDetector, svlFilterBase)

svlFilterImageBlobDetector::svlFilterImageBlobDetector() :
    svlFilterBase(),
    OutputBlobIDs(0),
    OutputBlobs(0),
    MaxBlobCount(1000),
    FiltMinArea(0),
    FiltMaxArea(0),
    FiltMinCompactness(0.0),
    FiltMaxCompactness(0.0)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageMono8);
    AddInputType("input", svlTypeImageMono8Stereo);

    AddOutput("blobsmap", true);
    AddOutput("blobs", false);
    SetOutputType("blobs", svlTypeBlobs);
}

svlFilterImageBlobDetector::~svlFilterImageBlobDetector()
{
    delete OutputBlobIDs;
    delete OutputBlobs;
}

int svlFilterImageBlobDetector::SetMaxBlobCount(unsigned int max_blobs)
{
    if (IsInitialized()) return SVL_FAIL;
    MaxBlobCount = max_blobs;
    return SVL_OK;
}

void svlFilterImageBlobDetector::SetFilterArea(unsigned int min_area, unsigned int max_area)
{
    FiltMinArea = min_area;
    FiltMaxArea = max_area;
}

void svlFilterImageBlobDetector::SetFilterCompactness(double min_compactness, double max_compactness)
{
    FiltMinCompactness = min_compactness;
    FiltMaxCompactness = max_compactness;
}

int svlFilterImageBlobDetector::OnConnectInput(svlFilterInput &input, svlStreamType type)
{
    // Check if type is on the supported list
    if (!input.IsTypeSupported(type)) return SVL_FAIL;

    svlStreamType output_type;
    unsigned int channelcount = 0;

    if (type == svlTypeImageMono8) {
        output_type = svlTypeImageMono32;
        channelcount = 1;
    }
    else if (type == svlTypeImageMono8Stereo) {
        output_type = svlTypeImageMono32Stereo;
        channelcount = 2;
    }
    else return SVL_FAIL;

    SetOutputType("blobsmap", output_type);

    delete OutputBlobIDs;
    OutputBlobIDs = dynamic_cast<svlSampleImage*>(svlSample::GetNewFromType(output_type));

    delete OutputBlobs;
    OutputBlobs = new svlSampleBlobs;
    OutputBlobs->SetChannelCount(channelcount);

    return SVL_OK;
}

int svlFilterImageBlobDetector::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    Release();

    OutputBlobIDs->SetSize(syncInput);
    syncOutput = OutputBlobIDs;

    BlobsOutputConnected = GetOutput("blobs")->IsConnected();
    if (BlobsOutputConnected) {
        for (unsigned int i = 0; i < OutputBlobs->GetChannelCount(); i ++) {
            OutputBlobs->SetBufferSize(MaxBlobCount, i);
        }
    }

    return SVL_OK;
}

int svlFilterImageBlobDetector::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = OutputBlobIDs;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);
    _SkipIfDisabled();

    svlSampleImage* img = dynamic_cast<svlSampleImage*>(syncInput);
    unsigned int videochannels = img->GetVideoChannels();
    unsigned int idx;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        if (videochannels == 1) {
            svlImageProcessing::LabelBlobs(dynamic_cast<svlSampleImageMono8*>(img),
                                           dynamic_cast<svlSampleImageMono32*>(OutputBlobIDs),
                                           DetectorInternals[idx]);
            if (BlobsOutputConnected) {
                svlImageProcessing::GetBlobsFromLabels(dynamic_cast<svlSampleImageMono8*>(img),
                                                       dynamic_cast<svlSampleImageMono32*>(OutputBlobIDs),
                                                       OutputBlobs,
                                                       DetectorInternals[idx],
                                                       FiltMinArea,
                                                       FiltMaxArea,
                                                       FiltMinCompactness,
                                                       FiltMaxCompactness);
            }
        }
        else {
            svlImageProcessing::LabelBlobs(dynamic_cast<svlSampleImageMono8Stereo*>(img),
                                           dynamic_cast<svlSampleImageMono32Stereo*>(OutputBlobIDs),
                                           idx,
                                           DetectorInternals[idx]);
            if (BlobsOutputConnected) {
                svlImageProcessing::GetBlobsFromLabels(dynamic_cast<svlSampleImageMono8Stereo*>(img),
                                                       dynamic_cast<svlSampleImageMono32Stereo*>(OutputBlobIDs),
                                                       OutputBlobs,
                                                       idx,
                                                       DetectorInternals[idx],
                                                       FiltMinArea,
                                                       FiltMaxArea,
                                                       FiltMinCompactness,
                                                       FiltMaxCompactness);
            }
        }
    }

    if (BlobsOutputConnected)
    {
        _SynchronizeThreads(procInfo);
        _OnSingleThread(procInfo)
        {
            GetOutput("blobs")->PushSample(OutputBlobs);
        }
    }

    return SVL_OK;
}

