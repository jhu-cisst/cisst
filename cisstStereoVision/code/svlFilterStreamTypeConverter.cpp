/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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
#include <cisstStereoVision/svlFilterInput.h>
#include <cisstStereoVision/svlFilterOutput.h>


/******************************************/
/*** svlFilterStreamTypeConverter class ***/
/******************************************/

CMN_IMPLEMENT_SERVICES(svlFilterStreamTypeConverter)

svlFilterStreamTypeConverter::svlFilterStreamTypeConverter() :
    svlFilterBase(),
    OutputSample(0),
    Scaling(1.0f),
    Mono16ShiftDown(8)
{
}

svlFilterStreamTypeConverter::svlFilterStreamTypeConverter(svlStreamType inputtype, svlStreamType outputtype) :
    svlFilterBase(),
    OutputSample(0),
    Scaling(1.0f),
    Mono16ShiftDown(8)
{
    SetType(inputtype, outputtype);
}

svlFilterStreamTypeConverter::~svlFilterStreamTypeConverter()
{
    if (OutputSample) delete OutputSample;
}

int svlFilterStreamTypeConverter::SetType(svlStreamType inputtype, svlStreamType outputtype)
{
    if (OutputSample == 0) {

        if (inputtype == outputtype) return SVL_FAIL;

    ///////////////////////////////////////////////////////////////////
    // Single channel image mappings:
        if (((inputtype == svlTypeImageRGB    ||
              inputtype == svlTypeImageRGBA   ||
              inputtype == svlTypeImageMono8  ||
              inputtype == svlTypeImageMono16 ||
              inputtype == svlTypeImageMono32) &&
                 (outputtype == svlTypeImageRGB    ||
                  outputtype == svlTypeImageRGBA   ||
                  outputtype == svlTypeImageMono8  ||
                  outputtype == svlTypeImageMono16 ||
                  outputtype == svlTypeImageMono32))      ||
    ///////////////////////////////////////////////////////////////////
    // Stereo image mappings:
            ((inputtype == svlTypeImageRGBStereo    ||
              inputtype == svlTypeImageRGBAStereo   ||
              inputtype == svlTypeImageMono8Stereo  ||
              inputtype == svlTypeImageMono16Stereo ||
              inputtype == svlTypeImageMono32Stereo) &&
                (outputtype == svlTypeImageRGBStereo    ||
                 outputtype == svlTypeImageRGBAStereo   ||
                 outputtype == svlTypeImageMono8Stereo  ||
                 outputtype == svlTypeImageMono16Stereo ||
                 outputtype == svlTypeImageMono32Stereo)) ||
    ///////////////////////////////////////////////////////////////////
    // Matrix mappings:
            ((inputtype == svlTypeMatrixInt8   ||
              inputtype == svlTypeMatrixInt16  ||
              inputtype == svlTypeMatrixInt32  ||
              inputtype == svlTypeMatrixInt64  ||
              inputtype == svlTypeMatrixUInt8  ||
              inputtype == svlTypeMatrixUInt16 ||
              inputtype == svlTypeMatrixUInt32 ||
              inputtype == svlTypeMatrixUInt64 ||
              inputtype == svlTypeMatrixFloat  ||
              inputtype == svlTypeMatrixDouble) &&
                (outputtype == svlTypeMatrixInt8   ||
                 outputtype == svlTypeMatrixInt16  ||
                 outputtype == svlTypeMatrixInt32  ||
                 outputtype == svlTypeMatrixInt64  ||
                 outputtype == svlTypeMatrixUInt8  ||
                 outputtype == svlTypeMatrixUInt16 ||
                 outputtype == svlTypeMatrixUInt32 ||
                 outputtype == svlTypeMatrixUInt64 ||
                 outputtype == svlTypeMatrixFloat  ||
                 outputtype == svlTypeMatrixDouble))      ||
    ///////////////////////////////////////////////////////////////////
    // Matrix-to-image mappings:
            ((inputtype == svlTypeMatrixUInt8  ||
              inputtype == svlTypeMatrixUInt16 ||
              inputtype == svlTypeMatrixUInt32 ||
              inputtype == svlTypeMatrixFloat) &&
                 (outputtype == svlTypeImageRGB    ||
                  outputtype == svlTypeImageRGBA   ||
                  outputtype == svlTypeImageMono8  ||
                  outputtype == svlTypeImageMono16 ||
                  outputtype == svlTypeImageMono32))      ||
    ///////////////////////////////////////////////////////////////////
    // Image-to-matrix mappings:
            ((inputtype == svlTypeImageRGB    ||
              inputtype == svlTypeImageRGBA   ||
              inputtype == svlTypeImageMono8  ||
              inputtype == svlTypeImageMono16 ||
              inputtype == svlTypeImageMono32) &&
                 (outputtype == svlTypeMatrixInt8   ||
                  outputtype == svlTypeMatrixInt16  ||
                  outputtype == svlTypeMatrixInt32  ||
                  outputtype == svlTypeMatrixInt64  ||
                  outputtype == svlTypeMatrixUInt8  ||
                  outputtype == svlTypeMatrixUInt16 ||
                  outputtype == svlTypeMatrixUInt32 ||
                  outputtype == svlTypeMatrixUInt64 ||
                  outputtype == svlTypeMatrixFloat  ||
                  outputtype == svlTypeMatrixDouble))) {

            // mapping input type to output type
            AddInput("input", true);
            AddInputType("input", inputtype);

            AddOutput("output", true);
            SetAutomaticOutputType(false);
            GetOutput()->SetType(outputtype);

            // initializing output sample
            OutputSample = svlSample::GetNewFromType(outputtype);

            return SVL_OK;
        }

#if CISST_SVL_HAS_CUDA
    ///////////////////////////////////////////////////////////////////
    // CUDA image mappings:
        if ((inputtype == svlTypeImageRGB          && outputtype == svlTypeCUDAImageRGB)          ||
            (inputtype == svlTypeImageRGBA         && outputtype == svlTypeCUDAImageRGBA)         ||
            (inputtype == svlTypeImageMono8        && outputtype == svlTypeCUDAImageMono8)        ||
            (inputtype == svlTypeImageMono16       && outputtype == svlTypeCUDAImageMono16)       ||
            (inputtype == svlTypeImageMono32       && outputtype == svlTypeCUDAImageMono32)       ||
            (inputtype == svlTypeImageRGBStereo    && outputtype == svlTypeCUDAImageRGBStereo)    ||
            (inputtype == svlTypeImageRGBAStereo   && outputtype == svlTypeCUDAImageRGBAStereo)   ||
            (inputtype == svlTypeImageMono8Stereo  && outputtype == svlTypeCUDAImageMono8Stereo)  ||
            (inputtype == svlTypeImageMono16Stereo && outputtype == svlTypeCUDAImageMono16Stereo) ||
            (inputtype == svlTypeImageMono32Stereo && outputtype == svlTypeCUDAImageMono32Stereo) ||
            (inputtype == svlTypeCUDAImageRGB          && outputtype == svlTypeImageRGB)          ||
            (inputtype == svlTypeCUDAImageRGBA         && outputtype == svlTypeImageRGBA)         ||
            (inputtype == svlTypeCUDAImageMono8        && outputtype == svlTypeImageMono8)        ||
            (inputtype == svlTypeCUDAImageMono16       && outputtype == svlTypeImageMono16)       ||
            (inputtype == svlTypeCUDAImageMono32       && outputtype == svlTypeImageMono32)       ||
            (inputtype == svlTypeCUDAImageRGBStereo    && outputtype == svlTypeImageRGBStereo)    ||
            (inputtype == svlTypeCUDAImageRGBAStereo   && outputtype == svlTypeImageRGBAStereo)   ||
            (inputtype == svlTypeCUDAImageMono8Stereo  && outputtype == svlTypeImageMono8Stereo)  ||
            (inputtype == svlTypeCUDAImageMono16Stereo && outputtype == svlTypeImageMono16Stereo) ||
            (inputtype == svlTypeCUDAImageMono32Stereo && outputtype == svlTypeImageMono32Stereo)) {

            // mapping input type to output type
            AddInput("input", true);
            AddInputType("input", inputtype);

            AddOutput("output", true);
            SetAutomaticOutputType(false);
            GetOutput()->SetType(outputtype);

            // initializing output sample
            OutputSample = svlSample::GetNewFromType(outputtype);

            return SVL_OK;
        }
#endif // CISST_SVL_HAS_CUDA

        // Otherwise, the filter will fail to initialize.
    }

    return SVL_FAIL;
}

int svlFilterStreamTypeConverter::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    if (OutputSample == 0) return SVL_FAIL;
    syncOutput = OutputSample;

    OutputSample->SetSize(*syncInput);

    return SVL_OK;
}

int svlFilterStreamTypeConverter::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = OutputSample;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);

#if CISST_SVL_HAS_CUDA
    svlSampleCUDAImage* cudaimg = 0;
    cudaimg = dynamic_cast<svlSampleCUDAImage*>(OutputSample);
    if (cudaimg) {
        cudaimg->CopyOf(syncInput);
        return SVL_OK;
    }
    cudaimg = dynamic_cast<svlSampleCUDAImage*>(syncInput);
    if (cudaimg) {
        cudaimg->CopyTo(OutputSample);
        return SVL_OK;
    }
#endif // CISST_SVL_HAS_CUDA

    svlSampleImage* inimg  = dynamic_cast<svlSampleImage*>(syncInput);
    svlSampleImage* outimg = dynamic_cast<svlSampleImage*>(OutputSample);
    svlSampleMatrix* inmtrx  = dynamic_cast<svlSampleMatrix*>(syncInput);
    svlSampleMatrix* outmtrx = dynamic_cast<svlSampleMatrix*>(OutputSample);

    if (outimg) {
        svlStreamType inputtype = GetInput()->GetType();
        int param = 0;

        if (inimg) {
        //////////////////////////////////////////////////////////
        // Both the input and the output are images

            if (inputtype == svlTypeImage3DMap) {
                param = static_cast<int>(Scaling * 1000.0);
            }
            else if (inputtype == svlTypeImageMono16 || inputtype == svlTypeImageMono16Stereo) {
                param = Mono16ShiftDown;
            }

            svlConverter::ConvertImage(dynamic_cast<svlSampleImage*>(syncInput),
                                       dynamic_cast<svlSampleImage*>(OutputSample),
                                       param,
                                       procInfo->count, procInfo->ID);

            return SVL_OK;
        }
        else if (inmtrx) {
        ////////////////////////////////////////////////////////////////////
        // The output is image and the input is matrix

            _OnSingleThread(procInfo) {
                if (inputtype == svlTypeMatrixFloat) {
                    param = static_cast<int>(Scaling * 1000.0);
                }
                outimg->ImportMatrix(inmtrx, param);
            }
            return SVL_OK;
        }
    }

    if (outmtrx) {
        ////////////////////////////////////////////////////////////////////
        // The output is matrix and the input is either image or matrix

        if (inmtrx) {
            _OnSingleThread(procInfo) {
                outmtrx->ImportMatrix(inmtrx);
            }
            return SVL_OK;
        }
        else if (inimg) {
            _OnSingleThread(procInfo) {
                outmtrx->ImportImage(inimg);
            }
            return SVL_OK;
        }
    }

    return SVL_FAIL;
}

