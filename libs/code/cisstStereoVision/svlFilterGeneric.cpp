/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlGenericFilter.cpp 75 2009-02-24 16:47:20Z adeguet1 $
  
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

#include <cisstStereoVision/svlFilterGeneric.h>

using namespace std;


/******************************************/
/*** svlFilterGeneric class ***************/
/******************************************/

////////////////////////////////////////////////////
// Valid inputs:
//          svlTypeImageRGB,
//          svlTypeImageRGBA,
//          svlTypeImageMono8,
//          svlTypeImageMono16,
//          svlTypeImageRGBStereo,
//          svlTypeImageRGBAStereo,
//          svlTypeImageMono8Stereo,
//          svlTypeImageMono16Stereo,
//          svlTypeDepthMap,
//          svlTypeRigidXform,
//          svlTypePointCloud
// Valid outputs:
//          svlTypeImageRGB,
//          svlTypeImageRGBA,
//          svlTypeImageMono8,
//          svlTypeImageMono16,
//          svlTypeImageRGBStereo,
//          svlTypeImageRGBAStereo,
//          svlTypeImageMono8Stereo,
//          svlTypeImageMono16Stereo,
//          svlTypeDepthMap,
//          svlTypeRigidXform,
//          svlTypePointCloud
// Otherwise, the filter will fail to initialize.
//
// Inplace processing is available only if the
// input type is the same as the output type and
// the sample sizes match
//
svlFilterGeneric::svlFilterGeneric(svlStreamType inputtype, svlStreamType outputtype) : svlFilterBase()
{
    if (inputtype != svlTypeInvalid &&
        inputtype != svlTypeStreamSink &&
        inputtype != svlTypeStreamSource &&
        outputtype != svlTypeInvalid &&
        outputtype != svlTypeStreamSink &&
        outputtype != svlTypeStreamSource) {

        // mapping input type to output type
        AddSupportedType(inputtype, outputtype);
        OutputData = svlSample::GetNewFromType(outputtype);
    }

    CallbackObj = 0;
}

svlFilterGeneric::~svlFilterGeneric()
{
    Release();

    // destroying output sample
    if (OutputData) delete OutputData;
}

int svlFilterGeneric::SetCallback(svlGenericFilterCallbackBase* callbackobj)
{
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;
    CallbackObj = callbackobj;
    return SVL_OK;
}

int svlFilterGeneric::Initialize(svlSample* inputdata)
{
    if (CallbackObj == 0)
        return SVL_GEN_NO_CALLBACK_OBJECT;

    // Call initialize callback
    int ret = CallbackObj->InitializeCallback(inputdata, OutputData);
    if (ret != SVL_OK) return ret;

    // checking if the callback has initialized the output data correctly
    if (OutputData == 0) return SVL_GEN_INVALID_OUTPUT_DATA;
    if (OutputData->IsImage()) {
        svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(OutputData);
        if (img == 0) return SVL_GEN_INVALID_OUTPUT_DATA;
        for (unsigned int i = 0; i < img->GetVideoChannels(); i ++) {
            if ((img->GetWidth(i) * img->GetHeight(i) * img->GetDataChannels()) != img->GetDataSize(i))
                return SVL_GEN_INVALID_OUTPUT_DATA;
        }
    }

    return SVL_OK;
}

int svlFilterGeneric::ProcessFrame(ProcInfo* procInfo, svlSample* inputdata)
{
    int ret = SVL_OK;

    _OnSingleThread(procInfo)
    {
        // Call process callback
        ret = CallbackObj->ProcessCallback(inputdata, OutputData);
    }

    return ret;
}

int svlFilterGeneric::Release()
{
    // Call release callback
    if (CallbackObj) CallbackObj->ReleaseCallback();

    return SVL_OK;
}


/*******************************************/
/*** svlGenericFilterCallbackBase class ****/
/*******************************************/

svlGenericFilterCallbackBase::~svlGenericFilterCallbackBase()
{
}

void svlGenericFilterCallbackBase::ReleaseCallback()
{
}

