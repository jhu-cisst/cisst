/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2011

  (C) Copyright 2006-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlVideoCodecFFMPEG_h
#define _svlVideoCodecFFMPEG_h

#include <cisstStereoVision/svlVideoIO.h>
#include <cisstStereoVision/svlTypes.h>

#ifndef INT64_C
    #define INT64_C(c) (c ## LL)
    #define UINT64_C(c) (c ## ULL)
#endif

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavformat/avio.h>
    #include <libswscale/swscale.h>
}

#define CALC_FFMPEG_VERSION(a,b,c) ( a<<16 | b<<8 | c )
#if LIBAVCODEC_BUILD >= CALC_FFMPEG_VERSION(54,25,0)
#  define CV_CODEC_ID AVCodecID
#  define CV_CODEC(name) AV_##name
#else
#  define CV_CODEC_ID CodecID
#  define CV_CODEC(name) name
#endif

class svlVideoCodecFFMPEG : public svlVideoCodecBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    typedef struct _CompressionData {
        unsigned int EncoderID;
        bool         QualityBased;
        double       TargetQuantizer;
        unsigned int Bitrate;
        unsigned int GoP;
    } CompressionData;

public:
    svlVideoCodecFFMPEG();
    virtual ~svlVideoCodecFFMPEG();

    virtual int Open(const std::string &filename, unsigned int &width, unsigned int &height, double &framerate);
    virtual int Create(const std::string &filename, const unsigned int width, const unsigned int height, const double framerate);
    virtual int Close();

    virtual int GetBegPos() const;
    virtual int GetEndPos() const;
    virtual int GetPos() const;
    virtual int SetPos(const int pos);

    virtual svlVideoIO::Compression* GetCompression() const;
    virtual int SetCompression(const svlVideoIO::Compression *compression);
    virtual int DialogCompression();
    virtual int DialogCompression(const std::string &filename);

    virtual double GetTimestamp() const;

    virtual int Read(svlProcInfo* procInfo, svlSampleImage &image, const unsigned int videoch, const bool noresize = false);
    virtual int Write(svlProcInfo* procInfo, const svlSampleImage &image, const unsigned int videoch);

public:
    virtual void SetExtension(const std::string & extension);
    virtual void SetEncoderID(const int & encoder_id);
    virtual void SetCompressionLevel(const int & compr_level);
    virtual void SetQualityBased(const bool & enabled);
    virtual void SetTargetQuantizer(const double & target_quant);
    virtual void SetDatarate(const int & datarate);
    virtual void SetKeyFrameEvery(const int & key_every);
    virtual void IsCompressionLevelEnabled(bool & enabled) const;
    virtual void IsEncoderListEnabled(bool & enabled) const;
    virtual void IsTargetQuantizerEnabled(bool & enabled) const;
    virtual void IsDatarateEnabled(bool & enabled) const;
    virtual void IsKeyFrameEveryEnabled(bool & enabled) const;
    virtual void GetCompressionLevel(int & compr_level) const;
    virtual void GetEncoderList(std::string & encoder_list) const;
    virtual void GetEncoderID(int & encoder_id) const;
    virtual void GetQualityBased(bool & enabled) const;
    virtual void GetTargetQuantizer(double & target_quant) const;
    virtual void GetDatarate(int & datarate) const;
    virtual void GetKeyFrameEvery(int & key_every) const;

protected:
    CompressionData Config;

    bool Opened;
    bool Writing;
    unsigned int Width;
    unsigned int Height;
    double Framerate;
    int Framestep;
    int Length;
    int Position;

    virtual int DialogCompression(const std::string &extension, unsigned int encoder_id);

private:
    AVFormatContext *pFormatCtx;
    AVCodecContext *pDecoderCtx;
    AVCodecContext *pEncoderCtx;
    AVStream *pStream;
    AVFrame *pFrame;
    AVFrame *pFrameRGB;
    SwsContext *pConvertCtx;
    int VideoStreamID;
    int LastExtractedFrame;
    int LastDTS;
    bool RepeatFrame;
    osaCriticalSection CS;
    bool UseIndex;
    vctDynamicVector<int> KeyFrameIndex;
    vctDynamicVector<unsigned char> ConversionBuffer;
    vctDynamicVector<unsigned char> OutputBuffer;
    vctDynamicVector<std::string> EncoderNames;
    vctDynamicVector<unsigned int> EncoderIDs;

    svlProcInfo ProcInfoSingleThread;

    void BuildIndex();
    void ConfigureEncoder();
    void BuildEncoderList();
    /// \note AVCodecID
    int GetFFMPEGEncoderID(CV_CODEC_ID codec_id) const;
    std::string GetExtensionFromEncoderID(unsigned int encoder_id);
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlVideoCodecFFMPEG)

#endif // _svlVideoCodecFFMPEG_h

