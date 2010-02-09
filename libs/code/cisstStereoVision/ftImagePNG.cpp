/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $
  
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

#include "ftImagePNG.h"
#include <cisstStereoVision/svlStreamDefs.h>
#include "png.h"


const static int PNG_DEFAULT_QUALITY = 4;

/*****************************************/
/*** Callbacks for direct memory write ***/
/*****************************************/

typedef struct _PNG_target_mem_info {
    unsigned char* ptr;
    unsigned int size;
    unsigned int comprsize;
    bool error;
} PNG_target_mem_info;

void PNG_user_write_data_proc(png_structp png_ptr, png_bytep data, png_size_t length)
{
    if (png_ptr && png_ptr->io_ptr && data && length) {
        PNG_target_mem_info* targetinfo = reinterpret_cast<PNG_target_mem_info*>(png_ptr->io_ptr);
        if (!targetinfo->error && targetinfo->ptr && targetinfo->size && (targetinfo->size - targetinfo->comprsize) >= length) {
            memcpy(targetinfo->ptr + targetinfo->comprsize, data, length);
            targetinfo->comprsize += length;
            targetinfo->error = false;
            return;
        }
        targetinfo->error = true;
    }
    // TO DO: Some 'Fatal Error' handling
}

void PNG_user_flush_data_proc(png_structp png_ptr)
{
    // NOP
}


/*************************************/
/*** ftImagePNG class ****************/
/*************************************/

CMN_IMPLEMENT_SERVICES(ftImagePNG)

ftImagePNG::ftImagePNG() :
    svlImageCodec(),
    cmnGenericObject(),
    pngBuffer(0),
    pngRows(0),
    pngBufferSize(0),
    pngRowsSize(0)
{
    ExtensionList = ".png;";
}

ftImagePNG::~ftImagePNG()
{
    if (pngBuffer) delete [] pngBuffer;
    if (pngRows) delete [] pngRows;
}

int ftImagePNG::ReadDimensions(const std::string &filename, unsigned int &width, unsigned int &height)
{
    return SVL_FAIL;
}

int ftImagePNG::ReadDimensions(std::istream &stream, unsigned int &width, unsigned int &height)
{
    return SVL_FAIL;
}

int ftImagePNG::ReadDimensions(const unsigned char *buffer, const size_t buffersize, unsigned int &width, unsigned int &height)
{
    return SVL_FAIL;
}

int ftImagePNG::Read(svlSampleImageBase &image, const unsigned int videoch, const std::string &filename, bool noresize)
{
    return SVL_FAIL;
}

int ftImagePNG::Read(svlSampleImageBase &image, const unsigned int videoch, std::istream &stream, bool noresize)
{
    return SVL_FAIL;
}

int ftImagePNG::Read(svlSampleImageBase &image, const unsigned int videoch, const unsigned char *buffer, const size_t buffersize, bool noresize)
{
    return SVL_FAIL;
}

int ftImagePNG::Write(const svlSampleImageBase &image, const unsigned int videoch, const std::string &filename, const int compression)
{
    std::ofstream stream(filename.c_str(), std::ios_base::out | std::ios_base::binary);
    return Write(image, videoch, stream, compression);
}

int ftImagePNG::Write(const svlSampleImageBase &image, const unsigned int videoch, std::ostream &stream, const int compression)
{
    if (videoch >= image.GetVideoChannels()) return SVL_FAIL;

    size_t size = image.GetDataSize(videoch) + 1024;
    if (!pngBuffer) {
        pngBuffer = new unsigned char[size];
        pngBufferSize = size;
    }
    else if (pngBuffer && pngBufferSize < size) {
        delete [] pngBuffer;
        pngBuffer = new unsigned char[size];
        pngBufferSize = size;
    }

    if (Write(image, videoch, pngBuffer, size, compression) == SVL_OK) {
        stream.write(reinterpret_cast<char*>(pngBuffer), size);
        return SVL_OK;
    }

    return SVL_FAIL;
}

int ftImagePNG::Write(const svlSampleImageBase &image, const unsigned int videoch, unsigned char *buffer, size_t &buffersize, const int compression)
{
    if (videoch >= image.GetVideoChannels()) return SVL_FAIL;
    if (!buffer) return SVL_FAIL;

    const int width = static_cast<int>(image.GetWidth(videoch));
    const int height = static_cast<int>(image.GetHeight(videoch));
    const int row_stride = width * 3;
    unsigned char *src = const_cast<unsigned char*>(image.GetUCharPointer(videoch));
    int i, compr = (compression >= 0) ? std::min(compression, 9) : PNG_DEFAULT_QUALITY;

    // Allocate row buffer if not done yet
    if (!pngRows) {
        pngRows = new unsigned char*[height];
        pngRowsSize = height;
    }
    else if (pngRows && pngRowsSize < static_cast<unsigned int>(height)) {
        delete [] pngRows;
        pngRows = new unsigned char*[height];
        pngRowsSize = height;
    }

    PNG_target_mem_info targetinfo;
    targetinfo.ptr = buffer;
    targetinfo.size = buffersize;
    targetinfo.comprsize = 0;
    targetinfo.error = false;

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (!png_ptr) return SVL_FAIL;

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, reinterpret_cast<png_infopp>(0));
        return SVL_FAIL;
    }

    // error handling for the following calls
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return SVL_FAIL;
    }
    png_set_write_fn(png_ptr,
                     &targetinfo,
                     PNG_user_write_data_proc,
                     PNG_user_flush_data_proc);
    if (targetinfo.error) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return SVL_FAIL;
    }

    // error handling for the following calls
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return SVL_FAIL;
    }
    // set compression options
    png_set_compression_level(png_ptr, compr);
    png_set_IHDR(png_ptr,
                 info_ptr,
                 width,
                 height,
                 8,
                 PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE,
                 PNG_FILTER_TYPE_DEFAULT);
    png_set_bgr(png_ptr);
    // write header
    png_write_info(png_ptr, info_ptr);
    if (targetinfo.error) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return SVL_FAIL;
    }

    // generate row pointer array
    for (i = 0; i < height; i ++) {
        pngRows[i] = src;
        src += row_stride;
    }

    // error handling for the following call
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return SVL_FAIL;
    }
    // write image data
    png_write_image(png_ptr, pngRows);
    if (targetinfo.error) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return SVL_FAIL;
    }

    // error handling for the following call
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return SVL_FAIL;
    }
    // write ending
    png_write_end(png_ptr, 0);

    // clean up
    png_destroy_write_struct(&png_ptr, &info_ptr);

    buffersize = static_cast<int>(targetinfo.comprsize);

    return SVL_OK;
}

