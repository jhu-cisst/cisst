/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlImageFileWriter.cpp 545 2009-07-14 08:38:33Z adeguet1 $
  
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

#include <cisstStereoVision/svlFilterImageFileWriter.h>
#include <cisstStereoVision/svlConverters.h>
#include <string.h>


using namespace std;

/***************************************/
/*** svlFilterImageFileWriter class ****/
/***************************************/

CMN_IMPLEMENT_SERVICES(svlFilterImageFileWriter)

svlFilterImageFileWriter::svlFilterImageFileWriter() :
    svlFilterBase(),
    cmnGenericObject(),
    TimestampsEnabled(false)
{
    AddSupportedType(svlTypeImageRGB, svlTypeImageRGB);
    AddSupportedType(svlTypeImageRGBStereo, svlTypeImageRGBStereo);
    AddSupportedType(svlTypeImageMonoFloat, svlTypeImageMonoFloat);

    DistanceScaling = 1.0f;

    for (int i = 0; i < 2; i ++) {
        Disabled[i] = false;
        ImageFile[i] = 0;
        FilePathPrefix[i][0] = 0;
        Extension[i][0] = 0;
    }

    // Continuous saving by default
    CaptureLength = -1;
}

svlFilterImageFileWriter::~svlFilterImageFileWriter()
{
    Release();
}

int svlFilterImageFileWriter::Initialize(svlSample* inputdata)
{
    svlSampleImageMonoFloat* depth;

    Release();

    // stream type specific initialization
    switch (GetInputType()) {
        case svlTypeImageRGB:
            ImageFile[SVL_LEFT] = ImageTypeList.GetHandlerInstance(Extension[SVL_LEFT]);
            if (ImageFile[SVL_LEFT] == 0)
                return SVL_IFW_EXTENSION_NOT_SUPPORTED;
        break;

        case svlTypeImageRGBStereo:
            ImageFile[SVL_LEFT] = ImageTypeList.GetHandlerInstance(Extension[SVL_LEFT]);
            ImageFile[SVL_RIGHT] = ImageTypeList.GetHandlerInstance(Extension[SVL_RIGHT]);
            if (ImageFile[SVL_LEFT] == 0 || ImageFile[SVL_RIGHT] == 0)
                return SVL_IFW_EXTENSION_NOT_SUPPORTED;
        break;

        case svlTypeImageMonoFloat:
            ImageFile[SVL_LEFT] = ImageTypeList.GetHandlerInstance(Extension[SVL_LEFT]);
            depth = dynamic_cast<svlSampleImageMonoFloat*>(inputdata);
            if (ImageFile[SVL_LEFT] == 0)
                return SVL_IFW_EXTENSION_NOT_SUPPORTED;
            ImageBuffer.SetSize(*depth);
        break;

        case svlTypeImageRGBA:
        case svlTypeImageRGBAStereo:
        case svlTypeImage3DMap:
        case svlTypeInvalid:
        case svlTypeStreamSource:
        case svlTypeStreamSink:
        case svlTypeImageMono8:
        case svlTypeImageMono8Stereo:
        case svlTypeImageMono16:
        case svlTypeImageMono16Stereo:
        case svlTypeImageCustom:
        case svlTypeRigidXform:
        case svlTypePointCloud:
            return SVL_INVALID_INPUT_TYPE;
    }

    OutputData = inputdata;

    return SVL_OK;
}

int svlFilterImageFileWriter::ProcessFrame(ProcInfo* procInfo, svlSample* inputdata)
{
    // Passing the same image for the next filter
    OutputData = inputdata;

    if (CaptureLength == 0) return SVL_OK;

    svlSampleImageBase* img = dynamic_cast<svlSampleImageBase*>(OutputData);
    unsigned int videochannels = img->GetVideoChannels();
    unsigned int idx, buffersize;
    unsigned char* buffer;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        if (Disabled[idx]) continue;

        if (img->GetType() == svlTypeImageMonoFloat) {
            svlConverter::float32toRGB24(reinterpret_cast<float*>(img->GetUCharPointer(idx)),
                                         ImageBuffer.GetUCharPointer(idx),
                                         img->GetWidth(idx) * img->GetHeight(idx),
                                         DistanceScaling);
            buffer = ImageBuffer.GetUCharPointer(idx);
            buffersize = ImageBuffer.GetDataSize(idx);
        }
        else {
            buffer = img->GetUCharPointer(idx);
            buffersize = img->GetDataSize(idx);
        }

        if (TimestampsEnabled) {
            sprintf(FilePath[idx], "%s%.3f.%s", FilePathPrefix[idx], inputdata->GetTimestamp(), Extension[idx]);
        }
        else {
            sprintf(FilePath[idx], "%s%07d.%s", FilePathPrefix[idx], FrameCounter, Extension[idx]);
        }

        ImageProps[idx].DataType = svlTypeImageRGB;
        ImageProps[idx].DataSize = buffersize;
        ImageProps[idx].Width = img->GetWidth(idx);
        ImageProps[idx].Height = img->GetHeight(idx);
        ImageProps[idx].Padding = false;

        if (ImageFile[idx]->Create(FilePath[idx], &(ImageProps[idx]), buffer) != SVL_OK) return SVL_FAIL;
    }

    _SynchronizeThreads(procInfo);

    _OnSingleThread(procInfo)
    {
        if (CaptureLength > 0) CaptureLength --;
    }

    return SVL_OK;
}

int svlFilterImageFileWriter::Release()
{
    for (int i = 0; i < 2; i ++) {
        if (ImageFile[0] != 0) delete ImageFile[0];
        ImageFile[i] = 0;
    }

    return SVL_OK;
}

int svlFilterImageFileWriter::Disable(bool disable, int videoch)
{
    if (IsInitialized() == true)
        return SVL_FAIL;
    if (videoch != SVL_LEFT && videoch != SVL_RIGHT)
        return SVL_WRONG_CHANNEL;

    Disabled[videoch] = disable;

    return SVL_OK;
}

int svlFilterImageFileWriter::SetFilePath(const char* filepathprefix, const char* extension, int videoch)
{
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;
    if (videoch != SVL_LEFT && videoch != SVL_RIGHT)
        return SVL_WRONG_CHANNEL;
    if (filepathprefix == 0 || extension == 0)
        return SVL_FAIL;

    int len;

    // checking path prefix length
    len = static_cast<int>(strlen(filepathprefix));
    if (len < 1 || len >= SVL_IFW_FILEPATH_LENGTH)
        return SVL_IFW_INVALID_FILEPATH;

    // storing path prefix
    memset(FilePathPrefix[videoch], 0, SVL_IFW_FILEPATH_LENGTH);
    memcpy(FilePathPrefix[videoch], filepathprefix, len);

    // checking extension length
    len = static_cast<int>(strlen(extension));
    if (len < 1 || len >= SVL_IFW_EXTENSION_LENGTH)
        return SVL_IFW_INVALID_FILEPATH;

    // checking if file extension is supported
    if (ImageFile[videoch])
        delete ImageFile[videoch];
    ImageFile[videoch] = ImageTypeList.GetHandlerInstance(extension);
    if (ImageFile[videoch] == 0)
        return SVL_IFW_EXTENSION_NOT_SUPPORTED;
    delete ImageFile[videoch];
    ImageFile[videoch] = 0;

    // storing extension
    memset(Extension[videoch], 0, SVL_IFW_EXTENSION_LENGTH);
    memcpy(Extension[videoch], extension, len);

    return SVL_OK;
}

