/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlImageFileSource.cpp 596 2009-07-27 17:01:59Z bvagvol1 $
  
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

#include <cisstStereoVision/svlFilterSourceImageFile.h>

#include <math.h>
#include <string.h>

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

using namespace std;

/***************************************/
/*** svlFilterSourceImageFile class ****/
/***************************************/

svlFilterSourceImageFile::svlFilterSourceImageFile(bool stereo) :
    svlFilterSourceBase(),
    NumberOfDigits(0),
    From(0),
    To(0)
{
    Stereo = stereo;
    if (Stereo) {
        AddSupportedType(svlTypeImageRGBStereo);
        OutputData = new svlSampleImageRGBStereo;
    }
    else {
        AddSupportedType(svlTypeImageRGB);
        OutputData = new svlSampleImageRGB;
    }

    for (int i = 0; i < 2; i ++) {
        ImageFile[i] = 0;
        FilePathPrefix[i][0] = 0;
        Extension[i][0] = 0;
    }
}

svlFilterSourceImageFile::~svlFilterSourceImageFile()
{
    Release();

    if (OutputData) delete OutputData;
}

int svlFilterSourceImageFile::Initialize()
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

    return SVL_OK;
}

int svlFilterSourceImageFile::OnStart(unsigned int procCount)
{
    StopLoop = false;
    return SVL_OK;
}

int svlFilterSourceImageFile::ProcessFrame(ProcInfo* procInfo)
{
    // Increment file counter
    if (FrameCounter > 0) {
        _OnSingleThread(procInfo)
        {
            FileCounter ++;
            if (FileCounter > To) {
                if (LoopFlag) FileCounter = From;
                else StopLoop = true;
            }
        }

        _SynchronizeThreads(procInfo);

        if (StopLoop) return SVL_STOP_REQUEST;
    }

    // Try to keep TargetFrequency
    _OnSingleThread(procInfo) WaitForTargetTimer();

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
        if (ImageFile[idx]->ReadAndClose(img->GetUCharPointer(idx), img->GetDataSize(idx)) != SVL_OK)
            return SVL_IFS_WRONG_IMAGE_DATA_SIZE;
    }

    return SVL_OK;
}

int svlFilterSourceImageFile::Release()
{
    for (int i = 0; i < 2; i ++) {
        if (ImageFile[0] != 0) delete ImageFile[0];
        ImageFile[i] = 0;
    }

    return SVL_OK;
}

int svlFilterSourceImageFile::SetFilePath(const char* filepathprefix, const char* extension, int videoch)
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

int svlFilterSourceImageFile::SetSequence(unsigned int numberofdigits, unsigned int from, unsigned int to)
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

void svlFilterSourceImageFile::BuildFilePath(int videoch, unsigned int framecounter)
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

