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

#ifndef _svlFilterSourceVideoFile_h
#define _svlFilterSourceVideoFile_h

#include <cisstStereoVision/svlFilterSourceBase.h>
#include <cisstStereoVision/svlVideoIO.h>

#include <cisstMultiTask/mtsFixedSizeVectorTypes.h>
#include <cisstMultiTask/mtsStateTable.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlFilterSourceVideoFile : public svlFilterSourceBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    typedef svlFilterSourceVideoFile ThisType;

    class Config : public SourceConfig
    {
    public:
        Config();
        Config(const Config& objref);

        int                           Channels;
        vctDynamicVector<std::string> FilePath;
        vctDynamicVector<int>         Length;
        vctDynamicVector<int>         Position;
        vctDynamicVector<vctInt2>     Range;

        void SetChannels(const int channels);
        friend std::ostream & operator << (std::ostream & stream, const Config & objref);
    };

public:
    svlFilterSourceVideoFile();
    svlFilterSourceVideoFile(unsigned int channelcount);
    ~svlFilterSourceVideoFile();

    // Configuration methods
    int SetChannelCount(unsigned int channelcount);
    int DialogFilePath(unsigned int videoch = SVL_LEFT);
    int SetFilePath(const std::string &filepath, unsigned int videoch = SVL_LEFT);
    int GetFilePath(std::string &filepath, unsigned int videoch = SVL_LEFT) const;
    int SetPosition(const int position, unsigned int videoch = SVL_LEFT);
    int GetPosition(unsigned int videoch = SVL_LEFT) const;
    int SetRange(const vctInt2 range, unsigned int videoch = SVL_LEFT);
    int GetRange(vctInt2& range, unsigned int videoch = SVL_LEFT) const;
    int GetLength(unsigned int videoch = SVL_LEFT) const;

    // Run-time methods (available when 'Initialized')
    unsigned int GetWidth(unsigned int videoch = SVL_LEFT);
    unsigned int GetHeight(unsigned int videoch = SVL_LEFT);
    int GetPositionAtTime(const double time, unsigned int videoch = SVL_LEFT) const;
    double GetTimeAtPosition(const int position, unsigned int videoch = SVL_LEFT) const;

protected:
    virtual int Initialize(svlSample* &syncOutput);
    virtual int OnStart(unsigned int procCount);
    virtual int Process(svlProcInfo* procInfo, svlSample* &syncOutput);
    virtual int Release();
    virtual void OnResetTimer();

private:
    svlSampleImage* OutputImage;
    vctDynamicVector<svlVideoCodecBase*> Codec;
    bool ResetTimer;
    double FirstTimestamp;
    double NativeFramerate;
    osaStopwatch Timer;

protected:
    Config Settings;

    virtual void CreateInterfaces();
    virtual void GetCommand(ThisType::Config & objref) const;
    virtual void SetCommand(const ThisType::Config & objref);
    virtual void SetChannelsCommand(const int & channels);
    virtual void SetPathLCommand(const std::string & filepath);
    virtual void SetPathRCommand(const std::string & filepath);
    virtual void SetPosLCommand(const int & position);
    virtual void SetPosRCommand(const int & position);
    virtual void SetRangeLCommand(const vctInt2 & position);
    virtual void SetRangeRCommand(const vctInt2 & position);
    virtual void GetPositionAtTimeLCommand(const double & time, int & position) const;
    virtual void GetPositionAtTimeRCommand(const double & time, int & position) const;
    virtual void GetTimeAtPositionLCommand(const int & position, double & time) const;
    virtual void GetTimeAtPositionRCommand(const int & position, double & time) const;
};

std::ostream & operator << (std::ostream & stream, const svlFilterSourceVideoFile::Config & objref);

typedef mtsGenericObjectProxy<svlFilterSourceVideoFile::Config> svlFilterSourceVideoFile_Config;
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterSourceVideoFile_Config);
typedef mtsGenericObjectProxy<vctInt2> svlFilterSourceVideoFile_vctInt2;
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterSourceVideoFile_vctInt2);

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterSourceVideoFile)

#endif // _svlFilterSourceVideoFile_h

