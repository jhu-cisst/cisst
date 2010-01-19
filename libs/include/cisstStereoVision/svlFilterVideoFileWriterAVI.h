/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

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

#ifndef _svlFilterVideoFileWriterAVI_h
#define _svlFilterVideoFileWriterAVI_h

#include <cisstStereoVision/svlStreamManager.h>
#include <string>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterVideoFileWriterAVI : public svlFilterBase, public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    svlFilterVideoFileWriterAVI();
    ~svlFilterVideoFileWriterAVI();

    int DialogFilePath(unsigned int videoch = SVL_LEFT);
    int DialogCodec();
    void GetEncoderName(std::string& name) { name = EncoderName; }

    int Disable(bool disable, unsigned int videoch = SVL_LEFT);
    int SetFilePath(const std::string filepath, unsigned int videoch = SVL_LEFT);
    int SetFramerate(double fps);
    int SetKeyFrameInteval(unsigned int interval);

    void Pause();
    void Record(int frames = -1);

    int SaveCodecSettings(const std::string filepath);
    int LoadCodecSettings(const std::string filepath);

protected:
    virtual int Initialize(svlSample* inputdata);
    virtual int OnStart(unsigned int procCount);
    virtual int ProcessFrame(ProcInfo* procInfo, svlSample* inputdata);
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
    unsigned int KeyFrames;
    void* CompressOptions;
    vctDynamicVector<void*> VideoObj;
    vctDynamicVector<bool> Disabled;
    vctDynamicVector<std::string> FilePath;
    std::string EncoderName;

    int UpdateStreamCount(unsigned int count);
};

CMN_DECLARE_SERVICES_INSTANTIATION(svlFilterVideoFileWriterAVI)

#endif // _svlFilterVideoFileWriterAVI_h

