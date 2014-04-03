/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
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

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterImageFileWriter, svlFilterBase)

svlFilterImageFileWriter::svlFilterImageFileWriter() :
    svlFilterBase(),
    TimestampsEnabled(false)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBStereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);

    ImageCodec.SetSize(2);
    FilePathPrefix.SetSize(2);
    Extension.SetSize(2);
    Disabled.SetSize(2);
    Compression.SetSize(2);
    ImageCodec.SetAll(0);
    Disabled.SetAll(false);

    // Continuous saving by default
    CaptureLength = -1;
}

svlFilterImageFileWriter::~svlFilterImageFileWriter()
{
    Release();
}

int svlFilterImageFileWriter::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    Release();

    svlSampleImage* img = dynamic_cast<svlSampleImage*>(syncInput);
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

    syncOutput = syncInput;

    return SVL_OK;
}

int svlFilterImageFileWriter::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    _SkipIfDisabled();

    if (CaptureLength == 0) return SVL_OK;

    svlSampleImage* img = dynamic_cast<svlSampleImage*>(syncOutput);
    unsigned int videochannels = img->GetVideoChannels();
    unsigned int idx;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        if (Disabled[idx]) continue;

        std::stringstream path;
        path << FilePathPrefix[idx];

        if (TimestampsEnabled) {
            path.precision(3);
            path << std::fixed << syncInput->GetTimestamp();
        }
        else {
            path.fill('0');
            path << std::setw(7) << FrameCounter << std::setw(1);
        }

        path << "." << Extension[idx];

        if (ImageCodec[idx]->Write(*img, idx, path.str(), Compression[idx]) != SVL_OK) return SVL_FAIL;
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

