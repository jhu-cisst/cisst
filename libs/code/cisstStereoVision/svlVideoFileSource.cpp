/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlVideoFileSource.cpp,v 1.14 2008/11/07 01:04:16 vagvoba Exp $
  
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

#include <cisstStereoVision/svlVideoFileSource.h>
#include <cisstOSAbstraction/osaSleep.h>

#if (CISST_OS == CISST_WINDOWS)
    #include "VfWAvi.h"

    static int VFS_OleInitCounter = 0;
#endif


/*************************************/
/*** svlVideoFileSource class ********/
/*************************************/

svlVideoFileSource::svlVideoFileSource(bool stereo) :
    svlFilterBase(),
    Hertz(-1.0)
{
#if (CISST_OS == CISST_WINDOWS)
    if (VFS_OleInitCounter < 1) {
        CoInitialize(0);
        VFS_OleInitCounter = 1;
    }
#endif

    if (stereo) {
        SetFilterToSource(svlTypeImageRGBStereo);
        OutputData = new svlSampleImageRGBStereo;
    }
    else {
        SetFilterToSource(svlTypeImageRGB);
        OutputData = new svlSampleImageRGB;
    }

    unsigned int videochannels = dynamic_cast<svlSampleImageBase*>(OutputData)->GetVideoChannels();
    VideoObj.SetSize(videochannels);
    FilePath.SetSize(videochannels);
    for (unsigned int i = 0; i < videochannels; i ++) {
#if (CISST_OS == CISST_WINDOWS)
        VideoObj[i] = new CVfWAvi;
#else
        VideoObj[i] = 0;
#endif
    }
}

svlVideoFileSource::~svlVideoFileSource()
{
    Release();

    if (OutputData) delete OutputData;
    for (unsigned int i = 0; i < VideoObj.size(); i ++) {
#if (CISST_OS == CISST_WINDOWS)
        if (VideoObj[i]) delete reinterpret_cast<CVfWAvi*>(VideoObj[i]);
#endif
    }

#if (CISST_OS == CISST_WINDOWS)
    if (VFS_OleInitCounter > 0) {
        CoUninitialize();
        VFS_OleInitCounter = 0;
    }
#endif
}

int svlVideoFileSource::Initialize(svlSample* inputdata)
{
    Release();

    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(OutputData);

#if (CISST_OS == CISST_WINDOWS)
    CVfWAvi* tavi;
#else
	FILE *fp;
    int readlen;
    double dbvalue;
    unsigned int width, height;
#endif

    Hertz = 0.0;

    for (unsigned int i = 0; i < img->GetVideoChannels(); i ++) {
#if (CISST_OS == CISST_WINDOWS)
        tavi = reinterpret_cast<CVfWAvi*>(VideoObj[i]);
        tavi->Close();
        if (tavi->InitPlaying(FilePath[i].c_str()) == 0) goto labError;

        Hertz += std::max(0.1, tavi->GetFramerate());
        img->SetSize(i, tavi->GetWidth(), tavi->GetHeight());
#else
        VideoObj[i] = fp = fopen(FilePath[i].c_str(), "rb");
        if (fp == 0) goto labError;

	    // Read "width"
        readlen = static_cast<int>(fread(&width, sizeof(unsigned int), 1, fp));
	    if (readlen < 1) goto labError;
        if (width < 1 || width > 4096) goto labError;

        // Read "height"
        readlen = static_cast<int>(fread(&height, sizeof(unsigned int), 1, fp));
	    if (readlen < 1) goto labError;
        if (height < 1 || height > 4096) goto labError;

        // Read "framerate"
        readlen = static_cast<int>(fread(&dbvalue, sizeof(double), 1, fp));
	    if (readlen < 1) goto labError;
        if (dbvalue <= 0.0 || dbvalue > 1000.0) goto labError;

        Hertz += dbvalue;
        img->SetSize(i, width, height);
#endif
    }

    // Averaging framerates
    Hertz /= img->GetVideoChannels();

    Timer.Reset();
    Timer.Start();
    ulFrameTime = 1.0 / Hertz;

    return SVL_OK;

labError:
    Release();
    return SVL_FAIL;
}

int svlVideoFileSource::ProcessFrame(ProcInfo* procInfo, svlSample* inputdata)
{
    _OnSingleThread(procInfo)
    {
        if (FrameCounter > 0) {
            double time = Timer.GetElapsedTime();
            double t1 = ulFrameTime * FrameCounter;
            double t2 = time - ulStartTime;
            if (t1 > t2) osaSleep(t1 - t2);
        }
        else {
            ulStartTime = Timer.GetElapsedTime();
        }
    }

    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(OutputData);
    unsigned int videochannels = img->GetVideoChannels();
    unsigned int idx;
    int ret = SVL_OK;

    _ParallelLoop(procInfo, idx, videochannels)
    {
#if (CISST_OS == CISST_WINDOWS)
        if (reinterpret_cast<CVfWAvi*>(VideoObj[idx])->CopyNextAVIFrame(reinterpret_cast<unsigned char*>(img->GetPointer(idx)),
                                                                        img->GetDataSize(idx)) == 0) ret = SVL_FAIL;
#else
        // Read raw frame
	    if (fread(img->GetPointer(idx),
                  1,
                  img->GetDataSize(idx),
                  reinterpret_cast<FILE*>(VideoObj[idx])) < img->GetDataSize(idx)) {

            // End of file reached
            if (FrameCounter > 0) {

                // Go back to the beginning of the file, just after the header
                if (fseek(reinterpret_cast<FILE*>(VideoObj[idx]),
                          2 * sizeof(unsigned int) + sizeof(double),
                          SEEK_SET) == 0) {

                    // Try to read again
	                if (fread(img->GetPointer(idx),
                              1,
                              img->GetDataSize(idx),
                              reinterpret_cast<FILE*>(VideoObj[idx])) < img->GetDataSize(idx)) ret = SVL_FAIL;
                }
                else ret = SVL_FAIL;
            }
            else {

                // If it was the first frame, then file is invalid
                ret = SVL_FAIL;
            }
        }
#endif
    }

    return ret;
}

int svlVideoFileSource::Release()
{
    for (unsigned int i = 0; i < VideoObj.size(); i ++) {
#if (CISST_OS == CISST_WINDOWS)
        if (VideoObj[i]) reinterpret_cast<CVfWAvi*>(VideoObj[i])->Close();
#else
        if (VideoObj[i]) fclose(reinterpret_cast<FILE*>(VideoObj[i]));
        VideoObj[i] = 0;
#endif
    }

    Timer.Stop();
    Hertz = -1.0;

    return SVL_OK;
}

int svlVideoFileSource::GetWidth(unsigned int videoch)
{
    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(OutputData);
    if (videoch >= img->GetVideoChannels()) return SVL_WRONG_CHANNEL;
    return static_cast<int>(img->GetWidth(videoch));
}

int svlVideoFileSource::GetHeight(unsigned int videoch)
{
    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(OutputData);
    if (videoch >= img->GetVideoChannels()) return SVL_WRONG_CHANNEL;
    return static_cast<int>(img->GetHeight(videoch));
}

double svlVideoFileSource::GetFramerate()
{
    return Hertz;
}

int svlVideoFileSource::DialogFilePath(unsigned int videoch)
{
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;

    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(OutputData);
    if (videoch >= img->GetVideoChannels()) return SVL_WRONG_CHANNEL;

    std::cout << "Enter filename for [channel #" << videoch << "]: ";
    std::cin >> FilePath[videoch];

    return SVL_OK;
}

int svlVideoFileSource::SetFilePath(const std::string filepath, unsigned int videoch)
{
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;

    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(OutputData);
    if (videoch >= img->GetVideoChannels()) return SVL_WRONG_CHANNEL;

    FilePath[videoch] = filepath;

    return SVL_OK;
}

