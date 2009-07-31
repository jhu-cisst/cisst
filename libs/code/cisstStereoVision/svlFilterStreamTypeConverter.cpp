/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlStreamTypeConverter.cpp 75 2009-02-24 16:47:20Z adeguet1 $
  
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

#include <cisstStereoVision/svlFilterStreamTypeConverter.h>
#include "svlConverters.h"

using namespace std;

/*******************************************/
/*** svlFilterStreamTypeConverter class ****/
/*******************************************/

////////////////////////////////////////////////////
// Valid input->output mappings are:
//          svlTypeImageRGB          -> svlTypeImageMono8
//          svlTypeImageRGB          -> svlTypeImageMono16
//          svlTypeImageMono8        -> svlTypeImageRGB
//          svlTypeImageMono8        -> svlTypeImage16
//          svlTypeImageMono16       -> svlTypeImageRGB
//          svlTypeImageMono16       -> svlTypeImage8
//          svlTypeImageRGBStereo    -> svlTypeImageMono8Stereo,
//          svlTypeImageRGBStereo    -> svlTypeImageMono16Stereo,
//          svlTypeImageMono8Stereo  -> svlTypeImageRGBStereo,
//          svlTypeImageMono8Stereo  -> svlTypeImageMono16Stereo,
//          svlTypeImageMono16Stereo -> svlTypeImageRGBStereo,
//          svlTypeImageMono16Stereo -> svlTypeImageMono8Stereo,
//          svlTypeDepthMap          -> svlTypeImageMono8
//          svlTypeDepthMap          -> svlTypeImageMono16
//          svlTypeDepthMap          -> svlTypeImageRGB
// Otherwise, the filter will fail to initialize.
//
svlFilterStreamTypeConverter::svlFilterStreamTypeConverter(svlStreamType inputtype, svlStreamType outputtype) : svlFilterBase()
{
    if ((inputtype == svlTypeImageRGB          && outputtype == svlTypeImageMono8) ||
        (inputtype == svlTypeImageRGB          && outputtype == svlTypeImageMono16) ||
        (inputtype == svlTypeImageMono8        && outputtype == svlTypeImageRGB) ||
        (inputtype == svlTypeImageMono8        && outputtype == svlTypeImageMono16) ||
        (inputtype == svlTypeImageMono16       && outputtype == svlTypeImageRGB) ||
        (inputtype == svlTypeImageMono16       && outputtype == svlTypeImageMono8) ||
        (inputtype == svlTypeImageRGBStereo    && outputtype == svlTypeImageMono8Stereo) ||
        (inputtype == svlTypeImageRGBStereo    && outputtype == svlTypeImageMono16Stereo) ||
        (inputtype == svlTypeImageMono8Stereo  && outputtype == svlTypeImageRGBStereo) ||
        (inputtype == svlTypeImageMono8Stereo  && outputtype == svlTypeImageMono16Stereo) ||
        (inputtype == svlTypeImageMono16Stereo && outputtype == svlTypeImageRGBStereo) ||
        (inputtype == svlTypeImageMono16Stereo && outputtype == svlTypeImageMono8Stereo) ||
        (inputtype == svlTypeDepthMap          && outputtype == svlTypeImageMono8) ||
        (inputtype == svlTypeDepthMap          && outputtype == svlTypeImageMono16) ||
        (inputtype == svlTypeDepthMap          && outputtype == svlTypeImageRGB)) {

        // mapping input type to output type
        AddSupportedType(inputtype, outputtype);

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

            // Other types may be added in the future
            case svlTypeDepthMap:
            case svlTypeInvalid:
            case svlTypeStreamSource:
            case svlTypeStreamSink:
            case svlTypeImageCustom:
            case svlTypeRigidXform:
            case svlTypePointCloud:
            break;
        }
    }

    DistanceScaling = 1.0f;
    Mono16ShiftDown = 8;
}

svlFilterStreamTypeConverter::~svlFilterStreamTypeConverter()
{
    Release();

    if (OutputData) delete OutputData;
}

int svlFilterStreamTypeConverter::Initialize(svlSample* inputdata)
{
    Release();

    dynamic_cast<svlSampleImageBase*>(OutputData)->SetSize(*(dynamic_cast<svlSampleImageBase*>(inputdata)));

    return SVL_OK;
}

int svlFilterStreamTypeConverter::ProcessFrame(ProcInfo* procInfo, svlSample* inputdata)
{
    ///////////////////////////////////////////
    // Check if the input sample has changed //
      if (!IsNewSample(inputdata))
          return SVL_ALREADY_PROCESSED;
    ///////////////////////////////////////////

    int param = 0;
    svlStreamType inputtype = GetInputType();

    if (inputtype == svlTypeDepthMap) param = static_cast<int>(DistanceScaling * 1000.0);
    else if (inputtype == svlTypeImageMono16 || inputtype == svlTypeImageMono16Stereo) param = Mono16ShiftDown;

    ConvertImage(dynamic_cast<svlSampleImageBase*>(inputdata), dynamic_cast<svlSampleImageBase*>(OutputData), param, procInfo->count, procInfo->id);

    return SVL_OK;
}

int svlFilterStreamTypeConverter::Release()
{
    return SVL_OK;
}

