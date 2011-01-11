/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id$ */

#ifndef _sineTask_h
#define _sineTask_h

// include for the whole cisstMultiTask library
#include <cisstMultiTask.h>
#include <sineData.h>

class sineTask: public mtsTaskPeriodic {
    // used to control the log level, "Run Error" by default
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

    MTS_DECLARE_COMPONENT(sineTask, mtsTaskPeriodic);

 protected:

    MTS_STATE_TABLE_BEGIN;
    // data generated by the sine wave generator
    MTS_STATE_TABLE_DATA(sineData, SineData);
    MTS_STATE_TABLE_END;
    // this is the amplitude of the sine wave
    double SineAmplitude;

    inline void SetAmplitude(const mtsDouble & amp) {
        SineAmplitude = amp.Data;
    }

    MTS_INTERFACE_PROVIDED_BEGIN("MainInterface");
    MTS_COMMAND_WRITE(SetAmplitude, "SetAmplitude", mtsDouble(1.0));
    MTS_COMMAND_STATEREAD(SineData, "GetData");
    MTS_INTERFACE_PROVIDED_END("MainInterface");

 public:
    // provide a name for the task and define the frequency (time
    // interval between calls to the periodic Run).  Also used to
    // populate the interface(s)
    sineTask(const std::string & taskName, double period);
    ~sineTask() {};
    // all four methods are pure virtual in mtsTask
    void Configure(const std::string & CMN_UNUSED(filename)) {};
    void Startup(void);    // set some initial values
    void Run(void);        // performed periodically
    void Cleanup(void) {}; // user defined cleanup
};

CMN_DECLARE_SERVICES_INSTANTIATION(sineTask);

#endif // _sineTask_h

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
