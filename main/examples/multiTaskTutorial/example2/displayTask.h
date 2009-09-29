/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
// $Id: displayTask.h 456 2009-06-13 03:11:44Z adeguet1 $

#ifndef _displayTask_h
#define _displayTask_h

#include <cisstMultiTask.h>
#include "displayUI.h"

class displayTask: public mtsTaskPeriodic {

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
    double StartValue;
    
 protected:
    mtsDouble Data;
    mtsDouble Amplitude;
    mtsDouble TriggerValue;
    volatile bool WaitingForTrigger;

    struct GeneratorStruct {
       // functions which will be bound to commands
       mtsFunctionRead GetData;
       mtsFunctionWrite SetAmplitude;
       mtsFunctionWrite SetTriggerValue;
       mtsFunctionVoid ResetTrigger;
    } Generator;

    struct ClockStruct {
       mtsFunctionRead GetClockData;
    } Clock;

    // event handler
    void HandleTrigger(const mtsDouble & value);

    displayUI UI;
    
 public:
    displayTask(const std::string taskName, double period);
    ~displayTask();
    void Configure(const std::string & CMN_UNUSED(filename) = "");
    void Startup(void);
    void Run(void);
    void Cleanup(void) {};
};

CMN_DECLARE_SERVICES_INSTANTIATION(displayTask);

#endif // _displayTask_h

/*
  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/
