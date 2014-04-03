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

#include <cisstStereoVision/svlDefinitions.h>
#include <cisstStereoVision/svlImageIO.h>
#include "svlImageCodecInitializer.h"


const static int CACHE_SIZE_STEP = 5;

/*******************************/
/*** svlImageIO class **********/
/*******************************/

svlImageIO::svlImageIO()
{
    int handlers = 0;
    svlImageCodecBase* ft;
    cmnGenericObject* go;

    Codecs.SetSize(256);
    Extensions.SetSize(256);

    // Go through all registered classes
    for (cmnClassRegister::const_iterator iter = cmnClassRegister::begin(); iter != cmnClassRegister::end(); iter ++) {
        if ((*iter).second && (handlers < 256) && (*iter).second->IsDerivedFrom<svlImageCodecBase>()) {
            go = (*iter).second->Create();
            ft = dynamic_cast<svlImageCodecBase*>(go);
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
    CodecCache.SetSize(handlers);
    CodecCacheUsed.SetSize(handlers);
}

svlImageIO* svlImageIO::GetInstance()
{
    static svlImageIO Instance;
    return &Instance;
}

svlImageIO::~svlImageIO()
{
    const unsigned int size = static_cast<unsigned int>(CodecCache.size());
    unsigned int i, j, cachesize;
    for (i = 0; i < size; i ++) {
        cachesize = static_cast<unsigned int>(CodecCache[i].size());
        for (j = 0; j < cachesize; j ++) {
            delete CodecCache[i][j];
        }
    }
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
    const unsigned int len = static_cast<unsigned int>(extension.size());
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

svlImageCodecBase* svlImageIO::GetCodec(const std::string &filename)
{
    // Extension list format:
    //      ('.' + [a..z0..9]* + ';')*

    std::string extension;
    if (GetExtension(filename, extension) != SVL_OK) return 0;
    extension.insert(0, ".");
    extension.append(";");

    svlImageIO* instance = GetInstance();
    const unsigned int size = static_cast<unsigned int>(instance->Codecs.size());
    unsigned int i;
    int j, cachesize, cacheitem;

    for (i = 0; i < size; i ++) {
        if (instance->Extensions[i].find(extension) != std::string::npos) {

            ///////////////////////////
            // Enter critical section
            instance->CS.Enter();

            // check if we have any unused image handlers in the cache
            cacheitem = 0;
            cachesize = static_cast<int>(instance->CodecCacheUsed[i].size());
            while (cacheitem < cachesize && instance->CodecCacheUsed[i][cacheitem]) cacheitem ++;

            // if there is no unused image handler in the
            // cache, then increase the size of cache
            if (cacheitem >= cachesize) {
                cacheitem = cachesize;
                cachesize += CACHE_SIZE_STEP;
                instance->CodecCache[i].resize(cachesize);
                instance->CodecCacheUsed[i].resize(cachesize);
                for (j = cacheitem; j < cachesize; j ++) {
                    instance->CodecCache[i][j] = dynamic_cast<svlImageCodecBase*>(instance->Codecs[i]->Create());
                    instance->CodecCacheUsed[i][j] = false;
                }
            }

            instance->CodecCacheUsed[i][cacheitem] = true;

            instance->CS.Leave();
            // Leave critical section
            ///////////////////////////

            return instance->CodecCache[i][cacheitem];
        }
    }

    return 0;
}

void svlImageIO::ReleaseCodec(svlImageCodecBase* codec)
{
    if (!codec) return;

    svlImageIO* instance = GetInstance();

    ///////////////////////////
    // Enter critical section
    instance->CS.Enter();

    const unsigned int size = static_cast<unsigned int>(instance->CodecCache.size());
    unsigned int i, j, cachesize;
    for (i = 0; i < size; i ++) {
        cachesize = static_cast<unsigned int>(instance->CodecCache[i].size());
        for (j = 0; j < cachesize; j ++) {
            if (codec == instance->CodecCache[i][j]) {
                instance->CodecCacheUsed[i][j] = false;

                instance->CS.Leave();
                // Leave critical section
                ///////////////////////////

                return;
            }
        }
    }

    instance->CS.Leave();
    // Leave critical section
    ///////////////////////////
}

int svlImageIO::ReadDimensions(const std::string &filename,
                               unsigned int &width,
                               unsigned int &height)
{
    svlImageCodecBase* decoder = GetCodec(filename);
    if (decoder == 0) return SVL_FAIL;

    int ret = decoder->ReadDimensions(filename, width, height);

    ReleaseCodec(decoder);

    return ret;
}

int svlImageIO::ReadDimensions(const std::string &codec,
                               std::istream &stream,
                               unsigned int &width,
                               unsigned int &height)
{
    svlImageCodecBase* decoder = GetCodec("." + codec);
    if (decoder == 0) return SVL_FAIL;

    // Get current position
    std::streampos startpos = stream.tellg();

    int ret = decoder->ReadDimensions(stream, width, height);

    ReleaseCodec(decoder);

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
    svlImageCodecBase* decoder = GetCodec("." + codec);
    if (decoder == 0) return SVL_FAIL;

    int ret = decoder->ReadDimensions(buffer, buffersize, width, height);

    ReleaseCodec(decoder);

    return ret;
}

int svlImageIO::Read(svlSampleImage &image,
                     const unsigned int videoch,
                     const std::string &filename,
                     bool noresize)
{
    svlImageCodecBase* decoder = GetCodec(filename);
    if (decoder == 0) return SVL_FAIL;

    int ret = decoder->Read(image, videoch, filename, noresize);

    ReleaseCodec(decoder);

    return ret;
}

int svlImageIO::Read(svlSampleImage &image,
                     const unsigned int videoch,
                     const std::string &codec,
                     std::istream &stream,
                     bool noresize)
{
    svlImageCodecBase* decoder = GetCodec("." + codec);
    if (decoder == 0) return SVL_FAIL;

    // Get current position
    std::streampos startpos = stream.tellg();

    int ret = decoder->Read(image, videoch, stream, noresize);

    ReleaseCodec(decoder);

    if (ret != SVL_OK) {
        // In case of error:
        //   Seek back to the initial position to leave the stream intact
        stream.seekg(startpos);
    }

    return ret;
}

int svlImageIO::Read(svlSampleImage &image,
                     const unsigned int videoch,
                     const std::string &codec,
                     const unsigned char *buffer,
                     const size_t buffersize,
                     bool noresize)
{
    svlImageCodecBase* decoder = GetCodec("." + codec);
    if (decoder == 0) return SVL_FAIL;

    int ret = decoder->Read(image, videoch, buffer, buffersize, noresize);

    ReleaseCodec(decoder);

    return ret;
}

int svlImageIO::Write(const svlSampleImage &image,
                      const unsigned int videoch,
                      const std::string &filename,
                      const int compression)
{
    svlImageCodecBase* encoder = GetCodec(filename);
    if (encoder == 0) return SVL_FAIL;

    int ret = encoder->Write(image, videoch, filename, compression);

    ReleaseCodec(encoder);

    return ret;
}

int svlImageIO::Write(const svlSampleImage &image,
                      const unsigned int videoch,
                      const std::string &codec,
                      std::ostream &stream,
                      const int compression)
{
    svlImageCodecBase* encoder = GetCodec("." + codec);
    if (encoder == 0) return SVL_FAIL;

    // Generate lowercase codec string
    std::string lwrcodec;
    GetExtension(codec, lwrcodec);

    int ret = encoder->Write(image, videoch, stream, lwrcodec, compression);

    ReleaseCodec(encoder);

    return ret;
}

int svlImageIO::Write(const svlSampleImage &image,
                      const unsigned int videoch,
                      const std::string &codec,
                      unsigned char *buffer,
                      size_t &buffersize,
                      const int compression)
{
    svlImageCodecBase* encoder = GetCodec("." + codec);
    if (encoder == 0) return SVL_FAIL;

    // Generate lowercase codec string
    std::string lwrcodec;
    GetExtension(codec, lwrcodec);

    int ret = encoder->Write(image, videoch, buffer, buffersize, lwrcodec, compression);

    ReleaseCodec(encoder);

    return ret;
}


/**********************************/
/*** svlImageCodecBase class ******/
/**********************************/

CMN_IMPLEMENT_SERVICES(svlImageCodecBase)

svlImageCodecBase::svlImageCodecBase() : cmnGenericObject()
{
}

svlImageCodecBase::~svlImageCodecBase()
{
}

const std::string& svlImageCodecBase::GetExtensions() const
{
    return ExtensionList;
}

int svlImageCodecBase::ReadDimensions(const std::string & CMN_UNUSED(filename),
                                      unsigned int & CMN_UNUSED(width),
                                      unsigned int & CMN_UNUSED(height))
{
    return SVL_FAIL;
}

int svlImageCodecBase::ReadDimensions(std::istream & CMN_UNUSED(stream),
                                      unsigned int & CMN_UNUSED(width),
                                      unsigned int & CMN_UNUSED(height))
{
    return SVL_FAIL;
}

int svlImageCodecBase::ReadDimensions(const unsigned char * CMN_UNUSED(buffer),
                                      const size_t CMN_UNUSED(buffersize),
                                      unsigned int & CMN_UNUSED(width),
                                      unsigned int & CMN_UNUSED(height))
{
    return SVL_FAIL;
}

int svlImageCodecBase::Read(svlSampleImage & CMN_UNUSED(image),
                            const unsigned int CMN_UNUSED(videoch),
                            const std::string & CMN_UNUSED(filename),
                            bool CMN_UNUSED(noresize))
{
    return SVL_FAIL;
}

int svlImageCodecBase::Read(svlSampleImage & CMN_UNUSED(image),
                            const unsigned int CMN_UNUSED(videoch),
                            std::istream & CMN_UNUSED(stream),
                            bool CMN_UNUSED(noresize))
{
    return SVL_FAIL;
}

int svlImageCodecBase::Read(svlSampleImage & CMN_UNUSED(image),
                            const unsigned int CMN_UNUSED(videoch),
                            const unsigned char * CMN_UNUSED(buffer),
                            const size_t CMN_UNUSED(buffersize),
                            bool CMN_UNUSED(noresize))
{
    return SVL_FAIL;
}

int svlImageCodecBase::Write(const svlSampleImage & CMN_UNUSED(image),
                             const unsigned int CMN_UNUSED(videoch),
                             const std::string & CMN_UNUSED(filename),
                             const int CMN_UNUSED(compression))
{
    return SVL_FAIL;
}

int svlImageCodecBase::Write(const svlSampleImage & CMN_UNUSED(image),
                             const unsigned int CMN_UNUSED(videoch),
                             std::ostream & CMN_UNUSED(stream),
                             const int CMN_UNUSED(compression))
{
    return SVL_FAIL;
}

int svlImageCodecBase::Write(const svlSampleImage & image,
                             const unsigned int videoch,
                             std::ostream & stream,
                             const std::string & CMN_UNUSED(codec),
                             const int compression)
{
    return Write(image, videoch, stream, compression);
}

int svlImageCodecBase::Write(const svlSampleImage & CMN_UNUSED(image),
                             const unsigned int CMN_UNUSED(videoch),
                             unsigned char * CMN_UNUSED(buffer),
                             size_t & CMN_UNUSED(buffersize),
                             const int CMN_UNUSED(compression))
{
    return SVL_FAIL;
}
int svlImageCodecBase::Write(const svlSampleImage & image,
                             const unsigned int videoch,
                             unsigned char * buffer,
                             size_t & buffersize,
                             const std::string & CMN_UNUSED(codec),
                             const int compression)
{
    return Write(image, videoch, buffer, buffersize, compression);
}

void svlImageCodecBase::SetExtensionList(const std::string &list)
{
    ExtensionList = list;
}

