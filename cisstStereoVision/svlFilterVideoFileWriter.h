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

#ifndef _svlFilterVideoFileWriter_h
#define _svlFilterVideoFileWriter_h

#include <cisstStereoVision/svlFilterBase.h>
#include <cisstStereoVision/svlVideoIO.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


// Forward declarations
class osaTimeServer;
class svlFile;


class CISST_EXPORT svlFilterVideoFileWriter : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterVideoFileWriter();
    virtual ~svlFilterVideoFileWriter();

    int DialogOpenFile(unsigned int videoch = SVL_LEFT);
    int DialogFramerate(unsigned int videoch = SVL_LEFT);
    int DialogCodec(const std::string &extension, unsigned int videoch = SVL_LEFT);

    int SetFilePath(const std::string &filepath, unsigned int videoch = SVL_LEFT);
    int SetFramerate(const double framerate, unsigned int videoch = SVL_LEFT);
    int SetEnableTimestampsFile(bool enable, unsigned int videoch = SVL_LEFT);
    int SetCodecParams(const svlVideoIO::Compression *compression, unsigned int videoch = SVL_LEFT);
    int ResetCodec(unsigned int videoch = SVL_LEFT);
    int SaveCodec(const std::string &filepath, unsigned int videoch = SVL_LEFT) const;
    int LoadCodec(const std::string &filepath, unsigned int videoch = SVL_LEFT);

    std::string GetFilePath(unsigned int videoch = SVL_LEFT) const;
    int GetFilePath(std::string &filepath, unsigned int videoch = SVL_LEFT) const;
    double GetFramerate(unsigned int videoch = SVL_LEFT);
    bool GetEnableTimestampsFile(unsigned int videoch = SVL_LEFT) const;
    std::string GetCodecName(unsigned int videoch = SVL_LEFT) const;
    int GetCodecName(std::string &name, unsigned int videoch = SVL_LEFT) const;
    svlVideoIO::Compression* GetCodecParams(unsigned int videoch = SVL_LEFT) const;
    int GetCodecParams(svlVideoIO::Compression **compression, unsigned int videoch = SVL_LEFT) const;

    int OpenFile(unsigned int videoch = SVL_LEFT);
    int CISST_DEPRECATED OpenFile(const std::string &filepath, unsigned int videoch = SVL_LEFT);
    int CloseFile(unsigned int videoch = SVL_LEFT);

    void Record(int frames = -1);
    void RecordAtTime(int frames = -1, double time = -1.0);
    void Pause();
    void PauseAtTime(double time = -1.0);

    bool GetIsRecording() const;

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int OnStart(unsigned int procCount);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);
    virtual int Release();

private:
    osaCriticalSection CS;

    bool ErrorInProcess;
    bool Action;
    double ActionTime;
    double TargetActionTime;
    bool IsRecording;
    osaTimeServer* TimeServer;
    unsigned int TargetCaptureLength;

    unsigned int VideoFrameCounter;
    unsigned int CaptureLength;
    bool CodecsMultithreaded;
    vctDynamicVector<bool> ErrorOnChannel;
    vctDynamicVector<vctUInt2> ImageDimensions;
    vctDynamicVector<svlVideoCodecBase*> Codec;
    vctDynamicVector<svlVideoCodecBase*> CodecProto;
    vctDynamicVector<svlVideoIO::Compression*> CodecParam;
    vctDynamicVector<std::string> FilePath;
    vctDynamicVector<double> Framerate;
    vctDynamicVector<unsigned int> FramesWritten;
    vctDynamicVector<bool> EnableTimestampsFile;
    vctDynamicVector<svlFile*> TimestampsFile;
    vctDynamicVector<double> FirstTimestamp;

    void UpdateCodecCount(const unsigned int count);
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterVideoFileWriter)

#endif // _svlFilterVideoFileWriter_h

