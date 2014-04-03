/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2008 

  (C) Copyright 2006-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlFilterImageColorConverter.h>
#include <cisstStereoVision/svlConverters.h>


/*******************************************/
/*** svlFilterImageColorConverter class ****/
/*******************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterImageColorConverter, svlFilterBase)

svlFilterImageColorConverter::svlFilterImageColorConverter() :
    svlFilterBase()

{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBStereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);

    InputColorSpace = OutputColorSpace = svlColorSpaceRGB;
    Channel1 = Channel2 = Channel3 = true;
}

int svlFilterImageColorConverter::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    return SVL_OK;
}

int svlFilterImageColorConverter::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);
    _SkipIfDisabled();

    if (InputColorSpace == OutputColorSpace) return SVL_OK;

    svlSampleImage* img = dynamic_cast<svlSampleImage*>(syncInput);
    unsigned int videochannels = img->GetVideoChannels();
    unsigned int idx;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        ConvertColorSpace(img->GetUCharPointer(idx), img->GetWidth(idx) * img->GetHeight(idx));
    }

    return SVL_OK;
}

void svlFilterImageColorConverter::SetConversion(svlColorSpace input, svlColorSpace output)
{
    InputColorSpace = input;
    OutputColorSpace = output;
}

void svlFilterImageColorConverter::SetChannelMask(bool ch1, bool ch2, bool ch3)
{
    Channel1 = ch1;
    Channel2 = ch2;
    Channel3 = ch3;
}

void svlFilterImageColorConverter::ConvertColorSpace(unsigned char* buffer, unsigned int numofpixels)
{
    // Processing the input image directly
    switch (InputColorSpace) {
        case svlColorSpaceRGB:
            switch (OutputColorSpace) {
                case svlColorSpaceRGB:
                    // NOP
                break;

                case svlColorSpaceHSV:
                    svlConverter::RGB24toHSV24(buffer, buffer, numofpixels, Channel1, Channel2, Channel3);
                break;

                case svlColorSpaceHSL:
                    svlConverter::RGB24toHSL24(buffer, buffer, numofpixels, Channel1, Channel2, Channel3);
                break;

                case svlColorSpaceYUV:
                    svlConverter::RGB24toYUV444(buffer, buffer, numofpixels, Channel1, Channel2, Channel3);
                break;
            }
        break;

        case svlColorSpaceHSV:
            switch (OutputColorSpace) {
                case svlColorSpaceRGB:
                    // TODO
                break;

                case svlColorSpaceHSV:
                    // NOP
                break;

                case svlColorSpaceHSL:
                    // TODO
                break;

                case svlColorSpaceYUV:
                    // TODO
                break;
            }
        break;

        case svlColorSpaceHSL:
            switch (OutputColorSpace) {
                case svlColorSpaceRGB:
                    // TODO
                break;

                case svlColorSpaceHSV:
                    // TODO
                break;

                case svlColorSpaceHSL:
                    // NOP
                break;

                case svlColorSpaceYUV:
                    // TODO
                break;
            }
        break;

        case svlColorSpaceYUV:
            switch (OutputColorSpace) {
                case svlColorSpaceRGB:
                    // TODO
                break;

                case svlColorSpaceHSV:
                    // TODO
                break;

                case svlColorSpaceHSL:
                    // TODO
                break;

                case svlColorSpaceYUV:
                    // NOP
                break;
            }
        break;
    }
}

