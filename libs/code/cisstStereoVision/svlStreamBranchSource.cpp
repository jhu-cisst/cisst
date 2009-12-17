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
    SampleQueue(type, buffersize)
{
    AddSupportedType(type);
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

int svlStreamBranchSource::Initialize()
{
    Release();

    // Pass unused but initialized sample downstream
    OutputData = SampleQueue.Peek();

    return SVL_OK;
}

int svlStreamBranchSource::ProcessFrame(ProcInfo* procInfo)
{
    _OnSingleThread(procInfo)
    {
        do {
            if (IsRunning() == false) break;
            OutputData = SampleQueue.Pull(0.5);
        } while (OutputData == 0);
    }

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
        case svlTypeImageRGBA:
        case svlTypeImageRGBAStereo:
        case svlTypeImageMonoFloat:
        case svlTypeImage3DMap:
        case svlTypeImageCustom:
        case svlTypeRigidXform:
        case svlTypePointCloud:
            return true;

        case svlTypeInvalid:
        case svlTypeStreamSource:
        case svlTypeStreamSink:
        break;
    }
    return false;
}

void svlStreamBranchSource::SetInput(svlSample* inputdata)
{
    SampleQueue.PreAllocate(*inputdata);
}

void svlStreamBranchSource::PushSample(svlSample* inputdata)
{
    if (InputBlocked) return;
    SampleQueue.Push(*inputdata);
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

