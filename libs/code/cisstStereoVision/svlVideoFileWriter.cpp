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
    YUVBuffer(0),
    YUVBufferSize(0),
    CompressedBuffer(0),
    CompressedBufferSize(0),
    CompressedPartOffset(0),
    CompressedPartSize(0)
{
    AddSupportedType(svlTypeImageRGB, svlTypeImageRGB);
    AddSupportedType(svlTypeImageRGBStereo, svlTypeImageRGBStereo);
}

svlVideoFileWriter::~svlVideoFileWriter()
{
    Release();
}

int svlVideoFileWriter::Initialize(svlSample* inputdata)
{
    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(inputdata);

    Release();

    if (VideoFile.size() < img->GetVideoChannels()) return SVL_FAIL;

    const std::string filestartmarker = "CisstSVLVideo\r\n";
    int writelen;
    unsigned int uivalue;
    unsigned int maxdatasize = 0;

    for (unsigned int i = 0; i < img->GetVideoChannels(); i ++) {
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

        if (maxdatasize < img->GetDataSize(i)) maxdatasize = img->GetDataSize(i);
    }

    // Compute YUV buffer size
    YUVBufferSize = maxdatasize * 2 / 3;
    // Allocate YUV buffer
    YUVBuffer = new unsigned char[YUVBufferSize];

    // Compute data size and add some additional room for the compressor
    CompressedBufferSize = maxdatasize + maxdatasize / 100 + 4096;
    // Allocate compression buffer
    CompressedBuffer = new unsigned char[CompressedBufferSize];

    // Initialize video frame counter
    VideoFrameCounter = 0;

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
        CompressedPartOffset = new unsigned int[procCount];
        CompressedPartSize = new unsigned int[procCount];

        for (unsigned int i = 0; i < img->GetVideoChannels(); i ++) {
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

    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(OutputData);
    const std::string framestartmarker = "\r\nFrame\r\n";
    const unsigned int videochannels = img->GetVideoChannels();
    const unsigned int procid = procInfo->id;
    const unsigned int proccount = procInfo->count;
    const double timestamp = inputdata->GetTimestamp();
    unsigned int i, j, start, end, size, width, height, offset;
    unsigned long comprsize;
    int err, ret = SVL_OK;

    for (i = 0; i < videochannels; i ++) {
        if (Disabled[i]) continue;

        _OnSingleThread(procInfo)
        {
            // Convert RGB to YUV422 planar format
            RGB24toYUV422P(reinterpret_cast<unsigned char*>(img->GetPointer(i)),
                           YUVBuffer,
                           img->GetWidth(i) * img->GetHeight(i));
        }

        _SynchronizeThreads(procInfo);

        // Compute part size and offset
        width = img->GetWidth(i);
        height = img->GetHeight(i);
        size = height / proccount + 1;
        comprsize = CompressedBufferSize / proccount;
        start = procid * size;
        if (start >= height) break;
        end = start + size;
        if (end > height) end = height;
        offset = start * width;
        size = width * (end - start);
        CompressedPartOffset[procid] = procid * comprsize;

        // Compress part
        err = compress2(CompressedBuffer + CompressedPartOffset[procid],
                        &comprsize,
                        YUVBuffer + offset * 2,
                        size * 2,
                        1); // compression level [0,9]
        if (err != Z_OK) {
            ret = SVL_FAIL;
            break;
        }

        CompressedPartSize[procid] = comprsize;

        _SynchronizeThreads(procInfo);

        _OnSingleThread(procInfo)
        {
            // Write "frame start marker"
            if (static_cast<int>(fwrite(framestartmarker.c_str(),
                                        1,
                                        framestartmarker.length(),
                                        VideoFile[i])) < 1) {
                ret = SVL_FAIL;
                break;
            }

            // Write "time stamp"
            if (static_cast<int>(fwrite(&timestamp,
                                        sizeof(double),
                                        1,
                                        VideoFile[i])) < 1) {
                ret = SVL_FAIL;
                break;
            }

            for (j = 0; j < proccount; j ++) {
                // Write "compressed part size"
	            if (static_cast<int>(fwrite(&(CompressedPartSize[j]),
                                            sizeof(unsigned int),
                                            1,
                                            VideoFile[i])) < 1) {
                    ret = SVL_FAIL;
                    break;
                }
                // Write compressed frame
	            if (fwrite(CompressedBuffer + CompressedPartOffset[j],
                           1,
                           CompressedPartSize[j],
                           VideoFile[i]) < CompressedPartSize[j]) ret = SVL_FAIL;
            }
        }
    }

    _SynchronizeThreads(procInfo);

    _OnSingleThread(procInfo)
    {
        if (CaptureLength > 0) CaptureLength --;
    }

    return ret;
}

int svlVideoFileWriter::Release()
{
    for (unsigned int i = 0; i < VideoFile.size(); i ++) {
        if (VideoFile[i]) fclose(VideoFile[i]);
        VideoFile[i] = 0;
    }

    if (YUVBuffer) {
        delete [] YUVBuffer;
        YUVBuffer = 0;
    }
    if (CompressedBuffer) {
        delete [] CompressedBuffer;
        CompressedBuffer = 0;
    }
    if (CompressedPartOffset) {
        delete [] CompressedPartOffset;
        CompressedPartOffset = 0;
    }
    if (CompressedPartSize) {
        delete [] CompressedPartSize;
        CompressedPartSize = 0;
    }
    YUVBufferSize = 0;
    CompressedBufferSize = 0;

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

int svlVideoFileWriter::UpdateStreamCount(unsigned int count)
{
    if (count > 2) return SVL_FAIL;

    unsigned int prevsize = VideoFile.size();
    if (prevsize < count) {
        VideoFile.resize(count);
        Disabled.resize(count);
        FilePath.resize(count);
        for (unsigned int i = prevsize; i < count; i ++) {
            VideoFile[i] = 0;
            Disabled[i] = false;
        }
    }

    return SVL_OK;
}

