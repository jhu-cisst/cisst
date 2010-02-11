/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlImageFileWriter.cpp 545 2009-07-14 08:38:33Z adeguet1 $
  
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

#include <cisstStereoVision/svlFilterImageFileWriter.h>
#include <cisstStereoVision/svlConverters.h>

#include <string.h>


/***************************************/
/*** svlFilterImageFileWriter class ****/
/***************************************/

CMN_IMPLEMENT_SERVICES(svlFilterImageFileWriter)

svlFilterImageFileWriter::svlFilterImageFileWriter() :
    svlFilterBase(),
    cmnGenericObject(),
    TimestampsEnabled(false)
{
    AddSupportedType(svlTypeImageRGB, svlTypeImageRGB);
    AddSupportedType(svlTypeImageRGBStereo, svlTypeImageRGBStereo);
    AddSupportedType(svlTypeImageMonoFloat, svlTypeImageMonoFloat);

    ImageCodec.SetSize(2);
    FilePathPrefix.SetSize(2);
    Extension.SetSize(2);
    Disabled.SetSize(2);
    Compression.SetSize(2);
    ImageCodec.SetAll(0);
    Disabled.SetAll(false);

    DistanceScaling = 1.0f;
    // Continuous saving by default
    CaptureLength = -1;
}

svlFilterImageFileWriter::~svlFilterImageFileWriter()
{
    Release();
}

int svlFilterImageFileWriter::Initialize(svlSample* inputdata)
{
    Release();

    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(inputdata);
    unsigned int videochannels = img->GetVideoChannels();

    for (unsigned int i = 0; i < videochannels; i ++) {
        // checking extensions
        // creating image file objects
        ImageCodec[i] = svlImageIO::GetCodec("." + Extension[i]);
        if (ImageCodec[i] == 0) {
            Release();
            return SVL_FAIL;
        }
    }

    if (inputdata->GetType() == svlTypeImageMonoFloat) {
        ImageBuffer.SetSize(*inputdata);
    }

    OutputData = inputdata;

    return SVL_OK;
}

int svlFilterImageFileWriter::ProcessFrame(ProcInfo* procInfo, svlSample* inputdata)
{
    // Passing the same image for the next filter
    OutputData = inputdata;

    if (CaptureLength == 0) return SVL_OK;

    svlSampleImageBase* tosave = 0;
    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(OutputData);
    unsigned int videochannels = img->GetVideoChannels();
    std::stringstream path;
    unsigned int idx;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        if (Disabled[idx]) continue;

        if (img->GetType() == svlTypeImageMonoFloat) {
            svlConverter::float32toRGB24(reinterpret_cast<float*>(img->GetUCharPointer(idx)),
                                         ImageBuffer.GetUCharPointer(idx),
                                         img->GetWidth(idx) * img->GetHeight(idx),
                                         DistanceScaling);
            tosave = &ImageBuffer;
        }
        else {
            tosave = img;
        }

        path << FilePathPrefix[idx];

        if (TimestampsEnabled) {
            path.precision(3);
            path << std::fixed << inputdata->GetTimestamp();
        }
        else {
            path.fill('0');
            path << std::setw(7) << FrameCounter << std::setw(1);
        }

        path << "." << Extension[idx];

        if (ImageCodec[idx]->Write(*tosave, idx, path.str(), Compression[idx]) != SVL_OK) return SVL_FAIL;
    }

    _SynchronizeThreads(procInfo);

    _OnSingleThread(procInfo)
    {
        if (CaptureLength > 0) CaptureLength --;
    }

    return SVL_OK;
}

int svlFilterImageFileWriter::Release()
{
    for (unsigned int i = 0; i < ImageCodec.size(); i ++) {
        svlImageIO::ReleaseCodec(ImageCodec[i]);
        ImageCodec[i] = 0;
    }

    return SVL_OK;
}

int svlFilterImageFileWriter::Disable(bool disable, int videoch)
{
    if (IsInitialized() == true)
        return SVL_FAIL;
    if (videoch >= static_cast<int>(Disabled.size()))
        return SVL_WRONG_CHANNEL;

    Disabled[videoch] = disable;

    return SVL_OK;
}

int svlFilterImageFileWriter::SetFilePath(const std::string & filepathprefix, const std::string & extension, int videoch)
{
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;
    if (videoch >= static_cast<int>(ImageCodec.size()))
        return SVL_WRONG_CHANNEL;

    // checking if file extension is supported
    svlImageIO::ReleaseCodec(ImageCodec[videoch]);
    ImageCodec[videoch] = svlImageIO::GetCodec("." + extension);
    if (ImageCodec[videoch] == 0) return SVL_FAIL;
    svlImageIO::ReleaseCodec(ImageCodec[videoch]);
    ImageCodec[videoch] = 0;

    FilePathPrefix[videoch] = filepathprefix;
    Extension[videoch] = extension;

    return SVL_OK;
}

int svlFilterImageFileWriter::SetCompression(int compression, int videoch)
{
    if (videoch >= static_cast<int>(ImageCodec.size()))
        return SVL_WRONG_CHANNEL;

    Compression[videoch] = compression;

    return SVL_OK;
}

void svlFilterImageFileWriter::EnableTimestamps(bool enable)
{
    TimestampsEnabled = enable;
}

void svlFilterImageFileWriter::Pause()
{
    CaptureLength = 0;
}

void svlFilterImageFileWriter::Record(int frames)
{
    CaptureLength = frames;
}

void svlFilterImageFileWriter::SetDistanceIntensityRatio(float ratio)
{
    DistanceScaling = ratio;
}

float svlFilterImageFileWriter::GetDistanceIntensityRatio()
{
    return DistanceScaling;
}

