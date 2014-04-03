/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2010

  (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include "svlVideoCodecOpenCV.h"
#include <cisstCommon/cmnGetChar.h>
#include <iostream>

#ifndef CV_FOURCC_DEFAULT
    #define CV_FOURCC_DEFAULT -1
#endif

/*************************************/
/*** svlVideoCodecOpenCV class *******/
/*************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlVideoCodecOpenCV, svlVideoCodecBase)

svlVideoCodecOpenCV::svlVideoCodecOpenCV() :
    svlVideoCodecBase(),
    Width(0),
    Height(0),
    BegPos(-1),
    EndPos(0),
    Pos(-1),
    Writing(false),
    Opened(false),
    Framerate(30.0),
    OCVReader(0),
    OCVWriter(0)
{
    SetName("FFMPEG Codec");
    SetExtensionList(".avi;.mpg;.mpeg;.wmv;");
    SetMultithreaded(false);
    SetVariableFramerate(false);
}

svlVideoCodecOpenCV::~svlVideoCodecOpenCV()
{
    Close();
}

int svlVideoCodecOpenCV::Open(const std::string &filename, unsigned int &width, unsigned int &height, double &framerate)
{
    if (Opened) return SVL_FAIL;

    // Hack to get frame count in an alternative way, in case the OpenCV way doesn't work
    int framecountfromheader = 0;
    std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);
    if (file.is_open()) {
        file.seekg(0x30, std::ios::beg);
        unsigned char temp[4];
        file.read(reinterpret_cast<char*>(temp), 4);
        framecountfromheader =  static_cast<int>(temp[0]) +
                               (static_cast<int>(temp[1]) << 8) +
                               (static_cast<int>(temp[2]) << 16) +
                               (static_cast<int>(temp[3]) << 24);
        file.close();
    }

    OCVReader = cvCreateFileCapture(filename.c_str());
    if (OCVReader &&
        cvQueryFrame(OCVReader)) {

        FileName.assign(filename);

        width = static_cast<unsigned int>(cvGetCaptureProperty(OCVReader, CV_CAP_PROP_FRAME_WIDTH));
        height = static_cast<unsigned int>(cvGetCaptureProperty(OCVReader, CV_CAP_PROP_FRAME_HEIGHT));
        Framerate = framerate = cvGetCaptureProperty(OCVReader, CV_CAP_PROP_FPS);

        BegPos = 0;
        Pos = 1; // already captured a frame
        EndPos = static_cast<unsigned int>(cvGetCaptureProperty(OCVReader, CV_CAP_PROP_FRAME_COUNT));
        if (EndPos < 1) {
            EndPos = framecountfromheader;
            if (EndPos < 1) EndPos = -1;
        }

        // Reset position to the first frame
        SetPos(BegPos);

        Width = width;
        Height = height;
        Opened = true;
        Writing = false;
        return SVL_OK;
    }

    Close();
    return SVL_FAIL;
}

int svlVideoCodecOpenCV::Create(const std::string &filename, const unsigned int width, const unsigned int height, const double framerate)
{
	if (Opened || !Codec || width < 1 || height < 1 || framerate < 0.1) return SVL_FAIL;

    OCVWriter = cvCreateVideoWriter(filename.c_str(), GetFourccFromExtension(filename), framerate, cvSize(width, height), 1);
    if (OCVWriter) {
		BegPos = 0;
		EndPos = 0;
		Pos = -1;
        Width = width;
        Height = height;
        Opened = true;
    	Writing = true;
		return SVL_OK;
    }

    Close();
	return SVL_FAIL;
}

int svlVideoCodecOpenCV::Close()
{
    if (OCVReader) {
        cvReleaseCapture(&OCVReader);
        OCVReader = 0;
    }
    if (OCVWriter) {
        cvReleaseVideoWriter(&OCVWriter);
        OCVWriter = 0;
    }

    FileName.clear();
    Width = 0;
    Height = 0;
    BegPos = -1;
    EndPos = 0;
    Pos = -1;
    Writing = false;
    Opened = false;

    return SVL_OK;
}

int svlVideoCodecOpenCV::GetBegPos() const
{
    return BegPos;
}

int svlVideoCodecOpenCV::GetEndPos() const
{
    return EndPos;
}

int svlVideoCodecOpenCV::GetPos() const
{
    return Pos;
}

int svlVideoCodecOpenCV::SetPos(const int pos)
{
    if (pos < BegPos || pos >= EndPos) return SVL_FAIL;

    int ret = SVL_OK;

    if (Pos != pos) {
        Pos = pos;

        if (Opened && !Writing) {
            if (!cvSetCaptureProperty(OCVReader, CV_CAP_PROP_POS_FRAMES, static_cast<double>(Pos))) {
                if (Pos == BegPos) {
                    // If first frame is requested, try to reopen file
                    cvReleaseCapture(&OCVReader);
                    OCVReader = cvCreateFileCapture(FileName.c_str());
                    if (!OCVReader) {
                        Close();
                        ret = SVL_FAIL;
                    }
                }
                else ret = SVL_FAIL;
            }
        }
    }

    return ret;
}

svlVideoIO::Compression* svlVideoCodecOpenCV::GetCompression() const
{
    // The caller will need to release it by calling the
    // svlVideoIO::ReleaseCompression() method
    unsigned int size = sizeof(svlVideoIO::Compression);
    svlVideoIO::Compression* compression = reinterpret_cast<svlVideoIO::Compression*>(new unsigned char[size]);
    
    std::string name("FFMPEG Codec");
    memset(&(compression->extension[0]), 0, 16);
    memcpy(&(compression->extension[0]), ".avi", 4);
    memset(&(compression->name[0]), 0, 64);
    memcpy(&(compression->name[0]), name.c_str(), std::min(static_cast<int>(name.length()), 63));
    compression->size = size;
    compression->datasize = 0;
    compression->data[0] = 0;

    return compression;
}

int svlVideoCodecOpenCV::SetCompression(const svlVideoIO::Compression *compression)
{
    if (Opened || !compression || compression->size < sizeof(svlVideoIO::Compression)) return SVL_FAIL;

    std::string extensionlist(GetExtensions());
    std::string extension(compression->extension);
    extension += ";";
    if (extensionlist.find(extension) == std::string::npos) {
        // Codec parameters do not match this codec
        return SVL_FAIL;
    }

    svlVideoIO::ReleaseCompression(Codec);
    Codec = reinterpret_cast<svlVideoIO::Compression*>(new unsigned char[compression->size]);
    memcpy(Codec, compression, compression->size);

    return SVL_OK;
}

int svlVideoCodecOpenCV::DialogCompression()
{
    if (Opened) return SVL_FAIL;

    svlVideoIO::ReleaseCompression(Codec);

    std::string fourccstr("FFMPEG Codec");
    unsigned int size = sizeof(svlVideoIO::Compression);
    Codec = reinterpret_cast<svlVideoIO::Compression*>(new unsigned char[size]);

    Codec->size = size;
    memset(&(Codec->extension[0]), 0, 16);
    memcpy(&(Codec->extension[0]), ".avi", 4);
    memset(&(Codec->name[0]), 0, 64);
    memcpy(&(Codec->name[0]), fourccstr.c_str(), std::min(static_cast<int>(fourccstr.length()), 63));
    Codec->datasize = 0;
    Codec->data[0] = 0;

	return SVL_OK;
}

double svlVideoCodecOpenCV::GetTimestamp() const
{
    if (!Opened || Writing) return -1.0;
    return static_cast<double>(Pos) / Framerate;
}

int svlVideoCodecOpenCV::Read(svlProcInfo* procInfo, svlSampleImage &image, const unsigned int videoch, const bool noresize)
{
    if (videoch >= image.GetVideoChannels()) return SVL_FAIL;
    if (!Opened || Writing) return SVL_FAIL;

    // Uses only a single thread
    if (procInfo && procInfo->ID != 0) return SVL_OK;

    // Allocate image buffer if not done yet
    if (Width  != image.GetWidth(videoch) || Height != image.GetHeight(videoch)) {
        if (noresize) return SVL_FAIL;
        image.SetSize(videoch, Width, Height);
    }

    if (EndPos >= 0 && Pos >= EndPos) {
        SetPos(BegPos);
        return SVL_VID_END_REACHED;
    }

    if (!cvGrabFrame(OCVReader)) {
        if (Pos > 0) {
            // We must have reached the end earlier than expected
            // Correct 'EndPos' accordingly
            EndPos = Pos;
            // Signal end of stream
            SetPos(BegPos);
            return SVL_VID_END_REACHED;
        }
        // No frames found in the file
        return SVL_FAIL;
    }
    IplImage *img = cvRetrieveFrame(OCVReader);
    if (!img) return SVL_FAIL;
    cvCopy(img, image.IplImageRef(videoch));

    Pos ++;

    return SVL_OK;
}

int svlVideoCodecOpenCV::Write(svlProcInfo* procInfo, const svlSampleImage &image, const unsigned int videoch)
{
    if (videoch >= image.GetVideoChannels()) return SVL_FAIL;
    if (!Opened || !Writing) return SVL_FAIL;
	if (Width != image.GetWidth(videoch) || Height != image.GetHeight(videoch)) return SVL_FAIL;

    // Uses only a single thread
    if (procInfo && procInfo->ID != 0) return SVL_OK;

    // Return values undocumented: assuming success...
    cvWriteFrame(OCVWriter, image.IplImageRef(videoch));

	EndPos ++; Pos ++;
	return SVL_OK;
}

int svlVideoCodecOpenCV::GetFourccFromExtension(const std::string &filename)
{
    int fourcc = -1;
    std::string extension;
    svlImageIO::GetExtension(filename, extension);

    if (extension == "mpg" || extension == "mpeg") {
        fourcc = CV_FOURCC('P','I','M','1');
    }
    if (extension == "wmv") {
        fourcc = CV_FOURCC('D','I','V','3');
    }
    if (extension == "avi") {
        fourcc = CV_FOURCC('D','I','V','X');
    }

    return fourcc;
}

