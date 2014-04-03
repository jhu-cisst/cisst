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

#include <cisstStereoVision/svlTypes.h>
#include <cisstStereoVision/svlConverters.h>


/****************************/
/*** svlSampleImage class ***/
/****************************/

svlSampleImage::svlSampleImage() :
    svlSample()
{
    SetEncoder("bmp", 0);
}

svlSampleImage::svlSampleImage(const svlSampleImage & other) :
    svlSample(other)
{
    SetEncoder("bmp", 0);
}

svlSampleImage::~svlSampleImage()
{
}

svlPixelType svlSampleImage::GetPixelType() const
{
    switch (GetType()) {
        case svlTypeImageRGB:
        case svlTypeImageRGBStereo:
        case svlTypeCUDAImageRGB:
        case svlTypeCUDAImageRGBStereo:
            return svlPixelRGB;
        break;

        case svlTypeImageRGBA:
        case svlTypeImageRGBAStereo:
        case svlTypeCUDAImageRGBA:
        case svlTypeCUDAImageRGBAStereo:
            return svlPixelRGBA;
        break;

        case svlTypeImageMono8:
        case svlTypeImageMono8Stereo:
        case svlTypeCUDAImageMono8:
        case svlTypeCUDAImageMono8Stereo:
            return svlPixelMono8;
        break;

        case svlTypeImageMono16:
        case svlTypeImageMono16Stereo:
        case svlTypeCUDAImageMono16:
        case svlTypeCUDAImageMono16Stereo:
            return svlPixelMono16;
        break;

        case svlTypeImageMono32:
        case svlTypeImageMono32Stereo:
        case svlTypeCUDAImageMono32:
        case svlTypeCUDAImageMono32Stereo:
            return svlPixelMono32;
        break;

        case svlTypeImage3DMap:
        case svlTypeCUDAImage3DMap:
            return svlPixel3DFloat;
        break;

        case svlTypeMatrixInt8:
        case svlTypeMatrixInt16:
        case svlTypeMatrixInt32:
        case svlTypeMatrixInt64:
        case svlTypeMatrixUInt8:
        case svlTypeMatrixUInt16:
        case svlTypeMatrixUInt32:
        case svlTypeMatrixUInt64:
        case svlTypeMatrixFloat:
        case svlTypeMatrixDouble:
        case svlTypeInvalid:
        case svlTypeStreamSource:
        case svlTypeStreamSink:
        case svlTypeTransform3D:
        case svlTypeTargets:
        case svlTypeText:
        case svlTypeCameraGeometry:
        case svlTypeBlobs:
        break;
    }
    return svlPixelUnknown;
}

int svlSampleImage::GetAlphaChannel() const
{
    switch (GetType()) {
        case svlTypeImageRGBA:
        case svlTypeImageRGBAStereo:
        case svlTypeCUDAImageRGBA:
        case svlTypeCUDAImageRGBAStereo:
            return 3;
        break;

        case svlTypeImageMono8:
        case svlTypeImageMono8Stereo:
        case svlTypeImageMono16:
        case svlTypeImageMono16Stereo:
        case svlTypeImageMono32:
        case svlTypeImageMono32Stereo:
        case svlTypeImage3DMap:
        case svlTypeImageRGB:
        case svlTypeImageRGBStereo:
        case svlTypeCUDAImageMono8:
        case svlTypeCUDAImageMono8Stereo:
        case svlTypeCUDAImageMono16:
        case svlTypeCUDAImageMono16Stereo:
        case svlTypeCUDAImageMono32:
        case svlTypeCUDAImageMono32Stereo:
        case svlTypeCUDAImage3DMap:
        case svlTypeCUDAImageRGB:
        case svlTypeCUDAImageRGBStereo:
        case svlTypeMatrixInt8:
        case svlTypeMatrixInt16:
        case svlTypeMatrixInt32:
        case svlTypeMatrixInt64:
        case svlTypeMatrixUInt8:
        case svlTypeMatrixUInt16:
        case svlTypeMatrixUInt32:
        case svlTypeMatrixUInt64:
        case svlTypeMatrixFloat:
        case svlTypeMatrixDouble:
        case svlTypeInvalid:
        case svlTypeStreamSource:
        case svlTypeStreamSink:
        case svlTypeTransform3D:
        case svlTypeTargets:
        case svlTypeText:
        case svlTypeCameraGeometry:
        case svlTypeBlobs:
        break;
    }
    return SVL_FAIL;
}

int svlSampleImage::ImportData(unsigned char *input, const unsigned int size, const int CMN_UNUSED(param), const unsigned int videoch)
{
    if (!input || size == 0 || videoch >= GetVideoChannels()) return SVL_FAIL;

    unsigned int i = GetWidth(videoch) * GetHeight(videoch);
    if (size < i) i = size;

    const unsigned int datachannels = GetDataChannels();
    const unsigned int bpp = GetBPP();

    if (datachannels == 3) {
    // RGB
        svlConverter::Gray8toRGB24(input, GetUCharPointer(videoch), size);
        return SVL_OK;
    }
    else if (datachannels == 4) {
    // RGBA
        svlConverter::Gray8toRGBA32(input, GetUCharPointer(videoch), size);
        return SVL_OK;
    }
    else {
        if (bpp == 1) {
        // Mono8
            memcpy(GetUCharPointer(videoch), input, size);
            return SVL_OK;
        }
        else if (bpp == 2) {
        // Mono16
            svlConverter::Gray8toGray16(input, reinterpret_cast<unsigned short*>(GetUCharPointer(videoch)), size);
            return SVL_OK;
        }
    }

    return SVL_FAIL;
}

int svlSampleImage::ImportData(unsigned short* input, const unsigned int size, const int param, const unsigned int videoch)
{
    if (!input || size == 0 || videoch >= GetVideoChannels()) return SVL_FAIL;

    unsigned int i = GetWidth(videoch) * GetHeight(videoch);
    if (size < i) i = size;

    const unsigned int datachannels = GetDataChannels();
    const unsigned int bpp = GetBPP();

    if (datachannels == 3) {
        // RGB
        svlConverter::Gray16toRGB24(input, GetUCharPointer(videoch), size, param);
        return SVL_OK;
    }
    else if (datachannels == 4) {
        // RGBA
        svlConverter::Gray16toRGBA32(input, GetUCharPointer(videoch), size, param);
        return SVL_OK;
    }
    else {
        if (bpp == 1) {
            // Mono8
            svlConverter::Gray16toGray8(input, GetUCharPointer(videoch), size, param);
            return SVL_OK;
        }
        else if (bpp == 2) {
            // Mono16
            memcpy(GetUCharPointer(videoch), input, size * bpp);
            return SVL_OK;
        }
    }

    return SVL_FAIL;
}

int svlSampleImage::ImportData(unsigned int* input, const unsigned int size, const int param, const unsigned int videoch)
{
    if (!input || size == 0 || videoch >= GetVideoChannels()) return SVL_FAIL;

    unsigned int i = GetWidth(videoch) * GetHeight(videoch);
    if (size < i) i = size;

    const unsigned int datachannels = GetDataChannels();
    const unsigned int bpp = GetBPP();

    if (datachannels == 3) {
        // RGB
        svlConverter::Gray32toRGB24(input, GetUCharPointer(videoch), size, param);
        return SVL_OK;
    }
    else if (datachannels == 4) {
        // RGBA
        svlConverter::Gray32toRGBA32(input, GetUCharPointer(videoch), size, param);
        return SVL_OK;
    }
    else {
        if (bpp == 1) {
            // Mono8
            svlConverter::Gray32toGray8(input, GetUCharPointer(videoch), size, param);
            return SVL_OK;
        }
        else if (bpp == 2) {
            // Mono16
            svlConverter::Gray32toGray16(input, reinterpret_cast<unsigned short*>(GetUCharPointer(videoch)), size, param);
            return SVL_OK;
        }
    }

    return SVL_FAIL;
}

int svlSampleImage::ImportData(float* input, const unsigned int size, const int param, const unsigned int videoch)
{
    if (!input || size == 0 || videoch >= GetVideoChannels()) return SVL_FAIL;

    unsigned int i = GetWidth(videoch) * GetHeight(videoch);
    if (size < i) i = size;

    const unsigned int datachannels = GetDataChannels();
    const unsigned int bpp = GetBPP();
    const float fparam = static_cast<float>(param) / 1000.0f;

    if (datachannels == 3) {
        // RGB
        svlConverter::float32toRGB24(input, GetUCharPointer(videoch), size, fparam);
        return SVL_OK;
    }
    else if (datachannels == 4) {
        // RGBA
        svlConverter::float32toRGBA32(input, GetUCharPointer(videoch), size, fparam);
        return SVL_OK;
    }
    else {
        if (bpp == 1) {
            // Mono8
            svlConverter::float32toGray8(input, GetUCharPointer(videoch), size, fparam);
            return SVL_OK;
        }
        else if (bpp == 2) {
            // Mono16
            svlConverter::float32toGray16(input, reinterpret_cast<unsigned short*>(GetUCharPointer(videoch)), size, fparam);
            return SVL_OK;
        }
    }

    return SVL_FAIL;
}

int svlSampleImage::ImportImage(const svlSampleImage* image, const int param)
{
    if (image == 0) return SVL_FAIL;
    return ImportImage(*image, param);
}

int svlSampleImage::ImportImage(const svlSampleImage& image, const int param)
{
    const unsigned int channels = image.GetVideoChannels();
    if (channels != GetVideoChannels()) return SVL_FAIL;
    
    int ret = SVL_OK;
    
    for (unsigned int i = 0; i < channels; i ++) {
        if (ImportImage(image, i, i, param) != SVL_OK) ret = SVL_FAIL;
    }
    
    return ret;
}

int svlSampleImage::ImportImage(const svlSampleImage* image, const unsigned int src_channel, const unsigned int dest_channel, const int param)
{
    if (image == 0) return SVL_FAIL;
    return ImportImage(*image, src_channel, dest_channel, param);
}

int svlSampleImage::ImportImage(const svlSampleImage& image, const unsigned int src_channel, const unsigned int dest_channel, const int param)
{
    if (src_channel  >= image.GetVideoChannels() || dest_channel >= GetVideoChannels()) return SVL_FAIL;
    
    SetSize(dest_channel, image.GetWidth(src_channel), image.GetHeight(src_channel));
    
    if (image.GetPixelType() != GetPixelType()) {
        return svlConverter::ConvertImage(&image, src_channel, this, dest_channel, param);
    }
    
    // Same pixel type: copy instead of convert
    memcpy(GetUCharPointer(dest_channel), image.GetUCharPointer(src_channel), GetDataSize(dest_channel));
    return SVL_OK;
}

int svlSampleImage::ImportMatrix(const svlSampleMatrix* matrix, const int param, const unsigned int videoch)
{
    if (!matrix) return SVL_FAIL;

    const unsigned int cols = matrix->GetCols();
    const unsigned int rows = matrix->GetRows();
    SetSize(videoch, cols, rows);

    svlSampleMatrix* nc_matrix = const_cast<svlSampleMatrix*>(matrix);
    const unsigned int size = cols * rows;

    switch (matrix->GetType()) {
        case svlTypeMatrixUInt8:  return ImportData(reinterpret_cast<unsigned char*>(nc_matrix->GetUCharPointer()), size, param, videoch); break;
        case svlTypeMatrixUInt16: return ImportData(reinterpret_cast<unsigned short*>(nc_matrix->GetUCharPointer()), size, param, videoch); break;
        case svlTypeMatrixUInt32: return ImportData(reinterpret_cast<unsigned int*>(nc_matrix->GetUCharPointer()), size, param, videoch); break;
        case svlTypeMatrixFloat:  return ImportData(reinterpret_cast<float*>(nc_matrix->GetUCharPointer()), size, param, videoch); break;

        case svlTypeMatrixInt8:
        case svlTypeMatrixInt16:
        case svlTypeMatrixInt32:
        case svlTypeMatrixInt64:
        case svlTypeMatrixUInt64:
        case svlTypeMatrixDouble:
        break;

        case svlTypeInvalid:
        case svlTypeStreamSource:
        case svlTypeStreamSink:
        case svlTypeImageRGB:
        case svlTypeImageRGBStereo:
        case svlTypeImageRGBA:
        case svlTypeImageRGBAStereo:
        case svlTypeImageMono8:
        case svlTypeImageMono8Stereo:
        case svlTypeImageMono16:
        case svlTypeImageMono16Stereo:
        case svlTypeImageMono32:
        case svlTypeImageMono32Stereo:
        case svlTypeImage3DMap:
        case svlTypeCUDAImageRGB:
        case svlTypeCUDAImageRGBStereo:
        case svlTypeCUDAImageRGBA:
        case svlTypeCUDAImageRGBAStereo:
        case svlTypeCUDAImageMono8:
        case svlTypeCUDAImageMono8Stereo:
        case svlTypeCUDAImageMono16:
        case svlTypeCUDAImageMono16Stereo:
        case svlTypeCUDAImageMono32:
        case svlTypeCUDAImageMono32Stereo:
        case svlTypeCUDAImage3DMap:
        case svlTypeTransform3D:
        case svlTypeTargets:
        case svlTypeText:
        case svlTypeCameraGeometry:
        case svlTypeBlobs:
        break;
    }

    return SVL_FAIL;
}

int svlSampleImage::ImportSample(const svlSample* sample)
{
    const svlSampleImage* image = dynamic_cast<const svlSampleImage*>(sample);
    if (image) return ImportImage(image);

    const svlSampleMatrix* matrix = dynamic_cast<const svlSampleMatrix*>(sample);
    if (matrix) return ImportMatrix(matrix);

    return SVL_FAIL;
}

int svlSampleImage::ImportSample(const svlSample* sample, const unsigned int videoch)
{
    const svlSampleImage* image = dynamic_cast<const svlSampleImage*>(sample);
    if (image) return ImportImage(image, videoch, videoch);
    
    const svlSampleMatrix* matrix = dynamic_cast<const svlSampleMatrix*>(sample);
    if (matrix) return ImportMatrix(matrix, videoch);
    
    return SVL_FAIL;
}

