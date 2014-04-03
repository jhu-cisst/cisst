/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Daniel Obenshain & Tom Tantillo
  Created on: 2011

  (C) Copyright 2006-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlVideoCodecUDPStream_h
#define _svlVideoCodecUDPStream_h

#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaThreadSignal.h>
#include <cisstOSAbstraction/osaTimeServer.h>
#include <cisstStereoVision/svlVideoIO.h>
#include <cisstStereoVision/svlBufferMemory.h>
#include <cisstStereoVision/svlTypes.h>

#if (CISST_OS == CISST_WINDOWS)
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <errno.h>
#endif

class svlVideoCodecUDPStream : public svlVideoCodecBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    svlVideoCodecUDPStream();
    virtual ~svlVideoCodecUDPStream();

    virtual int Open(const std::string &filename, unsigned int &width, unsigned int &height, double &framerate);
    virtual int Create(const std::string &filename, const unsigned int width, const unsigned int height, const double framerate);
    virtual int Close();

    virtual int GetBegPos() const;
    virtual int GetEndPos() const;
    virtual int GetPos() const;

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
    const std::string FrameStartMarker;

    unsigned int PacketCount;
    double StartTime;
    int SizePacket; // Why is it here and not in the .cpp?

    std::fstream* File;
    unsigned int PartCount;
    unsigned int Width;
    unsigned int Height;
    int BegPos;
    int EndPos;
    int Pos;
    bool Opened;
    bool Writing;
    double Timestamp;

    char* PacketData;
    unsigned char* yuvBuffer;
    unsigned int yuvBufferSize;
    unsigned char* comprBuffer;
    unsigned int comprBufferSize;
    vctDynamicVector<unsigned int> ComprPartOffset;
    vctDynamicVector<unsigned int> ComprPartSize;

    sockaddr_in SendAddress;
    int ServerSocket;
    osaThread *ServerThread;
    osaThreadSignal* ServerInitEvent;
    bool ServerInitialized;
    bool KillServerThread;

    svlBufferMemory* ReceiveBuffer;
    svlBufferMemory* SendBuffer;

    osaThread *SendThread;
    int SendConnection;
    bool KillSendThread;

    int ReceiveSocket;
    sockaddr_in ReceiveAddress;
    osaThread *ReceiveThread;
    osaThreadSignal* ReceiveInitEvent;
    bool ReceiveInitialized;
    bool KillReceiveThread;

    osaThread *ReceiveSub;
    osaThread *ReceiveSubSync;
    bool KillReceiveSub;

    unsigned short SocketPort;
    std::string SocketAddress;
    char* SockAddr;

    osaTimeServer *TimeServer;

    //unsigned int		totalReceived=0;/*The number of packets we know we definitely got*/
    //unsigned int		totalPackets=0;/*The total number of packets (we think) the sender sent so far*/
    //unsigned int		totalOutOfOrder=0;/*The number of packets received out of order but not too late(a subset of the received packets)*/
    ///*current data*/
    //unsigned int    startPkt = 0;
    //unsigned int    globalPktNum = 1; /*The next expected global packet number*/
    //double		min = 99999, max = 0, total = 0, t;

    void  CloseSocket();
    void* ServerProc(unsigned short port);
    //void* ServerSync(unsigned short port);
    void* SendProc(int param);
    void* ReceiveProc(int param);
    //void* ReceiveSync(int param);
    void*  Receive(int param);
    bool  CompareData(const unsigned char* data1, const unsigned char* data2, unsigned int size);
    int   ParseFilename(const std::string & filename);
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlVideoCodecUDPStream)

#endif // _svlVideoCodecUDPStream_h

