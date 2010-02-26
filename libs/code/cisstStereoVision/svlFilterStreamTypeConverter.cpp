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

#include <cisstStereoVision/svlFilterStreamTypeConverter.h>
#include <cisstStereoVision/svlConverters.h>

using namespace std;


/*******************************************/
/*** svlFilterStreamTypeConverter class ****/
/*******************************************/

CMN_IMPLEMENT_SERVICES(svlFilterStreamTypeConverter)

svlFilterStreamTypeConverter::svlFilterStreamTypeConverter() :
    svlFilterBase(),
    cmnGenericObject(),
    Scaling(1.0f),
    Mono16ShiftDown(8)
{
    OutputData = 0;
}

svlFilterStreamTypeConverter::svlFilterStreamTypeConverter(svlStreamType inputtype, svlStreamType outputtype) :
    svlFilterBase(),
    cmnGenericObject(),
    Scaling(1.0f),
    Mono16ShiftDown(8)
{
    OutputData = 0;

    SetType(inputtype, outputtype);
}

svlFilterStreamTypeConverter::~svlFilterStreamTypeConverter()
{
    Release();

    if (OutputData) delete OutputData;
}

int svlFilterStreamTypeConverter::SetType(svlStreamType inputtype, svlStreamType outputtype)
{
    if (OutputData == 0) {

        ////////////////////////////////////////////////////
        // Valid input->output mappings are:
        //          svlTypeImageRGB          -> svlTypeImageRGBA
        //          svlTypeImageRGB          -> svlTypeImageMono8
        //          svlTypeImageRGB          -> svlTypeImageMono16
        //          svlTypeImageMono8        -> svlTypeImageRGB
        //          svlTypeImageMono8        -> svlTypeImageRGBA
        //          svlTypeImageMono8        -> svlTypeImage16
        //          svlTypeImageMono16       -> svlTypeImageRGB
        //          svlTypeImageMono16       -> svlTypeImageRGBA
        //          svlTypeImageMono16       -> svlTypeImage8
        //          svlTypeImageRGBStereo    -> svlTypeImageRGBAStereo,
        //          svlTypeImageRGBStereo    -> svlTypeImageMono8Stereo,
        //          svlTypeImageRGBStereo    -> svlTypeImageMono16Stereo,
        //          svlTypeImageMono8Stereo  -> svlTypeImageRGBStereo,
        //          svlTypeImageMono8Stereo  -> svlTypeImageRGBAStereo,
        //          svlTypeImageMono8Stereo  -> svlTypeImageMono16Stereo,
        //          svlTypeImageMono16Stereo -> svlTypeImageRGBStereo,
        //          svlTypeImageMono16Stereo -> svlTypeImageRGBAStereo,
        //          svlTypeImageMono16Stereo -> svlTypeImageMono8Stereo,
        //          svlTypeImageMonoFloat    -> svlTypeImageMono8
        //          svlTypeImageMonoFloat    -> svlTypeImageMono16
        //          svlTypeImageMonoFloat    -> svlTypeImageRGB
        //          svlTypeImageMonoFloat    -> svlTypeImageRGBA
        //          svlTypeImage3DMap        -> svlTypeImageMono8
        //          svlTypeImage3DMap        -> svlTypeImageMono16
        //          svlTypeImage3DMap        -> svlTypeImageRGB
        //          svlTypeImage3DMap        -> svlTypeImageRGBA
        // Otherwise, the filter will fail to initialize.
        //
        if ((inputtype == svlTypeImageRGB          && outputtype == svlTypeImageRGBA) ||
            (inputtype == svlTypeImageRGB          && outputtype == svlTypeImageMono8) ||
            (inputtype == svlTypeImageRGB          && outputtype == svlTypeImageMono16) ||
            (inputtype == svlTypeImageMono8        && outputtype == svlTypeImageRGB) ||
            (inputtype == svlTypeImageMono8        && outputtype == svlTypeImageRGBA) ||
            (inputtype == svlTypeImageMono8        && outputtype == svlTypeImageMono16) ||
            (inputtype == svlTypeImageMono16       && outputtype == svlTypeImageRGB) ||
            (inputtype == svlTypeImageMono16       && outputtype == svlTypeImageRGBA) ||
            (inputtype == svlTypeImageMono16       && outputtype == svlTypeImageMono8) ||
            (inputtype == svlTypeImageRGBStereo    && outputtype == svlTypeImageRGBAStereo) ||
            (inputtype == svlTypeImageRGBStereo    && outputtype == svlTypeImageMono8Stereo) ||
            (inputtype == svlTypeImageRGBStereo    && outputtype == svlTypeImageMono16Stereo) ||
            (inputtype == svlTypeImageMono8Stereo  && outputtype == svlTypeImageRGBStereo) ||
            (inputtype == svlTypeImageMono8Stereo  && outputtype == svlTypeImageRGBAStereo) ||
            (inputtype == svlTypeImageMono8Stereo  && outputtype == svlTypeImageMono16Stereo) ||
            (inputtype == svlTypeImageMono16Stereo && outputtype == svlTypeImageRGBStereo) ||
            (inputtype == svlTypeImageMono16Stereo && outputtype == svlTypeImageRGBAStereo) ||
            (inputtype == svlTypeImageMono16Stereo && outputtype == svlTypeImageMono8Stereo) ||
            (inputtype == svlTypeImageMonoFloat    && outputtype == svlTypeImageMono8) ||
            (inputtype == svlTypeImageMonoFloat    && outputtype == svlTypeImageMono16) ||
            (inputtype == svlTypeImageMonoFloat    && outputtype == svlTypeImageRGB) ||
            (inputtype == svlTypeImageMonoFloat    && outputtype == svlTypeImageRGBA) ||
            (inputtype == svlTypeImage3DMap        && outputtype == svlTypeImageMono8) ||
            (inputtype == svlTypeImage3DMap        && outputtype == svlTypeImageMono16) ||
            (inputtype == svlTypeImage3DMap        && outputtype == svlTypeImageRGB) ||
            (inputtype == svlTypeImage3DMap        && outputtype == svlTypeImageRGBA)) {

            // mapping input type to output type
            AddSupportedType(inputtype, outputtype);

            // initializing output sample
            OutputData = svlSample::GetNewFromType(outputtype);

            return SVL_OK;
        }
    }

    return SVL_FAIL;
}

int svlFilterStreamTypeConverter::Initialize(svlSample* inputdata)
{
    if (OutputData == 0) return SVL_FAIL;

    Release();

    OutputData->SetSize(*inputdata);

    return SVL_OK;
}

int svlFilterStreamTypeConverter::ProcessFrame(svlProcInfo* procInfo, svlSample* inputdata)
{
    ///////////////////////////////////////////
    // Check if the input sample has changed //
      if (!IsNewSample(inputdata))
          return SVL_ALREADY_PROCESSED;
    ///////////////////////////////////////////

    int param = 0;
    svlStreamType inputtype = GetInputType();
    if (inputtype == svlTypeImageMonoFloat ||
        inputtype == svlTypeImage3DMap) param = static_cast<int>(Scaling * 1000.0);
    else
    if (inputtype == svlTypeImageMono16 ||
        inputtype == svlTypeImageMono16Stereo) param = Mono16ShiftDown;

    svlConverter::ConvertImage(dynamic_cast<svlSampleImageBase*>(inputdata),
                               dynamic_cast<svlSampleImageBase*>(OutputData),
                               param,
                               procInfo->count, procInfo->id);

    return SVL_OK;
}

int svlFilterStreamTypeConverter::Release()
{
    return SVL_OK;
}

