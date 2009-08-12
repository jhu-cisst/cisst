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


#ifndef _svlStreamManager_h
#define _svlStreamManager_h

#include <vector>
#include <map>

#include <cisstVector/vctDynamicVector.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaTimeServer.h>
#include <cisstOSAbstraction/osaStopwatch.h>
#include <cisstStereoVision/svlStreamDefs.h>
#include <cisstStereoVision/svlSyncPoint.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


// Status codes
#define SVL_STREAM_RUNNING            0
#define SVL_STREAM_CREATED            1
#define SVL_STREAM_INITIALIZED        2
#define SVL_STREAM_STOPPED            3
#define SVL_STREAM_RELEASED           4
#define SVL_ALREADY_PROCESSED        10
#define SVL_STOP_REQUEST             11

// Error codes
#define SVL_TYPE_MISMATCH           -12
#define SVL_ALREADY_INITIALIZED     -13
#define SVL_NOT_INITIALIZED         -14
#define SVL_ALREADY_RUNNING         -15
#define SVL_FILTER_NOT_IN_LIST      -16
#define SVL_NOT_SOURCE              -17
#define SVL_NO_SOURCE_IN_LIST       -18
#define SVL_INVALID_INPUT_TYPE      -19
#define SVL_INVALID_OUTPUT_TYPE     -20
#define SVL_NO_INPUT_DATA           -21
#define SVL_WRONG_CHANNEL           -22
#define SVL_CONSTRUCTION_FAILED     -23
#define SVL_ALLOCATION_ERROR        -24
#define SVL_NOT_IMAGE               -25
#define SVL_WAIT_TIMEOUT            -26


#define _OnSingleThread(_info) \
            if((_info)->id==0)
#define _ParallelLoop(_info, _idx, _count) \
            _idx=(_info)->id*std::max(1001*(_count)/(_info)->count,1000u)/1000; \
            for(const unsigned int _end=std::min((_count),((_info)->id+1)*std::max(1001*(_count)/(_info)->count,1000u)/1000);_idx<_end;_idx++)
#define _ParallelInterleavedLoop(_info, _idx, _count) \
            _idx=(_info)->id;for(const unsigned int _step=(_info)->count,_end=(_count);_idx<_end;_idx+=_step)
#define _GetParallelSubRange(_info, _count, _from, _to) \
            _to=(_count)/(_info)->count+1;_from=(_info)->id*_to;_to+=_from;if(_to>(_count)){_to=(_count);}
#define _SynchronizeThreads(_info) \
            if((_info)->count>1){if((_info)->sync->Sync((_info)->id)!=SVL_SYNC_OK){return SVL_FAIL;}}
#define _CriticalSection(_info) \
            if((_info)->count>1){(_info)->cs->Enter();}for(bool _incs=true;_incs&&((_info)->count>1);_incs=false,(_info)->cs->Leave())


class svlFilterBase;
class svlFilterSourceBase;
class svlStreamManager;
class svlStreamControlMultiThread;


class CISST_EXPORT svlStreamEntity
{
friend class svlStreamManager;
friend class svlStreamControlMultiThread;

public:
    int Append(svlFilterBase* filter);

private:
    svlStreamEntity();
    svlStreamEntity(svlStreamEntity const &) {};
    svlStreamManager * Stream;
};

class CISST_EXPORT svlFilterBase
{
friend class svlFilterSourceBase;
friend class svlStreamManager;
friend class svlStreamControlMultiThread;

    typedef std::map<svlStreamType, svlStreamType> _StreamTypeMap;
    typedef std::vector<svlStreamEntity*> _OutputBranchList;

public:
    typedef struct _ProcInfo {
        unsigned int  count;
        unsigned int  id;
        svlSyncPoint* sync;
        osaCriticalSection* cs;
    } ProcInfo;

public:
    svlFilterBase();
    virtual ~svlFilterBase();

    svlStreamType GetInputType();
    svlStreamType GetOutputType();
    bool IsInitialized();
    bool IsRunning();

protected:
    svlSample* OutputData;
    unsigned int FrameCounter;

    virtual int Initialize(svlSample* inputdata);
    virtual int OnStart(unsigned int procCount);
    virtual int ProcessFrame(ProcInfo* procInfo, svlSample* inputdata) = 0;
    virtual void OnStop();
    virtual int Release();

    int AddSupportedType(svlStreamType input, svlStreamType output);
    void UpdateOutputFormat();
    int IsDataValid(svlStreamType type, svlSample* data);
    bool IsNewSample(svlSample* sample);

private:
    bool Initialized;
    bool Running;
    bool OutputSampleModified;
    bool OutputFormatModified;
    double PrevInputTimestamp;
    _StreamTypeMap SupportedTypes;
    _OutputBranchList OutputBranches;
    svlStreamType InputType;
    svlStreamType OutputType;
    svlFilterBase* PrevFilter;
    svlFilterBase* NextFilter;

    int OnConnectInput(svlStreamType inputtype);

    svlFilterBase* GetUpstreamFilter();
    svlFilterBase* GetDownstreamFilter();
};


class CISST_EXPORT svlFilterSourceBase : public svlFilterBase
{
friend class svlFilterBase;
friend class svlStreamManager;
friend class svlStreamControlMultiThread;

public:
    svlFilterSourceBase(bool autotimestamps = true);
    virtual ~svlFilterSourceBase();

    virtual int GetWidth(unsigned int videoch = SVL_LEFT);
    virtual int GetHeight(unsigned int videoch = SVL_LEFT);
    virtual double GetTargetFrequency();
    virtual int SetTargetFrequency(double hertz);
    virtual void SetLoop(bool loop = true);
    virtual bool GetLoop();

protected:
    int AddSupportedType(svlStreamType output);

    virtual int Initialize();
    virtual int OnStart(unsigned int procCount);
    virtual int ProcessFrame(ProcInfo* procInfo) = 0;
    virtual void OnStop();
    virtual int Release();

    int RestartTargetTimer();
    int StopTargetTimer();
    int WaitForTargetTimer();
    bool IsTargetTimerRunning();

    double TargetFrequency;
    bool LoopFlag;

private:
    // Dispatched to source specific methods declared above
    int Initialize(svlSample* inputdata);
    int ProcessFrame(ProcInfo* procInfo, svlSample* inputdata);

    // Hidden from derived classes
    int AddSupportedType(svlStreamType input, svlStreamType output);

    bool AutoTimestamp;
    osaStopwatch TargetTimer;
    double TargetStartTime;
    double TargetFrameTime;
};


#include <cisstStereoVision/svlStreamBranchSource.h>


class CISST_EXPORT svlStreamManager
{
friend class svlStreamEntity;
friend class svlStreamControlMultiThread;

    typedef struct tagBranchStruct {
        svlStreamEntity* entity;
        std::string name;
    } _BranchStruct;
    typedef std::map<_BranchStruct*, svlStreamManager*> _BranchMap;
    typedef std::vector<svlFilterBase*> _FilterList;

public:
    svlStreamManager();
    svlStreamManager(unsigned int threadcount);
    ~svlStreamManager();

    svlStreamEntity& Trunk();
    svlStreamEntity& Branch(const std::string & name);
    svlStreamEntity* CreateBranchAfterFilter(svlFilterBase* filter);
    svlStreamEntity* CreateBranchAfterFilter(svlFilterBase* filter, const std::string & name);
    int RemoveBranch(svlStreamEntity* entity);
    int RemoveBranch(const std::string & name);
    int RemoveFilter(svlFilterBase* filter);
    int EmptyFilterList();

    int Initialize();
    void Release();
    bool IsInitialized();
    int Start();
    void Stop();
    bool IsRunning();
    int WaitForStop(double timeout = -1.0);
    int GetStreamStatus();

private:
    ///////////////////////////////////////////////
    // Methods that may be public in the future

    int SetThreads(unsigned int threadcount);

    int AddFilter(svlFilterBase* filter);
    int SetSourceFilter(svlFilterSourceBase* source);

    int ConnectFilterToBranch(svlStreamEntity* entity, svlFilterBase* filter);
    int ConnectFilters(svlFilterBase* filter1, svlFilterBase* filter2);

    // Convenience method combining AddFilter and SetSourceFilter
    int AddSourceFilter(svlFilterSourceBase* source);
    // Convenience method combining AddFilter and ConnectFilters
    int AddFilterAndConnect(svlFilterBase* filter, svlFilterBase* connect_to);

    int DisconnectFilterInput(svlFilterBase* filter);
    int DisconnectFilterOutput(svlFilterBase* filter);
    int GetUpstreamFilter(svlFilterBase* filter, svlFilterBase** upstreamfilter);
    int GetDownstreamFilter(svlFilterBase* filter, svlFilterBase** downstreamfilter);

private:
    unsigned int ThreadCount;
    vctDynamicVector<svlStreamControlMultiThread*> ControlInstanceMulti;
    vctDynamicVector<osaThread*> ControlThreadMulti;
    svlSyncPoint* SyncPoint;
    osaCriticalSection* CS;

    svlStreamEntity Entity;
    svlStreamEntity InvalidEntity;
    _BranchMap Branches;
    _FilterList Filters;
    svlFilterSourceBase* StreamSource;
    bool Initialized;
    bool Running;
    bool StopThread;
    int StreamStatus;

    int AppendFilterToTrunk(svlFilterBase* filter);
    svlStreamEntity* GetBranchEntityOfFilter(svlFilterBase* filter);
    bool IsFilterInList(svlFilterBase* filter);
    int ReInitializeDownstreamFilters(svlFilterBase* fromfilter);
    void InternalStop(unsigned int callingthreadID);
};


class CISST_EXPORT svlStreamControlMultiThread
{
public:
    svlStreamControlMultiThread(unsigned int threadcount, unsigned int threadid);
    ~svlStreamControlMultiThread();

    void* Proc(svlStreamManager* baseref);

private:
    svlStreamControlMultiThread();

    double GetAbsoluteTime(osaTimeServer* timeserver);

    unsigned int ThreadID;
    unsigned int ThreadCount;
};

#endif // _svlStreamManager_h

