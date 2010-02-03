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


typedef struct _PNGTargetMemInfo {
    unsigned char* ptr;
    unsigned int size;
    unsigned int comprsize;
} PNGTargetMemInfo;

void PNG_user_write_data_proc(png_structp png_ptr, png_bytep data, png_size_t length)
{
    if (png_ptr && png_ptr->io_ptr && data && length) {
        PNGTargetMemInfo* targetinfo = reinterpret_cast<PNGTargetMemInfo*>(png_ptr->io_ptr);
        if (targetinfo->ptr && targetinfo->size && (targetinfo->size - targetinfo->comprsize) >= length) {
            memcpy(targetinfo->ptr + targetinfo->comprsize, data, length);
            targetinfo->comprsize += length;
            return;
        }
    }

    png_error(png_ptr, "PNG_user_write_data_proc: Write error");
}

void PNG_user_flush_data_proc(png_structp png_ptr)
{
    // NOP
}

int RGB24_to_PNG_mem(unsigned char* src, unsigned int width, unsigned int height, unsigned char* dest, unsigned int destsize)
{
    if (!src || !width || !height || !dest || !destsize) return -1;

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (!png_ptr) return -2;

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, (png_infopp)0);
        return -3;
    }

    PNGTargetMemInfo targetinfo;
    targetinfo.ptr = dest;
    targetinfo.size = destsize;
    targetinfo.comprsize = 0; // Important!!!

    // error handling for the following calls
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return -4;
    }
    png_set_write_fn(png_ptr,
                     &targetinfo,
                     PNG_user_write_data_proc,
                     PNG_user_flush_data_proc);

    // error handling for the following calls
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return -5;
    }
    // set compression options
    png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);
    png_set_IHDR(png_ptr,
                 info_ptr,
                 width,
                 height,
                 8,
                 PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE,
                 PNG_FILTER_TYPE_DEFAULT);
    // write header
    png_write_info(png_ptr, info_ptr);

    // generate row pointer array
    const unsigned int linesize = width * 3;
    png_byte *rawsrc = reinterpret_cast<png_byte*>(src);
    png_byte **rowptrs = new png_byte*[height];
    for (unsigned int i = 0; i < height; i ++) {
        rowptrs[i] = rawsrc;
        rawsrc += linesize;
    }

    // error handling for the following call
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        delete [] rowptrs;
        return -6;
    }
    // write image data
    png_write_image(png_ptr, rowptrs);

    // release row pointer array
    delete [] rowptrs;

    // error handling for the following call
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return -7;
    }
    // write ending
    png_write_end(png_ptr, 0);

    // clean up
    png_destroy_write_struct(&png_ptr, &info_ptr);

    return static_cast<int>(targetinfo.comprsize);
}


/*************************************/
/*** ftImagePNG class ****************/
/*************************************/

CMN_IMPLEMENT_SERVICES(ftImagePNG)

ftImagePNG::ftImagePNG() :
    svlImageCodec(),
    cmnGenericObject()
{
    ExtensionList = ".png;";
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
    return SVL_FAIL;
}

int ftImagePNG::Write(const svlSampleImageBase &image, const unsigned int videoch, std::ostream &stream, const int compression)
{
    return SVL_FAIL;
}

int ftImagePNG::Write(const svlSampleImageBase &image, const unsigned int videoch, unsigned char *buffer, size_t &buffersize, const int compression)
{
    return SVL_FAIL;
}

