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

#include "svlImageCodecBMP.h"


/*************************************/
/*** svlImageCodecBMP class **********/
/*************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlImageCodecBMP, svlImageCodecBase)

svlImageCodecBMP::svlImageCodecBMP() :
    svlImageCodecBase()
{
    SetExtensionList(".bmp;");
}

int svlImageCodecBMP::ReadDimensions(const std::string &filename, unsigned int &width, unsigned int &height)
{
    std::ifstream stream(filename.c_str(), std::ios_base::in | std::ios_base::binary);
    return ReadDimensions(stream, width, height);
}

int svlImageCodecBMP::ReadDimensions(std::istream &stream, unsigned int &width, unsigned int &height)
{
    if (stream.read(reinterpret_cast<char*>(&FileHeader), sizeof(svlBMPFileHeader)).fail() ||
        FileHeader.bfType != 0x4D42 ||
        stream.read(reinterpret_cast<char*>(&DIBHeader), sizeof(svlDIBHeader)).fail() ||
        DIBHeader.biSize  != (sizeof(svlDIBHeader))) return SVL_FAIL;

    width = static_cast<unsigned int>(DIBHeader.biWidth);
    if (DIBHeader.biHeight >= 0) height = static_cast<unsigned int>(DIBHeader.biHeight);
    else height = static_cast<unsigned int>(-DIBHeader.biHeight);

    return SVL_OK;
}

int svlImageCodecBMP::ReadDimensions(const unsigned char *buffer, const size_t buffersize, unsigned int &width, unsigned int &height)
{
    if (buffer == 0 ||
        buffersize <= (sizeof(svlBMPFileHeader) + sizeof(svlDIBHeader))) return SVL_FAIL;

    const svlBMPFileHeader *fileheader = reinterpret_cast<const svlBMPFileHeader*>(buffer);
    const svlDIBHeader *dibheader = reinterpret_cast<const svlDIBHeader*>(buffer + sizeof(svlBMPFileHeader));

    if (fileheader->bfType != 0x4D42 ||
        dibheader->biSize  != (sizeof(svlDIBHeader))) return SVL_FAIL;

    width = static_cast<unsigned int>(dibheader->biWidth);
    if (dibheader->biHeight >= 0) height = static_cast<unsigned int>(dibheader->biHeight);
    else height = static_cast<unsigned int>(-dibheader->biHeight);

    return SVL_OK;
}

int svlImageCodecBMP::Read(svlSampleImage &image, const unsigned int videoch, const std::string &filename, bool noresize)
{
    std::ifstream stream(filename.c_str(), std::ios_base::in | std::ios_base::binary);
    return Read(image, videoch, stream, noresize);
}

int svlImageCodecBMP::Read(svlSampleImage &image, const unsigned int videoch, std::istream &stream, bool noresize)
{
    if (videoch >= image.GetVideoChannels()) return SVL_FAIL;

    bool upsidedown;
    unsigned char *buffer;
    int width, height, linesize, padding, stride, i;

    if (stream.read(reinterpret_cast<char*>(&FileHeader), sizeof(svlBMPFileHeader)).fail() ||
        FileHeader.bfType != 0x4D42) return SVL_FAIL;
    if (stream.read(reinterpret_cast<char*>(&DIBHeader), sizeof(svlDIBHeader)).fail() ||
        DIBHeader.biSize != (sizeof(svlDIBHeader)) ||
        DIBHeader.biBitCount != 24 ||
        DIBHeader.biCompression != 0 ||
        DIBHeader.biClrUsed != 0) return SVL_FAIL;

    upsidedown = true;
    width = DIBHeader.biWidth;
    height = DIBHeader.biHeight;
    if (height < 0) {
        height = -height;
        upsidedown = false;
    }
    if (width  < 1 || width  > MAX_DIMENSION ||
        height < 1 || height > MAX_DIMENSION) return SVL_FAIL;

    if (static_cast<unsigned int>(width)  != image.GetWidth(videoch) ||
        static_cast<unsigned int>(height) != image.GetHeight(videoch)) {
        if (noresize) return SVL_FAIL;
        image.SetSize(videoch, static_cast<unsigned int>(width), static_cast<unsigned int>(height));
    }

    linesize = width * 3;
    padding = (4 - (linesize % 4)) % 4;
    stride = linesize + padding;
    buffer = image.GetUCharPointer(videoch);

    // Read data
    if (upsidedown) {
        buffer += linesize * (height - 1);
        for (i = 0; i < height; i ++) {
            if (stream.read(reinterpret_cast<char*>(buffer), linesize).fail() ||
                (padding > 0 && stream.seekg(padding, std::ios_base::cur).fail())) return SVL_FAIL;
            buffer -= linesize;
        }
    }
    else {
        if (padding == 0) {
            if (stream.read(reinterpret_cast<char*>(buffer), linesize * height).fail()) return SVL_FAIL;
        }
        else {
            for (i = 0; i < height; i ++) {
                if (stream.read(reinterpret_cast<char*>(buffer), linesize).fail() ||
                    stream.seekg(padding, std::ios_base::cur).fail()) return SVL_FAIL;
                buffer += linesize;
            }
        }
    }

    return SVL_OK;
}

int svlImageCodecBMP::Read(svlSampleImage &image, const unsigned int videoch, const unsigned char *buffer, const size_t buffersize, bool noresize)
{
    if (videoch >= image.GetVideoChannels()) return SVL_FAIL;

    if (buffer == 0) return SVL_FAIL;
    unsigned int dataoffset = sizeof(svlBMPFileHeader) + sizeof(svlDIBHeader);
    if (buffersize < dataoffset) return SVL_FAIL;

    const svlBMPFileHeader *fileheader = reinterpret_cast<const svlBMPFileHeader*>(buffer);
    const svlDIBHeader *dibheader = reinterpret_cast<const svlDIBHeader*>(buffer + sizeof(svlBMPFileHeader));

    bool upsidedown;
    unsigned char *imagebuf;
    int width, height, linesize, padding, stride, i;

    if (fileheader->bfType != 0x4D42 ||
        dibheader->biSize  != sizeof(svlDIBHeader) ||
        dibheader->biBitCount != 24 ||
        dibheader->biCompression != 0 ||
        dibheader->biClrUsed != 0)  return SVL_FAIL;

    upsidedown = true;
    width = dibheader->biWidth;
    height = dibheader->biHeight;
    if (height < 0) {
        height = -height;
        upsidedown = false;
    }
    if (width  < 1 || width  > MAX_DIMENSION ||
        height < 1 || height > MAX_DIMENSION) return SVL_FAIL;

    if (static_cast<unsigned int>(width)  != image.GetWidth(videoch) ||
        static_cast<unsigned int>(height) != image.GetHeight(videoch)) {
        if (noresize) return SVL_FAIL;
        image.SetSize(videoch, static_cast<unsigned int>(width), static_cast<unsigned int>(height));
    }

    linesize = width * 3;
    padding = (4 - (linesize % 4)) % 4;
    stride = linesize + padding;
    if (buffersize < (dataoffset + stride * height)) return SVL_FAIL;

    imagebuf = image.GetUCharPointer(videoch);

    // Copy data
    if (upsidedown) {
        imagebuf += linesize * (height - 1);
        for (i = 0; i < height; i ++) {
            memcpy(imagebuf, buffer + dataoffset, linesize);
            imagebuf -= linesize;
            dataoffset += stride;
        }
    }
    else {
        if (padding == 0) {
            memcpy(imagebuf, buffer, linesize * height);
        }
        else {
            for (i = 0; i < height; i ++) {
                memcpy(imagebuf, buffer + dataoffset, linesize);
                imagebuf += linesize;
                dataoffset += stride;
            }
        }
    }

    return SVL_OK;
}

int svlImageCodecBMP::Write(const svlSampleImage &image, const unsigned int videoch, const std::string &filename, const int compression)
{
    std::ofstream stream(filename.c_str(), std::ios_base::out | std::ios_base::binary);
    return Write(image, videoch, stream, compression);
}

int svlImageCodecBMP::Write(const svlSampleImage &image, const unsigned int videoch, std::ostream &stream, const int CMN_UNUSED(compression))
{
    if (videoch >= image.GetVideoChannels()) return SVL_FAIL;

    const unsigned char *imagebuf = image.GetUCharPointer(videoch);
    unsigned int width, height, linesize, stride, padding, pad = 0, sourceoffset = 0;
    const char *ppad = reinterpret_cast<char*>(&pad);

    width = image.GetWidth(videoch);
    height = image.GetHeight(videoch);

    if (image.GetBPP() != 3 ||
        width  < 1 || width  > MAX_DIMENSION ||
        height < 1 || height > MAX_DIMENSION) return SVL_FAIL;

    linesize = width * 3;
    padding = (4 - (linesize % 4)) % 4;
    stride = linesize + padding;

    // Build headers
	DIBHeader.biSize = sizeof(svlDIBHeader);
    DIBHeader.biWidth = width;
    DIBHeader.biHeight = -1 * height;
	DIBHeader.biPlanes = 1;
	DIBHeader.biBitCount = 24;
	DIBHeader.biCompression = 0;
	DIBHeader.biSizeImage = 0;
	DIBHeader.biXPelsPerMeter = 0;
	DIBHeader.biYPelsPerMeter = 0;
	DIBHeader.biClrUsed = 0;
	DIBHeader.biClrImportant = 0;
    FileHeader.bfType = 0x4D42;
    FileHeader.bfReserved1 = 0;
    FileHeader.bfReserved2 = 0;
    FileHeader.bfOffBits = sizeof(svlBMPFileHeader) + sizeof(svlDIBHeader);
    FileHeader.bfSize = FileHeader.bfOffBits + stride * height;

    // Write headers
    if (stream.write(reinterpret_cast<char*>(&FileHeader), sizeof(svlBMPFileHeader)).fail() ||
        stream.write(reinterpret_cast<char*>(&DIBHeader), sizeof(svlDIBHeader)).fail()) return SVL_FAIL;

    // Write data
    if (padding) {
        for (unsigned int i = 0; i < height; i ++) {
            if (stream.write(reinterpret_cast<const char*>(imagebuf + sourceoffset), linesize).fail() ||
                stream.write(ppad, padding).fail()) return SVL_FAIL;
            sourceoffset += linesize;
        }
    }
    else {
        if (stream.write(reinterpret_cast<const char*>(imagebuf), image.GetDataSize(videoch)).fail()) return SVL_FAIL;
    }

    return SVL_OK;
}

int svlImageCodecBMP::Write(const svlSampleImage &image, const unsigned int videoch, unsigned char *buffer, size_t &buffersize, const int CMN_UNUSED(compression))
{
    if (videoch >= image.GetVideoChannels()) return SVL_FAIL;
    if (buffer == 0) return SVL_FAIL;

    const unsigned char *imagebuf = image.GetUCharPointer(videoch);
    unsigned int width, height, linesize, stride, padding, size;
    const unsigned int fileheadsize = sizeof(svlBMPFileHeader);
    const unsigned int dibheadsize = sizeof(svlDIBHeader);
    svlBMPFileHeader *fileheader;
    svlDIBHeader *dibheader;

    width = image.GetWidth(videoch);
    height = image.GetHeight(videoch);

    if (image.GetBPP() != 3 ||
        width  < 1 || width  > MAX_DIMENSION ||
        height < 1 || height > MAX_DIMENSION) return SVL_FAIL;

    linesize = width * 3;
    padding = (4 - (linesize % 4)) % 4;
    stride = linesize + padding;

    size = fileheadsize + dibheadsize + stride * height;
    if (buffersize < size) {
        // Let caller know what size of memory is needed
        buffersize = size;
        return SVL_FAIL;
    }
    // Return buffer size in 'buffersize'
    buffersize = size;

    fileheader = reinterpret_cast<svlBMPFileHeader*>(buffer);
    dibheader = reinterpret_cast<svlDIBHeader*>(buffer + fileheadsize);
    buffer += fileheadsize + dibheadsize;

    // Build headers right in place
	dibheader->biSize = dibheadsize;
    dibheader->biWidth = width;
    dibheader->biHeight = -1 * height;
	dibheader->biPlanes = 1;
	dibheader->biBitCount = 24;
	dibheader->biCompression = 0;
	dibheader->biSizeImage = 0;
	dibheader->biXPelsPerMeter = 0;
	dibheader->biYPelsPerMeter = 0;
	dibheader->biClrUsed = 0;
	dibheader->biClrImportant = 0;
    fileheader->bfType = 0x4D42;
    fileheader->bfReserved1 = 0;
    fileheader->bfReserved2 = 0;
    fileheader->bfOffBits = fileheadsize + dibheadsize;
    fileheader->bfSize = fileheader->bfOffBits + stride * height;

    // Copy data
    if (padding == 0) {
        memcpy(buffer, imagebuf, image.GetDataSize(videoch));
    }
    else {
        unsigned int sourceoffset = 0, pad = 0;
        const char *ppad = reinterpret_cast<char*>(&pad);

        for (unsigned int i = 0; i < height; i ++) {
            memcpy(buffer, imagebuf + sourceoffset, linesize);
            sourceoffset += linesize;
            buffer += linesize;
            memcpy(buffer, ppad, padding);
            buffer += padding;
        }
    }

    return SVL_OK;
}

