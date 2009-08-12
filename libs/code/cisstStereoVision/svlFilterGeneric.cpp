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
//          svlTypeImageMono8,
//          svlTypeImageMono16,
//          svlTypeImageRGBStereo,
//          svlTypeImageMono8Stereo,
//          svlTypeImageMono16Stereo,
//          svlTypeDepthMap,
//          svlTypeRigidXform,
//          svlTypePointCloud
// Valid outputs:
//          svlTypeImageRGB,
//          svlTypeImageMono8,
//          svlTypeImageMono16,
//          svlTypeImageRGBStereo,
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

    unsigned int buffersize;

    // checking if the callback has initialized the output data correctly
    switch (GetOutputType()) {
        case svlTypeImageRGB:
        {
            svlSampleImageRGB* img = dynamic_cast<svlSampleImageRGB*>(OutputData);
            buffersize = img->GetWidth() * img->GetHeight() * sizeof(svlRGB);
            if (buffersize != img->GetDataSize())
                return SVL_GEN_INVALID_OUTPUT_DATA;
        }
        break;

        case svlTypeImageRGBStereo:
        {
            svlSampleImageRGBStereo* stimg = dynamic_cast<svlSampleImageRGBStereo*>(OutputData);
            buffersize = stimg->GetWidth(SVL_LEFT) * stimg->GetHeight(SVL_LEFT) * sizeof(svlRGB);
            if (buffersize != stimg->GetDataSize(SVL_LEFT))
                return SVL_GEN_INVALID_OUTPUT_DATA;
            buffersize = stimg->GetWidth(SVL_RIGHT) * stimg->GetHeight(SVL_RIGHT) * sizeof(svlRGB);
            if (buffersize != stimg->GetDataSize(SVL_RIGHT))
                return SVL_GEN_INVALID_OUTPUT_DATA;
        }
        break;

        case svlTypeImageMono8:
        {
            svlSampleImageMono8* img = dynamic_cast<svlSampleImageMono8*>(OutputData);
            buffersize = img->GetWidth() * img->GetHeight() * sizeof(unsigned char);
            if (buffersize != img->GetDataSize())
                return SVL_GEN_INVALID_OUTPUT_DATA;
        }
        break;

        case svlTypeImageMono8Stereo:
        {
            svlSampleImageMono8Stereo* stimg = dynamic_cast<svlSampleImageMono8Stereo*>(OutputData);
            buffersize = stimg->GetWidth(SVL_LEFT) * stimg->GetHeight(SVL_LEFT) * sizeof(unsigned char);
            if (buffersize != stimg->GetDataSize(SVL_LEFT))
                return SVL_GEN_INVALID_OUTPUT_DATA;
            buffersize = stimg->GetWidth(SVL_RIGHT) * stimg->GetHeight(SVL_RIGHT) * sizeof(unsigned char);
            if (buffersize != stimg->GetDataSize(SVL_RIGHT))
                return SVL_GEN_INVALID_OUTPUT_DATA;
        }

        case svlTypeImageMono16:
        {
            svlSampleImageMono16* img = dynamic_cast<svlSampleImageMono16*>(OutputData);
            buffersize = img->GetWidth() * img->GetHeight() * sizeof(unsigned short);
            if (buffersize != img->GetDataSize())
                return SVL_GEN_INVALID_OUTPUT_DATA;
        }
        break;

        case svlTypeImageMono16Stereo:
        {
            svlSampleImageMono16Stereo* stimg = dynamic_cast<svlSampleImageMono16Stereo*>(OutputData);
            buffersize = stimg->GetWidth(SVL_LEFT) * stimg->GetHeight(SVL_LEFT) * sizeof(unsigned short);
            if (buffersize != stimg->GetDataSize(SVL_LEFT))
                return SVL_GEN_INVALID_OUTPUT_DATA;
            buffersize = stimg->GetWidth(SVL_RIGHT) * stimg->GetHeight(SVL_RIGHT) * sizeof(unsigned short);
            if (buffersize != stimg->GetDataSize(SVL_RIGHT))
                return SVL_GEN_INVALID_OUTPUT_DATA;
        }

        case svlTypeDepthMap:
        {
            svlSampleDepthMap* img = dynamic_cast<svlSampleDepthMap*>(OutputData);
            buffersize = img->GetWidth() * img->GetHeight() * sizeof(float);
            if (buffersize != img->GetDataSize())
                return SVL_GEN_INVALID_OUTPUT_DATA;
        }
        break;

        case svlTypeRigidXform:
        case svlTypePointCloud:
            if (OutputData == 0)
                return SVL_GEN_INVALID_OUTPUT_DATA;
        break;

        // Other types may be added in the future
        case svlTypeInvalid:
        case svlTypeStreamSource:
        case svlTypeStreamSink:
        case svlTypeImageCustom:
        break;
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

