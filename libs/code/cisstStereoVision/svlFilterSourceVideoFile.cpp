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

#include <cisstStereoVision/svlFilterSourceVideoFile.h>
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

CMN_IMPLEMENT_SERVICES(svlFilterSourceVideoFile)

svlFilterSourceVideoFile::svlFilterSourceVideoFile() :
    svlFilterSourceBase(false),  // manual timestamp management
    cmnGenericObject(),
    Framerate(-1.0),
    FirstTimestamp(-1.0)
{
    TargetFrequency = -1.0;
    OutputData = 0;
}

svlFilterSourceVideoFile::svlFilterSourceVideoFile(unsigned int channelcount) :
    svlFilterSourceBase(false),  // manual timestamp management
    cmnGenericObject(),
    Framerate(-1.0),
    FirstTimestamp(-1.0)
{
    TargetFrequency = -1.0;
    OutputData = 0;

    SetChannelCount(channelcount);
}

svlFilterSourceVideoFile::~svlFilterSourceVideoFile()
{
    Release();

    if (OutputData) delete OutputData;
}

int svlFilterSourceVideoFile::SetChannelCount(unsigned int channelcount)
{
    if (OutputData) return SVL_FAIL;

    if (channelcount == 1) {
        AddSupportedType(svlTypeImageRGB);
        OutputData = new svlSampleImageRGB;
    }
    else if (channelcount == 2) {
        AddSupportedType(svlTypeImageRGBStereo);
        OutputData = new svlSampleImageRGBStereo;
    }
    else return SVL_FAIL;

    Codec.SetSize(channelcount);
    Codec.SetAll(0);
    FilePath.SetSize(channelcount);

    return SVL_OK;
}

int svlFilterSourceVideoFile::Initialize()
{
    if (OutputData == 0) return SVL_FAIL;

    Release();

    svlSampleImageBase* image = dynamic_cast<svlSampleImageBase*>(OutputData);
    unsigned int width, height;
    double framerate;
    int ret = SVL_OK;

    for (unsigned int i = 0; i < image->GetVideoChannels(); i ++) {

        // Get video codec for file extension
        Codec[i] = svlVideoIO::GetCodec(FilePath[i]);
        // Open video file
        if (!Codec[i] || Codec[i]->Open(FilePath[i], width, height, framerate) != SVL_OK) {
            ret = SVL_FAIL;
            break;
        }

        if (i == 0) {
            // The first video channel defines the video
            // framerate of all channels in the stream
            Framerate = framerate;
        }

        // Create image sample of matching dimensions
        image->SetSize(i, width, height);
    }

    // Initialize timestamp for case of timestamp errors
    OutputData->SetTimestamp(0.0);

    if (ret != SVL_OK) Release();
    return ret;
}

int svlFilterSourceVideoFile::OnStart(unsigned int CMN_UNUSED(procCount))
{
    if (TargetFrequency < 0.1) TargetFrequency = Framerate;
    RestartTargetTimer();

    return SVL_OK;
}

int svlFilterSourceVideoFile::ProcessFrame(svlProcInfo* procInfo)
{
    // Try to keep TargetFrequency
    _OnSingleThread(procInfo) WaitForTargetTimer();

    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(OutputData);
    unsigned int idx, videochannels = img->GetVideoChannels();
    double timestamp, timespan;
    int ret = SVL_OK;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        if (Codec[idx]) {
            ret = Codec[idx]->Read(0, *img, idx, true);
            if (ret == SVL_VID_END_REACHED) {
                if (!LoopFlag) ret = SVL_STOP_REQUEST;
                else {
                    // Loop around
                    ret = Codec[idx]->Read(0, *img, idx, true);
                }
            }
            if (ret != SVL_OK) break;

            if (idx == 0) {
                if (!IsTargetTimerRunning()) {

                    // Synchronizing all channels to channel #0
                    timestamp = Codec[idx]->GetTimestamp();
                    if (timestamp > 0.0) {

                        // Try to keep orignal frame intervals
                        if (Codec[idx]->GetPos() == 1) {
                            FirstTimestamp = timestamp;
                            Timer.Reset();
                            Timer.Start();
                        }
                        else {
                            timespan = (timestamp - FirstTimestamp) - Timer.GetElapsedTime();
                            if (timespan > 0.0) osaSleep(timespan);
                        }

                        // Set timestamp to the one stored in the video file
                        OutputData->SetTimestamp(timestamp);

                        continue;
                    }
                }

                // Ask Stream Manager for current timestamp
                OutputData->SetTimestamp(-1.0);
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

    Framerate = -1.0;

    return SVL_OK;
}

int svlFilterSourceVideoFile::DialogFilePath(unsigned int videoch)
{
    if (OutputData == 0) return SVL_FAIL;
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;

    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(OutputData);
    if (videoch >= img->GetVideoChannels()) return SVL_WRONG_CHANNEL;

    std::ostringstream out;
    out << "Open video file [channel #" << videoch << "]";
    std::string title(out.str());

    return svlVideoIO::DialogFilePath(false, title, FilePath[videoch]);
}

int svlFilterSourceVideoFile::SetFilePath(const std::string &filepath, unsigned int videoch)
{
    if (OutputData == 0) return SVL_FAIL;
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;

    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(OutputData);
    if (videoch >= img->GetVideoChannels()) return SVL_WRONG_CHANNEL;

    FilePath[videoch] = filepath;

    return SVL_OK;
}

int svlFilterSourceVideoFile::GetFilePath(std::string &filepath, unsigned int videoch) const
{
    if (FilePath.size() <= videoch) return SVL_FAIL;
    filepath = FilePath[videoch];
    return SVL_OK;
}

