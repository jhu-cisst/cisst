/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2010

  (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#ifndef _svlFilterSourceBase_h
#define _svlFilterSourceBase_h

#include <cisstOSAbstraction/osaStopwatch.h>
#include <cisstStereoVision/svlFilterBase.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


// Forward declarations
class svlStreamManager;
class svlStreamProc;


class CISST_EXPORT svlFilterSourceBase : public svlFilterBase
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT)

friend class svlFilterOutput;
friend class svlStreamManager;
friend class svlStreamProc;

public:
    class SourceConfig
    {
    public:
        SourceConfig();
        SourceConfig(const SourceConfig& config);

        double Framerate;
        bool Loop;
    };

public:
    svlFilterSourceBase();
    svlFilterSourceBase(bool autotimestamps);
    virtual ~svlFilterSourceBase();

    virtual void SetTargetFrequency(const double & hertz);
    virtual double GetTargetFrequency() const;
    virtual void SetLoop(const bool & loop);
    virtual bool GetLoop() const;

    void Pause();
    void Play();
    void Play(const int & frames);

protected:
    virtual int Initialize(svlSample* &syncOutput);
    virtual int OnStart(unsigned int procCount);
    virtual int Process(svlProcInfo* procInfo, svlSample* &syncOutput) = 0;
    virtual void OnStop();
    virtual int Release();
    virtual void OnResetTimer();

    void ResetTargetTimer();
    int RestartTargetTimer();
    int StopTargetTimer();
    int WaitForTargetTimer();
    bool IsTargetTimerRunning();

private:
    // Dispatched to source-specific methods declared above
    int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);

    // Hide input setup methods from derived classes
    int AddInput(const std::string &inputname, bool trunk = true);
    int AddInputType(const std::string &inputname, svlStreamType type);
    int OnConnectInput(svlFilterInput &input, svlStreamType type);

    bool AutoTimestamp;
    osaStopwatch TargetTimer;
    double TargetStartTime;
    double TargetFrameTime;
    unsigned int PlaybackFrameOffset;

    int PlayCounter;
    int PauseAtFrameID;

    SourceConfig SourceSettings;
};

CMN_DECLARE_SERVICES_INSTANTIATION(svlFilterSourceBase)

#endif // _svlFilterSourceBase_h

