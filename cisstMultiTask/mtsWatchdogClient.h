/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
Author(s):  Marcin Balicki
Created on: 2014

(C) Copyright 2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsWatchdogClient_h
#define _mtsWatchdogClient_h

#include <cisstCommon/cmnGenericObject.h>
#include <cisstOSAbstraction/osaStopwatch.h>
#include <cisstMultiTask/mtsFunctionRead.h>
#include <cisstMultiTask/mtsFunctionWrite.h>
#include <cisstMultiTask/mtsFunctionVoid.h>

class mtsWatchdogClient: public cmnGenericObject {
    // used to control the log level, "Run Error" by default
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
protected:
    osaStopwatch StopWatch;
    double Timeout;
    struct {
        mtsFunctionRead  ReadState;
        mtsFunctionWrite WriteState;
        mtsFunctionVoid  ResetState;
    } Watchdog;

    bool IsOK;
public:
    inline mtsWatchdogClient():
        Timeout(0),
        IsOK(false)
    {};

    ~mtsWatchdogClient() {};

    void AddToRequiredInterface(mtsInterfaceRequired & reqInt);

    void SetTimeoutPeriod(const double & seconds) {
        Timeout = seconds;
    }

    bool Start(void);

    // true if everything is ok;
    bool CheckAndUpdate(void);
    bool Reset(void);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsWatchdogClient);

#endif // _mtsWatchdogClient_h
