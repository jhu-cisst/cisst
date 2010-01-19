/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlImageSampler.cpp 545 2009-07-14 08:38:33Z adeguet1 $
  
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


#include <cisstStereoVision/svlFilterImageSampler.h>
#include <cisstStereoVision/svlConverters.h>
#include <string.h>


using namespace std;

/******************************************/
/*** svlFilterImageSampler class *********/
/******************************************/

CMN_IMPLEMENT_SERVICES(svlFilterImageSampler)

svlFilterImageSampler::svlFilterImageSampler() :
    svlFilterBase(),
    cmnGenericObject()
{
    AddSupportedType(svlTypeImageMono8, svlTypeImageMono8);
    AddSupportedType(svlTypeImageMono8Stereo, svlTypeImageMono8Stereo);
    AddSupportedType(svlTypeImageMono16, svlTypeImageMono16);
    AddSupportedType(svlTypeImageMono16Stereo, svlTypeImageMono16Stereo);
    AddSupportedType(svlTypeImageRGB, svlTypeImageRGB);
    AddSupportedType(svlTypeImageRGBStereo, svlTypeImageRGBStereo);
    AddSupportedType(svlTypeImageMonoFloat, svlTypeImageMonoFloat);

    CallbackObj = 0;
    FileHeader[0] = FileHeader[1] = 0;
    DIBHeader[0] = DIBHeader[1] = 0;
    DistanceScaling = 1.0f;
    ImageBuffer = 0;
}

svlFilterImageSampler::~svlFilterImageSampler()
{
    Release();
}

void svlFilterImageSampler::SetCallback(svlImageSamplerCallbackBase* callbackobj)
{
    CallbackObj = callbackobj;
}

int svlFilterImageSampler::Initialize(svlSample* inputdata)
{
    svlSampleImageBase* image = dynamic_cast<svlSampleImageBase*>(inputdata);
    unsigned char* ucharptr;
    unsigned int dibhdrsize;

    Release();

    switch (GetInputType()) {
        case svlTypeImageMono16:
        case svlTypeImageMonoFloat:
            ImageBuffer = new svlSampleImageMono8;
            ImageBuffer->SetSize(*image);

        case svlTypeImageMono8:
            dibhdrsize = sizeof(svlDIBHeader) + 256 * sizeof(svlRGBA);
            ucharptr = new unsigned char[dibhdrsize];
            memset(ucharptr, 0, dibhdrsize * sizeof(unsigned char));

            DIBHeader[0] = reinterpret_cast<svlDIBHeader*>(ucharptr);
	        DIBHeader[0]->biSize = sizeof(svlDIBHeader);
            DIBHeader[0]->biWidth = image->GetWidth();
	        DIBHeader[0]->biHeight = -1 * static_cast<int>(image->GetHeight());
	        DIBHeader[0]->biPlanes = 1;
	        DIBHeader[0]->biBitCount = 8;
	        DIBHeader[0]->biCompression = 0;
	        DIBHeader[0]->biSizeImage = 0;
	        DIBHeader[0]->biXPelsPerMeter = 0;
	        DIBHeader[0]->biYPelsPerMeter = 0;
	        DIBHeader[0]->biClrUsed = 256;
	        DIBHeader[0]->biClrImportant = 0;

            // color palette
            ucharptr += sizeof(svlDIBHeader);
            for (unsigned int i = 0; i < 256; i ++) {
                ucharptr[0] = ucharptr[1] = ucharptr[2] = i; ucharptr[3] = 0;
                ucharptr += 4;
            }

            FileHeader[0] = new svlBMPFileHeader;
            memset(FileHeader[0], 0, sizeof(svlBMPFileHeader));
            FileHeader[0]->bfType = 0x4D42;
            FileHeader[0]->bfReserved1 = 0;
            FileHeader[0]->bfReserved2 = 0;
            FileHeader[0]->bfOffBits = sizeof(svlBMPFileHeader) + dibhdrsize;
            FileHeader[0]->bfSize = FileHeader[0]->bfOffBits + image->GetWidth() * image->GetHeight();
        break;

        case svlTypeImageMono16Stereo:
            ImageBuffer = new svlSampleImageMono8Stereo;
            ImageBuffer->SetSize(*image);

        case svlTypeImageMono8Stereo:
            dibhdrsize = sizeof(svlDIBHeader) + 256 * sizeof(svlRGBA);
            ucharptr = new unsigned char[2 * dibhdrsize];
            memset(ucharptr, 0, 2 * dibhdrsize * sizeof(unsigned char));

            DIBHeader[SVL_LEFT] = reinterpret_cast<svlDIBHeader*>(ucharptr);
	        DIBHeader[SVL_LEFT]->biSize = sizeof(svlDIBHeader);
            DIBHeader[SVL_LEFT]->biWidth = image->GetWidth(SVL_LEFT);
	        DIBHeader[SVL_LEFT]->biHeight = -1 * static_cast<int>(image->GetHeight(SVL_LEFT));
	        DIBHeader[SVL_LEFT]->biPlanes = 1;
	        DIBHeader[SVL_LEFT]->biBitCount = 8;
	        DIBHeader[SVL_LEFT]->biCompression = 0;
	        DIBHeader[SVL_LEFT]->biSizeImage = 0;
	        DIBHeader[SVL_LEFT]->biXPelsPerMeter = 0;
	        DIBHeader[SVL_LEFT]->biYPelsPerMeter = 0;
	        DIBHeader[SVL_LEFT]->biClrUsed = 256;
	        DIBHeader[SVL_LEFT]->biClrImportant = 0;

            // color palette
            ucharptr += sizeof(svlDIBHeader);
            for (unsigned int i = 0; i < 256; i ++) {
                ucharptr[0] = ucharptr[1] = ucharptr[2] = i; ucharptr[3] = 0;
                ucharptr += 4;
            }

            DIBHeader[SVL_RIGHT] = reinterpret_cast<svlDIBHeader*>(ucharptr);
	        DIBHeader[SVL_RIGHT]->biSize = sizeof(svlDIBHeader);
            DIBHeader[SVL_RIGHT]->biWidth = image->GetWidth(SVL_RIGHT);
	        DIBHeader[SVL_RIGHT]->biHeight = -1 * static_cast<int>(image->GetHeight(SVL_RIGHT));
	        DIBHeader[SVL_RIGHT]->biPlanes = 1;
	        DIBHeader[SVL_RIGHT]->biBitCount = 8;
	        DIBHeader[SVL_RIGHT]->biCompression = 0;
	        DIBHeader[SVL_RIGHT]->biSizeImage = 0;
	        DIBHeader[SVL_RIGHT]->biXPelsPerMeter = 0;
	        DIBHeader[SVL_RIGHT]->biYPelsPerMeter = 0;
	        DIBHeader[SVL_RIGHT]->biClrUsed = 256;
	        DIBHeader[SVL_RIGHT]->biClrImportant = 0;

            // color palette
            ucharptr += sizeof(svlDIBHeader);
            for (unsigned int i = 0; i < 256; i ++) {
                ucharptr[0] = ucharptr[1] = ucharptr[2] = i;
                ucharptr[3] = 0;
                ucharptr += 4;
            }

            FileHeader[0] = new svlBMPFileHeader[2];
            memset(FileHeader[0], 0, 2 * sizeof(svlBMPFileHeader));
            FileHeader[0]->bfType = 0x4D42;
            FileHeader[0]->bfReserved1 = 0;
            FileHeader[0]->bfReserved2 = 0;
            FileHeader[0]->bfOffBits = sizeof(svlBMPFileHeader) + dibhdrsize;
            FileHeader[0]->bfSize = FileHeader[0]->bfOffBits + image->GetWidth(SVL_LEFT) * image->GetHeight(SVL_LEFT);

            FileHeader[1] = FileHeader[0] + 1;
            FileHeader[1]->bfType = 0x4D42;
            FileHeader[1]->bfReserved1 = 0;
            FileHeader[1]->bfReserved2 = 0;
            FileHeader[1]->bfOffBits = sizeof(svlBMPFileHeader) + dibhdrsize;
            FileHeader[1]->bfSize = FileHeader[1]->bfOffBits + image->GetWidth(SVL_RIGHT) * image->GetHeight(SVL_RIGHT);
        break;

        case svlTypeImageRGB:
            ucharptr = new unsigned char[sizeof(svlDIBHeader)];
            memset(ucharptr, 0, sizeof(svlDIBHeader) * sizeof(unsigned char));

            DIBHeader[0] = reinterpret_cast<svlDIBHeader*>(ucharptr);
	        DIBHeader[0]->biSize = sizeof(svlDIBHeader);
            DIBHeader[0]->biWidth = image->GetWidth();
	        DIBHeader[0]->biHeight = -1 * static_cast<int>(image->GetHeight());
	        DIBHeader[0]->biPlanes = 1;
	        DIBHeader[0]->biBitCount = 24;
	        DIBHeader[0]->biCompression = 0;
	        DIBHeader[0]->biSizeImage = 0;
	        DIBHeader[0]->biXPelsPerMeter = 0;
	        DIBHeader[0]->biYPelsPerMeter = 0;
	        DIBHeader[0]->biClrUsed = 0;
	        DIBHeader[0]->biClrImportant = 0;

            FileHeader[0] = new svlBMPFileHeader[1];
            memset(FileHeader[0], 0, sizeof(svlBMPFileHeader));
            FileHeader[0]->bfType = 0x4D42;
            FileHeader[0]->bfReserved1 = 0;
            FileHeader[0]->bfReserved2 = 0;
            FileHeader[0]->bfOffBits = sizeof(svlBMPFileHeader) + sizeof(svlDIBHeader);
            FileHeader[0]->bfSize = FileHeader[0]->bfOffBits + image->GetDataSize();
        break;

        case svlTypeImageRGBStereo:
            image = dynamic_cast<svlSampleImageRGBStereo*>(inputdata);

            Release();

            ucharptr = new unsigned char[2 * sizeof(svlDIBHeader)];
            memset(ucharptr, 0, 2 * sizeof(svlDIBHeader) * sizeof(unsigned char));

            DIBHeader[0] = reinterpret_cast<svlDIBHeader*>(ucharptr);
            DIBHeader[1] = reinterpret_cast<svlDIBHeader*>(ucharptr + sizeof(svlDIBHeader));

	        DIBHeader[SVL_LEFT]->biSize = sizeof(svlDIBHeader);
            DIBHeader[SVL_LEFT]->biWidth = image->GetWidth(SVL_LEFT);
	        DIBHeader[SVL_LEFT]->biHeight = -1 * static_cast<int>(image->GetHeight(SVL_LEFT));
	        DIBHeader[SVL_LEFT]->biPlanes = 1;
	        DIBHeader[SVL_LEFT]->biBitCount = 24;
	        DIBHeader[SVL_LEFT]->biCompression = 0;
	        DIBHeader[SVL_LEFT]->biSizeImage = 0;
	        DIBHeader[SVL_LEFT]->biXPelsPerMeter = 0;
	        DIBHeader[SVL_LEFT]->biYPelsPerMeter = 0;
	        DIBHeader[SVL_LEFT]->biClrUsed = 0;
	        DIBHeader[SVL_LEFT]->biClrImportant = 0;

	        DIBHeader[SVL_RIGHT]->biSize = sizeof(svlDIBHeader);
            DIBHeader[SVL_RIGHT]->biWidth = image->GetWidth(SVL_RIGHT);
	        DIBHeader[SVL_RIGHT]->biHeight = -1 * static_cast<int>(image->GetHeight(SVL_RIGHT));
	        DIBHeader[SVL_RIGHT]->biPlanes = 1;
	        DIBHeader[SVL_RIGHT]->biBitCount = 24;
	        DIBHeader[SVL_RIGHT]->biCompression = 0;
	        DIBHeader[SVL_RIGHT]->biSizeImage = 0;
	        DIBHeader[SVL_RIGHT]->biXPelsPerMeter = 0;
	        DIBHeader[SVL_RIGHT]->biYPelsPerMeter = 0;
	        DIBHeader[SVL_RIGHT]->biClrUsed = 0;
	        DIBHeader[SVL_RIGHT]->biClrImportant = 0;

            FileHeader[0] = new svlBMPFileHeader[2];
            memset(FileHeader[0], 0, 2 * sizeof(svlBMPFileHeader));
            FileHeader[0]->bfType = 0x4D42;
            FileHeader[0]->bfReserved1 = 0;
            FileHeader[0]->bfReserved2 = 0;
            FileHeader[0]->bfOffBits = sizeof(svlBMPFileHeader) + sizeof(svlDIBHeader);
            FileHeader[0]->bfSize = FileHeader[0]->bfOffBits + image->GetDataSize(SVL_LEFT);

            FileHeader[1] = FileHeader[0] + 1;
            FileHeader[1]->bfType = 0x4D42;
            FileHeader[1]->bfReserved1 = 0;
            FileHeader[1]->bfReserved2 = 0;
            FileHeader[1]->bfOffBits = sizeof(svlBMPFileHeader) + sizeof(svlDIBHeader);
            FileHeader[1]->bfSize = FileHeader[1]->bfOffBits + image->GetDataSize(SVL_RIGHT);
        break;

        case svlTypeImageRGBA:
        case svlTypeImageRGBAStereo:
        case svlTypeImage3DMap:
        case svlTypeInvalid:
        case svlTypeStreamSource:
        case svlTypeStreamSink:
        case svlTypeImageCustom:
        case svlTypeRigidXform:
        case svlTypePointCloud:
            return SVL_INVALID_INPUT_TYPE;
    }

    OutputData = inputdata;

    return SVL_OK;
}

int svlFilterImageSampler::ProcessFrame(ProcInfo* procInfo, svlSample* inputdata)
{
    _OnSingleThread(procInfo)
    {
        svlSampleImageBase* inimage = dynamic_cast<svlSampleImageBase*>(inputdata);
        svlSampleImageBase* outimage = 0;

        switch (GetInputType()) {
            case svlTypeImageMonoFloat:
                // Convert float32 values to grayscale8
                svlConverter::float32toGray8(reinterpret_cast<float*>(inimage->GetUCharPointer()),
                                             ImageBuffer->GetUCharPointer(),
                                             inimage->GetWidth() * inimage->GetHeight(),
                                             DistanceScaling);
                outimage = ImageBuffer;
            break;

            case svlTypeImageMono16:
                // Convert grayscale16 values to grayscale8
                svlConverter::Gray16toGray8(reinterpret_cast<unsigned short*>(inimage->GetUCharPointer()),
                                            ImageBuffer->GetUCharPointer(),
                                            inimage->GetWidth() * inimage->GetHeight(),
                                            0);
                outimage = ImageBuffer;
            break;

            case svlTypeImageMono16Stereo:
                // Convert grayscale16 values to grayscale8
                svlConverter::Gray16toGray8(reinterpret_cast<unsigned short*>(inimage->GetUCharPointer(SVL_LEFT)),
                                            ImageBuffer->GetUCharPointer(SVL_LEFT),
                                            inimage->GetWidth(SVL_LEFT) * inimage->GetHeight(SVL_LEFT),
                                            0);
                svlConverter::Gray16toGray8(reinterpret_cast<unsigned short*>(inimage->GetUCharPointer(SVL_RIGHT)),
                                            ImageBuffer->GetUCharPointer(SVL_RIGHT),
                                            inimage->GetWidth(SVL_RIGHT) * inimage->GetHeight(SVL_RIGHT),
                                            0);
                outimage = ImageBuffer;
            break;

            case svlTypeImageMono8:
            case svlTypeImageMono8Stereo:
            case svlTypeImageRGB:
            case svlTypeImageRGBStereo:
                outimage = dynamic_cast<svlSampleImageBase*>(inputdata);
            break;

            case svlTypeImageRGBA:
            case svlTypeImageRGBAStereo:
            case svlTypeImage3DMap:
            case svlTypeInvalid:
            case svlTypeStreamSource:
            case svlTypeStreamSink:
            case svlTypeImageCustom:
            case svlTypeRigidXform:
            case svlTypePointCloud:
                return SVL_INVALID_INPUT_TYPE;
        }

        if (CallbackObj) {
            CallbackObj->FrameCallback(outimage, FileHeader[0], DIBHeader[0], FileHeader[1], DIBHeader[1]);
            CallbackObj->PostCallback();
            return SVL_OK;
        }
    }

    // Passing the same image for the next filter
    OutputData = inputdata;

    return SVL_OK;
}

int svlFilterImageSampler::Release()
{
    if (DIBHeader[0]) {
        delete [] reinterpret_cast<unsigned char*>(DIBHeader[0]);
        DIBHeader[0] = DIBHeader[1] = 0;
    }
    if (FileHeader[0]) {
        delete [] FileHeader[0];
        FileHeader[0] = FileHeader[1] = 0;
    }
    if (ImageBuffer) {
        delete ImageBuffer;
        ImageBuffer = 0;
    }

    return SVL_OK;
}

