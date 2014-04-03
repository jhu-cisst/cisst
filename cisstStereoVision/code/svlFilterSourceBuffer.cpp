/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Ali Uneri
  Created on: 2010-05-27

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstStereoVision/svlFilterSourceBuffer.h>
#include <cisstStereoVision/svlFilterOutput.h>


/*************************************/
/*** svlFilterSourceBuffer class *****/
/*************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterSourceBuffer, svlFilterSourceBase)

svlFilterSourceBuffer::svlFilterSourceBuffer() :
    svlFilterSourceBase(),
    OutputSample(0),
    Buffer(0),
    Timeout(5.0)
{
    AddOutput("output", true);
    SetAutomaticOutputType(false);
}

svlFilterSourceBuffer::~svlFilterSourceBuffer()
{
    if (OutputSample) delete OutputSample;
}

int svlFilterSourceBuffer::SetBuffer(svlBufferSample& buffer)
{
    if (IsInitialized()) return SVL_FAIL;

    svlStreamType type = buffer.GetType();
    if (GetOutput()->SetType(type) != SVL_OK) return SVL_FAIL;

    if (OutputSample) delete OutputSample;
    OutputSample = svlSample::GetNewFromType(type);
    Buffer = &buffer;

    return SVL_OK;
}

void svlFilterSourceBuffer::SetTimeout(const double timeout)
{
    Timeout = timeout;
}

int svlFilterSourceBuffer::Initialize(svlSample* &syncOutput)
{
    if (!OutputSample || !Buffer) return SVL_FAIL;

    svlSample* input = Buffer->Pull(true, Timeout);
    if (!input || OutputSample->CopyOf(input) != SVL_OK) return SVL_FAIL;

    syncOutput = OutputSample;
    return SVL_OK;
}

int svlFilterSourceBuffer::Process(svlProcInfo* procInfo, svlSample* &syncOutput)
{
    if (!OutputSample || !Buffer) return SVL_FAIL;

    _OnSingleThread(procInfo)
    {
        svlSample* input = Buffer->Pull(true, Timeout);
        if (!input || OutputSample->CopyOf(input) != SVL_OK) return SVL_FAIL;
    }

    syncOutput = OutputSample;
    return SVL_OK;
}

