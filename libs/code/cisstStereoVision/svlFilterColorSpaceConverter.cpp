/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlColorSpaceConverter.cpp 319 2009-05-06 17:40:46Z bvagvol1 $
  
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

#include <cisstStereoVision/svlFilterColorSpaceConverter.h>
#include "svlConverters.h"

using namespace std;

/*******************************************/
/*** svlFilterColorSpaceConverter class ****/
/*******************************************/

svlFilterColorSpaceConverter::svlFilterColorSpaceConverter() : svlFilterBase()
{
    AddSupportedType(svlTypeImageRGB, svlTypeImageRGB);
    AddSupportedType(svlTypeImageRGBStereo, svlTypeImageRGBStereo);
    InputColorSpace = OutputColorSpace = svlColorSpaceRGB;
    Channel1 = Channel2 = Channel3 = true;
}

svlFilterColorSpaceConverter::~svlFilterColorSpaceConverter()
{
    Release();
}

int svlFilterColorSpaceConverter::Initialize(svlSample* inputdata)
{
    Release();

    OutputData = inputdata;

    return SVL_OK;
}

int svlFilterColorSpaceConverter::ProcessFrame(ProcInfo* procInfo, svlSample* inputdata)
{
    ///////////////////////////////////////////
    // Check if the input sample has changed //
      if (!IsNewSample(inputdata))
          return SVL_ALREADY_PROCESSED;
    ///////////////////////////////////////////

    // Passing the same image for the next filter
    OutputData = inputdata;

    if (InputColorSpace == OutputColorSpace) return SVL_OK;

    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(inputdata);
    unsigned int videochannels = img->GetVideoChannels();
    unsigned int idx;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        ConvertColorSpace(reinterpret_cast<unsigned char*>(img->GetPointer(idx)), img->GetWidth(idx) * img->GetHeight(idx));
    }

    return SVL_OK;
}

int svlFilterColorSpaceConverter::Release()
{
    return SVL_OK;
}

void svlFilterColorSpaceConverter::SetConversion(svlColorSpace input, svlColorSpace output)
{
    InputColorSpace = input;
    OutputColorSpace = output;
}

void svlFilterColorSpaceConverter::SetChannelMask(bool ch1, bool ch2, bool ch3)
{
    Channel1 = ch1;
    Channel2 = ch2;
    Channel3 = ch3;
}

void svlFilterColorSpaceConverter::ConvertColorSpace(unsigned char* buffer, unsigned int numofpixels)
{
    // Processing the input image directly
    switch (InputColorSpace) {
        case svlColorSpaceRGB:
            switch (OutputColorSpace) {
                case svlColorSpaceRGB:
                    // NOP
                break;

                case svlColorSpaceHSV:
                    RGB24toHSV24(buffer, buffer, numofpixels, Channel1, Channel2, Channel3);
                break;

                case svlColorSpaceHSL:
                    RGB24toHSL24(buffer, buffer, numofpixels, Channel1, Channel2, Channel3);
                break;

                case svlColorSpaceYUV:
                    RGB24toYUV444(buffer, buffer, numofpixels, Channel1, Channel2, Channel3);
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

