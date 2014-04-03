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

#include <cisstStereoVision/svlFilterImageBlobTracker.h>
#include <cisstStereoVision/svlFilterInput.h>
#include <cisstStereoVision/svlFilterOutput.h>

#define BLOB_BUFFER_SIZE    1000


/***************************************/
/*** svlFilterImageBlobTracker class ***/
/***************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterImageBlobTracker, svlFilterBase)

svlFilterImageBlobTracker::svlFilterImageBlobTracker() :
    svlFilterBase(),
    OutputBlobs(0)
{
    AddInput("blobsmap", true);
    AddInputType("blobsmap", svlTypeImageMono32);
    AddInputType("blobsmap", svlTypeImageMono32Stereo);

    AddInput("blobs", false);
    AddInputType("blobs", svlTypeBlobs);

    AddOutput("blobsmap", true);
    SetAutomaticOutputType(true);

    AddOutput("blobs", false);
    SetOutputType("blobs", svlTypeBlobs);
}

svlFilterImageBlobTracker::~svlFilterImageBlobTracker()
{
    delete OutputBlobs;
}

int svlFilterImageBlobTracker::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    Release();

    syncOutput = syncInput;

    svlSampleImage* img = dynamic_cast<svlSampleImage*>(syncInput);
    if (!img) return SVL_FAIL;
    const unsigned int channelcount = img->GetVideoChannels();

    delete OutputBlobs;
    OutputBlobs = new svlSampleBlobs;
    OutputBlobs->SetChannelCount(channelcount);
    for (unsigned int i = 0; i < channelcount; i ++) {
        OutputBlobs->SetBufferSize(BLOB_BUFFER_SIZE, i);
    }

    GetOutput("blobs")->PushSample(OutputBlobs);

    return SVL_OK;
}

int svlFilterImageBlobTracker::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);
    _SkipIfDisabled();

    svlSampleBlobs *in_blobs = 0;

    _OnSingleThread(procInfo)
    {
        // Attempting to pull a sample out of the 'blobs' input
        svlFilterInput *input = GetInput("blobs");
        if (input) in_blobs = dynamic_cast<svlSampleBlobs*>(input->PullSample(true, 0.0));

        if (in_blobs) {
            // TO DO: implement tracking
        }
    }

    _SynchronizeThreads(procInfo);

    _OnSingleThread(procInfo)
    {
        if (in_blobs) GetOutput("blobs")->PushSample(in_blobs);
    }

    return SVL_OK;
}

