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

#include <cisstStereoVision/svlFilterSampler.h>
#include <cisstStereoVision/svlBufferSample.h>


/******************************/
/*** svlFilterSampler class ***/
/******************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterSampler, svlFilterBase)

svlFilterSampler::svlFilterSampler() :
    svlFilterBase(),
    Buffer(0)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBA);
    AddInputType("input", svlTypeImageRGBStereo);
    AddInputType("input", svlTypeImageRGBAStereo);
    AddInputType("input", svlTypeImageMono8);
    AddInputType("input", svlTypeImageMono8Stereo);
    AddInputType("input", svlTypeImageMono16);
    AddInputType("input", svlTypeImageMono16Stereo);
    AddInputType("input", svlTypeImageMono32);
    AddInputType("input", svlTypeImageMono32Stereo);
    AddInputType("input", svlTypeImage3DMap);
    AddInputType("input", svlTypeMatrixInt8);
    AddInputType("input", svlTypeMatrixInt16);
    AddInputType("input", svlTypeMatrixInt32);
    AddInputType("input", svlTypeMatrixInt64);
    AddInputType("input", svlTypeMatrixUInt8);
    AddInputType("input", svlTypeMatrixUInt16);
    AddInputType("input", svlTypeMatrixUInt32);
    AddInputType("input", svlTypeMatrixUInt64);
    AddInputType("input", svlTypeMatrixFloat);
    AddInputType("input", svlTypeMatrixDouble);
    AddInputType("input", svlTypeTransform3D);
    AddInputType("input", svlTypeTargets);
    AddInputType("input", svlTypeText);
    AddInputType("input", svlTypeBlobs);

    AddOutput("output", true);
    SetAutomaticOutputType(true);
}

const svlSample* svlFilterSampler::PullSample(bool waitfornew, double timeout)
{
    if (!Buffer) return 0;
    return Buffer->Pull(waitfornew, timeout);
}

int svlFilterSampler::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    Release();

    syncOutput = syncInput;

    Buffer = new svlBufferSample(syncInput->GetType());

    return SVL_OK;
}

int svlFilterSampler::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    _SkipIfDisabled();

    _OnSingleThread(procInfo) Buffer->Push(syncInput);

    return SVL_OK;
}

int svlFilterSampler::Release()
{
    if (Buffer) {
        delete Buffer;
        Buffer = 0;
    }

    return SVL_OK;
}

