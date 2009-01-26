/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlGenericFilter.cpp,v 1.15 2008/10/22 20:04:13 vagvoba Exp $
  
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

#include <cisstStereoVision/svlGenericFilter.h>

using namespace std;


/******************************************/
/*** svlGenericFilter class ***************/
/******************************************/

////////////////////////////////////////////////////
// Valid inputs:
//          svlTypeStreamSource,
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
svlGenericFilter::svlGenericFilter(svlStreamType inputtype, svlStreamType outputtype) : svlFilterBase()
{
    if (inputtype != svlTypeInvalid &&
        inputtype != svlTypeStreamSink &&
        outputtype != svlTypeInvalid &&
        outputtype != svlTypeStreamSource &&
        outputtype != svlTypeStreamSink) {

        if (inputtype == svlTypeStreamSource) {
            // setting output type
            SetFilterToSource(outputtype);
        }
        else {
            // mapping input type to output type
            AddSupportedType(inputtype, outputtype);
        }

        // initializing output sample
        switch (outputtype) {
            case svlTypeImageRGB:
                OutputData = new svlSampleImageRGB;
            break;

            case svlTypeImageRGBStereo:
                OutputData = new svlSampleImageRGBStereo;
            break;

            case svlTypeImageMono8:
                OutputData = new svlSampleImageMono8;
            break;

            case svlTypeImageMono8Stereo:
                OutputData = new svlSampleImageMono8Stereo;
            break;

            case svlTypeImageMono16:
                OutputData = new svlSampleImageMono16;
            break;

            case svlTypeImageMono16Stereo:
                OutputData = new svlSampleImageMono16Stereo;
            break;

            case svlTypeDepthMap:
                OutputData = new svlSampleDepthMap;
            break;

            case svlTypeRigidXform:
                OutputData = new svlSampleRigidXform;
                if (OutputData) dynamic_cast<svlSampleRigidXform*>(OutputData)->frame4x4 = svlRigidXform::Eye();
            break;

            case svlTypePointCloud:
                OutputData = new svlSamplePointCloud;
            break;

            // Other types may be added in the future
            case svlTypeInvalid:
            case svlTypeStreamSource:
            case svlTypeStreamSink:
            case svlTypeImageCustom:
            break;
        }
    }

    Hertz = 30.0;
    CallbackObj = 0;
}

svlGenericFilter::~svlGenericFilter()
{
    Release();

    // destroying output sample
    if (OutputData) delete OutputData;
}

int svlGenericFilter::SetCallback(svlGenericFilterCallbackBase* callbackobj)
{
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;
    CallbackObj = callbackobj;
    return SVL_OK;
}

int svlGenericFilter::Initialize(svlSample* inputdata)
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

    // Initialize source timer
    if (GetInputType() == svlTypeStreamSource) {
        Timer.Reset();
        Timer.Start();
        ulFrameTime = 1.0 / Hertz;
    }

    return SVL_OK;
}

int svlGenericFilter::ProcessFrame(ProcInfo* procInfo, svlSample* inputdata)
{
    int ret = SVL_OK;

    _OnSingleThread(procInfo)
    {
        // Try to maintain source sample frequency
        if (GetInputType() == svlTypeStreamSource) {
            if (FrameCounter > 0) {
                double time = Timer.GetElapsedTime();
                double t1 = ulFrameTime * FrameCounter;
                double t2 = time - ulStartTime;
                if (t1 > t2) osaSleep(t1 - t2);
            }
            else {
                ulStartTime = Timer.GetElapsedTime();
            }
        }

        // Call process callback
        ret = CallbackObj->ProcessCallback(inputdata, OutputData);
    }

    return ret;
}

int svlGenericFilter::Release()
{
    // Stop source timer
    if (GetInputType() == svlTypeStreamSource) Timer.Stop();

    // Call release callback
    if (CallbackObj) CallbackObj->ReleaseCallback();

    return SVL_OK;
}

