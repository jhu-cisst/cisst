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

svlStreamBranchSource::svlStreamBranchSource(svlStreamType type, unsigned int buffersize) :
    svlFilterSourceBase(false), // manual timestamp management
    InputBlocked(false),
    BufferSize(std::max(3, static_cast<int>(buffersize))), // buffer size is greater or equal to 3
    DroppedSamples(0),
    BufferUsage(0)
{
    AddSupportedType(type);

    SampleBuffer.SetSize(BufferSize);
    SampleBuffer.SetAll(0);
    BackwardPos.SetSize(BufferSize);
    ForwardPos.SetSize(BufferSize);

    int i;

    // Create samples in the buffer
    for (i = 0; i < BufferSize; i ++) {
        SampleBuffer[i] = svlSample::GetNewFromType(type);
    }

    // Initialize chained list of samples
    for (i = 2; i < BufferSize; i ++) {
        BackwardPos[i] = i - 1;
        ForwardPos[i - 1] = i;
    }
    BackwardPos[1] = BufferSize - 1;
    ForwardPos[BufferSize - 1] = 1;
    NewestPos = 1;
    OldestPos = -1;
    LockedPos = -1;
}

svlStreamBranchSource::svlStreamBranchSource() :
    svlFilterSourceBase(),
    BufferSize(0)
{
    // Never should be called called
}

svlStreamBranchSource::~svlStreamBranchSource()
{
    Release();

    for (int i = 0; i < BufferSize; i ++) {
        if (SampleBuffer[i]) delete SampleBuffer[i];
    }
}

int svlStreamBranchSource::Initialize()
{
    Release();

    // Pass unused but initialized sample downstream
    OutputData = SampleBuffer[0];

    return SVL_OK;
}

int svlStreamBranchSource::ProcessFrame(ProcInfo* procInfo)
{
    int ret = SVL_OK;

    _OnSingleThread(procInfo)
    {
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
        case svlTypeImageRGBA:
        case svlTypeImageRGBAStereo:
        case svlTypeInvalid:
        case svlTypeStreamSource:
        case svlTypeStreamSink:
        case svlTypeImageCustom:
        case svlTypeImageMonoFloat:
        case svlTypeImage3DMap:
        case svlTypeRigidXform:
        case svlTypePointCloud:
        break;
    }
    return false;
}

void svlStreamBranchSource::SetInputSample(svlSample* inputdata)
{
    for (int i = 0; i < BufferSize; i ++) {
        SampleBuffer[i]->SetSize(*inputdata);
    }
}

void svlStreamBranchSource::PushSample(svlSample* inputdata)
{
    if (InputBlocked) return;

    svlSampleImageBase* outputbuffer = dynamic_cast<svlSampleImageBase*>(SampleBuffer[NewestPos]);
    const unsigned int vidchannels = outputbuffer->GetVideoChannels();

    // Passing time stamp
    outputbuffer->SetTimestamp(inputdata->GetTimestamp());

    for (unsigned int i = 0; i < vidchannels; i ++) {
        memcpy(outputbuffer->GetUCharPointer(i), dynamic_cast<svlSampleImageBase*>(inputdata)->GetUCharPointer(i), outputbuffer->GetDataSize(i));
    }

    CS.Enter();
        NewestPos = ForwardPos[NewestPos];
        if (NewestPos != OldestPos) {
            BufferUsage ++;
        }
        else {
            OldestPos = ForwardPos[NewestPos];
            DroppedSamples ++;
        }
    CS.Leave();

    NewFrameEvent.Raise();
}

int svlStreamBranchSource::PullSample()
{
    // Make sure a new frame has arrived since the last call
    if (BufferUsage <= 1) {
        while (!NewFrameEvent.Wait(0.5)) {
            if (IsRunning() == false) return SVL_OK;
        }
    }

    CS.Enter();
        if (LockedPos >= 0) {
            ForwardPos[LockedPos] = ForwardPos[OldestPos];
            BackwardPos[LockedPos] = BackwardPos[OldestPos];
            ForwardPos[BackwardPos[OldestPos]] = LockedPos;
            BackwardPos[ForwardPos[OldestPos]] = LockedPos;

            LockedPos = OldestPos;
            OldestPos = ForwardPos[OldestPos];

            BufferUsage --;
        }
        else {
            // At start: wait for one more pushed frame
            LockedPos = 0;
            OldestPos = 1;
            BufferUsage --;

            CS.Leave();
            return PullSample();
        }
    CS.Leave();

    OutputData = SampleBuffer[LockedPos];

    return SVL_OK;
}

int svlStreamBranchSource::GetBufferUsage()
{
    return BufferUsage;
}

double svlStreamBranchSource::GetBufferUsageRatio()
{
    return static_cast<double>(BufferUsage) / (BufferSize - 1);
}

unsigned int svlStreamBranchSource::GetDroppedSampleCount()
{
    return DroppedSamples;
}

int svlStreamBranchSource::BlockInput(bool block)
{
    InputBlocked = block;
    return SVL_OK;
}
