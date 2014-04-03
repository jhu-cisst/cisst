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

#ifndef _svlVideoCodecVfW32_h
#define _svlVideoCodecVfW32_h

#include <cisstStereoVision/svlVideoIO.h>
#include <cisstStereoVision/svlTypes.h>

#include "windows.h"
#include "vfw.h"


class svlVideoCodecVfW32 : public svlVideoCodecBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    svlVideoCodecVfW32();
    virtual ~svlVideoCodecVfW32();

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
    void Flip(unsigned char* image);
    void FlipCopy(const unsigned char* source, unsigned char* dest);

    unsigned int Width;
    unsigned int Height;
    int BegPos;
    int EndPos;
    int Pos;
    int KeyFrameEvery;
    bool Writing;
    bool Opened;
    double Framerate;

    PAVIFILE        pAviFile;
    PAVISTREAM      pAviStream;
    PAVISTREAM      pAviStreamCompressed;
    PGETFRAME       pGetFrame;
    AVIFILEINFO     AviFileInfo;
    AVISTREAMINFO   AviStreamInfo;

    bool UpsideDown;
	unsigned char* comprBuffer;
	unsigned int comprBufferSize;

    void LogError(HRESULT ret);
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlVideoCodecVfW32)

#endif // _svlVideoCodecVfW32_h

