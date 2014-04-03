/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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
#include <cisstStereoVision/svlFilterOutput.h>
#include <cisstOSAbstraction/osaSleep.h>

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


/*************************************/
/*** svlStreamBranchSource class *****/
/*************************************/

svlStreamBranchSource::svlStreamBranchSource(svlStreamType type, unsigned int buffersize) :
    svlFilterSourceBase(false), // manual timestamp management
    InputBlocked(false),
    SampleQueue(type, buffersize)
{
    AddOutput("output", true);
    SetAutomaticOutputType(false);
    GetOutput()->SetType(type);
}

svlStreamBranchSource::svlStreamBranchSource() :
    svlFilterSourceBase(),
    SampleQueue(svlTypeInvalid, 0)
{
    // Never should be called
}

svlStreamBranchSource::~svlStreamBranchSource()
{
    Release();
}

int svlStreamBranchSource::Initialize(svlSample* &syncOutput)
{
    Release();

    // Pass unused but initialized sample downstream
    syncOutput = SampleQueue.Pull(0.0);
    if (!syncOutput) return SVL_FAIL;

    return SVL_OK;
}

int svlStreamBranchSource::Process(svlProcInfo* procInfo, svlSample* &syncOutput)
{
    _OnSingleThread(procInfo)
    {
        do {
            if (IsRunning() == false) break;
            OutputSample = SampleQueue.Pull(0.5);
        } while (OutputSample == 0);
    }

    _SynchronizeThreads(procInfo);

    syncOutput = OutputSample;

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
        case svlTypeImageMono32:
        case svlTypeImageMono32Stereo:
        case svlTypeImageRGBA:
        case svlTypeImageRGBAStereo:
        case svlTypeImage3DMap:
        case svlTypeCUDAImageRGB:
        case svlTypeCUDAImageRGBA:
        case svlTypeCUDAImageRGBStereo:
        case svlTypeCUDAImageRGBAStereo:
        case svlTypeCUDAImageMono8:
        case svlTypeCUDAImageMono8Stereo:
        case svlTypeCUDAImageMono16:
        case svlTypeCUDAImageMono16Stereo:
        case svlTypeCUDAImageMono32:
        case svlTypeCUDAImageMono32Stereo:
        case svlTypeCUDAImage3DMap:
        case svlTypeMatrixInt8:
        case svlTypeMatrixInt16:
        case svlTypeMatrixInt32:
        case svlTypeMatrixInt64:
        case svlTypeMatrixUInt8:
        case svlTypeMatrixUInt16:
        case svlTypeMatrixUInt32:
        case svlTypeMatrixUInt64:
        case svlTypeMatrixFloat:
        case svlTypeMatrixDouble:
        case svlTypeTransform3D:
        case svlTypeTargets:
        case svlTypeText:
        case svlTypeCameraGeometry:
        case svlTypeBlobs:
            return true;

        case svlTypeInvalid:
        case svlTypeStreamSource:
        case svlTypeStreamSink:
        break;
    }
    return false;
}

void svlStreamBranchSource::SetInput(svlSample* inputsample)
{
    SampleQueue.Push(inputsample);
}

void svlStreamBranchSource::PushSample(const svlSample* inputsample)
{
    if (InputBlocked) return;
    SampleQueue.Push(inputsample);
}

int svlStreamBranchSource::GetBufferUsage()
{
    return SampleQueue.GetUsage();
}

double svlStreamBranchSource::GetBufferUsageRatio()
{
    return SampleQueue.GetUsageRatio();
}

unsigned int svlStreamBranchSource::GetDroppedSampleCount()
{
    return SampleQueue.GetDroppedSampleCount();
}

int svlStreamBranchSource::BlockInput(bool block)
{
    InputBlocked = block;
    return SVL_OK;
}

