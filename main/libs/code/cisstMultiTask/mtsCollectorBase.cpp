/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsCollectorBase.cpp 188 2009-03-20 17:07:32Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2009-03-20

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstMultiTask/mtsCollectorBase.h>

CMN_IMPLEMENT_SERVICES(mtsCollectorBase)

unsigned int mtsCollectorBase::CollectorCount;
mtsTaskManager * mtsCollectorBase::TaskManager;

//-------------------------------------------------------
//	Constructor, Destructor, and Initializer
//-------------------------------------------------------
mtsCollectorBase::mtsCollectorBase(const std::string & collectorName, 
                                   const CollectorLogFormat logFormat): 
    mtsTaskContinuous(collectorName),
    LogFormat(logFormat),
    TimeOffsetToZero(false),    
    IsRunnable(false)
{
    ++CollectorCount;

    if (TaskManager == 0) {
        TaskManager = mtsTaskManager::GetInstance();
    }

    Init();
}


mtsCollectorBase::~mtsCollectorBase()
{
    --CollectorCount;
    CMN_LOG_CLASS_INIT_VERBOSE << "Collector " << GetName() << " ends." << std::endl;
}


void mtsCollectorBase::Init()
{
    TimeOffsetToZero = false;
    Status = COLLECTOR_STOP;
    DelayedStart = 0.0;
    DelayedStop = 0.0;
    ClearTaskMap();
}


//-------------------------------------------------------
//	Thread management functions (called internally)
//-------------------------------------------------------
void mtsCollectorBase::Run()
{
    if (Status == COLLECTOR_STOP) return;

    // Check for the state transition
    switch (Status) {
    case COLLECTOR_WAIT_START:
        if (Stopwatch.IsRunning()) {
            if (Stopwatch.GetElapsedTime() < DelayedStart) {
                return;
            } else {
                // Start collecting
                DelayedStart = 0.0;
                Status = COLLECTOR_COLLECTING;
                IsRunnable = true;
                Stopwatch.Stop();
                
                // Call Collect() method to activate the data collection feature 
                // of all registered tasks. Normally, Collect() is called by
                // an event generated from another task of which data is being
                // collected.
                Collect();
            }
        } else {
            return;
        }
        break;
        
    case COLLECTOR_WAIT_STOP:
        if (Stopwatch.IsRunning()) {
            if (Stopwatch.GetElapsedTime() < DelayedStop) {
                return;
            } else {
                // Stop collecting
                DelayedStop = 0.0;
                Status = COLLECTOR_STOP;
                IsRunnable = false;
                Stopwatch.Stop();
            }
        } else {
            return;
        }
        break;
    }
    
    CMN_ASSERT(Status == COLLECTOR_COLLECTING ||
               Status == COLLECTOR_STOP);
    
    if (Status == COLLECTOR_STOP) {
        DelayedStop = 0.0;
        CMN_LOG_CLASS_RUN_VERBOSE << "The collector stopped." << std::endl;
        return;
    }
}


void mtsCollectorBase::Cleanup(void)
{
    ClearTaskMap();
}


void mtsCollectorBase::StartCollection(const double delayedStartInSecond)
{    
    // Check for state transition
    switch (Status) {
    case COLLECTOR_WAIT_START:
        CMN_LOG_CLASS_INIT_VERBOSE << "Waiting for the collector to start." << std::endl;
        return;
        
    case COLLECTOR_WAIT_STOP:
        CMN_LOG_CLASS_INIT_VERBOSE << "Waiting for the collector to stop." << std::endl;
        return;
        
    case COLLECTOR_COLLECTING:
        CMN_LOG_CLASS_INIT_VERBOSE << "The collector is now running." << std::endl;
        return;
    }
    
    DelayedStart = delayedStartInSecond;
    Status = COLLECTOR_WAIT_START;

    Stopwatch.Reset();
    Stopwatch.Start();
}


void mtsCollectorBase::StopCollection(const double delayedStopInSecond)
{
    // Check for state transition
    switch (Status) {
    case COLLECTOR_WAIT_START:
        CMN_LOG_CLASS_INIT_VERBOSE << "Waiting for the collector to start." << std::endl;
        return;
        
    case COLLECTOR_WAIT_STOP:
        CMN_LOG_CLASS_INIT_VERBOSE << "Waiting for the collector to stop." << std::endl;
        return;
        
    case COLLECTOR_STOP:
        CMN_LOG_CLASS_INIT_VERBOSE << "The collector is not running." << std::endl;
        return;
    }
    
    DelayedStop = delayedStopInSecond;
    Status = COLLECTOR_WAIT_STOP;
    
    Stopwatch.Reset();
    Stopwatch.Start();
}


//-------------------------------------------------------
//	Miscellaneous Functions
//-------------------------------------------------------
void mtsCollectorBase::ClearTaskMap(void)
{
    if (!TaskMap.empty()) {        
        TaskMapType::iterator itr = TaskMap.begin();
        SignalMapType::iterator _itr;
        for (; itr != TaskMap.end(); ++itr) {
            itr->second->clear();
            delete itr->second;
        }
        TaskMap.clear();
    }
}
