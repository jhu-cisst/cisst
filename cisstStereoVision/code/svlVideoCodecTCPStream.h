/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi & Min Yang Jung
  Created on: 2010

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlVideoCodecTCPStream_h
#define _svlVideoCodecTCPStream_h

#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaThreadSignal.h>
#include <cisstStereoVision/svlVideoIO.h>
#include <cisstStereoVision/svlBufferMemory.h>
#include <cisstStereoVision/svlTypes.h>


class svlVideoCodecTCPStream : public svlVideoCodecBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    enum CompressorType {
        CVI,
        JPEG
    };

    svlVideoCodecTCPStream();
    virtual ~svlVideoCodecTCPStream();

    virtual int Open(const std::string &filename, unsigned int &width, unsigned int &height, double &framerate);
    virtual int Create(const std::string &filename, const unsigned int width, const unsigned int height, const double framerate);
    virtual int Close();

    virtual int GetBegPos() const;
    virtual int GetEndPos() const;
    virtual int GetPos() const;

    virtual svlVideoIO::Compression* GetCompression() const;
    virtual int SetCompression(const svlVideoIO::Compression *compression);
    virtual int DialogCompression();
    virtual int DialogCompression(const std::string &filename);

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
    CompressorType Compressor;

    const std::string CodecName;
    const std::string FrameStartMarker;

    std::fstream* File;
    unsigned int Width;
    unsigned int Height;
    int BegPos;
    int EndPos;
    int Pos;
    bool Opened;
    bool Writing;
    double Timestamp;

    svlProcInfo ProcInfoSingleThread;

    char* PacketData;
    char* PacketDataAccumulator;
    unsigned int AccumulatedSize;
    unsigned char* yuvBuffer;
    unsigned int yuvBufferSize;
    unsigned char* comprBuffer;
    unsigned int comprBufferSize;
    vctDynamicVector<unsigned int> ComprPartOffset;
    vctDynamicVector<unsigned int> ComprPartSize;

    int ServerSocket;
    osaThread* ServerThread;
    osaThreadSignal* ServerInitEvent;
    bool ServerInitialized;
    bool KillServerThread;

    bool ReadError;
    svlBufferMemory* ReceiveBuffer;

    vctDynamicVector<svlBufferMemory*> SendBuffer;
    vctDynamicVector<osaThread*> SendThread;
    vctDynamicVector<int> SendConnection;
    vctDynamicVector<bool> KillSendThread;

    int ReceiveSocket;
    osaThread* ReceiveThread;
    osaThreadSignal* ReceiveInitEvent;
    bool ReceiveInitialized;
    bool KillReceiveThread;
    bool ReceiveThreadError;

    unsigned short SocketPort;
    std::string SocketAddress;
    char* SockAddr;

    void  CloseSocket();
    void* ServerProc(unsigned short port);
    void* SendProc(unsigned int clientid);
    void* ReceiveProc(int param);
    int   Receive();
    int   FindFrameHeader(unsigned char* data1, const unsigned char* data2, unsigned int size);
    bool  CompareData(const unsigned char* data1, const unsigned char* data2, unsigned int size);
    int   ParseFilename(const std::string & filename);
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlVideoCodecTCPStream)

#endif // _svlVideoCodecTCPStream_h

