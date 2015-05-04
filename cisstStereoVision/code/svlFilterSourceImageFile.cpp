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

#include <cisstStereoVision/svlFilterSourceImageFile.h>
#include <cisstStereoVision/svlFilterOutput.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

#include <math.h>

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

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterSourceImageFile, svlFilterSourceBase)

svlFilterSourceImageFile::svlFilterSourceImageFile() :
    svlFilterSourceBase(),
    OutputImage(0),
    NumberOfDigits(0),
    From(0),
    To(0),
    FrameSet(false)
{
    CreateInterfaces();

    AddOutput("output", true);
    SetAutomaticOutputType(false);
    SetTargetFrequency(30.0);
}

svlFilterSourceImageFile::svlFilterSourceImageFile(unsigned int channelcount) :
    svlFilterSourceBase(),
    OutputImage(0),
    NumberOfDigits(0),
    From(0),
    To(0),
    FrameSet(false)
{
    CreateInterfaces();

    AddOutput("output", true);
    SetAutomaticOutputType(false);
    SetChannelCount(channelcount);
    SetTargetFrequency(30.0);
}

svlFilterSourceImageFile::~svlFilterSourceImageFile()
{
    Release();

    if (OutputImage) delete OutputImage;
}

int svlFilterSourceImageFile::SetChannelCount(unsigned int channelcount)
{
    if (OutputImage ||
        channelcount < 1 || channelcount > 2) return SVL_FAIL;

    if (channelcount == 1) {
        GetOutput()->SetType(svlTypeImageRGB);
        OutputImage = new svlSampleImageRGB;
    }
    else {
        GetOutput()->SetType(svlTypeImageRGBStereo);
        OutputImage = new svlSampleImageRGBStereo;
    }

    ImageCodec.SetSize(channelcount);
    FilePathPrefix.SetSize(channelcount);
    Extension.SetSize(channelcount);
    FilePath.SetSize(channelcount);
    ImageCodec.SetAll(0);

    return SVL_OK;
}

int svlFilterSourceImageFile::Initialize(svlSample* &syncOutput)
{
    if (OutputImage == 0) return SVL_FAIL;
    syncOutput = OutputImage;

    Release();

    unsigned int videochannels = OutputImage->GetVideoChannels();
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
        OutputImage->SetSize(i, w, h);
    }

    return SVL_OK;
}

int svlFilterSourceImageFile::OnStart(unsigned int CMN_UNUSED(procCount))
{
    StopLoop = false;
    return SVL_OK;
}

int svlFilterSourceImageFile::Process(svlProcInfo* procInfo, svlSample* &syncOutput)
{
    syncOutput = OutputImage;

    // Increment file counter
    if (FrameCounter > 0) {
        _OnSingleThread(procInfo)
        {
            if(!FrameSet){
                FileCounter ++;
                if (FileCounter > To) {
                    if (GetLoop()) FileCounter = From;
                    else StopLoop = true;
                }
            }
        }

        _SynchronizeThreads(procInfo);

        if (StopLoop) return SVL_STOP_REQUEST;
    }

    // Try to keep target frequency
    _OnSingleThread(procInfo) WaitForTargetTimer();

    ////////////////////////////////////////////
    //    Check if the filename has changed   //
      if (FrameCounter > 0 &&
          (NumberOfDigits == 0 || From == To))
          return SVL_ALREADY_PROCESSED;
    ////////////////////////////////////////////

    unsigned int videochannels = OutputImage->GetVideoChannels();
    unsigned int idx;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        // constructing filename (counter ignored if NumberOfDigits is zero)
        BuildFilePath(idx, FileCounter);

        // opening file
        if (ImageCodec[idx]->Read(*OutputImage, idx, FilePath[idx], true) != SVL_OK)
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
    if (OutputImage == 0)
        return SVL_FAIL;
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;

    unsigned int videochannels = OutputImage->GetVideoChannels();
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

unsigned int svlFilterSourceImageFile::GetWidth(unsigned int videoch) const
{
    if (!IsInitialized()) return 0;
    return OutputImage->GetWidth(videoch);
}

unsigned int svlFilterSourceImageFile::GetHeight(unsigned int videoch) const
{
    if (!IsInitialized()) return 0;
    return OutputImage->GetHeight(videoch);
}

int svlFilterSourceImageFile::BuildFilePath(int videoch, unsigned int framecounter)
{
    if (OutputImage == 0)
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


int svlFilterSourceImageFile::SetFrame(unsigned int numberofdigits, unsigned int frame)
{
    if (numberofdigits > 9)
        return SVL_FAIL;
    unsigned int maxval = static_cast<unsigned int>(pow(10.0f, static_cast<int>(numberofdigits)));
    if (frame >= maxval) return SVL_FAIL;

    NumberOfDigits = numberofdigits;
    FileCounter = frame;
    FrameSet = true;

    return SVL_OK;
}

void svlFilterSourceImageFile::CreateInterfaces()
{
    // Add NON-QUEUED provided interface for configuration management
    mtsInterfaceProvided* provided = AddInterfaceProvided("Settings", MTS_COMMANDS_SHOULD_NOT_BE_QUEUED);
    if (provided) {
        provided->AddCommandWrite(&svlFilterSourceBase::SetTargetFrequency, dynamic_cast<svlFilterSourceBase*>(this), "SetFramerate");
        provided->AddCommandWrite(&svlFilterSourceBase::SetLoop,            dynamic_cast<svlFilterSourceBase*>(this), "SetLoop");
        provided->AddCommandVoid (&svlFilterSourceBase::Pause,              dynamic_cast<svlFilterSourceBase*>(this), "Pause");
        provided->AddCommandVoid (&svlFilterSourceBase::Play,               dynamic_cast<svlFilterSourceBase*>(this), "Play");
        provided->AddCommandWrite(&svlFilterSourceBase::Play,               dynamic_cast<svlFilterSourceBase*>(this), "PlayFrames");
        provided->AddCommandWrite(&svlFilterSourceImageFile::SetChannelsCommand,    this, "SetChannels");
        provided->AddCommandWrite(&svlFilterSourceImageFile::SetFileCommand,        this, "SetFile");
        provided->AddCommandRead (&svlFilterSourceImageFile::GetChannelsCommand,    this, "GetChannels");
        provided->AddCommandRead (&svlFilterSourceImageFile::GetFileCommand,        this, "GetFile");
        provided->AddCommandRead (&svlFilterSourceImageFile::GetDimensionsLCommand, this, "GetDimensions");
        provided->AddCommandRead (&svlFilterSourceImageFile::GetDimensionsLCommand, this, "GetLeftDimensions");
        provided->AddCommandRead (&svlFilterSourceImageFile::GetDimensionsRCommand, this, "GetRightDimensions");
    }
}

void svlFilterSourceImageFile::SetChannelsCommand(const int& channels)
{
    if (SetChannelCount(static_cast<unsigned int>(channels)) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "SetChannelsCommand: \"SetChannelCount(" << channels << ")\" returned error" << std::endl;
    }
}

void svlFilterSourceImageFile::SetFileCommand(const FileInfo & fileinfo)
{
    if (fileinfo.channel >= ImageCodec.size()) {
        CMN_LOG_CLASS_INIT_ERROR << "SetFileCommand: \"FileInfo::channel\" argument value ("
                                 << fileinfo.channel
                                 << ") is out of range"
                                 << std::endl;
        return;
    }
    if (SetFilePath(fileinfo.path_prefix, fileinfo.path_extension, fileinfo.channel) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "SetFileCommand: \"SetFilePath(\""
                                 << fileinfo.path_prefix
                                 << "\", \""
                                 << fileinfo.path_extension
                                 << "\", "
                                 << fileinfo.channel
                                 << ")\" returned error"
                                 << std::endl;
        return;
    }
    if (SetSequence(fileinfo.sequence_digits, fileinfo.sequence_from, fileinfo.sequence_to) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "SetFileCommand: \"SetSequence("
                                 << fileinfo.sequence_digits
                                 << ", "
                                 << fileinfo.sequence_from
                                 << ", "
                                 << fileinfo.sequence_to
                                 << ")\" returned error"
                                 << std::endl;
    }
}

void svlFilterSourceImageFile::GetChannelsCommand(int & channels) const
{
    channels = static_cast<int>(ImageCodec.size());
}

void svlFilterSourceImageFile::GetFileCommand(FileInfo & fileinfo) const
{
    if (fileinfo.channel >= ImageCodec.size()) {
        CMN_LOG_CLASS_INIT_ERROR << "GetFileCommand: \"FileInfo::channel\" argument value ("
                                 << fileinfo.channel
                                 << ") is out of range"
                                 << std::endl;
        return;
    }
    fileinfo.path_prefix     = FilePathPrefix[fileinfo.channel];
    fileinfo.path_extension  = Extension[fileinfo.channel];
    fileinfo.sequence_digits = NumberOfDigits;
    fileinfo.sequence_from   = From;
    fileinfo.sequence_to     = To;
}

void svlFilterSourceImageFile::GetDimensionsLCommand(vctInt2 & dimensions) const
{
    dimensions[0] = static_cast<int>(GetWidth(SVL_LEFT));
    dimensions[1] = static_cast<int>(GetHeight(SVL_LEFT));
    if (dimensions[0] <= 0 || dimensions[1] <= 0) {
        CMN_LOG_CLASS_INIT_ERROR << "GetDimensionsLCommand: invalid image dimensions; "
                                 << "make sure the stream is already initialized"
                                 << std::endl;
    }
}

void svlFilterSourceImageFile::GetDimensionsRCommand(vctInt2 & dimensions) const
{
    dimensions[0] = static_cast<int>(GetWidth(SVL_RIGHT));
    dimensions[1] = static_cast<int>(GetHeight(SVL_RIGHT));
    if (dimensions[0] <= 0 || dimensions[1] <= 0) {
        CMN_LOG_CLASS_INIT_ERROR << "GetDimensionsRCommand: invalid image dimensions; "
                                 << "make sure the stream is already initialized"
                                 << std::endl;
    }
}
