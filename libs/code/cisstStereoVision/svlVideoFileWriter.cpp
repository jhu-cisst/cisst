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

#include <cisstStereoVision/svlVideoFileWriter.h>

#include "svlConverters.h"
#include "zlib.h"

#if (CISST_OS == CISST_WINDOWS)
    #include "commdlg.h"
#endif

/*********************************************/
/*** svlVideoFileWriter class ****************/
/*********************************************/

svlVideoFileWriter::svlVideoFileWriter() :
    svlFilterBase(),
    CaptureLength(-1), // Continuous saving by default
    CompressionLevel(1)
{
    AddSupportedType(svlTypeImageRGB, svlTypeImageRGB);
    AddSupportedType(svlTypeImageRGBStereo, svlTypeImageRGBStereo);

    SaveBuffer[0] = SaveBuffer[1] = 0;
}

svlVideoFileWriter::~svlVideoFileWriter()
{
    Release();
}

int svlVideoFileWriter::Initialize(svlSample* inputdata)
{
    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(inputdata);

    SaveInitialized = false;

    Release();

    if (VideoFile.size() < img->GetVideoChannels()) return SVL_FAIL;

    const std::string filestartmarker = "CisstSVLVideo\r\n";
    int writelen;
    unsigned int i, uivalue;

    SaveBufferSize = 0;

    for (i = 0; i < img->GetVideoChannels(); i ++) {
        if (Disabled[i]) continue;

        VideoFile[i] = fopen(FilePath[i].c_str(), "wb");
        if (VideoFile[i] == 0) goto labError;

        // Write "file start marker"
        writelen = static_cast<int>(fwrite(filestartmarker.c_str(), filestartmarker.length(), 1, VideoFile[i]));
	    if (writelen < 1) goto labError;

	    // Write "width"
        uivalue = img->GetWidth(i);
        writelen = static_cast<int>(fwrite(&uivalue, sizeof(unsigned int), 1, VideoFile[i]));
	    if (writelen < 1) goto labError;

        // Write "height"
        uivalue = img->GetHeight(i);
        writelen = static_cast<int>(fwrite(&uivalue, sizeof(unsigned int), 1, VideoFile[i]));
	    if (writelen < 1) goto labError;
        // Compute YUV buffer size
        YUVBufferSize[i] = img->GetWidth(i) * img->GetHeight(i) * 2;
        // Allocate YUV buffer
        YUVBuffer[i] = new unsigned char[YUVBufferSize[i]];

        // Compute data size and add some additional room for the compressor
        CompressedBufferSize[i] = img->GetDataSize(i) + img->GetDataSize(i) / 100 + 4096;
        // Allocate compression buffer
        CompressedBuffer[i] = new unsigned char[CompressedBufferSize[i]];

        SaveBufferUsed[i] = 0;
        SaveBufferOffset[i] = SaveBufferSize;
        SaveBufferSize += CompressedBufferSize[i];
    }

    // Initialize video frame counter
    VideoFrameCounter = 0;

    // Initialize data saving double buffer
    SaveBuffer[0] = new unsigned char[SaveBufferSize];
    SaveBuffer[1] = new unsigned char[SaveBufferSize];
    SaveBufferUsedID = 0;

    // Start data saving thread
    SaveInitialized = false;
    KillSaveThread = false;
    SaveThread.Create<svlVideoFileWriter, unsigned int>(this, &svlVideoFileWriter::SaveProc, img->GetVideoChannels());
    SaveInitEvent.Wait();
    if (SaveInitialized == false) goto labError;

    OutputData = inputdata;

    return SVL_OK;

labError:
    Release();
    return SVL_FAIL;
}

int svlVideoFileWriter::OnStart(unsigned int procCount)
{
    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(OutputData);

    if (VideoFrameCounter == 0) {
        for (unsigned int i = 0; i < img->GetVideoChannels(); i ++) {
            CompressedPartOffset[i] = new unsigned int[procCount];
            CompressedPartSize[i] = new unsigned int[procCount];

            if (Disabled[i]) continue;

            // Write "thread count"
	        if (static_cast<int>(fwrite(&procCount, sizeof(unsigned int), 1, VideoFile[i])) < 1) return SVL_FAIL;
        }
    }

    return SVL_OK;
}

int svlVideoFileWriter::ProcessFrame(ProcInfo* procInfo, svlSample* inputdata)
{
    // Passing the same image for the next filter
    OutputData = inputdata;

    // Do nothing if recording is paused
    if (CaptureLength == 0) return SVL_OK;

    // Check for video saving errors
    if (SaveThreadError) return SVL_FAIL;

    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(OutputData);
    const std::string framestartmarker = "\r\nFrame\r\n";
    const unsigned int videochannels = img->GetVideoChannels();
    const unsigned int procid = procInfo->id;
    const unsigned int proccount = procInfo->count;
    const unsigned int savebufferid = SaveBufferUsedID ? 0 : 1;
    const double timestamp = inputdata->GetTimestamp();
    unsigned int i, j, start, end, size, width, height, offset;
    unsigned long comprsize;
    int err, ret = SVL_OK;

    for (i = 0; i < videochannels; i ++) {
        if (Disabled[i]) continue;

        // Compute part size and offset
        width = img->GetWidth(i);
        height = img->GetHeight(i);
        size = height / proccount + 1;
        comprsize = CompressedBufferSize[i] / proccount;
        start = procid * size;
        if (start >= height) continue;
        end = start + size;
        if (end > height) end = height;
        offset = start * width;
        size = width * (end - start);
        CompressedPartOffset[i][procid] = procid * comprsize;

        // Convert RGB to YUV422 planar format
        RGB24toYUV422P(reinterpret_cast<unsigned char*>(img->GetPointer(i)) + offset * 3,
                       YUVBuffer[i] + offset * 2,
                       size);

        // Compress part
        err = compress2(CompressedBuffer[i] + CompressedPartOffset[i][procid],
                        &comprsize,
                        YUVBuffer[i] + offset * 2,
                        size * 2,
                        CompressionLevel);
        if (err != Z_OK) {
            ret = SVL_FAIL;
            break;
        }

        CompressedPartSize[i][procid] = comprsize;
    }

    _SynchronizeThreads(procInfo);

    _OnSingleThread(procInfo)
    {
        WriteDoneEvent.Wait();

        for (i = 0; i < videochannels; i ++) {
            SaveBufferUsed[i] = 0;

            // Add "frame start marker"
            memcpy(SaveBuffer[savebufferid] + SaveBufferOffset[i] + SaveBufferUsed[i],
                   framestartmarker.c_str(),
                   framestartmarker.length());
            SaveBufferUsed[i] += framestartmarker.length();

            // Add "time stamp"
            memcpy(SaveBuffer[savebufferid] + SaveBufferOffset[i] + SaveBufferUsed[i],
                   &timestamp,
                   sizeof(double));
            SaveBufferUsed[i] += sizeof(double);

            for (j = 0; j < proccount; j ++) {
                // Add "compressed part size"
                memcpy(SaveBuffer[savebufferid] + SaveBufferOffset[i] + SaveBufferUsed[i],
                       &(CompressedPartSize[i][j]),
                       sizeof(unsigned int));
                SaveBufferUsed[i] += sizeof(unsigned int);

                // Add compressed frame
                memcpy(SaveBuffer[savebufferid] + SaveBufferOffset[i] + SaveBufferUsed[i],
                       CompressedBuffer[i] + CompressedPartOffset[i][j],
                       CompressedPartSize[i][j]);
                SaveBufferUsed[i] += CompressedPartSize[i][j];
            }
        }

        // Signal data saving thread to start writing
        SaveBufferUsedID = savebufferid;
        NewFrameEvent.Raise();

        if (CaptureLength > 0) CaptureLength --;
    }

    return ret;
}

int svlVideoFileWriter::Release()
{
    // Stop data saving thread
    KillSaveThread = true;
    if (SaveInitialized) {
        NewFrameEvent.Raise();
        SaveThread.Wait();
    }

    for (unsigned int i = 0; i < VideoFile.size(); i ++) {
        if (VideoFile[i]) fclose(VideoFile[i]);
        VideoFile[i] = 0;

        if (YUVBuffer[i]) {
            delete [] YUVBuffer[i];
            YUVBuffer[i] = 0;
        }
        if (CompressedBuffer[i]) {
            delete [] CompressedBuffer[i];
            CompressedBuffer[i] = 0;
        }
        if (CompressedPartOffset[i]) {
            delete [] CompressedPartOffset[i];
            CompressedPartOffset[i] = 0;
        }
        if (CompressedPartSize[i]) {
            delete [] CompressedPartSize[i];
            CompressedPartSize[i] = 0;
        }
        YUVBufferSize[i] = 0;
        CompressedBufferSize[i] = 0;
    }

    if (SaveBuffer[0]) {
        delete [] SaveBuffer[0];
        SaveBuffer[0] = 0;
    }
    if (SaveBuffer[1]) {
        delete [] SaveBuffer[1];
        SaveBuffer[1] = 0;
    }

    return SVL_OK;
}

int svlVideoFileWriter::DialogFilePath(unsigned int videoch)
{
    if (IsInitialized() == true)
        return SVL_FAIL;
    if (UpdateStreamCount(videoch + 1) != SVL_OK)
        return SVL_FAIL;

#if (CISST_OS == CISST_WINDOWS)
    OPENFILENAME ofn;
    char path[2048], title[256];
    char filter[] = "CISST Video Files (*.cvi) *.cvi All Files (*.*) *.* ";

    memset(path, 0, 2048);
    sprintf(title, "Save Video File for [channel #%d] As", videoch);
    filter[25] = filter[31] = filter[47] = filter[51] = 0;

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
        if (FilePath[videoch].rfind(".cvi") != FilePath[videoch].length() - 4) {
            if (FilePath[videoch].at(FilePath[videoch].length() - 1) == '.') FilePath[videoch] += "cvi";
            else FilePath[videoch] += ".cvi";
        }
        return SVL_OK;
    }
#else
    std::cout << "Enter filename for [channel #" << videoch << "]: ";
    std::cin >> FilePath[videoch];
    if (FilePath[videoch].rfind(".cvi") != FilePath[videoch].length() - 4) {
        if (FilePath[videoch].at(FilePath[videoch].length() - 1) == '.') FilePath[videoch] += "cvi";
        else FilePath[videoch] += ".cvi";
    }
#endif

    return SVL_FAIL;
}

int svlVideoFileWriter::Disable(bool disable, unsigned int videoch)
{
    if (IsInitialized() == true)
        return SVL_FAIL;
    if (UpdateStreamCount(videoch + 1) != SVL_OK)
        return SVL_FAIL;

    Disabled[videoch] = disable;

    return SVL_OK;
}

int svlVideoFileWriter::SetFilePath(const std::string filepath, unsigned int videoch)
{
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;
    if (UpdateStreamCount(videoch + 1) != SVL_OK)
        return SVL_FAIL;

    FilePath[videoch] = filepath;

    return SVL_OK;
}

void svlVideoFileWriter::SetCompressionLevel(unsigned int level)
{
    CompressionLevel = std::min(level, 9u);
}

int svlVideoFileWriter::UpdateStreamCount(unsigned int count)
{
    if (count > 2) return SVL_FAIL;

    unsigned int prevsize = VideoFile.size();
    if (prevsize < count) {
        VideoFile.resize(count);
        Disabled.resize(count);
        FilePath.resize(count);
        SaveBufferUsed.resize(count);
        SaveBufferOffset.resize(count);
        YUVBuffer.resize(count);
        YUVBufferSize.resize(count);
        CompressedBuffer.resize(count);
        CompressedBufferSize.resize(count);
        CompressedPartOffset.resize(count);
        CompressedPartSize.resize(count);
        for (unsigned int i = prevsize; i < count; i ++) {
            VideoFile[i] = 0;
            Disabled[i] = false;
            SaveBufferUsed[i] = 0;
            SaveBufferOffset[i] = 0;
            YUVBuffer[i] = 0;
            CompressedBuffer[i] = 0;
            CompressedPartOffset[i] = 0;
            CompressedPartSize[i] = 0;
        }
    }

    return SVL_OK;
}

void* svlVideoFileWriter::SaveProc(unsigned int videochannels)
{
    SaveThreadError = false;
    SaveInitialized = true;
    SaveInitEvent.Raise();
    WriteDoneEvent.Raise();

    unsigned int i;

    while (1) {
        // Wait for new frame to arrive
        NewFrameEvent.Wait();
        if (KillSaveThread || SaveThreadError) break;

        // Save frame(s)
        for (i = 0; i < videochannels; i ++) {
            if (fwrite(SaveBuffer[SaveBufferUsedID] + SaveBufferOffset[i],
                       1,
                       SaveBufferUsed[i],
                       VideoFile[i]) < SaveBufferUsed[i]) {
                // Write error
                SaveThreadError = true;
                return this;
            }
        }
        // Signal that write is done
        WriteDoneEvent.Raise();
    }

    return this;
}


