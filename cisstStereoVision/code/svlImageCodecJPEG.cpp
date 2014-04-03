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

#include "svlImageCodecJPEG.h"
#include <cisstStereoVision/svlTypes.h>
#include <setjmp.h>

#if defined(_MSC_VER) && defined(_WIN64)
    // Balazs: seems to be needed on Visual Studio 2008 x64
    #undef FAR
    #ifndef XMD_H
        #define XMD_H
        #include "jpeglib.h"
        #undef XMD_H
    #endif // XMD_H
#else // _MSC_VER && _WIN64
    #include "jpeglib.h"
#endif // _MSC_VER && _WIN64

#define __JPEG_ERROR_VERBOSE__

const static int JPEG_DEFAULT_QUALITY = 85;


/***********************************/
/*** Callback for error handling ***/
/***********************************/

typedef struct _JPEG_custom_error_mgr {
    jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
} JPEG_custom_error_mgr;
typedef JPEG_custom_error_mgr * JPEG_error_ptr;

void JPEG_error_exit_proc(j_common_ptr cinfo)
{
    JPEG_error_ptr errptr = (JPEG_error_ptr)cinfo->err;
#ifdef __JPEG_ERROR_VERBOSE__
    (*cinfo->err->output_message)(cinfo);
#endif // __JPEG_ERROR_VERBOSE__
    longjmp(errptr->setjmp_buffer, 1);
}


/*******************************/
/*** Callbacks for stream IO ***/
/*******************************/

const static size_t JPEG_STREAM_BUF_SIZE = 16384;

typedef struct _JPEG_custom_stream_source_mgr {
    jpeg_source_mgr pub;
    std::istream* is;
    JOCTET* buffer;
} JPEG_custom_stream_source_mgr;
typedef JPEG_custom_stream_source_mgr * JPEG_stream_source_ptr;

void JPEG_init_stream_source_proc(j_decompress_ptr CMN_UNUSED(cinfo))
{
    // source buffer and buffer size has already been specified
}

boolean JPEG_fill_stream_input_proc(j_decompress_ptr cinfo)
{
    JPEG_stream_source_ptr src = reinterpret_cast<JPEG_stream_source_ptr>(cinfo->src);

    src->is->read(reinterpret_cast<char*>(src->buffer), JPEG_STREAM_BUF_SIZE);
    size_t bytes = src->is->gcount();
    if (bytes == 0) {
        // Retrieve whatever is possible
        src->buffer[0] = static_cast<JOCTET>(0xFF);
        src->buffer[1] = static_cast<JOCTET>(JPEG_EOI);
        bytes = 2;
    }
    src->pub.next_input_byte = src->buffer;
    src->pub.bytes_in_buffer = bytes;

    return TRUE;
}

void JPEG_skip_stream_input_data_proc(j_decompress_ptr cinfo, long num_bytes)
{
    JPEG_stream_source_ptr src = reinterpret_cast<JPEG_stream_source_ptr>(cinfo->src);

    if (num_bytes > 0) {
        while (num_bytes > static_cast<long>(src->pub.bytes_in_buffer)) {
            num_bytes -= static_cast<long>(src->pub.bytes_in_buffer);
            JPEG_fill_stream_input_proc(cinfo);
        }
        src->pub.next_input_byte += static_cast<size_t>(num_bytes);
        src->pub.bytes_in_buffer -= static_cast<size_t>(num_bytes);
    }
}

void JPEG_term_stream_source_proc(j_decompress_ptr cinfo)
{
    JPEG_stream_source_ptr src = reinterpret_cast<JPEG_stream_source_ptr>(cinfo->src);

    // must seek backward so that future reads will start at correct place.
    src->is->clear();
    src->is->seekg(src->is->tellg() - static_cast<std::streampos>(src->pub.bytes_in_buffer));
}


/****************************************/
/*** Callbacks for direct memory read ***/
/****************************************/

typedef struct _JPEG_custom_source_mgr {
    jpeg_source_mgr pub;
    JOCTET eoi_buffer[2];
} JPEG_custom_source_mgr;
typedef JPEG_custom_source_mgr * JPEG_source_ptr;

void JPEG_init_source_proc(j_decompress_ptr CMN_UNUSED(cinfo))
{
    // source buffer and buffer size has already been specified
}

boolean JPEG_fill_input_proc(j_decompress_ptr cinfo)
{
    JPEG_source_ptr src = reinterpret_cast<JPEG_source_ptr>(cinfo->src);

    // Retrieve whatever is possible
    src->eoi_buffer[0] = static_cast<JOCTET>(0xFF);
    src->eoi_buffer[1] = static_cast<JOCTET>(JPEG_EOI);
    src->pub.next_input_byte = src->eoi_buffer;
    src->pub.bytes_in_buffer = 2;

    return TRUE;
}

void JPEG_skip_input_data_proc(j_decompress_ptr cinfo, long num_bytes)
{
    JPEG_source_ptr src = reinterpret_cast<JPEG_source_ptr>(cinfo->src);

    if (num_bytes > 0) {
        while (num_bytes > static_cast<long>(src->pub.bytes_in_buffer)) {
            num_bytes -= static_cast<long>(src->pub.bytes_in_buffer);
            JPEG_fill_input_proc(cinfo);
        }
        src->pub.next_input_byte += static_cast<size_t>(num_bytes);
        src->pub.bytes_in_buffer -= static_cast<size_t>(num_bytes);
    }
}

void JPEG_term_source_proc(j_decompress_ptr CMN_UNUSED(cinfo))
{
    // nothing to clean up
}


/*****************************************/
/*** Callbacks for direct memory write ***/
/*****************************************/

void JPEG_init_destination_proc(j_compress_ptr CMN_UNUSED(cinfo))
{
    // destination buffer and buffer size has already been specified
}

boolean JPEG_empty_output_buffer_proc(j_compress_ptr CMN_UNUSED(cinfo))
{
    // buffer filled: this is an error in the mem-to-mem case
    return FALSE;
}

void JPEG_term_destination_proc(j_compress_ptr CMN_UNUSED(cinfo))
{
    // nothing to clean up
}


/*************************************/
/*** svlImageCodecJPEG class *********/
/*************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlImageCodecJPEG, svlImageCodecBase)

svlImageCodecJPEG::svlImageCodecJPEG() :
    svlImageCodecBase(),
    jpegBuffer(0),
    jpegRowBuffer(0),
    jpegBufferSize(0),
    jpegRowBufferSize(0)
{
    SetExtensionList(".jpg;.jpeg;");
}

svlImageCodecJPEG::~svlImageCodecJPEG()
{
    if (jpegBuffer) delete [] jpegBuffer;
    if (jpegRowBuffer) delete [] jpegRowBuffer;
}

int svlImageCodecJPEG::ReadDimensions(const std::string &filename, unsigned int &width, unsigned int &height)
{
    std::ifstream stream(filename.c_str(), std::ios_base::in | std::ios_base::binary);
    return ReadDimensions(stream, width, height);
}

int svlImageCodecJPEG::ReadDimensions(std::istream &stream, unsigned int &width, unsigned int &height)
{
    // Allocate buffer if not done yet
    if (!jpegBuffer) {
        jpegBuffer = new unsigned char[JPEG_STREAM_BUF_SIZE];
        jpegBufferSize = JPEG_STREAM_BUF_SIZE;
    }
    else if (jpegBuffer && jpegBufferSize < JPEG_STREAM_BUF_SIZE) {
        delete [] jpegBuffer;
        jpegBuffer = new unsigned char[JPEG_STREAM_BUF_SIZE];
        jpegBufferSize = JPEG_STREAM_BUF_SIZE;
    }

    // Setup source manager to handle input stream
    JPEG_custom_stream_source_mgr source_manager;
    source_manager.is = &stream;
    source_manager.buffer = reinterpret_cast<JOCTET*>(jpegBuffer);
    source_manager.pub.init_source = JPEG_init_stream_source_proc;
    source_manager.pub.fill_input_buffer = JPEG_fill_stream_input_proc;
    source_manager.pub.skip_input_data = JPEG_skip_stream_input_data_proc;
    source_manager.pub.resync_to_restart = jpeg_resync_to_restart; // using default handler
    source_manager.pub.term_source = JPEG_term_stream_source_proc;
    source_manager.pub.next_input_byte = 0;
    source_manager.pub.bytes_in_buffer = 0;

    width = 0;
    height = 0;

    // Setup error handling: failure without crashing
    jpeg_decompress_struct cinfo;
    JPEG_custom_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = JPEG_error_exit_proc;
    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        return SVL_OK;
    }

    // Setup decompression and source manager
    cinfo.output_components = 3;
    cinfo.out_color_space = JCS_RGB;
    jpeg_create_decompress(&cinfo);
    cinfo.src = reinterpret_cast<jpeg_source_mgr*>(&source_manager);

    // Read header
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    width = cinfo.output_width;
    height = cinfo.output_height;

    // Clean up
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    return SVL_OK;
}

int svlImageCodecJPEG::ReadDimensions(const unsigned char *buffer, const size_t buffersize, unsigned int &width, unsigned int &height)
{
    // Setup source manager to handle input memory buffer
    JPEG_custom_source_mgr source_manager;
    source_manager.pub.init_source = JPEG_init_source_proc;
    source_manager.pub.fill_input_buffer = JPEG_fill_input_proc;
    source_manager.pub.skip_input_data = JPEG_skip_input_data_proc;
    source_manager.pub.resync_to_restart = jpeg_resync_to_restart; // using default handler
    source_manager.pub.term_source = JPEG_term_source_proc;
    source_manager.pub.next_input_byte = buffer;
    source_manager.pub.bytes_in_buffer = buffersize;

    width = 0;
    height = 0;

    // Setup error handling: failure without crashing
    jpeg_decompress_struct cinfo;
    JPEG_custom_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = JPEG_error_exit_proc;
    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        return SVL_OK;
    }

    // Setup decompression and source manager
    cinfo.output_components = 3;
    cinfo.out_color_space = JCS_RGB;
    jpeg_create_decompress(&cinfo);
    cinfo.src = reinterpret_cast<jpeg_source_mgr*>(&source_manager);

    // Read header
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    width = cinfo.output_width;
    height = cinfo.output_height;

    // Clean up
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    return SVL_OK;
}

int svlImageCodecJPEG::Read(svlSampleImage &image, const unsigned int videoch, const std::string &filename, bool noresize)
{
    std::ifstream stream(filename.c_str(), std::ios_base::in | std::ios_base::binary);
    return Read(image, videoch, stream, noresize);
}

int svlImageCodecJPEG::Read(svlSampleImage &image, const unsigned int videoch, std::istream &stream, bool noresize)
{
    if (videoch >= image.GetVideoChannels()) return SVL_FAIL;

    int width, height, i, row_stride;
    unsigned char *dest, *src;
    JSAMPROW row_pointer[1];

    // Allocate buffer if not done yet
    if (!jpegBuffer) {
        jpegBuffer = new unsigned char[JPEG_STREAM_BUF_SIZE];
        jpegBufferSize = JPEG_STREAM_BUF_SIZE;
    }
    else if (jpegBuffer && jpegBufferSize < JPEG_STREAM_BUF_SIZE) {
        delete [] jpegBuffer;
        jpegBuffer = new unsigned char[JPEG_STREAM_BUF_SIZE];
        jpegBufferSize = JPEG_STREAM_BUF_SIZE;
    }

    // Setup source manager to handle input stream
    JPEG_custom_stream_source_mgr source_manager;
    source_manager.is = &stream;
    source_manager.buffer = reinterpret_cast<JOCTET*>(jpegBuffer);
    source_manager.pub.init_source = JPEG_init_stream_source_proc;
    source_manager.pub.fill_input_buffer = JPEG_fill_stream_input_proc;
    source_manager.pub.skip_input_data = JPEG_skip_stream_input_data_proc;
    source_manager.pub.resync_to_restart = jpeg_resync_to_restart; // using default handler
    source_manager.pub.term_source = JPEG_term_stream_source_proc;
    source_manager.pub.next_input_byte = 0;
    source_manager.pub.bytes_in_buffer = 0;

    // Setup error handling: failure without crashing
    jpeg_decompress_struct cinfo;
    JPEG_custom_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = JPEG_error_exit_proc;
    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        return SVL_FAIL;
    }

    // Setup decompression and source manager
    cinfo.output_components = 3;
    cinfo.out_color_space = JCS_RGB;
    jpeg_create_decompress(&cinfo);
    cinfo.src = reinterpret_cast<jpeg_source_mgr*>(&source_manager);

    // Read header
    jpeg_read_header(&cinfo, TRUE);

    // Proceed with decompression
    jpeg_start_decompress(&cinfo);
    width = cinfo.output_width;
    height = cinfo.output_height;
    row_stride = width * 3;

    if (width  < 1 || height < 1) return SVL_FAIL;

    // Allocate image buffer if not done yet
    if (static_cast<unsigned int>(width)  != image.GetWidth(videoch) ||
        static_cast<unsigned int>(height) != image.GetHeight(videoch)) {
        if (noresize) return SVL_FAIL;
        image.SetSize(videoch, static_cast<unsigned int>(width), static_cast<unsigned int>(height));
    }

    // Allocate row buffer if not done yet
    if (!jpegRowBuffer) {
        jpegRowBuffer = new unsigned char[row_stride];
        jpegRowBufferSize = row_stride;
    }
    else if (jpegRowBuffer && jpegRowBufferSize < static_cast<unsigned int>(row_stride)) {
        delete [] jpegRowBuffer;
        jpegRowBuffer = new unsigned char[row_stride];
        jpegRowBufferSize = row_stride;
    }

    dest = image.GetUCharPointer(videoch);
    while (cinfo.output_scanline < static_cast<unsigned int>(height)) {
        row_pointer[0] = jpegRowBuffer;
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
        src = jpegRowBuffer;

        // RGB >> BGR
        for (i = 0; i < width; i ++) {
            *dest = src[2]; dest ++;
            *dest = src[1]; dest ++;
            *dest = src[0]; dest ++;
            src += 3;
        }
    }

    // Clean up
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    return SVL_OK;
}

int svlImageCodecJPEG::Read(svlSampleImage &image, const unsigned int videoch, const unsigned char *buffer, const size_t buffersize, bool noresize)
{
    if (videoch >= image.GetVideoChannels()) return SVL_FAIL;
    if (!buffer) return SVL_FAIL;

    int width, height, i, row_stride;
    unsigned char *dest, *src;
    JSAMPROW row_pointer[1];

    // Setup source manager to handle input memory buffer
    JPEG_custom_source_mgr source_manager;
    source_manager.pub.init_source = JPEG_init_source_proc;
    source_manager.pub.fill_input_buffer = JPEG_fill_input_proc;
    source_manager.pub.skip_input_data = JPEG_skip_input_data_proc;
    source_manager.pub.resync_to_restart = jpeg_resync_to_restart; // using default handler
    source_manager.pub.term_source = JPEG_term_source_proc;
    source_manager.pub.next_input_byte = buffer;
    source_manager.pub.bytes_in_buffer = buffersize;

    // Setup error handling: failure without crashing
    jpeg_decompress_struct cinfo;
    JPEG_custom_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = JPEG_error_exit_proc;
    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        return SVL_FAIL;
    }

    // Setup decompression and source manager
    cinfo.output_components = 3;
    cinfo.out_color_space = JCS_RGB;
    jpeg_create_decompress(&cinfo);
    cinfo.src = reinterpret_cast<jpeg_source_mgr*>(&source_manager);

    // Read header
    jpeg_read_header(&cinfo, TRUE);

    // Proceed with decompression
    jpeg_start_decompress(&cinfo);
    width = cinfo.output_width;
    height = cinfo.output_height;
    row_stride = width * 3;

    if (width  < 1 || height < 1) return SVL_FAIL;

    // Allocate image buffer if not done yet
    if (static_cast<unsigned int>(width)  != image.GetWidth(videoch) ||
        static_cast<unsigned int>(height) != image.GetHeight(videoch)) {
        if (noresize) return SVL_FAIL;
        image.SetSize(videoch, static_cast<unsigned int>(width), static_cast<unsigned int>(height));
    }

    // Allocate row buffer if not done yet
    if (!jpegRowBuffer) {
        jpegRowBuffer = new unsigned char[row_stride];
        jpegRowBufferSize = row_stride;
    }
    else if (jpegRowBuffer && jpegRowBufferSize < static_cast<unsigned int>(row_stride)) {
        delete [] jpegRowBuffer;
        jpegRowBuffer = new unsigned char[row_stride];
        jpegRowBufferSize = row_stride;
    }

    dest = image.GetUCharPointer(videoch);
    while (cinfo.output_scanline < static_cast<unsigned int>(height)) {
        row_pointer[0] = jpegRowBuffer;
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
        src = jpegRowBuffer;

        // RGB >> BGR
        for (i = 0; i < width; i ++) {
            *dest = src[2]; dest ++;
            *dest = src[1]; dest ++;
            *dest = src[0]; dest ++;
            src += 3;
        }
    }

    // Clean up
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    return SVL_OK;
}

int svlImageCodecJPEG::Write(const svlSampleImage &image, const unsigned int videoch, const std::string &filename, const int compression)
{
    std::ofstream stream(filename.c_str(), std::ios_base::out | std::ios_base::binary);
    return Write(image, videoch, stream, compression);
}

int svlImageCodecJPEG::Write(const svlSampleImage &image, const unsigned int videoch, std::ostream &stream, const int compression)
{
    if (videoch >= image.GetVideoChannels()) return SVL_FAIL;

    size_t size = image.GetDataSize(videoch) + 1024;
    if (!jpegBuffer) {
        jpegBuffer = new unsigned char[size];
        jpegBufferSize = size;
    }
    else if (jpegBuffer && jpegBufferSize < size) {
        delete [] jpegBuffer;
        jpegBuffer = new unsigned char[size];
        jpegBufferSize = size;
    }

    if (Write(image, videoch, jpegBuffer, size, compression) == SVL_OK) {
        stream.write(reinterpret_cast<char*>(jpegBuffer), size);
        return SVL_OK;
    }

    return SVL_FAIL;
}

int svlImageCodecJPEG::Write(const svlSampleImage &image, const unsigned int videoch, unsigned char *buffer, size_t &buffersize, const int compression)
{
    if (videoch >= image.GetVideoChannels()) return SVL_FAIL;
    if (!buffer) return SVL_FAIL;

    const int width = static_cast<int>(image.GetWidth(videoch));
    const int height = static_cast<int>(image.GetHeight(videoch));
    const int row_stride = width * 3;
    unsigned char *src = const_cast<unsigned char*>(image.GetUCharPointer(videoch));
    unsigned char *buff_bgr;
    JSAMPROW row_pointer[1];
    int i, offset;

    // Allocate row buffer if not done yet
    if (!jpegRowBuffer) {
        jpegRowBuffer = new unsigned char[row_stride];
        jpegRowBufferSize = row_stride;
    }
    else if (jpegRowBuffer && jpegRowBufferSize < static_cast<unsigned int>(row_stride)) {
        delete [] jpegRowBuffer;
        jpegRowBuffer = new unsigned char[row_stride];
        jpegRowBufferSize = row_stride;
    }

    // Setup destination manager to handle output to memory buffer
    jpeg_destination_mgr destination_manager;
    destination_manager.init_destination = JPEG_init_destination_proc;
    destination_manager.empty_output_buffer = JPEG_empty_output_buffer_proc;
    destination_manager.term_destination = JPEG_term_destination_proc;
    destination_manager.next_output_byte = buffer;
    destination_manager.free_in_buffer = buffersize;

    // Setup error handling: failure without crashing
    jpeg_compress_struct cinfo;
    JPEG_custom_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = JPEG_error_exit_proc;
    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_compress(&cinfo);
        return SVL_FAIL;
    }

    // Setup compression and image parameters
    jpeg_create_compress(&cinfo);
    cinfo.dest = &destination_manager;
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, (compression >= 0) ? std::min(compression, 100) : JPEG_DEFAULT_QUALITY, TRUE);

    // Proceed with compression
    jpeg_start_compress(&cinfo, TRUE);
    while (cinfo.next_scanline < cinfo.image_height) {
        offset = cinfo.next_scanline * row_stride;

        // RGB >> BGR
        buff_bgr = jpegRowBuffer;
        for (i = 0; i < width; i ++) {
            buff_bgr[2] = src[offset]; offset ++;
            buff_bgr[1] = src[offset]; offset ++;
            buff_bgr[0] = src[offset]; offset ++;
            buff_bgr += 3;
        }

        row_pointer[0] = jpegRowBuffer;
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    // Clean up
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    buffersize -= destination_manager.free_in_buffer;

    return SVL_OK;
}

