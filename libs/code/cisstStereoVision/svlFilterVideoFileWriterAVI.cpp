/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

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

#include <cisstStereoVision/svlFilterVideoFileWriterAVI.h>

#include "VfWAvi.h"
#include "commdlg.h"

static int VFW_OleInitCounter = 0;


/*********************************************/
/*** svlFilterVideoFileWriterAVI class *******/
/*********************************************/

svlFilterVideoFileWriterAVI::svlFilterVideoFileWriterAVI() :
    svlFilterBase(),
    Action(false),
    ActionTime(0.0),
    CaptureLength(-1), // Continuous saving by default
    Framerate(30.0),
    KeyFrames(30),
    CompressOptions(0)
{
    AddSupportedType(svlTypeImageRGB, svlTypeImageRGB);
    AddSupportedType(svlTypeImageRGBStereo, svlTypeImageRGBStereo);

    if (VFW_OleInitCounter < 1) {
        CoInitialize(0);
        VFW_OleInitCounter = 1;
    }

    TimeServer = new osaTimeServer;
    TimeServer->SetTimeOrigin();
}

svlFilterVideoFileWriterAVI::~svlFilterVideoFileWriterAVI()
{
    Release();

    if (VFW_OleInitCounter > 0) {
        CoUninitialize();
        VFW_OleInitCounter = 0;
    }
    if (CompressOptions) delete reinterpret_cast<AVICOMPRESSOPTIONS*>(CompressOptions);

    delete TimeServer;
}

int svlFilterVideoFileWriterAVI::Initialize(svlSample* inputdata)
{
    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(inputdata);

    Release();

    if (VideoObj.size() < img->GetVideoChannels()) return SVL_FAIL;

    CVfWAvi* pavi;
    AVICOMPRESSOPTIONS* coptions = reinterpret_cast<AVICOMPRESSOPTIONS*>(CompressOptions);

    for (unsigned int i = 0; i < img->GetVideoChannels(); i ++) {
        if (Disabled[i]) continue;

        VideoObj[i] = pavi = new CVfWAvi;
        if (coptions) pavi->SetCompressOptions(coptions[0]);
        if (pavi->InitCreation(FilePath[i].c_str(),
                               img->GetWidth(i),
                               img->GetHeight(i),
                               Framerate) != 1)  goto labError;
        pavi->SetKeyFrameInterval(KeyFrames);
    }

    // Initialize video frame counter
    VideoFrameCounter = 0;

    OutputData = inputdata;

    return SVL_OK;

labError:
    Release();
    return SVL_FAIL;
}

int svlFilterVideoFileWriterAVI::OnStart(unsigned int procCount)
{
    CaptureLength = TargetCaptureLength;
    ActionTime = TargetActionTime;
    Action = false;

    return SVL_OK;
}

int svlFilterVideoFileWriterAVI::ProcessFrame(ProcInfo* procInfo, svlSample* inputdata)
{
    // Passing the same image for the next filter
    OutputData = inputdata;

    _OnSingleThread(procInfo) {
        if (Action) {
            CaptureLength = TargetCaptureLength;
            ActionTime = TargetActionTime;
            Action = false;
        }
    }
    _SynchronizeThreads(procInfo);

    if (CaptureLength == 0) {
        if (ActionTime < inputdata->GetTimestamp()) return SVL_OK;
        // Process remaining samples in the buffer when paused
    }
    else {
        // Drop frames when restarted
        if (ActionTime > inputdata->GetTimestamp()) return SVL_OK;
    }

    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(OutputData);
    unsigned int videochannels = img->GetVideoChannels();
    unsigned int idx;
    int ret = SVL_OK;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        if (reinterpret_cast<CVfWAvi*>(VideoObj[idx])->AddFrameToAVI(img->GetUCharPointer(idx),
                                                                     img->GetDataSize(idx)) == 0) ret = SVL_FAIL;
    }

    _SynchronizeThreads(procInfo);

    _OnSingleThread(procInfo)
    {
        if (CaptureLength > 0) CaptureLength --;
    }

    return ret;
}

int svlFilterVideoFileWriterAVI::Release()
{
    for (unsigned int i = 0; i < VideoObj.size(); i ++) {
        if (VideoObj[i]) delete reinterpret_cast<CVfWAvi*>(VideoObj[i]);
        VideoObj[i] = 0;
    }

    return SVL_OK;
}

int svlFilterVideoFileWriterAVI::DialogFilePath(unsigned int videoch)
{
    if (IsInitialized() == true)
        return SVL_FAIL;
    if (UpdateStreamCount(videoch + 1) != SVL_OK)
        return SVL_FAIL;

    OPENFILENAME ofn;
    char path[2048], title[256];
    char filter[] = "Video Files (*.avi) *.avi All Files (*.*) *.* ";

    memset(path, 0, 2048);
    sprintf(title, "Save Video File for [channel #%d] As", videoch);
    filter[19] = filter[25] = filter[41] = filter[45] = 0;

    memset(&ofn, 0, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = GetForegroundWindow();
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile= path;
    ofn.nMaxFile = 2048;
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrTitle = title;

    // Removing the keyboard focus from the parent window
    SetForegroundWindow(GetDesktopWindow());

    if (GetSaveFileName(&ofn)) {
        FilePath[videoch] = path;
        return SVL_OK;
    }

    return SVL_FAIL;
}

int svlFilterVideoFileWriterAVI::DialogCodec()
{
    if (IsInitialized() == true)
        return SVL_FAIL;

    if (CompressOptions == 0) {
        CompressOptions = new AVICOMPRESSOPTIONS;
        memset(CompressOptions, 0, sizeof(AVICOMPRESSOPTIONS));
    }

    CVfWAvi tavi;
    if (tavi.ShowCompressionDialog()) {
        tavi.GetCompressOptions(reinterpret_cast<AVICOMPRESSOPTIONS*>(CompressOptions)[0]);
        tavi.GetCompressorName(EncoderName);
        return SVL_OK;
    }

    return SVL_FAIL;
}

int svlFilterVideoFileWriterAVI::Disable(bool disable, unsigned int videoch)
{
    if (IsInitialized() == true)
        return SVL_FAIL;
    if (UpdateStreamCount(videoch + 1) != SVL_OK)
        return SVL_FAIL;

    Disabled[videoch] = disable;

    return SVL_OK;
}

int svlFilterVideoFileWriterAVI::SetFilePath(const std::string filepath, unsigned int videoch)
{
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;
    if (UpdateStreamCount(videoch + 1) != SVL_OK)
        return SVL_FAIL;

    FilePath[videoch] = filepath;

    return SVL_OK;
}

int svlFilterVideoFileWriterAVI::SetFramerate(double fps)
{
    if (IsInitialized() == true)
        return SVL_FAIL;

    if (fps > 0.0) {
        Framerate = fps;
        return SVL_OK;
    }

    return SVL_FAIL;
}

int svlFilterVideoFileWriterAVI::SetKeyFrameInteval(unsigned int interval)
{
    if (interval == 0) return SVL_FAIL;

    KeyFrames = interval;

    return SVL_OK;
}

void svlFilterVideoFileWriterAVI::Pause()
{
    // Get current absolute time
    osaAbsoluteTime abstime;
    TimeServer->RelativeToAbsolute(TimeServer->GetRelativeTime(), abstime);
    TargetActionTime = abstime.sec + abstime.nsec / 1000000000.0;

    TargetCaptureLength = 0;
    Action = true;
}

void svlFilterVideoFileWriterAVI::Record(int frames)
{
    // Get current absolute time
    osaAbsoluteTime abstime;
    TimeServer->RelativeToAbsolute(TimeServer->GetRelativeTime(), abstime);
    TargetActionTime = abstime.sec + abstime.nsec / 1000000000.0;

    TargetCaptureLength = frames;
    Action = true;
}

int svlFilterVideoFileWriterAVI::SaveCodecSettings(const std::string filepath)
{
    int writelen, ret = SVL_FAIL;
    unsigned int uivalue;
	FILE *fp;

    fp = fopen(filepath.c_str(), "wb");
    if (fp == 0) goto labError;

	// Write "frame rate"
    writelen = static_cast<int>(fwrite(&Framerate, sizeof(double), 1, fp));
	if (writelen < 1) goto labError;

	// Write "key frames"
    writelen = static_cast<int>(fwrite(&KeyFrames, sizeof(unsigned int), 1, fp));
	if (writelen < 1) goto labError;

	// Write "compression options size"
    if (CompressOptions) {
        uivalue = sizeof(AVICOMPRESSOPTIONS);
    }
    else {
        uivalue = 0;
    }
    writelen = static_cast<int>(fwrite(&uivalue, sizeof(unsigned int), 1, fp));
	if (writelen < 1) goto labError;

    // Write "compression options"
    if (CompressOptions) {
        writelen = static_cast<int>(fwrite(CompressOptions, 1, uivalue, fp));
        if (writelen < static_cast<int>(uivalue)) goto labError;
    }

    // Write "compressor name"
    char codecname[64];
    memset(codecname, 0, 64);
    memcpy(codecname, EncoderName.c_str(), std::max(static_cast<std::string::size_type>(63), EncoderName.length()));
    writelen = static_cast<int>(fwrite(codecname, 1, 64, fp));
    if (writelen < 64) goto labError;

    ret = SVL_OK;

labError:
    if (fp) fclose(fp);

    return ret;
}

int svlFilterVideoFileWriterAVI::LoadCodecSettings(const std::string filepath)
{
    if (IsInitialized() == true)
        return SVL_FAIL;

    int readlen, ret = SVL_FAIL;
    double dbvalue;
    unsigned int uivalue;
	FILE *fp;

    fp = fopen(filepath.c_str(), "rb");
    if (fp == 0) goto labError;

	// Read "frame rate"
    readlen = static_cast<int>(fread(&dbvalue, sizeof(double), 1, fp));
	if (readlen < 1) goto labError;
	if (dbvalue > 0.0 || dbvalue <= 1000.0) Framerate = dbvalue;

	// Read "key frames"
    readlen = static_cast<int>(fread(&KeyFrames, sizeof(unsigned int), 1, fp));
	if (readlen < 1) goto labError;

	// Read "compression options size"
    readlen = static_cast<int>(fread(&uivalue, sizeof(unsigned int), 1, fp));
	if (readlen < 1) goto labError;

    // Read "compression options"
    if (uivalue > 0) {
        if (uivalue != sizeof(AVICOMPRESSOPTIONS)) goto labError;
        if (CompressOptions == 0) CompressOptions = new AVICOMPRESSOPTIONS;
    }
    if (uivalue > 0) {
        readlen = static_cast<int>(fread(CompressOptions, 1, uivalue, fp));
        if (readlen < static_cast<int>(uivalue)) goto labError;
        AVICOMPRESSOPTIONS* tco = reinterpret_cast<AVICOMPRESSOPTIONS*>(CompressOptions);
        tco->lpFormat = 0;
        tco->cbFormat = 0;
        tco->cbParms = 0;
        tco->lpParms = 0;
    }

    // Read "compressor name"
    char codecname[64];
    readlen = static_cast<int>(fread(codecname, 1, 64, fp));
    if (readlen < 64) goto labError;
    codecname[63] = 0;
    EncoderName = codecname;

    ret = SVL_OK;

labError:
    if (fp) fclose(fp);

    return ret;
}

int svlFilterVideoFileWriterAVI::UpdateStreamCount(unsigned int count)
{
    if (count > 2) return SVL_FAIL;

    unsigned int prevsize = VideoObj.size();
    if (prevsize < count) {
        VideoObj.resize(count);
        Disabled.resize(count);
        FilePath.resize(count);
        for (unsigned int i = prevsize; i < count; i ++) {
            VideoObj[i] = 0;
            Disabled[i] = false;
        }
    }

    return SVL_OK;
}

