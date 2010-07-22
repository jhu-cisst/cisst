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
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    svlFilterVideoFileWriter();
    ~svlFilterVideoFileWriter();

    int Disable(bool disable, unsigned int videoch = SVL_LEFT);
    int DialogFilePath(unsigned int videoch = SVL_LEFT);
    int SetFilePath(const std::string &filepath, unsigned int videoch = SVL_LEFT);
    int GetFilePath(std::string &filepath, unsigned int videoch = SVL_LEFT) const;
    int SetFramerate(double framerate);

    int DialogCodec(unsigned int videoch = SVL_LEFT);
    int GetCodecName(std::string &name, unsigned int videoch = SVL_LEFT) const;
    int GetCodec(svlVideoIO::Compression **compression, unsigned int videoch = SVL_LEFT) const;
    int SetCodec(const svlVideoIO::Compression *compression, unsigned int videoch = SVL_LEFT);
    int SaveCodec(const std::string &filepath, unsigned int videoch = SVL_LEFT) const;
    int LoadCodec(const std::string &filepath, unsigned int videoch = SVL_LEFT);

    void Pause();
    void Record(int frames = -1);

protected:
    virtual int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    virtual int OnStart(unsigned int procCount);
    virtual int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);
    virtual int Release();

private:
    bool Action;
    double ActionTime;
    double TargetActionTime;
    osaTimeServer* TimeServer;
    unsigned int TargetCaptureLength;

    unsigned int VideoFrameCounter;
    unsigned int CaptureLength;
    double Framerate;
    bool CodecsMultithreaded;
    vctDynamicVector<svlVideoIO::Compression*> CodecParams;
    vctDynamicVector<svlVideoCodecBase*> Codec;
    vctDynamicVector<bool> Disabled;
    vctDynamicVector<std::string> FilePath;

    int UpdateStreamCount(unsigned int count);
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterVideoFileWriter)

#endif // _svlFilterVideoFileWriter_h

