/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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


class CISST_EXPORT svlFilterVideoFileWriter : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    svlFilterVideoFileWriter();
    ~svlFilterVideoFileWriter();

    int OpenFile(const std::string &filepath, unsigned int videoch = SVL_LEFT);
    int DialogOpenFile(unsigned int videoch = SVL_LEFT);
    int CloseFile(unsigned int videoch = SVL_LEFT);

    int SetCodec(const svlVideoIO::Compression *compression, double framerate, unsigned int videoch = SVL_LEFT);
    int DialogCodec(const std::string &extension, unsigned int videoch = SVL_LEFT);
    int ResetCodec(unsigned int videoch = SVL_LEFT);
    int SaveCodec(const std::string &filepath, unsigned int videoch = SVL_LEFT) const;
    int LoadCodec(const std::string &filepath, unsigned int videoch = SVL_LEFT);

    int GetFilePath(std::string &filepath, unsigned int videoch = SVL_LEFT) const;
    int GetCodecName(std::string &name, unsigned int videoch = SVL_LEFT) const;
    int GetCodec(svlVideoIO::Compression **compression, unsigned int videoch = SVL_LEFT) const;

    void Record(int frames = -1);
    void RecordAtTime(int frames = -1, double time = -1.0);
    void Pause();
    void PauseAtTime(double time = -1.0);

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
    osaTimeServer* TimeServer;
    unsigned int TargetCaptureLength;

    unsigned int VideoFrameCounter;
    unsigned int CaptureLength;
    bool CodecsMultithreaded;
    vctDynamicVector<bool> ErrorOnChannel;
    vctDynamicVector<vctUInt2> ImageDimensions;
    vctDynamicVector<svlVideoCodecBase*> Codec;
    vctDynamicVector<svlVideoIO::Compression*> CodecParam;
    vctDynamicVector<std::string> FilePath;
    vctDynamicVector<double> Framerate;

    void UpdateCodecCount(const unsigned int count);
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterVideoFileWriter)

#endif // _svlFilterVideoFileWriter_h

