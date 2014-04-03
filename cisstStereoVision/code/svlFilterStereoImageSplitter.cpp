/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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

#include <cisstStereoVision/svlFilterStereoImageSplitter.h>
#include <cisstStereoVision/svlFilterInput.h>
#include <cisstStereoVision/svlFilterOutput.h>

/******************************************/
/*** svlFilterStereoImageSplitter class ***/
/******************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterStereoImageSplitter, svlFilterBase)

svlFilterStereoImageSplitter::svlFilterStereoImageSplitter() :
    svlFilterBase(),
    OutputImage(0),
    Layout(svlLayoutSideBySide)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageMono8);
    AddInputType("input", svlTypeImageMono16);
    AddInputType("input", svlTypeImageMono32);

    AddOutput("output", true);
    SetAutomaticOutputType(false);
}

svlFilterStereoImageSplitter::~svlFilterStereoImageSplitter()
{
    Release();
}

int svlFilterStereoImageSplitter::OnConnectInput(svlFilterInput &input, svlStreamType type)
{
    // Check if type is on the supported list
    if (!input.IsTypeSupported(type)) return SVL_FAIL;

    if      (type == svlTypeImageRGB)    GetOutput()->SetType(svlTypeImageRGBStereo);
    else if (type == svlTypeImageMono8)  GetOutput()->SetType(svlTypeImageMono8Stereo);
    else if (type == svlTypeImageMono16) GetOutput()->SetType(svlTypeImageMono16Stereo);
    else if (type == svlTypeImageMono32) GetOutput()->SetType(svlTypeImageMono32Stereo);
    else return SVL_FAIL;

    return SVL_OK;
}

int svlFilterStereoImageSplitter::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    svlSampleImage* input = dynamic_cast<svlSampleImage*>(syncInput);
    unsigned int width = input->GetWidth();
    unsigned int height = input->GetHeight();

    switch (Layout) {
        case svlLayoutInterlaced:
        case svlLayoutInterlacedRL:
            if ((height % 2) != 0) return SVL_FAIL;
            height = input->GetHeight() / 2;
        break;

        case svlLayoutSideBySide:
        case svlLayoutSideBySideRL:
            if ((width % 2) != 0) return SVL_FAIL;
            width = input->GetWidth() / 2;
        break;

        default:
            return SVL_FAIL;
    }

    Release();

    if      (GetInput()->GetType() == svlTypeImageRGB)    OutputImage = new svlSampleImageRGBStereo;
    else if (GetInput()->GetType() == svlTypeImageMono8)  OutputImage = new svlSampleImageMono8Stereo;
    else if (GetInput()->GetType() == svlTypeImageMono16) OutputImage = new svlSampleImageMono16Stereo;
    else if (GetInput()->GetType() == svlTypeImageMono32) OutputImage = new svlSampleImageMono32Stereo;
    else return SVL_FAIL;
    OutputImage->SetSize(width, height);

    syncOutput = OutputImage;

    return SVL_OK;
}

int svlFilterStereoImageSplitter::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = OutputImage;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);

    _OnSingleThread(procInfo)
    {
        svlSampleImage* id = dynamic_cast<svlSampleImage*>(syncInput);
        unsigned int stride = OutputImage->GetWidth(SVL_LEFT) * OutputImage->GetBPP();
        unsigned int height = OutputImage->GetHeight(SVL_LEFT);
        unsigned char *input = id->GetUCharPointer();
        unsigned char *output1 = OutputImage->GetUCharPointer(SVL_LEFT);
        unsigned char *output2 = OutputImage->GetUCharPointer(SVL_RIGHT);

        // copy data
        switch (Layout) {
            case svlLayoutInterlaced:
            case svlLayoutSideBySide:
                for (unsigned int j = 0; j < height; j ++) {
                    memcpy(output1, input, stride);
                    input += stride;
                    output1 += stride;
                    memcpy(output2, input, stride);
                    input += stride;
                    output2 += stride;
                }
            break;

            case svlLayoutInterlacedRL:
            case svlLayoutSideBySideRL:
                for (unsigned int j = 0; j < height; j ++) {
                    memcpy(output2, input, stride);
                    input += stride;
                    output2 += stride;
                    memcpy(output1, input, stride);
                    input += stride;
                    output1 += stride;
                }
            break;

            default:
                return SVL_FAIL;
        }
    }

    return SVL_OK;
}

int svlFilterStereoImageSplitter::Release()
{
    if (OutputImage) {
        delete OutputImage;
        OutputImage = 0;
    }
    return SVL_OK;
}

int svlFilterStereoImageSplitter::SetLayout(svlStereoLayout layout)
{
    if (IsInitialized()) return SVL_FAIL;
    Layout = layout;
    return SVL_OK;
}

svlStereoLayout svlFilterStereoImageSplitter::GetLayout()
{
    return Layout;
}

