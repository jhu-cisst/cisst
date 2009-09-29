/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlVideoFileSource.cpp 596 2009-07-27 17:01:59Z bvagvol1 $
  
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

#if (CISST_OS == CISST_WINDOWS)
    #include "VfWAvi.h"
    #include "commdlg.h"

    static int VFS_OleInitCounter = 0;
#endif

#if (CISST_SVL_HAS_ZLIB == ON)
    #include "zlib.h"
    #include <cisstStereoVision/svlConverters.h>
#endif // CISST_SVL_HAS_ZLIB

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

svlFilterSourceVideoFile::svlFilterSourceVideoFile(bool stereo) :
    svlFilterSourceBase(false),  // manual timestamp management
    AVIFrequency(-1.0)
{
#if (CISST_OS == CISST_WINDOWS)
    if (VFS_OleInitCounter < 1) {
        CoInitialize(0);
        VFS_OleInitCounter = 1;
    }
#endif

    if (stereo) {
        AddSupportedType(svlTypeImageRGBStereo);
        OutputData = new svlSampleImageRGBStereo;
    }
    else {
        AddSupportedType(svlTypeImageRGB);
        OutputData = new svlSampleImageRGB;
    }

    unsigned int videochannels = dynamic_cast<svlSampleImageBase*>(OutputData)->GetVideoChannels();
    VideoObj.SetSize(videochannels);
    VideoObj.SetAll(0);
    VideoFile.SetSize(videochannels);
    VideoFile.SetAll(0);
    FilePath.SetSize(videochannels);
    FilePartCount.SetSize(videochannels);
    YUVBuffer.SetSize(videochannels);
    YUVBuffer.SetAll(0);
    YUVBufferSize.SetSize(videochannels);
    CompressedBuffer.SetSize(videochannels);
    CompressedBuffer.SetAll(0);
    CompressedBufferSize.SetSize(videochannels);
    FirstTimestamp.SetSize(videochannels);

    TargetFrequency = -1.0;
}

svlFilterSourceVideoFile::~svlFilterSourceVideoFile()
{
    Release();

    if (OutputData) delete OutputData;

#if (CISST_OS == CISST_WINDOWS)
    if (VFS_OleInitCounter > 0) {
        CoUninitialize();
        VFS_OleInitCounter = 0;
    }
#endif
}

int svlFilterSourceVideoFile::Initialize()
{
    Release();

    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(OutputData);

#if (CISST_OS == CISST_WINDOWS)
    CVfWAvi* tavi;
    unsigned int avicounter = 0;
    AVIFrequency = 0.0;
#endif
#if (CISST_SVL_HAS_ZLIB == ON)
    int readlen;
    unsigned int uivalue;
    const std::string filestartmarker = "CisstSVLVideo\r\n";
    char strbuffer[32];
#endif

    bool opened;
    unsigned int i, width, height;

    for (i = 0; i < img->GetVideoChannels(); i ++) {
        if (FilePath[i].empty()) return SVL_FAIL;
    }

    for (i = 0; i < img->GetVideoChannels(); i ++) {
        opened = false;

#if (CISST_OS == CISST_WINDOWS)
    // Try to open as an AVI first
        VideoObj[i] = tavi = new CVfWAvi;
        if (tavi->InitPlaying(FilePath[i].c_str()) > 0) {
            AVIFrequency += std::max(0.1, tavi->GetFramerate());
            width = tavi->GetWidth();
            height = tavi->GetHeight();
            avicounter ++;
            opened = true;
        }
        else {
            delete tavi;
            VideoObj[i] = 0;
        }
#endif
#if (CISST_SVL_HAS_ZLIB == ON)
    // Try to open as a CISST video
        while (opened == false) {
            VideoFile[i] = fopen(FilePath[i].c_str(), "rb");
            if (VideoFile[i] == 0) {
                // Try adding the "cvi" extension
                if (FilePath[i].at(FilePath[i].length() - 1) == '.') FilePath[i] += "cvi";
                else FilePath[i] += ".cvi";
                VideoFile[i] = fopen(FilePath[i].c_str(), "rb");
                if (VideoFile[i] == 0) break;
            }

	        // Read "file start marker"
            readlen = static_cast<int>(fread(strbuffer, filestartmarker.length(), 1, VideoFile[i]));
            if (readlen < 1) break;
            strbuffer[filestartmarker.length()] = 0;
            if (filestartmarker.compare(strbuffer) != 0) break;

	        // Read "width"
            readlen = static_cast<int>(fread(&width, sizeof(unsigned int), 1, VideoFile[i]));
            if (readlen < 1 || width < 1 || width > 4096) break;

            // Read "height"
            readlen = static_cast<int>(fread(&height, sizeof(unsigned int), 1, VideoFile[i]));
            if (readlen < 1 || height < 1 || height > 4096) break;

            // Read "part count"
            readlen = static_cast<int>(fread(&uivalue, sizeof(unsigned int), 1, VideoFile[i]));
            if (readlen < 1 || uivalue < 1 || uivalue > 256) break;
            FilePartCount[i] = uivalue;

            // Compute YUV buffer size
            YUVBufferSize[i] = width * height * 2;
            // Allocate YUV buffer
            YUVBuffer[i] = new unsigned char[YUVBufferSize[i]];

            // Compute data size and add some additional room for the compressor
            CompressedBufferSize[i] = YUVBufferSize[i] + YUVBufferSize[i] / 100 + 4096;
            // Allocate compression buffer
            CompressedBuffer[i] = new unsigned char[CompressedBufferSize[i]];

            // Initialize video frame counter
            VideoFrameCounter = 0;

            opened = true;

            break;
        }
#endif

        // If either channel fails to open, return error
        if (opened == false) {
            Release();
            return SVL_FAIL;
        }

        img->SetSize(i, width, height);
    }

#if (CISST_OS == CISST_WINDOWS)
    if (avicounter > 0) {
        // Averaging framerates
        AVIFrequency /= avicounter;
    }
#endif

    return SVL_OK;
}

int svlFilterSourceVideoFile::OnStart(unsigned int CMN_UNUSED(procCount))
{
    if (TargetFrequency < 0.1) TargetFrequency = AVIFrequency;
    RestartTargetTimer();

    return SVL_OK;
}

int svlFilterSourceVideoFile::ProcessFrame(ProcInfo* procInfo)
{
    // Try to keep TargetFrequency
    _OnSingleThread(procInfo) WaitForTargetTimer();

    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(OutputData);
    unsigned int videochannels = img->GetVideoChannels();
    unsigned char* imptr;
    unsigned int idx, datasize, timestampcount = 0;
    double timestampsum = 0;
    int ret = SVL_OK;

#if (CISST_SVL_HAS_ZLIB == ON)
    const std::string framestartmarker = "\r\nFrame\r\n";
    char strbuffer[16];
    double timestamp, timespan;
    unsigned int i, compressedpartsize, offset;
    unsigned long longsize;
    int readlen, err;
    bool eof;
#endif

    _ParallelLoop(procInfo, idx, videochannels)
    {
        imptr = img->GetUCharPointer(idx);
        datasize = img->GetDataSize(idx);
        ret = SVL_FAIL;

#if (CISST_OS == CISST_WINDOWS)
        if (VideoObj[idx]) {
            if (reinterpret_cast<CVfWAvi*>(VideoObj[idx])->CopyNextAVIFrame(imptr, datasize) == 1) {
                ret = SVL_OK;
            }
        }
#endif
#if (CISST_SVL_HAS_ZLIB == ON)
        while (VideoFile[idx]) {
            eof = false;

            while (1) {
                // Read "frame start marker"
                readlen = static_cast<int>(fread(strbuffer, framestartmarker.length(), 1, VideoFile[idx]));
                if (readlen < 1) {
                    eof = true;
                    break;
                }
                strbuffer[framestartmarker.length()] = 0;
                if (framestartmarker.compare(strbuffer) != 0) break;

                // Read "time stamp"
                readlen = static_cast<int>(fread(&timestamp, sizeof(double), 1, VideoFile[idx]));
                if (readlen < 1) {
                    eof = true;
                    break;
                }
                if (timestamp < 0.0) break;
                // Saving timestamp in order to be able to write it into the sample later
                timestampsum += timestamp;
                timestampcount ++;

                if (!IsTargetTimerRunning()) {
                    // Try to keep orignal frame intervals
                    if (VideoFrameCounter == 0) {
                        FirstTimestamp[idx] = timestamp;
                        CVITimer.Reset();
                        CVITimer.Start();
                    }
                    else {
                        timespan = (timestamp - FirstTimestamp[idx]) - CVITimer.GetElapsedTime();
                        if (timespan > 0.0) osaSleep(timespan);
                    }
                }

                offset = 0;
                for (i = 0; i < FilePartCount[idx]; i ++) {
                    // Read "compressed part size"
                    readlen = static_cast<int>(fread(&compressedpartsize, sizeof(unsigned int), 1, VideoFile[idx]));
                    if (readlen < 1) {
                        eof = true;
                        break;
                    }
                    if (compressedpartsize == 0 || compressedpartsize > CompressedBufferSize[idx]) break;

                    // Read compressed frame part
                    readlen = static_cast<int>(fread(CompressedBuffer[idx], 1, compressedpartsize, VideoFile[idx]));
                    if (readlen < static_cast<int>(compressedpartsize)) {
                        eof = true;
                        break;
                    }

                    // Decompress frame part
                    longsize = YUVBufferSize[idx] - offset;
                    err = uncompress(YUVBuffer[idx] + offset,
                                     &longsize,
                                     CompressedBuffer[idx],
                                     compressedpartsize);
                    if (err != Z_OK) break;

                    // Convert YUV422 planar to RGB format
                    svlConverter::YUV422PtoRGB24(YUVBuffer[idx] + offset,
                                                 imptr + offset * 3 / 2,
                                                 longsize >> 1);

                    offset += longsize;
                }
                if (i < FilePartCount[idx]) break;

                if (eof == false) ret = SVL_OK;

                break;
            }

            if (eof) {
                // End of file reached
                if (VideoFrameCounter > 0) {

                    // Go back to the beginning of the file, just after the header
                    if (fseek(VideoFile[idx], 27, SEEK_SET) == 0) {
                        // Play again if needed
                        if (!LoopFlag) return SVL_STOP_REQUEST;
                        VideoFrameCounter = 0;
                        continue;
                    }
                    else {
                        // Can't seek back to the beginning
                        ret = SVL_FAIL;
                    }
                }
                else {
                    // If it was the first frame, then file is invalid
                    ret = SVL_FAIL;
                }
            }
            else {
                // Other error, let it fail
            }

            VideoFrameCounter ++;

            break;
        }
#endif

        if (ret == SVL_FAIL) break;
    }

    // Set timestamp to the one stored in the video file
    if (timestampcount > 0) OutputData->SetTimestamp(timestampsum / timestampcount);

    return ret;
}

int svlFilterSourceVideoFile::Release()
{
    unsigned int i;

    for (i = 0; i < VideoObj.size(); i ++) {
#if (CISST_OS == CISST_WINDOWS)
        if (VideoObj[i]) {
            reinterpret_cast<CVfWAvi*>(VideoObj[i])->Close();
            VideoObj[i] = 0;
        }
#endif
    }
    for (i = 0; i < VideoFile.size(); i ++) {
#if (CISST_SVL_HAS_ZLIB == ON)
        if (VideoFile[i]) {
            fclose(VideoFile[i]);
            VideoFile[i] = 0;
        }
        if (YUVBuffer[i]) {
            delete [] YUVBuffer[i];
            YUVBuffer[i] = 0;
        }
        if (CompressedBuffer[i]) {
            delete [] CompressedBuffer[i];
            CompressedBuffer[i] = 0;
        }
#endif
    }

    if (CVITimer.IsRunning()) CVITimer.Stop();

#if (CISST_OS == CISST_WINDOWS)
    AVIFrequency = -1.0;
#endif

    return SVL_OK;
}

int svlFilterSourceVideoFile::DialogFilePath(unsigned int videoch)
{
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;

    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(OutputData);
    if (videoch >= img->GetVideoChannels()) return SVL_WRONG_CHANNEL;

#if (CISST_OS == CISST_WINDOWS)
    OPENFILENAME ofn;
    char path[2048], title[256];
    char filter[] = "All Video Files (*.cvi; *.avi) *.cvi;*.avi CISST Video Files (*.cvi) *.cvi AVI Files (*.avi) *.avi All Files (*.*) *.* ";

    memset(path, 0, 2048);
    sprintf(title, "Open Video File for [channel #%d]", videoch);
    filter[30] = filter[42] = filter[68] = filter[74] = filter[92] = filter[98] = filter[114] = filter[118] = 0;

    memset(&ofn, 0, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = GetForegroundWindow();
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile= path;
    ofn.nMaxFile = 2048;
    ofn.Flags = OFN_ENABLESIZING | OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST;
    ofn.lpstrTitle = title;

    // Removing the keyboard focus from the parent window
    SetForegroundWindow(GetDesktopWindow());

    if (GetOpenFileName(&ofn)) {
        FilePath[videoch] = path;
        return SVL_OK;
    }
#else
    std::cout << "Enter filename for [channel #" << videoch << "]: ";
    std::cin >> FilePath[videoch];
#endif

    return SVL_OK;
}

int svlFilterSourceVideoFile::SetFilePath(const std::string filepath, unsigned int videoch)
{
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;

    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(OutputData);
    if (videoch >= img->GetVideoChannels()) return SVL_WRONG_CHANNEL;

    FilePath[videoch] = filepath;

    return SVL_OK;
}

