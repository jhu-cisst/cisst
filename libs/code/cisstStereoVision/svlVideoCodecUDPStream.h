/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $
  
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

#ifndef _svlVideoCodecUDPStream_h
#define _svlVideoCodecUDPStream_h

#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaThreadSignal.h>
#include <cisstStereoVision/svlVideoIO.h>
#include <cisstStereoVision/svlBufferMemory.h>
#include <cisstStereoVision/svlTypes.h>


class svlVideoCodecUDPStream : public svlVideoCodecBase, public cmnGenericObject
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

protected:
    const std::string CodecName;
    const std::string FrameStartMarker;

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

    unsigned char* yuvBuffer;
    unsigned int yuvBufferSize;
    unsigned char* comprBuffer;
    unsigned int comprBufferSize;
    vctDynamicVector<unsigned int> ComprPartOffset;
    vctDynamicVector<unsigned int> ComprPartSize;

    char* PacketData;
    svlBufferMemory* streamingBuffer;
    unsigned int streamingBufferSize;
    unsigned int StreamingBufferUsedSize;
    unsigned int StreamingBufferUsedID;
    osaThread StreamingThread;
    osaThreadSignal StreamingInitEvent;
    bool StreamingInitialized;
    bool KillStreamingThread;
    bool StreamingThreadError;

    unsigned short SocketPort;
    std::string SocketAddress;
    int Socket;
    char* SockAddr;
    unsigned int ReceivedWidth;
    unsigned int ReceivedHeight;

    int CreateServer();
    int CreateClient();
    void CloseSocket();
    int Send(unsigned char* buffer, unsigned int size, unsigned int& packet_id);
    int Receive(unsigned char* buffer, unsigned int size);

    void* SendProc(int param);
    void* ReceiveProc(int param);
    bool CompareData(const unsigned char* data1, const unsigned char* data2, unsigned int size);
    int ParseFilename(const std::string & filename);
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlVideoCodecUDPStream)

#endif // _svlVideoCodecUDPStream_h

