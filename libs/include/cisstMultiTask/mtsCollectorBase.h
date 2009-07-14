/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsCollectorBase.h 2009-03-02 mjung5

  Author(s):  Min Yang Jung
  Created on: 2009-02-25

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

/*!
  \file
  \brief A data collection tool
*/

#ifndef _mtsCollectorBase_h
#define _mtsCollectorBase_h

#include <cisstCommon/cmnUnits.h>
#include <cisstCommon/cmnNamedMap.h>
#include <cisstOSAbstraction/osaStopwatch.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsHistory.h>

#include <string>
#include <stdexcept>

// If the following line is commented out, C2491 error is generated.
#include <cisstMultiTask/mtsExport.h>

/*!
\ingroup cisstMultiTask

This class provides a way to collect data from state table in real-time.
Collected data can be either saved as a log file or displayed in GUI like an oscilloscope.
*/
class CISST_EXPORT mtsCollectorBase : public mtsTaskContinuous
{
    friend class mtsCollectorBaseTest;

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);

    //-------------------- Auxiliary class definition -----------------------//
public:
    typedef enum {
        COLLECTOR_LOG_FORMAT_PLAIN_TEXT,
        COLLECTOR_LOG_FORMAT_BINARY,
        COLLECTOR_LOG_FORMAT_CSV
    } CollectorLogFormat;

    //------------------------- Protected Members ---------------------------//
protected:

    /*! State definition of the collector */
    typedef enum { 
        COLLECTOR_STOP,         // nothing happens.
        COLLECTOR_WAIT_START,   // Start(void) has been called. Wait for some time elapsed.
        COLLECTOR_COLLECTING,   // Currently collecting data
        COLLECTOR_WAIT_STOP     // Stop(void) has been called. Wait for some time elapsed.
    } CollectorStatus;

    CollectorStatus Status;
    CollectorLogFormat LogFormat;

    class SignalMapElement {
    public:
        mtsTask * Task;
        mtsHistoryBase * History;
    
        SignalMapElement(void) {}
        ~SignalMapElement(void) {}
    };

    /*! Container definition for tasks and signals.
        TaskMap:   (taskName, SignalMap*)
        SignalMap: (SignalName, SignalMapElement)
    */
    typedef cmnNamedMap<SignalMapElement> SignalMapType;
    typedef cmnNamedMap<SignalMapType> TaskMapType;
    TaskMapType TaskMap;

    /*! If this flag is set, start time is subtracted from each time measurement. */
    bool TimeOffsetToZero;    

    /*! A child collector class can run only if this flag is set. */
    bool IsRunnable;

    /*! For delayed start(void) end stop(void) */
    double DelayedStart;
    double DelayedStop;
    osaStopwatch Stopwatch;

    /*! Static member variables */
    static unsigned int CollectorCount;
    static mtsTaskManager * TaskManager;

    /*! Initialize this collector instance */
    void Init(void);

    /*! Clean up internal data. */
    void Cleanup(void);

    /*! Clear TaskMap */
    void ClearTaskMap(void);

    /*! Set some initial values */
    virtual void Startup(void) = 0;

    /*! Collect data */
    virtual void Collect(void) = 0;

    /*! Performed periodically */
    virtual void Run(void);

public:
    mtsCollectorBase(const std::string & collectorName, const CollectorLogFormat logFormat);

    virtual ~mtsCollectorBase(void);

    /*! Begin collecting data. Data collection will begin after delayedStart 
    second(s). If it is zero (by default), it means 'start now'. */
    void Start(const double delayedStartInSeconds = 0.0);

    /*! End collecting data. Data collection will end after delayedStop
    second(s). If it is zero (by default), it means 'stop now'. */
    void Stop(const double delayedStopInSeconds = 0.0);

    //---------------------- Miscellaneous functions ------------------------//
    inline static unsigned int GetCollectorCount(void) { return CollectorCount; }

};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsCollectorBase)

#endif // _mtsCollectorBase_h

