/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2006 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlFilterSourceDummy.h>
#include <string.h>
#include "time.h"


/*************************************/
/*** svlFilterSourceDummy class ******/
/*************************************/

CMN_IMPLEMENT_SERVICES(svlFilterSourceDummy)

svlFilterSourceDummy::svlFilterSourceDummy() :
    svlFilterSourceBase(),
    cmnGenericObject(),
    Width(0),
    Height(0),
    Noise(false)
{
    OutputData = 0;
}

svlFilterSourceDummy::svlFilterSourceDummy(svlStreamType type) :
    svlFilterSourceBase(),
    cmnGenericObject(),
    Width(0),
    Height(0),
    Noise(false)
{
    OutputData = 0;
    SetType(type);
}

svlFilterSourceDummy::svlFilterSourceDummy(const svlSampleImageBase & image) :
    svlFilterSourceBase(),
    cmnGenericObject(),
    Width(0),
    Height(0),
    Noise(false)
{
    OutputData = 0;
    SetImage(image);
}

svlFilterSourceDummy::~svlFilterSourceDummy()
{
    if (OutputData) delete OutputData;
}

int svlFilterSourceDummy::SetType(svlStreamType type)
{
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;

    // Other types may be added in the future
    if (type != svlTypeImageRGB && type != svlTypeImageRGBStereo) return SVL_FAIL;

    if (OutputData && OutputData->GetType() != type) {
        delete OutputData;
        OutputData = svlSample::GetNewFromType(type);
    }
    else if (!OutputData) OutputData = svlSample::GetNewFromType(type);

    if (Width > 0 && Height > 0) dynamic_cast<svlSampleImageBase*>(OutputData)->SetSize(Width, Height);
    AddSupportedType(type);

    return SVL_OK;
}

int svlFilterSourceDummy::SetImage(const svlSampleImageBase & image)
{
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;

    svlStreamType type = image.GetType();

    // Other types may be added in the future
    if (type != svlTypeImageRGB && type != svlTypeImageRGBStereo) return SVL_FAIL;

    if (OutputData && OutputData->GetType() != type) {
        delete OutputData;
        OutputData = svlSample::GetNewFromType(type);
    }
    else if (!OutputData) OutputData = svlSample::GetNewFromType(type);

    OutputData->CopyOf(image);
    AddSupportedType(type);
    Noise = false;

    return SVL_OK;
}

int svlFilterSourceDummy::SetDimensions(unsigned int width, unsigned int height)
{
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;

    Width = width;
    Height = height;

    if (OutputData) {
        dynamic_cast<svlSampleImageBase*>(OutputData)->SetSize(width, height);
    }

    return SVL_OK;
}

void svlFilterSourceDummy::EnableNoiseImage(bool noise)
{
    Noise = noise;
}

int svlFilterSourceDummy::Initialize()
{
    if (OutputData == 0) return SVL_FAIL;

    srand(static_cast<unsigned int>(time(0)));

    return SVL_OK;
}

int svlFilterSourceDummy::ProcessFrame(svlProcInfo* procInfo)
{
    // Try to keep TargetFrequency
    _OnSingleThread(procInfo) WaitForTargetTimer();

    if (Noise) {

        svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(OutputData);
        const unsigned int videochannels = img->GetVideoChannels();
        unsigned int channel, idx;
        unsigned char* ptr;

        for (channel = 0; channel < videochannels; channel ++) {

            ptr = img->GetUCharPointer(channel);

            _ParallelLoop(procInfo, idx, img->GetDataSize(channel))
            {
                ptr[idx] = static_cast<unsigned char>(rand());
            }
        }
    }

    return SVL_OK;
}

