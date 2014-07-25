/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2010

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlVideoCodecCVI_h
#define _svlVideoCodecCVI_h

#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaThreadSignal.h>
#include <cisstStereoVision/svlVideoIO.h>
#include <cisstStereoVision/svlTypes.h>
#include <cisstStereoVision/svlFile.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

class CISST_EXPORT svlVideoCodecCVI : public svlVideoCodecBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    typedef struct _CompressionData {
        unsigned char Level;
        unsigned char Differential;
    } CompressionData;

public:
    svlVideoCodecCVI();
    virtual ~svlVideoCodecCVI();

    virtual int Open(const std::string &filename, unsigned int &width, unsigned int &height, double &framerate);
    virtual int Create(const std::string &filename, const unsigned int width, const unsigned int height, const double framerate);
    virtual int Close();

    virtual int GetBegPos() const;
    virtual int GetEndPos() const;
    virtual int GetPos() const;
    virtual int SetPos(const int pos);

    virtual double GetBegTime() const;
    virtual double GetEndTime() const;
    virtual double GetTimeAtPos(const int pos) const;
    virtual int GetPosAtTime(const double time) const;
    
    virtual svlVideoIO::Compression* GetCompression() const;
    virtual int SetCompression(const svlVideoIO::Compression *compression);
    virtual int DialogCompression();

    virtual double GetTimestamp() const;
    virtual int SetTimestamp(const double timestamp);

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
    const std::string CodecName;
    const vctFixedSizeVector<std::string, 4> FileStartMarker;
    const std::string FrameStartMarker;

    CompressionData Config;

    int Version;
    svlFile File;
    long long int FooterOffset;
    long long int DataOffset;
    unsigned int PartCount;
    unsigned int Width;
    unsigned int Height;
    int BegPos;
    int EndPos;
    int Pos;
    bool Opened;
    bool Writing;
    double Timestamp;

    vctDynamicVector<long long int> FrameOffsets;
    vctDynamicVector<double> FrameTimestamps;

    unsigned char* prevYuvBuffer;
    unsigned int prevYuvBufferSize;
    unsigned char* yuvBuffer;
    unsigned int yuvBufferSize;
    unsigned char* comprBuffer;
    unsigned int comprBufferSize;
    vctDynamicVector<unsigned int> ComprPartOffset;
    vctDynamicVector<unsigned int> ComprPartSize;

    vctFixedSizeVector<unsigned char*, 2> saveBuffer;
    unsigned int saveBufferSize;
    unsigned int SaveBufferUsedSize;
    unsigned int SaveBufferUsedID;
    osaThread* SaveThread;
    osaThreadSignal* SaveInitEvent;
    osaThreadSignal* NewFrameEvent;
    osaThreadSignal* WriteDoneEvent;
    bool SaveInitialized;
    bool KillSaveThread;
    bool SaveThreadError;

    svlProcInfo ProcInfoSingleThread;

    void DiffEncode(unsigned char* input, unsigned char* previous, unsigned char* output, const unsigned int size);
    void DiffDecode(unsigned char* input, unsigned char* previous, unsigned char* output, const unsigned int size);

    void* SaveProc(int param);
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlVideoCodecCVI)

#endif // _svlVideoCodecCVI_h

