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


/***************************************/
/*** svlFilterSourceImageFile class ****/
/***************************************/

CMN_IMPLEMENT_SERVICES(svlFilterSourceImageFile)

svlFilterSourceImageFile::svlFilterSourceImageFile() :
    svlFilterSourceBase(),
    cmnGenericObject(),
    NumberOfDigits(0),
    From(0),
    To(0)
{
    OutputData = 0;
}

svlFilterSourceImageFile::svlFilterSourceImageFile(unsigned int channelcount) :
    svlFilterSourceBase(),
    cmnGenericObject(),
    NumberOfDigits(0),
    From(0),
    To(0)
{
    OutputData = 0;

    SetChannelCount(channelcount);
}

svlFilterSourceImageFile::~svlFilterSourceImageFile()
{
    Release();

    if (OutputData) delete OutputData;
}

int svlFilterSourceImageFile::SetChannelCount(unsigned int channelcount)
{
    if (OutputData ||
        channelcount < 1 || channelcount > 2) return SVL_FAIL;

    if (channelcount == 1) {
        AddSupportedType(svlTypeImageRGB);
        OutputData = new svlSampleImageRGB;
    }
    else {
        AddSupportedType(svlTypeImageRGBStereo);
        OutputData = new svlSampleImageRGBStereo;
    }

    ImageCodec.SetSize(channelcount);
    FilePathPrefix.SetSize(channelcount);
    Extension.SetSize(channelcount);
    FilePath.SetSize(channelcount);
    ImageCodec.SetAll(0);

    return SVL_OK;
}

int svlFilterSourceImageFile::Initialize()
{
    if (OutputData == 0) return SVL_FAIL;

    Release();

    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(OutputData);
    unsigned int videochannels = img->GetVideoChannels();
    unsigned int w, h;

    for (unsigned int i = 0; i < videochannels; i ++) {
        // checking extensions
        // creating image file objects
        ImageCodec[i] = svlImageIO::GetCodec("." + Extension[i]);
        if (ImageCodec[i] == 0) {
            Release();
            return SVL_FAIL;
        }

        // constructing filename
        if (NumberOfDigits > 0) {
            // first file in the sequence
            FileCounter = From;
            BuildFilePath(i, FileCounter);
        }
        else {
            // single file
            BuildFilePath(i);
        }

        // opening first files (in the sequence)
        if (ImageCodec[i]->ReadDimensions(FilePath[i], w, h) != SVL_OK) {
            Release();
            return SVL_FAIL;
        }

        // setting image size
        svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(OutputData);
        img->SetSize(i, w, h);
    }

    return SVL_OK;
}

int svlFilterSourceImageFile::OnStart(unsigned int CMN_UNUSED(procCount))
{
    StopLoop = false;
    return SVL_OK;
}

int svlFilterSourceImageFile::ProcessFrame(svlProcInfo* procInfo)
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
        if (ImageCodec[idx]->Read(*img, idx, FilePath[idx], true) != SVL_OK)
            return SVL_FAIL;
    }

    return SVL_OK;
}

int svlFilterSourceImageFile::Release()
{
    for (unsigned int i = 0; i < ImageCodec.size(); i ++) {
        svlImageIO::ReleaseCodec(ImageCodec[i]);
        ImageCodec[i] = 0;
    }

    return SVL_OK;
}

int svlFilterSourceImageFile::SetFilePath(const std::string & filepathprefix, const std::string & extension, int videoch)
{
    if (OutputData == 0)
        return SVL_FAIL;
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;

    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(OutputData);
    unsigned int videochannels = img->GetVideoChannels();
    if (videoch < 0 || videoch >= static_cast<int>(videochannels)) return SVL_FAIL;

    FilePathPrefix[videoch] = filepathprefix;
    Extension[videoch] = extension;

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

int svlFilterSourceImageFile::BuildFilePath(int videoch, unsigned int framecounter)
{
    if (OutputData == 0)
        return SVL_FAIL;

    std::stringstream path;

    path << FilePathPrefix[videoch];

    if (NumberOfDigits > 0) {
        path.fill('0');
        path << std::setw(NumberOfDigits) << framecounter << std::setw(1);
    }

    path << "." << Extension[videoch];
    FilePath[videoch] = path.str();

    return SVL_OK;
}

