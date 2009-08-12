/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2007

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlStreamBranchSource.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <string.h>

#ifdef _MSC_VER
    // Quick fix for Visual Studio Intellisense:
    // The Intellisense parser can't handle the CMN_UNUSED macro
    // correctly if defined in cmnPortability.h, thus
    // we should redefine it here for it.
    // Removing this part of the code will not effect compilation
    // in any way, on any platforms.
    #undef CMN_UNUSED
    #define CMN_UNUSED(argument) argument
#endif

using namespace std;


/*************************************/
/*** svlStreamBranchSource class *****/
/*************************************/

svlStreamBranchSource::svlStreamBranchSource(svlStreamType type) :
    svlFilterSourceBase(false), // manual timestamp management
    NextFreeBufferPos(0)
{
    unsigned int i;

    OutputData = 0;
    for (i = 0; i < SMPSRC_BUFFERS; i ++) DataBuffer[i] = 0;

    AddSupportedType(type);
    for (i = 0; i < SMPSRC_BUFFERS; i ++) {
        DataBuffer[i] = svlSample::GetNewFromType(type);
    }
}

svlStreamBranchSource::svlStreamBranchSource() : svlFilterSourceBase()
{
    // Never should be called called
}

svlStreamBranchSource::~svlStreamBranchSource()
{
    Release();

    for (unsigned int i = 0; i < SMPSRC_BUFFERS; i ++) {
        if (DataBuffer[i]) delete DataBuffer[i];
    }
}

int svlStreamBranchSource::Initialize()
{
    Release();

    int readypos = NextFreeBufferPos + 1;
    if (readypos >= SMPSRC_BUFFERS) readypos = 0;
    OutputData = DataBuffer[readypos];

    return SVL_OK;
}

int svlStreamBranchSource::ProcessFrame(ProcInfo* procInfo)
{
    int ret = SVL_OK;

    _OnSingleThread(procInfo)
    {
        // Try to keep TargetFrequency
        WaitForTargetTimer();

        ret = PullSample();
    }

    return ret;
}

bool svlStreamBranchSource::IsTypeSupported(svlStreamType type)
{
    switch (type) {
        case svlTypeImageRGB:
        case svlTypeImageRGBStereo:
        case svlTypeImageMono8:
        case svlTypeImageMono8Stereo:
        case svlTypeImageMono16:
        case svlTypeImageMono16Stereo:
            return true;

        // Other types may be added in the future
        case svlTypeInvalid:
        case svlTypeStreamSource:
        case svlTypeStreamSink:
        case svlTypeImageCustom:
        case svlTypeDepthMap:
        case svlTypeRigidXform:
        case svlTypePointCloud:
        break;
    }
    return false;
}

void svlStreamBranchSource::SetInputSample(svlSample* inputdata)
{
    for (unsigned int i = 0; i < SMPSRC_BUFFERS; i ++) {
        dynamic_cast<svlSampleImageBase*>(DataBuffer[i])->SetSize(*(dynamic_cast<svlSampleImageBase*>(inputdata)));
    }
}

void svlStreamBranchSource::PushSample(svlSample* inputdata)
{
    int freepos = NextFreeBufferPos;

    svlSampleImageBase* outputbuffer = dynamic_cast<svlSampleImageBase*>(DataBuffer[freepos]);
    unsigned int vidchannels = outputbuffer->GetVideoChannels();

    // TODO: tripple buffering
    for (unsigned int i = 0; i < vidchannels; i ++) {
        memcpy(outputbuffer->GetUCharPointer(i), dynamic_cast<svlSampleImageBase*>(inputdata)->GetUCharPointer(i), outputbuffer->GetDataSize(i));
    }
    // TODO: tripple buffering

    freepos ++;
    if (freepos >= SMPSRC_BUFFERS) freepos = 0;
    NextFreeBufferPos = freepos;

    NewFrameEvent.Raise();
}

int svlStreamBranchSource::PullSample()
{
    while (!NewFrameEvent.Wait(1.0)) {
        if (IsRunning() == false) break;
    }

    int readypos = NextFreeBufferPos + 1;
    if (readypos >= SMPSRC_BUFFERS) readypos = 0;
    OutputData = DataBuffer[readypos];

    return SVL_OK;
}

