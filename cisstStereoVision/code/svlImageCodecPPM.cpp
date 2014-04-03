/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Ben Mitchell
  Created on: 2006 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

Original code courtesy of Ben Mitchell.
*/

#include "svlImageCodecPPM.h"
#include <cisstStereoVision/svlTypes.h>


/*************************************/
/*** PPM/PGM handler functions *******/
/*************************************/

int ppmOpen(std::istream &stream, unsigned int &width, unsigned int &height)
{
    // Only 24bpp PPMs and 8bpp PGMs are supported

    std::string magicword;
    unsigned int colors;
    char ch;

    for (int i = 0; i < 4; i ++) {

        ch = stream.peek();
        while (ch == '\n' || ch == ' ' || ch == '#') {
            if (ch == '#') stream.ignore(1024, '\n');
            else stream.get();
            ch = stream.peek();
        }

        if (stream.fail()) return -1;

        switch (i) {
            case 0: // first real block of data = magic number
                stream >> magicword;
            break;

            case 1: // second is width
                stream >> width;
            break;

            case 2: // third is height
                stream >> height;
            break;

            case 3: // fourth is the max. allowed value of a color 
                stream >> colors;
            break;
        }
    }

    if (stream.fail() ||
        colors != 255 ||
        (magicword != "P5" && magicword != "P6")) return -1;

    ch = stream.peek();
    while (ch == '\n' || ch == ' ' || ch == '#') {
        if (ch == '#') stream.ignore(1024, '\n');
        else stream.get();
        ch = stream.peek();
    }

    if (magicword == "P5") {
        return 5;
    }

    return 6;
}

int ppmOpen(const unsigned char *source, const size_t sourcesize, unsigned int &width, unsigned int &height)
{
    if (!source) return SVL_FAIL;

    // The header shall be shorter than 256 bytes
    std::string str(reinterpret_cast<const char*>(source), std::min(static_cast<unsigned int>(sourcesize), 256u));
    std::istringstream stream(str);

    return ppmOpen(stream, width, height);
}

int ppmRead(std::istream &stream, unsigned char* buffer, unsigned int pixelcount, int magicnumber)
{
    if (!buffer) return SVL_FAIL;

    unsigned int i;
    unsigned char uch;

    if (magicnumber == 5) {
        unsigned char *source = buffer + (pixelcount << 1);

        if (stream.read(reinterpret_cast<char*>(source), pixelcount).fail()) return SVL_FAIL;

        for (i = 0; i < pixelcount; i ++) {
            uch = *source; source ++;
            *buffer = uch; buffer ++;
            *buffer = uch; buffer ++;
            *buffer = uch; buffer ++;
        }
    }
    else {
        if (stream.read(reinterpret_cast<char*>(buffer), pixelcount * 3).fail()) return SVL_FAIL;

        // BGR >> RGB
        for (i = 0; i < pixelcount; i++) {
            uch = buffer[0];
            buffer[0] = buffer[2];
            buffer[2] = uch;
            buffer += 3;
        }
    }

    return SVL_OK;
}

int ppmRead(const unsigned char *source, const size_t sourcesize, unsigned char* buffer, unsigned int pixelcount, int magicnumber)
{
    if (!buffer || !source || sourcesize < pixelcount) return SVL_FAIL;

    unsigned int i, j;
    unsigned char uch;

    if (magicnumber == 5) {
        for (i = 0; i < pixelcount; i ++) {
            uch = source[i];
            *buffer = uch; buffer ++;
            *buffer = uch; buffer ++;
            *buffer = uch; buffer ++;
        }
    }
    else {
        // BGR >> RGB
        j = 0;
        for (i = 0; i < pixelcount; i++) {
            buffer[2] = source[j]; j ++;
            buffer[1] = source[j]; j ++;
            buffer[0] = source[j]; j ++;
            buffer += 3;
        }
    }

    return SVL_OK;
}

int ppmWrite(const unsigned char* buffer, unsigned char* workbuffer, const unsigned int width, const unsigned int height, const int magicnumber, std::ostream &stream)
{
    if (!buffer || !workbuffer) return SVL_FAIL;

    const unsigned int pixelcount = width * height;
    unsigned char *target = workbuffer;
    unsigned int i, j = 0, sum;

    if (magicnumber == 5) {
        for (i = 0; i < pixelcount; i ++) {
            sum  = buffer[j]; j ++;
            sum += buffer[j]; j ++;
            sum += buffer[j]; j ++;
            *target = sum / 3; target ++;
        }
    }
    else {
        // RGB >> BGR
        for (i = 0; i < pixelcount; i++) {
            target[2] = buffer[j]; j ++;
            target[1] = buffer[j]; j ++;
            target[0] = buffer[j]; j ++;
            target += 3;
        }
    }

    stream << "P" << magicnumber << "\n"
           << width << "\n"
           << height << "\n"
           << 255 << "\n";
    if (stream.fail()) return SVL_FAIL;

    if (magicnumber == 5) {
        if (stream.write(reinterpret_cast<char*>(workbuffer), pixelcount).fail()) return SVL_FAIL;
    }
    else {
        if (stream.write(reinterpret_cast<char*>(workbuffer), pixelcount * 3).fail()) return SVL_FAIL;
    }

    return SVL_OK;
}

int ppmWrite(const unsigned char* buffer, const unsigned int width, const unsigned int height, const int magicnumber, unsigned char *target, size_t &targetsize)
{
    if (!buffer || !target) return SVL_FAIL;

    const unsigned int pixelcount = width * height;
    unsigned int headersize, size = pixelcount;
    unsigned int i, j = 0, sum;
    std::ostringstream stream;

    // Build header in string stream
    stream << "P" << magicnumber << "\n"
           << width << "\n"
           << height << "\n"
           << 255 << "\n";
    headersize = static_cast<unsigned int>(stream.str().size());

    if (magicnumber == 6) size *= 3;

    if (targetsize < (headersize + size)) {
        // Let caller know what size of memory is needed
        targetsize = headersize + size;
        return SVL_FAIL;
    }
    // Return buffer size in 'targetsize'
    targetsize = headersize + size;

    // Store header
    memcpy(target, stream.str().c_str(), headersize);
    target += headersize;

    // Store pixel data
    if (magicnumber == 5) {
        for (i = 0; i < pixelcount; i ++) {
            sum  = buffer[j]; j ++;
            sum += buffer[j]; j ++;
            sum += buffer[j]; j ++;
            *target = sum / 3; target ++;
        }
    }
    else {
        // RGB >> BGR
        for (i = 0; i < pixelcount; i++) {
            target[2] = buffer[j]; j ++;
            target[1] = buffer[j]; j ++;
            target[0] = buffer[j]; j ++;
            target += 3;
        }
    }

    return SVL_OK;
}


/*************************************/
/*** svlImageCodecPPM class **********/
/*************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlImageCodecPPM, svlImageCodecBase)

svlImageCodecPPM::svlImageCodecPPM() :
    svlImageCodecBase(),
    ppmBuffer(0),
    ppmBufferSize(0)
{
    SetExtensionList(".ppm;.pgm;");
}

svlImageCodecPPM::~svlImageCodecPPM()
{
    if (ppmBuffer) delete [] ppmBuffer;
}

int svlImageCodecPPM::ReadDimensions(const std::string &filename, unsigned int &width, unsigned int &height)
{
    std::ifstream stream(filename.c_str(), std::ios_base::in | std::ios_base::binary);
    return ReadDimensions(stream, width, height);
}

int svlImageCodecPPM::ReadDimensions(std::istream &stream, unsigned int &width, unsigned int &height)
{
    int magicnumber = ppmOpen(stream, width, height);
    if (magicnumber < 0) return SVL_FAIL;
    return SVL_OK;
}

int svlImageCodecPPM::ReadDimensions(const unsigned char *buffer, const size_t buffersize, unsigned int &width, unsigned int &height)
{
    int magicnumber = ppmOpen(buffer, buffersize, width, height);
    if (magicnumber < 0) return SVL_FAIL;
    return SVL_OK;
}

int svlImageCodecPPM::Read(svlSampleImage &image, const unsigned int videoch, const std::string &filename, bool noresize)
{
    std::ifstream stream(filename.c_str(), std::ios_base::in | std::ios_base::binary);
    return Read(image, videoch, stream, noresize);
}

int svlImageCodecPPM::Read(svlSampleImage &image, const unsigned int videoch, std::istream &stream, bool noresize)
{
    if (videoch >= image.GetVideoChannels()) return SVL_FAIL;

    unsigned int width, height;

    int magicnumber = ppmOpen(stream, width, height);
    if (magicnumber < 0) return SVL_FAIL;

    if (width  < 1 || width  > MAX_DIMENSION ||
        height < 1 || height > MAX_DIMENSION) return SVL_FAIL;

    if (width  != image.GetWidth(videoch) ||
        height != image.GetHeight(videoch)) {
        if (noresize) return SVL_FAIL;
        image.SetSize(videoch, width, height);
    }

    return ppmRead(stream, image.GetUCharPointer(videoch), width * height, magicnumber);
}

int svlImageCodecPPM::Read(svlSampleImage &image, const unsigned int videoch, const unsigned char *buffer, const size_t buffersize, bool noresize)
{
    if (videoch >= image.GetVideoChannels()) return SVL_FAIL;

    unsigned int width, height;

    int magicnumber = ppmOpen(buffer, buffersize, width, height);
    if (magicnumber < 0) return SVL_FAIL;

    if (width  < 1 || width  > MAX_DIMENSION ||
        height < 1 || height > MAX_DIMENSION) return SVL_FAIL;

    if (width  != image.GetWidth(videoch) ||
        height != image.GetHeight(videoch)) {
        if (noresize) return SVL_FAIL;
        image.SetSize(videoch, width, height);
    }

    return ppmRead(buffer, buffersize, image.GetUCharPointer(videoch), width * height, magicnumber);
}

int svlImageCodecPPM::Write(const svlSampleImage &image, const unsigned int videoch, const std::string &filename, const int compression)
{
    std::ofstream stream(filename.c_str(), std::ios_base::out | std::ios_base::binary);

    // Get codec (lowercase extension)
    std::string codec;
    svlImageIO::GetExtension(filename, codec);

    return Write(image, videoch, stream, codec, compression);
}

int svlImageCodecPPM::Write(const svlSampleImage &image, const unsigned int videoch, std::ostream &stream, const int compression)
{
    return Write(image, videoch, stream, "ppm", compression);
}

int svlImageCodecPPM::Write(const svlSampleImage &image, const unsigned int videoch, std::ostream &stream, const std::string &codec, const int CMN_UNUSED(compression))
{
    if (videoch >= image.GetVideoChannels()) return SVL_FAIL;
  
    const unsigned char *imagebuf = image.GetUCharPointer(videoch);
    unsigned int width, height, size;
    int magicnumber;

    width = image.GetWidth(videoch);
    height = image.GetHeight(videoch);

    if (image.GetBPP() != 3 ||
        width  < 1 || width  > MAX_DIMENSION ||
        height < 1 || height > MAX_DIMENSION) return SVL_FAIL;

    if (codec == "pgm") {
        magicnumber = 5;
        size = width * height;
    }
    else {
        magicnumber = 6;
        size = width * height * 3;
    }

    // Prepare work buffer
    if (!ppmBuffer) {
        ppmBuffer = new unsigned char[size];
        ppmBufferSize = size;
    }
    else if (ppmBuffer && ppmBufferSize < size) {
        delete [] ppmBuffer;
        ppmBuffer = new unsigned char[size];
        ppmBufferSize = size;
    }

    return ppmWrite(imagebuf, ppmBuffer, width, height, magicnumber, stream);
}

int svlImageCodecPPM::Write(const svlSampleImage &image, const unsigned int videoch, unsigned char *buffer, size_t &buffersize, const int compression)
{
    return Write(image, videoch, buffer, buffersize, "ppm", compression);
}

int svlImageCodecPPM::Write(const svlSampleImage &image, const unsigned int videoch, unsigned char *buffer, size_t &buffersize, const std::string &codec, const int CMN_UNUSED(compression))
{
    if (videoch >= image.GetVideoChannels()) return SVL_FAIL;
  
    const unsigned char *imagebuf = image.GetUCharPointer(videoch);
    unsigned int width, height;

    width = image.GetWidth(videoch);
    height = image.GetHeight(videoch);

    if (image.GetBPP() != 3 ||
        width  < 1 || width  > MAX_DIMENSION ||
        height < 1 || height > MAX_DIMENSION) return SVL_FAIL;

    int magicnumber = 6;
    if (codec == "pgm") magicnumber = 5;

    return ppmWrite(imagebuf, width, height, magicnumber, buffer, buffersize);
}

