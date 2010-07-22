/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
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


class svlVideoCodecCVI : public svlVideoCodecBase, public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

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

protected:
    const std::string CodecName;
    const vctFixedSizeVector<std::string, 3> FileStartMarker;
    const std::string FrameStartMarker;

    int Version;
    std::fstream* File;
    long long int FooterOffset;
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
    osaThread SaveThread;
    osaThreadSignal SaveInitEvent;
    osaThreadSignal NewFrameEvent;
    osaThreadSignal WriteDoneEvent;
    bool SaveInitialized;
    bool KillSaveThread;
    bool SaveThreadError;
    void* SaveProc(int param);
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlVideoCodecCVI)

#endif // _svlVideoCodecCVI_h

