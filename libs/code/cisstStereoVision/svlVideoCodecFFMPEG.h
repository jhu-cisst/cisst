/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $
  
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
//#include <cisstStereoVision/svlFile.h>

extern "C" {
    #include <avcodec.h>
    #include <avformat.h>
    #include <avio.h>
    #include <swscale.h>
}


class svlVideoCodecFFMPEG : public svlVideoCodecBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    typedef struct _CompressionData {
        unsigned int EncoderID;
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
//    svlFile *OutputFile;
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

    void BuildIndex();
//    int  WriteFooter();
    void ConfigureEncoder();
    void BuildEncoderList();
    int GetEncoderID(CodecID codec_id);
    std::string GetExtensionFromEncoderID(unsigned int encoder_id);
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlVideoCodecFFMPEG)

#endif // _svlVideoCodecFFMPEG_h

