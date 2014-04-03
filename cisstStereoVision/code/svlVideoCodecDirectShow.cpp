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

#include "svlVideoCodecDirectShow.h"
#include <cisstStereoVision/svlConverters.h>


/*************************************/
/*** svlVideoCodecDirectShow class ***/
/*************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlVideoCodecDirectShow, svlVideoCodecBase)

svlVideoCodecDirectShow::svlVideoCodecDirectShow() :
    svlVideoCodecBase(),
    Width(0),
    Height(0),
    BegPos(-1),
    EndPos(0),
    Pos(-1),
    KeyFrameEvery(10),
    Writing(false),
    Opened(false),
    pGraphBuilder(0),
    pMediaControl(0),
    pSampleGrabberFilter(0),
    pSampleGrabber(0),
    UpsideDown(false),
	comprBuffer(0),
	comprBufferSize(0)
{
    SetName("AVI Files");
    SetExtensionList(".avi;");
    SetMultithreaded(false);
    SetVariableFramerate(false);

    memset(&MediaType, 0, sizeof(AM_MEDIA_TYPE));
}

svlVideoCodecDirectShow::~svlVideoCodecDirectShow()
{
    Close();
    if (comprBuffer) delete [] comprBuffer;
}

int svlVideoCodecDirectShow::Open(const std::string &filename, unsigned int &width, unsigned int &height, double &framerate)
{
    if (Opened) return SVL_FAIL;

    CoCreateInstance(CLSID_FilterGraph, 0, CLSCTX_INPROC, IID_IGraphBuilder, (LPVOID*)&pGraphBuilder);

    CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC, IID_IBaseFilter, (LPVOID*)&pSampleGrabberFilter);
    pSampleGrabberFilter->QueryInterface(IID_ISampleGrabber, (LPVOID*)&pSampleGrabber);
    memset(&MediaType, 0, sizeof(AM_MEDIA_TYPE));
    MediaType.majortype = MEDIATYPE_Video;
    MediaType.subtype = MEDIASUBTYPE_RGB24;
    MediaType.formattype = FORMAT_VideoInfo;
    pSampleGrabber->SetMediaType(&MediaType);
    pGraphBuilder->AddFilter(pSampleGrabberFilter, L"Sample Grabber");

    pGraphBuilder->QueryInterface(IID_IMediaControl, (LPVOID*)&pMediaControl);
    std::wstringstream wstrstr;
    wstrstr << filename;
    std::wstring wstr(wstrstr.str());
    pMediaControl->RenderFile(wstr.c_str());

    pSampleGrabber->GetConnectedMediaType(&MediaType);
    VIDEOINFOHEADER *pVideoInfoHeader = (VIDEOINFOHEADER*)MediaType.pbFormat;
    Width = static_cast<unsigned int>(pVideoInfoHeader->bmiHeader.biWidth);
    Height = static_cast<unsigned int>(pVideoInfoHeader->bmiHeader.biHeight);
    //MediaType.lSampleSize
    pSampleGrabber->SetBufferSamples(TRUE);

    pMediaControl->Pause();


    Opened = true;

    if (AVIFileOpen(&pAviFile, filename.c_str(), OF_READ, 0) == 0 &&
        AVIFileInfo(pAviFile, &AviFileInfo, sizeof(AVIFILEINFO)) == 0 &&
        AVIFileGetStream(pAviFile, &pAviStream, streamtypeVIDEO, 0) == 0 &&
        AVIStreamInfo(pAviStream, &AviStreamInfo, sizeof(AVISTREAMINFO)) == 0) {

        pGetFrame = AVIStreamGetFrameOpen(pAviStream, 0);
        if (pGetFrame) {

            // Read the first frame to get video properties
            unsigned char* temp = reinterpret_cast<unsigned char*>(AVIStreamGetFrame(pGetFrame, 0));
            if (temp) {

                Pos = BegPos = AviStreamInfo.dwStart;
                EndPos = AviStreamInfo.dwStart + AviStreamInfo.dwLength;
                framerate = static_cast<double>(AviStreamInfo.dwRate) / AviStreamInfo.dwScale;

                BITMAPINFOHEADER* header = reinterpret_cast<BITMAPINFOHEADER*>(temp);
                Width = static_cast<unsigned int>(header->biWidth);
                Height = static_cast<unsigned int>(abs(header->biHeight));
                if ((header->biBitCount == 24 || header->biBitCount == 32) && header->biHeight > 0) UpsideDown = true;
                else UpsideDown = false;

                // Allocate upside-down buffer if not done yet
                unsigned int size = Width * 3;
                if (!comprBuffer) {
                    comprBuffer = new unsigned char[size];
                    comprBufferSize = size;
                }
                else if (comprBuffer && comprBufferSize < size) {
                    delete [] comprBuffer;
                    comprBuffer = new unsigned char[size];
                    comprBufferSize = size;
                }

                width = Width;
                height = Height;
                Writing = false;
                return SVL_OK;
            }
        }
        else {
            // Codec not supported
        }
    }

    Close();
    return SVL_FAIL;
}

int svlVideoCodecDirectShow::Create(const std::string &filename, const unsigned int width, const unsigned int height, const double framerate)
{
	if (Opened || !Codec || width < 1 || height < 1 || framerate < 0.1) return SVL_FAIL;

    DeleteFile(filename.c_str());

	BITMAPINFOHEADER header;
    memset(&header, 0, sizeof(BITMAPINFOHEADER));
	header.biSize = sizeof(BITMAPINFOHEADER);
	header.biWidth = width;
	header.biHeight = height;
	header.biPlanes = 1;
	header.biBitCount = 24;
	header.biClrUsed = 0;

	AVIFileInit();
    Opened = true;

	if (AVIFileOpen(&pAviFile, filename.c_str(), OF_WRITE|OF_CREATE, 0) == 0) {

		AviStreamInfo.fccType = streamtypeVIDEO;
		AviStreamInfo.fccHandler = 0;
		AviStreamInfo.dwScale = static_cast<DWORD>(1000000 / framerate);
		AviStreamInfo.dwRate = 1000000;
        AviStreamInfo.dwSuggestedBufferSize = 1000000;
		AviStreamInfo.rcFrame.left = 0;
		AviStreamInfo.rcFrame.top = 0;
		AviStreamInfo.rcFrame.right = width;
		AviStreamInfo.rcFrame.bottom = height;

        AVICOMPRESSOPTIONS* options = 0;
        if (Codec->datasize >= sizeof(AVICOMPRESSOPTIONS)) options = reinterpret_cast<AVICOMPRESSOPTIONS*>(&(Codec->data[0]));

		if (AVIFileCreateStream(pAviFile, &pAviStream, &AviStreamInfo) == 0 &&
            AVIMakeCompressedStream(&pAviStreamCompressed, pAviStream, options, 0) == AVIERR_OK &&
			AVIStreamSetFormat(pAviStreamCompressed, 0, &header, header.biSize) == 0) {

            // Allocate compression buffer if not done yet
            unsigned int size = width * height * 3;
            if (!comprBuffer) {
                comprBuffer = new unsigned char[size];
                comprBufferSize = size;
            }
            else if (comprBuffer && comprBufferSize < size) {
                delete [] comprBuffer;
                comprBuffer = new unsigned char[size];
                comprBufferSize = size;
            }

			BegPos = 0;
			EndPos = 0;
			Pos = -1;
            if (options && options->dwKeyFrameEvery > 0) KeyFrameEvery = static_cast<int>(options->dwKeyFrameEvery);
            else KeyFrameEvery = 10;
            Width = width;
            Height = height;
        	Writing = true;
			return SVL_OK;
		}
	}

	Close();
	return SVL_FAIL;
}

int svlVideoCodecDirectShow::Close()
{
    if (pGetFrame) {
        AVIStreamGetFrameClose(pGetFrame);
        pGetFrame = 0;
    }
    if (pAviStream) {
        AVIStreamRelease(pAviStream);
        pAviStream = 0;
    }
    if (pAviStreamCompressed) {
        AVIStreamRelease(pAviStreamCompressed);
        pAviStreamCompressed = 0;
    }
    if (pAviFile) {
        AVIFileRelease(pAviFile);
        pAviFile = 0;
    }
    if (Opened) {
        AVIFileExit();
        Opened = false;
    }

    memset(&AviFileInfo, 0, sizeof(AVIFILEINFO));
    memset(&AviStreamInfo, 0, sizeof(AVISTREAMINFO));

    Width = 0;
    Height = 0;
    BegPos = -1;
    EndPos = 0;
    Pos = -1;
    KeyFrameEvery = 10;
    Writing = false;
    UpsideDown = false;

    return SVL_OK;
}

int svlVideoCodecDirectShow::GetBegPos() const
{
    return BegPos;
}

int svlVideoCodecDirectShow::GetEndPos() const
{
    return EndPos;
}

int svlVideoCodecDirectShow::GetPos() const
{
    return Pos;
}

int svlVideoCodecDirectShow::SetPos(const int pos)
{
    if (pos < BegPos || pos >= EndPos) return SVL_FAIL;
    Pos = pos;
    return SVL_OK;
}

svlVideoIO::Compression* svlVideoCodecDirectShow::GetCompression() const
{
    if (!Codec) return 0;
    // Make a copy and return the pointer to it
    // The caller will need to release it by calling the
    // svlVideoIO::ReleaseCompression() method
    svlVideoIO::Compression* compression = reinterpret_cast<svlVideoIO::Compression*>(new unsigned char[Codec->size]);
    memcpy(compression, Codec, Codec->size);
    return compression;
}

int svlVideoCodecDirectShow::SetCompression(const svlVideoIO::Compression *compression)
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

    if (Codec->datasize >= sizeof(AVICOMPRESSOPTIONS)) {
        // Reset compressor states, since they are no longer valid
        AVICOMPRESSOPTIONS* options = reinterpret_cast<AVICOMPRESSOPTIONS*>(Codec->data);
        options->lpFormat = 0;
        options->cbFormat = 0;
        options->lpParms = 0;
        options->cbParms = 0;
    }

    return SVL_OK;
}

int svlVideoCodecDirectShow::DialogCompression()
{
    if (Opened) return SVL_FAIL;

    int ret = SVL_FAIL;

    AVIFileInit();
    Opened = true;

    if (AVIFileOpen(&pAviFile, "_temp_.avi", OF_WRITE|OF_CREATE, 0) == 0) {

        AviStreamInfo.fccType = streamtypeVIDEO;
        AviStreamInfo.fccHandler = 0;
        AviStreamInfo.dwScale = static_cast<DWORD>(1000000 / 30.0);
        AviStreamInfo.dwRate = 1000000;
        AviStreamInfo.dwSuggestedBufferSize = 1000000;
        AviStreamInfo.rcFrame.left = 0;
        AviStreamInfo.rcFrame.top = 0;
        AviStreamInfo.rcFrame.right = 320;
        AviStreamInfo.rcFrame.bottom = 240;

        if (AVIFileCreateStream(pAviFile, &pAviStream, &AviStreamInfo) == 0) {

            AVICOMPRESSOPTIONS *options = new AVICOMPRESSOPTIONS;
            if (Codec && Codec->datasize >= sizeof(AVICOMPRESSOPTIONS)) {
                memcpy(options, Codec->data, sizeof(AVICOMPRESSOPTIONS));
            }
            else {
                memset(options, 0, sizeof(AVICOMPRESSOPTIONS));
            }

		    if (AVISaveOptions(0, ICMF_CHOOSE_DATARATE|ICMF_CHOOSE_KEYFRAME, 1, &pAviStream, &options) == TRUE) {

                ICINFO icinfo;
                int fccType = ICTYPE_VIDEO;

                HIC hIC = ICOpen(fccType, options->fccHandler, ICMODE_QUERY);
                if (hIC) {
                    ICGetInfo(hIC, &icinfo, sizeof(icinfo));
                    std::wstring wname = icinfo.szDescription;
                    std::string name(wname.begin(), wname.end());

                    svlVideoIO::ReleaseCompression(Codec);

                    unsigned int size = sizeof(svlVideoIO::Compression);
                    if (options) size += sizeof(AVICOMPRESSOPTIONS) + 8; // +8 just in case of weird alignments
                    Codec = reinterpret_cast<svlVideoIO::Compression*>(new unsigned char[size]);

                    Codec->size = size;
                    memset(&(Codec->extension[0]), 0, 16);
                    memcpy(&(Codec->extension[0]), ".avi", 4);
                    memset(&(Codec->name[0]), 0, 64);
                    memcpy(&(Codec->name[0]), name.c_str(), std::min(static_cast<int>(name.length()), 63));
                    Codec->supports_timestamps = false;
                    Codec->datasize = sizeof(AVICOMPRESSOPTIONS);
                    memcpy(&(Codec->data[0]), options, Codec->datasize);

	                ret = SVL_OK;
                }
            }

            delete options;
        }
    }

	Close();
    DeleteFile("_temp_.avi");

	return ret;
}

int svlVideoCodecDirectShow::Read(svlProcInfo* procInfo, svlSampleImageBase &image, const unsigned int videoch, const bool noresize)
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

    // start playing
    pMediaControl-Run();

    long nBufferSize = am_media_type.lSampleSize;
    long *pBuffer = (long *)malloc(nBufferSize);
    pSampleGrabber->GetCurrentBuffer(&nBufferSize, pBuffer);






    if (Pos >= EndPos) {
        Pos = BegPos;
        return SVL_VID_END_REACHED;
    }





    unsigned char* source = reinterpret_cast<unsigned char*>(AVIStreamGetFrame(pGetFrame, Pos));
    if (source == 0) return SVL_FAIL;

    BITMAPINFOHEADER* header = reinterpret_cast<BITMAPINFOHEADER*>(source);
    source += header->biSize + header->biClrUsed * 4;
    unsigned char* dest = image.GetUCharPointer(videoch);
    const unsigned int size = Width * Height;

    switch (header->biBitCount) {
        case 32:
            svlConverter::RGBA32toRGB24(source, dest, size);
        break;

        case 24:
            memcpy(dest, source, size * 3);
        break;

        case 16:
            if (header->biCompression == STR2FOURCC("YUY2")) {
                // YUV422
                svlConverter::YUV422toRGB24(source, dest, size);
            }
            else {
                // RGB16
                svlConverter::RGB16toRGB24(source, dest, size);
            }
        break;

        case 8:
            svlConverter::Gray8toRGB24(source, dest, size);
        break;

        default:
         return SVL_FAIL;
    }

    if (UpsideDown) Flip(dest);

    Pos ++;

    return SVL_OK;
}

int svlVideoCodecDirectShow::Write(svlProcInfo* procInfo, const svlSampleImageBase &image, const unsigned int videoch)
{
    if (videoch >= image.GetVideoChannels()) return SVL_FAIL;
    if (!Opened || !Writing) return SVL_FAIL;
	if (Width != image.GetWidth(videoch) || Height != image.GetHeight(videoch)) return SVL_FAIL;

    // Uses only a single thread
    if (procInfo && procInfo->ID != 0) return SVL_OK;

    FlipCopy(image.GetUCharPointer(videoch), comprBuffer);

    DWORD flag = 0;
    if ((Pos % static_cast<DWORD>(KeyFrameEvery)) == 0) flag = AVIIF_KEYFRAME;

    HRESULT ret = AVIStreamWrite(pAviStreamCompressed, EndPos, 1, comprBuffer, comprBufferSize, flag, 0, 0);
	if (ret == 0) {
		EndPos ++; Pos ++;
		return SVL_OK;
	}

    LogError(ret);
    return SVL_FAIL;
}

void svlVideoCodecDirectShow::Flip(unsigned char* image)
{
    const unsigned int stride = Width * 3;
    const unsigned int rows = Height >> 1;
    unsigned char *down = image;
    unsigned char *up = image + (Height - 1) * stride;

    for (unsigned int i = 0; i < rows; i ++) {
        memcpy(comprBuffer, down, stride);
        memcpy(down, up, stride);
        memcpy(up, comprBuffer, stride);

        down += stride; up -= stride;
    }
}

void svlVideoCodecDirectShow::FlipCopy(const unsigned char* source, unsigned char* dest)
{
    const unsigned int stride = Width * 3;
    unsigned char *down = const_cast<unsigned char*>(source);
    unsigned char *up = dest + (Height - 1) * stride;

    for (unsigned int i = 0; i < Height; i ++) {
        memcpy(up, down, stride);
        down += stride; up -= stride;
    }
}

