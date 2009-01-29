/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlStreamBranchSource.cpp,v 1.4 2008/10/22 20:04:13 vagvoba Exp $
  
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

using namespace std;


/*************************************/
/*** svlStreamBranchSource class *****/
/*************************************/

svlStreamBranchSource::svlStreamBranchSource(svlStreamType type) : svlFilterBase()
{
    unsigned int i;

    OutputData = 0;
    for (i = 0; i < SMPSRC_BUFFERS; i ++) DataBuffer[i] = 0;

    switch (type) {
        case svlTypeImageRGB:
        {
            SetFilterToSource(svlTypeImageRGB);
            for (i = 0; i < SMPSRC_BUFFERS; i ++) {
                DataBuffer[i] = new svlSampleImageRGB;
            }
        }
        break;

        case svlTypeImageRGBStereo:
        {
            SetFilterToSource(svlTypeImageRGBStereo);
            for (i = 0; i < SMPSRC_BUFFERS; i ++) {
                DataBuffer[i] = new svlSampleImageRGBStereo;
            }
        }
        break;

        case svlTypeImageMono8:
        {
            SetFilterToSource(svlTypeImageMono8);
            for (i = 0; i < SMPSRC_BUFFERS; i ++) {
                DataBuffer[i] = new svlSampleImageMono8;
            }
        }
        break;

        case svlTypeImageMono8Stereo:
        {
            SetFilterToSource(svlTypeImageMono8Stereo);
            for (i = 0; i < SMPSRC_BUFFERS; i ++) {
                DataBuffer[i] = new svlSampleImageMono8Stereo;
            }
        }
        break;

        case svlTypeImageMono16:
        {
            SetFilterToSource(svlTypeImageMono16);
            for (i = 0; i < SMPSRC_BUFFERS; i ++) {
                DataBuffer[i] = new svlSampleImageMono16;
            }
        }
        break;

        case svlTypeImageMono16Stereo:
        {
            SetFilterToSource(svlTypeImageMono16Stereo);
            for (i = 0; i < SMPSRC_BUFFERS; i ++) {
                DataBuffer[i] = new svlSampleImageMono16Stereo;
            }
        }
        break;

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

    NextFreeBufferPos = 0;
    Hertz = 30.0;
}

svlStreamBranchSource::svlStreamBranchSource() : svlFilterBase()
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

int svlStreamBranchSource::Initialize(svlSample* inputdata)
{
    Release();

    int readypos = NextFreeBufferPos + 1;
    if (readypos >= SMPSRC_BUFFERS) readypos = 0;
    OutputData = DataBuffer[readypos];

    Timer.Reset();
    Timer.Start();
    ulFrameTime = 1.0 / Hertz;

    return SVL_OK;
}

int svlStreamBranchSource::ProcessFrame(ProcInfo* procInfo, svlSample* inputdata)
{
    int ret = SVL_OK;

    _OnSingleThread(procInfo)
    {
        if (FrameCounter > 0) {
            double time = Timer.GetElapsedTime();
            double t1 = ulFrameTime * FrameCounter;
            double t2 = time - ulStartTime;
            if (t1 > t2) osaSleep(t1 - t2);
        }
        else {
            ulStartTime = Timer.GetElapsedTime();
        }
        ret = PullSample();
    }

    return ret;
}

int svlStreamBranchSource::Release()
{
    return SVL_OK;
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

void svlStreamBranchSource::SetupSource(svlSample* inputdata, double hertz)
{
    for (unsigned int i = 0; i < SMPSRC_BUFFERS; i ++) {
        dynamic_cast<svlSampleImageBase*>(DataBuffer[i])->SetSize(*(dynamic_cast<svlSampleImageBase*>(inputdata)));
    }
    Hertz = hertz;
}

void svlStreamBranchSource::PushSample(svlSample* inputdata)
{
    int freepos = NextFreeBufferPos;

    svlSampleImageBase* outputbuffer = dynamic_cast<svlSampleImageBase*>(DataBuffer[freepos]);
    unsigned int vidchannels = outputbuffer->GetVideoChannels();

    // TODO: tripple buffering
    for (unsigned int i = 0; i < vidchannels; i ++) {
        memcpy(outputbuffer->GetPointer(i), dynamic_cast<svlSampleImageBase*>(inputdata)->GetPointer(i), outputbuffer->GetDataSize(i));
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

