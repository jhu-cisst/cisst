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

#include <cisstStereoVision/svlFilterSourceVideoFile.h>
#include <cisstStereoVision/svlFilterOutput.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstOSAbstraction/osaSleep.h>


#ifdef _MSC_VER
    // Quick fix for Visual Studio Intellisense:
    // The Intellisense parser can't handle the CMN_UNUSED macro
    // correctly if defined in cmnPortability.h, thus
    // we should redefine it here for it.
    // Removing this part of the code will not effect compilation
    // in any way, on any platforms.
    #undef CMN_UNUSED
    #define CMN_UNUSED(argument) argument
#endif


/***************************************/
/*** svlFilterSourceVideoFile class ****/
/***************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterSourceVideoFile, svlFilterSourceBase)

svlFilterSourceVideoFile::svlFilterSourceVideoFile() :
    svlFilterSourceBase(false),  // manual timestamp management
    OutputImage(0),
    FirstTimestamp(-1.0),
    NativeFramerate(-1.0)
{
    CreateInterfaces();

    AddOutput("output", true);
    SetAutomaticOutputType(false);
}

svlFilterSourceVideoFile::svlFilterSourceVideoFile(unsigned int channelcount) :
    svlFilterSourceBase(false),  // manual timestamp management
    OutputImage(0),
    FirstTimestamp(-1.0),
    NativeFramerate(-1.0)
{
    CreateInterfaces();

    AddOutput("output", true);
    SetAutomaticOutputType(false);
    SetChannelCount(channelcount);
}

svlFilterSourceVideoFile::~svlFilterSourceVideoFile()
{
    Release();

    if (OutputImage) delete OutputImage;
}

int svlFilterSourceVideoFile::SetChannelCount(unsigned int channelcount)
{
    if (OutputImage) {
        CMN_LOG_CLASS_INIT_ERROR << "SetChannelCount: channel count already set" << std::endl;
        return SVL_FAIL;
    }

    if (channelcount == 1) {
        GetOutput()->SetType(svlTypeImageRGB);
        OutputImage = new svlSampleImageRGB;
    }
    else if (channelcount == 2) {
        GetOutput()->SetType(svlTypeImageRGBStereo);
        OutputImage = new svlSampleImageRGBStereo;
    }
    else {
        CMN_LOG_CLASS_INIT_ERROR << "SetChannelCount: invalid channel count" << std::endl;
        return SVL_FAIL;
    }

    Codec.SetSize(channelcount);
    Codec.SetAll(0);

    FilePath.SetSize(channelcount);
    Length.SetSize(channelcount);
    Position.SetSize(channelcount);
    UseRange.SetSize(channelcount);
    UseRange.SetAll(false);
    Range.SetSize(channelcount);
    Range.SetAll(vctInt2(-1, -1));

    return SVL_OK;
}

int svlFilterSourceVideoFile::Initialize(svlSample* &syncOutput)
{
    if (OutputImage == 0) {
        CMN_LOG_CLASS_INIT_ERROR << "Initialize: failed to initialize because channel count is not yet set" << std::endl; 
        return SVL_FAIL;
    }
    syncOutput = OutputImage;

    Release();

    unsigned int width, height;
    double framerate;
    int ret = SVL_OK;

    for (unsigned int i = 0; i < OutputImage->GetVideoChannels(); i ++) {

        // Get video codec for file extension
        Codec[i] = svlVideoIO::GetCodec(FilePath[i]);
        // Open video file
        if (!Codec[i] || Codec[i]->Open(FilePath[i], width, height, framerate) != SVL_OK) {
            CMN_LOG_CLASS_INIT_ERROR << "Initialize: failed to open file \"" << FilePath[i] << "\"" << std::endl;
            ret = SVL_FAIL;
            break;
        }

        if (i == 0) {
            // The first video channel defines the video
            // framerate of all channels in the stream
            NativeFramerate = framerate;
        }

        Length[i] = Codec[i]->GetEndPos() + 1;
        Position[i] = Codec[i]->GetPos();

        if (UseRange[i]) {
            if (Range[i][0] < 0) Range[i][0] = 0;
            if (Range[i][1] < 0) Range[i][1] = Length[i];
        }

        // Create image sample of matching dimensions
        OutputImage->SetSize(i, width, height);
    }

    // Initialize timestamp for case of timestamp errors
    OutputImage->SetTimestamp(0.0);

    if (ret != SVL_OK) Release();
    return ret;
}

int svlFilterSourceVideoFile::OnStart(unsigned int CMN_UNUSED(procCount))
{
    if (GetTargetFrequency() < 0.1) SetTargetFrequency(NativeFramerate);
    RestartTargetTimer();

    return SVL_OK;
}

int svlFilterSourceVideoFile::Process(svlProcInfo* procInfo, svlSample* &syncOutput)
{
    syncOutput = OutputImage;

    // Try to keep target frequency
    _OnSingleThread(procInfo) WaitForTargetTimer();

    unsigned int idx, videochannels = OutputImage->GetVideoChannels();
    double timestamp, timespan;
    int pos, ret = SVL_OK;

    // TO DO: add a little more sophisticated logic here
    if (Codec[0]->IsMultithreaded()) {
        // Codecs are multithreaded, so it's worth
        // splitting work between all threads

        for (idx = 0; idx < videochannels; idx ++) {

            if (Codec[idx]) {

                // Seek (if needed) to frame that needs to be extracted
                _OnSingleThread(procInfo)
                {
                    pos = Codec[idx]->GetPos();
                    Position[idx] = pos;

                    if (UseRange[idx]) {
                        // Check if position is outside of the playback segment
                        if (pos < Range[idx][0]) {
                            Codec[idx]->SetPos(Range[idx][0]);
                            ResetTimer = true;
                        }
                        else if (pos > Range[idx][1]) {
                            if (!GetLoop()) {
                                ret = SVL_STOP_REQUEST;
                                break;
                            }
                            else {
                                Codec[idx]->SetPos(Range[idx][0]);
                                ResetTimer = true;
                            }
                        }
                    }

                    Status = SVL_OK;
                }

                _SynchronizeThreads(procInfo);

                // Extract frame
                ret = Codec[idx]->Read(procInfo, *OutputImage, idx, true);

                if (ret != SVL_OK) Status = ret;
                if (ret == SVL_VID_END_REACHED && !GetLoop()) Status = SVL_STOP_REQUEST;

                _SynchronizeThreads(procInfo);

                if (Status == SVL_STOP_REQUEST) {
                    ret = SVL_STOP_REQUEST;
                    break;
                }

                // Manage looped playback and frame timing
                _OnSingleThread(procInfo)
                {
                    if (Status == SVL_VID_END_REACHED || Status == SVL_VID_RETRY) {
                        // Loop around
                        Status = Codec[idx]->Read(0, *OutputImage, idx, true);
                    }
                    if (Status != SVL_OK) {
                        CMN_LOG_CLASS_INIT_ERROR << "Process: failed to read video frame on channel: " << idx << std::endl; 
                        break;
                    }

                    // Run timer based in the first video channel
                    if (idx == 0) {

                        // Get timestamp stored in the video file
                        timestamp = Codec[idx]->GetTimestamp();
                        if (timestamp > 0.0) {

                            if (!IsTargetTimerRunning()) {

                                // Try to keep orignal frame intervals
                                if (ResetTimer || Codec[idx]->GetPos() == 1) {
                                    FirstTimestamp = timestamp;
                                    Timer.Reset();
                                    Timer.Start();
                                    ResetTimer = false;
                                }
                                else {
                                    timespan = (timestamp - FirstTimestamp) - Timer.GetElapsedTime();
                                    if (timespan > 0.0) osaSleep(timespan);
                                }
                            }
                        }

                        OutputImage->SetTimestamp(timestamp);
                    }
                }
            }
        }
    }
    else {
        // Codecs are not multithreaded, so assigning
        // each video channel to a single thread

        _ParallelLoop(procInfo, idx, videochannels)
        {
            if (Codec[idx]) {

                // Seek (if needed) to frame that needs to be extracted
                pos = Codec[idx]->GetPos();
                Position[idx] = pos;

                if (UseRange[idx]) {
                    // Check if position is outside of the playback segment
                    if (pos < Range[idx][0]) {
                        Codec[idx]->SetPos(Range[idx][0]);
                        ResetTimer = true;
                    }
                    else if (pos > Range[idx][1]) {
                        if (!GetLoop()) {
                            ret = SVL_STOP_REQUEST;
                            break;
                        }
                        else {
                            Codec[idx]->SetPos(Range[idx][0]);
                            ResetTimer = true;
                        }
                    }
                }

                // Extract frame
                ret = Codec[idx]->Read(0, *OutputImage, idx, true);

                // Manage looped playback and errors
                if (ret == SVL_VID_END_REACHED) {
                    if (!GetLoop()) {
                        ret = SVL_STOP_REQUEST;
                        break;
                    }
                    else {
                        // Loop around
                        ret = Codec[idx]->Read(0, *OutputImage, idx, true);
                    }
                }
                if (ret != SVL_OK) {
                    CMN_LOG_CLASS_INIT_ERROR << "Process: failed to read video frame on channel: " << idx << std::endl; 
                    break;
                }

                // Run timer based in the first video channel
                if (idx == 0) {

                    // Get timestamp stored in the video file
                    timestamp = Codec[idx]->GetTimestamp();
                    if (timestamp > 0.0) {

                        if (!IsTargetTimerRunning()) {

                            // Try to keep orignal frame intervals
                            if (ResetTimer || Codec[idx]->GetPos() == 1) {
                                FirstTimestamp = timestamp;
                                Timer.Reset();
                                Timer.Start();
                                ResetTimer = false;
                            }
                            else {
                                timespan = (timestamp - FirstTimestamp) - Timer.GetElapsedTime();
                                if (timespan > 0.0) osaSleep(timespan);
                            }
                        }
                    }

                    OutputImage->SetTimestamp(timestamp);
                }
            }
        }
    }

    return ret;
}

int svlFilterSourceVideoFile::Release()
{
    for (unsigned int i = 0; i < Codec.size(); i ++) {
        svlVideoIO::ReleaseCodec(Codec[i]);
        Codec[i] = 0;
    }

    if (Timer.IsRunning()) Timer.Stop();
    StopTargetTimer();

    return SVL_OK;
}

void svlFilterSourceVideoFile::OnResetTimer()
{
    ResetTimer = true;
}

int svlFilterSourceVideoFile::DialogFilePath(unsigned int videoch)
{
    if (OutputImage == 0) {
        CMN_LOG_CLASS_INIT_ERROR << "DialogFilePath: failed to initialize because channel count is not yet set" << std::endl; 
        return SVL_FAIL;
    }
    if (IsInitialized() == true) {
        CMN_LOG_CLASS_INIT_ERROR << "DialogFilePath: filter has already been initialized" << std::endl;
        return SVL_ALREADY_INITIALIZED;
    }

    if (videoch >= OutputImage->GetVideoChannels()) {
        CMN_LOG_CLASS_INIT_ERROR << "DialogFilePath: video channel out of range: " << videoch << std::endl;
        return SVL_WRONG_CHANNEL;
    }

    std::ostringstream out;
    out << "Open video file [channel #" << videoch << "]";
    std::string title(out.str());

    return svlVideoIO::DialogFilePath(false, title, FilePath[videoch]);
}

int svlFilterSourceVideoFile::SetFilePath(const std::string &filepath, unsigned int videoch)
{
    if (OutputImage == 0) {
        CMN_LOG_CLASS_INIT_ERROR << "SetFilePath: failed to initialize because channel count is not yet set" << std::endl; 
        return SVL_FAIL;
    }
    if (IsInitialized() == true) {
        CMN_LOG_CLASS_INIT_ERROR << "SetFilePath: filter has already been initialized" << std::endl;
        return SVL_ALREADY_INITIALIZED;
    }

    if (videoch >= OutputImage->GetVideoChannels()) {
        CMN_LOG_CLASS_INIT_ERROR << "SetFilePath: video channel out of range: " << videoch << std::endl;
        return SVL_WRONG_CHANNEL;
    }

    FilePath[videoch] = filepath;

    return SVL_OK;
}

int svlFilterSourceVideoFile::GetFilePath(std::string &filepath, unsigned int videoch) const
{
    if (FilePath.size() <= videoch) {
        CMN_LOG_CLASS_INIT_ERROR << "GetFilePath: video channel out of range: " << videoch << std::endl;
        return SVL_FAIL;
    }
    filepath = FilePath[videoch];
    return SVL_OK;
}

int svlFilterSourceVideoFile::SetPosition(const int position, unsigned int videoch)
{
    if (Codec.size() <= videoch || !Codec[videoch]) {
        CMN_LOG_CLASS_INIT_ERROR << "SetPosition: video channel out of range: " << videoch << std::endl;
        return SVL_FAIL;
    }
    Codec[videoch]->SetPos(position);
    Position[videoch] = position;
    ResetTimer = true;
    return SVL_OK;
}

int svlFilterSourceVideoFile::GetPosition(unsigned int videoch) const
{
    if (Codec.size() <= videoch || !Codec[videoch]) {
        CMN_LOG_CLASS_INIT_ERROR << "GetPosition: video channel out of range: " << videoch << std::endl;
        return SVL_FAIL;
    }
    return Codec[videoch]->GetPos();
}

int svlFilterSourceVideoFile::SetRange(const int from, const int to, unsigned int videoch)
{
    vctInt2 range;
    range.Assign(from, to);
    return SetRange(range, videoch);
}

int svlFilterSourceVideoFile::SetRange(const vctInt2 range, unsigned int videoch)
{
    if (Codec.size() <= videoch) {
        CMN_LOG_CLASS_INIT_ERROR << "SetRange: video channel out of range: " << videoch << std::endl;
        return SVL_FAIL;
    }
    Range[videoch] = range;
    UseRange[videoch] = true;
    return SVL_OK;
}

int svlFilterSourceVideoFile::GetRange(vctInt2& range, unsigned int videoch) const
{
    if (Codec.size() <= videoch) {
        CMN_LOG_CLASS_INIT_ERROR << "GetRange: video channel out of range: " << videoch << std::endl;
        return SVL_FAIL;
    }
    if (!UseRange[videoch]) {
        CMN_LOG_CLASS_INIT_WARNING << "GetRange: range is not set for video channel: " << videoch << std::endl;
        return SVL_FAIL;
    }
    range = Range[videoch];
    return SVL_OK;
}

int svlFilterSourceVideoFile::GetLength(unsigned int videoch) const
{
    if (Codec.size() <= videoch || !Codec[videoch]) {
        CMN_LOG_CLASS_INIT_ERROR << "GetLength: video channel out of range: " << videoch << std::endl;
        return SVL_FAIL;
    }
    return (Codec[videoch]->GetEndPos() + 1);
}

unsigned int svlFilterSourceVideoFile::GetWidth(unsigned int videoch) const
{
    if (!IsInitialized()) {
        CMN_LOG_CLASS_INIT_ERROR << "GetWidth: filter not yet initialized" << std::endl;
        return 0;
    }
    return OutputImage->GetWidth(videoch);
}

unsigned int svlFilterSourceVideoFile::GetHeight(unsigned int videoch) const
{
    if (!IsInitialized()) {
        CMN_LOG_CLASS_INIT_ERROR << "GetHeight: filter not yet initialized" << std::endl;
        return 0;
    }
    return OutputImage->GetHeight(videoch);
}

int svlFilterSourceVideoFile::GetPositionAtTime(const double time, unsigned int videoch) const
{
    if (Codec.size() <= videoch || !Codec[videoch]) {
        CMN_LOG_CLASS_INIT_ERROR << "GetPositionAtTime: video channel out of range: " << videoch << std::endl;
        return -1;
    }
    return (Codec[videoch]->GetPosAtTime(time));
}

double svlFilterSourceVideoFile::GetTimeAtPosition(const int position, unsigned int videoch) const
{
    if (Codec.size() <= videoch || !Codec[videoch]) {
        CMN_LOG_CLASS_INIT_ERROR << "GetTimeAtPosition: video channel out of range: " << videoch << std::endl;
        return -1.0;
    }
    return (Codec[videoch]->GetTimeAtPos(position));
}

void svlFilterSourceVideoFile::CreateInterfaces()
{
    // Add NON-QUEUED provided interface for configuration management
    mtsInterfaceProvided* provided = AddInterfaceProvided("Settings", MTS_COMMANDS_SHOULD_NOT_BE_QUEUED);
    if (provided) {
        provided->AddCommandWrite(&svlFilterSourceBase::SetTargetFrequency, dynamic_cast<svlFilterSourceBase*>(this), "SetFramerate");
        provided->AddCommandWrite(&svlFilterSourceBase::SetLoop,            dynamic_cast<svlFilterSourceBase*>(this), "SetLoop");
        provided->AddCommandVoid (&svlFilterSourceBase::Pause,              dynamic_cast<svlFilterSourceBase*>(this), "Pause");
        provided->AddCommandVoid (&svlFilterSourceBase::Play,               dynamic_cast<svlFilterSourceBase*>(this), "Play");
        provided->AddCommandWrite(&svlFilterSourceBase::Play,               dynamic_cast<svlFilterSourceBase*>(this), "PlayFrames");
        provided->AddCommandWrite(&svlFilterSourceVideoFile::SetChannelsCommand,    this, "SetChannels");
        provided->AddCommandWrite(&svlFilterSourceVideoFile::SetPathLCommand,       this, "SetFilename");
        provided->AddCommandWrite(&svlFilterSourceVideoFile::SetPathLCommand,       this, "SetLeftFilename");
        provided->AddCommandWrite(&svlFilterSourceVideoFile::SetPathRCommand,       this, "SetRightFilename");
        provided->AddCommandWrite(&svlFilterSourceVideoFile::SetPosLCommand,        this, "SetPosition");
        provided->AddCommandWrite(&svlFilterSourceVideoFile::SetPosLCommand,        this, "SetLeftPosition");
        provided->AddCommandWrite(&svlFilterSourceVideoFile::SetPosRCommand,        this, "SetRightPosition");
        provided->AddCommandWrite(&svlFilterSourceVideoFile::SetRangeLCommand,      this, "SetRange");
        provided->AddCommandWrite(&svlFilterSourceVideoFile::SetRangeLCommand,      this, "SetLeftRange");
        provided->AddCommandWrite(&svlFilterSourceVideoFile::SetRangeRCommand,      this, "SetRightRange");
        provided->AddCommandRead (&svlFilterSourceVideoFile::GetChannelsCommand,    this, "GetChannels");
        provided->AddCommandRead (&svlFilterSourceVideoFile::GetPathLCommand,       this, "GetFilename");
        provided->AddCommandRead (&svlFilterSourceVideoFile::GetPathLCommand,       this, "GetLeftFilename");
        provided->AddCommandRead (&svlFilterSourceVideoFile::GetPathRCommand,       this, "GetRightFilename");
        provided->AddCommandRead (&svlFilterSourceVideoFile::GetLengthLCommand,     this, "GetLength");
        provided->AddCommandRead (&svlFilterSourceVideoFile::GetLengthLCommand,     this, "GetLeftLength");
        provided->AddCommandRead (&svlFilterSourceVideoFile::GetLengthRCommand,     this, "GetRightLength");
        provided->AddCommandRead (&svlFilterSourceVideoFile::GetPosLCommand,        this, "GetPosition");
        provided->AddCommandRead (&svlFilterSourceVideoFile::GetPosLCommand,        this, "GetLeftPosition");
        provided->AddCommandRead (&svlFilterSourceVideoFile::GetPosRCommand,        this, "GetRightPosition");
        provided->AddCommandRead (&svlFilterSourceVideoFile::GetRangeLCommand,      this, "GetRange");
        provided->AddCommandRead (&svlFilterSourceVideoFile::GetRangeLCommand,      this, "GetLeftRange");
        provided->AddCommandRead (&svlFilterSourceVideoFile::GetRangeRCommand,      this, "GetRightRange");
        provided->AddCommandRead (&svlFilterSourceVideoFile::GetDimensionsLCommand, this, "GetDimensions");
        provided->AddCommandRead (&svlFilterSourceVideoFile::GetDimensionsLCommand, this, "GetLeftDimensions");
        provided->AddCommandRead (&svlFilterSourceVideoFile::GetDimensionsRCommand, this, "GetRightDimensions");
        provided->AddCommandQualifiedRead(&svlFilterSourceVideoFile::GetPositionAtTimeLCommand, this, "GetPositionAtTime");
        provided->AddCommandQualifiedRead(&svlFilterSourceVideoFile::GetPositionAtTimeLCommand, this, "GetLeftPositionAtTime");
        provided->AddCommandQualifiedRead(&svlFilterSourceVideoFile::GetPositionAtTimeRCommand, this, "GetRightPositionAtTime");
        provided->AddCommandQualifiedRead(&svlFilterSourceVideoFile::GetTimeAtPositionLCommand, this, "GetTimeAtPosition");
        provided->AddCommandQualifiedRead(&svlFilterSourceVideoFile::GetTimeAtPositionLCommand, this, "GetLeftTimeAtPosition");
        provided->AddCommandQualifiedRead(&svlFilterSourceVideoFile::GetTimeAtPositionRCommand, this, "GetRightTimeAtPosition");
    }
}

void svlFilterSourceVideoFile::SetChannelsCommand(const int& channels)
{
    if (SetChannelCount(static_cast<unsigned int>(channels)) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "SetChannelsCommand: \"SetChannelCount(" << channels << ")\" returned error" << std::endl;
    }
}

void svlFilterSourceVideoFile::SetPathLCommand(const std::string& filepath)
{
    if (SetFilePath(filepath, SVL_LEFT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "SetPathLCommand: \"SetFilePath(\""
                                 << filepath
                                 << "\", 0)\" returned error"
                                 << std::endl;
    }
}

void svlFilterSourceVideoFile::SetPathRCommand(const std::string& filepath)
{
    if (SetFilePath(filepath, SVL_RIGHT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "SetPathRCommand: \"SetFilePath(\""
                                 << filepath
                                 << "\", 1)\" returned error"
                                 << std::endl;
    }
}

void svlFilterSourceVideoFile::SetPosLCommand(const int& position)
{
    if (SetPosition(position, SVL_LEFT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "SetPosLCommand: \"SetPosition("
                                 << position
                                 << ", 0)\" returned error"
                                 << std::endl;
    }
}

void svlFilterSourceVideoFile::SetPosRCommand(const int& position)
{
    if (SetPosition(position, SVL_RIGHT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "SetPosRCommand: \"SetPosition("
                                 << position
                                 << ", 1)\" returned error"
                                 << std::endl;
    }
}

void svlFilterSourceVideoFile::SetRangeLCommand(const vctInt2& range)
{
    if (SetRange(range, SVL_LEFT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "SetRangeLCommand: \"SetRange(vctInt2("
                                 << range[0]
                                 << ", "
                                 << range[1]
                                 << "), 0)\" returned error"
                                 << std::endl;
    }
}

void svlFilterSourceVideoFile::SetRangeRCommand(const vctInt2& range)
{
    if (SetRange(range, SVL_RIGHT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "SetRangeRCommand: \"SetRange(vctInt2("
                                 << range[0]
                                 << ", "
                                 << range[1]
                                 << "), 1)\" returned error"
                                 << std::endl;
    }
}

void svlFilterSourceVideoFile::GetChannelsCommand(int & channels) const
{
    channels = static_cast<int>(Codec.size());
}

void svlFilterSourceVideoFile::GetPathLCommand(std::string & filepath) const
{
    if (GetFilePath(filepath, SVL_LEFT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "GetPathLCommand: \"GetFilePath(., 0)\" returned error" << std::endl;
    }
}

void svlFilterSourceVideoFile::GetPathRCommand(std::string & filepath) const
{
    if (GetFilePath(filepath, SVL_RIGHT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "GetPathRCommand: \"GetFilePath(., 1)\" returned error" << std::endl;
    }
}

void svlFilterSourceVideoFile::GetLengthLCommand(int & length) const
{
    length = GetLength(SVL_LEFT);
    if (length < 0) {
        CMN_LOG_CLASS_INIT_ERROR << "GetLengthLCommand: codec configuration on channel 0 is invalid" << std::endl;
    } else if (length == 0) {
        CMN_LOG_CLASS_INIT_ERROR << "GetLengthLCommand: video file on channel 0 has not been opened" << std::endl;
    }
}

void svlFilterSourceVideoFile::GetLengthRCommand(int & length) const
{
    length = GetLength(SVL_RIGHT);
    if (length < 0) {
        CMN_LOG_CLASS_INIT_ERROR << "GetLengthRCommand: codec configuration on channel 1 is invalid" << std::endl;
    } else if (length == 0) {
        CMN_LOG_CLASS_INIT_ERROR << "GetLengthRCommand: video file on channel 1 has not been opened" << std::endl;
    }
}

void svlFilterSourceVideoFile::GetPosLCommand(int & position) const
{
    position = GetPosition(SVL_LEFT);
    if (position < 0) {
        CMN_LOG_CLASS_INIT_ERROR << "GetPosLCommand: \"GetPosition(., 0)\" returned error" << std::endl;
    }
}

void svlFilterSourceVideoFile::GetPosRCommand(int & position) const
{
    position = GetPosition(SVL_RIGHT);
    if (position < 0) {
        CMN_LOG_CLASS_INIT_ERROR << "GetPosRCommand: \"GetPosition(., 1)\" returned error" << std::endl;
    }
}

void svlFilterSourceVideoFile::GetRangeLCommand(vctInt2 & range) const
{
    if (GetRange(range, SVL_LEFT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "GetRangeLCommand: \"GetRange(., 0)\" returned error" << std::endl;
    }
}

void svlFilterSourceVideoFile::GetRangeRCommand(vctInt2 & range) const
{
    if (GetRange(range, SVL_RIGHT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "GetRangeRCommand: \"GetRange(., 1)\" returned error" << std::endl;
    }
}

void svlFilterSourceVideoFile::GetDimensionsLCommand(vctInt2 & dimensions) const
{
    dimensions[0] = static_cast<int>(GetWidth(SVL_LEFT));
    dimensions[1] = static_cast<int>(GetHeight(SVL_LEFT));
    if (dimensions[0] <= 0 || dimensions[1] <= 0) {
        CMN_LOG_CLASS_INIT_ERROR << "GetDimensionsLCommand: invalid image dimensions; "
                                 << "make sure the stream is already initialized"
                                 << std::endl;
    }
}

void svlFilterSourceVideoFile::GetDimensionsRCommand(vctInt2 & dimensions) const
{
    dimensions[0] = static_cast<int>(GetWidth(SVL_RIGHT));
    dimensions[1] = static_cast<int>(GetHeight(SVL_RIGHT));
    if (dimensions[0] <= 0 || dimensions[1] <= 0) {
        CMN_LOG_CLASS_INIT_ERROR << "GetDimensionsRCommand: invalid image dimensions; "
                                 << "make sure the stream is already initialized"
                                 << std::endl;
    }
}

void svlFilterSourceVideoFile::GetPositionAtTimeLCommand(const double & time, int & position) const
{
    position = GetPositionAtTime(time, SVL_LEFT);
    if (position < 0) {
        CMN_LOG_CLASS_INIT_ERROR << "GetPositionAtTimeLCommand: codec configuration on channel 0 is invalid" << std::endl;
    }
}

void svlFilterSourceVideoFile::GetPositionAtTimeRCommand(const double & time, int & position) const
{
    position = GetPositionAtTime(time, SVL_RIGHT);
    if (position < 0) {
        CMN_LOG_CLASS_INIT_ERROR << "GetPositionAtTimeRCommand: codec configuration on channel 1 is invalid" << std::endl;
    }
}

void svlFilterSourceVideoFile::GetTimeAtPositionLCommand(const int & position, double & time) const
{
    time = GetTimeAtPosition(position, SVL_LEFT);
    if (time < 0.0) {
        CMN_LOG_CLASS_INIT_ERROR << "GetTimeAtPositionLCommand: codec configuration on channel 0 is invalid" << std::endl;
    }
}

void svlFilterSourceVideoFile::GetTimeAtPositionRCommand(const int & position, double & time) const
{
    time = GetTimeAtPosition(position, SVL_RIGHT);
    if (time < 0.0) {
        CMN_LOG_CLASS_INIT_ERROR << "GetTimeAtPositionRCommand: codec configuration on channel 1 is invalid" << std::endl;
    }
}

