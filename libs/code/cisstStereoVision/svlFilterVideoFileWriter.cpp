/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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

#include <cisstStereoVision/svlFilterVideoFileWriter.h>
#include <cisstOSAbstraction/osaTimeServer.h>
#include <cisstCommon/cmnGetChar.h>


/******************************************/
/*** svlFilterVideoFileWriter class *******/
/******************************************/

CMN_IMPLEMENT_SERVICES(svlFilterVideoFileWriter)

svlFilterVideoFileWriter::svlFilterVideoFileWriter() :
    svlFilterBase(),
    Action(false),
    ActionTime(0.0),
    TargetActionTime(0.0),
    TargetCaptureLength(-1),
    CaptureLength(-1), // Continuous saving by default
    Framerate(30.0),
    CodecsMultithreaded(false)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBStereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);

    TimeServer = new osaTimeServer;
    TimeServer->SetTimeOrigin();
}

svlFilterVideoFileWriter::~svlFilterVideoFileWriter()
{
    Release();

    for (unsigned int i = 0; i < CodecParams.size(); i ++) {
        svlVideoIO::ReleaseCompression(CodecParams[i]);
    }

    delete TimeServer;
}

int svlFilterVideoFileWriter::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    svlSampleImage* img = dynamic_cast<svlSampleImage*>(syncInput);
    if (Codec.size() < img->GetVideoChannels()) return SVL_FAIL;

    Release();

    CodecsMultithreaded = true;

    for (unsigned int i = 0; i < img->GetVideoChannels(); i ++) {

        if (Disabled[i]) continue;

        // Get video codec for file extension
        Codec[i] = svlVideoIO::GetCodec(FilePath[i]);

        // Open video file
        if (!Codec[i] ||
            (CodecParams[i] && Codec[i]->SetCompression(CodecParams[i]) != SVL_OK) ||
            Codec[i]->Create(FilePath[i], img->GetWidth(i), img->GetHeight(i), Framerate) != SVL_OK) {

            Release();
            return SVL_FAIL;
        }

        if (!Codec[i]->GetMultithreaded()) CodecsMultithreaded = false;
    }

    // Initialize video frame counter
    VideoFrameCounter = 0;

    syncOutput = syncInput;

    return SVL_OK;
}

int svlFilterVideoFileWriter::OnStart(unsigned int CMN_UNUSED(procCount))
{
    CaptureLength = TargetCaptureLength;
    ActionTime = TargetActionTime;
    Action = false;

    return SVL_OK;
}

int svlFilterVideoFileWriter::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    _SkipIfDisabled();

    _OnSingleThread(procInfo) {
        if (Action) {
            CaptureLength = TargetCaptureLength;
            ActionTime = TargetActionTime;
            Action = false;
        }
    }
    _SynchronizeThreads(procInfo);

    if (CaptureLength == 0) {
        if (ActionTime < syncInput->GetTimestamp()) return SVL_OK;
        // Process remaining samples in the buffer when paused
    }
    else {
        // Drop frames when restarted
        if (ActionTime > syncInput->GetTimestamp()) return SVL_OK;
    }

    svlSampleImage* img = dynamic_cast<svlSampleImage*>(syncOutput);
    unsigned int videochannels = img->GetVideoChannels();
    unsigned int idx;
    int ret = SVL_OK;

    if (CodecsMultithreaded) {
        // Codecs are multithreaded, so it's worth
        // splitting work between all threads
        for (idx = 0; idx < videochannels; idx ++) {
            // Codec is responsible for thread synchronzation
            if (Codec[idx] && Codec[idx]->Write(procInfo, *img, idx) != SVL_OK) ret = SVL_FAIL;
        }
    }
    else {
        // Codecs are not multithreaded, so assigning
        // each video channel to a single thread
        _ParallelLoop(procInfo, idx, videochannels)
        {
            if (Codec[idx] && Codec[idx]->Write(0, *img, idx) != SVL_OK) ret = SVL_FAIL;
        }
    }

    _SynchronizeThreads(procInfo);

    _OnSingleThread(procInfo)
    {
        if (CaptureLength > 0) CaptureLength --;
    }

    return ret;
}

int svlFilterVideoFileWriter::Release()
{
    for (unsigned int i = 0; i < Codec.size(); i ++) {
        svlVideoIO::ReleaseCodec(Codec[i]);
        Codec[i] = 0;
    }

    return SVL_OK;
}

int svlFilterVideoFileWriter::Disable(bool disable, unsigned int videoch)
{
    if (IsInitialized() == true)
        return SVL_FAIL;
    if (UpdateStreamCount(videoch + 1) != SVL_OK)
        return SVL_FAIL;

    Disabled[videoch] = disable;

    return SVL_OK;
}

int svlFilterVideoFileWriter::DialogFilePath(unsigned int videoch)
{
    if (IsInitialized() == true)
        return SVL_FAIL;
    if (UpdateStreamCount(videoch + 1) != SVL_OK)
        return SVL_FAIL;

    std::ostringstream out;
    out << "Save video file [channel #" << videoch << "]";
    std::string title(out.str());

    return svlVideoIO::DialogFilePath(true, title, FilePath[videoch]);
}

int svlFilterVideoFileWriter::SetFilePath(const std::string &filepath, unsigned int videoch)
{
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;
    if (UpdateStreamCount(videoch + 1) != SVL_OK)
        return SVL_FAIL;

    FilePath[videoch] = filepath;

    // Set default comression parameters for video channel
    svlVideoCodecBase* codec = svlVideoIO::GetCodec(filepath);
    svlVideoIO::Compression* compression = codec->GetCompression();
    SetCodec(compression, videoch);
    svlVideoIO::ReleaseCompression(compression);
    svlVideoIO::ReleaseCodec(codec);

    return SVL_OK;
}

int svlFilterVideoFileWriter::GetFilePath(std::string &filepath, unsigned int videoch) const
{
    if (FilePath.size() <= videoch) return SVL_FAIL;
    filepath = FilePath[videoch];
    return SVL_OK;
}

int svlFilterVideoFileWriter::SetFramerate(double framerate)
{
    if (IsInitialized() == true)
        return SVL_FAIL;

    if (framerate > 0.1) {
        Framerate = framerate;
        return SVL_OK;
    }

    return SVL_FAIL;
}

void svlFilterVideoFileWriter::Pause()
{
    // Get current absolute time
    osaAbsoluteTime abstime;
    TimeServer->RelativeToAbsolute(TimeServer->GetRelativeTime(), abstime);
    TargetActionTime = abstime.sec + abstime.nsec / 1000000000.0;

    TargetCaptureLength = 0;
    Action = true;
}

void svlFilterVideoFileWriter::Record(int frames)
{
    // Get current absolute time
    osaAbsoluteTime abstime;
    TimeServer->RelativeToAbsolute(TimeServer->GetRelativeTime(), abstime);
    TargetActionTime = abstime.sec + abstime.nsec / 1000000000.0;

    TargetCaptureLength = frames;
    Action = true;
}

int svlFilterVideoFileWriter::DialogCodec(unsigned int videoch)
{
    if (IsInitialized() == true) return SVL_FAIL;
    if (UpdateStreamCount(videoch + 1) != SVL_OK) return SVL_FAIL;

    if (svlVideoIO::DialogCodec(FilePath[videoch], &(CodecParams[videoch])) == SVL_OK &&
        CodecParams[videoch]) return SVL_OK;

    return SVL_FAIL;
}

int svlFilterVideoFileWriter::GetCodecName(std::string &encoder, unsigned int videoch) const
{
    if (CodecParams.size() <= videoch ||
        !CodecParams[videoch] ||
        CodecParams[videoch]->size < sizeof(svlVideoIO::Compression)) return SVL_FAIL;

    encoder.assign(CodecParams[videoch]->name);

    return SVL_OK;
}

int svlFilterVideoFileWriter::GetCodec(svlVideoIO::Compression **compression, unsigned int videoch) const
{
    if (CodecParams.size() <= videoch ||
        !CodecParams[videoch] ||
        CodecParams[videoch]->size < sizeof(svlVideoIO::Compression)) return SVL_FAIL;

    compression[0] = reinterpret_cast<svlVideoIO::Compression*>(new unsigned char[CodecParams[videoch]->size]);
    memcpy(compression[0], CodecParams[videoch], CodecParams[videoch]->size);

    return SVL_OK;
}

int svlFilterVideoFileWriter::SetCodec(const svlVideoIO::Compression *compression, unsigned int videoch)
{
    if (IsInitialized()) return SVL_FAIL;
    if (UpdateStreamCount(videoch + 1) != SVL_OK) return SVL_FAIL;
    if (!compression || compression->size < sizeof(svlVideoIO::Compression)) return SVL_FAIL;
    if (compression->size > 4096) {
        // 4096 is an arbitrary number but most likely large enough for everything
        return SVL_FAIL;
    }

    svlVideoIO::ReleaseCompression(CodecParams[videoch]);
    CodecParams[videoch] = reinterpret_cast<svlVideoIO::Compression*>(new unsigned char[compression->size]);
    memcpy(CodecParams[videoch], compression, compression->size);
    CodecParams[videoch]->size = compression->size;

    return SVL_OK;
}

int svlFilterVideoFileWriter::SaveCodec(const std::string &filepath, unsigned int videoch) const
{
    if (CodecParams.size() <= videoch ||
        !CodecParams[videoch] ||
        CodecParams[videoch]->size < sizeof(svlVideoIO::Compression)) return SVL_FAIL;

    unsigned int size;

    while (1) {
        // Open file for writing
        std::ofstream file(filepath.c_str(), std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
        if (!file.is_open()) break;

        // Write "framerate"
        if (file.write(reinterpret_cast<const char*>(&Framerate), sizeof(double)).fail()) break;

        // Write "codec params size"
        size = CodecParams[videoch]->size;
        if (file.write(reinterpret_cast<char*>(&size), sizeof(unsigned int)).fail()) break;
        // Write "codec parameters"
        if (file.write(reinterpret_cast<const char*>(CodecParams[videoch]), size).fail()) break;

        return SVL_OK;
    }

    return SVL_FAIL;
}

int svlFilterVideoFileWriter::LoadCodec(const std::string &filepath, unsigned int videoch)
{
    if (IsInitialized()) return SVL_FAIL;
    if (UpdateStreamCount(videoch + 1) != SVL_OK) return SVL_FAIL;

    double framerate;
    unsigned int size;
    svlVideoIO::Compression* params = 0;

    while (1) {

        // Open file for reading
        std::ifstream file(filepath.c_str(), std::ios_base::in | std::ios_base::binary);
        if (!file.is_open()) break;

        // Read "framerate"
        if (file.read(reinterpret_cast<char*>(&framerate), sizeof(double)).fail()) break;

        // Read "codec params size"
        if (file.read(reinterpret_cast<char*>(&size), sizeof(unsigned int)).fail()) break;
        if (size < sizeof(svlVideoIO::Compression) || size > 4096) {
            // 4096 is an arbitrary number but most likely large enough for everything
            break;
        }

        params = reinterpret_cast<svlVideoIO::Compression*>(new unsigned char[size]);

        // Read "codec parameters"
        if (file.read(reinterpret_cast<char*>(params), size).fail()) break;
        params->size = size; // Just to make sure we are fine...

        svlVideoIO::ReleaseCompression(CodecParams[videoch]);
        CodecParams[videoch] = params;

        if (videoch == 0) {
            // The  first video channel defines the framerate
            if (framerate >= 0.1 || framerate <= 1000.0) Framerate = framerate;
        }

        return SVL_OK;
    }

    svlVideoIO::ReleaseCompression(params);

    return SVL_FAIL;
}

int svlFilterVideoFileWriter::UpdateStreamCount(unsigned int count)
{
    if (count > 2) return SVL_FAIL;

    unsigned int prevsize = Codec.size();
    if (prevsize < count) {
        Codec.resize(count);
        CodecParams.resize(count);
        Disabled.resize(count);
        FilePath.resize(count);
        for (unsigned int i = prevsize; i < count; i ++) {
            Codec[i] = 0;
            CodecParams[i] = 0;
            Disabled[i] = false;
        }
    }

    return SVL_OK;
}

