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

#include <cisstStereoVision/svlFilterVideoFileWriter.h>
#include <cisstStereoVision/svlFile.h>
#include <cisstOSAbstraction/osaTimeServer.h>
#include <cisstOSAbstraction/osaSleep.h>
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
    IsRecording(false),
    TargetCaptureLength(-1), // Continuous saving by default?
    CaptureLength(-1),
    CodecsMultithreaded(false)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBStereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);

    UpdateCodecCount(2);

    TimeServer = new osaTimeServer;
    TimeServer->SetTimeOrigin();
}

svlFilterVideoFileWriter::~svlFilterVideoFileWriter()
{
    Release();

    for (unsigned int i = 0; i < CodecParam.size(); i ++) {
        svlVideoIO::ReleaseCodec(CodecProto[i]);
        svlVideoIO::ReleaseCompression(CodecParam[i]);
    }

    delete TimeServer;
}

int svlFilterVideoFileWriter::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    svlSampleImage* img = dynamic_cast<svlSampleImage*>(syncInput);

    Release();

    const unsigned int videochannels = img->GetVideoChannels();

    UpdateCodecCount(videochannels);

    for (unsigned int i = 0; i < videochannels; i ++) {

        ImageDimensions[i].Assign(img->GetWidth(i), img->GetHeight(i));

        // Open file if needed
        if (!Codec[i] && !FilePath[i].empty()) {
            SetFilePath(FilePath[i], i);
            OpenFile(i);
        }
        else {
            CMN_LOG_CLASS_INIT_WARNING << "Initialize: missing file information on channel: " << i << std::endl;
        }
    }

    // Initialize video frame counter
    VideoFrameCounter = 0;
    IsRecording = false;

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
        ErrorInProcess = false;

        if (Action) {
            CaptureLength = TargetCaptureLength;
            ActionTime = TargetActionTime;
            Action = false;
        }

        CS.Enter();
    }

    _SynchronizeThreads(procInfo);

    if (CaptureLength == 0) {
        if (ActionTime < syncInput->GetTimestamp()) {
            IsRecording = false;
            _OnSingleThread(procInfo) CS.Leave();
            return SVL_OK;
        }
        // Process remaining samples in the buffer when paused
    }
    else {
        // Drop frames when restarted
        if (ActionTime > syncInput->GetTimestamp()) {
            _OnSingleThread(procInfo) CS.Leave();
            return SVL_OK;
        }
    }

    IsRecording = true;

    svlSampleImage* img = dynamic_cast<svlSampleImage*>(syncOutput);
    const unsigned int videochannels = img->GetVideoChannels();
    unsigned int idx;

    if (CodecsMultithreaded) {
        // Codecs are multithreaded, so it's worth
        // splitting work between all threads
        for (idx = 0; idx < videochannels; idx ++) {
            // Codec is responsible for thread synchronzation
            if (Codec[idx] && Codec[idx]->Write(procInfo, *img, idx) != SVL_OK) {
                CMN_LOG_CLASS_INIT_ERROR << "Process: failed to write video frame on channel: " << idx
                                         << ", in thread: " << procInfo->ID << std::endl;
                ErrorOnChannel[idx] = true;
                ErrorInProcess      = true;
            }
        }
    }
    else {
        // Codecs are not multithreaded, so assigning
        // each video channel to a single thread
        _ParallelLoop(procInfo, idx, videochannels)
        {
            if (Codec[idx] && Codec[idx]->Write(0, *img, idx) != SVL_OK) {
                CMN_LOG_CLASS_INIT_ERROR << "Process: failed to write video frame on channel: " << idx << std::endl;
                ErrorOnChannel[idx] = true;
                ErrorInProcess      = true;
            }
        }
    }

    _SynchronizeThreads(procInfo);

    _OnSingleThread(procInfo)
    {
        for (idx = 0; idx < videochannels; idx ++) {
            if (Codec[idx]) {
                FramesWritten[idx] ++;

                if (TimestampsFile[idx]) {
                    double time = syncInput->GetTimestamp();
                    if (FramesWritten[idx] > 1) {
                        time -= FirstTimestamp[idx];
                    }
                    else {
                        FirstTimestamp[idx] = time;
                    }

                    std::stringstream ts;
                    ts << (FramesWritten[idx] - 1) << " " << std::fixed << std::setprecision(4) << time << "\r\n";
                    long long int len = ts.str().length();
                    if (TimestampsFile[idx]->Write(ts.str().c_str(), len) < len) {
                        CMN_LOG_CLASS_INIT_WARNING << "Process: failed to write timestamp on channel: " << idx << std::endl;
                    }
                }
            }
        }

        CS.Leave();

        if (ErrorInProcess) {
            for (idx = 0; idx < videochannels; idx ++) {
                if (ErrorOnChannel[idx]) {
                    CMN_LOG_CLASS_INIT_ERROR << "Process: attempting to close video file on channel: " << idx << std::endl;
                    CloseFile(idx);
                }
            }
            IsRecording = false;
        }

        if (CaptureLength > 0) {
            IsRecording = true;
            CaptureLength --;
        }
    }

    return SVL_OK;
}

int svlFilterVideoFileWriter::Release()
{
    for (unsigned int i = 0; i < Codec.size(); i ++) {
        CloseFile(i);
        ImageDimensions[i].SetAll(0);
    }

    return SVL_OK;
}

int svlFilterVideoFileWriter::DialogOpenFile(unsigned int videoch)
{
    std::ostringstream out;
    out << "Save video file [channel #" << videoch << "]";
    std::string path, title(out.str());

    if (svlVideoIO::DialogFilePath(true, title, path) == SVL_OK) {

        SetFilePath(path, videoch);

        // Open codec dialog
        if (DialogFramerate(videoch) != SVL_OK) {
            CMN_LOG_CLASS_INIT_WARNING << "DialogOpenFile: failed to set video framerate" << std::endl;
        }

        UpdateCodecCount(videoch + 1);

        if (!CodecParam[videoch]) {
            // Open codec dialog
            if (DialogCodec(path, videoch) != SVL_OK || !CodecParam[videoch]) {
                CMN_LOG_CLASS_INIT_WARNING << "DialogOpenFile: failed to configure video compression" << std::endl;
            }
        }

        return OpenFile(videoch);
    }

    return SVL_FAIL;
}

int svlFilterVideoFileWriter::DialogFramerate(unsigned int videoch)
{
    UpdateCodecCount(videoch + 1);

    char input[256];
    double min = 0.1, max = 1000.0, defaultval = 30.0, framerate;
    std::cout << " # Enter video frame rate (min=" << std::fixed << std::setprecision(1) << min
                                       << "; max=" << std::fixed << std::setprecision(1) << max
                                       << "; default=" << std::fixed << std::setprecision(1) << defaultval << "): ";
    std::cin.getline(input, 256);
    if (std::cin.gcount() > 1) {
        framerate = atof(input);
        if (framerate < min) framerate = min;
        if (framerate > max) framerate = max;
    }
    else framerate = defaultval;
    std::cout << "    Framerate = " << std::fixed << std::setprecision(2) << framerate << std::endl;
    Framerate[videoch] = framerate;

    return SVL_OK;
}

int svlFilterVideoFileWriter::DialogCodec(const std::string &extension, unsigned int videoch)
{
    // The argument `extension` may be file extension or file name or full path

    UpdateCodecCount(videoch + 1);

    // Add '.' before extension in order to make sure `svlVideoIO::GetCodec`
    // and `svlVideoIO::DialogCodec` accept it as filename
    std::string filename(extension);
    filename.insert(0, ".");

    if (svlVideoIO::DialogCodec(filename, &(CodecParam[videoch])) != SVL_OK || !CodecParam[videoch]) {
        CodecParam[videoch] = 0;
        CMN_LOG_CLASS_INIT_ERROR << "DialogCodec: failed to configure video compression" << std::endl;
        return SVL_FAIL;
    }

    return SVL_OK;
}

int svlFilterVideoFileWriter::SetFilePath(const std::string &filepath, unsigned int videoch)
{
    UpdateCodecCount(videoch + 1);

    FilePath[videoch] = filepath;

    return SVL_OK;
}

int svlFilterVideoFileWriter::SetFramerate(const double framerate, unsigned int videoch)
{
    UpdateCodecCount(videoch + 1);

    Framerate[videoch] = framerate;

    return SVL_OK;
}

int svlFilterVideoFileWriter::SetEnableTimestampsFile(bool enable, unsigned int videoch)
{
    UpdateCodecCount(videoch + 1);

    EnableTimestampsFile[videoch] = enable;

    return SVL_OK;
}

int svlFilterVideoFileWriter::SetCodecParams(const svlVideoIO::Compression *compression, unsigned int videoch)
{
    if (!compression || compression->size < sizeof(svlVideoIO::Compression)) {
        CMN_LOG_CLASS_INIT_ERROR << "SetCodec: invalid compression structure" << std::endl;
        return SVL_FAIL;
    }
    if (compression->size > 4096) {
        // 4096 is an arbitrary number but most likely large enough for everything
        CMN_LOG_CLASS_INIT_ERROR << "SetCodec: compression structure too large" << std::endl;
        return SVL_FAIL;
    }

    UpdateCodecCount(videoch + 1);

    svlVideoIO::ReleaseCompression(CodecParam[videoch]);
    CodecParam[videoch] = reinterpret_cast<svlVideoIO::Compression*>(new unsigned char[compression->size]);
    memcpy(CodecParam[videoch], compression, compression->size);
    CodecParam[videoch]->size = compression->size;

    return SVL_OK;
}

int svlFilterVideoFileWriter::ResetCodec(unsigned int videoch)
{
    if (videoch >= CodecParam.size()) {
        CMN_LOG_CLASS_INIT_ERROR << "ResetCodec: video channel out of range: " << videoch << std::endl;
        return SVL_FAIL;
    }

    svlVideoIO::ReleaseCompression(CodecParam[videoch]);
    CodecParam[videoch] = 0;
    Framerate[videoch]  = 30.0;

    return SVL_OK;
}

int svlFilterVideoFileWriter::SaveCodec(const std::string &filepath, unsigned int videoch) const
{
    if (videoch >= CodecParam.size()) {
        CMN_LOG_CLASS_INIT_ERROR << "SaveCodec: video channel out of range: " << videoch << std::endl;
        return SVL_FAIL;
    }
    if (!CodecParam[videoch] ||
        CodecParam[videoch]->size < sizeof(svlVideoIO::Compression)) {
        CMN_LOG_CLASS_INIT_ERROR << "SaveCodec: invalid compression structure" << std::endl;
        return SVL_FAIL;
    }

    while (1) {

        // Open file for writing
        svlFile file(filepath, svlFile::W);
        if (!file.IsOpen()) break;

        // Write "framerate"
        if (!file.Write(Framerate[videoch])) break;

        // Write "codec params size"
        if (!file.Write(CodecParam[videoch]->size)) break;

        // Write "codec parameters"
        long long int len = CodecParam[videoch]->size;
        if (file.Write(reinterpret_cast<const char*>(CodecParam[videoch]), len) < len) break;

        return SVL_OK;
    }

    CMN_LOG_CLASS_INIT_ERROR << "SaveCodec: failed to save codec settings to file: " << filepath << std::endl;

    return SVL_FAIL;
}

int svlFilterVideoFileWriter::LoadCodec(const std::string &filepath, unsigned int videoch)
{
    UpdateCodecCount(videoch + 1);

    svlVideoIO::Compression* params = 0;
    unsigned int size;
    long long int len;

    while (1) {

        // Open file for reading
        svlFile file(filepath, svlFile::R);
        if (!file.IsOpen()) break;

        // Read "framerate"
        if (!file.Read(Framerate[videoch])) break;
        if (Framerate[videoch] < 0.1) Framerate[videoch] = 0.1;
        else if (Framerate[videoch] > 1000.0) Framerate[videoch] = 1000.0;

        // Read "codec params size"
        if (!file.Read(size)) break;
        if (size < sizeof(svlVideoIO::Compression) || size > 4096) {
            // 4096 is an arbitrary number but most likely large enough for everything
            break;
        }

        params = reinterpret_cast<svlVideoIO::Compression*>(new unsigned char[size]);
        memset(params, 0, size);

        // Read "codec parameters"
        len = size;
        if (file.Read(reinterpret_cast<char*>(params), len) < len) break;
        params->size = size; // Just to make sure we are fine...

        svlVideoIO::ReleaseCompression(CodecParam[videoch]);
        CodecParam[videoch] = params;

        return SVL_OK;
    }

    svlVideoIO::ReleaseCompression(params);

    CMN_LOG_CLASS_INIT_ERROR << "LoadCodec: failed to load codec settings from file: " << filepath << std::endl;

    return SVL_FAIL;
}

std::string svlFilterVideoFileWriter::GetFilePath(unsigned int videoch) const
{
    if (FilePath.size() <= videoch) {
        CMN_LOG_CLASS_INIT_ERROR << "GetFilePath: video channel out of range: " << videoch << std::endl;
        return "";
    }
    return FilePath[videoch];
}

int svlFilterVideoFileWriter::GetFilePath(std::string &filepath, unsigned int videoch) const
{
    filepath = GetFilePath(videoch);
    if (filepath.empty()) return SVL_FAIL;
    return SVL_OK;
}

double svlFilterVideoFileWriter::GetFramerate(unsigned int videoch)
{
    if (videoch >= CodecParam.size()) {
        CMN_LOG_CLASS_INIT_ERROR << "GetFramerate: video channel out of range: " << videoch << std::endl;
        return -1.0;
    }
    return Framerate[videoch];
}

bool svlFilterVideoFileWriter::GetEnableTimestampsFile(unsigned int videoch) const
{
    if (videoch >= CodecParam.size()) {
        CMN_LOG_CLASS_INIT_ERROR << "GetEnableTimestampsFile: video channel out of range: " << videoch << std::endl;
        return false;
    }
    return EnableTimestampsFile[videoch];
}

std::string svlFilterVideoFileWriter::GetCodecName(unsigned int videoch) const
{
    if (videoch >= CodecParam.size()) {
        CMN_LOG_CLASS_INIT_ERROR << "GetCodecName: video channel out of range: " << videoch << std::endl;
        return "";
    }
    if (!CodecParam[videoch] ||
        CodecParam[videoch]->size < sizeof(svlVideoIO::Compression)) {
        CMN_LOG_CLASS_INIT_ERROR << "GetCodecName: invalid compression structure" << std::endl;
        return "";
    }

    return CodecParam[videoch]->name;
}

int svlFilterVideoFileWriter::GetCodecName(std::string &encoder, unsigned int videoch) const
{
    encoder = GetCodecName(videoch);
    if (encoder.empty()) return SVL_FAIL;
    return SVL_OK;
}

svlVideoIO::Compression* svlFilterVideoFileWriter::GetCodecParams(unsigned int videoch) const
{
    if (videoch >= CodecParam.size()) {
        CMN_LOG_CLASS_INIT_ERROR << "GetCodecParams: video channel out of range: " << videoch << std::endl;
        return 0;
    }
    if (!CodecParam[videoch] ||
        CodecParam[videoch]->size < sizeof(svlVideoIO::Compression)) {
        CMN_LOG_CLASS_INIT_ERROR << "GetCodecParams: invalid compression structure" << std::endl;
        return 0;
    }

    svlVideoIO::Compression* compression = reinterpret_cast<svlVideoIO::Compression*>(new unsigned char[CodecParam[videoch]->size]);
    memcpy(compression, CodecParam[videoch], CodecParam[videoch]->size);

    return compression;
}

int svlFilterVideoFileWriter::GetCodecParams(svlVideoIO::Compression **compression, unsigned int videoch) const
{
    if (!compression) return SVL_FAIL;
    compression[0] = GetCodecParams(videoch);
    if (compression[0] == 0) return SVL_FAIL;
    return SVL_OK;
}

int svlFilterVideoFileWriter::OpenFile(unsigned int videoch)
{
    UpdateCodecCount(videoch + 1);

    if (ImageDimensions[videoch].X() == 0 || ImageDimensions[videoch].Y() == 0) {
        // Ooops, we don't know the image dimensions yet
        // Delay this call. It will be executed on `Initialize`
        return SVL_OK;
    }

    CS.Enter();

    while (1) {

        // Close video file if currently open
        if (Codec[videoch]) {
            svlVideoIO::ReleaseCodec(Codec[videoch]);
        }
        if (TimestampsFile[videoch]) {
            delete TimestampsFile[videoch];
            TimestampsFile[videoch] = 0;
        }

        // Get video codec for file extension
        Codec[videoch] = svlVideoIO::GetCodec(FilePath[videoch]);
        if (!Codec[videoch]) {
            CMN_LOG_CLASS_INIT_ERROR << "OpenFile: failed to find suitable codec for file \"" << FilePath[videoch]
                                     << "\" on channel: " << videoch << std::endl;
            break;
        }

        // Apply compression settings configured before this call
        if (!CodecParam[videoch] || Codec[videoch]->SetCompression(CodecParam[videoch]) != SVL_OK) {

            // Set compression settings to default
            svlVideoIO::Compression* compression = Codec[videoch]->GetCompression();
            if (Codec[videoch]->SetCompression(compression) != SVL_OK) {
                CMN_LOG_CLASS_INIT_WARNING << "OpenFile: failed to configure video compression for file \"" << FilePath[videoch]
                                           << "\" on channel: " << videoch << std::endl;
            }
            svlVideoIO::ReleaseCompression(compression);
        }

        // Open video file
        if (Codec[videoch]->Create(FilePath[videoch],
                                   ImageDimensions[videoch].X(),
                                   ImageDimensions[videoch].Y(),
                                   Framerate[videoch]) != SVL_OK) {
            CMN_LOG_CLASS_INIT_ERROR << "OpenFile: failed to create video file \"" << FilePath[videoch]
                                     << "\" on channel: " << videoch << std::endl;
            break;
        }

        // Update `CodecsMultithreaded` flag
        bool multithreaded = true;
        for (unsigned int i = 0; i < Codec.size(); i ++) {
            if (Codec[i] && !Codec[i]->IsMultithreaded()) {
                multithreaded = false;
                break;
            }
        }
        CodecsMultithreaded = multithreaded;

        if (EnableTimestampsFile[videoch]) {
            // Extract file name without extension
            std::string filepath(FilePath[videoch]);
            std::string extension;
            std::string::size_type pos = filepath.rfind('.');
            if (pos != std::string::npos) {
                extension = filepath.substr(pos + 1);
                filepath.erase(pos);
            }

            filepath.append("_" + extension + "_ts.txt");
            TimestampsFile[videoch] = new svlFile(filepath, svlFile::W);
            if (!TimestampsFile[videoch]->IsOpen()) {
                delete TimestampsFile[videoch];
                TimestampsFile[videoch] = 0;
                CMN_LOG_CLASS_INIT_WARNING << "OpenFile: failed to create timestamp file \"" << filepath
                                           << "\" on channel: " << videoch << std::endl;
            }
        }

        CS.Leave();

        std::stringstream strstr;
        strstr << "OpenFile: succeeded to create video file \"" << FilePath[videoch];
        if (Codec[videoch]->IsVariableFramerate()) strstr << "\" (variable framerate)";
        else strstr << "\" (framerate=" << std::fixed << std::setprecision(2) << Framerate[videoch] << ")";
        strstr << " on channel: " << videoch;
        CMN_LOG_CLASS_INIT_VERBOSE << strstr.str() << std::endl;

        ErrorOnChannel[videoch] = false;
        FramesWritten[videoch] = 0;

        return SVL_OK;
    }

    CS.Leave();

    CloseFile(videoch);

    return SVL_FAIL;
}

int svlFilterVideoFileWriter::OpenFile(const std::string &filepath, unsigned int videoch)
{
    if (SetFilePath(filepath, videoch) != SVL_OK ||
        OpenFile(videoch)              != SVL_OK) return SVL_FAIL;
    return SVL_OK;
}

int svlFilterVideoFileWriter::CloseFile(unsigned int videoch)
{
    if (videoch >= Codec.size()) {
        CMN_LOG_CLASS_INIT_ERROR << "CloseFile: video channel out of range: " << videoch << std::endl;
        return SVL_FAIL;
    }

    CS.Enter();

    if (Codec[videoch]) {
        svlVideoIO::ReleaseCodec(Codec[videoch]);
        Codec[videoch] = 0;
    }
    if (TimestampsFile[videoch]) {
        delete TimestampsFile[videoch];
        TimestampsFile[videoch] = 0;
    }

    CS.Leave();

    return SVL_OK;
}

void svlFilterVideoFileWriter::Record(int frames)
{
    TargetActionTime = -1.0;
    TargetCaptureLength = frames;
    Action = true;
}

void svlFilterVideoFileWriter::RecordAtTime(int frames, double time)
{

    if (time <= 0.0) {
        // Get current absolute time
        osaAbsoluteTime abstime;
        TimeServer->RelativeToAbsolute(TimeServer->GetRelativeTime(), abstime);
        TargetActionTime = abstime.sec + abstime.nsec / 1000000000.0;
    }
    else {
        TargetActionTime = time;
    }

    TargetCaptureLength = frames;
    Action = true;
}

void svlFilterVideoFileWriter::Pause()
{
    TargetActionTime = -1.0;
    TargetCaptureLength = 0;
    Action = true;
}

void svlFilterVideoFileWriter::PauseAtTime(double time)
{
    if (time <= 0.0) {
        // Get current absolute time
        osaAbsoluteTime abstime;
        TimeServer->RelativeToAbsolute(TimeServer->GetRelativeTime(), abstime);
        TargetActionTime = abstime.sec + abstime.nsec / 1000000000.0;
    }
    else {
        TargetActionTime = time;
    }

    TargetCaptureLength = 0;
    Action = true;
}

void svlFilterVideoFileWriter::UpdateCodecCount(const unsigned int count)
{
    const unsigned int prevsize = static_cast<unsigned int>(Codec.size());
    if (prevsize < count) {
        ErrorOnChannel.resize(count);
        ImageDimensions.resize(count);
        Codec.resize(count);
        CodecProto.resize(count);
        CodecParam.resize(count);
        FilePath.resize(count);
        Framerate.resize(count);
        FramesWritten.resize(count);
        EnableTimestampsFile.resize(count);
        TimestampsFile.resize(count);
        FirstTimestamp.resize(count);
        for (unsigned int i = prevsize; i < count; i ++) {
            ErrorOnChannel[i]       = false;
            ImageDimensions[i].SetAll(0);
            Codec[i]                = 0;
            CodecProto[i]           = 0;
            CodecParam[i]           = 0;
            FilePath[i]             = "";
            Framerate[i]            = 30.0;
            FramesWritten[i]        = 0;
            EnableTimestampsFile[i] = false;
            TimestampsFile[i]       = 0;
            FirstTimestamp[i]       = 0.0;
        }
    }
}

bool svlFilterVideoFileWriter::GetIsRecording() const {
    return IsRecording;
}

