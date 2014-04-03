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


#ifndef _svlStreamManager_h
#define _svlStreamManager_h

#include <cisstVector/vctDynamicVector.h>
#include <cisstMultiTask/mtsComponent.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


// Forward declarations
class svlSyncPoint;
class svlFilterBase;
class svlFilterSourceBase;
class svlStreamProc;
class osaThread;
class osaCriticalSection;


class CISST_EXPORT svlStreamManager: public mtsComponent
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

friend class svlStreamProc;

public:
    svlStreamManager();
    svlStreamManager(unsigned int threadcount);
    ~svlStreamManager();

    int SetSourceFilter(svlFilterSourceBase * source);
    int Initialize(void);
    void Release(void);
    bool IsInitialized(void) const;
    int Play(void);
    void Stop(void);
    bool IsRunning(void) const;
    int WaitForStop(double timeout = -1.0);
    int GetStreamStatus(void) const;
    void DisconnectAll(void);

    // Virtual methods from mtsComponent (these are temporary measures until 
    // ticket #67 is resolved)
    void Start(void) { Play(); }
    void Suspend(void) { Stop(); }

private:
    unsigned int ThreadCount;
    vctDynamicVector<svlStreamProc*> StreamProcInstance;
    vctDynamicVector<osaThread*> StreamProcThread;
    svlSyncPoint* SyncPoint;
    osaCriticalSection* CS;

    svlFilterSourceBase* StreamSource;
    bool Initialized;
    bool Running;
    bool StopThread;
    int StreamStatus;

    void InternalStop(unsigned int callingthreadID);

protected:
    virtual void CreateInterfaces(void);
    virtual void PlayCommand(void);
    virtual void InitializeCommand(void);
    virtual void SetSourceFilterCommand(const mtsStdString & source);
};

CMN_DECLARE_SERVICES_INSTANTIATION(svlStreamManager);

#endif // _svlStreamManager_h

