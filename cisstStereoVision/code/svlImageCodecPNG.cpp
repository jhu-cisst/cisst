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

#include "svlImageCodecPNG.h"
#include <cisstStereoVision/svlTypes.h>
#include <cisstStereoVision/svlConverters.h>
#include "png.h"


const static int PNG_DEFAULT_QUALITY = 4;
const static int PNG_SIG_SIZE = 8;


/*****************************************/
/*** Callbacks for reading from stream ***/
/*****************************************/

typedef struct _PNG_source_stream {
    std::istream* stream;
    bool error;
} PNG_source_stream;

void PNG_read_data_stream(png_structp png_ptr, png_bytep data, png_size_t length)
{
    PNG_source_stream* source = reinterpret_cast<PNG_source_stream*>(png_get_io_ptr(png_ptr));
    if (source && source->error == false) {
        if (source->stream) {
            if (source->stream->read(reinterpret_cast<char*>(data), length).good()) {
                source->error = false;
                return;
            }
        }
        source->error = true;
    }
}


/*****************************************/
/*** Callbacks for reading from memory ***/
/*****************************************/

typedef struct _PNG_source_memory {
    unsigned char* buffer;
    unsigned int buffersize;
    bool error;
} _PNG_source_memory;

void PNG_read_data_memory(png_structp png_ptr, png_bytep data, png_size_t length)
{
    _PNG_source_memory* source = reinterpret_cast<_PNG_source_memory*>(png_get_io_ptr(png_ptr));
    if (source && source->error == false) {
        if (source->buffer && source->buffersize >= length) {
            memcpy(data, source->buffer, length);
            source->buffer += length;
            source->buffersize -= static_cast<unsigned int>(length);
            source->error = false;
            return;
        }
        source->error = true;
    }
}


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
    PNG_target_mem_info* targetinfo = reinterpret_cast<PNG_target_mem_info*>(png_get_io_ptr(png_ptr));
    if (targetinfo) {
        if (targetinfo->error == false && data && length && targetinfo->ptr && targetinfo->size && (targetinfo->size - targetinfo->comprsize) >= length) {
            memcpy(targetinfo->ptr + targetinfo->comprsize, data, length);
            targetinfo->comprsize += static_cast<unsigned int>(length);
            targetinfo->error = false;
            return;
        }
        targetinfo->error = true;
    }
    // TO DO: Some 'Fatal Error' handling
}

void PNG_user_flush_data_proc(png_structp CMN_UNUSED(png_ptr))
{
    // NOP
}


/*************************************/
/*** svlImageCodecPNG class **********/
/*************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlImageCodecPNG, svlImageCodecBase)

svlImageCodecPNG::svlImageCodecPNG() :
    svlImageCodecBase(),
    pngBuffer(0),
    pngRows(0),
    pngBufferSize(0),
    pngRowsSize(0)
{
    SetExtensionList(".png;");
}

svlImageCodecPNG::~svlImageCodecPNG()
{
    if (pngBuffer) delete [] pngBuffer;
    if (pngRows) delete [] pngRows;
}

int svlImageCodecPNG::ReadDimensions(const std::string &filename, unsigned int &width, unsigned int &height)
{
    std::ifstream stream(filename.c_str(), std::ios_base::in | std::ios_base::binary);
    return ReadDimensions(stream, width, height);
}

int svlImageCodecPNG::ReadDimensions(std::istream &stream, unsigned int &width, unsigned int &height)
{
    // check file for signature
    png_byte pngsig[PNG_SIG_SIZE];
    if (stream.read(reinterpret_cast<char*>(pngsig), PNG_SIG_SIZE).fail() ||
        png_sig_cmp(pngsig, 0, PNG_SIG_SIZE) != 0) {
        return SVL_FAIL;
    }

    // create read structure
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (png_ptr == 0) return SVL_FAIL;

    // create info structure
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == 0) {
        png_destroy_read_struct(&png_ptr, reinterpret_cast<png_infopp>(0), reinterpret_cast<png_infopp>(0));
        return SVL_FAIL;
    }

    width = 0;
    height = 0;

    // setup error handling
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, reinterpret_cast<png_infopp>(0));
        return SVL_OK;
    }
    PNG_source_stream source;
    source.stream = &stream;
    source.error = false;
    png_set_read_fn(png_ptr, &source, PNG_read_data_stream);

    png_set_sig_bytes(png_ptr, PNG_SIG_SIZE);
    png_read_info(png_ptr, info_ptr);

    // check file header
    width = static_cast<unsigned int>(png_get_image_width(png_ptr, info_ptr));
    height = static_cast<unsigned int>(png_get_image_height(png_ptr, info_ptr));

    // clean up
    png_destroy_read_struct(&png_ptr, &info_ptr,reinterpret_cast<png_infopp>(0));

    return SVL_OK;
}

int svlImageCodecPNG::ReadDimensions(const unsigned char *buffer, const size_t buffersize, unsigned int &width, unsigned int &height)
{
    if (!buffer || buffersize < static_cast<unsigned int>(PNG_SIG_SIZE)) return SVL_FAIL;

    // check file for signature
    if (png_sig_cmp(const_cast<unsigned char*>(buffer), 0, PNG_SIG_SIZE) != 0) return SVL_FAIL;

    // create read structure
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (png_ptr == 0) return SVL_FAIL;

    // create info structure
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == 0) {
        png_destroy_read_struct(&png_ptr, reinterpret_cast<png_infopp>(0), reinterpret_cast<png_infopp>(0));
        return SVL_FAIL;
    }

    width = 0;
    height = 0;

    // setup error handling
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, reinterpret_cast<png_infopp>(0));
        return SVL_OK;
    }
    _PNG_source_memory source;
    source.buffer = const_cast<unsigned char*>(buffer + PNG_SIG_SIZE);
    source.buffersize = static_cast<unsigned int>(buffersize - PNG_SIG_SIZE);
    source.error = false;
    png_set_read_fn(png_ptr, &source, PNG_read_data_memory);

    png_set_sig_bytes(png_ptr, PNG_SIG_SIZE);
    png_read_info(png_ptr, info_ptr);

    // check file header
    width = static_cast<unsigned int>(png_get_image_width(png_ptr, info_ptr));
    height = static_cast<unsigned int>(png_get_image_height(png_ptr, info_ptr));

    // clean up
    png_destroy_read_struct(&png_ptr, &info_ptr,reinterpret_cast<png_infopp>(0));

    return SVL_OK;
}

int svlImageCodecPNG::Read(svlSampleImage &image, const unsigned int videoch, const std::string &filename, bool noresize)
{
    std::ifstream stream(filename.c_str(), std::ios_base::in | std::ios_base::binary);
    return Read(image, videoch, stream, noresize);
}

int svlImageCodecPNG::Read(svlSampleImage &image, const unsigned int videoch, std::istream &stream, bool noresize)
{
    if (videoch >= image.GetVideoChannels()) return SVL_FAIL;

    // check file for signature
    png_byte pngsig[PNG_SIG_SIZE];
    if (stream.read(reinterpret_cast<char*>(pngsig), PNG_SIG_SIZE).fail() ||
        png_sig_cmp(pngsig, 0, PNG_SIG_SIZE) != 0) {
        return SVL_FAIL;
    }

    // create read structure
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (png_ptr == 0) return SVL_FAIL;

    // create info structure
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == 0) {
        png_destroy_read_struct(&png_ptr, reinterpret_cast<png_infopp>(0), reinterpret_cast<png_infopp>(0));
        return SVL_FAIL;
    }

    // setup error handling
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, reinterpret_cast<png_infopp>(0));
        return SVL_FAIL;
    }
    PNG_source_stream source;
    source.stream = &stream;
    source.error = false;
    png_set_read_fn(png_ptr, &source, PNG_read_data_stream);

    png_set_sig_bytes(png_ptr, PNG_SIG_SIZE);
    png_read_info(png_ptr, info_ptr);

    // check file header
    const unsigned int width = static_cast<unsigned int>(png_get_image_width(png_ptr, info_ptr));
    const unsigned int height = static_cast<unsigned int>(png_get_image_height(png_ptr, info_ptr));
    const unsigned int row_stride = width * 3;

    if (width  < 1 || height < 1) return SVL_FAIL;

    // Allocate image buffer if not done yet
    if (static_cast<unsigned int>(width)  != image.GetWidth(videoch) ||
        static_cast<unsigned int>(height) != image.GetHeight(videoch)) {
        if (noresize) return SVL_FAIL;
        image.SetSize(videoch, static_cast<unsigned int>(width), static_cast<unsigned int>(height));
    }

    // allocate row buffer if not done yet
    if (!pngRows) {
        pngRows = new unsigned char*[height];
        pngRowsSize = height;
    }
    else if (pngRows && pngRowsSize < static_cast<unsigned int>(height)) {
        delete [] pngRows;
        pngRows = new unsigned char*[height];
        pngRowsSize = height;
    }

    // generate row pointer array
    unsigned char *dest = image.GetUCharPointer(videoch);
    for (unsigned int i = 0; i < height; i ++) {
        pngRows[i] = dest;
        dest += row_stride;
    }

    const unsigned int bitdepth = static_cast<unsigned int>(png_get_bit_depth(png_ptr, info_ptr));
    const unsigned int color_type = static_cast<unsigned int>(png_get_color_type(png_ptr, info_ptr));

    switch (color_type) {
        case PNG_COLOR_TYPE_PALETTE:
            png_set_palette_to_rgb(png_ptr);
        break;

        case PNG_COLOR_TYPE_GRAY:
            if (bitdepth < 8) {
#if PNG_LIBPNG_VER >= 10300
                png_set_expand_gray_1_2_4_to_8(png_ptr);
#else
                png_set_gray_1_2_4_to_8(png_ptr);
#endif
            }
        break;
    }
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png_ptr);
    }
    if (bitdepth == 16) {
        png_set_strip_16(png_ptr);
    }
    png_set_bgr(png_ptr);

    // read image
    png_read_image(png_ptr, pngRows);

    // read ending
    png_read_end(png_ptr, info_ptr);

    if (color_type == PNG_COLOR_TYPE_GRAY) {
        // Gray8toRGB24 can do in-place conversion
        svlConverter::Gray8toRGB24(dest, dest, width * height);
    }

    // clean up
    png_destroy_read_struct(&png_ptr, &info_ptr,reinterpret_cast<png_infopp>(0));

    return SVL_OK;
}

int svlImageCodecPNG::Read(svlSampleImage &image, const unsigned int videoch, const unsigned char *buffer, const size_t buffersize, bool noresize)
{
    if (videoch >= image.GetVideoChannels()) return SVL_FAIL;
    if (!buffer || buffersize < static_cast<unsigned int>(PNG_SIG_SIZE)) return SVL_FAIL;

    // check file for signature
    if (png_sig_cmp(const_cast<unsigned char*>(buffer), 0, PNG_SIG_SIZE) != 0) return SVL_FAIL;

    // create read structure
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (png_ptr == 0) return SVL_FAIL;

    // create info structure
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == 0) {
        png_destroy_read_struct(&png_ptr, reinterpret_cast<png_infopp>(0), reinterpret_cast<png_infopp>(0));
        return SVL_FAIL;
    }

    // setup error handling
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, reinterpret_cast<png_infopp>(0));
        return SVL_FAIL;
    }
    _PNG_source_memory source;
    source.buffer = const_cast<unsigned char*>(buffer + PNG_SIG_SIZE);
    source.buffersize = static_cast<unsigned int>(buffersize - PNG_SIG_SIZE);
    source.error = false;
    png_set_read_fn(png_ptr, &source, PNG_read_data_memory);

    png_set_sig_bytes(png_ptr, PNG_SIG_SIZE);
    png_read_info(png_ptr, info_ptr);

    // check file header
    const unsigned int width = static_cast<unsigned int>(png_get_image_width(png_ptr, info_ptr));
    const unsigned int height = static_cast<unsigned int>(png_get_image_height(png_ptr, info_ptr));
    const unsigned int row_stride = width * 3;

    if (width  < 1 || height < 1) return SVL_FAIL;

    // Allocate image buffer if not done yet
    if (static_cast<unsigned int>(width)  != image.GetWidth(videoch) ||
        static_cast<unsigned int>(height) != image.GetHeight(videoch)) {
        if (noresize) return SVL_FAIL;
        image.SetSize(videoch, static_cast<unsigned int>(width), static_cast<unsigned int>(height));
    }

    // allocate row buffer if not done yet
    if (!pngRows) {
        pngRows = new unsigned char*[height];
        pngRowsSize = height;
    }
    else if (pngRows && pngRowsSize < static_cast<unsigned int>(height)) {
        delete [] pngRows;
        pngRows = new unsigned char*[height];
        pngRowsSize = height;
    }

    // generate row pointer array
    unsigned char *dest = image.GetUCharPointer(videoch);
    for (unsigned int i = 0; i < height; i ++) {
        pngRows[i] = dest;
        dest += row_stride;
    }

    const unsigned int bitdepth = static_cast<unsigned int>(png_get_bit_depth(png_ptr, info_ptr));
    const unsigned int color_type = static_cast<unsigned int>(png_get_color_type(png_ptr, info_ptr));

    switch (color_type) {
        case PNG_COLOR_TYPE_PALETTE:
            png_set_palette_to_rgb(png_ptr);
        break;

        case PNG_COLOR_TYPE_GRAY:
            if (bitdepth < 8) {
#if PNG_LIBPNG_VER >= 10300
                png_set_expand_gray_1_2_4_to_8(png_ptr);
#else
                png_set_gray_1_2_4_to_8(png_ptr);
#endif
            }
        break;
    }
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png_ptr);
    }
    if (bitdepth == 16) {
        png_set_strip_16(png_ptr);
    }
    png_set_bgr(png_ptr);

    // read image
    png_read_image(png_ptr, pngRows);

    // read ending
    png_read_end(png_ptr, info_ptr);

    if (color_type == PNG_COLOR_TYPE_GRAY) {
        // Gray8toRGB24 can do in-place conversion
        svlConverter::Gray8toRGB24(dest, dest, width * height);
    }

    // clean up
    png_destroy_read_struct(&png_ptr, &info_ptr,reinterpret_cast<png_infopp>(0));

    return SVL_OK;
}

int svlImageCodecPNG::Write(const svlSampleImage &image, const unsigned int videoch, const std::string &filename, const int compression)
{
    std::ofstream stream(filename.c_str(), std::ios_base::out | std::ios_base::binary);
    return Write(image, videoch, stream, compression);
}

int svlImageCodecPNG::Write(const svlSampleImage &image, const unsigned int videoch, std::ostream &stream, const int compression)
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

int svlImageCodecPNG::Write(const svlSampleImage &image, const unsigned int videoch, unsigned char *buffer, size_t &buffersize, const int compression)
{
    if (videoch >= image.GetVideoChannels()) return SVL_FAIL;
    if (!buffer) return SVL_FAIL;

    const int width = static_cast<int>(image.GetWidth(videoch));
    const int height = static_cast<int>(image.GetHeight(videoch));
    const int row_stride = width * 3;
    unsigned char *src;
    int i;

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
    targetinfo.size = static_cast<unsigned int>(buffersize);
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
    png_set_compression_level(png_ptr, (compression >= 0) ? std::min(compression, 9) : PNG_DEFAULT_QUALITY);
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
    src = const_cast<unsigned char*>(image.GetUCharPointer(videoch));
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

