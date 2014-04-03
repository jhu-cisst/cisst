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

#ifndef _svlVideoIO_h
#define _svlVideoIO_h

#include <cisstMultiTask/mtsComponent.h>
#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstOSAbstraction/osaCriticalSection.h>
#include <string>

// Always include last!
#include <cisstStereoVision/svlExport.h>


// Forward declarations
struct svlProcInfo;
class svlSampleImage;
class svlVideoCodecBase;


/***********************************/
/*** svlVideoIO class **************/
/***********************************/

class CISST_EXPORT svlVideoIO
{
    friend void svlInitializeVideoCodecs();

public:
    typedef struct _Compression
    {
        unsigned int    size;
        char            extension[16];
        char            name[64];
        unsigned int    datasize;
        unsigned char   data[1];
    } Compression;

private:
    typedef vctDynamicVector<cmnClassServicesBase*> _CodecList;
    typedef vctDynamicVector<svlVideoCodecBase*> _CodecCacheList;
    typedef vctDynamicVector<bool> _CodecCacheUsedList;
    typedef vctDynamicVector<std::string> _StringList;

    svlVideoIO();
    svlVideoIO(const svlVideoIO &);
    static svlVideoIO* GetInstance();

    _CodecList Codecs;
    _StringList Names;
    _StringList Extensions;
    vctDynamicVector<_CodecCacheList> CodecCache;
    vctDynamicVector<_CodecCacheUsedList> CodecCacheUsed;
    osaCriticalSection CS;

public:
    ~svlVideoIO();

    static int DialogCodec(const std::string &filename, Compression **compression);
    static int DialogFilePath(bool save, const std::string &title, std::string &filename);

    static int GetFormatList(std::string &formatlist);
    static int GetWindowsExtensionFilter(std::string &filter);
    static int GetExtension(const std::string &filename, std::string &extension);

    static svlVideoCodecBase* GetCodec(const std::string &filename);
    static void ReleaseCodec(svlVideoCodecBase* codec);
    static int ReleaseCompression(Compression *compression);
};


/*************************************/
/*** svlVideoCodecBase class *********/
/*************************************/

class CISST_EXPORT svlVideoCodecBase : public mtsComponent
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT)

public:
    svlVideoCodecBase();
    virtual ~svlVideoCodecBase();

    const std::string& GetEncoderName() const;
    const std::string& GetExtensions() const;
    bool IsMultithreaded() const;
    bool IsVariableFramerate() const;

    virtual int Open(const std::string &filename, unsigned int &width, unsigned int &height, double &framerate) = 0;
    virtual int Create(const std::string &filename, const unsigned int width, const unsigned int height, const double framerate) = 0;
    virtual int Close() = 0;

    virtual int GetBegPos() const = 0;
    virtual int GetEndPos() const = 0;
    virtual int GetPos() const = 0;
    virtual int SetPos(const int pos);

    virtual double GetBegTime() const;
    virtual double GetEndTime() const;
    virtual double GetTimeAtPos(const int pos) const;
    virtual int GetPosAtTime(const double time) const;

    virtual svlVideoIO::Compression* GetCompression() const;
    virtual int SetCompression(const svlVideoIO::Compression *compression);
    virtual int DialogCompression();
    virtual int DialogCompression(const std::string &filename);

    virtual double GetTimestamp() const = 0;
    virtual int SetTimestamp(const double timestamp);

    virtual int Read(svlProcInfo* procInfo, svlSampleImage &image, const unsigned int videoch, const bool noresize = false) = 0;
    virtual int Write(svlProcInfo* procInfo, const svlSampleImage &image, const unsigned int videoch) = 0;

public:
    virtual void SetExtension(const std::string & extension) = 0;
    virtual void SetEncoderID(const int & encoder_id) = 0;
    virtual void SetCompressionLevel(const int & compr_level) = 0;
    virtual void SetQualityBased(const bool & enabled) = 0;
    virtual void SetTargetQuantizer(const double & target_quant) = 0;
    virtual void SetDatarate(const int & datarate) = 0;
    virtual void SetKeyFrameEvery(const int & key_every) = 0;
    virtual void IsCompressionLevelEnabled(bool & enabled) const = 0;
    virtual void IsEncoderListEnabled(bool & enabled) const = 0;
    virtual void IsTargetQuantizerEnabled(bool & enabled) const = 0;
    virtual void IsDatarateEnabled(bool & enabled) const = 0;
    virtual void IsKeyFrameEveryEnabled(bool & enabled) const = 0;
    virtual void GetCompressionLevel(int & compr_level) const = 0;
    virtual void GetEncoderList(std::string & encoder_list) const = 0;
    virtual void GetEncoderID(int & encoder_id) const = 0;
    virtual void GetQualityBased(bool & enabled) const = 0;
    virtual void GetTargetQuantizer(double & target_quant) const = 0;
    virtual void GetDatarate(int & datarate) const = 0;
    virtual void GetKeyFrameEvery(int & key_every) const = 0;

protected:
    void SetName(const std::string &name);
    void SetExtensionList(const std::string &list);
    void SetMultithreaded(bool multithreaded);
    void SetVariableFramerate(bool variableframerate);

    svlVideoIO::Compression* Codec;

private:
    std::string EncoderName;
    std::string ExtensionList;
    bool Multithreaded;
    bool VariableFramerate;

    bool ComponentAddedtoLCM;
    std::string EncoderConnectedComponent;
    std::string EncoderConnectedInterface;
    void CreateInterfaces();
};

CMN_DECLARE_SERVICES_INSTANTIATION(svlVideoCodecBase)

#endif // _svlVideoIO_h

