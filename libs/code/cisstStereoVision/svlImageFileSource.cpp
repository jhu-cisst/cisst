/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlImageFileSource.cpp,v 1.16 2008/10/22 20:04:13 vagvoba Exp $
  
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

#include <cisstStereoVision/svlImageFileSource.h>
#include <cisstOSAbstraction/osaSleep.h>

#include <math.h>

using namespace std;

/*************************************/
/*** svlImageFileSource class ********/
/*************************************/

svlImageFileSource::svlImageFileSource(bool stereo) : svlFilterBase()
{
    Stereo = stereo;
    if (Stereo) {
        SetFilterToSource(svlTypeImageRGBStereo);
        OutputData = new svlSampleImageRGBStereo;
    }
    else {
        SetFilterToSource(svlTypeImageRGB);
        OutputData = new svlSampleImageRGB;
    }

    for (int i = 0; i < 2; i ++) {
        ImageFile[i] = 0;
        FilePathPrefix[i][0] = 0;
        Extension[i][0] = 0;
    }
    NumberOfDigits = 0;
    From = 0;
    To = 0;

    Hertz = 30.0;
}

svlImageFileSource::~svlImageFileSource()
{
    Release();

    if (OutputData) delete OutputData;
}

int svlImageFileSource::Initialize(svlSample* inputdata)
{
    Release();

    if (Stereo) {
        // creating image file objects
        ImageFile[SVL_LEFT] = ImageTypeList.GetHandlerInstance(Extension[SVL_LEFT]);
        ImageFile[SVL_RIGHT] = ImageTypeList.GetHandlerInstance(Extension[SVL_RIGHT]);
        if (ImageFile[SVL_LEFT] == 0 || ImageFile[SVL_RIGHT] == 0) {
            Release();
            return SVL_IFS_EXTENSION_NOT_SUPPORTED;
        }

        // constructing filename
        if (NumberOfDigits > 0) {
            // first file in the sequence
            FileCounter = From;
            BuildFilePath(SVL_LEFT, FileCounter);
            BuildFilePath(SVL_RIGHT, FileCounter);
        }
        else {
            // single file
            BuildFilePath(SVL_LEFT);
            BuildFilePath(SVL_RIGHT);
        }

        // opening first files (in the sequence)
        ImageProps[SVL_LEFT].Padding = false;
        ImageProps[SVL_RIGHT].Padding = false;
        if (ImageFile[SVL_LEFT]->Open(FilePath[SVL_LEFT], ImageProps[SVL_LEFT]) != SVL_OK ||
            ImageFile[SVL_RIGHT]->Open(FilePath[SVL_RIGHT], ImageProps[SVL_RIGHT]) != SVL_OK) {
            Release();
            return SVL_IFS_UNABLE_TO_OPEN;
        }

        // setting image size
        svlSampleImageRGBStereo* img = dynamic_cast<svlSampleImageRGBStereo*>(OutputData);
        img->SetSize(SVL_LEFT, ImageProps[SVL_LEFT].Width, ImageProps[SVL_LEFT].Height);
        img->SetSize(SVL_RIGHT, ImageProps[SVL_RIGHT].Width, ImageProps[SVL_RIGHT].Height);
    }
    else {
        // checking extensions
        // creating image file objects
        ImageFile[SVL_LEFT] = ImageTypeList.GetHandlerInstance(Extension[SVL_LEFT]);
        if (ImageFile[SVL_LEFT] == 0) {
            Release();
            return SVL_IFS_EXTENSION_NOT_SUPPORTED;
        }

        // constructing filename
        if (NumberOfDigits > 0) {
            // first file in the sequence
            FileCounter = From;
            BuildFilePath(SVL_LEFT, FileCounter);
        }
        else {
            // single file
            BuildFilePath(SVL_LEFT);
        }

        // opening first files (in the sequence)
        ImageProps[SVL_LEFT].Padding = false;
        if (ImageFile[SVL_LEFT]->Open(FilePath[SVL_LEFT], ImageProps[SVL_LEFT]) != SVL_OK) {
            Release();
            return SVL_IFS_UNABLE_TO_OPEN;
        }

        // setting image size
        svlSampleImageRGB* img = dynamic_cast<svlSampleImageRGB*>(OutputData);
        img->SetSize(SVL_LEFT, ImageProps[SVL_LEFT].Width, ImageProps[SVL_LEFT].Height);
    }

    Timer.Reset();
    Timer.Start();
    ulFrameTime = 1.0 / Hertz;

    return SVL_OK;
}

int svlImageFileSource::ProcessFrame(ProcInfo* procInfo, svlSample* inputdata)
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

    ////////////////////////////////////////////
    //    Check if the filename has changed   //
      if (FrameCounter > 0 &&
          (NumberOfDigits == 0 || From == To))
          return SVL_ALREADY_PROCESSED;
    ////////////////////////////////////////////

    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(OutputData);
    unsigned int videochannels = img->GetVideoChannels();
    unsigned int idx;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        // constructing filename (counter ignored if NumberOfDigits is zero)
        BuildFilePath(idx, FileCounter);

        // opening file
        ImageProps[idx].Padding = false;
        if (ImageFile[idx]->Open(FilePath[idx], ImageProps[idx]) != SVL_OK)
            return SVL_IFS_UNABLE_TO_OPEN;

        // checking image dimensions
        if (ImageProps[idx].Width != img->GetWidth(idx) ||
            ImageProps[idx].Height != img->GetHeight(idx))
            return SVL_IFS_WRONG_IMAGE_SIZE;

        // reading data and closing file
        if (ImageFile[idx]->ReadAndClose(reinterpret_cast<unsigned char*>(img->GetPointer(idx)), img->GetDataSize(idx)) != SVL_OK)
            return SVL_IFS_WRONG_IMAGE_DATA_SIZE;
    }

    _OnSingleThread(procInfo)
    {
        if (NumberOfDigits > 0) {
            FileCounter ++;
            if (FileCounter > To) FileCounter = From;
        }
    }

    return SVL_OK;
}

int svlImageFileSource::Release()
{
    for (int i = 0; i < 2; i ++) {
        if (ImageFile[0] != 0) delete ImageFile[0];
        ImageFile[i] = 0;
    }

    return SVL_OK;
}

int svlImageFileSource::GetWidth(int videoch)
{
    if (IsDataValid(GetOutputType(), OutputData) != SVL_OK)
        return SVL_FAIL;
    if (Stereo) {
        svlSampleImageRGBStereo* img = dynamic_cast<svlSampleImageRGBStereo*>(OutputData);
        if (videoch == SVL_LEFT) return img->GetWidth(SVL_LEFT);
        if (videoch == SVL_RIGHT) return img->GetWidth(SVL_RIGHT);
    }
    else {
        svlSampleImageRGB* img = dynamic_cast<svlSampleImageRGB*>(OutputData);
        return img->GetWidth();
    }
    return SVL_WRONG_CHANNEL;
}

int svlImageFileSource::GetHeight(int videoch)
{
    if (IsDataValid(GetOutputType(), OutputData) != SVL_OK)
        return SVL_FAIL;
    if (Stereo) {
        svlSampleImageRGBStereo* img = dynamic_cast<svlSampleImageRGBStereo*>(OutputData);
        if (videoch == SVL_LEFT) return img->GetHeight(SVL_LEFT);
        if (videoch == SVL_RIGHT) return img->GetHeight(SVL_RIGHT);
    }
    else {
        svlSampleImageRGB* img = dynamic_cast<svlSampleImageRGB*>(OutputData);
        return img->GetHeight();
    }
    return SVL_WRONG_CHANNEL;
}

int svlImageFileSource::SetFilePath(const char* filepathprefix, const char* extension, int videoch)
{
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;
    if (videoch != SVL_LEFT && videoch != SVL_RIGHT)
        return SVL_WRONG_CHANNEL;
    if (!Stereo && videoch == SVL_RIGHT)
        return SVL_WRONG_CHANNEL;
    if (filepathprefix == 0 || extension == 0)
        return SVL_FAIL;

    int len;

    // checking path prefix length
    len = static_cast<int>(strlen(filepathprefix));
    if (len < 1 || len >= SVL_IFS_FILEPATH_LENGTH)
        return SVL_IFS_INVALID_FILEPATH;

    // storing path prefix
    memset(FilePathPrefix[videoch], 0, SVL_IFS_FILEPATH_LENGTH);
    memcpy(FilePathPrefix[videoch], filepathprefix, len);

    // checking extension length
    len = static_cast<int>(strlen(extension));
    if (len < 1 || len >= SVL_IFS_EXTENSION_LENGTH)
        return SVL_IFS_INVALID_FILEPATH;

    // storing extension
    memset(Extension[videoch], 0, SVL_IFS_FILEPATH_LENGTH);
    memcpy(Extension[videoch], extension, len);

    return SVL_OK;
}

int svlImageFileSource::SetSequence(unsigned int numberofdigits, unsigned int from, unsigned int to)
{
    if (numberofdigits > 9 || from > to)
        return SVL_FAIL;
    unsigned int maxval = static_cast<unsigned int>(pow(10.0f, static_cast<int>(numberofdigits)));
    if (from >= maxval || to >= maxval) return SVL_FAIL;

    NumberOfDigits = numberofdigits;
    From = from;
    To = to;

    return SVL_OK;
}

void svlImageFileSource::BuildFilePath(int videoch, unsigned int framecounter)
{
    if (NumberOfDigits > 0) {
        int i, inlen, rmnlen;
        char innum[10], outnum[10];

        // building number string
        sprintf(innum, "%d", framecounter);
        inlen = strlen(innum);
        rmnlen = NumberOfDigits - inlen;
        for (i = 0; i < rmnlen; i ++) outnum[i] = '0';
        for (i = 0; i < inlen; i ++) outnum[i + rmnlen] = innum[i];
        outnum[i + rmnlen] = 0;

        sprintf(FilePath[videoch], "%s%s.%s", FilePathPrefix[videoch], outnum, Extension[videoch]);
    }
    else {
        sprintf(FilePath[videoch], "%s.%s", FilePathPrefix[videoch], Extension[videoch]);
    }
}

