/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlImageIO.cpp 618 2009-07-31 16:39:42Z bvagvol1 $
  
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

#include <cisstStereoVision/svlImageIO.h>
#include "ftInitializer.h"


svlImageIO::svlImageIO()
{
    int handlers = 0;
    svlImageCodec* ft;
    cmnGenericObject* go;

    Codecs.SetSize(256);
    Extensions.SetSize(256);

    // Go through all registered classes
    for (cmnClassRegister::const_iterator iter = cmnClassRegister::begin(); iter != cmnClassRegister::end(); iter ++) {
        if ((*iter).second && handlers < 256) {
            go = (*iter).second->Create();
            ft = dynamic_cast<svlImageCodec*>(go);
            if (ft) {
                Codecs[handlers]     = (*iter).second;
                Extensions[handlers] = ft->GetExtensions();
                handlers ++;
            }
            (*iter).second->Delete(go);
        }
    }

    Codecs.resize(handlers);
    Extensions.resize(handlers);
}

int svlImageIO::GetExtension(const std::string &filename,
                             std::string &extension)
{
    size_t dotpos = filename.find_last_of('.');
    // If no '.' is found, then take the whole string as extension
    if (dotpos != std::string::npos) {
        extension = filename.substr(dotpos + 1);
    }
    else {
        extension = filename;
    }

    char ch;
    const int offset = 'a' - 'A';
    const unsigned int len = extension.size();
    if (len == 0) return SVL_FAIL;

    for (unsigned int i = 0; i < len; i ++) {
        ch = extension[i];
        if (ch >= 'A' && ch <= 'Z') {
            extension[i] = ch + offset;
        }
        else {
            extension[i] = ch;
        }
    }

    return SVL_OK;
}

svlImageCodec* svlImageIO::GetCodec(const std::string &filename)
{
    // Extension list format:
    //      ('.' + [a..z0..9]* + ';')*

    std::string extension;
    if (GetExtension(filename, extension) != SVL_OK) return 0;
    extension.insert(0, ".");
    extension.append(";");

    static svlImageIO Instance;
    const unsigned int size = Instance.Codecs.size();

    for (unsigned int i = 0; i < size; i ++) {
        if (Instance.Extensions[i].find(extension) != std::string::npos) {
            return dynamic_cast<svlImageCodec*>(Instance.Codecs[i]->Create());
        }
    }

    return 0;
}

int svlImageIO::ReadDimensions(const std::string &filename,
                               unsigned int &width,
                               unsigned int &height)
{
    svlImageCodec* decoder = GetCodec(filename);
    if (decoder == 0) return SVL_FAIL;

    int ret = decoder->ReadDimensions(filename, width, height);
    delete decoder;

    return ret;
}

int svlImageIO::ReadDimensions(const std::string &codec,
                               std::istream &stream,
                               unsigned int &width,
                               unsigned int &height)
{
    svlImageCodec* decoder = GetCodec("." + codec);
    if (decoder == 0) return SVL_FAIL;

    // Get current position
    std::streampos startpos = stream.tellg();

    int ret = decoder->ReadDimensions(stream, width, height);
    delete decoder;

    // Seek back to the initial position to leave the stream intact
    stream.seekg(startpos);

    return ret;
}

int svlImageIO::ReadDimensions(const std::string &codec,
                               const unsigned char *buffer,
                               const size_t buffersize,
                               unsigned int &width,
                               unsigned int &height)
{
    svlImageCodec* decoder = GetCodec("." + codec);
    if (decoder == 0) return SVL_FAIL;

    int ret = decoder->ReadDimensions(buffer, buffersize, width, height);
    delete decoder;

    return ret;
}

int svlImageIO::Read(svlSampleImageBase &image,
                     const unsigned int videoch,
                     const std::string &filename,
                     bool noresize)
{
    svlImageCodec* decoder = GetCodec(filename);
    if (decoder == 0) return SVL_FAIL;

    int ret = decoder->Read(image, videoch, filename, noresize);
    delete decoder;

    return ret;
}

int svlImageIO::Read(svlSampleImageBase &image,
                     const unsigned int videoch,
                     const std::string &codec,
                     std::istream &stream,
                     bool noresize)
{
    svlImageCodec* decoder = GetCodec("." + codec);
    if (decoder == 0) return SVL_FAIL;

    // Get current position
    std::streampos startpos = stream.tellg();

    int ret = decoder->Read(image, videoch, stream, noresize);
    delete decoder;

    if (ret != SVL_OK) {
        // In case of error:
        //   Seek back to the initial position to leave the stream intact
        stream.seekg(startpos);
    }

    return ret;
}

int svlImageIO::Read(svlSampleImageBase &image,
                     const unsigned int videoch,
                     const std::string &codec,
                     const unsigned char *buffer,
                     const size_t buffersize,
                     bool noresize)
{
    svlImageCodec* decoder = GetCodec("." + codec);
    if (decoder == 0) return SVL_FAIL;

    int ret = decoder->Read(image, videoch, buffer, buffersize, noresize);
    delete decoder;

    return ret;
}

int svlImageIO::Write(const svlSampleImageBase &image,
                      const unsigned int videoch,
                      const std::string &filename,
                      const int compression)
{
    svlImageCodec* encoder = GetCodec(filename);
    if (encoder == 0) return SVL_FAIL;

    int ret = encoder->Write(image, videoch, filename, compression);
    delete encoder;

    return ret;
}

int svlImageIO::Write(const svlSampleImageBase &image,
                      const unsigned int videoch,
                      const std::string &codec,
                      std::ostream &stream,
                      const int compression)
{
    svlImageCodec* encoder = GetCodec("." + codec);
    if (encoder == 0) return SVL_FAIL;

    // Generate lowercase codec string
    std::string lwrcodec;
    GetExtension(codec, lwrcodec);

    int ret = encoder->Write(image, videoch, stream, lwrcodec, compression);
    delete encoder;

    return ret;
}

int svlImageIO::Write(const svlSampleImageBase &image,
                      const unsigned int videoch,
                      const std::string &codec,
                      unsigned char *buffer,
                      size_t &buffersize,
                      const int compression)
{
    svlImageCodec* encoder = GetCodec("." + codec);
    if (encoder == 0) return SVL_FAIL;

    // Generate lowercase codec string
    std::string lwrcodec;
    GetExtension(codec, lwrcodec);

    int ret = encoder->Write(image, videoch, buffer, buffersize, lwrcodec, compression);
    delete encoder;

    return ret;
}


/**********************************/
/*** svlImageCodec class **********/
/**********************************/

svlImageCodec::svlImageCodec()
{
}

svlImageCodec::~svlImageCodec()
{
}

const std::string& svlImageCodec::GetExtensions() const
{
    return ExtensionList;
}

int svlImageCodec::ReadDimensions(const std::string & CMN_UNUSED(filename),
                                  unsigned int & CMN_UNUSED(width),
                                  unsigned int & CMN_UNUSED(height))
{
    return SVL_FAIL;
}

int svlImageCodec::ReadDimensions(std::istream & CMN_UNUSED(stream),
                                  unsigned int & CMN_UNUSED(width),
                                  unsigned int & CMN_UNUSED(height))
{
    return SVL_FAIL;
}

int svlImageCodec::ReadDimensions(const unsigned char * CMN_UNUSED(buffer),
                                  const size_t CMN_UNUSED(buffersize),
                                  unsigned int & CMN_UNUSED(width),
                                  unsigned int & CMN_UNUSED(height))
{
    return SVL_FAIL;
}

int svlImageCodec::Read(svlSampleImageBase & CMN_UNUSED(image),
                        const unsigned int CMN_UNUSED(videoch),
                        const std::string & CMN_UNUSED(filename),
                        bool CMN_UNUSED(noresize))
{
    return SVL_FAIL;
}

int svlImageCodec::Read(svlSampleImageBase & CMN_UNUSED(image),
                        const unsigned int CMN_UNUSED(videoch),
                        std::istream & CMN_UNUSED(stream),
                        bool CMN_UNUSED(noresize))
{
    return SVL_FAIL;
}

int svlImageCodec::Read(svlSampleImageBase & CMN_UNUSED(image),
                        const unsigned int CMN_UNUSED(videoch),
                        const unsigned char * CMN_UNUSED(buffer),
                        const size_t CMN_UNUSED(buffersize),
                        bool CMN_UNUSED(noresize))
{
    return SVL_FAIL;
}

int svlImageCodec::Write(const svlSampleImageBase & CMN_UNUSED(image),
                         const unsigned int CMN_UNUSED(videoch),
                         const std::string & CMN_UNUSED(filename),
                         const int CMN_UNUSED(compression))
{
    return SVL_FAIL;
}

int svlImageCodec::Write(const svlSampleImageBase & CMN_UNUSED(image),
                         const unsigned int CMN_UNUSED(videoch),
                         std::ostream & CMN_UNUSED(stream),
                         const int CMN_UNUSED(compression))
{
    return SVL_FAIL;
}

int svlImageCodec::Write(const svlSampleImageBase & image,
                         const unsigned int videoch,
                         std::ostream & stream,
                         const std::string & CMN_UNUSED(codec),
                         const int compression)
{
    return Write(image, videoch, stream, compression);
}

int svlImageCodec::Write(const svlSampleImageBase & CMN_UNUSED(image),
                         const unsigned int CMN_UNUSED(videoch),
                         unsigned char * CMN_UNUSED(buffer),
                         size_t & CMN_UNUSED(buffersize),
                         const int CMN_UNUSED(compression))
{
    return SVL_FAIL;
}
int svlImageCodec::Write(const svlSampleImageBase & image,
                         const unsigned int videoch,
                         unsigned char * buffer,
                         size_t & buffersize,
                         const std::string & CMN_UNUSED(codec),
                         const int compression)
{
    return Write(image, videoch, buffer, buffersize, compression);
}

