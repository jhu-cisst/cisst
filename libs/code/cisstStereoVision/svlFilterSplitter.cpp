/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

  Author(s):  Balazs Vagvolgyi
  Created on: 2010

  (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlFilterSplitter.h>
#include <cisstStereoVision/svlFilterInput.h>
#include <cisstStereoVision/svlFilterOutput.h>

/**************************************/
/*** svlFilterSplitter class **********/
/**************************************/

CMN_IMPLEMENT_SERVICES(svlFilterSplitter)

svlFilterSplitter::svlFilterSplitter() :
    svlFilterBase()
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

    // Add the trunk output by default
    svlFilterBase::AddOutput("output", true);
    SetAutomaticOutputType(false);
}

int svlFilterSplitter::AddOutput(const std::string &name, const unsigned int threadcount, const unsigned int buffersize)
{
    if (GetOutput(name)) return SVL_FAIL;

    svlFilterOutput* output = svlFilterBase::AddOutput(name, false);
    output->SetThreadCount(threadcount);
    output->SetBufferSize(buffersize);

    const unsigned int size = AsyncOutputs.size();
    AsyncOutputs.resize(size + 1);
    AsyncOutputs[size] = output;

    return SVL_OK;
}

int svlFilterSplitter::UpdateTypes(svlFilterInput &input, svlStreamType type)
{
    // Check if type is on the supported list
    if (!input.IsTypeSupported(type)) return SVL_FAIL;

    // If all is good, set the trunk output type
    GetOutput()->SetType(type);

    // Then set the output types of non-trunk outputs
    const unsigned int size = AsyncOutputs.size();
    for (unsigned int i = 0; i < size; i ++) {
        if (AsyncOutputs[i]) AsyncOutputs[i]->SetType(type);
    }

    return SVL_OK;
}

int svlFilterSplitter::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;

    // Non-trunk outputs setup their buffers based on the input sample
    const unsigned int size = AsyncOutputs.size();
    for (unsigned int i = 0; i < size; i ++) {
        if (AsyncOutputs[i]) AsyncOutputs[i]->SetupSample(syncInput);
    }

    return SVL_OK;
}

int svlFilterSplitter::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;

    _OnSingleThread(procInfo) {
        // Non-trunk outputs copy the input sample into their buffers
        const unsigned int size = AsyncOutputs.size();
        for (unsigned int i = 0; i < size; i ++) {
            if (AsyncOutputs[i]) AsyncOutputs[i]->PushSample(syncInput);
        }
    }

    return SVL_OK;
}

