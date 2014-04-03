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

#ifndef _svlImageIO_h
#define _svlImageIO_h

#include <cisstCommon/cmnGenericObject.h>
#include <cisstOSAbstraction/osaCriticalSection.h>
#include <cisstVector/vctDynamicVector.h>
#include <string>

// Always include last!
#include <cisstStereoVision/svlExport.h>


// Forward declarations
class svlSampleImage;
class cmnClassServicesBase;

/*************************************/
/*** svlImageCodecBase class *********/
/*************************************/

class CISST_EXPORT svlImageCodecBase : public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT)
public:
    svlImageCodecBase();
    virtual ~svlImageCodecBase();

    const std::string& GetExtensions() const;

    virtual int ReadDimensions(const std::string &filename, unsigned int &width, unsigned int &height);
    virtual int ReadDimensions(std::istream &stream, unsigned int &width, unsigned int &height);
    virtual int ReadDimensions(const unsigned char *buffer, const size_t buffersize, unsigned int &width, unsigned int &height);

    virtual int Read(svlSampleImage &image, const unsigned int videoch, const std::string &filename, bool noresize = false);
    virtual int Read(svlSampleImage &image, const unsigned int videoch, std::istream &stream, bool noresize = false);
    virtual int Read(svlSampleImage &image, const unsigned int videoch, const unsigned char *buffer, const size_t buffersize, bool noresize = false);

    virtual int Write(const svlSampleImage &image, const unsigned int videoch, const std::string &filename, const int compression = -1);
    virtual int Write(const svlSampleImage &image, const unsigned int videoch, std::ostream &stream, const int compression = -1);
    virtual int Write(const svlSampleImage &image, const unsigned int videoch, std::ostream &stream, const std::string &codec, const int compression = -1);
    virtual int Write(const svlSampleImage &image, const unsigned int videoch, unsigned char *buffer, size_t &buffersize, const int compression = -1);
    virtual int Write(const svlSampleImage &image, const unsigned int videoch, unsigned char *buffer, size_t &buffersize, const std::string &codec, const int compression = -1);

protected:
    void SetExtensionList(const std::string &list);

private:
    std::string ExtensionList;
};

CMN_DECLARE_SERVICES_INSTANTIATION(svlImageCodecBase)

/***********************************/
/*** svlImageIO class **************/
/***********************************/

class CISST_EXPORT svlImageIO
{
    friend void svlInitializeImageCodecs();

private:
    typedef vctDynamicVector<cmnClassServicesBase*> _CodecList;
    typedef vctDynamicVector<svlImageCodecBase*> _CodecCacheList;
    typedef vctDynamicVector<bool> _CodecCacheUsedList;
    typedef vctDynamicVector<std::string> _ExtensionList;

    svlImageIO();
    svlImageIO(const svlImageIO &);
    static svlImageIO* GetInstance();

    _CodecList Codecs;
    _ExtensionList Extensions;
    vctDynamicVector<_CodecCacheList> CodecCache;
    vctDynamicVector<_CodecCacheUsedList> CodecCacheUsed;
    osaCriticalSection CS;

public:
    ~svlImageIO();

    static int GetExtension(const std::string &filename, std::string &extension);

    static svlImageCodecBase* GetCodec(const std::string &filename);
    static void ReleaseCodec(svlImageCodecBase* codec);

    static int ReadDimensions(const std::string &filename, unsigned int &width, unsigned int &height);
    static int ReadDimensions(const std::string &codec, std::istream &stream, unsigned int &width, unsigned int &height);
    static int ReadDimensions(const std::string &codec, const unsigned char *buffer, const size_t buffersize, unsigned int &width, unsigned int &height);

    static int Read(svlSampleImage &image, const unsigned int videoch, const std::string &filename, bool noresize = false);
    static int Read(svlSampleImage &image, const unsigned int videoch, const std::string &codec, std::istream &stream, bool noresize = false);
    static int Read(svlSampleImage &image, const unsigned int videoch, const std::string &codec, const unsigned char *buffer, const size_t buffersize, bool noresize = false);

    static int Write(const svlSampleImage &image, const unsigned int videoch, const std::string &filename, const int compression = -1);
    static int Write(const svlSampleImage &image, const unsigned int videoch, const std::string &codec, std::ostream &stream, const int compression = -1);
    static int Write(const svlSampleImage &image, const unsigned int videoch, const std::string &codec, unsigned char *buffer, size_t &buffersize, const int compression = -1);
};


#endif // _svlImageIO_h

